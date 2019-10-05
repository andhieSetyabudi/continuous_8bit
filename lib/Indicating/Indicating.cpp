#include "Config.h"
#include "Indicating.h"

bool Indicating::sleep = false;
bool Indicating::sleepBuffer = false;

void Indicating::setup(void) {
  Wire.begin();
  Translation::setup();
  Display::setup();
  Indicating::Display::splash(F(""));
  RTC::setup();
}

void Indicating::loop(void) {
  wdt_reset();
  RTC::setSleep(sleepBuffer);
  Display::setSleep(sleepBuffer);
  sleep = RTC::isSleep() &
          Display::isSleep();

  RTC::loop();
  Display::loop();
}

bool Indicating::isSleep(void) {
  return sleep;
}

bool Indicating::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void Indicating::setSleep(bool sleep) {
  sleepBuffer = sleep;
}
