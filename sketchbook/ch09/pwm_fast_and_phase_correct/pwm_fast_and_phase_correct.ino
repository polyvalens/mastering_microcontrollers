/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * fast pwm & phase correct pwm
 */

int oc0a = 6;
int oc0b = 5;
int oc1a = 9;
int oc1b = 10;
int oc2a = 11;
int oc2b = 3;

void setup(void)
{
  int pwma = 75;
  int pwmb = 150;
  analogWrite(oc0a,pwma);
  analogWrite(oc0b,pwmb);
  analogWrite(oc1a,pwma);
  analogWrite(oc1b,pwmb);
  analogWrite(oc2a,pwma);
  analogWrite(oc2b,pwmb);
}

void loop(void)
{
}

