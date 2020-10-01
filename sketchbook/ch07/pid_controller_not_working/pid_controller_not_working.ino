/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Not working PID controller
 */

char motor1 = 9;
char motor2 = 10;
char slider = A0;
const float Kr = 1.95; // Proportional gain.
float yi[2] = { 0.0, 0.0 }; // Low-pass filter 1.
float ki[2];
float yd[2] = { 0.0, 0.0 }; // Low-pass filter 2.
float kd[2];
int target = 500; // Target value.

float lpf(int sample, float coeffs[], float y[])
{
  y[0] = coeffs[1]*sample + coeffs[0]*y[1];
  y[1] = y[0]; // One-sample delay.
  return y[0];
}

void setup(void)
{
  pinMode(motor1,OUTPUT);
  digitalWrite(motor1,LOW);
  pinMode(motor2,OUTPUT);
  digitalWrite(motor2,LOW);
  pinMode(slider,INPUT);
}

void loop(void)
{
  int error;
  float spd_p;
  float spd_d;
  float spd_i;
  int spd;

  int val = analogRead(slider);

  // Calculate error.
  error = target - val;

  // Calculate proportionnal part (P).
  spd_p = error*Kr;
  // Calculate integral part (I).
  spd_i = lpf(error,ki,yi);
  // Calculate differential part (D).
  spd_d = error - lpf(error,kd,yd);
  // Add P, I & D.
  spd = spd_p + spd_i + spd_d;

  if (spd<0)
  {
    // Clockwise.
    digitalWrite(motor1,HIGH);
    analogWrite(motor2,-spd);
  }
  else
  {
    // Counter clockwise.
    digitalWrite(motor2,HIGH);
    analogWrite(motor1,spd);
  }
}

