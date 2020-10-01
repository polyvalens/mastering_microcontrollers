/*
 * LCD
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
  lcd.print("Welcome,");
  lcd.setCursor(0,1);
  lcd.print("Master of MCU");
}

void loop(void) 
{
}

