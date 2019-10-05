#include "Config.h"
#include "Indicating.h"
#include "Sensor.h"

bool Sensor::Battery::sleep = false;
bool Sensor::Battery::sleepBuffer = false;

bool Sensor::Battery::lowStatus = false;
bool Sensor::Battery::lowEvent = false;

unsigned int Sensor::Battery::voltage;

void Sensor::Battery::setup(void) {
  pinMode(BATTERY_ADC_PIN, INPUT);
  pinMode(CHG_CE,OUTPUT); digitalWrite(CHG_CE,LOW);
  pinMode(CHARGER_STATUS_PIN, INPUT_PULLUP); digitalWrite(CHARGER_STATUS_PIN, HIGH);
  loop();
}

void Sensor::Battery::loop(void) 
{

  float volts = 0 ;
  volts  = Indicating::Display::getBatteryVolt();
  voltage = round ( volts * 100 );
  lowEvent = false;
  if ( getPercentage() <= min_percentage_power && !lowStatus) {
    lowStatus = true;
    lowEvent = true;
  } else if ( getPercentage() > min_percentage_power && lowStatus) {
    lowStatus = false;
  }
  Indicating::Display::BMS_control();

  if (sleepBuffer != sleep) {
    sleep = sleepBuffer;
  }
  #if BATTERY_DEBUG
    SYSTEM_SERIAL.print(F(" Battery Voltage    : ")); SYSTEM_SERIAL.print(Indicating::Display::getBatteryVolt(),3);
    SYSTEM_SERIAL.print(F(" Battery Percentage : ")); SYSTEM_SERIAL.print(getPercentage());
    SYSTEM_SERIAL.print(F(" Charge status      : ")); SYSTEM_SERIAL.println(digitalRead(CHARGER_STATUS_PIN)?"not Connected":" Connected");
    if(Indicating::Display::isCharging())
      SYSTEM_SERIAL.println("V bus : "+String(Indicating::Display::getSourceVoltage()));
    SYSTEM_SERIAL.flush();
  #endif
  if (sleep) return;
}

uint8_t Sensor::Battery::getPercentage(void) {
  uint8_t percentage_ = 25;
  if( voltage < min_power ) 
     percentage_=0;
  else
    percentage_ =round( float(voltage - min_power)  * 100 / float(max_power - min_power) ); 
    percentage_ = ( percentage_ > 100 ) ? 100 : ( percentage_ < 0 ) ? 0 : percentage_;

  return percentage_;
}

bool Sensor::Battery::isSleep(void) {
  return sleep;
}

bool Sensor::Battery::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void Sensor::Battery::setSleep(bool sleep) {
  sleepBuffer = sleep;
}
