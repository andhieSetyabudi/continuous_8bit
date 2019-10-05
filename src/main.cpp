#include <Arduino.h>
#include <EEPROM.h>
#include "Indicating.h"
#include "Config.h"
#include "Communication.h"
#include "Sensor.h"
#include "Storage.h"
#include "System.h"
#include "UserInput.h"
#include "main.h"
#include "lang.h"
#include <EnableInterrupt.h>

void(* resetFunc) (void) = 0;

char info_[6];

void setup() {
  Indicating::setup();
  System::setup();
  Sensor::Battery::setup();
  if (!Indicating::Display::isCharging() && Sensor::Battery::isLowStatus()) 
  {
    Indicating::Display::info(t(WARNING_BATTERY_LOW));
    delay(1000);
  }
  #ifdef SOFT_POWER_BUTTON
    while (true) {
      if (!Indicating::Display::isCharging() && Sensor::Battery::getPercentage() < 1) 
        System::SoftPower::powerOff();
      else
          break;
      System::SoftPower::loop();
      if (System::SoftPower::isLongPressed())
        break;
    }
#endif
UserInput::setup();

byte buttonReg = PINJ;
  if (!bitRead(buttonReg,2)) 
  {
    System::Identity::setId("DEBUG");
    Indicating::Display::splash(F("Set ID \"DEBUG\""));
    delay(1000);
  };
  if(!bitRead(buttonReg,4))
  {
    if ( Storage::Eeprom::getLocale() > 0 ) 
    {
      if(Indicating::Translation::setLocale(0))
      {
        Indicating::Display::splash(F("Set ENGLISH"));
        delay(1000);
        resetFunc();
      }
    }
    else
    {
      if(Indicating::Translation::setLocale(1))
      {
        Indicating::Display::splash(F("Set BAHASA"));
        delay(1000);
        resetFunc();
      }
    }
    Indicating::Translation::setup();
  }
  
  
  Sensor::setup();
  #if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE || TEMPERATURE_SENSOR_INTERFACE ==_INTERFACE_SERIAL
    sensorCalibration();
  #endif
  //return;
  Communication::setup();
  //return;
  sendPendingDataConfirmation();
  //enableInterrupt(CHARGE_STAT_PIN, chargingState, CHANGE);
  wdt_enable(WDTO_8S); wdt_reset();
  //return;
  //displayData(measurementData , 800);
}

void loop() 
{
  static bool isSleep = false; 
  volatile unsigned int currentMinuteInDay = ((unsigned int)Indicating::RTC::getHour() * 60) + Indicating::RTC::getMinute();
  static unsigned int lastSleep = Indicating::RTC::getSecondTime();
  wdt_reset();
  System::loop();
  Indicating::loop();
  UserInput::loop();
  Communication::loop();
  UserInput::loop();
  Sensor::loop();
  if ( !Indicating::Display::isCharging() && Sensor::Battery::isLowStatus())//Sensor::Battery::getPercentage <= 15U )
  {
      static unsigned int timeWarn = Indicating::RTC::getSecondTime();
      if( millis() - timeWarn >= 300UL)    // every 5 minutes
      {
        setDisplayedStatus(false);
        displayData(notifBatteryLowbat, 1000);
        timeWarn = Indicating::RTC::getSecondTime();;
      }
  }

  if (!Indicating::Display::isCharging() && Sensor::Battery::getPercentage() <= min_percentage_off) 
  {
    Indicating::Display::info(t(PLEASE_CHARGE_BATTERY));
    setSystem(true);
    #ifdef SOFT_POWER_BUTTON
        if (!Indicating::Display::isCharging() && Sensor::Battery::getPercentage() < min_percentage_off) 
          System::SoftPower::powerOff();
    #endif
      //return;
  } 
  if (isSleep)
  {
    UserInput::Button::loop();
    System::SoftPower::loop(false);
    if ( System::SoftPower::isPressed() || UserInput::Button::isCalibrationPressed() || UserInput::Button::isPressed() )
    {
      isSleep = false;
      Indicating::Display::info("",t(PLEASE_WAIT),"","");
      while(System::SoftPower::isPressed() || UserInput::Button::isCalibrationPressed() || UserInput::Button::isPressed())
      {
        System::SoftPower::loop(false);
        UserInput::Button::loop();
        delay(10);
      };
      isSleep = false;
      lastSleep = Indicating::RTC::getSecondTime();
    };
  }
  else
  {
      UserInput::Button::loop();
      if (strlen(System::Identity::getId()) == 0 && UserInput::Button::isPressed()) 
      {
        setDisplayedStatus(false);
        displayData(notifTapID, 300);
      } 
      else if ( UserInput::Button::isPressed() && strlen(System::Identity::getId()) > 0) 
      {
        BlumbangReksaData blumbangReksaData = getCurrentBlumbangReksaData();
        setDisplayedStatus(false);
        displayData(notifHoldToSend, 100);
        while (UserInput::Button::isPressed() && !UserInput::Button::isLongPressed()) 
        {
          UserInput::Button::loop();
          delay(10);
        }
        if (UserInput::Button::isLongPressed()) {
          if (Communication::Cellular::queueBlumbangReksaData(blumbangReksaData)) 
          {
            setDisplayedStatus(false);
            displayData(notifSendData, 300);
            Communication::Cellular::setAllowToConnect(true);
          } 
          else
          {
            setDisplayedStatus(false);
            displayData(notifDataQueueFull, 300);
          }
        }
      }
  }
  
  
  #if CONTINUOUS_MODE
  unsigned int diff = currentMinuteInDay % CONTINUOUS_MINUTE_INTERVAL;
  static bool isAlreadyGrabData = false;
  bool timeOutWake = ( Indicating::RTC::getSecondTime() - lastSleep >= 90 )? true:false;
  
  if(!isSleep)
  {
    if( !isAlreadyGrabData && (diff == 0) )
    {
        if (strlen(System::Identity::getId()) > 0) 
        {
          Communication::Cellular::queueBlumbangReksaData(getCurrentBlumbangReksaData());
          Communication::Cellular::queueBlumbangReksaDevice(getCurrentBlumbangReksaDevice());
          Communication::Cellular::setAllowToConnect(true);
        }
        lastSleep = Indicating::RTC::getSecondTime(); //currentMinuteInDay;
        isAlreadyGrabData = true;
    } 

    // re-set isAlreadyGrabData flag 
    else if ( isAlreadyGrabData )
    {
      if ( Indicating::RTC::getSecondTime() - lastSleep >= 75 )//  && Indicating::RTC::getSecond() >= lastSecond ) // re- set after 65 seconds
        isAlreadyGrabData = false;
    }
  };

  // set isSleep flag to false, system will run normally
  if((timeOutWake && isSleep && ( diff >= (CONTINUOUS_MINUTE_INTERVAL - wakeUpFromSleep) ) ) )
  {
      isSleep = false;
      isAlreadyGrabData = false;  
  }
  // re - set isSleep flag to be true, system will be in sleep mode
  else if (timeOutWake  && !isSleep && ( /*( Indicating::RTC::getSecond() >= lastSecond ) &&*/ ( diff > sleepAfterSent ) && (diff <= (CONTINUOUS_MINUTE_INTERVAL - wakeUpFromSleep) ) )  && ( Communication::Cellular::totalQueuedBlumbangReksaData() == 0 ) ) 
  {
      isSleep = true;
      isAlreadyGrabData = isAlreadyGrabData?false:true;
  }
  
#endif
  setSystem(isSleep);

  static bool deviceUpdated = false;
  if (!deviceUpdated && strlen(System::Identity::getId()) > 0 &&
      strlen(Communication::Cellular::getLatitude()) > 0 &&
      strlen(Communication::Cellular::getLongitude()) > 0) {
    Communication::Cellular::queueBlumbangReksaDevice(getCurrentBlumbangReksaDevice());
    deviceUpdated = true;
  }
 
  //if(isSleep)
    //strcpy_P(info_, (const char *)F("..."));
  if (strlen(System::Identity::getId())) 
    sprintf_P(info_, (const char *)F("%s"), System::Identity::getId());
  else 
    strcpy(info_, t(TAP_ID_CARD));
  
   
#if CALIBRATION_BUTTON_ENABLE == true
  UserInput::Button::loop();
  if (UserInput::Button::isCalibrationPressed() && !isSleep) {
    calibrationMenu();
  }
#endif
  if (isSleep)
  {
    displayData(sleepDisplay , 500);
  }
  else
    displayData(measurementData , 500);
  wdt_reset();
}


bool confirmation(const char *confirmationText, const char *yesText, const char *noText, bool defaultChoose, unsigned long timeout) {
  bool confirmed = false;
  if (defaultChoose) 
  {
    unsigned long startConfirmation = millis();
    while (true) 
    {
      UserInput::Button::loop();
      if (millis() - startConfirmation >= timeout) 
            return defaultChoose;
      confirmed = Indicating::Display::confirmation(confirmationText, yesText, noText);
      if (UserInput::Button::isPressed()) 
      {
        do {
          UserInput::Button::loop();
          if (UserInput::Button::isLongPressed()) {
            return confirmed;
          }
        } while (UserInput::Button::isPressed());
        Indicating::Display::confirmationNext();
      }
    }
  } else {
    unsigned long startConfirmation = millis();
    while (true) {
      UserInput::Button::loop();
      confirmed = Indicating::Display::confirmation(confirmationText, yesText, noText);
      if (millis() - startConfirmation >= timeout) {
        return false;
      }
      if (UserInput::Button::isPressed()) {
        do {
          UserInput::Button::loop();
          if (UserInput::Button::isLongPressed()) {
            return confirmed;
          }
        } while (UserInput::Button::isPressed());
        Indicating::Display::confirmationNext();
      }
    }
  }
}

void sensorCalibration(void) {
  char buffer[20];
  char calibrationPointBuffer[5] = {'\0'};
  unsigned char calibrationPoint;
#if PH_SENSOR_SERIAL_ENABLE & !CALIBRATION_BUTTON_ENABLE
  strcpy_P(calibrationPointBuffer, (const char *)F("Init pH"));
  calibrationPoint = Sensor::Water::pHCalibrationPoint();
  for (uint8_t i = 0; i < calibrationPoint; i++)
    calibrationPointBuffer[i + 7] = '.';
  calibrationPointBuffer[calibrationPoint + 7] = '\0';
  Indicating::Display::splash(calibrationPointBuffer);
  delay(700);
#if !CALIBRATION_BUTTON_ENABLE
  UserInput::Button::loop();
  if (UserInput::Button::isPressed()) {
    sprintf_P(buffer, (const char*)F("%s pH"), t(CALIBRATION));
    Indicating::Display::info(buffer);
    do {
      delay(100);
      UserInput::Button::loop();
    } while (UserInput::Button::isPressed());
    Indicating::Display::info(t(PUT_PROBE_PH_TO_7));
    do {
      delay(100);
      UserInput::Button::loop();
    } while (!UserInput::Button::isPressed());
    Sensor::Water::pHCalibratingMid();
    Indicating::Display::info(t(PUT_PROBE_PH_TO_4_10));
    do {
      delay(100);
      UserInput::Button::loop();
    } while (!UserInput::Button::isPressed());
    Sensor::Water::pHCalibratingLowHigh();
    Indicating::Display::info(t(PH_CALIBRATION_DONE));
    delay(500);
  }
#endif
#endif
#if EC_SENSOR_SERIAL_ENABLE & !CALIBRATION_BUTTON_ENABLE
  strcpy_P(calibrationPointBuffer, (const char *)F("Init EC"));
  calibrationPoint = Sensor::Water::electricalConductivityCalibrationPoint();
  for (uint8_t i = 0; i < calibrationPoint; i++)
    calibrationPointBuffer[i + 7] = '.';
  calibrationPointBuffer[calibrationPoint + 7] = '\0';
  Indicating::Display::splash(calibrationPointBuffer);
  delay(750);
#if !CALIBRATION_BUTTON_ENABLE
  UserInput::Button::loop();
  if (UserInput::Button::isPressed()) {
    sprintf_P(buffer, (const char*)F("%s EC"), t(CALIBRATION));
    Indicating::Display::info(buffer);
    do {
      delay(100);
      UserInput::Button::loop();
    } while (UserInput::Button::isPressed());
    Indicating::Display::info(t(EC_PROBE_DRY));
    do {
      delay(100);
      UserInput::Button::loop();
    } while (!UserInput::Button::isPressed());
    Sensor::Water::electricalConductivityCalibratingDry();
    Indicating::Display::info(t(PUT_PROBE_EC_TO_12880));
    do {
      delay(100);
      UserInput::Button::loop();
    } while (!UserInput::Button::isPressed());
    Sensor::Water::electricalConductivityCalibratingLow();
    Indicating::Display::info(t(PUT_PROBE_EC_TO_80000));
    do {
      delay(100);
      UserInput::Button::loop();
    } while (!UserInput::Button::isPressed());
    Sensor::Water::electricalConductivityCalibratingHigh();
    Indicating::Display::info(t(EC_CALIBRATION_DONE));
    delay(500);
  }
#endif
#endif
#if DO_SENSOR_SERIAL_ENABLE   //& !CALIBRATION_BUTTON_ENABLE
  strcpy_P(calibrationPointBuffer, (const char *)F("Init DO"));
  calibrationPoint = Sensor::Water::dissolvedOxygenCalibrationPoint();
  for (uint8_t i = 0; i < calibrationPoint; i++)
    calibrationPointBuffer[i + 7] = '.';
  calibrationPointBuffer[calibrationPoint + 7] = '\0';
  Indicating::Display::splash(calibrationPointBuffer);
  delay(750);
#if !CALIBRATION_BUTTON_ENABLE
  UserInput::Button::loop();
  if (UserInput::Button::isPressed()) {
    sprintf_P(buffer, (const char*)F("%s DO"), t(CALIBRATION));
    Indicating::Display::info(F(""), buffer, F(""), F(""));
    do {
      delay(100);
      UserInput::Button::loop();
    } while (UserInput::Button::isPressed());
    Indicating::Display::info(t(DAMP_THE_PROBE_DO));
    do {
      delay(100);
      UserInput::Button::loop();
    } while (!UserInput::Button::isPressed());
    Sensor::Water::dissolvedOxygenCalibratingAtmospheric();
    Indicating::Display::info(t(DO_CALIBRATION_DONE));
    delay(500);
  }
#else
  
  
  UserInput::Button::loop();
  if (UserInput::Button::isPressed()) 
  {
    sprintf_P(buffer, (const char *)F("DO Zero Calibration"));
    Indicating::Display::info(F(""), buffer, F(""),F(""));
    delay(3500);
    do 
    {
      delay(100);
      UserInput::Button::loop();
    } while (!UserInput::Button::isPressed());

    Sensor::Water::dissolvedOxygenCalibratingZero();

    sprintf_P(buffer, (const char*)F("%s DO"), t(CALIBRATION));
    Indicating::Display::info(F(""), buffer, F(""), F(""));
    do 
    {
      delay(100);
      UserInput::Button::loop();
    } while (UserInput::Button::isPressed());
    Indicating::Display::info(t(DAMP_THE_PROBE_DO));
    do 
    {
      delay(100);
      UserInput::Button::loop();
    } while (!UserInput::Button::isPressed());
    Sensor::Water::dissolvedOxygenCalibratingAtmospheric();
    Indicating::Display::info(t(DO_CALIBRATION_DONE));
    delay(500);
  }
#endif
#endif
#if ORP_SENSOR_SERIAL_ENABLE & !CALIBRATION_BUTTON_ENABLE
  strcpy_P(calibrationPointBuffer, (const char *)F("Init ORP"));
  calibrationPoint = Sensor::Water::oxidationReductionPotentialCalibrationPoint();
  for (size_t i = 0; i < calibrationPoint; i++)
    calibrationPointBuffer[i + 7] = '.';
  calibrationPointBuffer[calibrationPoint + 7] = '\0';
  Indicating::Display::splash(calibrationPointBuffer);
  delay(750);
#if !CALIBRATION_BUTTON_ENABLE
  UserInput::Button::loop();
  if (UserInput::Button::isPressed()) {
    sprintf_P(buffer, (const char*)F("%s ORP"), t(CALIBRATION));
    Indicating::Display::info(F(""), buffer, F(""), F(""));
    do {
      delay(100);
      UserInput::Button::loop();
    } while (UserInput::Button::isPressed());
    Indicating::Display::info(t(PUT_PROBE_ORP_TO_255));
    do {
      delay(100);
      UserInput::Button::loop();
    } while (!UserInput::Button::isPressed());
    Sensor::Water::oxidationReductionPotentialCalibrating();
    Indicating::Display::info(t(ORP_CALIBRATION_DONE));
    delay(500);
  }
#endif
#endif

#if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE || _INTERFACE_SERIAL
  strcpy_P(buffer, (const char *)F("Init Temp"));
  Indicating::Display::splash(buffer);
  delay(750);

  for(size_t ui = 0; ui < 10; ui++)
  {
    Sensor::Water::getTemperature();
    delay(100);
    UserInput::Button::loop();
    if (UserInput::Button::isPressed())
    {
      sprintf_P(buffer, (const char*)F("%s Temperature"), t(CALIBRATION));
      Indicating::Display::info(F(""), buffer, F(""), F(""));
      uint8_t timeouts = 0;
      do {
        delay(100);
        UserInput::Button::loop();
        timeouts++;
      } while ( UserInput::Button::isPressed() && (timeouts < 100));
      if(timeouts >= 100)
      {
        sprintf_P(buffer, (const char*)F("Timeout"));
        Indicating::Display::info(F(""), buffer, F(""), F(""));
        delay(500);
        break;
      }

      timeouts = 0;
      Indicating::Display::info(t(TEMP_REF_PLUG_IN));
      do {
        delay(100);
        UserInput::Button::loop();
        timeouts++;
      } while ( !UserInput::Button::isPressed() && (timeouts < 100) );
      if(timeouts >= 100)
      {
        sprintf_P(buffer, (const char*)F("Timeout"));
        Indicating::Display::info(F(""), buffer, F(""), F(""));
        delay(500);
        break;
      }
      delay(100);
      for(size_t uii = 0; uii < 3; uii++)
      {
      if (Sensor::Water::temperatureCalibration()){
        sprintf_P(buffer, (const char*)F("Calibration done "));
        break;
      }
      else
        sprintf_P(buffer, (const char*)F("Calibration failed !"));
      }
      Indicating::Display::info(buffer);
      delay(500);
      return;
    }
  }
#endif
}

#if CALIBRATION_BUTTON_ENABLE == true
#define buttonClickInterval     5
bool buttonClickedLoop (void)
{
  do {
        delay(buttonClickInterval);
        //Communication::loop();
        UserInput::Button::loop();
      } while (!UserInput::Button::isPressed() && !UserInput::Button::isCalibrationPressed());
      return true;
}
void calibrationMenu(void) {
  unsigned char position = 255;
  char buffer[16];
   
  while(UserInput::Button::isCalibrationPressed()){UserInput::Button::loop();};
  //unsigned long timeUps = millis();
  while (true) {
    wdt_reset();
    UserInput::Button::loop();
    System::loop();
    Indicating::Display::menuCalibration();
    if (UserInput::Button::isCalibrationPressed()) {
      Indicating::Display::menuCalibrationNext();
    }

    if (UserInput::Button::isPressed()) {
      delay(buttonClickInterval);
      position = Indicating::Display::menuCalibrationGetPosition();
    } else {
      position = 255;
    }

    if (position == 0) {
      sprintf_P(buffer, (const char*)F(" %s pH"), t(CALIBRATION));
      Indicating::Display::info(buffer);
      delay(1000);
      do {
        delay(10);
        UserInput::Button::loop();
      } while (UserInput::Button::isPressed());
      Indicating::Display::info(t(PUT_PROBE_PH_TO_7));
      if (buttonClickedLoop())
        if (UserInput::Button::isCalibrationPressed()) {continue;};
      if(!Sensor::Water::pHCalibratingMid()) 
        continue;
//calib_second_point:
      Indicating::Display::info(t(PUT_PROBE_PH_TO_4_10));
      if (buttonClickedLoop())
        if (UserInput::Button::isCalibrationPressed())  continue;
        
      if (Sensor::Water::pHCalibratingLowHigh())
        Indicating::Display::info(t(PH_CALIBRATION_DONE));
      else
      {
  //      goto calib_second_point;
        continue;
      }
      delay(500);
      Indicating::Display::menuCalibrationNext();
    } 
    else if (position == 1) 
    {
      sprintf_P(buffer, (const char*)F(" %s EC"), t(CALIBRATION));
      Indicating::Display::info(buffer);
      delay(1000);
       if(!Sensor::Water::isECAvailable())
      {
        sprintf(buffer, Indicating::Translation::get(PLEASE_CHECK_SENSOR), (const char*)"EC");
        Indicating::Display::info(buffer);
        delay(1500);
        goto ec_next;
      };
      do {
        delay(buttonClickInterval);
        UserInput::Button::loop();
        wdt_reset();
      } while (UserInput::Button::isPressed());
      Indicating::Display::info(t(EC_PROBE_DRY));
      
      if (buttonClickedLoop())
        if (UserInput::Button::isCalibrationPressed()) { continue; }
      
      Sensor::Water::electricalConductivityCalibratingDry();
      Indicating::Display::info(t(PUT_PROBE_EC_TO_12880));
      
      if (buttonClickedLoop())
        if (UserInput::Button::isCalibrationPressed()) { continue; }
      
      Sensor::Water::electricalConductivityCalibratingLow();
      Indicating::Display::info(t(PUT_PROBE_EC_TO_80000));
      
      if (buttonClickedLoop())
        if (UserInput::Button::isCalibrationPressed()) { continue; }
      
      Sensor::Water::electricalConductivityCalibratingHigh();
      Indicating::Display::info(t(EC_CALIBRATION_DONE));
    ec_next:
      delay(500);
      Indicating::Display::menuCalibrationNext();
    } else if (position == 2) {
      sprintf_P(buffer, (const char*)F(" %s DO"), t(CALIBRATION));
      Indicating::Display::info(buffer);
      delay(1000);
      if(!Sensor::Water::isDOAvailable())
      {
        sprintf(buffer, Indicating::Translation::get(PLEASE_CHECK_SENSOR), (const char*)"DO");
        Indicating::Display::info(buffer);
        delay(1500);
        goto do_next;
      };
      
      do {
        delay(100);
        UserInput::Button::loop();
      } while (UserInput::Button::isPressed());
      Indicating::Display::info(t(DAMP_THE_PROBE_DO));
      if (buttonClickedLoop())
        if (UserInput::Button::isCalibrationPressed()) { continue; }
      Sensor::Water::dissolvedOxygenCalibratingAtmospheric();
      Indicating::Display::info(t(DO_CALIBRATION_DONE));
      delay(500);
    do_next:
      Indicating::Display::menuCalibrationNext();
    }
    
    else if (position == 3) {
      Indicating::Display::menuCalibrationNext();
      break;
    }
  }
}
#endif

BlumbangReksaDevice getCurrentBlumbangReksaDevice(void) {
  BlumbangReksaDevice blumbangReksaDevice;
  strcpy(blumbangReksaDevice.code, System::Identity::getId());
  strcpy(blumbangReksaDevice.imei, Communication::Cellular::getImei());
  strcpy(blumbangReksaDevice.phoneNumber, Communication::Cellular::getSubscriberNumber());
  blumbangReksaDevice.inputVoltage = Sensor::Battery::getVoltage();
  blumbangReksaDevice.networkSignal = Communication::Cellular::getSignalQualityRssi();
  strcpy(blumbangReksaDevice.latitude, Communication::Cellular::getLatitude());
  strcpy(blumbangReksaDevice.longitude, Communication::Cellular::getLongitude());
  strcpy(blumbangReksaDevice.timeStamp, Indicating::RTC::getDateTimeAtomFormat());
  return blumbangReksaDevice;
}

struct BlumbangReksaData getCurrentBlumbangReksaData(void) {
  struct BlumbangReksaData blumbangReksaData;
  strcpy(blumbangReksaData.deviceCode, System::Identity::getId());
  strcpy(blumbangReksaData.deviceImei, Communication::Cellular::getImei());
  strcpy(blumbangReksaData.timeStamp, Indicating::RTC::getDateTimeAtomFormat());
#if DHT_USED
  blumbangReksaData.airTemperature = isnan( Sensor::Ambient::getTemperature() )? 0 : Sensor::Ambient::getTemperature();
  blumbangReksaData.humidity = isnan( Sensor::Ambient::getHumidity() ) ? 0 : Sensor::Ambient::getHumidity();
#endif
  blumbangReksaData.temperature = isnan( Sensor::Water::getTemperature() ) ? 0 : Sensor::Water::getTemperature();
#if DO_SENSOR_SERIAL_ENABLE
  blumbangReksaData.DOPercentage = isnan( Sensor::Water::getPercentageDissolvedOxygen() ) ? 0 : Sensor::Water::getPercentageDissolvedOxygen();
  blumbangReksaData.DO = isnan( Sensor::Water::getDissolvedOxygen() ) ? 0 : Sensor::Water::getDissolvedOxygen();
#endif
#if EC_SENSOR_SERIAL_ENABLE
  blumbangReksaData.EC = isnan( Sensor::Water::getElectricalConductivity() ) ? 0 : Sensor::Water::getElectricalConductivity();
  blumbangReksaData.TDS = isnan( Sensor::Water::getTotalDissolvedSolids() ) ? 0 : Sensor::Water::getTotalDissolvedSolids();
  blumbangReksaData.salinity = isnan( Sensor::Water::getSalinity() ) ? 0 : Sensor::Water::getSalinity();
  blumbangReksaData.specificGravityOfSeaWater = isnan( Sensor::Water::getSpecificGravityOfSeaWater() ) ? 0 : Sensor::Water::getSpecificGravityOfSeaWater();
#endif
#if PH_SENSOR_SERIAL_ENABLE
  blumbangReksaData.pH = isnan( Sensor::Water::getpH() ) ? 0 : Sensor::Water::getpH();
#endif
  return blumbangReksaData;
}

void sendPendingDataConfirmation(void) {
  bool sendPending = false;
  if (Communication::Cellular::totalQueuedBlumbangReksaData() > 0) 
  {
    char buffer[40];
    sprintf(buffer, t(SEND_DELAYED_DATA), Communication::Cellular::totalQueuedBlumbangReksaData());
    char options[2][7];
    strcpy(options[0], t(YES));
    strcpy(options[1], t(DELETE));
    sendPending = confirmation(buffer, options[0], options[1], true, (10*60*100UL));
    unsigned long ti = millis();
    while (UserInput::Button::isPressed()) {
      if ( millis() - ti >= 10000)
      {
        Indicating::Display::info(t(SENDING_DELAYED_DATA));
        sendPending = true;
        break;
      }
      if (!sendPending) {
        Indicating::Display::info(t(DELAYED_DATA_DELETED));
      } else {
        Indicating::Display::info(t(SENDING_DELAYED_DATA));
      }
      UserInput::Button::loop();
    }
    delay(1000);
  }
  if (!sendPending) {
    Communication::Cellular::clearQueuedBlumbangReksaDevice();
    Communication::Cellular::clearQueuedBlumbangReksaData();
  }
  
}

const char* t(int key) {
  return Indicating::Translation::get(key);
}


bool getDisplayStatus(void)
{
  return displayed;
}

bool setDisplayedStatus(bool status){displayed = status; return true;}

void displayData(uint8_t dataDispType, unsigned int timeDisplay)
{
  static uint8_t dataDispType_;  
  static unsigned long startDisplay;
  if ( !displayed )
  {
    startDisplay = millis();
    if(dataDispType_ != dataDispType)
    {
      dataDispType_ = dataDispType;
    }
    
    switch (dataDispType_)
    {
    case measurementData:
                                    if(!displayed)
                                    {
                                      Indicating::Display::measurement(
                                                                        Indicating::RTC::getHour(), Indicating::RTC::getMinute(),
                                                                        (strlen(Communication::Cellular::getCardName()) > 5 )?Communication::Cellular::getCardName():Communication::Cellular::getOperator(),
                                                                        Communication::Cellular::isGprsAttach(),
                                                                        Communication::Cellular::getConnectionStatus(),
                                                                        Communication::Cellular::getSignalQualityRssi() * 0.181818,
                                                                        Sensor::Battery::getPercentage(), Indicating::Display::isCharging(),
                                                                        info_ ,
                                                                        Communication::Cellular::totalQueuedBlumbangReksaData(),
                                                                        Sensor::Water::getTemperature(),
                                                                        //#if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE
                                                                          Sensor::Water::isTemperatureStable(),
                                                                        //#else
                                                                        //  true,
                                                                        //#endif
                                                                        Sensor::Water::getpH(), Sensor::Water::ispHStable() | Sensor::isSleep(),
                                                                        Sensor::Water::getSalinity(), Sensor::Water::isElectricalConductivityStable() | Sensor::isSleep(),
                                                                        Sensor::Water::getDissolvedOxygen(), Sensor::Water::isDissolvedOxygenStable() | Sensor::isSleep());
                                      displayed = true;
                                    }
                                    break;
    case notifBatteryLowbat:
                                    if(!displayed)
                                    {
                                      Indicating::Display::clearDisplay();
                                      Indicating::Display::info(t(WARNING_BATTERY_LOW));
                                      displayed = true;
                                    }
                                    break;
    case notifBatteryChargePlugged:
                                    if(!displayed)
                                    {
                                      Indicating::Display::clearDisplay();
                                      Indicating::Display::info(t(CHARGER_PLUGED));
                                      displayed = true;
                                    }
                                    break;
    case notifBatteryChargeRelease:
                                    if(!displayed)
                                    {
                                      Indicating::Display::clearDisplay();
                                      Indicating::Display::info(t(CHARGER_RELEASED));
                                      displayed = true;
                                    }
                                    break;
    case notifTapID:                
                                    if(!displayed)
                                    {
                                      Indicating::Display::clearDisplay();
                                      Indicating::Display::info(t(PLEASE_TAP_ID));
                                      displayed = true;
                                    }
                                    break;
    case notifSendData:
                                    if(!displayed)
                                    {
                                      Indicating::Display::clearDisplay();
                                      Indicating::Display::info(t(SEND_DATA));
                                      displayed = true;
                                    }
                                    break;
    case notifHoldToSend:
                                    if(!displayed)
                                    {
                                      Indicating::Display::clearDisplay();
                                      Indicating::Display::info(t(HOLD_TO_SEND));
                                      displayed = true;
                                    }
                                    break;
    case notifDataQueueFull:        
                                    if(!displayed)
                                    {
                                      Indicating::Display::clearDisplay();
                                      Indicating::Display::info(t(DATA_SENDING_QUEUE_FULL));
                                      displayed = true;
                                    }
                                    break;
    case displayIdCardData:
                                    if(!displayed)
                                    {
                                      Indicating::Display::clearDisplay();
                                      //Indicating::Display::info(F(""), F(" ID: "), UserInput::RFID::getUidHex(), F(""));
                                      displayed = true;
                                    }
                                    break;
    case sleepDisplay:              
                                    if(!displayed)
                                    {
                                      char tempBuff[10];
                                      char timeBuffer[6];
                                      char valueBuffer[5];
                                      sprintf(tempBuff,"%s 'C", dtostrf(Sensor::Water::getTemperature(), 3, 1, valueBuffer));
                                      sprintf_P(timeBuffer, (const char *)F("%02d:%02d"), Indicating::RTC::getHour(), Indicating::RTC::getMinute());
                                      Indicating::Display::standbyScreen(info_,tempBuff,timeBuffer);
                                      displayed = true;
                                    }
                                    break;
    default:    // do nothing
              break;
    }
    delay(5);
  }
  else
  {
      if(millis() - startDisplay >= timeDisplay && displayed)
      {
        displayed = false;

      }  
  }
}

/*
void chargingState(void)
{
 // static unsigned long lastTimeAccess = millis();
  if(! digitalRead(CHARGER_STATUS_PIN) )
  {
   // if(millis() - lastTimeAccess >= 1000)
    //{
      Indicating::Display::clearDisplay();
      setDisplayedStatus(false);
      displayData(notifBatteryChargePlugged, 500);
   // }
  };
  if( digitalRead(CHARGER_STATUS_PIN) )
  {
    //if(millis() - lastTimeAccess >= 1000)
    //{
      Indicating::Display::clearDisplay();
      setDisplayedStatus(false);
      displayData(notifBatteryChargeRelease, 500);
    //}
  };
  //lastTimeAccess = millis();
}
*/



void setSystem(bool sleepS)
{   
  Communication::setSleep(false);
  Indicating::setSleep(sleepS & Communication::isSleep());
 // Sensor::setSleep(sleepS);
  UserInput::setSleep(sleepS);
  //System::setSleep(sleepS);
  System::setSleep(sleepS                   &
                   //Communication::isSleep() &
                   Indicating::isSleep()    &
                  // Sensor::isSleep()        &
                   UserInput::isSleep());
}