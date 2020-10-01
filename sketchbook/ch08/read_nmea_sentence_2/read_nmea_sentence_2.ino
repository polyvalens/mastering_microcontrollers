/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * read nmea sentence 2
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
    //Serial.print(ch);
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
    // display a maximum of 16 characters on the first line
    lcd_write_line(0,sentence);
    if (sentence.length()>LCD_CHARACTERS_PER_LINE)
    {
      // if there are more characters,
      // display a maximum of 16 characters on the second line
      lcd_write_line(1,sentence.substring(LCD_CHARACTERS_PER_LINE));
    }
    state = WAITING;
  }
}

