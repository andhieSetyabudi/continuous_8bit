#define LIBCALL_ENABLEINTERRUPT

#include "Config.h"
#include "Indicating.h"
#include "System.h"
#include "lang.h"


#include <EnableInterrupt.h>

void System::setup(void) {
  initPin();
  char firmware[12];
  char buffer[18];
  strcpy_P(firmware, (const char *)F(VERSION));
  sprintf(buffer, Indicating::Translation::get(CONTINOUS_VERSION), firmware);
  Indicating::Display::splash(buffer);
  delay(100);
  for(uint8_t i = 0; i < 5; i++)
  {
    strcat_P(buffer,(const char*)F("."));
    Indicating::Display::splash(buffer);
    delay(75);
  }
  
#ifdef SOFT_POWER_BUTTON
  SoftPower::setup();
#endif
  Debugging::setup();
  Identity::setup();

  return;
}

void System::loop(void) {
  wdt_reset();
#ifdef SOFT_POWER_BUTTON
  SoftPower::loop();
#endif
  Debugging::loop();
  Identity::loop();
  Sleep::loop();
}

void System::initPin(void) {
  pinMode(GSM_POWER_PIN, OUTPUT);
  digitalWrite(GSM_POWER_PIN, HIGH);
  pinMode(GSM_REGULATOR_PIN, OUTPUT);
  digitalWrite(GSM_REGULATOR_PIN, HIGH);
  //GSM_SERIAL.begin(GSM_SERIAL_BAUDRATE);
#if GSM_SERIAL_TYPE == SOFTWARE_SERIAL
  pinMode(GSM_SERIAL_RX_PIN, INPUT_PULLUP);
  pinMode(GSM_SERIAL_TX_PIN, OUTPUT);
  digitalWrite(GSM_SERIAL_TX_PIN, HIGH);
#endif
#if PH_SENSOR_SERIAL_ENABLE
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_ANALOG
    pinMode(PH_SENSOR_ADC_PIN, INPUT);
  #endif 
#endif


#ifdef SOFT_POWER_BUTTON
  pinMode(SOFT_POWER_BUTTON, INPUT_PULLUP);
  digitalWrite(SOFT_POWER_BUTTON,HIGH);

  pinMode(FORCE_SHUTDOWN_PIN,OUTPUT);
  digitalWrite(FORCE_SHUTDOWN_PIN,HIGH);


 // pinMode(SOFT_POWER_CONTROL, OUTPUT);
 // digitalWrite(SOFT_POWER_CONTROL, LOW);
 // delay(10);
 // digitalWrite(SOFT_POWER_CONTROL, HIGH);
  delay(100);
#endif

//Wire.begin();
//Indicating::Display::clearDisplay();
//Wire.setClock(400000);

}
