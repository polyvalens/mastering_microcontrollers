/*
 * IR Transmitter
 * Uses Output Compare Module (OCM), ATmega328PB only.
 */
 
#define CARRIER  38000
#define RC_CODE  0x6170807f

const int ir_led = 2;
const uint16_t pulse = 1125; // 2*562.5
uint32_t tx_buffer = 0;
uint32_t tx_index = 0;

#define TIMER3_ON  TCCR3B |= (1<<CS30) /* prescaler 1 */
#define TIMER3_OFF  TCCR3B &= ~(1<<CS30)
#define TIMER4_ON  TCCR4B |= (1<<CS41) /* prescaler 8 */
#define TIMER4_OFF  TCCR4B &= ~(1<<CS41)

ISR(TIMER4_COMPB_vect)
{
  if (tx_index!=0)
  {
    OCR4B = pulse; // Set pulse length.
    if ((tx_buffer&tx_index)!=0)
    {
      OCR4A = OCR4B<<2; // Set '1' period.
    }
    else 
    {
      OCR4A = OCR4B<<1; // Set '0' period.
    }
    tx_index >>= 1;
  }
  else
  {
    if (OCR4A>=2*OCR4B)
    {
      OCR4A = OCR4B + 10; // Send closing burst.
    }
    else
    {
      // Done, clean up.
      TIMSK4 &= ~(1<<OCIE4B); // Disable interrupts.
      TIMER3_OFF;
      TIMER4_OFF;
    }
  }
}

void ir_send_code(uint32_t code)
{
  // Copy code to TX buffer.
  tx_buffer = code;
  tx_index = 0x80000000; // 32 bits.
  // Setup timer4 for preamble.
  OCR4A = 2*12*pulse;
  OCR4B = 2*8*pulse;
  TIFR4 = (1<<OCF4B); // Clear pending interrupts.
  TIMSK4 = (1<<OCIE4B); // Enable compare interrupt.
  // Go!
  TIMER3_ON;
  TIMER4_ON;
}

void setup(void)
{
  pinMode(ir_led,OUTPUT); // Enable output.
  digitalWrite(ir_led,LOW); // OCM in AND mode.
  // Timer3 mode 4 (CTC), toggle output on match.
  TCCR3A = (1<<COM3B0);
  TCCR3B = (1<<WGM32);
  OCR3A = 16000000/CARRIER/2; // Toggle at 76 kHz -> 38 kHz.
  OCR3B = OCR3A>>1; // 50% duty-cycle.
  // Timer4 mode 15 (Fast PWM), clear output on match.
  TCCR4A = (1<<WGM41) | (1<<WGM40) | (1<<COM4B1);
  TCCR4B = (1<<WGM43) | (1<<WGM42);
  ir_send_code(RC_CODE);
}

void loop(void)
{
}

