/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * multichannel voltmeter
 */

void setup(void)
{
  Serial.begin(9600);
}

void loop(void)
{
  for (int i=0; i<6; i++)
  {
    Serial.print("A");
    Serial.print(i);
    Serial.print("=");
    Serial.print(analogRead(i)/1024.0*5);
    Serial.print("V, ");
    if (i<5) Serial.print(", ");
  }
  Serial.println(".");
  delay(1000);
}

