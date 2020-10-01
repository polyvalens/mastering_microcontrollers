/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Key matrix 3
 */

const int COLUMN_MAX = 4;
const int ROW_MAX = 4;

int rows[ROW_MAX];
int columns[COLUMN_MAX];

int key_read(int r, int c)
{
  digitalWrite(c,LOW);
  int key = digitalRead(r);
  digitalWrite(c,HIGH);
  return key;
}

void setup(void)
{
  Serial.begin(115200);
  
  for (int c=0; c<COLUMN_MAX; c++)
  {
    columns[c] = c + 6;
    pinMode(columns[c],OUTPUT);
    digitalWrite(columns[c],HIGH);
  }
  
  for (int r=0; r<ROW_MAX; r++)
  {
    rows[r] = r + 2;
    pinMode(rows[r],INPUT_PULLUP);
  }
}

void loop(void)
{
  for (int r=0; r<ROW_MAX; r++)
  {
    for (int c=0; c<COLUMN_MAX; c++)
    {
      if (key_read(rows[r],columns[c])==LOW)
      {
        Serial.print('S');
        Serial.println(4*r+c+1);
      }
    }
  }

  delay(100);
}

