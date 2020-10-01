/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * rotary encoder
 */

int pin_gnd = 8;
int pin_vcc = 9;
int pin_sw = 10;
int pin_a = 11;
int pin_b = 12;
int state = 0;
int state_sub = 0;
volatile int counter = 0;
volatile boolean flag = false;

void encoder_read(void)
{
  uint8_t pin_states = PINB & PCMSK0;

  // Push button press.
  if ((pin_states&0x04)==0)
  {
    counter = 0;
    flag = true;
  }
  
  pin_states = (pin_states>>3)&0x03;
    
  // state is now 0, 1, 2 or 3.
    
  if (pin_states!=state)
  {
    // Exor the old & new states to determine the rotation direction.
    int inc = ((pin_states>>1)^state)&0x01;
    if (inc==0) inc = -1;
    state = pin_states;

    // Reset on change of direction.
    if ((inc<0 && state_sub>0) || (inc>0 && state_sub<0)) state_sub = 0;

    state_sub += inc;
    if (state_sub<=-4 || state_sub>=4)
    {
      state_sub -= (inc<<2);
      counter += inc;
      flag = true;
    }
  }
}

ISR(TIMER2_COMPA_vect)
{
  bitClear(TIMSK2,OCIE2A); // Disable timer2 interrupts.
  encoder_read();
  bitSet(PCIFR,PCIF0); // Clear pending interrupts.
  bitSet(PCICR,PCIE0); // Enable PCINT0-7.
}

ISR(PCINT0_vect)
{
  bitClear(PCICR,PCIE0); // Disable PCINT0-7.
  bitSet(TCCR2A,WGM21); // CTC mode.
  bitSet(TCCR2B,CS20);
  bitSet(TCCR2B,CS22); // Clk div by 128.
  OCR2A = 250; // 2 ms.
  TCNT2 = 0; // Clear counter.
  bitSet(TIFR2,OCF2A);
  bitSet(TIMSK2,OCIE2A);
}

void setup(void)
{
  Serial.begin(115200);
  pinMode(pin_gnd,OUTPUT);
  digitalWrite(pin_gnd,LOW);
  pinMode(pin_vcc,OUTPUT);
  digitalWrite(pin_vcc,HIGH);
  pinMode(pin_sw,INPUT_PULLUP);
  pinMode(pin_a,INPUT);
  pinMode(pin_b,INPUT);
  state = 0;
  state_sub = 0;
  counter = 0;
  flag = false;
  PCMSK0 = 0x1c; 
  bitSet(PCIFR,PCIF0); // Clear pending interrupts.
  bitSet(PCICR,PCIE0); // Enable PCINT0-7.
}

void loop(void)
{
  if (flag==true)
  {
    Serial.println(counter);
    flag = false;
  }
}
