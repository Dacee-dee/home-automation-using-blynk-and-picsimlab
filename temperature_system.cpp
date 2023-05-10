#include "temperature_system.h"
#include "Arduino.h"
#include "main.h"


void init_temperature_system(void)
{
  pinMode(HEATER, OUTPUT);
  pinMode(COOLER, OUTPUT);

  digitalWrite(HEATER, LOW);
  digitalWrite(COOLER, LOW);

  pinMode(TEMPERATURE_SENSOR, INPUT);   
    
}

float read_temperature(void)
{
   float temp;

   temp = ((analogRead(TEMPERATURE_SENSOR) * (float)5/1024) / (float)0.01);
   return temp;
}

void cooler_control(bool control)
{
    digitalWrite(COOLER, control);
}
void heater_control(bool control)
{
  digitalWrite(HEATER, control);
}
