/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Decode NEC IR RC protocol..
 */

#define TIMEOUT  150000
#define PULSES_MAX  34
#define TOLERANCE  15  /* % */

uint8_t ir_signal = 12; // was 8, 12 is good for TSOP1738
uint8_t ir_vcc = 10;
uint8_t ir_gnd = 9;

typedef struct
{
  uint32_t timestamp;
  uint32_t on_time;
}
point_t;

point_t pulses[PULSES_MAX];
uint8_t index;

// Durations in microseconds
uint16_t startbit = 9000;
uint16_t on_time_0 = 560;
uint16_t off_time_0 = 565;
uint16_t on_time_1 = 560;
uint16_t off_time_1 = 1690;
uint32_t time_start;
uint32_t rc_code = 0;

char to_hex[16] =
{
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

void print_hex(uint32_t v)
{
	Serial.print("0x");
	Serial.print(to_hex[(v>>28)&0xf]);
	Serial.print(to_hex[(v>>24)&0xf]);
	Serial.print(to_hex[(v>>20)&0xf]);
	Serial.print(to_hex[(v>>16)&0xf]);
	Serial.print(to_hex[(v>>12)&0xf]);
	Serial.print(to_hex[(v>>8)&0xf]);
	Serial.print(to_hex[(v>>4)&0xf]);
	Serial.print(to_hex[v&0xf]);
}

boolean interval_within_bounds(uint16_t interval, uint16_t target)
{
  uint32_t dx = (uint32_t)target*TOLERANCE/100;
  return (interval>target-dx && interval<target+dx);
}

boolean bit_add(uint8_t value, uint16_t interval, uint16_t target)
{
  if (interval_within_bounds(interval,target)==true)
  {
    rc_code <<= 1;
    if (value!=0) rc_code |= 1;
    return true;
  }
  return false;
}

boolean decode_nec(void)
{
  uint8_t i;
  uint16_t interval;
  uint32_t t_prev = pulses[0].timestamp;
  uint8_t pulsecount = PULSES_MAX;
  
  rc_code = 0;
  for (i=1; i<pulsecount; i++)
  {
    // Four intervals are possible: normal "0" and "1" bits, 1st data bit & repeat code.
    interval = pulses[i].timestamp - t_prev;
    if (interval_within_bounds(interval,on_time_0+startbit/2)==true)
    {
      // First data bit.
      if (interval_within_bounds(pulses[i].on_time,on_time_0)==false)
      {
        goto decode_nec_error;
      }
    }
    else if (interval_within_bounds(interval,on_time_0+startbit/4)==true)
    {
      // Repeat code.
      if (interval_within_bounds(pulses[i].on_time,on_time_0)==false)
      {
        goto decode_nec_error;
      }
      pulsecount = 2;
    }
    else if (interval_within_bounds(interval,on_time_0+off_time_0)==true)
    {
      // Previous bit was a "0".
      if (bit_add(0,pulses[i].on_time,on_time_0)==false)
      {
        goto decode_nec_error;
      }
    }
    else if (interval_within_bounds(interval,on_time_1+off_time_1)==true)
    {
      // Previous bit was a "1".
      if (bit_add(1,pulses[i].on_time,on_time_1)==false)
      {
        goto decode_nec_error;
      }
    }
    else
    {
        goto decode_nec_error;
    }
    t_prev = pulses[i].timestamp;
  }
  return true;
  
decode_nec_error:
  Serial.print("bit ");
  Serial.print(i);
  Serial.print(", interval=");
  Serial.print(interval);
  Serial.print(", pulse=");
  Serial.print(pulses[i].on_time);
  Serial.print(", rc_code=");
  print_hex(rc_code);
  Serial.println();
  return false;
}

boolean decode(boolean startbit_only)
{
  if (interval_within_bounds(pulses[0].on_time,startbit)==true)
  {
    if (startbit_only==true)
    {
      return true;
    }
    else
    {
      return decode_nec();
    }
  }
  return false;
}

void setup(void)
{
  Serial.begin(115200);
  pinMode(ir_signal,INPUT);
  pinMode(ir_gnd,OUTPUT);
  digitalWrite(ir_gnd,LOW);
  pinMode(ir_vcc,OUTPUT);
  digitalWrite(ir_vcc,HIGH);
  // Ignore spurious pulses at startup.
  delay(500);
  while (pulseIn(ir_signal,LOW,500000)!=0);
}

void loop(void)
{
  uint32_t pulse = pulseIn(ir_signal,LOW,TIMEOUT);
  if (pulse!=0)
  {
    uint32_t time_now = micros();
    if (time_start==0)
    {
      time_start = time_now;
    }
    if (index<=PULSES_MAX)
    {
      pulses[index].timestamp = time_now - time_start;
      pulses[index].on_time = pulse;
      if (index==0 && decode(true)==false)
      {
      	// resync
        time_start = 0;
      }
      else index += 1;
    }
  }
  else
  {
    if (index>0)
    {
      if (decode(false)==true)
      {
        print_hex(rc_code);
        Serial.println();
      }
      index = 0;
    }
    time_start = 0;
  }
}
