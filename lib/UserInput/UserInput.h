#ifndef USER_INPUT_H_
#define USER_INPUT_H_

#include <Arduino.h>
//#include <MFRC522.h>
//#include <RFIDReader.h>

#include "Indicating.h"

class UserInput {
 private:
  static bool sleep;
  static bool sleepBuffer;

 public:
  static void setup(void);
  static void loop(void);

  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);
  class Button;
};

class UserInput::Button {
 private:
  static bool sleep;
  static bool sleepBuffer;

  static volatile bool interruptPressed;
  static bool pressed;
  static bool longPressed;

  static volatile bool interruptCalibrationPressed;
  #if CALIBRATION_BUTTON_ENABLE == true
  static bool calibrationPressed;
  static bool calibrationLongPressed;
#endif

 public:
  static void setup(void);
  static void loop(void);

  static void onButtonPressed(void);
#if CALIBRATION_BUTTON_ENABLE == true
  static void onCalibrationButtonPressed(void);
#endif

  static bool isPressed(void) { return pressed; };
  static bool isLongPressed(void) {return longPressed; };
#if CALIBRATION_BUTTON_ENABLE == true
  static bool isCalibrationPressed(void) {  return calibrationPressed; };
  static bool isCalibrationLongPressed(void) {  return calibrationLongPressed; };
#endif

  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);
};

#endif
