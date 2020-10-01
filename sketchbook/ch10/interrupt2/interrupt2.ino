/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * interrupt 2
 */

int timer0 = 6;
int led = 13;
unsigned long time_last;

extern volatile unsigned long timer0_overflow_count;
extern volatile unsigned long timer0_millis;

SIGNAL(TIMER0_COMPA_vect)
{
  timer0_millis++;
  timer0_overflow_count++;
}

void led_toggle(void)
{
  static int led_value = 0;
  digitalWrite(led,led_value);
  led_value ^= 1;
}

void setup(void)
{
  pinMode(timer0,OUTPUT);
  pinMode(led,OUTPUT);
  bitClear(TCCR0A,WGM00);
  bitSet(TCCR0A,COM0A0);
  OCR0A = 249;
  bitSet(TIMSK0,OCIE0A);
  time_last = millis();
}

void loop(void)
{
  unsigned long time = millis();
  if (time-time_last>=10)
  {
    led_toggle();
    time_last = time;
  }
}
