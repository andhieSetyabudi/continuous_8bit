#include "Indicating.h"
#include "Sensor.h"

bool Sensor::sleep = false;
bool Sensor::sleepBuffer = false;

void Sensor::setup(void) {
  Indicating::Display::splash(F("Init Sensor"));

  
#if DHT_USED
  Ambient::setup();
#endif
  Battery::setup();
  Water::setup();
}

void Sensor::loop(void) {
  wdt_reset();
  //Water::setSleep(sleepBuffer);
#if DHT_USED
  Ambient::setSleep(sleepBuffer);
#endif
  Battery::setSleep(sleepBuffer);
  sleep = Water::isSleep();
#if DHT_USED
  sleep &= Ambient::isSleep();
#endif
  sleep &= Battery::isSleep();

  
#if DHT_USED
  Ambient::loop();
#endif
  static unsigned long updateBatteryTime = millis();
  if(millis() - updateBatteryTime >= 1000)
  {
    Battery::loop();
    updateBatteryTime = millis();
  };
  Water::loop();
}


bool Sensor::isSleep(void) {
  return sleep;
}

bool Sensor::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void Sensor::setSleep(bool sleep) {
  sleepBuffer = sleep;
}
