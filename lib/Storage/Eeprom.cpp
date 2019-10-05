#include "Storage.h"

#define offsetAddrSpace 5;


FastCRC8 Storage::Eeprom::CRC8;

// write on Ext.eeprom
const int Storage::Eeprom::offsetAddressPhCalibrationValue = 0; //4 x 4 bytes + 1 crc
const int Storage::Eeprom::temperatureInterceptAddr        = 15;                 // 4 bytes + 1 crc
const int Storage::Eeprom::timezoneAddr                    = 20;

const int Storage::Eeprom::offsetAddressLocale = 24+offsetAddrSpace;
//const int Storage::Eeprom::offsetAddressLocale = 3687+offsetAddrSpace;

// write on Int.eeprom
const int Storage::Eeprom::offsetAddressBlumbangReksaDeviceQueueIndex = 25+offsetAddrSpace; // 2 bytes
const int Storage::Eeprom::offsetAddressBlumbangReksaDeviceReadIndex = 28+offsetAddrSpace; // 2 bytes
const int Storage::Eeprom::offsetAddressBlumbangReksaDeviceQueue = 31+offsetAddrSpace; // 19 x 98 bytes + 1 crc
const int Storage::Eeprom::bufferSizeBlumbangReksaDeviceQueue = 15;
const int Storage::Eeprom::offsetAddressBlumbangReksaDataQueueIndex = 2000+offsetAddrSpace; // 2 bytes
const int Storage::Eeprom::offsetAddressBlumbangReksaDataReadIndex = 2002+offsetAddrSpace; // 2 bytes
const int Storage::Eeprom::offsetAddressBlumbangReksaDataQueue = 2005+offsetAddrSpace; // 20 x 84 bytes + 1 crc
const int Storage::Eeprom::bufferSizeBlumbangReksaDataQueue = 15;


bool Storage::Eeprom::writePHCalibrationValue(const unsigned char* dataPH, unsigned int size) {
  for (size_t i = 0; i < size; i++) {
      EEPROM.write(offsetAddressPhCalibrationValue + i, *dataPH);
    dataPH++;
  }
  EEPROM.write(offsetAddressPhCalibrationValue + size, CRC8.maxim(dataPH, size));
  return true;
}

bool Storage::Eeprom::readPHCalibrationValue(unsigned char* dataPH, unsigned int size) {
  for (size_t i = 0; i < size; i++) {
    *dataPH = EEPROM.read(offsetAddressPhCalibrationValue + i);
    dataPH++;
  }
  uint8_t crc = EEPROM.read(offsetAddressPhCalibrationValue + size);
  if(crc == CRC8.maxim(dataPH, size)) return true;
  return false;
}

bool Storage::Eeprom::writeBlumbangReksaDeviceQueueIndex(int data) {
  EEPROM.put(offsetAddressBlumbangReksaDeviceQueueIndex, data);
  return true;
}

int Storage::Eeprom::getBlumbangReksaDeviceQueueIndex() {
  int data;
  EEPROM.get(offsetAddressBlumbangReksaDeviceQueueIndex, data);
  return data;
}

bool Storage::Eeprom::writeBlumbangReksaDeviceReadIndex(int data) {
  EEPROM.put(offsetAddressBlumbangReksaDeviceReadIndex, data);
  return true;
}

int Storage::Eeprom::getBlumbangReksaDeviceReadIndex() {
  int data;
  EEPROM.get(offsetAddressBlumbangReksaDeviceReadIndex, data);
  return data;
}

bool Storage::Eeprom::writeBlumbangReksaDeviceQueue(int index, const unsigned char *device, unsigned int size) {
  index = index * (size + 1);
  for (size_t i = 0; i < size; i++) {
    EEPROM.write(index + offsetAddressBlumbangReksaDeviceQueue + i, *device);
    device++;
  }
  EEPROM.write(index + offsetAddressBlumbangReksaDeviceQueue + size, CRC8.maxim(device, size));
  return true;
}

bool Storage::Eeprom::readBlumbangReksaDeviceQueue(int index, unsigned char *device, unsigned int size) {
  index = index * (size + 1);
  for (size_t i = 0; i < size; i++) {
    *device = EEPROM.read(index + offsetAddressBlumbangReksaDeviceQueue + i);
    device++;
  }
  uint8_t crc = EEPROM.read(index + offsetAddressBlumbangReksaDeviceQueue + size);
  if(crc == CRC8.maxim(device, size)) return true;
  return false;
}

bool Storage::Eeprom::writeBlumbangReksaDataQueueIndex(int data) {
  EEPROM.put(offsetAddressBlumbangReksaDataQueueIndex, data);
  return true;
}

int Storage::Eeprom::getBlumbangReksaDataQueueIndex() {
  int data;
  EEPROM.get(offsetAddressBlumbangReksaDataQueueIndex, data);
  return data;
}

bool Storage::Eeprom::writeBlumbangReksaDataReadIndex(int data) {
  EEPROM.put(offsetAddressBlumbangReksaDataReadIndex, data);
  return true;
}

int Storage::Eeprom::getBlumbangReksaDataReadIndex() {
  int data;
  EEPROM.get(offsetAddressBlumbangReksaDataReadIndex, data);
  return data;
}

bool Storage::Eeprom::writeBlumbangReksaDataQueue(int index, const unsigned char *data, unsigned int size) {
  index =  index * (size + 1);
  for (size_t i = 0; i < size; i++) {
    EEPROM.write(index + offsetAddressBlumbangReksaDataQueue + i, *data);
    data++;
  }
  EEPROM.write(index + offsetAddressBlumbangReksaDataQueue + size, CRC8.maxim(data, size));
  return true;
}

bool Storage::Eeprom::readBlumbangReksaDataQueue(int index, unsigned char* data, unsigned int size) {
  index = index * (size + 1);
  for (size_t i = 0; i < size; i++) {
    *data = EEPROM.read(index + offsetAddressBlumbangReksaDataQueue + i);
    data++;
  }
  uint8_t crc = EEPROM.read(index + offsetAddressBlumbangReksaDataQueue + size);

  if (crc == CRC8.maxim(data, size)) return true;
  return false;
}

bool Storage::Eeprom::setLocale(int lang) {
  if (lang > 1 && lang <= -1) {
    lang = 0;
  }
  EEPROM.write(offsetAddressLocale, lang);
  return true;
}

unsigned char Storage::Eeprom::getLocale(void) {
  //unsigned char buffer;
 if(EEPROM.read(offsetAddressLocale) > 0)
    return 1;
  else
    return 0;
}

bool Storage::Eeprom::writeTempIntercept(float slope)
{
  EEPROM.put(temperatureInterceptAddr, slope);
  return true;
}

float Storage::Eeprom::getTempIntercept(void)
{
  float intercept = 0 ;
  EEPROM.get(temperatureInterceptAddr, intercept);
  return ( ( isnan(intercept) ) ? 0 : intercept) ;
}


int Storage::Eeprom::getTimeZone(void)
{
  int timeZone = 0;
  EEPROM.get(timezoneAddr,timeZone);
  return timeZone;//(isnan(timeZone)?0:timeZone);
}

void Storage::Eeprom::writeTimeZone(int timeZone)
{
  EEPROM.put(timezoneAddr, timeZone);
 // return true;
}

