/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * gps inverter
 */
 
#define __HAS_GPS__
#define __HAS_LCD__

/* 
 * There is a bug somewhere in the Arduino IDE
 * that breaks typedefs and (sometimes?) ifdefs too.
 * A work-around is to include arduino.h in the sketch.
 */
#include <arduino.h>

#ifdef __HAS_LCD__
#include <LiquidCrystal.h>
#define LCD_LINES  2
#define LCD_CHARACTERS_PER_LINE  16
LiquidCrystal lcd(2,3,4,5,6,7);
#endif /* __HAS_LCD__ */

#define LF  0x0a
#define CR  0x0d
String sentence;

#define WAITING  0
#define COLLECTING  1
#define DONE  2
int state;

float fix_lat;
float fix_lon;
int decimal_digits;
boolean inverting = true;
boolean need_fix = true;
int seconds = 0;

const char to_hex[16] =
{
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

int nmea_find_field(int nr, String str)
{
  if (nr==0) return 0;
  int i = 0;
  do
  {
    i = str.indexOf(',',i);
    if (i>=0)
    {
      nr--;
      i++;
    }
  }
  while (nr>0 && i>0);
  return i;  
}

void nmea_calculate_checksum(String& str)
{
  unsigned int i;
  unsigned char checksum = 0;
  for (i=1; i<str.length(); i++)
  {
    if (str[i]=='*') break;
    checksum ^= str[i];
  }
  checksum &= 0xff;
  str[i+1] = to_hex[(checksum&0xf0)>>4];
  str[i+2] = to_hex[checksum&0x0f];
}

float string_to_float(String str)
{
  int dp = str.indexOf('.');
  decimal_digits = str.length() - dp -1;
  float precision = pow(10,decimal_digits);
  float result = str.substring(0,dp).toInt();
  result += str.substring(dp+1).toInt()/precision;
  return result;
}

void int_to_string_zeropad(long value, int digits, String& s)
{
  while (value<pow(10,digits-1) && digits>1)
  {
    s += '0';
    digits -= 1;
  }
  s += value;
}

void float_to_string(float f, int digits, int decimals, String& s)
{
  long fi = (long)f;
  f = abs((f-fi)*pow(10,decimals));
  long fd = (long)f;
  int_to_string_zeropad(fi,digits,s);
  s += '.';
  int_to_string_zeropad(fd,decimals,s);
}

void string_set_substring(int start, String& dst, String src)
{
  for (unsigned int i=0; i<src.length(); i++)
  {
    dst[start+i] = src[i];
  }
}

float string_to_lat_lon(String str)
{
  int dp = str.indexOf('.');
  int cardinal = str.indexOf(',');
  float degs = str.substring(0,dp-2).toInt();
  degs += string_to_float(str.substring(dp-2,cardinal))/60.0;
  if (str[cardinal+1]=='S' || str[cardinal+1]=='W') degs *= -1;
  return degs;
}

void lat_lon_to_string(float value, boolean is_longitude, String& str)
{
  boolean negative = value<0;
  value = abs(value);
  int degs = (int)value;
  int_to_string_zeropad(degs,is_longitude?3:2,str);
  float_to_string((value-degs)*60,2,decimal_digits,str);
  str += ',';
  if (is_longitude==true)
  {
    if (negative==true) str += 'W';
    else str += 'E';
  }
  else
  {
    if (negative==true) str += 'S';
    else str += 'N';
  }
}

void set_lat_lon(int field, float value, boolean is_longitude, String& dst)
{
  String str;
  int pos_start = nmea_find_field(field,dst);
  lat_lon_to_string(value,is_longitude,str);
  string_set_substring(pos_start,dst,str);
}

void set_gga_position(float latitude, float longitude, String& gga)
{
  set_lat_lon(2,latitude,false,gga);
  set_lat_lon(4,longitude,true,gga);
}

float get_lat_lon(int field, String sentence)
{
  int value_start = nmea_find_field(field,sentence);
  int value_end = nmea_find_field(field+2,sentence);
  return string_to_lat_lon(sentence.substring(value_start,value_end-1));
}

void setup(void)
{
#ifdef __HAS_LCD__
  lcd.begin(LCD_CHARACTERS_PER_LINE,LCD_LINES);
#endif /* __HAS_LCD__ */
  Serial.begin(9600);
  state = WAITING;
#ifndef __HAS_GPS__
  sentence = "$GPGGA,132202.00,4740.94223,N,00257.63842,W,1,08,1.16,44.8,M,49.2,M,,*75";
#endif /* __HAS_GPS__ */
}

void loop(void)
{
#ifdef __HAS_GPS__
  if (Serial.available()>0)
  {
    char ch = Serial.read();
    if (ch=='$')
    {
      state = COLLECTING;
      sentence = "";
    }
    else if (ch==CR || ch==LF)
    {
      state = DONE;
    }
    if (state==COLLECTING)
    {
      sentence += ch;
    }
  }
#else
  delay(1000);
  state = DONE;
#endif /* __HAS_GPS__ */
  
  if (state==DONE)
  {
    if (sentence.startsWith("$GPGGA")==true)
    {
      float latitude = get_lat_lon(2,sentence);
      float longitude = get_lat_lon(4,sentence);
      
      if (inverting==true)
      {
        if (need_fix==true)
        {
          fix_lat = latitude;
          fix_lon = longitude;
          need_fix = false;
          seconds = 0;
        }
        else
        {
          float delta_lat = fix_lat - latitude;
          float delta_lon = fix_lon - longitude;
          latitude = fix_lat + delta_lat;
          longitude = fix_lon + delta_lon;
          set_gga_position(latitude,longitude,sentence);
          nmea_calculate_checksum(sentence);
          seconds += 1;
          if (seconds>30)
          {
            inverting = false;
            need_fix = true;
          }
        }
      }

#ifdef __HAS_LCD__
      lcd.setCursor(0,0);
      lcd.print(latitude);
      lcd.setCursor(0,1);
      lcd.print(longitude);
#endif /* __HAS_LCD__ */
    }
    state = WAITING;
    
    Serial.println(sentence);
  }
}

