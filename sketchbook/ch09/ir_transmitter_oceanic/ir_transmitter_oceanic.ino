/*
 * IR transmitter
 *
 * Belongs to:
 * "Mastering Microcontrollers Helped by Arduino"
 * ISBN 978-1-907920-23-3 (English)
 * ISBN 978-2-86661-190-3 (French)
 * http://www.polyvalens.com/
 *
 * For use with PolyValens Multipurpose Shield 129009-1
 *
 * Copyright (c) 2014, Clemens Valens
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 * BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#define CARRIER  38000

// Code consists of fixed 3-byte manufacturer (?) code 0x2fd followed
// by key value and its complement. The key value is bit-reversed, ie
// LSB first.
#define RC_CODE  0x2fd00ff

int ir_led = 2;
uint16_t startbit = 9000;
uint16_t on_time_0 = 560;
uint16_t off_time_0 = 565;
uint16_t on_time_1 = 560;
uint16_t off_time_1 = 1690;
extern volatile long timer2_toggle_count;

void ir_send_pulse(uint8_t pin, uint32_t frequency, uint32_t duration)
{
  // Overflow bug in Arduino, we have to calculate our own toggle_count.
  // A fudge factor is needed to correct for storing the value
  // in timer2_toggle_count. Carrier frequency dependent.
  unsigned long toggle_count = 2*frequency*duration/1000000 - 8; // 6 for 36 kHz;
  //TCNT2 = 0; // Put this in tone.cpp right before writing OCR2A (line 388 in arduino 1.8.12)
  tone(pin,frequency,duration);
  timer2_toggle_count = toggle_count;
}

void ir_send_bit(uint8_t pin, uint16_t frequency, uint16_t on_time, uint16_t off_time)
{
  ir_send_pulse(pin,frequency,on_time);
  // Wait for the burst to end.
  // OCR2A is cleared in disableTimer at the end of the burst.
  while (OCR2A!=0); 
  // About 104 us are needed to compensate for the delay between
  // the call of ir_send_pulse and the start of the burst.
  delayMicroseconds(off_time-104);
}

void ir_send_code(uint8_t pin, uint16_t frequency, uint32_t code)
{
  ir_send_bit(pin,frequency,startbit,startbit/2);
  for (uint32_t mask=0x80000000; mask!=0; mask>>=1)
  {
    if ((code&mask)==0)
    {
      ir_send_bit(pin,frequency,on_time_0,off_time_0);
    }
    else
    {
      ir_send_bit(pin,frequency,on_time_1,off_time_1);
    }
  }
  ir_send_bit(pin,frequency,on_time_1,off_time_1);
}

const uint8_t bit_reverse[16] = 
{
  0b0000, 0b1000, 0b0100, 0b1100, 0b0010, 0b1010, 0b0110, 0b1110,
  0b0001, 0b1001, 0b0101, 0b1101, 0b0011, 0b1011, 0b0111, 0b1111,
};

uint32_t build_code(uint8_t d)
{
  uint32_t code = 0;
  code |= bit_reverse[d&0xf];
  code <<= 4;
  code |= bit_reverse[(d>>4)&0xf];
  code <<= 4;
  code |= 15 - bit_reverse[d&0xf];
  code <<= 4;
  code |= 15 - bit_reverse[(d>>4)&0xf];
  code &= 0xffff;
  code |= 0x2fd0000;
  return code;
}

char to_hex[16] =
{
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

void print_hex16(uint16_t c)
{
  Serial.print(to_hex[(c>>12)&0xf]);
  Serial.print(to_hex[(c>>8)&0xf]);
  Serial.print(", ");
  Serial.print(to_hex[(c>>4)&0xf]);
  Serial.print(to_hex[(c>>0)&0xf]);
}

void tx(uint32_t code)
{
  uint32_t c = code & 0xffff;
  // Stop timer0 during transmission.
  bitClear(TIMSK0,TOIE0);
  ir_send_code(ir_led,CARRIER,code);
  bitSet(TIMSK0,TOIE0);
}

void print_right_alligned(uint8_t v)
{
  if (v<10) Serial.print(' ');
  if (v<100) Serial.print(' ');
  Serial.print(v);
  Serial.print(", ");
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println("IR Transmitter");
  Serial.println("nr,code,complement,comment");
}

void loop(void)
{
  static uint8_t value = 0xff;
  static uint32_t rc_code;
  
  if (Serial.available())
  {
    char ch = Serial.read();
    if (ch==' ') // Increment code, send it and receive comment.
    {
      value += 1;
      
      rc_code = build_code(value);
      tx(rc_code);

      print_right_alligned(value);
      print_hex16(rc_code);

      // Collect user comment.
      Serial.print(", ");
      Serial.print('"');
      while (1)
      {
        // Receive & display description/comment data until CR/LF is received.
        if (Serial.available())
        {
          ch = Serial.read();
          if (ch==0x0d || ch==0x0a)
          {
            // End of comment, leave loop.
            Serial.print('"');
            break;
          }
          else Serial.print(ch);
        }
      }
      Serial.println();

      if (value==0xff)
      {
        Serial.println("Last value."); // Roll over on next press.
      }
    }
    else if (ch=='r') // Retransmit current code.
    {
      Serial.print("Resending ");
      print_right_alligned(value);
      print_hex16(rc_code);
      Serial.println();
      tx(rc_code);
    }
    else if (ch=='0') // Reset code.
    {
      Serial.println("Restarting at 0");
      value = 0xff;
    }
    else if (ch=='-') // Decrement code.
    {
      value -= 1;
      rc_code = build_code(value);
      print_right_alligned(value);
      print_hex16(rc_code);
      Serial.println();
      //tx(rc_code);
    }
    else if (ch=='+') // Increment code.
    {
      value += 1;
      rc_code = build_code(value);
      print_right_alligned(value);
      print_hex16(rc_code);
      Serial.println();
      //tx(rc_code);
    }
  }
}
