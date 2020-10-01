/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Misophone
 */
 
int speaker = 9;

void setup(void)
{
}


void loop(void)
{
  unsigned int frequency = 1024 - analogRead(A0);
  frequency *= 3;
  if (frequency<100) noTone(speaker);
  else tone(speaker,frequency);
}

