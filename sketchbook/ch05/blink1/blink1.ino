/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Blink 1
 */

int led = 13;

void setup(void)
{
  pinMode(led,OUTPUT);
}

void loop(void)
{
  digitalWrite(led,HIGH); // LED on
  delay(500);             // wait 500 ms
  digitalWrite(led,LOW);  // LED off
  delay(500);             // wait 500 ms
}

