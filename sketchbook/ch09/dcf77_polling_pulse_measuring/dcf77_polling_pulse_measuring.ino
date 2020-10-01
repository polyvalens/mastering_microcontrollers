/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * DCF77 polling pulse measuring
 */
 
int dcf77 = A2;
int pulse_prev;
unsigned long time_falling;
unsigned long time_rising;

void setup(void) 
{
  Serial.begin(115200);
  pinMode(dcf77,INPUT_PULLUP);
  pulse_prev = digitalRead(dcf77);
  time_falling = millis();
  time_rising = millis();
}

void loop(void) 
{
  unsigned long time = millis();
  int pulse = digitalRead(dcf77);

  if (pulse==1 && pulse_prev==0)
  { 
    // rising edge
    time_rising = time;
    Serial.print("lo: ");
    Serial.println(time_rising-time_falling);
  }
  else if (pulse==0 && pulse_prev==1)
  { 
    // falling edge
    time_falling = time;
    Serial.print("hi: ");
    Serial.println(time_falling-time_rising);
  }

  pulse_prev = pulse;
}

