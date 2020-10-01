/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * HP03 temperature & pressure sensor.
 */

#include <Wire.h>

// According to the HP03 datasheet the device's address
// is 0xEE or 0xEF but that is including the R/W bit.
// For the Wire library it has to be shifted one bit
// to the right, which gives 0x77.
#define HP03_ADDRESS  0x77
#define HP03_ADDRESS_EEPROM  0x50
#define HP03_TEMPERATURE  0xe8
#define HP03_PRESSURE  0xf0
#define HP03_START_CONVERSION  0xff
#define HP03_STOP_CONVERSION  0xfd
#define HP03_XCLR_PIN  A0
#define HP03_MCLK_PIN  A1
#define HP03_EEPROM_SIZE  18

uint32_t pressure = 0;
uint16_t temperature = 0;
int32_t C[8];
uint8_t AA;
uint8_t BB;
uint8_t CC;
uint8_t DD;


void HP03_read_eeprom(void) 
{ 
  uint8_t buffer[HP03_EEPROM_SIZE];
  uint8_t i = 0;
  uint8_t j = 0;

  Wire.beginTransmission(HP03_ADDRESS_EEPROM); 
  Wire.write(16);
  // Restart without sending a stop condition.
  Wire.endTransmission(false); 
  Wire.requestFrom(HP03_ADDRESS_EEPROM,HP03_EEPROM_SIZE,true); 
  while (Wire.available()!=0 && i<HP03_EEPROM_SIZE)
  {
    buffer[i++] = Wire.read();
  }

  j = 0;
  for (i=1; i<8; i++)
  {
    C[i] = buffer[j++];
    C[i] <<= 8;
    C[i] |= buffer[j++];
    Serial.print("C");
    Serial.print(i);
    Serial.print("=");
    Serial.println(C[i]);
  }
  AA = buffer[j++];
  Serial.print("AA");
  Serial.print("=");
  Serial.println(AA);
  BB = buffer[j++];
  Serial.print("BB");
  Serial.print("=");
  Serial.println(BB);
  CC = buffer[j++];
  Serial.print("CC");
  Serial.print("=");
  Serial.println(CC);
  DD = buffer[j++];
  Serial.print("DD");
  Serial.print("=");
  Serial.println(DD);
}   

uint16_t HP03_read_value(uint8_t sensor) 
{ 
  uint16_t value = 0; 

  // Start a conversion.
  Wire.beginTransmission(HP03_ADDRESS); 
  Wire.write(HP03_START_CONVERSION); 
  Wire.write(sensor); 
  Wire.endTransmission(true);
  
  delay(40); 

  // Get the conversion result.
  Wire.beginTransmission(HP03_ADDRESS); 
  Wire.write(HP03_STOP_CONVERSION);
  // Restart without sending a stop condition.
  Wire.endTransmission(false); 
  Wire.requestFrom(HP03_ADDRESS,2,true); 
  if (Wire.available()!=0) value = Wire.read() << 8;
  if (Wire.available()!=0) value |= Wire.read();
  return value; 
}   

void HP03_read(void) 
{ 
  digitalWrite(HP03_XCLR_PIN,HIGH); 
  tone(HP03_MCLK_PIN,32768); 

  delay(2); 
       
  pressure = HP03_read_value(HP03_PRESSURE);
  pressure += HP03_read_value(HP03_PRESSURE);
  pressure += HP03_read_value(HP03_PRESSURE);
  pressure += HP03_read_value(HP03_PRESSURE);
  pressure /= 4;
  temperature = HP03_read_value(HP03_TEMPERATURE);

  noTone(HP03_MCLK_PIN); 
  digitalWrite(HP03_XCLR_PIN,LOW); 
}

void HP03_calculate(int32_t& t, int32_t& p)
{
  int32_t offset;
  int32_t sensitivity;
  t = temperature;
  t -= C[5];
  if (t>=0)
  {
    t -= (((t*t)>>14)*AA)>>CC;
  }
  else
  {
    t -= (((t*t)>>14)*BB)>>CC;
  }
  offset = (C[2] + (((C[4]-1024)*t)>>14)) * 4;
  sensitivity = C[1]+((C[3]*t)>>10);
  p = pressure;
  p = ((sensitivity*(p-7168))>>14) - offset;
  p = ((p*10)>>5) + C[7];
  t = 250 + ((t*C[6])>>16) - (t>>DD);
}

void HP03_test(void) 
{ 
  C[1] = 29908;
  C[2] = 3724;
  C[3] = 312;
  C[4] = 441;
  C[5] = 9191;
  C[6] = 3990;
  C[7] = 2500;
  AA = 1;
  BB = 4;
  CC = 4;
  DD = 9;
  pressure = 30036;
  temperature = 4107;
}

void setup(void)
{
  pinMode(HP03_XCLR_PIN,OUTPUT);
  digitalWrite(HP03_XCLR_PIN,LOW); 
  pinMode(HP03_MCLK_PIN,OUTPUT);
  digitalWrite(HP03_MCLK_PIN,LOW); 
  Serial.begin(115200);
  Wire.begin();
  HP03_read_eeprom();
}

void loop(void)
{
  int32_t t;
  int32_t p;
  
  HP03_read();
  Serial.print("T=");
  Serial.print(temperature);
  Serial.print(", P=");
  Serial.print(pressure);
  //HP03_test();
  HP03_calculate(t,p);
  Serial.print(", T=");
  Serial.print((float)t/10.0/*emperature*/);
  Serial.print(", P=");
  Serial.println((float)p/10.0/*ressure*/);
  delay(1000);
}

