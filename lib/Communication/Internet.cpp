#include "Config.h"
#include "Communication.h"
#include "Indicating.h"
#include "System.h"
#include "lang.h"


char Communication::Cellular::apiToken[18] = "";

void Communication::Cellular::Internet::setup(void) {

}

void Communication::Cellular::Internet::loop() {
  if (!asyncFlag.getLocation) {
    wdt_reset();
    httpTask();
  }
}

char* Communication::Cellular::Internet::getDataJson(char *json, char *param) {
  char *p;
  p = strtok(json, param);

  if (p != NULL) {
    for(uint8_t i=0; i<2; i++) p = strtok_P(NULL, (const char *) F("\""));
    return p;
  }
  return p;
}


#define countInternetRetry    4

void Communication::Cellular::Internet::httpTask(void) {
  /* If IP is open/connected */
  static uint8_t       httpRetry = 0;
  static uint8_t       connectionRetry = 0;
  static unsigned char httpBusyCode = 0;
  static unsigned long httpMillis = 0;
  static unsigned long httpTimeout = 30;


  if (connStatus.status == 1 && isAllowToConnect()) {
    if (httpBusyCode == 0) {
      if (totalQueuedBlumbangReksaDevice() > 0) {
        if (http.initialize(httpTimeout)) {
          http.setUserAgent(F("JALA"));
          http.setHeaders(F("Accept: application/json"));
          struct BlumbangReksaDevice blumbangReksaDevice = readQueuedBlumbangReksaDevice();
          char url1[80];   // default 80
          strcpy_P(url1, (const char *)F(""));
          char body1[200];
          //Serial.println("imei device : "+String(blumbangReksaDevice.imei));
          sprintf_P(url1, (const char *)F("http://app.jala.tech/api/device/device_multi_logs?imei=%s"), Communication::Cellular::imei);//blumbangReksaDevice.imei);
          param(body1, blumbangReksaDevice);
          http.action(POST_DATA, url1, body1);
          httpBusyCode = 1;
          httpMillis = millis();
        } else {
          httpRetry++;
        }
      }
      /* HTTP BlumbangReksaData */
      else if (totalQueuedBlumbangReksaData() > 0) {
        if (http.initialize(httpTimeout)) {
          http.setUserAgent(F("JALA"));
          http.setHeaders(F("Accept: application/json"));
          struct BlumbangReksaData blumbangReksaData = readQueuedBlumbangReksaData();
          char url2[80]; //default 80
          strcpy_P(url2, (const char *)F(""));
          char body2[200];
          
          sprintf_P(url2, (const char *)F("http://app.jala.tech/api/device/rfid_ponds/%s/multi_measurements?"), blumbangReksaData.deviceCode);
          param(body2, blumbangReksaData);
          http.action(POST_DATA, url2, body2);
          httpBusyCode = 2;
          httpMillis = millis();
        } else {
          httpRetry++;
        }
      } else if (smsUnknown.updated) {
        if (http.initialize(httpTimeout)) {
          http.setUserAgent(F("JALA"));
          http.setHeaders(F("Accept: application/json"));
          char body[250];
          param(body, smsUnknown);
          http.action(F("POST"), F("http://app.jala.tech/api/device/device_inboxes"), body);
          httpBusyCode = 3;
          httpMillis = millis();
        } else {
          httpRetry++;
        }
      } else if (false) {
        /* code */
      }
    }

    if (httpBusyCode == 1 && (Urc.httpAction.updated || (millis() - httpMillis > (httpTimeout * 1000)))) {
      if (200 <= Urc.httpAction.statusCode && Urc.httpAction.statusCode < 300 && Urc.httpAction.updated)  // if data has been sent
      {
        unsigned long bufferLength = Urc.httpAction.dataLength + 1;
        char buffer[bufferLength];
        http.readDataReceived(buffer, bufferLength - 1);
        strcpy(apiToken, get_token(buffer, ":"));
        updateReadBlumbangReksaDeviceIndex();
        httpRetry = 0;  connectionRetry = 0;
      } 
      else if (400 <= Urc.httpAction.statusCode && Urc.httpAction.statusCode < 500 && Urc.httpAction.statusCode != 408 && Urc.httpAction.updated) 
      {
        char message[17];
        sprintf_P(message, (const char *) F("   Error %3d!   "), Urc.httpAction.statusCode);
        Indicating::Display::info(F(""), message, F(""), F(""));
        updateReadBlumbangReksaDeviceIndex();
        httpRetry = connectionRetry = 0;
        wdt_reset();
        delay(700);
      } 
      else 
      {
        httpRetry++;
        if (httpRetry >=  countInternetRetry) 
        {
          Indicating::Display::info(Indicating::Translation::get(SEND_DATA_WITHOUT_INTERNET));
          asyncFlag.smsGeneral = true;
          char buffer[163];
          message(buffer, readQueuedBlumbangReksaDevice());
          strcpy(asyncMessage.destinationNumber, SMS_JACK_NUMBER);
          strcpy(asyncMessage.messageData, buffer);
          asyncMessage.typeOfSms = SMS_DEVICE;
        }
      }
      http.terminate();
      httpBusyCode = 0;
    } 
    else if (httpBusyCode == 2 && (Urc.httpAction.updated || (millis() - httpMillis > (httpTimeout * 1000)))) 
    {
      if (200 <= Urc.httpAction.statusCode && Urc.httpAction.statusCode < 300 && Urc.httpAction.updated) 
      {
        updateReadBlumbangReksaDataIndex();
        httpRetry = 0; connectionRetry = 0;
      } 
      else if (400 <= Urc.httpAction.statusCode && Urc.httpAction.statusCode < 500 && Urc.httpAction.statusCode != 408 && Urc.httpAction.updated) 
      {
        char message[17];
        sprintf_P(message, (const char *) F("   Error %3d!   "), Urc.httpAction.statusCode);
        if (Urc.httpAction.statusCode == 404U) 
        {
          Indicating::Display::info(F(""), message, Indicating::Translation::get(CANT_IDENTIFY_ID_CARD), F(""));
        } 
        else 
        {
          Indicating::Display::info(F(""), message, F("Missing data"), F(""));
        }
        updateReadBlumbangReksaDataIndex();
        httpRetry =0;  connectionRetry = 0;
        wdt_reset();
        delay(700);
      } 
      else 
      {
        httpRetry++;
        if (httpRetry >= countInternetRetry) 
        {
          Indicating::Display::info(Indicating::Translation::get(SEND_DATA_WITHOUT_INTERNET));
          asyncFlag.smsGeneral = true;
          char buffer[163];
          message(buffer, readQueuedBlumbangReksaData());
          strcpy(asyncMessage.destinationNumber, SMS_JACK_NUMBER);
          strcpy(asyncMessage.messageData, buffer);
          asyncMessage.typeOfSms = SMS_DATA;
        }
      }
      http.terminate();
      httpBusyCode = 0;
    }
  } else if (httpBusyCode == 3 && (Urc.httpAction.updated || (millis() - httpMillis > (httpTimeout * 1000)))) 
  {
    if (Urc.httpAction.statusCode == 201 && Urc.httpAction.updated) 
    {
      smsUnknown.updated = false;
      httpRetry = 0;  connectionRetry = 0;
    } 
    else if (400 <= Urc.httpAction.statusCode && Urc.httpAction.statusCode <= 404 && Urc.httpAction.updated) 
    {
      httpRetry = 0; connectionRetry = 0;
      smsUnknown.updated = false;
    } 
     else {
      smsUnknown.updated = false;
    }
    http.terminate();
    httpBusyCode = 0;
  } 
  else if ((networkRegistration.status == 1 || networkRegistration.status == 5) && !isAllowToConnect()) 
  {
    if (totalQueuedBlumbangReksaDevice() > 0) 
    {
      Indicating::Display::info(Indicating::Translation::get(SEND_DATA_WITHOUT_INTERNET));
      asyncFlag.smsGeneral = true;
      char buffer[163];
      message(buffer, readQueuedBlumbangReksaDevice());
      strcpy(asyncMessage.destinationNumber, SMS_JACK_NUMBER);
      strcpy(asyncMessage.messageData, buffer);
      asyncMessage.typeOfSms = SMS_DEVICE;
    } 
    else if (totalQueuedBlumbangReksaData() > 0) 
    {
      Indicating::Display::info(Indicating::Translation::get(SEND_DATA_WITHOUT_INTERNET));
      asyncFlag.smsGeneral = true;
      char buffer[163];
      message(buffer, readQueuedBlumbangReksaData());
      strcpy(asyncMessage.destinationNumber, SMS_JACK_NUMBER);
      strcpy(asyncMessage.messageData, buffer);
      asyncMessage.typeOfSms = SMS_DATA;
    }
  }

  if (httpRetry >= countInternetRetry) 
  {
    ip.closeConnection();
    httpRetry = 0;
    connectionRetry++;
  }
  if (connectionRetry >= countInternetRetry) 
  {
    wdt_reset();
    if (dte.AT()) dte.togglePower();
    connectionRetry = 0;
  }
}


void Communication::Cellular::Internet::removeChar(char *str, char garbage) 
{
  char *src, *dst;
  for (src = dst = str; *src != '\0'; src++) 
  {
      *dst = *src;
      if (*dst != garbage) dst++;
  }
  *dst = '\0';
}

char*  Communication::Cellular::Internet::get_token(char *str, char *delimiter)
{
	char *p;
  removeChar(str, '{');
	removeChar(str, '}');
	removeChar(str, '\"');
  p = strtok(str, delimiter);
  int a = 0;
  while (p != NULL) 
	{
    	p = strtok (NULL, delimiter);
    	++a;
    	if (a == 2)
    	{
    		 return p;
		  }
  	}return p;
}