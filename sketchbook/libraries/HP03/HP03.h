/*
 * Arduino driver for HP03 pressure sensor.
 *
 * Belongs to:
 * "Mastering Microcontrollers Helped by Arduino"
 * ISBN 978-1-907920-23-3 (English)
 * ISBN 978-2-86661-190-3 (French)
 * http://www.polyvalens.com/
 *
 * For use with PolyValens Multipurpose Shield 129009-1
 *
 * Copyright (c) 2014, Clemens Valens
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 * BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#ifndef __HP03_H__
#define __HP03_H__

#include "Arduino.h"


class HP03
{
public:
  HP03(void) {}
  HP03(uint8_t mclk, uint8_t xclr) { begin(mclk,xclr); }

  void begin(uint8_t mclk, uint8_t xclr);
  void update(void);

  float get_temperature(void) { return _temperature/10.0; }
  float get_pressure(void) { return _pressure/10.0; } 

  uint8_t read_eeprom(void);

  void test(void);

private:
  uint32_t _pressure;
  uint32_t _temperature;
  int32_t _C[8];
  uint8_t _AA;
  uint8_t _BB;
  uint8_t _CC;
  uint8_t _DD;
  uint8_t _mclk;
  uint8_t _xclr;

  uint16_t read_value(uint8_t sensor);
  void read(void);
  void calculate(void);
};


#endif /* __HP03_H__ */


