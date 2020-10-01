/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * buzzer 1
 */

#define BEEPS_MAX  7
#define DARK  900

int buzzer = 11;
int ldr = A3;
unsigned long int seconds;
int seconds_max;
int beeps;
int frequency[BEEPS_MAX];
int pulse[BEEPS_MAX];
int pause[BEEPS_MAX];

void arm(void)
{
  seconds = 0;
  seconds_max = random(600,3600);
  beeps = random(1,BEEPS_MAX);
  for (int i=0; i<beeps; i++)
  {
    pulse[i] = random(100,500);
    pause[i] = random(500,1500);
    frequency[i] = random(4000,5000);
  }
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
      for (int i=0; i<beeps; i++)
      {
        tone(buzzer,frequency[i],pulse[i]);
        delay(pulse[i]+pause[i]);
      }
      arm();
    }
  }
}
