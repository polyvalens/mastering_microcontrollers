/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Key 3
 */

int led = 13;
int key = 8;

void setup(void)
{
  pinMode(led,OUTPUT);
  pinMode(key,INPUT);
  digitalWrite(key,HIGH);
}

void loop(void)
{
  digitalWrite(led,digitalRead(key));
}

