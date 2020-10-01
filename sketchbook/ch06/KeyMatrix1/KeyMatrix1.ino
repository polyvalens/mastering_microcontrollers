/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Key matrix 1
 */

int row0 = 4;
int row1 = 5;
int col0 = 6;
int col1 = 7;

void setup(void)
{
  Serial.begin(115200);
  pinMode(col0,OUTPUT);
  pinMode(col1,OUTPUT);
  pinMode(row0,INPUT_PULLUP);
  pinMode(row1,INPUT_PULLUP);
  digitalWrite(col0,HIGH);
  digitalWrite(col1,HIGH);
}

void loop(void)
{
  digitalWrite(col0,LOW);
  digitalWrite(col1,HIGH);
  if (digitalRead(row0)==LOW)
  {
    Serial.println("S(0,0)");
  }
  if (digitalRead(row1)==LOW)
  {
    Serial.println("S(1,0)");
  }

  digitalWrite(col0,HIGH);
  digitalWrite(col1,LOW);
  if (digitalRead(row0)==LOW)
  {
    Serial.println("S(0,1)");
  }
  if (digitalRead(row1)==LOW)
  {
    Serial.println("S(1,1)");
  }

  delay(250);
}

