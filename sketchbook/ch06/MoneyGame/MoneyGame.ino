/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Money game 
 */

const int COLUMN_MAX = 4;
const int ROW_MAX = 4;

int rows[ROW_MAX] = { 2, 3, 4, 5 };
int columns_key[COLUMN_MAX] = { 6, 7, 8, 9 };
int columns_led[COLUMN_MAX] = { 10, 11, 12, 13 };
int keys_pressed[ROW_MAX];
int secret_code[ROW_MAX];
int seed = 0;

void led_write(int r, int c, int d)
{
  digitalWrite(rows[r],HIGH);
  digitalWrite(columns_led[c],LOW);
  delay(d);
  digitalWrite(rows[r],LOW);
  digitalWrite(columns_led[c],HIGH);
}

void led_animation(int repeat)
{
  rows_initialize(OUTPUT);
  for (int n=0; n<repeat; n++)
  {
    for (int c=0; c<COLUMN_MAX; c++)
    {
      for (int r=0; r<ROW_MAX; r++)
      {
        led_write(r,c,20);
      }
    }
  }
}

void rows_initialize(int direction)
{
  for (int i=0; i<ROW_MAX; i++)
  {
    pinMode(rows[i],direction);
    if (direction==OUTPUT)
    {
      digitalWrite(rows[i],LOW);
    }
  }
}

int key_read(int r, int c)
{
  digitalWrite(columns_key[c],LOW);
  int result = digitalRead(rows[r]);
  digitalWrite(columns_key[c],HIGH);
  return result;
}

void key_read_all(void)
{
  rows_initialize(INPUT_PULLUP);
  for (int r=0; r<ROW_MAX; r++)
  {
    keys_pressed[r] = -1;
    for (int c=0; c<COLUMN_MAX; c++)
    {
      if (key_read(r,c)==LOW)
      {
        keys_pressed[r] = c;
      }
    }
  }
}

void key_show_all(void)
{
  rows_initialize(OUTPUT);
  for (int i=0; i<ROW_MAX; i++)
  {
    if (keys_pressed[i]>-1) led_write(i,keys_pressed[i],1);
  }
}

boolean code_complete(void)
{
  for (int i=0; i<ROW_MAX; i++)
  {
    if (keys_pressed[i]==-1) return false;
  }
  return true;
}

boolean code_good(void)
{
  for (int i=0; i<ROW_MAX; i++)
  {
    if (secret_code[i]!=keys_pressed[i]) return false;
  }
  return true;
}

void code_show(int period)
{
  rows_initialize(OUTPUT);
  unsigned long int t = millis();
  do
  {
    for (int i=0; i<ROW_MAX; i++)
    {
      led_write(i,secret_code[i],1);
    }
  } 
  while (millis()-t<period);
}

void code_generate(int s)
{
  randomSeed(s);
  for (int i=0; i<ROW_MAX; i++)
  {
    secret_code[i] = random(COLUMN_MAX);
  }
}

void setup(void)
{
  rows_initialize(INPUT_PULLUP);
  
  for (int i=0; i<COLUMN_MAX; i++)
  {
    pinMode(columns_key[i],OUTPUT);
    digitalWrite(columns_key[i],HIGH);
    pinMode(columns_led[i],OUTPUT);
    digitalWrite(columns_led[i],HIGH);
  }
  
  led_animation(1);
}

void loop(void)
{
  seed += 1;

  key_read_all();
  key_show_all();

  if (code_complete()==true)
  {
    code_generate(seed);
    if (code_good()==false) led_animation(1);
    else led_animation(7);
    code_show(5000);
    led_animation(1);
  }
}
