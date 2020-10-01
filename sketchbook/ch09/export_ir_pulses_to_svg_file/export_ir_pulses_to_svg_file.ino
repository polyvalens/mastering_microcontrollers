/*
 * Author: Clemens Valens (www.polyvalens.com)
 * Book: Mastering Microcontrollers Helped By Arduino
 * WWW: www.elektor.com/arduinobook
 * ISBN: 978-1-907920-23-3
 *
 * Export IR pulses to SVG file.
 */

//#define __EMBED_IN_HTML__

#define X_SCALE  25

int led = 13;
int ir_signal = 12; // was 8, 12 is good for TSOP1738
int ir_vcc = 10;
int ir_gnd = 9;
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
  pinMode(ir_gnd,OUTPUT);
  digitalWrite(ir_gnd,LOW);
  pinMode(ir_vcc,OUTPUT);
  digitalWrite(ir_vcc,HIGH);

  // Ignore spurious pulses at startup.
  delay(500);
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
