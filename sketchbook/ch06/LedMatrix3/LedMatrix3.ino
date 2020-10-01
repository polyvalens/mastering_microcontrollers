/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * LED matrix 3
 */

const int COLUMN_MAX = 4;
const int ROW_MAX = 4;
const int LEDS_MAX = 8;

int rows[ROW_MAX] = { 2, 3, 4, 5 };
int columns[COLUMN_MAX] = { 10, 11, 12, 13 };
int leds[LEDS_MAX] = { 0x07c7, 0x05cf, 0x069f, 0x0f1e, 0x00f0, 0x078f, 0x0696, 0x0000 };
int image 0;
unsigned long int t0;

void led_write(int r, int c)
{
  digitalWrite(c,LOW);
  digitalWrite(r,HIGH);
  delay(1);
  digitalWrite(r,LOW);
  digitalWrite(c,HIGH);
}

void setup(void)
{
  int i;
  
  for (i=0; i<ROW_MAX; i++)
  {
    pinMode(rows[i],OUTPUT);
    digitalWrite(rows[i],LOW);
  }

  for (i=0; i<COLUMN_MAX; i++)
  {
    pinMode(columns[i],OUTPUT);
    digitalWrite(columns[i],HIGH);
  }
  
  t0 = millis();
}

void loop(void)
{
  for (int c=0; c<COLUMN_MAX; c++)
  {
    for (int r=0; r<ROW_MAX; r++)
    {
      if (bitRead(leds[image],c*COLUMN_MAX+r)!=0)
      {
        led_write(rows[r],columns[c]);
      }
    }
  }
  
  unsigned long int t = millis();
  if (t-t0>500)
  {
    t0 = t;
    image += 1;
    if (image>=LEDS_MAX) image = 0;
  }
}

