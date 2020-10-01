/*
 * Export IR pulses to SVG file.
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

//#define __EMBED_IN_HTML__

#define X_SCALE  25

// TSOP34836 connected to pin 8.
int led = 13;
int ir_signal = 8;
unsigned long time_start;
int x_offset = 10 * X_SCALE;

typedef struct
{
  unsigned long timestamp;
  unsigned long pulse;
}
point_t;

#define POINTS_MAX  50
point_t points[POINTS_MAX];
int index;

void svg_write_file(int x_offset, int x_scale, int y_min, int y_max)
{
#ifdef __EMBED_IN_HTML__
  Serial.println("<!DOCTYPE html><html><body>");
#endif /* __EMBED_IN_HTML__ */

  Serial.print("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">");
  Serial.print("<polyline points=\"0,");
  Serial.print(y_max);
  Serial.println(' ');
  for (int i=0; i<index; i++)
  {
    Serial.print((points[i].timestamp-points[i].pulse)/x_scale);
    Serial.print(',');
    Serial.print(y_max);
    Serial.print(' ');
    Serial.print((points[i].timestamp-points[i].pulse)/x_scale);
    Serial.print(',');
    Serial.print(y_min);
    Serial.print(' ');
    Serial.print(points[i].timestamp/x_scale);
    Serial.print(',');
    Serial.print(y_min);
    Serial.print(' ');
    Serial.print(points[i].timestamp/x_scale);
    Serial.print(',');
    Serial.println(y_max);
  }
  Serial.println("\" style=\"fill:white;stroke:black;stroke-width:1\"/></svg>");
#ifdef __EMBED_IN_HTML__
  Serial.println("</body></html>");
#endif /* __EMBED_IN_HTML__ */
}

void setup(void)
{
  Serial.begin(115200);
  pinMode(led,OUTPUT);
  digitalWrite(led,LOW);
  pinMode(ir_signal,INPUT);

  // Ignore spurious pulses at startup.
  while (pulseIn(ir_signal,LOW,500000)!=0);
  digitalWrite(led,HIGH); // start receiving
  time_start = 0;
  index = 0;
}

void loop(void)
{
  unsigned long pulse = pulseIn(ir_signal,LOW,2000000);
  if (pulse!=0 && index<POINTS_MAX)
  {
    unsigned long time_now = micros();
    if (time_start==0)
    {
      time_start = time_now - pulse - x_offset;
    }
    points[index].timestamp = time_now - time_start;
    points[index].pulse = pulse;
    index += 1;
  }
  else
  {
    svg_write_file(x_offset,X_SCALE,150,50);
    digitalWrite(led,LOW); // done
    while (1);
  }
}
