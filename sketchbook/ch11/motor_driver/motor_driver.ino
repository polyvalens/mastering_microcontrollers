/*
 * Motor driver
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

const int speed = 9;
const int direction = 10;
const int potentiometer = A3;
const int hysteresis = 2;
int dir = LOW;

void setup(void)
{
  pinMode(speed,OUTPUT);
  pinMode(direction,OUTPUT);
  analogWrite(speed,0);
  digitalWrite(direction,dir);
}

void loop(void)
{
  // Measure the input voltage (speed).
  int spd = analogRead(potentiometer);
  spd /= 2; // Bring spd in range 0-511.
  // Use most significant bit for direction.
  if (spd>255)
  {
    spd -= 256; // Bring spd in range 0-255.
    if (spd>hysteresis && dir==HIGH)
    {
      dir = LOW; // Change direction.
    }
  }
  else
  {
    spd = 255 - spd; // Bring spd in range 255-0.
    if (spd>hysteresis && dir==LOW)
    {
      dir = HIGH; // Change direction.
    }
  }
  if (spd<hysteresis)
  {
    spd = 0; // Create a dead zone.
  }
  digitalWrite(direction,dir);
  analogWrite(speed,spd);
}

