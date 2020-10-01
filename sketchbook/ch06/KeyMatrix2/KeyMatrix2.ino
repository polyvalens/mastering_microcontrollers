/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Key matrix 2
 */

int row0 = 2;
int row1 = 3;
int row2 = 4;
int row3 = 5;
int col0 = 6;
int col1 = 7;
int col2 = 8;
int col3 = 9;

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
  
  for (int c=col0; c<=col3; c++)
  {
    pinMode(c,OUTPUT);
    digitalWrite(c,HIGH);
  }
  
  for (int r=row0; r<=row3; r++)
  {
    pinMode(r,INPUT_PULLUP);
  }
}

void loop(void)
{
  for (int r=row0; r<=row3; r++)
  {
    for (int c=col0; c<=col3; c++)
    {
      if (key_read(r,c)==LOW)
      {
        Serial.print('S');
        Serial.println(4*(r-row0)+(c-col0)+1);
      }
    }
  }

  delay(100);
}

