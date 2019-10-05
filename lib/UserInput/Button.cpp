#define LIBCALL_ENABLEINTERRUPT

#include "Config.h"
#include "UserInput.h"
#include <EnableInterrupt.h>

bool UserInput::Button::sleep = false;
bool UserInput::Button::sleepBuffer = false;

volatile bool UserInput::Button::interruptPressed = false;
bool UserInput::Button::pressed = false;
bool UserInput::Button::longPressed = false;

volatile bool UserInput::Button::interruptCalibrationPressed = false;
#if CALIBRATION_BUTTON_ENABLE == true
bool UserInput::Button::calibrationPressed = false;
bool UserInput::Button::calibrationLongPressed = false;
#endif

void UserInput::Button::setup(void) {
  unsigned char port = BUTTON_PIN_PORT;
  unsigned char bit = BUTTON_PIN_BIT;
#if CALIBRATION_BUTTON_ENABLE == true
  unsigned char calibrationPort = CALIBRATION_BUTTON_PIN_PORT;
  unsigned char calibrationBit = CALIBRATION_BUTTON_PIN_BIT;
#endif

  *portModeRegister(port) &= ~bit;
  *portOutputRegister(port) |= bit;
#if CALIBRATION_BUTTON_ENABLE == true
  *portModeRegister(calibrationPort) &= ~calibrationBit;
  *portOutputRegister(calibrationPort) |= calibrationBit;
#endif

  enableInterrupt(BUTTON_PIN, UserInput::Button::onButtonPressed, FALLING);
#if CALIBRATION_BUTTON_ENABLE == true
  enableInterrupt(CALIBRATION_BUTTON_PIN, UserInput::Button::onCalibrationButtonPressed, FALLING);
#endif
}

void UserInput::Button::loop(void) {
  wdt_reset();
  static bool interruptEnabled = false;
  static bool interruptCalibrationEnabled = false;
  if (interruptPressed ||  interruptCalibrationPressed) {
    sleep = false;
  } else if (sleepBuffer != sleep) {
    if (!interruptEnabled) {
      enableInterrupt(BUTTON_PIN, UserInput::Button::onButtonPressed, FALLING);
      interruptEnabled = true;
    }
#if CALIBRATION_BUTTON_ENABLE == true
    if (!interruptCalibrationEnabled) {
      enableInterrupt(CALIBRATION_BUTTON_PIN, UserInput::Button::onCalibrationButtonPressed, FALLING);
      interruptCalibrationEnabled = true;
    }
#endif
    sleep = sleepBuffer;
  }

  static unsigned long pressedMillis = 0;
  static unsigned long calibrationPressedMillis = 0;
  if (interruptPressed) {
    interruptPressed = false;
    pressed = true;

    pressedMillis = millis();
    disableInterrupt(BUTTON_PIN);
    interruptEnabled = false;
  } else if ((*portInputRegister(BUTTON_PIN_PORT) & BUTTON_PIN_BIT) == LOW) {
    if (longPressed) {
      longPressed = false;
      pressedMillis = millis();
    } else if (millis() - pressedMillis > 1500) {
      longPressed = true;
    }
  } else if (millis() - pressedMillis > 50) {
    enableInterrupt(BUTTON_PIN, UserInput::Button::onButtonPressed, FALLING);
    interruptEnabled = true;
    pressed = false;
    longPressed = false;
  }
#if CALIBRATION_BUTTON_ENABLE == true
  if (interruptCalibrationPressed) {
    interruptCalibrationPressed = false;
    calibrationPressed = true;
    calibrationPressedMillis = millis();
    disableInterrupt(CALIBRATION_BUTTON_PIN);
    interruptCalibrationEnabled = false;
  } else if ((*portInputRegister(CALIBRATION_BUTTON_PIN_PORT) & CALIBRATION_BUTTON_PIN_BIT) == LOW) {
    if (calibrationLongPressed) {
      calibrationLongPressed = false;
      calibrationPressedMillis = millis();
    } else if (millis() - calibrationPressedMillis > 1500) {
      calibrationLongPressed = true;
    }
  } else if (millis() - calibrationPressedMillis > 50) {
    enableInterrupt(CALIBRATION_BUTTON_PIN, UserInput::Button::onCalibrationButtonPressed, FALLING);
    interruptCalibrationEnabled = true;
    calibrationPressed = false;
    calibrationLongPressed = false;
  }
#endif
wdt_reset();
}

bool UserInput::Button::isSleep(void) {
  return sleep;
}

bool UserInput::Button::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void UserInput::Button::setSleep(bool sleep) {
  sleepBuffer = sleep;
}

/**
 * Interrupt Routine
 */
void UserInput::Button::onButtonPressed(void) {
  interruptPressed = true;
  wdt_reset();
}

#if CALIBRATION_BUTTON_ENABLE == true
void UserInput::Button::onCalibrationButtonPressed(void) {
  interruptCalibrationPressed = true;
  wdt_reset();
}
#endif