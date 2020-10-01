/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * The difference between write and print
 */
 
void setup(void)
{
  Serial.begin(115200);
  Serial.println(150);
  Serial.write(150);
}

void loop(void)
{
}

