/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * voltmeter
 */

void setup(void)
{
  Serial.begin(9600);
}

void loop(void)
{
  int v = analogRead(A0);
  Serial.print("A0 = ");
  Serial.print(v);
  Serial.print(" = ");
  Serial.print(v/1024*5);
  Serial.println(" V");
  delay(1000);
}
