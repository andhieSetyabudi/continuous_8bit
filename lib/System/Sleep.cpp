#include "System.h"

bool System::Sleep::sleep = false;
bool System::Sleep::sleepBuffer;

void System::Sleep::setup(void) {
}

void System::Sleep::loop(void) {
  if (sleepBuffer != sleep) {
    if (sleep) {
      /* code */
    } else {
      sleep = sleepBuffer;
      return;
    }
  }
  if (!sleep) return;

  //set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  //sleep_enable();
  //sleep_mode();
  //sleep_disable();
}

bool System::Sleep::isSleep(void) {
  return sleep;
}

bool System::Sleep::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void System::Sleep::setSleep(bool sleep) {
  sleepBuffer = sleep;
}

bool System::sleep = false;
bool System::sleepBuffer;

bool System::isSleep(void) {
  return sleep;
}

bool System::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void System::setSleep(bool sleep) {
  sleepBuffer = sleep;
}
