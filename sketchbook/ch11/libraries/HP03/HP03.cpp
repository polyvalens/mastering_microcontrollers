/*
 * Arduino driver for HP03 _pressure sensor.
 *
 * Belongs to:
 * "Mastering Microcontrollers Helped by Arduino"
 * ISBN 978-1-907920-23-3 (English)
 * ISBN 978-2-86661-190-3 (French)
 * http://www.polyvalens.com/
 *
 * For use with PolyValens Multipurpose Shield 129009-1
 *
 * Copyright (c) 2014, Clemens Valens
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 * BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#include "HP03.h"
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
#define HP03_EEPROM_SIZE  18


void HP03::begin(uint8_t mclk, uint8_t xclr)
{
  _pressure = 0;
  _temperature = 0;
  _mclk = mclk;
  _xclr = xclr;
  pinMode(_xclr,OUTPUT);
  digitalWrite(_xclr,LOW); 
  pinMode(_mclk,OUTPUT);
  digitalWrite(_mclk,LOW); 
  Wire.begin();
  read_eeprom();
}


uint8_t HP03::read_eeprom(void)
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
    _C[i] = buffer[j++];
    _C[i] <<= 8;
    _C[i] |= buffer[j++];
    // Serial.print("C");
    // Serial.print(i);
    // Serial.print("=");
    // Serial.println(_C[i]);
  }
  _AA = buffer[j++];
  // Serial.print("AA");
  // Serial.print("=");
  // Serial.println(_AA);
  _BB = buffer[j++];
  // Serial.print("BB");
  // Serial.print("=");
  // Serial.println(_BB);
  _CC = buffer[j++];
  // Serial.print("CC");
  // Serial.print("=");
  // Serial.println(_CC);
  _DD = buffer[j++];
  // Serial.print("DD");
  // Serial.print("=");
  // Serial.println(_DD);

  return i;
}


uint16_t HP03::read_value(uint8_t sensor)
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


void HP03::read(void)
{
  digitalWrite(_xclr,HIGH);
  tone(_mclk,32768);

  delay(2);

  _pressure = read_value(HP03_PRESSURE);
  _pressure += read_value(HP03_PRESSURE);
  _pressure += read_value(HP03_PRESSURE);
  _pressure += read_value(HP03_PRESSURE);
  _pressure /= 4;
  _temperature = read_value(HP03_TEMPERATURE);

  noTone(_mclk);
  digitalWrite(_xclr,LOW);
}


void HP03::calculate(void)
{
  int32_t offset;
  int32_t sensitivity;
  int32_t t;
  int32_t p;

  t = _temperature;
  t -= _C[5];
  if (t>=0)
  {
    t -= (((t*t)>>14)*_AA)>>_CC;
  }
  else
  {
    t -= (((t*t)>>14)*_BB)>>_CC;
  }
  offset = (_C[2] + (((_C[4]-1024)*t)>>14)) * 4;
  sensitivity = _C[1]+((_C[3]*t)>>10);
  p = _pressure;
  p = ((sensitivity*(p-7168))>>14) - offset;
  _pressure = ((p*10)>>5) + _C[7];
  _temperature = 250 + ((t*_C[6])>>16) - (t>>_DD);
}


void HP03::test(void)
{
  // Load the values from the datasheet example.
  _C[1] = 29908;
  _C[2] = 3724;
  _C[3] = 312;
  _C[4] = 441;
  _C[5] = 9191;
  _C[6] = 3990;
  _C[7] = 2500;
  _AA = 1;
  _BB = 4;
  _CC = 4;
  _DD = 9;
  _pressure = 30036;
  _temperature = 4107;
  calculate();
  // get_temperature() should return -7.2°C
  // get_pressure() should return 991.8 hpa
}


void HP03::update(void)
{
  Wire.begin();
  read();
  calculate();
}

