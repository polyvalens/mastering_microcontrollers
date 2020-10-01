/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * interrupt 4
 */

int led = 13;

void setup(void)
{
  Serial.begin(115200);
  pinMode(led,OUTPUT);
  noInterrupts();
  Serial.println("LED no blink no");
}

void loop(void)
{
  digitalWrite(led,LOW);
  delay(250);
  digitalWrite(led,HIGH);
  delay(250);
}
