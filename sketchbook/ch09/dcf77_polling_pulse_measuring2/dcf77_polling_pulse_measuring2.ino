/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * DCF77 polling pulse measuring 2
 */
 
int dcf77 = A2;
int pulse_prev;
unsigned long time_falling;
unsigned long time_rising;
String bits;

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
  unsigned long dt = 0;
  int pulse = digitalRead(dcf77);

  if (pulse==1 && pulse_prev==0)
  { 
    // rising edge
    time_rising = time;
    dt = time_rising-time_falling;
  }
  else if (pulse==0 && pulse_prev==1)
  { 
    // falling edge
    time_falling = time;
    dt = time_falling-time_rising;
  }
  pulse_prev = pulse;

  if (dt>50)
  {
    if (dt<150)
    {
      bits += '0';
    }
    else if (dt<250)
    {
      bits += '1';
    }
    else if (dt>1500)
    {
      Serial.println(bits);
      bits = "";
    }
  }
}

