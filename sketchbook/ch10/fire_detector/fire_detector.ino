/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * fire detector
 */

#include <Wire.h>

#define TEMPERATURE_MAX  25
#define MLX90614_ADDRESS  0x5a
#define MLX90614_READ_TEMPERATURE  0x07

int alarm = 12;
int mlx90614_sda = A4;
int mlx90614_scl = A5;
uint8_t crc = 0;

const uint8_t MLX90614_crc_table[256] =
{
    0,   7,  14,   9,  28,  27,  18,  21,  56,  63,  54,  49,  36,  35,  42,  45,
  112, 119, 126, 121, 108, 107,  98, 101,  72,  79,  70,  65,  84,  83,  90,  93,
  224, 231, 238, 233, 252, 251, 242, 245, 216, 223, 214, 209, 196, 195, 202, 205,
  144, 151, 158, 153, 140, 139, 130, 133, 168, 175, 166, 161, 180, 179, 186, 189,
  199, 192, 201, 206, 219, 220, 213, 210, 255, 248, 241, 246, 227, 228, 237, 234,
  183, 176, 185, 190, 171, 172, 165, 162, 143, 136, 129, 134, 147, 148, 157, 154,
   39,  32,  41,  46,  59,  60,  53,  50,  31,  24,  17,  22,   3,   4,  13,  10,
   87,  80,  89,  94,  75,  76,  69,  66, 111, 104,  97, 102, 115, 116, 125, 122,
  137, 142, 135, 128, 149, 146, 155, 156, 177, 182, 191, 184, 173, 170, 163, 164,
  249, 254, 247, 240, 229, 226, 235, 236, 193, 198, 207, 200, 221, 218, 211, 212,
  105, 110, 103,  96, 117, 114, 123, 124,  81,  86,  95,  88,  77,  74,  67,  68,
   25,  30,  23,  16,   5,   2,  11,  12,  33,  38,  47,  40,  61,  58,  51,  52,
   78,  73,  64,  71,  82,  85,  92,  91, 118, 113, 120, 127, 106, 109, 100,  99,
   62,  57,  48,  55,  34,  37,  44,  43,   6,   1,   8,  15,  26,  29,  20,  19,
  174, 169, 160, 167, 178, 181, 188, 187, 150, 145, 152, 159, 138, 141, 132, 131,
  222, 217, 208, 215, 194, 197, 204, 203, 230, 225, 232, 239, 250, 253, 244, 243
};

void MLX90614_crc_update(uint8_t value)
{
  crc = MLX90614_crc_table[value^crc];
}

uint16_t MLX90614_read(void) 
{ 
  uint16_t value = 0; 
  uint8_t pec = 0; 
  uint8_t lsb; 
  uint8_t msb; 

  // Read temperature register.
  Wire.beginTransmission(MLX90614_ADDRESS); 
  Wire.write(MLX90614_READ_TEMPERATURE); 
  // Restart without sending a stop condition.
  Wire.endTransmission(false); 
  Wire.requestFrom(MLX90614_ADDRESS,3,true); 
  if (Wire.available()!=0) lsb = Wire.read();
  if (Wire.available()!=0) msb = Wire.read();
  if (Wire.available()!=0) pec = Wire.read();
  Wire.endTransmission(true); 

  crc = 0;
  MLX90614_crc_update(MLX90614_ADDRESS<<1);
  MLX90614_crc_update(MLX90614_READ_TEMPERATURE);
  MLX90614_crc_update((MLX90614_ADDRESS<<1)|0x01);
  MLX90614_crc_update(lsb);
  MLX90614_crc_update(msb);
  if (pec==crc) value = ((msb&0x7f)<<8) + lsb;
  return value; 
}   

void setup(void)
{
  Serial.begin(115200);
  pinMode(alarm,OUTPUT);
  digitalWrite(alarm,LOW);
  pinMode(mlx90614_sda,INPUT_PULLUP);
  pinMode(mlx90614_scl,INPUT_PULLUP);
  Wire.begin(MLX90614_ADDRESS);
}

void loop(void)
{
  delay(100);
  
  float t = MLX90614_read();
  t = t*0.02 - 273.15;
  Serial.println(t);
  
  if (t>TEMPERATURE_MAX)
  {
    Serial.println("alarm!");
    digitalWrite(alarm,HIGH);
    delay(3000);
    digitalWrite(alarm,LOW);
    delay(3000);
  }
}
