/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * read nmea sentence 3
 */

#include <LiquidCrystal.h>
#define LCD_LINES  2
#define LCD_CHARACTERS_PER_LINE  16
LiquidCrystal lcd(2,3,4,5,6,7);

#define LF  0x0a
#define CR  0x0d
String sentence;

#define WAITING  0
#define COLLECTING  1
#define DONE  2
int state;

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

void lcd_write_line(int line, String str)
{
  lcd.setCursor(0,line);
  for (int i=0; i<min(str.length(),LCD_CHARACTERS_PER_LINE); i++)
  {
    lcd.print(str[i]);
  }
}

void setup(void)
{
  lcd.begin(LCD_CHARACTERS_PER_LINE,LCD_LINES);
  Serial.begin(9600);
  state = WAITING;
}

void loop(void)
{
  if (Serial.available()>0)
  {
    char ch = Serial.read();
    if (ch=='$')
    {
      state = COLLECTING;
      sentence = "";
    }
    else if (ch==CR || ch==LF)
    {
      state = DONE;
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
      lcd_write_line(0,sentence.substring(lat_start,lon_start-1));
      lcd_write_line(1,sentence.substring(lon_start,lon_end-1));
    }
    state = WAITING;
  }
}

