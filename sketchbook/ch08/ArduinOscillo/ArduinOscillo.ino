//************************************************************************
//					Nokia Shield O-Scope
//************************************************************************
//* Written by Peter Davenport on December 23,2010.
//* http://peterdavenport.blogspot.com/
//* Uses the Sparkfun Color LCD Shield
//************************************************************************

#include <LCD_driver.h>


int sample_last = 1; // The last value taken on the analog pin
int timebase = 50; // Delay on sample rate.
uint8_t t = 1; // Variable for time axis position


void draw_timebase(void)
{
  if (timebase==1)
  {
    LCD_PutStr("  1ms",2,4,BLUE,WHITE);
  }
  else if (timebase==50)
  {
    LCD_PutStr(" 50ms",2,4,BLUE,WHITE);
  }
  else
  {
    LCD_PutStr("100ms",2,4,BLUE,WHITE);
  }
}


void draw_screen(void)
{
  LCD_Clear(WHITE); // Clear the LCD
  // Draw indicator lines
  LCD_SetLine(128,0,128,136,BLACK); // 0
  LCD_SetLine(44,0,44,136,ORANGE); // 3V3
  LCD_SetLine(2,0,2,136,RED);  // 5V
  // Write indicator voltages
  LCD_PutStr("3.3",44,102,ORANGE,WHITE);
  LCD_PutStr("5.0",2,102,RED,WHITE);
  // Show the timebase
  draw_timebase();
}


void setup(void)
{
  LCD_Init(); // Initialize the LCD
  LCD_Contrast(44); // Sets LCD contrast,adjust this if your contrast is odd looking.

  pinMode(2,OUTPUT);
  //while (1)
  {
    LCD_Clear(WHITE); // Clear LCD to solid white
    asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PIND)), "I" (2));
  }
  LCD_PutStr("Ardu-scope",0,4,BLACK,WHITE); // Intro and instructions on screen
  LCD_PutStr("code by Peter",16,4,BLACK,WHITE);
  LCD_PutStr("Davenport!",32,4,BLACK,WHITE);
  LCD_PutStr("The program is",48,4,RED,WHITE);
  LCD_PutStr("reading analog",64,4,RED,WHITE);
  LCD_PutStr("pin #2.",80,4,RED,WHITE);
  delay(1000);

  draw_screen();
}


void loop(void)
{
  if (t<=130)
  {
    // If the X position is still on the screen then...
    int sample = (1024-analogRead(2))/8; // Read the value from the sensor for line 2
    LCD_SetLine(sample_last,t,sample,t+1,BLUE); // Draw it as a line from last value to current value.
    sample_last = sample; //Update last value
    t++; // Add one to the X position
  }
  else
  {
    // If the X position is not on the screen then...
    t = 0; // Clear the timer
    draw_screen();
    asm("sbi %0,%1" : : "I" (_SFR_IO_ADDR(PIND)), "I" (2));
  }
  
  // Now it's time to check for button presses
  int s1 = digitalRead(kSwitch1_PIN);
  int s2 = digitalRead(kSwitch2_PIN);
  int s3 = digitalRead(kSwitch3_PIN);

  if (s1==0)
  { 
    timebase = 100; // Set the timebase to 100 ms
  }
  else if (s2==0)
  {
    timebase = 50; // Set the timebase to 50 ms
  }
  else if (s3==0)
  {
    timebase = 1; // Set the timebase to 1 ms
  }
  
  if (s1 || s2 || s3) draw_timebase();
  //delay(timebase); // Wait the prescribed time
}
