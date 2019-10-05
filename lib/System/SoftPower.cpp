#define LIBCALL_ENABLEINTERRUPT

#include "Config.h"
#include "Indicating.h"
#include "System.h"
#include "lang.h"


#ifdef SOFT_POWER_BUTTON

#include <EnableInterrupt.h>

bool System::SoftPower::pressed = false;
bool System::SoftPower::longPressed = false;
bool System::SoftPower::interruptPressed = false;
bool System::SoftPower::onMain = false;

void System::SoftPower::setup(void) {
  pinMode(SOFT_POWER_BUTTON, INPUT_PULLUP);
  pinMode(FORCE_SHUTDOWN_PIN,OUTPUT);
  digitalWrite(FORCE_SHUTDOWN_PIN,HIGH);
  //delay(1000);
  powerOn();
  while (isPressed());
  
  enableInterrupt(SOFT_POWER_BUTTON, System::SoftPower::onButtonPressed, FALLING);
}

void System::SoftPower::loop(boolean softRun) {
  static unsigned long pressedMillis = 0;
  onMain = true;
  wdt_reset();
  if (interruptPressed) {
    interruptPressed = false;
    pressed = true;
    pressedMillis = millis();
    disableInterrupt(SOFT_POWER_BUTTON);
  } else if (digitalRead(SOFT_POWER_BUTTON) == LOW) {
    if (longPressed) {
      longPressed = false;
      pressedMillis = millis();
    } else if (millis() - pressedMillis > 3000) {
      longPressed = true;
    }
  } else if (millis() - pressedMillis > 100) {
    enableInterrupt(SOFT_POWER_BUTTON, System::SoftPower::onButtonPressed, FALLING);
    pressed = false;
    longPressed = false;
  }
  if(softRun){
    if (longPressed) {
      //Indicating::Display::info(Indicating::Translation::get(SHUTING_DOWN));
      Indicating::Display::standbyScreen(Indicating::Translation::get(SHUTING_DOWN));
      wdt_reset();
      delay(1500);
      powerOff();
      //Indicating::Display::info(F(""), F(""), F(""), F(""));
      powerOff();
      unsigned long pressedOnMillis = millis();
      while (true) {
        if (!digitalRead(SOFT_POWER_BUTTON) && millis() - pressedOnMillis > 15000) break;
        else if (digitalRead(SOFT_POWER_BUTTON)) pressedOnMillis = millis();
      }
      powerOn();
      Indicating::Display::info(Indicating::Translation::get(TURNING_ON));
    }
  }
}

void System::SoftPower::onButtonPressed(void) {
  interruptPressed = true;
  if (!onMain) {
    powerOff();
  }
}

bool System::SoftPower::isPressed(void) {
  return !digitalRead(SOFT_POWER_BUTTON); // pressed
}

void System::SoftPower::powerOn(void) 
{
  //digitalWrite(SOFT_POWER_CONTROL, HIGH);
  //SOFT_POWER_CONTROL_PORT |=SOFT_POWER_CONTROL_PIN;
}

void System::SoftPower::powerOff(void) {
  //SOFT_POWER_CONTROL_PORT &=~SOFT_POWER_CONTROL_PIN;
  //digitalWrite(SOFT_POWER_CONTROL, LOW);
 /* while(Sensor::Battery::getVoltage() > 2.7)
  {
    Sensor::Battery::loop();
  };*/
  digitalWrite(FORCE_SHUTDOWN_PIN,LOW);

}
#endif
