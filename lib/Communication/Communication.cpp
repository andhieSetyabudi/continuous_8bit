#include "Communication.h"
#include "Indicating.h"

bool Communication::sleep = false;
bool Communication::sleepBuffer = false;
bool Communication::setup_run = false;
bool Communication::isSleep(void) {
  return sleep;
}

bool Communication::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void Communication::setSleep(bool sleep) {
  sleepBuffer = sleep; 
}

void Communication::setup(void) {
  Indicating::Display::splash(F("Init GSM"));

  Cellular::setup();
}

void Communication::loop(void) {
  wdt_reset();
  Cellular::setSleep(sleepBuffer);
  sleep = Cellular::isSleep();
  if(sleepBuffer)
    return;
  else
    Cellular::loop();
}
