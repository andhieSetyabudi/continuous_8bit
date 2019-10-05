#include "Config.h"
#include "Indicating.h"
#include "UserInput.h"

bool UserInput::sleep = false;
bool UserInput::sleepBuffer = false;

void UserInput::setup(void) {
  Indicating::Display::splash(F("Init Input"));
  Button::setup();
}

void UserInput::loop(void) {
  wdt_reset();
  Button::setSleep(sleepBuffer);
  sleep = Button::isSleep();
  Button::loop();
}

bool UserInput::isSleep(void) {
  return sleep;
}

bool UserInput::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void UserInput::setSleep(bool sleep) {
  sleepBuffer = sleep;
}
