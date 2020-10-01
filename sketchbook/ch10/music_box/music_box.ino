/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Music box
 */

#define BEEPS_MAX  7
#define DARK  900

int buzzer = 11;
int ldr = A3;
unsigned long int seconds;
int seconds_max;
int frequency[BEEPS_MAX] = { 2960, 2793, 2637, 3951, 2489, 3729, 3951 };
int pulse[BEEPS_MAX] = { 100, 100, 100, 100, 100, 100, 200 };
int pause[BEEPS_MAX] = { 50, 50, 150, 150, 150, 150, 150 };
 
void arm(void)
{
  seconds = 0;
  seconds_max = random(600,3600);
}

void setup(void)
{
  randomSeed(analogRead(ldr));
  delay(1000);
  randomSeed(random(1000000)*analogRead(ldr));
  arm();
}

void loop(void)
{
  delay(1000);
  if (analogRead(ldr)>DARK)
  {
    seconds += 1;
    if (seconds>=seconds_max)
    {
      for (int i=0; i<BEEPS_MAX; i++)
      {
        tone(buzzer,frequency[i],pulse[i]);
        delay(pulse[i]+pause[i]);
      }
      arm();
    }
  }
}
