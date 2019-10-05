#ifndef INDICATING_H_
#define INDICATING_H_

#include <Arduino.h>
#include <GadgetDisplay.h>
#include <GadgetConfirmation.h>
#include <GadgetSplash.h>
#include <GadgetStandbyScreen.h>
#include <RTClib.h>
#include <U8glib.h>
#include <Wire.h>


class Indicating {
 private:
  static bool sleep;
  static bool sleepBuffer;

 public:
  static void setup(void);
  static void loop(void);

  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);

  class RTC;
  class Display;
  class Translation;
};

class Indicating::RTC {
 private:
  static bool sleep;
  static bool sleepBuffer;

  static RTC_DS1307 ds1307;
  static DateTime dateTime;
  static DateTime dateTimeBuffer;
  static bool dateTimeChanged;

  static int timezone;
  static char dateTimeSQLFormat[21];
  static char dateTimeAtomFormat[26];
  static volatile bool halfSecond;
  static volatile unsigned int secondCount;

  static unsigned int startingTime;

 public:
  static void setup();
  static void loop();

  static void setDateTimeGSMFormat(const char *dateTimeGSMFormat);

  static DateTime getDateTime(void) { return dateTime; };
  static char *getDateTimeSQLFormat(void);
  static char *getDateTimeSQLFormatWithoutSecond(void);
  static char *getDateTimeAtomFormat(void);
  static const uint8_t getHour(void) { return dateTime.hour(); };
  static const uint8_t getMinute(void) { return dateTime.minute(); };
  static const uint8_t getSecond(void) { return dateTime.second();};
  static const unsigned int getSecondTime(void){dateTime = ds1307.now(); return dateTime.secondstime();};

  static void onChangeSQWO(void);

  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);
};

class Indicating::Display {
 private:
  static bool sleep;
  static bool sleepBuffer;
  static char optionsCalibration[4][17];
  static bool chargingStatus;

  static U8GLIB u8g;
  static GadgetDisplay gadgetDisplay;
  static GadgetConfirmation gadgetConfirmation;
  static GadgetSplash gadgetSplash;
  static GadgetMenu gadgetMenuCalibration;
  static GadgetStandbyScreen gadgedStandby;
  static void chargerStatus(void);

 public:
  static void setup();
  static void loop();

  static void info(const char row0[], const char row1[], const char row2[], const char row3[]);
  static void info(const __FlashStringHelper *row0, const char row1[], const char row2[], const char row3[]);
  static void info(const char row0[], const __FlashStringHelper *row1, const char row2[], const char row3[]);
  static void info(const __FlashStringHelper *row0, const __FlashStringHelper *row1, const char row2[], const char row3[]);
  static void info(const char row0[], const char row1[], const __FlashStringHelper *row2, const char row3[]);
  static void info(const __FlashStringHelper *row0, const char row1[], const __FlashStringHelper *row2, const char row3[]);
  static void info(const char row0[], const __FlashStringHelper *row1, const __FlashStringHelper *row2, const char row3[]);
  static void info(const __FlashStringHelper *row0, const __FlashStringHelper *row1, const __FlashStringHelper *row2, const char row3[]);
  static void info(const char row0[], const char row1[], const char row2[], const __FlashStringHelper *row3);
  static void info(const __FlashStringHelper *row0, const char row1[], const char row2[], const __FlashStringHelper *row3);
  static void info(const char row0[], const __FlashStringHelper *row1, const char row2[], const __FlashStringHelper *row3);
  static void info(const __FlashStringHelper *row0, const __FlashStringHelper *row1, const char row2[], const __FlashStringHelper *row3);
  static void info(const char row0[], const char row1[], const __FlashStringHelper *row2, const __FlashStringHelper *row3);
  static void info(const __FlashStringHelper *row0, const char row1[], const __FlashStringHelper *row2, const __FlashStringHelper *row3);
  static void info(const char row0[], const __FlashStringHelper *row1, const __FlashStringHelper *row2, const __FlashStringHelper *row3);
  static void info(const __FlashStringHelper *row0, const __FlashStringHelper *row1, const __FlashStringHelper *row2, const __FlashStringHelper *row3);
  static void info(const char text[]);
  static void info(const __FlashStringHelper *text);
  static void measurement(unsigned char hour, unsigned char minute, const char oper[], bool isGprsAttach, unsigned char connectionStatus, unsigned char signalStrength, 
                          unsigned char batt, bool isCharging,
                          const char id[], int totalQueuedData, 
                          float temperature, bool isTemperatureStable, 
                          float pH, bool ispHStable, 
                          float salinity, bool isSalinityStable,
                          float dissolvedOxygen, bool isDissolvedOxygenStable);

  static bool confirmation(const char confirmationText[], const char yesText[], const char noText[]);
  static bool confirmation(const char confirmationText[], const __FlashStringHelper yesText[], const __FlashStringHelper noText[]);
  static bool confirmation(const __FlashStringHelper confirmationText[], const __FlashStringHelper yesText[], const __FlashStringHelper noText[]);
  static void confirmationNext(void);
  static void confirmationPrevious(void);

  static void menuCalibration(void);
  static void resetMenuCalibrationPosition(void);
  static void menuCalibrationNext(void);
  static void menuCalibrationPrevious(void);
  static unsigned char menuCalibrationGetPosition(void);

  static void splash(const char info[]);
  static void splash(const __FlashStringHelper info[]);

  static void standbyScreen(const char info1[], const char info2[], const char info3[]);
  static void standbyScreen(const char info[]);

  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);

  static void chargingPinSetup(void);
  static bool isCharging(void);

  static void clearDisplay(void);

  static float getBatteryVolt(void);
  static float getSourceVoltage(void);
  static void  BMS_control(void);

};

class Indicating::Translation {
private:
  static unsigned char locale;

public:
  static char buffer[65];

  static void setup();
  static bool setLocale(unsigned char lang);
  static unsigned char getLocale(void) { return locale; };
  static const char* get(int key);
};

#endif
