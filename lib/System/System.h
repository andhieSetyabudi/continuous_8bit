#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <avr/sleep.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Config.h"


class System {
 private:
  static bool sleep;
  static bool sleepBuffer;

 public:
  static void setup(void);
  static void loop(void);

  static void initPin(void);

  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);

  class Debugging;
#ifdef SOFT_POWER_BUTTON
  class SoftPower;
#endif
  class Identity;
  class Sleep;
};

class System::Debugging {
 private:
  static bool debug;

 public:
  static void print(const char fmt[], ...);
  static void println(const char fmt[], ...);

  static void setup(void);
  static void loop(void);
};

#ifdef SOFT_POWER_BUTTON
class System::SoftPower {
 private:
  static bool pressed;
  static bool longPressed;
  static bool interruptPressed;
  static bool onMain;

 public:
  static void setup(void);
  static void loop(boolean softRun = true);

  static void onButtonPressed(void);
  static bool isPressed(void);
  static bool isLongPressed(void) { return longPressed; };

  static void powerOn(void);
  static void powerOff(void);
};
#endif

class System::Identity {
 private:
  static char id[9];
  static char idBuffer[9];

 public:
  static void setup(void);
  static void loop(void);

  static const char *getId(void) { return id; };
  static void setId(const char id[]);
};

class System::Sleep {
 private:
  static bool sleep;
  static bool sleepBuffer;

 public:
  static void setup(void);
  static void loop(void);

  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);
};

#endif
