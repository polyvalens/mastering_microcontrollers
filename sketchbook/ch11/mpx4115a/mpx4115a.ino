/*
 * MPX4115A pressure sensor.
 * Compare floating-point and fixed-point arithmetic.
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

void setup(void)
{
  lcd.begin(16,2);
  lcd.print("MPX4115A demo");
  lcd.setCursor(0,1);
  lcd.print("fix vs float pnt");
  delay(2500);
}

void loop(void)
{
  int p;
  int pressure_i;
  float pressure_f;
  int offset = 1016 - 993; // For my sensor.

  // Slow down.
  delay(500);

  // Read the sensor.
  p = analogRead(A1);

  // Do the calculations using floating-point arithmetic.
  pressure_f = 10.0*(5.0*p/1024.0/5.1 + 0.095)/0.009; // in mbar
  pressure_f = (int)(pressure_f + 0.5); // round the value.

  // Redo the calculations using fixed-point arithmetic
  // including rounding and error correction.
  pressure_i = 17*p + 1714;
  pressure_i >>= 4;

  // Display the results.
  lcd.clear();
  // Show fixed point result.
  lcd.setCursor(0,0);
  lcd.print("fix:   ");
  lcd.print(pressure_i+offset);
  // Show raw value too.
  lcd.setCursor(13,0);
  lcd.print(p);
  // Show floating point result.
  lcd.setCursor(0,1);
  lcd.print("float: ");
  // Print as int to hide the decimals.
  lcd.print((int)pressure_f+offset);
}
