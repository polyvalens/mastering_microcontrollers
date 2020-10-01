/*
 * DCF77 clock
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

#include <LiquidCrystal.h>
#define LCD_LINES  2
#define LCD_CHARACTERS_PER_LINE  16
LiquidCrystal lcd(6,7,5,4,3,2);

const int dcf77 = 0;
const int led = 13;
int pulse_prev;
unsigned long time_falling;
unsigned long time_rising;
String bits;
int seconds;
int minutes;
int hours;
int day_of_month;
int day_of_week;
int month;
int year;

boolean parity(String str, char parity_bit)
{
  unsigned int i;
  int sum = 0;
  for (i=0; i<str.length(); i++)
  {
    if (str[i]=='1') sum += 1;
  }
  return ((sum&1)+0x30 == parity_bit);
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

void setup(void) 
{
  lcd.begin(LCD_CHARACTERS_PER_LINE,LCD_LINES);
  lcd.clear();
  lcd.print("DCF77 clock");
  pinMode(dcf77,INPUT_PULLUP);
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
  pulse_prev = digitalRead(dcf77);
  time_falling = millis();
  time_rising = millis();
  seconds = 0;
}

void loop(void) 
{
  unsigned long time = millis();
  unsigned long dt = 0;
  int pulse = digitalRead(dcf77);

  if (pulse==1 && pulse_prev==0)
  { 
    // rising edge
    digitalWrite(led,HIGH);
    time_rising = time;
    dt = time_rising-time_falling;
    seconds += 1;
  }
  else if (pulse==0 && pulse_prev==1)
  { 
    // falling edge
    digitalWrite(led,LOW);
    time_falling = time;
    dt = time_falling-time_rising;
  }
  pulse_prev = pulse;

  if (dt>80)
  {
    if (dt<120)
    {
      bits += '0';
    }
    else if (dt>180 && dt<220)
    {
      bits += '1';
    }
    else if (dt>1800)
    {
      lcd.clear();
      if (decode_time(bits)==true)
      {
      	// Decode OK, show time & date.
        lcd.setCursor(0,0);
        if (hours<10) lcd.print(' ');
        lcd.print(hours);
        lcd.print(':');
        if (minutes<10) lcd.print('0');
        lcd.print(minutes);
        lcd.setCursor(0,1);
        lcd.print(day_of_month);
        lcd.print('/');
        lcd.print(month);
        lcd.print("/20");
        lcd.print(year);
      }
      else
      {
      	// Show that we are not synchronised.
        lcd.setCursor(15,1);
        lcd.print('*');
      }
      bits = "";
      seconds = 0;
    }
    lcd.setCursor(5,0);
    lcd.print(':');
    if (seconds<10) lcd.print('0');
    lcd.print(seconds);
  }
}

