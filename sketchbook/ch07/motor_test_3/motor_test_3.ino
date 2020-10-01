/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Motor test 3
 */

int PB5 = 13;  // PB5 is pin 13 (the LED)

#define SAMPLES_MAX  512
int samples[SAMPLES_MAX];
int i = 0;
boolean off = FALSE;

int motor1 = 9;
int motor2 = 10;
int slider = A0;

void setup(void)
{
  Serial.begin(115200);
  pinMode(PB5,OUTPUT);

  pinMode(motor1,OUTPUT);
  digitalWrite(motor1,LOW);
  pinMode(motor2,OUTPUT);
  digitalWrite(motor2,LOW);
  pinMode(slider,INPUT);

  i = 0;

  digitalWrite(motor2,HIGH);
  digitalWrite(motor1,0);
}

void loop(void)
{
  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));
  
  int s = analogRead(A0);
  s += analogRead(A0);
  s += analogRead(A0);
  s += analogRead(A0);
  s /= 4;
  
  if (i<SAMPLES_MAX)
  {
    samples[i] = s;
    i += 1;
  }
  
  if (off==FALSE && s>=600)
  {
    digitalWrite(motor1,LOW);
    digitalWrite(motor2,LOW);
    off = TRUE;
  }
  
  if (i>=SAMPLES_MAX)
  {
    Serial.println(off);
    for (int j=0; j<i; j++)
    {
      Serial.print(samples[j]);
      Serial.print(','); // or use '\t' instead of ','
    }
    Serial.println();
    Serial.println("done");
    while (1);
  }
}

