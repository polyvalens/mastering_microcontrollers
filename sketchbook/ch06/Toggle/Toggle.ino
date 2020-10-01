/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Toggle
 */

int PB5 = 13;  // PB5 is pin 13 (the LED)

void setup(void)
{
  pinMode(PB5,OUTPUT);
}

void loop(void)
{
  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));
}

