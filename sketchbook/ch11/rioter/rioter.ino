/*
 * Rioter
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

#define CARRIER  37000
#define THRESHOLD  200
#define TIMEOUT_DEFAULT  2000000
#define PULSECOUNT_DEFAULT  50

// Uncomment to output recorded key code on serial port.
//#define OUTPUT_KEY_CODE

// TSOP34836 connected to pin 8.
int ir_signal = 8;
int mic = A0;
int led = 11;
int ir_led = 13;
extern volatile long timer2_toggle_count;
const float Fs = 1760; // Hz, sample rate
const float Fc = 10; // Hz
float ki[2];
float yi[2];
int sound_max = 0;
int sound_min = 0;
unsigned long t_start;

typedef struct
{
  unsigned long timestamp;
  unsigned long on_time;
}
point_t;

point_t pulses[PULSECOUNT_DEFAULT];
int index = 0;

void ir_pulse(uint8_t pin, uint32_t frequency, uint32_t duration)
{
  // Overflow bug in Arduino, we have to calculate our own toggle_count.
  // A fudge factor is needed to correct for storing the value
  // in timer2_toggle_count. Carrier frequency dependent.
  unsigned long toggle_count = 2*frequency*duration/1000000 - 8; // 6 for 36 kHz;
  //TCNT2 = 0; // Put this in tone.cpp right before writing OCR2A (line 386 in Arduino 1.0.5)
  tone(pin,frequency,duration);
  timer2_toggle_count = toggle_count;
}

void ir_send_pulse(uint8_t pin, uint16_t frequency, uint16_t on_time, uint16_t off_time)
{
  ir_pulse(pin,frequency,on_time);
  // Wait for the burst to end.
  // OCR2A is cleared in disableTimer at the end of the burst.
  while (OCR2A!=0);
  // About 104 us are needed to compensate for the delay between
  // the call of ir_pulse and the start of the burst.
  delayMicroseconds(off_time-104);
}

void ir_send(uint8_t pin, uint16_t frequency)
{
  unsigned long off_time;
  int i;
  // Disable timer0 interrupts during transmission.
  bitClear(TIMSK0,TOIE0);
  for (i=0; i<index-1; i++)
  {
    off_time = pulses[i+1].timestamp - pulses[i].timestamp - pulses[i+1].on_time;
    ir_send_pulse(pin,frequency,pulses[i].on_time,off_time);
  }
  //ir_send_pulse(pin,frequency,pulses[i].on_time,off_time);
  // Re-enable timer0 interrupts.
  bitSet(TIMSK0,TOIE0);
}

int ir_receive(void)
{
  index = 0;
  unsigned long time_start = 0;
  unsigned long pulse = pulseIn(ir_signal,LOW,TIMEOUT_DEFAULT);
  while (pulse!=0 && index<=PULSECOUNT_DEFAULT)
  {
    unsigned long time_now = micros();
    if (time_start==0) time_start = time_now;
    pulses[index].timestamp = time_now - time_start;
    pulses[index].on_time = pulse;
    index += 1;
    pulse = pulseIn(ir_signal,LOW,TIMEOUT_DEFAULT);
  }
  return index;
}

void lpf_init(float fs, float fc, float coeffs[], float y[])
{
  coeffs[0] = exp(-TWO_PI*fc/fs); // k
  coeffs[1] = 1.0 - coeffs[0]; // 1-k
  y[0] = 0.0;
  y[1] = 0.0;
}

float lpf(int sample, float coeffs[], float y[])
{
  // y[n] = (1-k)*x[n] + k*y[n-1]
  // k = e^(-2*pi*fc/fs)
  // fs = sample rate
  // fc = cut-off frequency
  y[0] = coeffs[1]*sample + coeffs[0]*y[1];
  y[1] = y[0]; // delay sample
  return y[0];
}

boolean sound_detected(int threshold)
{
  int sound = analogRead(mic);
  sound += analogRead(mic);
  sound += analogRead(mic);
  sound += analogRead(mic);
  sound >>= 2;

  if (sound>sound_max) sound_max = sound;
  if (sound<sound_min) sound_min = sound;
  sound_max--;
  sound_min++;
  lpf(sound_max-sound_min,ki,yi);
  if (yi[0]>threshold) return true;
  return false;
}

void setup(void)
{
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
  lpf_init(Fs,Fc,ki,yi);

  pinMode(ir_signal,INPUT);

  // Ignore spurious pulses at startup.
  delay(500);
  while (pulseIn(ir_signal,LOW,500000)!=0);

  // Signal that receive window is open.
  digitalWrite(led,HIGH);
  // Record IR input.
  ir_receive();
  digitalWrite(led,LOW);
  
#ifdef OUTPUT_KEY_CODE
  Serial.begin(115200);
  Serial.println();
  for (int i=0; i<index; i++)
  {
    Serial.print(pulses[i].timestamp);
    Serial.print(",");
    Serial.print(pulses[i].on_time);
    Serial.println();
  }
#endif // OUTPUT_KEY_CODE
  
  t_start = millis();
}

void loop(void)
{
  unsigned long t = millis();
  if (sound_detected(THRESHOLD)==true && t-t_start>5000)
  {
    digitalWrite(led,HIGH);
    ir_send(ir_led,CARRIER);
    digitalWrite(led,LOW);
    t_start = millis();
  }
}
