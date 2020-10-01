/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * PID controller
 */

char motor1 = 9;
char motor2 = 10;
char slider = A0;

const float Fs = 1760; // Hz, sample rate
const float Kr = 1.45; // Proportional gain.
// Integral filter.
const float Fci = 1.5; // Hz
float ki[2];
float yi[2];
// Differential filter.
const float Fcd = 5.8; // Hz
float kd[2];
float yd[2];
int target = 500;

void lpf_init(float fs, float fc, float coeffs[], float y[])
{
  coeffs[0] = exp(-TWO_PI*fc/fs); // k
  coeffs[1] = 1.0 - coeffs[0]; // 1-k
  y[0] = 0.0;
  y[1] = 0.0;
}

float lpf(int sample, float coeffs[], float y[])
{
  // y[n] = (1-k)*x[n] + k*y[n-1]
  // k = e^(-2*pi*fc/fs)
  // fs = sample rate
  // fc = cut-off frequency
  y[0] = coeffs[1]*sample + coeffs[0]*y[1];
  y[1] = y[0]; // delay sample
  return y[0];
}

void setup(void)
{
  Serial.begin(115200);
  
  pinMode(motor1,OUTPUT);
  digitalWrite(motor1,LOW);
  pinMode(motor2,OUTPUT);
  digitalWrite(motor2,LOW);

  lpf_init(Fs,Fci,ki,yi);
  lpf_init(Fs,Fcd,kd,yd);
}

void loop(void)
{
  int error;
  float spd_p;
  float spd_d;
  float spd_i;
  int spd;

  // Sample 4 times to slow down loop to about 2 kHz
  int val = analogRead(slider);
  val += analogRead(slider);
  val += analogRead(slider);
  val += analogRead(slider);
  val >>= 2;

  error = target - val;  // calculate error
  spd_p = error*Kr;  // proportional part P
  spd_i = lpf(error,ki,yi);  // integral part I
  spd_d = error - lpf(error,kd,yd);  // differential part D
  spd = int(spd_p + spd_i + spd_d);  // PID together
  spd = constrain(spd,-255,255);  // keep speed within limits

  // Sign of speed determines direction. 
  if (spd<0)
  {
    // Rotate counter clockwise.
    digitalWrite(motor2,LOW);
    analogWrite(motor1,-spd);
  }
  else
  {
    // Rotate clockwise.
    digitalWrite(motor1,LOW);
    analogWrite(motor2,spd);
  }

  // See if a new target value was received.
  if (Serial.available()>=3)
  {
    target = Serial.read() - '0'; // Hundreds.
    target = Serial.read() - '0' + target*10; // Tens.
    target = Serial.read() - '0' + target*10; // Units.
  }
}

