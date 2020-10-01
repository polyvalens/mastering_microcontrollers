/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * interrupt 6
 */

uint8_t pcint0_states_prev;
String result;

ISR(PCINT0_vect)
{
  uint8_t pin_states = PINB;
  uint8_t pcint = (pin_states ^ pcint0_states_prev) & PCMSK0;
  pcint0_states_prev = pin_states;
  if (pcint&0x01) result += '0';
  if (pcint&0x02) result += '1';
  if (pcint&0x04) result += '2';
  if (pcint&0x08) result += '3';
  if (pcint&0x10) result += '4';
  if (pcint&0x20) result += '5';
  result += '-';
}

void setup(void)
{
  Serial.begin(115200);
  pinMode(8,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  pinMode(10,INPUT_PULLUP);
  pinMode(11,INPUT_PULLUP);
  pinMode(12,INPUT_PULLUP);
  pinMode(13,INPUT_PULLUP);
  pcint0_states_prev = PINB;
  PCMSK0 = 0x3f; // Enable PCINT pins.
  bitSet(PCIFR,PCIF0); // Clear pending interrupts.
  bitSet(PCICR,PCIE0); // Enable PCINT0-7.
  result = "";
}

void loop(void)
{
  if (result.length()!=0)
  {
    Serial.println(result);
    result = "";
  }
}
