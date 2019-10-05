#define LIBCALL_ENABLEINTERRUPT

#include "Storage.h"
#include "Config.h"
#include "Indicating.h"

#include <EnableInterrupt.h>



RTC_DS1307 Indicating::RTC::ds1307;
DateTime Indicating::RTC::dateTime;
DateTime Indicating::RTC::dateTimeBuffer;

bool Indicating::RTC::sleep = false;
bool Indicating::RTC::sleepBuffer = false;

int Indicating::RTC::timezone = 0;
char Indicating::RTC::dateTimeSQLFormat[21];
char Indicating::RTC::dateTimeAtomFormat[26];
bool Indicating::RTC::dateTimeChanged = false;
volatile bool Indicating::RTC::halfSecond = false;
volatile unsigned int Indicating::RTC::secondCount = 0;


void Indicating::RTC::setup(void) {
  pinMode(RTC_SQWO_PIN, INPUT_PULLUP);

  ds1307.begin();
  if ( !ds1307.isrunning())
  {
    int Hour, Min, Sec;
    char Month[12];
    int Day, Year;
    uint8_t monthIndex;

  const char *monthName[12] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    if (sscanf(__TIME__, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return;

    if (sscanf(__DATE__, "%s %d %d", Month, &Day, &Year) != 3) return;
    for (monthIndex = 0; monthIndex < 12; monthIndex++) 
    {
      if (strcmp(Month, monthName[monthIndex]) == 0) break;
    }
    if (monthIndex >= 12) return;
      ds1307.adjust(DateTime(Year, monthIndex + 1, Day, Hour, Min, Sec));
  }
  ds1307.writeSqwPinMode(DS1307_SquareWave1HZ);
  dateTime = ds1307.now();
  secondCount = 0;
  enableInterrupt(RTC_SQWO_PIN, Indicating::RTC::onChangeSQWO, CHANGE);
  dateTime = ds1307.now();
}

void Indicating::RTC::loop(void) {
  if (sleepBuffer != sleep) {
    sleep = sleepBuffer;
  }
  if (sleep) {
    if (!digitalRead(RTC_SQWO_PIN) || (secondCount % 10 == 0)) {
      dateTime = ds1307.now();
    }
    return;
  }
  if (dateTimeChanged) {
    dateTime = dateTimeBuffer + TimeSpan(secondCount);
    ds1307.adjust(dateTime);
    dateTimeChanged = false;
    secondCount = 0;
  }
  if ((dateTime.second() + secondCount) >= 60) {
    dateTime = ds1307.now();
    secondCount = 0;
  } else if (secondCount > 0) {
    dateTime = dateTime + TimeSpan(secondCount);
    secondCount = 0;
  }
}

bool Indicating::RTC::isSleep(void) {
  return sleep;
}

bool Indicating::RTC::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void Indicating::RTC::setSleep(bool sleep) {
  sleepBuffer = sleep;
}

void Indicating::RTC::setDateTimeGSMFormat(const char *dateTimeGSMFormat) {
  char dateTimeGSMFormatBuffer[strlen(dateTimeGSMFormat) + 1];
  uint16_t year;
  uint8_t  month, day, hour, minute, second;

  strcpy(dateTimeGSMFormatBuffer, dateTimeGSMFormat);
  year    = (uint16_t) atoi(strtok_P(dateTimeGSMFormatBuffer, (const char*)F("/,:+-")));
  month   = (uint8_t) atoi(strtok_P(NULL, (const char*)F("/,:+-")));
  day     = (uint8_t) atoi(strtok_P(NULL, (const char*)F("/,:+-")));
  hour    = (uint8_t) atoi(strtok_P(NULL, (const char*)F("/,:+-")));
  minute  = (uint8_t) atoi(strtok_P(NULL, (const char*)F("/,:+-")));
  second  = (uint8_t) atoi(strtok_P(NULL, (const char*)F("/,:+-")));
  timezone = atoi(dateTimeGSMFormat + 17) / 4;
  Storage::Eeprom::writeTimeZone(timezone);
  dateTimeBuffer = DateTime(year, month, day, hour, minute, second);
  secondCount = 0;
  dateTimeChanged = true;
}

char *Indicating::RTC::getDateTimeSQLFormat(void) {
  sprintf_P(dateTimeSQLFormat, (const char*)F("%04d-%02d-%02d %02d:%02d:%02d"),
          dateTime.year(), dateTime.month(), dateTime.day(),
          dateTime.hour(), dateTime.minute(), dateTime.second());
  return dateTimeSQLFormat;
}

char *Indicating::RTC::getDateTimeSQLFormatWithoutSecond(void) {
  char *dateTimeSQLFormat = getDateTimeSQLFormat();
  dateTimeSQLFormat[17] = '\0';
  return dateTimeSQLFormat;
}

char *Indicating::RTC::getDateTimeAtomFormat(void) {
  timezone = (int) Storage::Eeprom::getTimeZone();
  sprintf_P(dateTimeAtomFormat, (const char*)F("%04d-%02d-%02dT%02d:%02d:%02d%c%02d:00"), dateTime.year(), dateTime.month(), dateTime.day(),
  dateTime.hour(), dateTime.minute(), dateTime.second(), timezone >= 0 ? '+':'-', timezone);
  //Serial.println("time format :"+String(dateTimeAtomFormat));
  return dateTimeAtomFormat;
}

/**
 * Interrupt Routine
 */
void Indicating::RTC::onChangeSQWO(void) {
  halfSecond = digitalRead(RTC_SQWO_PIN);
  if (halfSecond)
  {
    wdt_reset();
    return;
  } 
  secondCount++;
}
