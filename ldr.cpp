#include "ldr.h"
#include "Arduino.h"
#include "main.h"

void init_ldr(void)
{
   pinMode(GARDEN_LIGHT, OUTPUT);
   
}

unsigned int ldr_val = 0;
void brightness_control(void)
{
  ldr_val = analogRead(LDR_SENSOR);
  /*Mapping 0 to 1023 -> 255 to 0*/
  /*Scaling down 0 to 1023 -> 255 to 0 */
  ldr_val = (1023 - ldr_val) / 4;

  /*to assign duty cycle to LED based LDR val*/
  analogWrite(GARDEN_LIGHT, ldr_val);

  delay(100);

}
