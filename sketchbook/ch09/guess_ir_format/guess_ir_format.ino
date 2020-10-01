/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * guess IR format
 */

int ir_signal = 12; // was 8, 12 is good for TSOP1738
int ir_vcc = 10;
int ir_gnd = 9;

typedef struct
{
  uint16_t startbit; // in microseconds
  char name[10];
}
protocol_t;

#define PROTOCOLS_MAX 10

const protocol_t protocols[PROTOCOLS_MAX] =
{
  { 140, "RECS80" },
  { 275, "DENON" },
  { 512, "MOTOROLA" },
  { 890, "RC5" },
  { 2400, "SIRCS" },
  { 2685, "RC6" },
  { 3380, "JAPANESE" },
  { 4500, "SAMSUNG" },
  { 8000, "DAEWOO" },
  { 9000, "NEC" },
};

boolean interval_within_bounds(uint16_t interval, uint16_t target, uint8_t tolerance)
{
  unsigned long dx = (unsigned long)target*tolerance/100;
  return (interval>target-dx && interval<target+dx);
}

void decode(unsigned long pulse)
{
  for (int i=0; i<PROTOCOLS_MAX; i++)
  {
    if (interval_within_bounds(pulse,protocols[i].startbit,10)==true)
    {
      Serial.print(protocols[i].name);
      break;
    }
  }
  Serial.print(" - ");
  Serial.println(pulse);
}

void setup(void)
{
  Serial.begin(115200);
  pinMode(ir_signal,INPUT);
  pinMode(ir_gnd,OUTPUT);
  digitalWrite(ir_gnd,LOW);
  pinMode(ir_vcc,OUTPUT);
  digitalWrite(ir_vcc,HIGH);
  // Suppress spurious pulses on power up.
  delay(500);
  while (pulseIn(ir_signal,LOW,500000)!=0);
}

void loop(void)
{
  unsigned long pulse = pulseIn(ir_signal,LOW,10000000);
  if (pulse!=0)
  {
    decode(pulse);
    while (1);
  }
}
