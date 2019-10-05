#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DTE.h>
#include <GPRS.h>
#include <GSM.h>
#include <SMS.h>
#include <GSMLOC.h>
#include <HTTP.h>
#include <IP.h>
#include <URC.h>
#include <SIMCOM.h>
#include "Config.h"

#define SMS_DEVICE '0'
#define SMS_DATA '1'
#define SMS_USSD '2'
#define SMS_GET_NUMBER '3'

struct APN {
  char oper[6];
  char apn[15];
  char user[15];
  char pwd[15];
};

struct UssdRequester {
  bool isProcessing;
  unsigned long millisProcessed;
  char address[17];
};

struct BlumbangReksaDevice {
  char code[9];
  char imei[17];
  char phoneNumber[17];
  float inputVoltage;
  unsigned char networkSignal;
  char latitude[12];
  char longitude[12];
  char timeStamp[26];
};

struct BlumbangReksaData {
  char deviceCode[9];
  char deviceImei[17];
  char timeStamp[26];
#if DHT_USED
  float airTemperature;
  float humidity;
#endif
  float temperature;
#if DO_SENSOR_SERIAL_ENABLE
  float DOPercentage;
  float DO;
#endif
#if EC_SENSOR_SERIAL_ENABLE
  float EC;
  float TDS;
  float salinity;
  float specificGravityOfSeaWater;
#endif
#if PH_SENSOR_SERIAL_ENABLE
  float pH;
#endif
#if ORP_SENSOR_SERIAL_ENABLE
  float orp;
#endif
};

struct AsyncFlag {
  bool smsGeneral;
  bool getLocation;
  bool waiting;
  bool openConnection;
};

struct AsyncMessage {
  char destinationNumber[17];
  char messageData[163];
  unsigned char typeOfSms;
};

struct SmsUnknown {
  bool updated;
  char deviceCode[9];
  char address[17];
  uint16_t year;
  uint8_t month;
  uint8_t date;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  int timezone;
};

class Communication {
 private:
  static bool setup_run;
  static bool sleep;
  static bool sleepBuffer;

 public:
  static void setup(void);
  static void loop(void);

  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);

  class Cellular;
};

class Communication::Cellular {
 private:
  static bool sleep;
  static bool sleepBuffer;
  static unsigned long lastReset;

#if GSM_SERIAL_TYPE == SOFTWARE_SERIAL
  static SoftwareSerial SSerial;
#endif
  static DTE dte;
  static GSM gsm;
  static SMS sms;
  static GPRS gprs;
  static IP ip;
  static GSMLOC gsmloc;
  static HTTP http;
  static SIMCOM simcom;

  static char imei[17];
  static char apiToken[18];
  static bool gprsAttach;
  static char cardName[12];
  static struct SubscriberNumber subscriberNumber;
  static struct NetworkRegistration networkRegistration;
  static struct SignalQuality signalQuality;
  static struct BatteryStatus batteryStatus;
  static struct Clock clock;
  static struct Operator oper;
  static struct ConnStatus connStatus;
  static struct LocationTime locationTime;

  static struct APN apn;

  static struct Message smsMessage;
  static struct SmsUnknown smsUnknown;

  static struct UssdRequester ussdRequester;
  static bool allowToConnect;

  static struct AsyncFlag asyncFlag;
  static struct AsyncMessage asyncMessage;

  static bool asyncTask(void);

  static void gsmTask(void);
  static void smsTask(void);
  class Internet;

  static void sendSMSAsync(const char destination[], const char message[]);
  static bool getLocationAsync(void);
  static bool openConnectionAsync(unsigned char cid = 1);
  static void resetProperties(void);

  static const char *urlEncode(char buffer[], const char param[]);
  static const char *param(char buffer[], struct BlumbangReksaDevice blumbangReksaDevice);
  static const char *param(char buffer[], struct BlumbangReksaData blumbangReksaData);
  static const char *param(char buffer[], struct SmsUnknown smsUnknown);
  static char *message(char buffer[], struct BlumbangReksaDevice blumbangReksaDevice);
  static char *message(char buffer[], struct BlumbangReksaData blumbangReksaData);
  static const char *implode(char buffer[], const char *glue, const struct BlumbangReksaDevice *blumbangReksaDevice);
  static const char *implode(char buffer[], const char *glue, const struct BlumbangReksaData *blumbangReksaData);
  static void setDateTimeMysqlFormat(const char dateTime[]);
  static const char *getDateTimeMySQLFormat(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second);
  static const char *getDateTimeAtomFormat(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, int timezone);

 public:
  static const char *getImei(void) { return imei; };
  static bool isGprsAttach(void) { return gprsAttach; };
  static unsigned char getNetworkRegistrationStatus(void) { return networkRegistration.status; };
  static unsigned char getSignalQualityRssi(void) { return signalQuality.rssi; };
  static unsigned char getSignalQualityPercentage(void) { return signalQuality.rssi * 3.125; };
  static float getBatteryStatusVoltage(void) { return batteryStatus.voltage; };
  static unsigned char getBatteryStatusCapacityLevel(void) { return batteryStatus.capacityLevel; };
  static const char *getSubscriberNumber(void) { return subscriberNumber.number; };
  static const char *getOperator(void) { return oper.oper; };
  static const char *getCardName(void) { return cardName;};
  static const char *getClockTimestamp(void) { return clock.timestamp; };
  static unsigned char getConnectionStatus(void) { return connStatus.status; };
  static const char *getLatitude(void);
  static const char *getLongitude(void);
  static bool isSubcriberNumberEmpty(void);
  static bool getNumberFromJack(void);

  static bool openConnection(void);

  static bool queueBlumbangReksaDevice(struct BlumbangReksaDevice blumbangReksaDevice);
  static struct BlumbangReksaDevice readQueuedBlumbangReksaDevice(void);
  static void updateReadBlumbangReksaDeviceIndex(void);
  static int totalQueuedBlumbangReksaDevice(void);
  static void clearQueuedBlumbangReksaDevice(void);

  static bool queueBlumbangReksaData(struct BlumbangReksaData blumbangReksaData);
  static struct BlumbangReksaData readQueuedBlumbangReksaData(void);
  static void updateReadBlumbangReksaDataIndex(void);
  static int totalQueuedBlumbangReksaData(void);
  static void clearQueuedBlumbangReksaData(void);

  static void setAllowToConnect(bool allow) { allowToConnect = allow; };
  static bool isAllowToConnect(void) { return allowToConnect; };

  static void setup(void);
  static void loop(void);

  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);
};

class Communication::Cellular::Internet {
public:
  static void httpTask(void);
  static char* getDataJson(char* json, char* param);
  static void removeChar(char *str, char garbage);
  static char*  get_token(char *str, char *delimiter);

  static void setup(void);
  static void loop(void);
};
#endif
