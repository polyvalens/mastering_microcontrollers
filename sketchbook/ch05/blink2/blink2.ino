/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Blink 2
 */

int led = 13;
int state = HIGH;

void setup(void)
{
  pinMode(led,OUTPUT);
}

void loop(void)
{
  digitalWrite(led,state); // LED on or off
  if (state==HIGH)
  {
    state = LOW ;          // inverst state
  }
  else
  {
    state = HIGH ;
  }
  delay(500);             // wait 500 ms
}

