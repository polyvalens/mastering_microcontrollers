/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * interrupt 5
 */

int Q = 13;
int not_Q = 12;
int S = 2;
int R = 3;

void reset(void)
{
  digitalWrite(Q,LOW);
  digitalWrite(not_Q,HIGH);
  //interrupts();
  //while (true);
}

void set(void)
{
  digitalWrite(not_Q,LOW);
  digitalWrite(Q,HIGH);
}

void setup(void)
{
  pinMode(Q,OUTPUT);
  pinMode(not_Q,OUTPUT);
  digitalWrite(Q,LOW);
  digitalWrite(not_Q,HIGH);
  pinMode(S,INPUT_PULLUP);
  pinMode(R,INPUT_PULLUP);
  attachInterrupt(0,set,LOW);
  attachInterrupt(1,reset,LOW);
}

void loop(void)
{
}
