/************************************************************************
 *
 *  LCD_driver.c: driver for Nokia LCD
 *
 ************************************************************************
 * Edit History
 * <MLS> = Mark Sproul msproul -at- jove.rutgers.edu
 ************************************************************************
 * Apr 2, 2010 <MLS> I received my Color LCD_ Shield sku: LCD_-09363 from sparkfun
 * Apr 2, 2010 <MLS> The code was written for WinAVR,I modified it to compile under Arduino
 * Apr 3, 2010 <MLS> Changed LCD_SetPixel to make it "RIGHT" side up
 * Apr 3, 2010 <MLS> Made LCD_SetPixel public
 * Apr 3, 2010 <MLS> Working on MEGA,pin defs in nokia_tester.h
 * Apr 4, 2010 <MLS> Removed delays from LCD_Command & LCD_Command,increased speed by 75%
 *
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Mods by Clemens Valens (CPV, c.valens -at- elektor.com
 * July 5, 2012 <CPV> - Adapted for Arduino 1.0.1.
 *                    - Added real SPI for increased speed.
 *
 ************************************************************************/

#include "LCD_driver.h"

#define sbi(var,mask) ((var) |= (uint8_t)(1<<mask))
#define cbi(var,mask) ((var) &= (uint8_t)~(1<<mask))
#define delay_us(a) delayMicroseconds(a)
#define delay_ms(a) /*delay(a)*/
//#define LCD_Command(a) LCD_Write(a,0)
//#define LCD_Data(a) LCD_Write(a,1)
//#define LCD_Command(a) LCD_WriteCommand(a)
//#define LCD_Data(a) LCD_WriteData(a)

#define cs_on() cbi(CS_PORT,CS_BIT) /* Activate chip select. */
#define cs_off() sbi(CS_PORT,CS_BIT) /* Deactivate chip select. */
#define mosi_lo() cbi(MOSI_PORT,MOSI_BIT)
#define mosi_hi() sbi(MOSI_PORT,MOSI_BIT)
#define sck_lo() cbi(SCK_PORT,SCK_BIT)
#define sck_hi() sbi(SCK_PORT,SCK_BIT)
#define reset_lo() cbi(RESET_PORT,RESET_BIT)
#define reset_hi() sbi(RESET_PORT,RESET_BIT)


void LCD_io_init(void)
{
  // Setup the switches for input.
  pinMode(kSwitch1_PIN,INPUT_PULLUP);
  pinMode(kSwitch2_PIN,INPUT_PULLUP);
  pinMode(kSwitch3_PIN,INPUT_PULLUP);
  
  // Setup the LCD_ control lines.
  pinMode(SS,OUTPUT); // To avoid accidentally entering SPI slave mode.
  pinMode(CS_PIN,OUTPUT);
  pinMode(MOSI,OUTPUT);
  pinMode(SCK,OUTPUT);
  pinMode(RESET_PIN,OUTPUT);
}


/*void LCD_Write(uint8_t data, uint8_t mode)
{
  cs_on();
  
  // First do 9th bit manually. 
  if (mode==0) mosi_lo(); // 9th bit low = command
  else mosi_hi(); // 9th bit high = data
  sck_lo(); // Strobe
  sck_hi();
  
  // Do the remaining 8 bits with SPI instead of bit_banging.
  // This improves performance dramatically (and saves code space).
  SPCR |= _BV(SPE); // Enable SPI.
  SPDR = data;
  while (!(SPSR & _BV(SPIF)));
  SPCR &= ~_BV(SPE); // Disable SPI.
  
  cs_off();
}*/

//#define SPI_BIT_BANG

#ifdef SPI_BIT_BANG
void LCD_Command(uint8_t data)
{
  uint8_t i;
  
  cs_on();
  
  // First do 9th bit manually. 
  mosi_lo(); // 9th bit low = command
  sck_lo(); // Strobe
  sck_hi();
  
  // Bit-bang the remaining 8 bits.
  for (i=0; i<8; i++)
  {
    if ((data&0x80)!=0) mosi_hi();
    else mosi_lo();
    sck_lo();
    sck_hi();
    data <<= 1;
  }
  
  cs_off();
}


void LCD_Data(uint8_t data)
{
  uint8_t i;

  cs_on();
  
  // First do 9th bit manually. 
  mosi_hi(); // 9th bit high = data
  sck_lo(); // Strobe
  sck_hi();
  
  // Bit-bang the remaining 8 bits.
  for (i=0; i<8; i++)
  {
    if ((data&0x80)!=0) mosi_hi();
    else mosi_lo();
    sck_lo();
    sck_hi();
    data <<= 1;
  }
  
  cs_off();
}
#else
void LCD_Command(uint8_t data)
{
  cs_on();
  
  // First do 9th bit manually. 
  mosi_lo(); // 9th bit low = command
//  sck_lo(); // Strobe
//  sck_hi();
  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  
  // Do the remaining 8 bits with SPI instead of bit_banging.
  // This improves performance 3x (and saves code space).
  SPCR |= _BV(SPE); // Enable SPI.
  SPDR = data;
  while (!(SPSR & _BV(SPIF)));
  SPCR &= ~_BV(SPE); // Disable SPI.

  cs_off();
}


void LCD_Data(uint8_t data)
{
  cs_on();
  
  // First do 9th bit manually. 
  mosi_hi(); // 9th bit high = data
//  sck_lo(); // Strobe
//  sck_hi();
  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  
  // Do the remaining 8 bits with SPI instead of bit_banging.
  // This improves performance 3x (and saves code space).
  SPCR |= _BV(SPE); // Enable SPI.
  SPDR = data;
  while (!(SPSR & _BV(SPIF)));
  SPCR &= ~_BV(SPE); // Disable SPI.

  cs_off();
}
#endif /* SPI_BIT_BANG */


void LCD_Data3xP(int data)
{
  cs_on();
  
  // First do 9th bit manually. 
  mosi_lo(); // 9th bit low = command
  sck_lo(); // Strobe
  sck_hi();
//  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
//  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  
  // Do the remaining 8 bits with SPI instead of bit_banging.
  // This improves performance 3x (and saves code space).
  SPCR |= _BV(SPE); // Enable SPI.
  SPDR = RAMWR;
  while (!(SPSR & _BV(SPIF)));
  SPCR &= ~_BV(SPE); // Disable SPI.

  cs_off();

  cs_on();
  
  // First do 9th bit manually. 
  mosi_hi(); // 9th bit high = data
  sck_lo(); // Strobe
  sck_hi();
//  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
//  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  

  // Do the remaining 8 bits with SPI instead of bit_banging.
  // This improves performance 3x (and saves code space).
  SPCR |= _BV(SPE); // Enable SPI.
  SPDR = data>>4;
  while (!(SPSR & _BV(SPIF)));
  SPCR &= ~_BV(SPE); // Disable SPI.

  cs_off();

  cs_on();
  
  // First do 9th bit manually. 
  mosi_hi(); // 9th bit high = data
  sck_lo(); // Strobe
  sck_hi();
//  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
//  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  
  // Do the remaining 8 bits with SPI instead of bit_banging.
  // This improves performance 3x (and saves code space).
  SPCR |= _BV(SPE); // Enable SPI.
  SPDR = (data<<4) | (data>>8);
  while (!(SPSR & _BV(SPIF)));
  SPCR &= ~_BV(SPE); // Disable SPI.

  cs_off();

  cs_on();
  
  // First do 9th bit manually. 
  mosi_hi(); // 9th bit high = data
  sck_lo(); // Strobe
  sck_hi();
//  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
//  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  
  // Do the remaining 8 bits with SPI instead of bit_banging.
  // This improves performance 3x (and saves code space).
  SPCR |= _BV(SPE); // Enable SPI.
  SPDR = data;
  while (!(SPSR & _BV(SPIF)));
  SPCR &= ~_BV(SPE); // Disable SPI.

  cs_off();
}


void LCD_Data3x(int data1, int data2)
{
  cs_on();
  
  // First do 9th bit manually. 
  mosi_hi(); // 9th bit high = data
//  sck_lo(); // Strobe
//  sck_hi();
  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  
  // Do the remaining 8 bits with SPI instead of bit_banging.
  // This improves performance 3x (and saves code space).
  SPCR |= _BV(SPE); // Enable SPI.
  SPDR = data1>>4;
  while (!(SPSR & _BV(SPIF)));
  SPCR &= ~_BV(SPE); // Disable SPI.

  cs_off();

  cs_on();
  
  // First do 9th bit manually. 
  mosi_hi(); // 9th bit high = data
//  sck_lo(); // Strobe
//  sck_hi();
  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  
  // Do the remaining 8 bits with SPI instead of bit_banging.
  // This improves performance 3x (and saves code space).
  SPCR |= _BV(SPE); // Enable SPI.
  SPDR = (data1<<4) | (data2>>8);
  while (!(SPSR & _BV(SPIF)));
  SPCR &= ~_BV(SPE); // Disable SPI.

  cs_off();

  cs_on();
  
  // First do 9th bit manually. 
  mosi_hi(); // 9th bit high = data
//  sck_lo(); // Strobe
//  sck_hi();
  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PINB)), "I" (5));  
  
  // Do the remaining 8 bits with SPI instead of bit_banging.
  // This improves performance 3x (and saves code space).
  SPCR |= _BV(SPE); // Enable SPI.
  SPDR = data2;
  while (!(SPSR & _BV(SPIF)));
  SPCR &= ~_BV(SPE); // Disable SPI.

  cs_off();
}


//Initializes the LCD regardless of if the controlller is an Epson or Philips.
void LCD_Init(void)
{
  LCD_io_init(); // Initialize I/O
     
  sck_lo();
  mosi_lo();
  delay_us(10);
  cs_off(); //output_high (LCD_CS);
  delay_us(10);
  reset_lo(); //output_low (LCD_RESET);
  delay_us(200);
  reset_hi(); //output_high (LCD_RESET);
  sck_hi();
  mosi_hi();
  delay_us(10);
  
  SPCR |= _BV(MSTR); // SPI in master mode,do not enable SPI here.
  SPCR &= ~(_BV(DORD)); // MSB first
  SPSR |= _BV(SPI2X); // SPI in master mode,do not enable SPI here.
  //SPCR |= _BV(SPE); // Enable SPI.

  LCD_Command(DISCTL); // display control(EPSON)
  LCD_Data(0x0C); // 12 = 1100 - CL dividing ratio [don't divide] switching period 8H (default)
  LCD_Data(0x20); 
  //LCD_Data(0x02);
  LCD_Data(0x00);
  
  LCD_Data(0x01);
  
  LCD_Command(COMSCN); // common scanning direction(EPSON)
  LCD_Data(0x01);
  
  LCD_Command(OSCON); // internal oscialltor ON(EPSON)
  
  LCD_Command(SLPOUT); // sleep out(EPSON)
  LCD_Command(P_SLEEPOUT); //sleep out(PHILIPS)
  
  LCD_Command(PWRCTR); // power ctrl(EPSON)
  LCD_Data(0x0F); //everything on,no external reference resistors
  LCD_Command(P_BSTRON); //Booset On(PHILIPS)
  
  LCD_Command(DISINV); // invert display mode(EPSON)
  LCD_Command(P_INVON); // invert display mode(PHILIPS)
  
  LCD_Command(DATCTL); // data control(EPSON)
  LCD_Data(0x03); // correct for normal sin7
  LCD_Data(0x00); // normal RGB arrangement
  //LCD_Data(0x01); // 8-bit Grayscale
  LCD_Data(0x02); // 16-bit Grayscale Type A
  
  LCD_Command(P_MADCTL); //Memory Access Control(PHILIPS)
  LCD_Data(0xC8);
  
  LCD_Command(P_COLMOD); // Set Color Mode(PHILIPS)
  LCD_Data(0x02); 
  
  LCD_Command(VOLCTR); // electronic volume,this is the contrast/brightness(EPSON)
  //LCD_Data(0x18); // volume (contrast) setting - fine tuning,original
  LCD_Data(0x24); // volume (contrast) setting - fine tuning,original
  LCD_Data(0x03); // internal resistor ratio - coarse adjustment
  LCD_Command(P_SETCON); // Set Contrast(PHILIPS)
  LCD_Data(0x30); 
  
  LCD_Command(NOP); // nop(EPSON)
  LCD_Command(P_NOP); // nop(PHILIPS)
  
  LCD_Command(DISON); // display on(EPSON)
  LCD_Command(P_DISPON); // display on(PHILIPS)
}


void LCD_Contrast(char setting) 
{
  LCD_Command(VOLCTR); // electronic volume,this is the contrast/brightness(EPSON)
  LCD_Data(setting); // volume (contrast) setting - course adjustment, -- original was 24
//  LCD_Data(0x03); // internal resistor ratio - coarse adjustment
//  LCD_Data(0x30); 
  LCD_Command(NOP); // nop(EPSON)
}


void LCD_Clear(int color)
{
  unsigned int i;
  
  #ifdef EPSON
  LCD_Command(PASET);
  LCD_Data(0);
  LCD_Data(ROWS-1);
  LCD_Command(CASET);
  LCD_Data(0);
  LCD_Data(COLS-1);
  LCD_Command(RAMWR);
  #endif
  
  #ifdef PHILIPS
  LCD_Command(PASETP);
  LCD_Data(0);
  LCD_Data(ROWS-1);
  LCD_Command(CASETP);
  LCD_Data(0);
  LCD_Data(COLS-1);
  LCD_Command(RAMWRP);
  #endif
  
  for (i=0; i<(ROWS-1)*(COLS-1)/2; i++)
  {
    LCD_Data(color>>4);
    LCD_Data((color<<4)|(color>>8));
    LCD_Data(color); // nop(EPSON)
    //LCD_Data3xP(color>>4,color&0xf); // nop(EPSON)
  }
}


void LCD_SetPixel(int color, uint8_t x, uint8_t y)
{
  y = (COLS - 1) - y;
  x = (ROWS - 1) - x;
 
  #ifdef EPSON
  LCD_Command(PASET); // page start/end ram
  LCD_Data(x);
  LCD_Data(ENDPAGE);
  
  LCD_Command(CASET); // column start/end ram
  LCD_Data(y);
  LCD_Data(ENDCOL);
  
  //LCD_Command(RAMWR); // write
  //LCD_Data(color>>4);
  //LCD_Data((color<<4)|(color>>8));
  //LCD_Data(color); // nop(EPSON)
  LCD_Data3xP(color); // nop(EPSON)
  #endif
 
  #ifdef PHILIPS
  LCD_Command(PASETP); // page start/end ram
  LCD_Data(x);
  LCD_Data(ENDPAGE);
  
  LCD_Command(CASETP); // column start/end ram
  LCD_Data(y);
  LCD_Data(ENDCOL);
  
  LCD_Command(RAMWRP); // write
  
  LCD_Data((uint8_t)((color>>4)&0x00FF));
  LCD_Data((uint8_t)(((color&0x0F)<<4)|0x00));
  #endif
}


void LCD_SetLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, int color) 
{
  //y0 = (COLS - 1) - y0;
  //x0 = (ROWS - 1) - x0;
  //y1 = (COLS - 1) - y1;
  //x1 = (ROWS - 1) - x1;

  int dy = y1 - y0;// Difference between y0 and y1
  int dx = x1 - x0;// Difference between x0 and x1
  int stepx,stepy;
  
  if (dy<0) 
  { 
    dy = -dy; 
    stepy = -1; 
  } 
  else 
  { 
    stepy = 1;
  }
  if (dx<0) 
  { 
    dx = -dx; 
    stepx = -1; 
  } 
  else 
  { 
    stepx = 1; 
  }
  dy <<= 1; // dy is now 2*dy
  dx <<= 1; // dx is now 2*dx
  LCD_SetPixel(color,x0,y0);
  if (dx>dy) 
  {
    int fraction = dy - (dx >> 1);
    while (x0 != x1) 
    {
      if (fraction >= 0) 
      {
        y0 += stepy;
        fraction -= dx;
      }
      x0 += stepx;
      fraction += dy;
      LCD_SetPixel(color,x0,y0);
    }
  } 
  else 
  {
    int fraction = dx - (dy >> 1);
    while (y0 != y1) 
    {
      if (fraction >= 0) 
      {
        x0 += stepx;
        fraction -= dy;
      }
      y0 += stepy;
      fraction += dx;
      LCD_SetPixel(color,x0,y0);
    }
  }
}


void LCD_SetRect(int x0, int y0, int x1, int y1, uint8_t fill, int color) 
{
  // check if the rectangle is to be filled
  if (fill==1) 
  {
    int xDiff;
    // Find the difference between the x vars
    if (x0>x1)
    { 
      xDiff = x0 - x1;
    }
    else
    {
      xDiff = x1 - x0;
    }
    while (xDiff>0)
    {
      LCD_SetLine(x0,y0,x0,y1,color);
      if (x0>x1)
      {
        x0--;
      }
      else
      {
        x0++;
      }
      xDiff--;
    }
  } 
  else 
  {
    // best way to draw an unfilled rectangle is to draw four lines
    LCD_SetLine(x0,y0,x1,y0,color);
    LCD_SetLine(x0,y1,x1,y1,color);
    LCD_SetLine(x0,y0,x0,y1,color);
    LCD_SetLine(x1,y0,x1,y1,color);
  }
}


void LCD_PutChar(char c, int x, int y, int fColor, int bColor) 
{
  y = (COLS - 1) - y; // make display "right" side up
  x = (ROWS - 2) - x;
  extern const uint8_t FONT8x16[97][16];
  
  int    i,j;
  unsigned int nCols;
  unsigned int nRows;
  unsigned int nBytes;
  uint8_t PixelRow;
  uint8_t Mask;
  unsigned int Word0;
  unsigned int Word1;
  uint8_t *pFont;
  uint8_t *pChar;
          
  // get pointer to the beginning of the selected font table
  pFont = (uint8_t *)FONT8x16;
  // get the nColumns,nRows and nBytes
  nCols = *pFont;
  nRows = *(pFont + 1);
  nBytes = *(pFont + 2);
  // get pointer to the last byte of the desired character
  pChar = pFont + (nBytes * (c - 0x1F)) + nBytes - 1;
  
  // Row address set (command 0x2B)
  LCD_Command(PASET);
  LCD_Data(x);
  LCD_Data(x + nRows - 1);
  // Column address set (command 0x2A)
  LCD_Command(CASET);
  LCD_Data(y);
  LCD_Data(y + nCols - 1);
  // WRITE MEMORY
  LCD_Command(RAMWR);
  
  // loop on each row,working backwards from the bottom to the top
  for (i = nRows - 1; i >= 0; i--) 
  {
    // copy pixel row from font table and then decrement row
    PixelRow = *pChar++;
    // loop on each pixel in the row (left to right)
    // Note: we do two pixels each loop
    Mask = 0x80;
    for (j=0; j<nCols; j+=2) 
    {
      // if pixel bit set,use foreground color; else use the background color
      // now get the pixel color for two successive pixels
      if ((PixelRow&Mask)==0) Word0 = bColor;
      else Word0 = fColor;
      Mask >>= 1;
      if ((PixelRow&Mask)==0) Word1 = bColor;
      else Word1 = fColor;
      Mask >>= 1;
      // use this information to output three data bytes
//      LCD_Data((Word0 >> 4) & 0xFF);
//      LCD_Data(((Word0 & 0xF) << 4) | ((Word1 >> 8) & 0xF));
//      LCD_Data(Word1 & 0xFF);
      //LCD_Data(Word0>>4);
      //LCD_Data((Word0<<4)|(Word1>>8));
      //LCD_Data(Word1);
      LCD_Data3x(Word0,Word1);
    }
  }
  // terminate the Write Memory command
  LCD_Command(NOP);
}


void LCD_PutStr(const char *pString, int x, int y, int fColor, int bColor) 
{
  x = x + 14;
  y = y + 8;
  // loop until null-terminator is seen
  while (*pString!=0) 
  {
    // draw the character
    LCD_PutChar(*pString++,x,y,fColor,bColor);
    // advance the y position
    y = y + 8;
    // bail out if y exceeds 131
    if (y>ROWS-1) break;
  }
}


void LCD_DrawCircle(int xCenter, int yCenter, int radius, int color, int circleType)
{
  int tSwitch,x1 = 0,y1 = radius;
  int Width = 2*radius,Height=Width;
  tSwitch = 3 - 2 * radius;
  
  while (x1<=y1) 
  {
    if (circleType==FULLCIRCLE || circleType==OPENSOUTH || circleType==OPENEAST || circleType==OPENSOUTHEAST) 
    {
      LCD_SetPixel(color,xCenter+radius - x1,yCenter+radius - y1);
      LCD_SetPixel(color,xCenter+radius - y1,yCenter+radius - x1);
    }
    if (circleType==FULLCIRCLE || circleType==OPENNORTH || circleType==OPENEAST || circleType==OPENNORTHEAST) 
    {
      LCD_SetPixel(color,xCenter+Width-radius + x1,yCenter+radius - y1);
      LCD_SetPixel(color,xCenter+Width-radius + y1,yCenter+radius - x1);
    }
    if (circleType==FULLCIRCLE || circleType==OPENNORTH || circleType==OPENWEST || circleType==OPENNORTHWEST) 
    {
      LCD_SetPixel(color,xCenter+Width-radius + x1,yCenter+Height-radius + y1);
      LCD_SetPixel(color,xCenter+Width-radius + y1,yCenter+Height-radius + x1);
    }
    if (circleType==FULLCIRCLE || circleType==OPENSOUTH || circleType==OPENWEST || circleType==OPENSOUTHWEST) 
    {
      LCD_SetPixel(color,xCenter+radius - x1,yCenter+Height-radius + y1);
      LCD_SetPixel(color,xCenter+radius - y1,yCenter+Height-radius + x1);
    }
    if (tSwitch<0) 
    {
      tSwitch += (4 * x1 + 6);
    } 
    else 
    {
      tSwitch += (4 * (x1 - y1) + 10);
      y1--;
    }
    x1++;
  }
}


