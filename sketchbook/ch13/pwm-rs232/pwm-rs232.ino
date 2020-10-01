void setup(void)
{
  // Prescaler = 1.
  bitSet(TCCR4B,CS40);
  bitClear(TCCR4B,CS41);
  bitClear(TCCR4B,CS42);
  // WGM mode 10: register ICR4 determines PWM frequency.
  bitClear(TCCR4A,WGM40);
  bitSet(TCCR4A,WGM41);
  bitClear(TCCR4B,WGM42);
  bitSet(TCCR4B,WGM43);
  // Set PWM frequency to 9600 Hz.
  // This is now the maximum value for analogWrite on pin 1.
  ICR4 = 0x3fff;
  // Set duty-cycle to 50%.
  analogWrite(1,0x1fff);
  analogWrite(0,127);
}

void loop(void)
{
}

