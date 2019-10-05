#include "Config.h"
#include "Indicating.h"
#include "splash_image.h"
#include "lang.h"
#include "softI2C.h"



bool Indicating::Display::sleep = false;
bool Indicating::Display::sleepBuffer = false;
char Indicating::Display::optionsCalibration[4][17];
bool Indicating::Display::chargingStatus = false;

U8GLIB_TYPE u8gType;//(U8G_I2C_OPT_NONE);
U8GLIB Indicating::Display::u8g = u8gType;
GadgetDisplay Indicating::Display::gadgetDisplay(u8g);
GadgetConfirmation Indicating::Display::gadgetConfirmation(u8g);
GadgetSplash Indicating::Display::gadgetSplash(u8g);
GadgetStandbyScreen Indicating::Display::gadgedStandby(u8g);
#if CALIBRATION_BUTTON_ENABLE
GadgetMenu Indicating::Display::gadgetMenuCalibration(u8g, optionsCalibration, 4);
#endif


softI2C CHG( CHG_SDA, CHG_SCL);

bool writeDataTo(byte addr, byte register_, byte data)
{
  CHG.beginTransmission(addr);
  CHG.write(register_);
  CHG.endTransmission(false);
  CHG.write(data);
  CHG.endTransmission();
  return true;
}
byte getDataFrom(byte addr, byte register_)
{
  byte tmp =0;
  CHG.beginTransmission(addr);
  CHG.write(register_);
  CHG.requestFrom(addr,1);
  if(CHG.available()==1)
    tmp = CHG.read();
  CHG.endTransmission();
  return tmp;
}

void Indicating::Display::setup(void) {
  //u8g.begin();
  //clearDisplay();
  strcpy(optionsCalibration[0], Indicating::Translation::get(PH_CALIBRATION));
  strcpy(optionsCalibration[1], Indicating::Translation::get(EC_CALIBRATION));
  strcpy(optionsCalibration[2], Indicating::Translation::get(DO_CALIBRATION));
  strcpy(optionsCalibration[3], Indicating::Translation::get(BACK));
#if CHARGING_DETECTION
   chargingPinSetup();
   chargerStatus();
#endif

}

void Indicating::Display::loop(void) {
  chargerStatus();
  //Serial.println("battery volt : "+String(getBatteryVolt(),4));
  if (sleepBuffer != sleep) {
    sleep = sleepBuffer;
  }
  if (sleep) return;
}

bool Indicating::Display::isSleep(void) {
  return sleep;
}

bool Indicating::Display::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void Indicating::Display::setSleep(bool sleep) {
  sleepBuffer = sleep;
}

void Indicating::Display::chargingPinSetup(void){
  pinMode(CHG_CE,OUTPUT);
  digitalWrite(CHG_CE,LOW);
  pinMode(CHARGER_STATUS_PIN,INPUT_PULLUP);
  writeDataTo(CHG_ADDR,0x01,0x3c);
  writeDataTo(CHG_ADDR,0x02,0x82);
  writeDataTo(CHG_ADDR,0x03,0xb0);
  writeDataTo(CHG_ADDR,0x06, 0b00111101);

}

bool Indicating::Display::isCharging(void) {
  return chargingStatus;
}

void Indicating::Display::info(const char row0[], const char row1[], const char row2[], const char row3[]) {
  gadgetDisplay.setString(0, 0, row0);
  gadgetDisplay.setString(1, 0, row1);
  gadgetDisplay.setString(2, 0, row2);
  gadgetDisplay.setString(3, 0, row3);
  gadgetDisplay.draw(false);
  //delay(200);
}

void Indicating::Display::info(const __FlashStringHelper *row0, const char row1[], const char row2[], const char row3[]) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row0);
  info(buffer, row1, row2, row3);
}

void Indicating::Display::info(const char row0[], const __FlashStringHelper *row1, const char row2[], const char row3[]) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row1);
  info(row0, buffer, row2, row3);
}

void Indicating::Display::info(const __FlashStringHelper *row0, const __FlashStringHelper *row1, const char row2[], const char row3[]) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row1);
  info(row0, buffer, row2, row3);
}

void Indicating::Display::info(const char row0[], const char row1[], const __FlashStringHelper *row2, const char row3[]) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row2);
  info(row0, row1, buffer, row3);
}

void Indicating::Display::info(const __FlashStringHelper *row0, const char row1[], const __FlashStringHelper *row2, const char row3[]) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row2);
  info(row0, row1, buffer, row3);
}

void Indicating::Display::info(const char row0[], const __FlashStringHelper *row1, const __FlashStringHelper *row2, const char row3[]) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row2);
  info(row0, row1, buffer, row3);
}

void Indicating::Display::info(const __FlashStringHelper *row0, const __FlashStringHelper *row1, const __FlashStringHelper *row2, const char row3[]) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row2);
  info(row0, row1, buffer, row3);
}

void Indicating::Display::info(const char row0[], const char row1[], const char row2[], const __FlashStringHelper *row3) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row3);
  info(row0, row1, row2, buffer);
}

void Indicating::Display::info(const __FlashStringHelper *row0, const char row1[], const char row2[], const __FlashStringHelper *row3) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row3);
  info(row0, row1, row2, buffer);
}

void Indicating::Display::info(const char row0[], const __FlashStringHelper *row1, const char row2[], const __FlashStringHelper *row3) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row3);
  info(row0, row1, row2, buffer);
}

void Indicating::Display::info(const __FlashStringHelper *row0, const __FlashStringHelper *row1, const char row2[], const __FlashStringHelper *row3) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row3);
  info(row0, row1, row2, buffer);
}

void Indicating::Display::info(const char row0[], const char row1[], const __FlashStringHelper *row2, const __FlashStringHelper *row3) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row3);
  info(row0, row1, row2, buffer);
}

void Indicating::Display::info(const __FlashStringHelper *row0, const char row1[], const __FlashStringHelper *row2, const __FlashStringHelper *row3) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row3);
  info(row0, row1, row2, buffer);
}

void Indicating::Display::info(const char row0[], const __FlashStringHelper *row1, const __FlashStringHelper *row2, const __FlashStringHelper *row3) {
  char buffer[17];
  strcpy_P(buffer, (const char *)row3);
  info(row0, row1, row2, buffer);
}

void Indicating::Display::info(const __FlashStringHelper *row0, const __FlashStringHelper *row1, const __FlashStringHelper *row2, const __FlashStringHelper *row3) {
  char buffer[12];
  //strcpy_P(buffer, (const char *)row3);
  sprintf_P(buffer, (const char *)F("     %s"),row2);
  info(row0, row1, buffer, row3);
}

void Indicating::Display::measurement(uint8_t hour,    uint8_t minute, const char oper[], bool isGprsAttach, unsigned char connectionStatus, unsigned char signalStrength, 
                                      unsigned char batt, bool isCharging_,
                                      const char id[], int totalQueuedData, 
                                      float temperature, bool isTemperatureStable, 
                                      float pH, bool ispHStable, 
                                      float salinity, bool isSalinityStable,
                                      float dissolvedOxygen, bool isDissolvedOxygenStable) {
  static bool pulse = false;
  static unsigned long pulseMillis = 0;
  char valueBuffer[5];
  char bufferText [ 8];
  int state = 0;
  //unsigned int count = 2;

  gadgetDisplay.setTime(hour, minute);
  gadgetDisplay.setOperator(oper);

  if (isGprsAttach) {
    state = 1;
    if (connectionStatus == 1) state = 2;
  } else
    state = 0;
  gadgetDisplay.setGPRSState(state);
  gadgetDisplay.setSignal(signalStrength);
  gadgetDisplay.setBatt(batt);

  gadgetDisplay.setString(1, 0, id);
  if (totalQueuedData > 0) {
    sprintf_P(bufferText, (const char *)F("     %c%2d"), pulse ? 187 : ' ', totalQueuedData);
    gadgetDisplay.setString(1, 1, bufferText);
  }
  if(abs(temperature) > 99)
    sprintf_P(bufferText, (const char *)F("T %c%s"), (pulse || isTemperatureStable) ? ':' : ' ', isnan(temperature)?"-":dtostrf(temperature, 4, 0, valueBuffer));
  else
    sprintf_P(bufferText, (const char *)F("T %c%s"), (pulse || isTemperatureStable) ? ':' : ' ', isnan(temperature)?"-":dtostrf(temperature, 4, 1, valueBuffer));
  gadgetDisplay.setString(2, 0, bufferText);
#if PH_SENSOR_SERIAL_ENABLE
    sprintf_P(bufferText, (const char *)F("pH%c%s"), (pulse || ispHStable || isnan(pH)) ? ':' : ' ', isnan(pH)?"-":dtostrf(pH, 4, (pH < 10.000) ? 2 : 2, valueBuffer));
    gadgetDisplay.setString(2, 1, bufferText);
#endif
#if EC_SENSOR_SERIAL_ENABLE
  sprintf_P(bufferText, (const char *)F("SL%c%s"), (pulse || isSalinityStable  || isnan(salinity)) ? ':' : ' ', isnan(salinity)?"-":dtostrf(salinity, 4, (salinity < 10.000) ? 2 : 1, valueBuffer));
  gadgetDisplay.setString(3, 0, bufferText);
#endif
#if DO_SENSOR_SERIAL_ENABLE
  sprintf_P(bufferText, (const char *)F("DO%c%s"), (pulse || isDissolvedOxygenStable || isnan(dissolvedOxygen) ) ? ':' : ' ',  isnan(dissolvedOxygen)?"-":dtostrf(dissolvedOxygen, 4, (dissolvedOxygen < 10.000) ? 2 : 1, valueBuffer));
  gadgetDisplay.setString(3, 1, bufferText);
#endif
  gadgetDisplay.draw(true, pulse && isCharging_);

  if (millis() - pulseMillis > 500) {
    pulse = !pulse;
    pulseMillis = millis();
  }
}

bool Indicating::Display::confirmation(const char confirmationText[], const char yesText[], const char noText[]) {
  gadgetConfirmation.setConfirmationText(confirmationText, yesText, noText);
  gadgetConfirmation.draw();
  return gadgetConfirmation.getConfirmationPosition();
}

bool Indicating::Display::confirmation(const char confirmationText[], const __FlashStringHelper yesText[], const __FlashStringHelper noText[]) {
  char buffer[2][17];
  strcpy_P(buffer[0], (const char *) yesText);
  strcpy_P(buffer[1], (const char *) noText);
  return confirmation(confirmationText, buffer[0], buffer[1]);
}

bool Indicating::Display::confirmation(const __FlashStringHelper confirmationText[], const __FlashStringHelper yesText[], const __FlashStringHelper noText[]) {
  char buffer[17];
  strcpy_P(buffer, (const char *)confirmationText);
  return confirmation(buffer, yesText, noText);
}

void Indicating::Display::confirmationNext(void) {
  gadgetConfirmation.next();
}

void Indicating::Display::confirmationPrevious(void) {
  gadgetConfirmation.previous();
}

void Indicating::Display::menuCalibration(void) {
  gadgetMenuCalibration.draw();
}

void Indicating::Display::resetMenuCalibrationPosition(void){
  gadgetMenuCalibration.resetPosition();
}

void Indicating::Display::menuCalibrationNext(void) {
  gadgetMenuCalibration.next();
}

void Indicating::Display::menuCalibrationPrevious(void) {
  gadgetMenuCalibration.previous();
}

unsigned char Indicating::Display::menuCalibrationGetPosition(void) {
  return gadgetMenuCalibration.getPosition();
}

void Indicating::Display::splash(const char *info) {
  gadgetSplash.setImage(0, 0, 16, 54, splash_image);
  gadgetSplash.setInfo(info);
  gadgetSplash.draw();
}

void Indicating::Display::splash(const __FlashStringHelper *info) {
  char buffer[19];
  strcpy_P(buffer, (const char *)info);
  splash(buffer);
}

void Indicating::Display::standbyScreen(const char info1[], const char info2[], const char info3[])
{
  gadgedStandby.setImage(0, 10, 8, 40, sleep_screen);
  gadgedStandby.setInfo1(info1);
  gadgedStandby.setInfo2(info2);
  gadgedStandby.setInfo3(info3);
  gadgedStandby.draw();
}

void Indicating::Display::standbyScreen(const char info[])
{
  char buffer[3][12];
  for (uint8_t i = 0; i < 3; i++) {
    if ((i * 11) < strlen(info)) {
      strncpy(buffer[i], info + (i * 11), 12);
      buffer[i][12] = '\0';
    } else {
      buffer[i][0] = '\0';
    }
  }
  standbyScreen(buffer[0], buffer[1], buffer[2]);
}


void Indicating::Display::info(const char *text) {
  char buffer[4][17];
  for (uint8_t i = 0; i < 4; i++) {
    if ((i * 16) < strlen(text)) {
      strncpy(buffer[i], text + (i * 16), 16);
      buffer[i][16] = '\0';
    } else {
      buffer[i][0] = '\0';
    }
  }
  info(buffer[0], buffer[1], buffer[2], buffer[3]);
}

void Indicating::Display::info(const __FlashStringHelper *text) {
  char buffer[65];
  strcpy_P(buffer, (const char *) text);
  info(buffer);
}

void Indicating::Display::clearDisplay(void)
{
  gadgetDisplay.clearDisplay();
}


void Indicating::Display::chargerStatus(void)
{
  static boolean bitBanged1,bitBanged2;
  bitBanged2 = bitBanged1;
  bitBanged1 = !digitalRead(CHARGER_STATUS_PIN);//==1?false:true;///bitRead(u, charger_pin_stat) ; 
  chargingStatus = (bitBanged1 | bitBanged2 );
  if(!chargingStatus)
  {
    if(getSourceVoltage() > 4.6f)
      chargingStatus = true;
  }
}


float Indicating::Display::getBatteryVolt(void)
{
  unsigned int data = getDataFrom(CHG_ADDR, 0x1D) << 8 | getDataFrom(CHG_ADDR, 0x1E);
  float v = (float)data/1000.0;
  writeDataTo(CHG_ADDR,0x15, 0b10010000);
  return v;
}

float Indicating::Display::getSourceVoltage(void)
{
  unsigned int data = getDataFrom(CHG_ADDR, 0x1B) << 8 | getDataFrom(CHG_ADDR, 0x1C);
  float v = (float)data/1000.0;
  writeDataTo(CHG_ADDR,0x15, 0b10010000);
  return v;
}

void Indicating::Display::BMS_control(void)
{
  writeDataTo(CHG_ADDR,0x01,30);
  writeDataTo(CHG_ADDR,0x02,68);
  writeDataTo(CHG_ADDR,0x03,176);
  writeDataTo(CHG_ADDR,0x06, 0b00111101);
  writeDataTo(CHG_ADDR,0x06, 0b00111101);
  writeDataTo(CHG_ADDR,0x15, 0b10010000);
}