/*
 * dimmer
 */

int led = 11;

void setup(void)
{
}

void loop(void)
{
  analogWrite(led,analogRead(A0)/4);
}

