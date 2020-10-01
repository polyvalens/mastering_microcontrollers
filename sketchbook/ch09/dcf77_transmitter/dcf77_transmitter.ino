/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * dcf77 transmitter
 */

//#define _CHECK_OUTPUT_

int output = 9;
int output_inverted = 10;
int led = 13;
int frequency = 206;
int am = 10;
String bits;
uint8_t utc_offset = 2;
uint8_t seconds = 0;
uint8_t minutes = 40;
uint8_t hours = 17;
uint8_t day_of_week = 4;
uint8_t day_of_month = 6;
uint8_t month = 9;
uint8_t year = 12;

uint8_t days_in_month[12] =
{
  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

uint8_t to_bcd(uint8_t value)
{
  // Maximum value is 99.
  if (value>99) value = 99;
  uint8_t tens = value/10;
  uint8_t units = value - tens*10;
  return (tens<<4) | units;
}

uint8_t sentence_add_value(uint8_t value, uint8_t nr_of_bits, String &str)
{
  uint8_t i;
  uint8_t parity = 0;
  uint8_t bcd_value = to_bcd(value);
  for (i=0; i<nr_of_bits; i++)
  {
    if ((bcd_value&1)!=0)
    {
      str += '1';
      parity ^= 1;
    }
    else str += '0';
    bcd_value >>= 1;
  }
  return parity;
}

void sentence_build(String &str)
{
  char parity;
  str = "00000000000000000"; // set bits 0 to 16 to 0
  if (utc_offset==2) str += "10";
  else str += "01";
  str += "01"; // add fixed bits 19 & 20
  parity = sentence_add_value(minutes,7,str);
  str += char('0'+parity);
  parity = sentence_add_value(hours,6,str);
  str += char('0'+parity);
  parity = sentence_add_value(day_of_month,6,str);
  parity ^= sentence_add_value(day_of_week,3,str);
  parity ^= sentence_add_value(month,5,str);
  parity ^= sentence_add_value(year,8,str);
  str += char('0'+parity);
}

boolean parity(String str, char parity_bit)
{
  unsigned int i;
  int parity = 0;
  for (i=0; i<str.length(); i++)
  {
    if (str[i]=='1') parity ^= 1;
  }
  return (parity+0x30 == parity_bit);
}

boolean verify(String str)
{
  if (str.length()!=59) return false;
  if (str[20]!='1') return false;
  if (parity(str.substring(21,28),str[28])==false) return false;
  if (parity(str.substring(29,35),str[35])==false) return false;
  if (parity(str.substring(36,58),str[58])==false) return false;
  return true;
}

int decode_bcd(String str)
{
  unsigned int i;
  int power_of_two;
  int power_of_ten = 1;
  int result = 0;
  for (i=0; i<str.length(); i++)
  {
    power_of_two = 1<<(i&0x3);
    result += (str[i]-'0')*power_of_two*power_of_ten;
    if (power_of_two==8) power_of_ten *= 10;
  }
  return result;
}

boolean decode_time(String &str)
{
  if (verify(str)==false) return false;
  minutes = decode_bcd(str.substring(21,28));
  hours = decode_bcd(str.substring(29,35));
  day_of_month = decode_bcd(str.substring(36,42));
  day_of_week = decode_bcd(str.substring(42,45));
  month = decode_bcd(str.substring(45,50));
  year = decode_bcd(str.substring(50,58));
  return true;
}

void time_tick(void)
{
  seconds += 1;
  if (seconds>59)
  {
    seconds = 0;
    minutes += 1;
    if (minutes>59)
    {
      minutes = 0;
      hours += 1;
      if (hours>23)
      {
        hours = 0;
        day_of_week += 1;
        if (day_of_week>7) day_of_week = 1;
        day_of_month += 1;
        if (day_of_month>days_in_month[month])
        {
          // Does not handle leap years.
          day_of_month = 1;
          month += 1;
          if (month>12)
          {
            month = 1;
            year += 1;
          }
        }
      }
    }
  }
}

void setup(void)
{
#ifdef _CHECK_OUTPUT_
  Serial.begin(115200);
#endif /* _CHECK_OUTPUT_ */

  pinMode(output,OUTPUT);
  pinMode(output_inverted,OUTPUT);
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);

  // Timer1, PWM mode 14, prescale factor of 1
  // OC1A normal, OC1B inverted.
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1B0) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
  ICR1 = frequency;
  OCR1A = ICR1 >> 1; // 50%
  OCR1B = OCR1A; // 50%

  seconds = 0;
}

void loop(void)
{
  sentence_build(bits);

#ifdef _CHECK_OUTPUT_
  Serial.print(bits);
  if (decode_time(bits)==false) Serial.print(" *");
  Serial.print(' ');
  Serial.print(hours);
  Serial.print(':');
  Serial.print(minutes);
  Serial.print(' ');
  Serial.print(day_of_month);
  Serial.print('/');
  Serial.print(month);
  Serial.print("/20");
  Serial.println(year);
#endif /* _CHECK_OUTPUT_ */

  uint8_t bit_length = 0;
  if (seconds<59)
  {
    bit_length = 100 << (bits[seconds]-'0');
    OCR1A = am; // 25% of full amplitude
    OCR1B = OCR1A;
    digitalWrite(led,HIGH);
    delay(bit_length);
    digitalWrite(led,LOW);
    OCR1A = ICR1 >> 1; // 100%, 50% duty cycle
    OCR1B = OCR1A;
  }
  delay(1000-bit_length);
  time_tick();
}

