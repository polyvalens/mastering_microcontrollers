/*
 * SHT1x demo
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
LiquidCrystal lcd(6,7,5,4,3,2);

#define __CELSIUS__

//#define __USE_CRC__
#include "SHT1x.h"
SHT1x sht1x;
const int led = 13;

// Custom degree character.
uint8_t degree[8] = 
{
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
};

void setup(void)
{
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
  lcd.createChar(0,degree);
  lcd.begin(16,2);
  lcd.print("SHT1x demo");
  delay(1500);
}

void loop(void)
{
  digitalWrite(led,HIGH);
  sht1x.begin(SDA,SCL,true);
  sht1x.update();
  float rh = (10.0*sht1x.get_humidity() + 0.5)/10.0;
  float temperature = sht1x.get_temperature();
#ifndef __CELSIUS__
  // Celsius to Fahrenheit.
  temperature = temperature*9.0/5.0 + 32.0;
#endif /* __CELSIUS__ */
  temperature = (10.0*temperature + 0.5)/10.0;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T ");
  lcd.print(temperature,1);
  lcd.write((uint8_t)0);
  lcd.print(" DP ");
  lcd.print(sht1x.get_dewpoint(),1);
  lcd.write((uint8_t)0);
  lcd.setCursor(0,1);
  lcd.print("RH ");
  lcd.print(rh,1);
  lcd.print('%');

  digitalWrite(led,LOW);
  delay(1000);
}

