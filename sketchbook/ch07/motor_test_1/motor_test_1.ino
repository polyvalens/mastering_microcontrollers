/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * motor test 1
 */

int motor1 = 9;
int motor2 = 10;

void setup(void)
{
  pinMode(motor1,OUTPUT);
  pinMode(motor2,OUTPUT);
  digitalWrite(motor2,LOW);
  digitalWrite(motor1,HIGH);
  delay(500);
  digitalWrite(motor1,LOW);
}

void loop(void)
{
}

