/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * weather3
 * Display temperature, pressure & humidity
 */
 
#include <LiquidCrystal.h>
LiquidCrystal lcd(2,3,4,5,6,7);

#define __SHT1__
#define __HP03__
#define __CELSIUS__

#ifdef __SHT1__
//#define __USE_CRC__
#include <SHT1x.h>
SHT1x sht1x;
#endif /* __SHT1__ */

#ifdef __HP03__
#include <Wire.h>
#include <HP03.h>
HP03 hp03;
#endif /* __HP03__ */

// Custom degree character.
uint8_t degree[8] = 
{
#ifdef __CELSIUS__
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00011,
  0b00100,
  0b00100,
  0b00011
#else
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00111,
  0b00100,
  0b00110,
  0b00100
#endif /* __CELSIUS__ */
};

void setup(void)
{
  // Serial.begin(115200);
  lcd.createChar(0,degree);
  lcd.begin(16,2);
  lcd.print("Arduino Weather");
  //lcd.setCursor(0,1);
  //lcd.print("");
  delay(750);
  lcd.clear();
  //lcd.setCursor(0,0);
  lcd.print(" T   RH  mbar");
  //hp03.begin(A1,A0);
}

void loop(void)
{
  int t = 0;
  int p = 0;
  int rh = 0;
  
#ifdef __HP03__
  hp03.begin(A1,A0);
  hp03.update();
  t = hp03.get_temperature() + 0.5;
  p = hp03.get_pressure() + 0.5;
#endif /* __HP03__ */

#ifdef __SHT1__
  sht1x.begin(SDA,SCL,true);
  sht1x.update();
  t += sht1x.get_temperature() + 0.5;
  t /= 2;
  rh = sht1x.get_humidity() + 0.5;
#endif /* __SHT1__ */

#ifndef __CELSIUS__
  t = t*9/5 + 32; // Celsius to Fahrenheit.
#endif /* __CELSIUS__ */

  lcd.setCursor(0,1);
  //lcd.print("                ");
  //lcd.setCursor(0,1);
  if (t>=0 && t<100) lcd.print(' ');
  lcd.print(t);
  //lcd.write((uint8_t)0xdf); // Default degree symbol
  lcd.write((uint8_t)0);
  lcd.print(' ');
  lcd.print(rh);
  lcd.print("% ");
  lcd.print(p);

  delay(1000);
}

