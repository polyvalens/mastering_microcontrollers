/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Thermometer
 *
 * - ATmega48PA/88PA/168PA/328P only. 
 * - Needs modification of wiring_analog.c to allow use
 *   of ADC channel 8.
 * - Modified standard\pins_arduino.h to add A8 (=22)
 */
  
void setup(void)
{
  Serial.begin(115200);
  analogReference(INTERNAL);
}

void loop(void)
{
  Serial.println(analogRead(A8));
  delay(1000);
}

