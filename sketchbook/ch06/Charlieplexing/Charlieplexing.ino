/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Charlieplexing
 */

int row0 = 8;
int row1 = 9;
int row2 = 10;

void setup(void)
{
  Serial.begin(115200);
  pinMode(row0,INPUT);
  pinMode(row1,INPUT);
  pinMode(row2,INPUT);
}

int key_read(int r0, int r1, int r2)
{
  pinMode(r0,OUTPUT);
  digitalWrite(r0,LOW);
  pinMode(r1,INPUT_PULLUP);
  pinMode(r2,INPUT);
  return digitalRead(r1);
}

void loop(void)
{
  if (key_read(row1,row0,row2)==LOW)
  {
    Serial.println("S(0,0) active");
  }

  if (key_read(row2,row1,row0)==LOW)
  {
    Serial.println("S(1,0) active");
  }

  if (key_read(row0,row1,row2)==LOW)
  {
    Serial.println("S(0,1) active");
  }

  if (key_read(row1,row2,row0)==LOW)
  {
    Serial.println("S(1,1) active");
  }

  delay(250);
}

