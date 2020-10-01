/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * interrupt 3
 */

int led = 13;

void setup(void)
{
  Serial.begin(1200);
  pinMode(led,OUTPUT);
  digitalWrite(led,HIGH);
  Serial.println("Six sick hicks nick six slick bricks with picks and sticks");
}

void loop(void)
{
  digitalWrite(led,LOW);
}
