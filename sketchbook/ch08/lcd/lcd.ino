/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * LCD
 */

#include <LiquidCrystal.h>

LiquidCrystal lcd(2,3,4,5,6,7);

void setup(void)
{
  lcd.begin(16,2);
  lcd.print("hello");
  lcd.setCursor(0,1);
  lcd.print("everyone!");
}

void loop(void) 
{
}

