
/*************************************************************
Title         :   Home automation using blynk
Description   :   To control light's brigntness with brightness,monitor temperature , monitor water level in the tank through blynk app
Pheripherals  :   Arduino UNO , Temperature system, LED, LDR module, Serial Tank, Blynk cloud, Blynk App.
 *************************************************************/

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BLYNK_TEMPLATE_ID "XXXXXXXXXXXXX"
#define BLYNK_TEMPLATE_NAME "XXXXXXXXX"
#define BLYNK_AUTH_TOKEN "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

#define BLYNK_PRINT Serial


#include "main.h"
#include "temperature_system.h"
#include "ldr.h"
#include "serial_tank.h"

bool heater_sw, cooler_sw;

bool inlet_sw,outlet_sw;
unsigned int tank_volume;
String temp;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// This function is called every time the Virtual Pin 0 state changes
/*To turn ON and OFF cooler based virtual PIN value*/
BLYNK_WRITE(COOLER_V_PIN)
{
  int val = param.asInt();
  if(val){
    cooler_control(ON);
    lcd.setCursor(7, 0);
    lcd.print("CO_LR ON ");
  }
  else{
    cooler_control(OFF);
    lcd.setCursor(7, 0);
    lcd.print("CO_LR OFF");
  }
  
}
/*To turn ON and OFF heater based virtual PIN value*/
BLYNK_WRITE(HEATER_V_PIN)
{
  heater_sw = param.asInt();
  if(heater_sw){
    heater_control(ON);
    lcd.setCursor(7, 0);
    lcd.print("HT_R ON  ");
  }
  else{
    heater_control(OFF);
    lcd.setCursor(7, 0);
    lcd.print("HT_R OFF ");
  }
  
}
/*To turn ON and OFF inlet vale based virtual PIN value*/
BLYNK_WRITE(INLET_V_PIN)
{
  inlet_sw = param.asInt();
  
  if(inlet_sw){
    enable_inlet();
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_ON ");
  }  
  else{
    disable_inlet();
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_OFF");
  }
  
}
/*To turn ON and OFF outlet value based virtual switch value*/
BLYNK_WRITE(OUTLET_V_PIN)
{
  outlet_sw = param.asInt();
  
  if(outlet_sw){
    enable_outlet();
    lcd.setCursor(7, 1);
    lcd.print("OT_FL_ON ");
  }
  else{
    disable_outlet();
    lcd.setCursor(7, 1);
    lcd.print("OT_FL_OFF");
  }
  
}
/* To display temperature and water volume as gauge on the Blynk App*/  
void update_temperature_reading()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(TEMPERATURE_GAUGE, read_temperature());
  Blynk.virtualWrite(WATER_VOL_GAUGE, volume());

  /*
  lcd.setCursor(2, 1);
  lcd.print(volume());
  lcd.setCursor(2, 0);
  lcd.print(read_temperature());
  */
}

/*To turn off the heater if the temperature raises above 35 deg C*/
void handle_temp(void)
{

  if(read_temperature() > float(35) && heater_sw){

    heater_sw = 0;
    heater_control(OFF);
    lcd.setCursor(7, 0);
    lcd.print("HT_R OFF ");

    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Temperature is more than 35 degrees C\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning OFF the heater\n");
    Blynk.virtualWrite(HEATER_V_PIN, OFF);
  }
  
}

/*To control water volume above 2000ltrs*/
void handle_tank(void)
{
  if(tank_volume < 2000 && inlet_sw == OFF){
    enable_inlet();
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_ON ");
    inlet_sw = ON;

    Blynk.virtualWrite(INLET_V_PIN, ON);

    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water volume is less than 2000\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning ON the inlet value\n");
  }

  if(tank_volume == 3000 && inlet_sw == ON){
    disable_inlet();
    lcd.setCursor(7, 1);
    lcd.print("IN_FL_OFF");

    inlet_sw = OFF;
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water volume is Full\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning OFF the inlet valve\n");
    Blynk.virtualWrite(INLET_V_PIN, OFF);

  }

}


void setup(void)
{
  
  
  /*Initialize the LCD*/
  lcd.init();
  /*turn on the backlight*/
  lcd.backlight();
  /*clear the lcd screen*/
  lcd.clear();
  /*set the cuesor to the initial position*/
  lcd.home();

  init_ldr();


  lcd.setCursor(0, 0);
  lcd.print("T=");

  lcd.setCursor(0, 1);
  lcd.print("V=");

  init_temperature_system();
  init_serial_tank();

  timer.setInterval(500L, update_temperature_reading);
  Blynk.begin(BLYNK_AUTH_TOKEN);


}

void loop(void) 
{
  
  brightness_control();

  lcd.setCursor(2, 0);

  temp = read_temperature();
  lcd.print (temp);

  lcd.setCursor(2, 1);
  tank_volume = volume();
  lcd.print(tank_volume);

  handle_temp();
  handle_tank();
  
  Blynk.run();
  timer.run(); 
   
}
