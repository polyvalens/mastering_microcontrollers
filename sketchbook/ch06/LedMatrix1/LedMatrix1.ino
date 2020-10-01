/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * LED matrix 1
 */

const int COLUMN_MAX = 4;
const int ROW_MAX = 4;

int rows[ROW_MAX] = { 2, 3, 4, 5 };
int columns[COLUMN_MAX] = { 10, 11, 12, 13 };
int leds = 1;

void led_write(int r, int c)
{
  digitalWrite(c,LOW);
  digitalWrite(r,HIGH);
  delay(50);
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
}

void loop(void)
{
  for (int r=0; r<ROW_MAX; r++)
  {
    for (int c=0; c<COLUMN_MAX; c++)
    {
      if (bitRead(leds,r*ROW_MAX+c)!=0)
      {
        led_write(rows[r],columns[c]);
      }
    }
  }
  
  leds *= 2;
  if (leds==0) leds = 1;
}

