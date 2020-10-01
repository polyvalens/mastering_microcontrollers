/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Key 2
 */

int led = 13;
int key = 8;

void setup(void)
{
  pinMode(led,OUTPUT);
  pinMode(key,INPUT_PULLUP);
}

void loop(void)
{
  int val = digitalRead(key);
  digitalWrite(led,val);
}

