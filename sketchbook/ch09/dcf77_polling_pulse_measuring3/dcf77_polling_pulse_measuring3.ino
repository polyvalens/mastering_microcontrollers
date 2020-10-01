/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * DCF77 polling pulse measuring 3
 */
 
int dcf77 = A2;
int pulse_prev;
unsigned long time_falling;
unsigned long time_rising;
String bits;
int seconds;
int minutes;
int hours;
int day_of_month;
int day_of_week;
int month;
int year;

boolean parity(String str, char parity_bit)
{
  unsigned int i;
  int sum = 0;
  for (i=0; i<str.length(); i++)
  {
    if (str[i]=='1') sum += 1;
  }
  return ((sum&1)+0x30 == parity_bit);
}

boolean verify(String str)
{
  if (str.length()!=59) return false;
  if (str[20]!='1') return false;
  if (parity(str.substring(21,28),str[28])==false) return false;
  if (parity(str.substring(29,35),str[35])==false) return false;
  if (parity(str.substring(36,58),str[58])==false) return false;
  return true;
}

/*boolean verify(String str)
{
  if (str.length()!=59) {Serial.print("length ");return false;}
  if (str[20]!='1') {Serial.print("bit20 ");return false;}
  if (parity(str.substring(21,28),str[28])==false) {Serial.print("minutes ");return false;}
  if (parity(str.substring(29,35),str[35])==false) {Serial.print("hours ");return false;}
  if (parity(str.substring(36,58),str[58])==false) {Serial.print("date ");return false;}
  return true;
}*/

int decode_bcd(String str)
{
  unsigned int i;
  int power_of_two;
  int power_of_ten = 1;
  int result = 0;
  for (i=0; i<str.length(); i++)
  {
    power_of_two = 1<<(i&0x3);
    result += (str[i]-'0')*power_of_two*power_of_ten;
    if (power_of_two==8) power_of_ten *= 10;
  }
  return result;
}

boolean decode_time(String &str)
{
  if (verify(str)==false) return false;
  minutes = decode_bcd(str.substring(21,28));
  hours = decode_bcd(str.substring(29,35));
  day_of_month = decode_bcd(str.substring(36,42));
  day_of_week = decode_bcd(str.substring(42,45));
  month = decode_bcd(str.substring(45,50));
  year = decode_bcd(str.substring(50,58));
  return true;
}

void setup(void) 
{
  Serial.begin(115200);
  pinMode(dcf77,INPUT_PULLUP);
  pulse_prev = digitalRead(dcf77);
  time_falling = millis();
  time_rising = millis();
  seconds = 0;
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
    seconds += 1;
  }
  else if (pulse==0 && pulse_prev==1)
  { 
    // falling edge
    time_falling = time;
    dt = time_falling-time_rising;
  }
  pulse_prev = pulse;

  if (dt>80)
  {
    Serial.write(0x0d);
    Serial.print(seconds);
    if (seconds<10) Serial.print(' ');

    if (dt<120)
    {
      bits += '0';
    }
    else if (dt>180 && dt<220)
    {
      bits += '1';
    }
    else if (dt>1800)
    {
      Serial.println();
      //Serial.println(bits);
      if (decode_time(bits)==true)
      {
        Serial.print(hours);
        Serial.print(':');
        Serial.println(minutes);
        Serial.print(day_of_month);
        Serial.print('/');
        Serial.print(month);
        Serial.print("/20");
        Serial.println(year);
      }
      else Serial.println("error");
      bits = "";
      seconds = 0;
    }
  }
}

