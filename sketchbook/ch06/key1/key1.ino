/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Key 1
 */

int led = 13;
int key = 8;

void setup(void)
{
  pinMode(led,OUTPUT);
}

void loop(void)
{
  int val = digitalRead(key);
  digitalWrite(led,val);
}

