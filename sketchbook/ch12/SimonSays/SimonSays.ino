#include "I2CLcd.h"

const uint8_t backlightPin = 7;
const uint8_t resetPin = A2;
I2CLcd lcd(resetPin,backlightPin);

#define LED_BUTTON_TABLE_SIZE  (4)
#define BUTTON_PRESS_INVALID  (0xff)
#define SEQUENCE_LENGTH_MAX  (30)
#define SEQUENCE_SPEED  (400) /* ms */
uint8_t sequence[SEQUENCE_LENGTH_MAX];
uint8_t game_round = 0;

const uint8_t led_button_table[LED_BUTTON_TABLE_SIZE] =
{
  4, 5, 6, 7
};

const uint8_t buzzer = 9;
const uint16_t button_sound[LED_BUTTON_TABLE_SIZE] =
{
  554, 659, 880, 1319 // C#5, E5, A5, E6
};

void led_set(uint8_t nr, uint8_t value)
{
  // Use table to map nr to output.
  pinMode(led_button_table[nr],OUTPUT);
  digitalWrite(led_button_table[nr],value);
}

bool button_read(uint8_t nr)
{
  // Buttons are pulled down externally.
  // Use table to map nr to input.
  pinMode(led_button_table[nr],INPUT);
  return digitalRead(led_button_table[nr]);
}

uint8_t button_read_all(void)
{
  uint8_t buttons = 0;
  for (int i=0; i<LED_BUTTON_TABLE_SIZE; i++)
  {
    if (button_read(i)==true) buttons |= (1<<i);
  }
  return buttons;
}

uint8_t button_read_all_debounced(void)
{
  uint8_t buttons = button_read_all();
  // Wait for any bouncing to stop.
  delay(10);
  // Read the buttons again.
  buttons &= button_read_all();
  // Only buttons that were read consistently as pressed will have a bit set. 
  return buttons;
}

bool button_pressed(void)
{
  return button_read_all_debounced()!=0;
}

uint8_t button_as_number(uint8_t buttons)
{
  uint8_t mask = 0x01;
  for (int i=0; i<8; i++)
  {
    if (buttons==mask) return i;
    mask <<= 1;
  }
  // If we get here we didn't find a valid button press.
  return BUTTON_PRESS_INVALID;
}

void sequence_create(uint8_t nr_of_leds)
{
  for (int i=0; i<SEQUENCE_LENGTH_MAX; i++)
  {
    //sequence[i] = rand() % nr_of_leds; // Poor method.
    // If nr_of_leds is much smaller than RAND_MAX it is better
    // to do this:
    sequence[i] = rand()/(RAND_MAX/nr_of_leds + 1); 
  }
}

void sequence_play(uint8_t len)
{
  for (int i=0; i<len; i++)
  {
    tone(buzzer,button_sound[sequence[i]],SEQUENCE_SPEED/2);
    led_set(sequence[i],true);
    delay(SEQUENCE_SPEED);
    led_set(sequence[i],false);
    delay(SEQUENCE_SPEED/4);
  }
}

bool sequence_read_buttons(uint8_t len)
{
  for (int i=0; i<len; i++)
  {
    uint8_t nr;
    uint8_t buttons;
    
    // Wait for a button press.
    do
    {
      buttons = button_read_all_debounced();
    }
    while (buttons==0);

    // Convert button press to button/led number.
    nr = button_as_number(buttons);
    
    // Check that we have a valid button press.
    if (nr!=BUTTON_PRESS_INVALID && nr<LED_BUTTON_TABLE_SIZE)
    {
      // Play sound & wait until it finishes.
      tone(buzzer,button_sound[nr],SEQUENCE_SPEED/2);
      delay(SEQUENCE_SPEED/2);
      // Wait until the player releases the button.
      while (button_pressed()==true);
      // Fail as soon as an incorrect button was detected.
      if (sequence[i]!=nr) return false;
    }
    else return false;
  }
  return true;
}

void display(char *p_str)
{
  lcd.clear();
  lcd.putString("Simon says:");
  lcd.cursor(1,0);
  lcd.putString(p_str);
}

void game_start(void)
{
  splash(100);
  display(" press a button");
  while (button_pressed()==false);
  srand(millis());
  sequence_create(LED_BUTTON_TABLE_SIZE);
  game_round = 1;
  display("    round 1");
  delay(500);
}

void splash(int speed)
{
  lcd.clear();
  lcd.putString("      Simon says");
  lcd.cursor(1,0);
  lcd.putString("Simon says      ");
  tone(buzzer,button_sound[0],speed);
  delay(speed);
  lcd.cursor(0,0);
  lcd.putString("    Simon says  ");
  lcd.cursor(1,0);
  lcd.putString("  Simon says    ");
  tone(buzzer,button_sound[1],speed);
  delay(speed);
  lcd.cursor(0,0);
  lcd.putString("  Simon says    ");
  lcd.cursor(1,0);
  lcd.putString("    Simon says  ");
  tone(buzzer,button_sound[2],speed);
  delay(speed);
  lcd.cursor(0,0);
  lcd.putString("Simon says      ");
  lcd.cursor(1,0);
  lcd.putString("      Simon says");
  tone(buzzer,button_sound[3],speed);
  delay(speed);
}

void setup(void)
{
  lcd.begin();
  lcd.backlight(false);
  game_start();
}

void loop(void)
{
  sequence_play(game_round);
  display("   your turn");
  if (sequence_read_buttons(game_round)==true)
  {
    if (game_round<SEQUENCE_LENGTH_MAX)
    {
      char str[5];
      // Next round.
      game_round++;
      display("    round ");
      lcd.putString(itoa(game_round,str,10));
      delay(1000);
    }
    else
    {
      // Player wins.
      splash(25);
      splash(50);
      splash(100);
      display("   Excellent!");
      delay(200);
      display("");
      delay(200);
      display("   Excellent!");
      delay(200);
      display("");
      delay(200);
      display("   Excellent!");
      delay(200);
      display("");
      delay(200);
      display("   Excellent!");
      delay(200);
      display("");
      delay(200);
      splash(100);
      splash(50);
      splash(25);
      display("   Excellent!");
      delay(1500);
      game_start();
    }
  }
  else
  {
    // Game over.
    display("   Game over!");
    tone(buzzer,110,400);
    delay(3000);
    game_start();
  }
}

