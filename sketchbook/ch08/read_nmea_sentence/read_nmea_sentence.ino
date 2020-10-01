/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * read nmea sentence
 */

#include <LiquidCrystal.h>
#define LCD_LINES  2
#define LCD_CHARACTERS_PER_LINE  16
LiquidCrystal lcd(2,3,4,5,6,7);

#define LF  0x0a
#define CR  0x0d
#define SENTENCE_MAX  128
char sentence[SENTENCE_MAX];
int sentence_length;

#define WAITING  0
#define COLLECTING  1
#define DONE  2
int state;

void lcd_write_line(int line, char str[])
{
  lcd.setCursor(0,line);
  for (int i=0; i<min(strlen(str),LCD_CHARACTERS_PER_LINE); i++)
  {
    lcd.print(str[i]);
  }
}

void setup(void)
{
  lcd.begin(LCD_CHARACTERS_PER_LINE,LCD_LINES);
  Serial.begin(9600);
  state = WAITING;
  sentence_length = 0;
}

void loop(void)
{
  if (Serial.available()>0)
  {
    char ch = Serial.read();
    if (ch=='$')
    {
      state = COLLECTING;
      sentence_length = 0;
    }
    else if (ch==CR || ch==LF)
    {
      state = DONE;
      sentence[sentence_length] = 0;
    }
    if (state==COLLECTING && sentence_length<SENTENCE_MAX)
    {
      sentence[sentence_length] = ch;
      sentence_length++;
    }
  }
  
  if (state==DONE)
  {
    // display a maximum of 16 characters on the first line
    lcd_write_line(0,sentence);
    if (strlen(sentence)>LCD_CHARACTERS_PER_LINE)
    {
      // if there are more characters,
      // display a maximum of 16 characters on the second line
      lcd_write_line(1,sentence+LCD_CHARACTERS_PER_LINE);
    }
    state = WAITING;
  }
}

