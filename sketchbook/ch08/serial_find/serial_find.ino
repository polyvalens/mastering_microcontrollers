/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Serial.find
 */
 
void setup(void)
{
  Serial.begin(4800);
}

void loop(void)
{
  if (Serial.find("$GPGGA,")==true)
  {
    // read the first interesting field.
  }
}

