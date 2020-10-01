/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * cucaracha
 */

#define DARK  1000
#define TEMPERATURE_MIN  16
#define REPEAT_MAX  2

int ds1820_gnd = 5;
int ds1820_vdd = 3;
int relay = 12;
int ldr = A3;
int repeat;

#define DS1820_DQ_DDR  DDRD
#define DS1820_DQ_PORT  PORTD
#define DS1820_DQ_PIN  PIND
#define DS1820_DQ_BIT  4
#define DS1820_SCRATCHPAD_SIZE  9
uint8_t scratchpad[DS1820_SCRATCHPAD_SIZE];

#define DS1820_DQ_LO DS1820_DQ_DDR |= _BV(DS1820_DQ_BIT); \
                     DS1820_DQ_PORT &= ~_BV(DS1820_DQ_BIT)

#define DS1820_DQ_HI DS1820_DQ_PORT |= _BV(DS1820_DQ_BIT); \
                     DS1820_DQ_DDR &= ~_BV(DS1820_DQ_BIT)

#define DS1820_DQ_IN (DS1820_DQ_PIN & _BV(DS1820_DQ_BIT))

boolean ds1820_reset(void)
{
  boolean presence = false;
  DS1820_DQ_LO;
  delayMicroseconds(500);
  DS1820_DQ_HI;
  int timeout = 480;
  int dt = 30;
  while (timeout>0)
  {
    delayMicroseconds(dt);
    timeout -= dt;
    if (DS1820_DQ_IN==0)
    {
      presence = true;
      break;
    }
  }
  // Finish timeout.
  delayMicroseconds(timeout);
  return presence;
}

uint8_t ds1820_time_slot(uint8_t value)
{
  uint8_t result = 0;
  DS1820_DQ_LO;
  delayMicroseconds(2);
  if (value!=0) DS1820_DQ_HI;
  delayMicroseconds(10);
  if (DS1820_DQ_IN!=0) result = 1;
  delayMicroseconds(50);
  DS1820_DQ_HI;
  return result;
}

void ds1820_write_byte(uint8_t value)
{
  for (uint8_t mask=0x01; mask!=0; mask<<=1)
  {
    ds1820_time_slot(value&mask);
  }
}

uint8_t ds1820_read_byte(void)
{
  uint8_t result = 0;
  for (uint8_t mask=0x01; mask!=0; mask<<=1)
  {
    if (ds1820_time_slot(1)!=0) result |= mask;
  }
  return result;
}

boolean ds1820_read_temperature(void)
{
  boolean result = false;
  if (ds1820_reset()==true)
  {
    ds1820_write_byte(0xcc);
    ds1820_write_byte(0x44);
    while (ds1820_time_slot(1)==0);
    if (ds1820_reset()==true)
    {
      ds1820_write_byte(0xcc);
      ds1820_write_byte(0xbe);
      for (int i=0; i<DS1820_SCRATCHPAD_SIZE; i++)
      {
        scratchpad[i] = ds1820_read_byte();
      }
      result = true;
      ds1820_reset();
    }
  }
  return result;
}

void setup(void)
{
  Serial.begin(115200);
  pinMode(ds1820_gnd,OUTPUT);
  digitalWrite(ds1820_gnd,LOW);
  pinMode(ds1820_vdd,OUTPUT);
  digitalWrite(ds1820_vdd,HIGH);
  pinMode(relay,OUTPUT);
  digitalWrite(relay,LOW);
  repeat = 0;
}

void loop(void)
{
  ds1820_read_temperature();
  float t = (scratchpad[1]*256.0 + scratchpad[0])/16.0;
  Serial.println(t);
  Serial.println(analogRead(ldr));
  
  if (analogRead(ldr)>DARK)
  {
    if (t<TEMPERATURE_MIN && repeat<REPEAT_MAX)
    {
      digitalWrite(relay,HIGH);
      delay(3000);
      digitalWrite(relay,LOW);
      for (int i=0; i<5; i++)
      {
        delay(60000);
      }
      repeat += 1;
      Serial.println(repeat);
    }
  }
  else
  {
    repeat = 0;
  }
  
  delay(1000);
}
