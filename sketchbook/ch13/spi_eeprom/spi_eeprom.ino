#include <SPI.h>

const int ssPin = A6;
#define mySPI  SPI1

// EEPROM instructions.
#define WRITE  (0x02) /* Write one byte. */
#define WRITE_AAI  (0xad) /* Write a sequence of bytes. */
#define READ  (0x03) /* Read one or more bytes, 25 MHz max. */
#define READ_HIGH_SPEED  (0x0b) /* As READ, but up to 80 MHz. */
#define WRITE_DISABLE  (0x04)
#define WRITE_ENABLE  (0x06)
#define WRITE_STATUS_ENABLE  (0x50)
#define WRITE_STATUS  (0x01)
#define READ_STATUS  (0x05)
#define READ_ID  (0x90) /* Also 0xab */
#define READ_ID_JEDEC  (0x9f)
#define ERASE_SECTOR  (0x20) /* 4 KB */
#define ERASE_BLOCK_32K  (0x52)
#define ERASE_BLOCK_64K  (0xd8)
#define ERASE_CHIP  (0x60) /* Also 0xc7 */
#define BUSY_DISABLE  (0x80) /* Disable SO as busy signal, WRITE_AAI only. */
#define BUSY_ENABLE  (0x70) /* Enable SO as busy signal, WRITE_AAI only. */

#define DATASIZE (10)

const char toHex[16] =
{
  '0', '1', '2', '3', '4', '5', '6', '7', 
  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

void printHex(uint8_t val, bool suffix=false)
{
  if (suffix==true) Serial.print("0x");
  Serial.print(toHex[(val&0xf0)>>4]);
  Serial.print(toHex[val&0x0f]);
}

void eeprom_command(uint8_t cmd)
{
  digitalWrite(ssPin,LOW);
  mySPI.transfer(cmd);
  digitalWrite(ssPin,HIGH);
}

void eeprom_transaction_begin(uint8_t cmd, uint32_t address)
{
  digitalWrite(ssPin,LOW);
  // Send read/write command.
  mySPI.transfer(cmd);
  // Send 24-bit address.
  mySPI.transfer((address>>16)&0xff);
  mySPI.transfer((address>>8)&0xff);
  mySPI.transfer(address&0xff);
}

void eeprom_transaction_end()
{
  digitalWrite(ssPin,HIGH);
}

void eeprom_erase_block(uint8_t cmd, uint32_t address)
{
  eeprom_command(WRITE_ENABLE);
  eeprom_transaction_begin(cmd,address);
  eeprom_transaction_end();
  delay(25);
  eeprom_command(WRITE_DISABLE);
}

void eeprom_erase(void)
{
  eeprom_command(WRITE_ENABLE);
  eeprom_command(ERASE_CHIP);
  delay(50);
  eeprom_command(WRITE_DISABLE);
}

void eeprom_write_status(uint8_t data)
{
  eeprom_command(WRITE_STATUS_ENABLE);
  digitalWrite(ssPin,LOW);
  mySPI.transfer(WRITE_STATUS);
  mySPI.transfer(data);
  digitalWrite(ssPin,HIGH);
  delayMicroseconds(25);
  eeprom_command(WRITE_DISABLE);
}

void eeprom_write_byte(uint32_t address, uint8_t data)
{
  eeprom_command(WRITE_ENABLE);
  eeprom_transaction_begin(WRITE,address);
  mySPI.transfer(data);
  eeprom_transaction_end();
  delayMicroseconds(25);
  eeprom_command(WRITE_DISABLE);
}

uint32_t eeprom_write(uint32_t address, uint8_t data[], uint32_t data_size, bool so_as_busy=false)
{
  uint32_t i = 0;
  if (so_as_busy==true) eeprom_command(BUSY_ENABLE);
  eeprom_command(WRITE_ENABLE);
  eeprom_transaction_begin(WRITE_AAI,address);
  mySPI.transfer(data[i++]);
  mySPI.transfer(data[i++]);
  while (i<data_size)
  {
    // Write using address auto-increment.
    digitalWrite(ssPin,HIGH);
    delayMicroseconds(25);
    digitalWrite(ssPin,LOW);
    mySPI.transfer(WRITE_AAI);
    mySPI.transfer(data[i++]);
    mySPI.transfer(data[i++]);
  }
  eeprom_transaction_end();
  delayMicroseconds(25);
  eeprom_command(WRITE_DISABLE);
  if (so_as_busy==true) eeprom_command(BUSY_DISABLE);
  return i;
}
  
uint32_t eeprom_read(uint32_t address, uint8_t data[], uint32_t data_size, bool high_speed=false)
{
  uint32_t i = 0;
  uint8_t cmd = high_speed==true? READ_HIGH_SPEED : READ;
  eeprom_transaction_begin(cmd,address);
  // High-speed read requires dummy byte.
  if (cmd==READ_HIGH_SPEED) mySPI.transfer(0xff);
  while (i<data_size)
  {
    // READ uses address auto-increment.
    data[i++] = mySPI.transfer(0xff);
  }
  eeprom_transaction_end();
  return i;
}

uint8_t eeprom_read_status(void)
{
  uint8_t data;
  digitalWrite(ssPin,LOW);
  mySPI.transfer(READ_STATUS);
  data = mySPI.transfer(0xff);
  digitalWrite(ssPin,HIGH);
  return data;
}

uint16_t eeprom_read_id(void)
{
  uint16_t data;
  eeprom_transaction_begin(READ_ID,0);
  data = mySPI.transfer(0xff);
  data <<= 8;
  data |= mySPI.transfer(0xff);
  eeprom_transaction_end();
  return data;
}

uint32_t eeprom_read_jedec_id(void)
{
  uint32_t data;
  digitalWrite(ssPin,LOW);
  mySPI.transfer(READ_ID_JEDEC);
  data = mySPI.transfer(0xff);
  data <<= 8;
  data |= mySPI.transfer(0xff);
  data <<= 8;
  data |= mySPI.transfer(0xff);
  digitalWrite(ssPin,HIGH);
  return data;
}

void readback(void)
{
  uint8_t data[DATASIZE];
  memset(data,0x5a,DATASIZE);
  digitalWrite(9,HIGH);
  eeprom_read(0,data,DATASIZE);
  digitalWrite(9,LOW);
  for (int i=0; i<DATASIZE; i++)
  {
    printHex(data[i]);
    Serial.print(", ");
  }
  Serial.println();
}

void setup(void)
{
  uint8_t data[DATASIZE];
  int i;

  pinMode(9,OUTPUT);
  digitalWrite(9,LOW);
  pinMode(8,OUTPUT);
  digitalWrite(8,LOW);
  
  Serial.begin(115200);
  pinMode(ssPin,OUTPUT);
  mySPI.begin();
  mySPI.beginTransaction(SPISettings(125000,MSBFIRST,SPI_MODE0));

  Serial.println();
  Serial.println("SPI test");
  uint32_t id = eeprom_read_jedec_id();
  uint32_t temp = (id>>8)&0xff;
  Serial.print("Found: ");
  if (temp==0x25) Serial.print("SPI Serial Flash");
  else
  {
    Serial.print("unknown device type ");
    printHex(temp,true);
  }
  Serial.println();

  Serial.print("Manufacturer: ");
  temp = (id>>16)&0xff;
  if (temp==0xbf) Serial.print("SST");
  else
  {
    printHex(temp,true);
    Serial.print(" unknown");
  }
  Serial.println();

  Serial.print("Device: ");
  temp = id&0xff;
  if (temp==0x41) Serial.print("25VF016B");
  else
  {
    printHex(temp,true);
    Serial.print(" unknown");
  }
  Serial.println();

  Serial.print("Status register: ");
  temp = eeprom_read_status();
  printHex(temp,true);
  Serial.println();
  // Unlock all memory blocks.
  eeprom_write_status(temp&0xc3);
  Serial.print("Status register: ");
  temp = eeprom_read_status();
  printHex(temp,true);
  Serial.println();

  eeprom_erase();
  readback();
  
  // Test write byte.
  memset(data,0,DATASIZE);
  for (i=0; i<DATASIZE; i++)
  {
    digitalWrite(8,HIGH);
    eeprom_write_byte(i,i);
    digitalWrite(8,LOW);
  }
  readback();

  eeprom_erase();
  readback();

  // Test write word.
  memset(data,0,DATASIZE);
  for (i=0; i<DATASIZE; i++)
  {
    data[i] = DATASIZE - i;
  }
  digitalWrite(8,HIGH);
  eeprom_write(0,data,DATASIZE);
  digitalWrite(8,LOW);
  readback();
  
  Serial.println("---------------");
}

void loop(void)
{
  //uint16_t id = eeprom_read_jedec_id();
  //delay(1);
}
