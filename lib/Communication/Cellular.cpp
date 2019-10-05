#include "Communication.h"
#include "Config.h"
#include "Indicating.h"
#include "Storage.h"
#include "System.h"
#include "lang.h"
#include "apnList.h"



char Communication::Cellular::cardName[12]="";
struct Message Communication::Cellular::smsMessage;

bool Communication::Cellular::sleep = false;
bool Communication::Cellular::sleepBuffer = false;

unsigned long Communication::Cellular::lastReset = millis();

#if GSM_SERIAL_TYPE == SOFTWARE_SERIAL
SoftwareSerial Communication::Cellular::SSerial(GSM_SERIAL_RX_PIN, GSM_SERIAL_TX_PIN);
#endif

DTE Communication::Cellular::dte(GSM_SERIAL, GSM_POWER_PIN, GSM_SERIAL_DEBUG);
GSM Communication::Cellular::gsm(dte);
SMS Communication::Cellular::sms(dte);
GPRS Communication::Cellular::gprs(dte);
IP Communication::Cellular::ip(dte, gprs);
GSMLOC Communication::Cellular::gsmloc(dte, ip);
HTTP Communication::Cellular::http(dte, ip);
SIMCOM Communication::Cellular::simcom(dte);

char Communication::Cellular::imei[17]="";
bool Communication::Cellular::gprsAttach = false;
struct SubscriberNumber Communication::Cellular::subscriberNumber = (struct SubscriberNumber){"", "", 0, 0, 0};
struct NetworkRegistration Communication::Cellular::networkRegistration = (struct NetworkRegistration){0, 3, "", ""};
struct SignalQuality Communication::Cellular::signalQuality = (struct SignalQuality){0, 0};
struct BatteryStatus Communication::Cellular::batteryStatus = (struct BatteryStatus){false, 0, 0.0f};
struct Clock Communication::Cellular::clock = (struct Clock){"", 15, 1, 1, 0, 0, 0, 0};
struct Operator Communication::Cellular::oper = (struct Operator){0, 0, "", ""};
struct ConnStatus Communication::Cellular::connStatus = (struct ConnStatus){0, ""};
struct LocationTime Communication::Cellular::locationTime = (struct LocationTime){0, "", "", "", ""};

struct APN Communication::Cellular::apn = (struct APN){"", "", "", ""};

struct SmsUnknown Communication::Cellular::smsUnknown = (struct SmsUnknown){false, "", "", 0, 0, 0, 0, 0, 0, 0};

struct UssdRequester Communication::Cellular::ussdRequester = (struct UssdRequester){false, 0, ""};

bool Communication::Cellular::allowToConnect = true;

struct AsyncFlag Communication::Cellular::asyncFlag = (struct AsyncFlag){false, false, false, false};

struct AsyncMessage Communication::Cellular::asyncMessage = (struct AsyncMessage){"", "", ' '};

bool Communication::Cellular::isSleep(void) {
  return sleep;
}

bool Communication::Cellular::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void Communication::Cellular::setSleep(bool sleep) {
  sleepBuffer = sleep;
  //if(sleepBuffer)
  //  gsm.turnOffRadio();
  //else
  //  gsm.turnOnRadio();
}

void Communication::Cellular::setup(void) {
  pinMode(GSM_REGULATOR_PIN, OUTPUT);
  digitalWrite(GSM_REGULATOR_PIN,HIGH);
  pinMode(GSM_POWER_PIN, OUTPUT);
  digitalWrite(GSM_POWER_PIN, LOW);
 // delay(750);
 // digitalWrite(GSM_POWER_PIN, HIGH);
#if GSM_SERIAL_TYPE == SOFTWARE_SERIAL
  pinMode(GSM_SERIAL_RX_PIN, INPUT_PULLUP);
  pinMode(GSM_SERIAL_TX_PIN, OUTPUT);
  digitalWrite(GSM_SERIAL_TX_PIN, HIGH);
#endif
  //pinMode(GSM_SERIAL_RX_PIN, INPUT_PULLUP);
  //pinMode(GSM_SERIAL_TX_PIN, OUTPUT);
  GSM_SERIAL.begin(GSM_SERIAL_BAUDRATE);
  
  
  //Internet::setup();

  clearQueuedBlumbangReksaDevice();
  if (Storage::Eeprom::getBlumbangReksaDataQueueIndex() < 0 ||
      Storage::Eeprom::getBufferSizeBlumbangReksaDataQueue() <= Storage::Eeprom::getBlumbangReksaDataQueueIndex() ||
      Storage::Eeprom::getBlumbangReksaDataReadIndex() < 0 ||
      Storage::Eeprom::getBufferSizeBlumbangReksaDataQueue() <= Storage::Eeprom::getBlumbangReksaDataReadIndex() ||
      Storage::Eeprom::getBlumbangReksaDataQueueIndex() == Storage::Eeprom::getBlumbangReksaDataReadIndex()) {
    clearQueuedBlumbangReksaData();
  }

#if GSM_SERIAL_TYPE == SOFTWARE_SERIAL
  if (!dte.isListening()) dte.listen();
#endif

  dte.powerReset();
  resetProperties();
  delay(1000);
  simcom.setModeGetTimestamp(true);
  Urc.newMessage.message = &smsMessage;
  //return;
  //sprintf_P(cardName, (const char *)F(""));
  //strcpy(cardName, gsm.getSIMCARDName());
 // gprsAttach = gprs.isAttached();
 // signalQuality = gsm.getSignalQuality();
  ////delay(250);
  //strcpy(imei, dte.getProductSerialNumberIdentification());
}

#define timeoutGetSubsnum       10000
void Communication::Cellular::loop(void) {
#if GSM_SERIAL_TYPE == SOFTWARE_SERIAL
  /* Active SoftwareSerial if isn't */
  if (!dte.isListening()) dte.listen();
#endif

  if (sleepBuffer != sleep) {
    if (sleep) 
    {
      sleep = sleepBuffer;
      resetProperties();
      setAllowToConnect(true);
    } 
    else 
    {
      if (totalQueuedBlumbangReksaDevice() == 0 && totalQueuedBlumbangReksaData() == 0) {
        if (!dte.isPowerDown())
        {
          dte.togglePower();
          wdt_reset();
        } 
        resetProperties();
        sleep = sleepBuffer;
      }
    }
  }
  if (sleep) 
  {
    //digitalWrite(GSM_REGULATOR_PIN,LOW);
    return;
  }
  

  if (asyncTask()) return;

  if (!dte.AT()) {
    wdt_reset();
    resetProperties();
    return;
  }

  //bool registeredOnNetwork = (networkRegistration.status == 1) | (networkRegistration.status == 5);

  /* If *PSUTTZ received */
  if (Urc.psuttz.updated) {
    clock = gsm.getClock();
    Indicating::RTC::setDateTimeGSMFormat(getClockTimestamp());
    Urc.psuttz.updated = false;
    gsmloc.getLocationTime(1);
  }
  
  static unsigned long tMain = millis();
  if (!gprsAttach && (millis() - tMain > 120000)) {   // default 120000
    setAllowToConnect(false);
    //gprs.dettached();
  } else if (gprsAttach) {
    tMain = millis();
  }

    //Serial.println((const char*)F("==============================================%s")+String(apn.apn));
#if SYSTEM_DEBUG
  //if(strlen(apn.apn) > 0)
    //Serial.println((const char*)F("==============================================%s")+String(apn.apn));
#endif
  if (strlen(apn.apn) == 0) 
  {
    if ((networkRegistration.status == 1 || networkRegistration.status == 5) && strlen(oper.operNumeric) > 0) 
    {
      sms.newMessageToURC(true);
      strcpy(cardName,gsm.getSIMCARDName());
      if (strlen(subscriberNumber.number) == 0) subscriberNumber = gsm.getSubscriberNumber();
      if( ( (strlen(cardName)>3) && ( (strcmp_P(cardName,(const char *)F("Hologram")) ) == 0  || (strcmp_P(cardName,(const char *)F("hologram")) ) == 0) )|| 
      (strcmp_P(subscriberNumber.number, (const char *)F("0")) == 0) ) 
      {
        strcpy_P(apn.apn, (const char *)F("hologram"));
        strcpy_P(apn.user, (const char *)F(""));
        strcpy_P(apn.pwd, (const char *)F(""));
        //ip.setConnectionParamGprs(apn.apn, apn.user, apn.pwd);
      } 
      else 
      {
        const char *operNumeric = oper.operNumeric;
        for (size_t i = 0; i < (sizeof(apnDatabase) / sizeof(struct APN)); i++) {
          if (strcmp_P(operNumeric, (const char *)apnDatabase[i].oper) == 0) 
          {
            memcpy_P(&apn, &apnDatabase[i], sizeof(struct APN));
            break;
          }
        }
      }
    }
    
    ip.setConnectionParamGprs(apn.apn, apn.user, apn.pwd);
  } else {
    /* TODO: Move to Communication.cpp */
    if (gprsAttach && isAllowToConnect()) {
      if ((totalQueuedBlumbangReksaData() > 0 || totalQueuedBlumbangReksaDevice() > 0 || (strlen(locationTime.latitude) == 0 && strlen(locationTime.longitude) == 0) || smsUnknown.updated) && connStatus.status == 3) 
      {
        ip.setConnectionParamGprs(apn.apn, apn.user, apn.pwd);
        ip.openConnection();
        asyncFlag.openConnection = true;
      } else if (!(strlen(locationTime.latitude) == 0 && strlen(locationTime.longitude) == 0) && totalQueuedBlumbangReksaData() == 0 && totalQueuedBlumbangReksaDevice() == 0 && connStatus.status == 1 && !smsUnknown.updated) {
        ip.closeConnection();
      }
    }
  }
  if (strlen(locationTime.latitude) == 0 && strlen(locationTime.longitude) == 0 && connStatus.status == 1) {
    asyncFlag.getLocation = true;
  }

  Internet::loop();
  smsTask();
  gsmTask();

  if (strlen(oper.oper) == 0 || strlen(oper.operNumeric) == 0) {
    oper = gsm.getOperator(1);
    if (strlen(oper.oper) == 0) {
      oper = gsm.getOperator(0);
    }
  }


gsm.updateNetworkStatus();
networkRegistration = gsm.getNetworkReg();
bool registeredOnNetwork = (networkRegistration.status == 1) || (networkRegistration.status == 5);

//Serial.println("===============on network register : "+String(registeredOnNetwork)?"registered":"not registered");
/*Serial.println("net status : "+String(networkRegistration.status)+
                    "\tgprs status: "+String(gprsAttach)+
                    "\t signal quality : "+String(signalQuality.rssi));
   //networkRegistration = gsm.getNetworkRegistration();*/
  if (registeredOnNetwork)
  {
    gprsAttach    = gsm.getGPRSState();//gprs.isAttached();
    signalQuality = gsm.getSignalQual();
    connStatus    = ip.getConnectionStatus();
  }  
  else
  //if(!registeredOnNetwork)
  {
    signalQuality = (struct SignalQuality){0, 0};
    gprsAttach    = false;
    return;
  }  
  
  if (strlen(subscriberNumber.number) == 0) {
    static unsigned long startTimeGetSubs = millis();
    if(millis() - startTimeGetSubs >= timeoutGetSubsnum )
    {
      subscriberNumber = gsm.getSubscriberNumber();
      startTimeGetSubs = millis();
    }
    //if (gsm.getNetworkRegistration().status == 1 || gsm.getNetworkRegistration().status == 5) getNumberFromJack();
    if(registeredOnNetwork) getNumberFromJack();
  }
  if (strlen(imei) == 0) 
  {
    strcpy(imei, dte.getProductSerialNumberIdentification());
    //Serial.println("imei : "+String(imei));
  }
}

void Communication::Cellular::sendSMSAsync(const char *destination, const char *message) {
  const __FlashStringHelper *command = F("AT+CMGS=\"%s\"\r");
  char buffer[12 + strlen(destination)];  // "AT+CMGS=\"{destination}\"\r"

  sprintf_P(buffer, (const char *)command, destination);

  if (sms.isTextMode()) {
    sms.selectSMSFormat(true);
  }

  dte.clearReceivedBuffer();
  if (!dte.ATCommand(buffer)) return;
  if (!dte.ATResponseContain("> ")) return;
  unsigned char length = 0;
  for (size_t i = 0; i < strlen(message) && length < 160; i++) {
    if (message[i] != '\r') {
      dte.write(message[i]);
      length++;
    }
  }
  dte.write('\x1A');
  if (dte.isEcho()) {
    dte.flush();
    dte.ATResponse(10);
  }
}

bool Communication::Cellular::getLocationAsync(void) {
  const __FlashStringHelper *command = F("AT+CIPGSMLOC=%d,%d\r");
  char buffer[18];  // "AT+CIPGSMLOC=X,X\r"

  sprintf_P(buffer, (const char *)command, 1, 1);
  dte.clearReceivedBuffer();
  if (!dte.ATCommand(buffer))
    return false;
  dte.setFlowControlStatusDce(true);
  return true;
}

bool Communication::Cellular::openConnectionAsync(unsigned char cid) {
  ip.setConnectionParamGprs(apn.apn, apn.user, apn.pwd);
  connStatus = ip.getConnectionStatus(cid);

  if (connStatus.status == 3) {
    if (!gprsAttach) return false;
    const __FlashStringHelper *command = F("AT+SAPBR=%d,%d\r");
    char buffer[20];
    sprintf_P(buffer, (const char *)command, 1, cid);
    if (!dte.ATCommand(buffer)) return false;
    dte.setFlowControlStatusDce(true);
    return false;
  } else if (connStatus.status >= 2)
    return false;
  return true;
}

void Communication::Cellular::resetProperties(void) {
  gprsAttach = false;
  oper = (struct Operator){0, 0, "", ""};
  networkRegistration = (struct NetworkRegistration){0, 3, "", ""};
  signalQuality = (struct SignalQuality){0, 0};
  batteryStatus = (struct BatteryStatus){false, 0, 0.0f};
  connStatus = (struct ConnStatus){0, ""};
  apn = (struct APN){"", "", "", ""};
  ussdRequester = (struct UssdRequester){false, 0, ""};

  lastReset = millis();
}

  const char *Communication::Cellular::urlEncode(char encoded[], const char param[]) {
  const __FlashStringHelper *specials = F("$&+,/:;=?@ <>#%{}|~[]`"); /* String containing chars you want encoded */
  const __FlashStringHelper *hexDigit = F("0123456789ABCDEF");
  char hexDigitBuffer[17];

  size_t j = 0;
  strcpy_P(hexDigitBuffer, (const char *)hexDigit);
  for (size_t i = 0; i < strlen(param); i++) {
    char c = param[i];
    if (strchr_P((const char *)specials, c)) {
      encoded[j++] = '%';
      encoded[j++] = hexDigitBuffer[(c >> 4) & 0x0F];
      encoded[j++] = hexDigitBuffer[c & 0x0F];
    } else
      encoded[j++] = c;
  }
  encoded[j] = '\0';

  return encoded;
}

const char *Communication::Cellular::param(char buffer[], struct BlumbangReksaDevice blumbangReksaDevice) {
  char valueBuffer[10];
  strcpy_P(buffer, (const char *)F(""));
  strcat_P(buffer, (const char *)F("voltage="));
  strcat(buffer, dtostrf(blumbangReksaDevice.inputVoltage, 1, 2, valueBuffer));
  strcat_P(buffer, (const char *)F("&signal_quality="));
  strcat(buffer, itoa(blumbangReksaDevice.networkSignal, valueBuffer, 10));
  strcat_P(buffer, (const char *)F("&latitude="));
  strcat(buffer, blumbangReksaDevice.latitude);
  strcat_P(buffer, (const char *)F("&longitude="));
  strcat(buffer, blumbangReksaDevice.longitude);
  char urlEncoded[36];
  if (strlen(blumbangReksaDevice.phoneNumber) > 0) {
    strcat_P(buffer, (const char *)F("&phone_number="));
    strcat(buffer, urlEncode(urlEncoded, blumbangReksaDevice.phoneNumber));
  }
  strcat_P(buffer, (const char *)F("&version="));
  strcat_P(buffer, (const char *)F(VERSION));
  strcat_P(buffer, (const char *)F("&logged_at="));
  strcat(buffer, urlEncode(urlEncoded, blumbangReksaDevice.timeStamp));
  //strcat(buffer,"\n");
  return buffer;
}

const char *Communication::Cellular::param(char buffer[], struct BlumbangReksaData blumbangReksaData) {
  char valueBuffer[10];
  strcpy_P(buffer, (const char *)F(""));
  if(!isnan(blumbangReksaData.temperature))
  {
    strcat_P(buffer, (const char *)F("temperature="));
    strcat(buffer, dtostrf(blumbangReksaData.temperature, 2, 2, valueBuffer));
  }
#if DO_SENSOR_SERIAL_ENABLE
  if(!isnan(blumbangReksaData.DO))
  {
    strcat_P(buffer, (const char *)F("&do="));
    if (blumbangReksaData.DO == 0.00f)
      strcat(buffer, dtostrf(blumbangReksaData.DO, 1, 0, valueBuffer));
    else
      strcat(buffer, dtostrf(blumbangReksaData.DO, 1, 2, valueBuffer));
  }
  if(!isnan(blumbangReksaData.DOPercentage))
  {
    strcat_P(buffer, (const char *)F("&do_percent="));
    strcat(buffer, dtostrf(blumbangReksaData.DOPercentage, 1, 2, valueBuffer));
  }
#endif
#if EC_SENSOR_SERIAL_ENABLE
  if(!isnan(blumbangReksaData.EC))
  {
    strcat_P(buffer, (const char *)F("&ec="));
    if (blumbangReksaData.EC == 0.00f)
      strcat(buffer, dtostrf(blumbangReksaData.EC, 1, 0, valueBuffer));
    else
      strcat(buffer, dtostrf(blumbangReksaData.EC, 1, 2, valueBuffer));
    strcat_P(buffer, (const char *)F("&tds="));
    if (blumbangReksaData.TDS == 0.00f)
        strcat(buffer, dtostrf(blumbangReksaData.TDS, 1, 0, valueBuffer));
    else
        strcat(buffer, dtostrf(blumbangReksaData.TDS, 1, 2, valueBuffer));
    strcat_P(buffer, (const char *)F("&salinity="));
    strcat(buffer, dtostrf(blumbangReksaData.salinity, 1, 2, valueBuffer));
    strcat_P(buffer, (const char *)F("&gravity="));
    strcat(buffer, dtostrf(blumbangReksaData.specificGravityOfSeaWater, 1, 3, valueBuffer));
  }
#endif
#if PH_SENSOR_SERIAL_ENABLE
  strcat_P(buffer, (const char *)F("&ph="));
  strcat(buffer, dtostrf(blumbangReksaData.pH, 1, 2, valueBuffer));
#endif
   strcat_P(buffer, (const char *)F("&api_token="));
   strcat(buffer, apiToken);
  char urlEncoded[36];
  strcat_P(buffer, (const char *)F("&measured_at="));
  strcat(buffer, urlEncode(urlEncoded, blumbangReksaData.timeStamp));
  return buffer;  //212
}

const char *Communication::Cellular::param(char buffer[], SmsUnknown smsUnknown) {
  strcpy_P(buffer, (const char *)F(""));
  strcat_P(buffer, (const char *)F("&imei="));
  char urlEncoded[36];
  strcat(buffer, urlEncode(urlEncoded, imei));
  strcat_P(buffer, (const char *)F("&address="));
  strcat(buffer, urlEncode(urlEncoded, smsUnknown.address));
  strcat_P(buffer, (const char *)F("&timestamp="));
  strcat(buffer, urlEncode(urlEncoded, getDateTimeAtomFormat(smsUnknown.year, smsUnknown.month, smsUnknown.date, smsUnknown.hour, smsUnknown.minute, smsUnknown.second, smsUnknown.timezone)));
  strcat_P(buffer, (const char *)F("&data="));
  strcat(buffer, urlEncode(urlEncoded, Urc.newMessage.message->data));
  return buffer;
}

char *Communication::Cellular::message(char *buffer, struct BlumbangReksaDevice blumbangReksaDevice) {
  char valueBuffer[10];
  strcpy_P(buffer, (const char *)F("ACT=J00\n"));
  strcat_P(buffer, (const char *)F("CODE="));
  strcat(buffer, blumbangReksaDevice.code);
  strcat_P(buffer, (const char *)F("\nIMEI="));
  strcat(buffer, blumbangReksaDevice.imei);
  strcat_P(buffer, (const char *)F("\nFIRMWARE="));
  strcat_P(buffer, (const char *)F(VERSION));
  strcat_P(buffer, (const char *)F("\nVOLTAGE="));
  strcat(buffer, dtostrf(blumbangReksaDevice.inputVoltage, 1, 2, valueBuffer));
  strcat_P(buffer, (const char *)F("\nSIGNAL="));
  strcat(buffer, itoa(blumbangReksaDevice.networkSignal, valueBuffer, 10));
  strcat_P(buffer, (const char *)F("\nLATITUDE="));
  strcat(buffer, blumbangReksaDevice.latitude);
  strcat_P(buffer, (const char *)F("\nLONGITUDE="));
  strcat(buffer, blumbangReksaDevice.longitude);
  strcat_P(buffer, (const char *)F("\nPHONE="));
  strcat(buffer, blumbangReksaDevice.phoneNumber);
  strcat_P(buffer, (const char *)F("\nLOG="));
  strcat(buffer, blumbangReksaDevice.timeStamp);
  return buffer;  //122
}

char *Communication::Cellular::message(char buffer[], struct BlumbangReksaData blumbangReksaData) {
  char valueBuffer[10];
  strcpy_P(buffer, (const char *)F("ACT=J01\n"));
  strcat_P(buffer, (const char *)F("CODE="));
  strcat(buffer, blumbangReksaData.deviceCode);
  strcat_P(buffer, (const char *)F("\nIMEI="));
  strcat(buffer, blumbangReksaData.deviceImei);
  strcat_P(buffer, (const char *)F("\nTIMESTAMP="));
  strcat(buffer, blumbangReksaData.timeStamp);
  strcat_P(buffer, (const char *)F("\nT="));
  strcat(buffer, dtostrf(blumbangReksaData.temperature, 1, 2, valueBuffer));
#if DO_SENSOR_SERIAL_ENABLE
  strcat_P(buffer, (const char *)F("\nDO_PERCENT="));
  strcat(buffer, dtostrf(blumbangReksaData.DOPercentage, 1, 2, valueBuffer));
  strcat_P(buffer, (const char *)F("\nDO="));
  strcat(buffer, dtostrf(blumbangReksaData.DO, 1, 2, valueBuffer));
#endif
#if EC_SENSOR_SERIAL_ENABLE
  strcat_P(buffer, (const char *)F("\nEC="));
  strcat(buffer, dtostrf(blumbangReksaData.EC, 1, 2, valueBuffer));
  strcat_P(buffer, (const char *)F("\nTDS="));
  strcat(buffer, dtostrf(blumbangReksaData.TDS, 1, 2, valueBuffer));
  strcat_P(buffer, (const char *)F("\nSAL="));
  strcat(buffer, dtostrf(blumbangReksaData.salinity, 1, 2, valueBuffer));
  strcat_P(buffer, (const char *)F("\nG="));
  strcat(buffer, dtostrf(blumbangReksaData.specificGravityOfSeaWater, 1, 3, valueBuffer));
#endif
#if PH_SENSOR_SERIAL_ENABLE
  strcat_P(buffer, (const char *)F("\nPH="));
  strcat(buffer, dtostrf(blumbangReksaData.pH, 1, 2, valueBuffer));
#endif
  return buffer;  //153
}

void Communication::Cellular::setDateTimeMysqlFormat(const char dateTime[]) {
  char dateTimeBuffer[strlen(dateTime) + 1];

  strcpy(dateTimeBuffer, dateTime);
  smsUnknown.year   = (uint16_t) atoi(strtok_P(dateTimeBuffer,(const char*) F("/,:+-"))) + 2000;
  smsUnknown.month  = (uint8_t) atoi(strtok_P(NULL, (const char*)F("/,:+-")));
  smsUnknown.date   = (uint8_t) atoi(strtok_P(NULL, (const char*)F("/,:+-")));
  smsUnknown.hour   = (uint8_t) atoi(strtok_P(NULL, (const char*)F("/,:+-")));
  smsUnknown.minute = (uint8_t) atoi(strtok_P(NULL, (const char*)F("/,:+-")));
  smsUnknown.second = (uint8_t) atoi(strtok_P(NULL, (const char*)F("/,:+-")));
  smsUnknown.timezone = atoi(dateTime + 17) / 4;
}

const char *Communication::Cellular::getDateTimeMySQLFormat(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second) {
  static char dateTimeBuffer[20] = "";
  strcpy_P(dateTimeBuffer, (const char *)F(""));
  sprintf_P(dateTimeBuffer, (const char *)F("%04d-%02d-%02d %02d:%02d:%02d"), year, month, date, hour, minute, second);
  return dateTimeBuffer;
}

const char *Communication::Cellular::getDateTimeAtomFormat(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second, int timezone) {
  static char dateTimeBuffer[26] = "";
  strcpy_P(dateTimeBuffer, (const char *)F(""));
  sprintf_P(dateTimeBuffer,(const char *)F("%04d-%02d-%02dT%02d:%02d:%02d%c%02d:00"), year, month, date, hour, minute, second, timezone >= 0 ? '+':'-', timezone);
  return dateTimeBuffer;
}

bool Communication::Cellular::asyncTask() {
  static volatile bool smsData, smsDevice, smsUssd, smsGetNumber, getLocation, openConnection;
  static unsigned long startWaiting = millis();

  if (asyncFlag.smsGeneral) 
  {
    if (asyncMessage.typeOfSms == SMS_DEVICE) 
    {
      asyncFlag.waiting = true;
      startWaiting = millis();
      smsDevice = true;
    } else if (asyncMessage.typeOfSms == SMS_DATA) {
      asyncFlag.waiting = true;
      startWaiting = millis();
      smsData = true;
    } else if (asyncMessage.typeOfSms == SMS_USSD) {
      asyncFlag.waiting = true;
      startWaiting = millis();
      smsUssd = true;
    } else if (asyncMessage.typeOfSms == SMS_GET_NUMBER) {;
      asyncFlag.waiting = true;
      startWaiting = millis();
      smsGetNumber = true;
    }
    wdt_reset();
    sendSMSAsync(asyncMessage.destinationNumber, asyncMessage.messageData);
    asyncFlag.smsGeneral = false;
  } else if (asyncFlag.openConnection) {
    wdt_reset();
    openConnectionAsync();
    asyncFlag.openConnection = false;
    asyncFlag.waiting = true;
    openConnection = true;
    startWaiting = millis();
  } else if (asyncFlag.getLocation) {
    wdt_reset();
    getLocationAsync();
    asyncFlag.getLocation = false;
    asyncFlag.waiting = true;
    getLocation = true;
    startWaiting = millis();
  }

  if (asyncFlag.waiting) 
  {
    if (dte.available() || strcmp_P(dte.getResponse(), (const char *) F("ERROR")) == 0) 
    {
      wdt_reset();
      if (dte.available()) dte.ATResponse();
      if (dte.isResponseContain(F("+CMGS: "))) {
        dte.ATResponseOk();
        if (smsDevice) {
          updateReadBlumbangReksaDeviceIndex();
          smsDevice = false;
        } else if (smsData) {
          updateReadBlumbangReksaDataIndex();
          smsData = false;
        } else if (smsUssd) {
          smsUssd = false;
        } else if (smsGetNumber) {
          smsGetNumber = false;
        }
      } 
      else if (dte.isResponseOk()) 
      {
        wdt_reset();
        openConnection = false;
        connStatus = ip.getConnectionStatus();
      } else if (dte.isResponseContain(F("+CIPGSMLOC: "))) {
        const __FlashStringHelper *response = F("+CIPGSMLOC: ");
        char *pointer = strstr_P(dte.getResponse(), (const char *)response) + strlen_P((const char *)response);
        char *str = strtok_P(pointer, (const char*)F(","));
        size_t i;
        for (i = 0; i < 5 && str != NULL; i++) {
          if (i == 0) locationTime.locationCode = (unsigned int)atol(str);
          if (i == 1) strcpy(locationTime.longitude, str);
          if (i == 2) strcpy(locationTime.latitude, str);
          if (i == 3) strcpy(locationTime.date, str);
          if (i == 4) strcpy(locationTime.time, str);
          str = strtok_P(NULL, (const char*)F(","));
        }
        if (i < 5) locationTime = (struct LocationTime){locationTime.locationCode, "110.417002", "-7.671272", "", ""};
        dte.ATResponseOk();
        getLocation = false;
      } else if (dte.isResponseContain(F("ERROR"))) {
        if (smsData || smsDevice || smsUssd) {
          if (smsDevice) {
            struct BlumbangReksaDevice blumbangReksaDevice = readQueuedBlumbangReksaDevice();
            char buffer[96];
            implode(buffer, ",", &blumbangReksaDevice);
            updateReadBlumbangReksaDeviceIndex();
          }
          if (smsData) {
            struct BlumbangReksaData blumbangReksaData = readQueuedBlumbangReksaData();
            char buffer[128];
            implode(buffer, ",", &blumbangReksaData);
            updateReadBlumbangReksaDataIndex();
          }
          Indicating::Display::info(Indicating::Translation::get(FAILED_RESTART));
          if (dte.AT()) dte.togglePower();
          //delay(1000);
        } else if (openConnection) {
          static unsigned char tryToConnect = 0;
          tryToConnect++;
          if (tryToConnect >= 4) {
            Indicating::Display::info(Indicating::Translation::get(CONNECTION_FAILED));
            strcpy_P(locationTime.latitude, (const char *)F("-7.671272"));
            strcpy_P(locationTime.longitude, (const char *)F("110.417002"));
            setAllowToConnect(false);
            resetProperties();
            tryToConnect = 0;
           // delay(500);
          }
        } else if (getLocation) {
          strcpy_P(locationTime.latitude, (const char *)F("-7.671272"));
          strcpy_P(locationTime.longitude, (const char *)F("110.417002"));
        }
      }
      asyncFlag.waiting = false;
      asyncFlag.smsGeneral = false;
      asyncFlag.getLocation = false;
      asyncFlag.openConnection = false;
      smsDevice = smsData = smsUssd = getLocation = openConnection = smsGetNumber = false;
    } 
    else if (millis() - startWaiting > 60000) {   // default 60000
      wdt_reset();
      if (openConnection) {
        connStatus = ip.getConnectionStatus();
        if (connStatus.status == 3) {
          static unsigned char tryToConnect = 0;
          tryToConnect++;
          if (tryToConnect >= 3) {
            setAllowToConnect(false);
            tryToConnect = 0;
          }
        }
      }
      if (getLocation) {
        strcpy_P(locationTime.latitude, (const char *)F("-7.671272"));
        strcpy_P(locationTime.longitude, (const char *)F("110.417002"));
      }
      asyncFlag.waiting = false;
      asyncFlag.smsGeneral = false;
      asyncFlag.getLocation = false;
      asyncFlag.openConnection = false;
      smsDevice = smsData = smsUssd = getLocation = openConnection = smsGetNumber = false;
    }
  }
wdt_reset();
  if (dte.isPowerDown() && !asyncFlag.waiting) {
    static unsigned char stateMachine = 0;
    static unsigned long powerDownMillis = 0;
    if (stateMachine == 0) {
      digitalWrite(GSM_POWER_PIN, HIGH);
      powerDownMillis = millis();
      stateMachine = 1;
      return true;
    } else if (stateMachine == 1) {
      if (millis() - powerDownMillis > 1200 && digitalRead(GSM_POWER_PIN)) {
        digitalWrite(GSM_POWER_PIN, LOW);
        powerDownMillis = millis();
      } else if (millis() - powerDownMillis > 5000) {
        stateMachine = 2;
      }
      return true;
    } else if (stateMachine == 2) {
      dte.clearReceivedBuffer();
      if (dte.AT()) {
        Urc.resetUnsolicitedResultCode();
        if (dte.isEcho())
          dte.setEcho(false);
        if (dte.getFlowControl().dce == 0)
          dte.setFlowControl(1, 0);
        dte.setFlowControlStatusDce(false);
      } else {
        stateMachine = 0;
        return true;
      }
      stateMachine = 0;
      resetProperties();
      return false;
    }
  }

  return asyncFlag.waiting;
}

#define timeout  60000
void Communication::Cellular::gsmTask(void) {
  //static unsigned long timeout = 60000;
  wdt_reset();
  if (Urc.serviceDataIndication.updated) {
    Urc.serviceDataIndication.updated = false;
    asyncFlag.smsGeneral = true;
    strcpy(asyncMessage.destinationNumber, ussdRequester.address);
    strcpy(asyncMessage.messageData, Urc.serviceDataIndication.str);
    asyncMessage.typeOfSms = SMS_USSD;
    if (Urc.serviceDataIndication.n == 2) {
      gsm.cancelServiceData();
      ussdRequester.isProcessing = false;
      strcpy_P(ussdRequester.address, (const char *)F(""));
    }
  }

  if (ussdRequester.isProcessing == true) {
    if (millis() - ussdRequester.millisProcessed > timeout) {
      gsm.cancelServiceData();
      ussdRequester.isProcessing = false;
      strcpy_P(ussdRequester.address, (const char *)F(""));
    }
  }

  static unsigned long lastRegistered = millis();
  if (networkRegistration.status == 1 || networkRegistration.status == 5 || (millis() - lastReset < 60000))
    lastRegistered = millis();
  else if (millis() - lastRegistered > 30000) // default  30000 
    if (dte.AT()) dte.togglePower();
}

void Communication::Cellular::smsTask(void) {
  wdt_reset();
if (Urc.newMessage.updated) {
    Urc.newMessage.updated = false;
    if ((smsMessage.data[0] == '*' && smsMessage.data[strlen(smsMessage.data) - 1] == '#') ||
        (ussdRequester.isProcessing == true && strcasecmp(ussdRequester.address, smsMessage.address) == 0)) {
      if (ussdRequester.isProcessing && strcasecmp(ussdRequester.address, smsMessage.address) != 0) {
        asyncFlag.smsGeneral = true;
        strcpy(asyncMessage.destinationNumber, ussdRequester.address);
        strcpy(asyncMessage.messageData, Urc.serviceDataIndication.str);
        asyncMessage.typeOfSms = SMS_USSD;
      } else if (!gsm.sendServiceData(smsMessage.data)) {
        asyncFlag.smsGeneral = true;
        strcpy(asyncMessage.destinationNumber, ussdRequester.address);
        strcpy(asyncMessage.messageData, Urc.serviceDataIndication.str);
        asyncMessage.typeOfSms = SMS_USSD;
        gsm.cancelServiceData();
        ussdRequester.isProcessing = false;
        strcpy_P(ussdRequester.address, (const char *)F(""));
      } else {
        ussdRequester.isProcessing = true;
        ussdRequester.millisProcessed = millis();
        strcpy(ussdRequester.address, smsMessage.address);
      }
    } else if (strcasecmp_P(smsMessage.data, (const char*)F("RESET GSM")) == 0) {
      if (dte.AT()) dte.powerReset();
    } else if (strstr_P(smsMessage.data, (const char *)F("ACT=B02"))) {
      char *pointer = strstr_P(smsMessage.data, (const char*)F("PHONE"));
      char envBuffer[20]; char *buffer;
      while ((pointer == smsMessage.data && (*(pointer + strlen_P((const char*)F("PHONE"))) != '=')) ||
             (pointer > smsMessage.data && (*(pointer + strlen_P((const char*)F("PHONE"))) != '=')) ||
             (pointer > smsMessage.data && (*(pointer - 1) != '\n'))) {
        pointer = strstr_P(pointer + 1, (const char*)F("PHONE"));
      }
      pointer += strlen_P((const char*)F("PHONE"));
      strcpy(envBuffer, pointer);
      buffer = strtok_P(envBuffer, (const char*)F("=\n"));
      gsm.setOwnNumber(buffer);
    } else {
      smsUnknown.updated = true;
      strcpy(smsUnknown.deviceCode, System::Identity::getId());
      strcpy(smsUnknown.address, smsMessage.address);
      setDateTimeMysqlFormat(smsMessage.timestamp);
    }
    // else {
    //   sms.sendSMS(F(SMS_DISPOSAL_NUMBER), smsMessage.data);
    // }
  }
}

const char *Communication::Cellular::getLatitude(void) {
  return strlen(locationTime.latitude) > 0 ? locationTime.latitude : "-7.671272";
}

const char *Communication::Cellular::getLongitude(void) {
  return strlen(locationTime.longitude) > 0 ? locationTime.longitude : "110.417002";
}

bool Communication::Cellular::isSubcriberNumberEmpty(void) {
  if (strlen(getSubscriberNumber()) == 0) return true;
  else return false;
}

bool Communication::Cellular::getNumberFromJack(void) {
  wdt_reset();
  static bool waitingNumberFromJack = false;
  if (!isSubcriberNumberEmpty()) return false;
  else if (isSubcriberNumberEmpty() && !waitingNumberFromJack) {
    asyncFlag.smsGeneral = true;
    strcpy(asyncMessage.destinationNumber, SMS_JACK_NUMBER);
    strcpy_P(asyncMessage.messageData, (const char*)F("ACT=B02"));
    asyncMessage.typeOfSms = SMS_GET_NUMBER;
    waitingNumberFromJack = true;
  }
  return true;
}

bool Communication::Cellular::openConnection(void) {
  if (strlen(apn.apn) == 0) return false;
  ip.setConnectionParamGprs(apn.apn, apn.user, apn.pwd);
  return ip.openConnection();
}

bool Communication::Cellular::queueBlumbangReksaDevice(struct BlumbangReksaDevice blumbangReksaDevice) {
  int bufferSize = Storage::Eeprom::getBufferSizeBlumbangReksaDeviceQueue();
  int dIndex = Storage::Eeprom::getBlumbangReksaDeviceQueueIndex() - Storage::Eeprom::getBlumbangReksaDeviceReadIndex();
  if (dIndex < 0) dIndex += bufferSize;
  if (dIndex >= bufferSize - 1) return false;
  Storage::Eeprom::writeBlumbangReksaDeviceQueueIndex((Storage::Eeprom::getBlumbangReksaDeviceQueueIndex() + 1) % bufferSize);
  Storage::Eeprom::writeBlumbangReksaDeviceQueue(Storage::Eeprom::getBlumbangReksaDeviceQueueIndex(), (unsigned char *)&blumbangReksaDevice, sizeof(blumbangReksaDevice));
  char buffer[96];
  implode(buffer, ",", &blumbangReksaDevice);
  return true;
}

struct BlumbangReksaDevice Communication::Cellular::readQueuedBlumbangReksaDevice(void) {
  BlumbangReksaDevice blumbangReksaDevice;
  int bufferSize = Storage::Eeprom::getBufferSizeBlumbangReksaDeviceQueue();
  int index = (Storage::Eeprom::getBlumbangReksaDeviceReadIndex() + 1) % bufferSize;
  Storage::Eeprom::readBlumbangReksaDeviceQueue(index, (unsigned char *)&blumbangReksaDevice, sizeof(blumbangReksaDevice));
  return blumbangReksaDevice;
}

void Communication::Cellular::updateReadBlumbangReksaDeviceIndex(void) {
  int bufferSize = Storage::Eeprom::getBufferSizeBlumbangReksaDeviceQueue();
  if (Storage::Eeprom::getBlumbangReksaDeviceReadIndex() == Storage::Eeprom::getBlumbangReksaDeviceQueueIndex()) return;
  Storage::Eeprom::writeBlumbangReksaDeviceReadIndex((Storage::Eeprom::getBlumbangReksaDeviceReadIndex() + 1) % bufferSize);
}

int Communication::Cellular::totalQueuedBlumbangReksaDevice(void) {
  int bufferSize = Storage::Eeprom::getBufferSizeBlumbangReksaDeviceQueue();
  int dIndex = Storage::Eeprom::getBlumbangReksaDeviceQueueIndex() - Storage::Eeprom::getBlumbangReksaDeviceReadIndex();
  if (dIndex < 0) dIndex += bufferSize;
  return dIndex;
}

void Communication::Cellular::clearQueuedBlumbangReksaDevice(void) {
  Storage::Eeprom::writeBlumbangReksaDeviceQueueIndex(0);
  Storage::Eeprom::writeBlumbangReksaDeviceReadIndex(0);
}

const char *Communication::Cellular::implode(char buffer[], const char *glue, const struct BlumbangReksaDevice *blumbangReksaDevice) {
  char valueBuffer[10];
  strcpy(buffer, blumbangReksaDevice->code); // 8
  strcat(buffer, glue);
  strcat(buffer, blumbangReksaDevice->imei); // 16
  strcat(buffer, glue);
  strcat_P(buffer, (const char *)F(VERSION)); // 11
  strcat(buffer, glue);
  strcat(buffer, blumbangReksaDevice->phoneNumber); // 16
  strcat(buffer, glue);
  strcat(buffer, dtostrf(blumbangReksaDevice->inputVoltage, 1, 2, valueBuffer)); // 4
  strcat(buffer, glue);
  strcat(buffer, itoa(blumbangReksaDevice->networkSignal, valueBuffer, 10)); // 2
  strcat(buffer, glue);
  strcat(buffer, blumbangReksaDevice->latitude); // 11
  strcat(buffer, glue);
  strcat(buffer, blumbangReksaDevice->longitude); // 11

  return buffer;
}

bool Communication::Cellular::queueBlumbangReksaData(struct BlumbangReksaData blumbangReksaData) {
  int bufferSize = Storage::Eeprom::getBufferSizeBlumbangReksaDataQueue();
  int dIndex = Storage::Eeprom::getBlumbangReksaDataQueueIndex() - Storage::Eeprom::getBlumbangReksaDataReadIndex();
  if (dIndex < 0) dIndex += bufferSize;
  if (dIndex >= bufferSize - 1) return false;
  Storage::Eeprom::writeBlumbangReksaDataQueueIndex((Storage::Eeprom::getBlumbangReksaDataQueueIndex() + 1) % bufferSize);
  Storage::Eeprom::writeBlumbangReksaDataQueue(Storage::Eeprom::getBlumbangReksaDataQueueIndex(), (unsigned char *)&blumbangReksaData, sizeof(blumbangReksaData));
  char buffer[128];
  implode(buffer, ",", &blumbangReksaData);
  return true;
}

struct BlumbangReksaData Communication::Cellular::readQueuedBlumbangReksaData(void) {
  BlumbangReksaData blumbangReksaData;
  int bufferSize = Storage::Eeprom::getBufferSizeBlumbangReksaDataQueue();
  int index = (Storage::Eeprom::getBlumbangReksaDataReadIndex() + 1) % bufferSize;
  Storage::Eeprom::readBlumbangReksaDataQueue(index, (unsigned char *)&blumbangReksaData, sizeof(blumbangReksaData));
  return blumbangReksaData;
}

void Communication::Cellular::updateReadBlumbangReksaDataIndex(void) {
  int bufferSize = Storage::Eeprom::getBufferSizeBlumbangReksaDataQueue();
  if (Storage::Eeprom::getBlumbangReksaDataReadIndex() == Storage::Eeprom::getBlumbangReksaDataQueueIndex()) return;
  Storage::Eeprom::writeBlumbangReksaDataReadIndex((Storage::Eeprom::getBlumbangReksaDataReadIndex() + 1) % bufferSize);
}

int Communication::Cellular::totalQueuedBlumbangReksaData(void) {
  int bufferSize = Storage::Eeprom::getBufferSizeBlumbangReksaDataQueue();
  int dIndex = Storage::Eeprom::getBlumbangReksaDataQueueIndex() - Storage::Eeprom::getBlumbangReksaDataReadIndex();
  if (dIndex < 0) dIndex += bufferSize;
  return dIndex;
}

void Communication::Cellular::clearQueuedBlumbangReksaData(void) {
  Storage::Eeprom::writeBlumbangReksaDataQueueIndex(0);
  Storage::Eeprom::writeBlumbangReksaDataReadIndex(0);
}

const char *Communication::Cellular::implode(char buffer[], const char *glue, const struct BlumbangReksaData *blumbangReksaData) {
  char valueBuffer[10];
  strcpy(buffer, blumbangReksaData->deviceCode); // 8
  strcat(buffer, glue);
  strcat(buffer, blumbangReksaData->deviceImei); // 16
  strcat(buffer, glue);
  strcat(buffer, blumbangReksaData->timeStamp); // 25
  strcat(buffer, glue);
  strcat(buffer, dtostrf(blumbangReksaData->temperature, 1, 2, valueBuffer)); // 6
  strcat(buffer, glue);
#if DO_SENSOR_SERIAL_ENABLE
  strcat(buffer, dtostrf(blumbangReksaData->DOPercentage, 1, 2, valueBuffer)); // 6
  strcat(buffer, glue);
  strcat(buffer, dtostrf(blumbangReksaData->DO, 1, 2, valueBuffer)); // 6
  strcat(buffer, glue);
#endif
#if EC_SENSOR_SERIAL_ENABLE
  strcat(buffer, dtostrf(blumbangReksaData->EC, 1, 2, valueBuffer)); // 9
  strcat(buffer, glue);
  strcat(buffer, dtostrf(blumbangReksaData->TDS, 1, 2, valueBuffer)); // 9
  strcat(buffer, glue);
  strcat(buffer, dtostrf(blumbangReksaData->salinity, 1, 2, valueBuffer)); // 5
  strcat(buffer, glue);
  strcat(buffer, dtostrf(blumbangReksaData->specificGravityOfSeaWater, 1, 2, valueBuffer)); // 5
  strcat(buffer, glue);
#endif
#if PH_SENSOR_SERIAL_ENABLE
  strcat(buffer, dtostrf(blumbangReksaData->pH, 1, 2, valueBuffer)); // 5
#endif
  return buffer;
}
