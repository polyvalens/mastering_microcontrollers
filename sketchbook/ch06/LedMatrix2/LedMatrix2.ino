/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * LED matrix 2
 */

const int ROW_MAX = 4;
const int COLUMN_MAX = 4;
const int TEXT_MAX = 8;

int rows[ROW_MAX] = { 2, 3, 4, 5 };
int columns[COLUMN_MAX] = { 10, 11, 12, 13 };

int text[TEXT_MAX] = 
{ 
  0b0000011110100111,
  0b0000010110101111,
  0b0000011010011111,
  0b0000111100011110,
  0b0000000011110000,
  0b0000011110001111,
  0b0000011010010110,
  0b0000000000000000
};

int text_index = 0;
int c_offset = 0;
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
    int ch;
    int c1 = c + c_offset;
    if (c1<COLUMN_MAX)
    {
      ch = text[text_index];
    }
    else
    {
      c1 -= COLUMN_MAX;
      if (text_index+1>=TEXT_MAX) 
      {
        ch = text[0];
      }
      else
      {
        ch = text[text_index+1];
      }
    }
    for (int r=0; r<ROW_MAX; r++)
    {
      if (bitRead(ch,c1*COLUMN_MAX+r)!=0)
      {
        led_write(rows[r],columns[c]);
      }
    }
  }
  
  unsigned long int t = millis();
  if (t-t0>200)
  {
    t0 = t;
    c_offset += 1;
    if (c_offset>=COLUMN_MAX)
    {
      c_offset = 0;
      text_index += 1;
      if (text_index>=TEXT_MAX)
      {
        text_index = 0;
      }
    }
  }
}


