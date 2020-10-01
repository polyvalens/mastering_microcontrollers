/*
 * IR Transmitter, two channel
 * Uses Output Compare Module (OCM), ATmega328PB only.
 * 
 * Pin 3: uses the tone-base technique as explained in Section 9.15
 * Pin 2: OCM method as explained in Section 13.6.6
 */
 
#define CARRIER  38000
#define RC_CODE  0x6170807f

const int ir_led2 = 2;
const uint16_t pulse = 1125; // 2*562.5
uint32_t tx_buffer = 0;
uint32_t tx_index = 0;

int ir_led1 = 3;
uint16_t startbit = 9000;
uint16_t on_time_0 = 560;
uint16_t off_time_0 = 565;
uint16_t on_time_1 = 560;
uint16_t off_time_1 = 1690;
extern volatile long timer2_toggle_count;

void ir_send_pulse(uint8_t pin, uint32_t frequency, uint32_t duration)
{
  // Overflow bug in Arduino, we have to calculate our own toggle_count.
  // A fudge factor is needed to correct for storing the value
  // in timer2_toggle_count. Carrier frequency dependent.
  unsigned long toggle_count = 2*frequency*duration/1000000 - 8; // 6 for 36 kHz;
  //TCNT2 = 0; // Put this in tone.cpp right before writing OCR2A
  tone(pin,frequency,duration);
  timer2_toggle_count = toggle_count;
}

void ir_send_bit(uint8_t pin, uint16_t frequency, uint16_t on_time, uint16_t off_time)
{
  ir_send_pulse(pin,frequency,on_time);
  // Wait for the burst to end.
  // OCR2A is cleared in disableTimer at the end of the burst.
  while (OCR2A!=0); 
  // About 104 us are needed to compensate for the delay between
  // the call of ir_send_pulse and the start of the burst.
  delayMicroseconds(off_time-104);
}

void ir_send_code1(uint8_t pin, uint16_t frequency, uint32_t code)
{
  ir_send_bit(pin,frequency,startbit,startbit/2);
  for (uint32_t mask=0x80000000; mask!=0; mask>>=1)
  {
    if ((code&mask)==0)
    {
      ir_send_bit(pin,frequency,on_time_0,off_time_0);
    }
    else
    {
      ir_send_bit(pin,frequency,on_time_1,off_time_1);
    }
  }
  ir_send_bit(pin,frequency,on_time_1,off_time_1);
}

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

void ir_send_code2(uint32_t code)
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
  pinMode(ir_led2,OUTPUT); // Enable output.
  digitalWrite(ir_led2,LOW); // OCM in AND mode.
  // Timer3 mode 4 (CTC), toggle output on match.
  TCCR3A = (1<<COM3B0);
  TCCR3B = (1<<WGM32);
  OCR3A = 16000000/CARRIER/2; // Toggle at 76 kHz -> 38 kHz.
  OCR3B = OCR3A>>1; // 50% duty-cycle.
  // Timer4 mode 15 (Fast PWM), clear output on match.
  TCCR4A = (1<<WGM41) | (1<<WGM40) | (1<<COM4B1);
  TCCR4B = (1<<WGM43) | (1<<WGM42);
}

void loop(void)
{
  // Stop timer0 during transmission.
  bitClear(TIMSK0,TOIE0);
  ir_send_code2(RC_CODE);
  ir_send_code1(ir_led1,CARRIER,RC_CODE);
  bitSet(TIMSK0, TOIE0);
  delay(1000);
}

