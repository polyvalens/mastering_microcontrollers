/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * motor test 2
 */

int motor1 = 9;
int motor2 = 10;
int slider = A0;

void setup(void)
{
  Serial.begin(115200);
  pinMode(motor1,OUTPUT);
  pinMode(motor2,OUTPUT);
  pinMode(slider,INPUT);
  digitalWrite(motor1,LOW);
  digitalWrite(motor2,LOW);
}

void loop(void)
{
  Serial.println(analogRead(slider));
  delay(250);
}

