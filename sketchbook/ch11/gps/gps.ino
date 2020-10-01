/*
 * GPS demo.
 * Displays GGA sentence contents (if not empty).
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

#define LF  0x0a
#define CR  0x0d
String sentence;

#define WAITING  0
#define COLLECTING  1
#define DONE  2
int state;

const int led = 13;
int seconds = 0;
uint32_t t_last = 0;

int nmea_find_field(int nr, String str)
{
  if (nr==0) return 0;
  int i = 0;
  do
  {
    i = str.indexOf(',',i);
    if (i>=0)
    {
      nr--;
      i++;
    }
  }
  while (nr>0 && i>0);
  return i;  
}

int lcd_write(int x, int y, String str)
{
  int i;
  lcd.setCursor(x,y);
  for (i=0; i<min(str.length(),LCD_CHARACTERS_PER_LINE-x); i++)
  {
    lcd.print(str[i]);
  }
  return i;
}

void lcd_write_time(int x, int y, int seconds)
{
  int hours = seconds/3600;
  seconds -= hours*3600;
  if (hours>23) hours -= 24;
  int minutes = seconds/60;
  seconds -= minutes*60;
  lcd.setCursor(x,y);
  if (hours<10) lcd.print(' ');
  lcd.print(hours);
  lcd.print(':');
  if (minutes<10) lcd.print('0');
  lcd.print(minutes);
  lcd.print(':');
  if (seconds<10) lcd.print('0');
  lcd.print(seconds);
}

void setup(void)
{
  lcd.begin(LCD_CHARACTERS_PER_LINE,LCD_LINES);
  Serial.begin(9600);
  pinMode(led,OUTPUT);
  state = WAITING;
  lcd_write(0,0,"Waiting for GPS");
  t_last = millis();
}

void loop(void)
{
  uint32_t t = millis();
  if (t-t_last>=1000)
  {
    seconds += 1;
    t_last = t;
  }
  
  if (Serial.available()>0)
  {
    char ch = Serial.read();
    Serial.print(ch);
    if (ch=='$')
    {
      state = COLLECTING;
      sentence = "";
      digitalWrite(led,HIGH);
    }
    else if (ch==CR || ch==LF)
    {
      state = DONE;
      digitalWrite(led,LOW);
    }
    if (state==COLLECTING)
    {
      sentence += ch;
    }
  }
  
  if (state==DONE)
  {
    if (sentence.startsWith("$GPGGA")==true)
    {
      int lat_start = nmea_find_field(2,sentence);
      int lon_start = nmea_find_field(4,sentence);
      int lon_end = nmea_find_field(6,sentence);
      // Don't show empty strings.
      if ((lat_start<lon_start-2) && (lon_start<lon_end-2))
      {
        int i = lcd_write(0,0," ");
        // Display latitude.
        i += lcd_write(1,0,sentence.substring(lat_start,lon_start-1));
        // Fill line with whitespace.
        for (; i<LCD_CHARACTERS_PER_LINE; i++) lcd.print(' ');
        // Display longitude.
        i = lcd_write(0,1,sentence.substring(lon_start,lon_end-1));
        // Fill line with whitespace.
        for (; i<LCD_CHARACTERS_PER_LINE; i++) lcd.print(' ');
      }
      else
      {
        // Display wait counter until a valid GGA sentence is received.
        lcd_write_time(0,1,seconds);
      }
    }
    state = WAITING;
  }
}

