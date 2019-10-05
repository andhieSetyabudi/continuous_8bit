#ifndef STORAGE_H_
#define STORAGE_H_

#include <Config.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <FastCRC.h>

class Storage {
public:
  class Eeprom;

  static void setup(void);
  static void loop(void);
};

  class Storage::Eeprom {
  private:
    static FastCRC8 CRC8;
    static const int offsetAddressPhCalibrationValue;
    static const int temperatureInterceptAddr;
    static const int timezoneAddr;
    
    static const int languageSetAddr;
    static const int offsetAddressBlumbangReksaDeviceQueueIndex;
    static const int offsetAddressBlumbangReksaDeviceReadIndex;
    static const int offsetAddressBlumbangReksaDeviceQueue;
    static const int bufferSizeBlumbangReksaDeviceQueue;
    static const int offsetAddressBlumbangReksaDataQueueIndex;
    static const int offsetAddressBlumbangReksaDataReadIndex;
    static const int offsetAddressBlumbangReksaDataQueue;
    static const int bufferSizeBlumbangReksaDataQueue;
    static const int offsetAddressLocale;
    static unsigned char locale;

  public:
    static bool writePHCalibrationValue(const unsigned char* dataPH, unsigned int size);
    static bool readPHCalibrationValue(unsigned char* dataPH, unsigned int size);

    static bool writeBlumbangReksaDeviceQueueIndex(int data);
    static int getBlumbangReksaDeviceQueueIndex();
    static bool writeBlumbangReksaDeviceReadIndex(int data);
    static int getBlumbangReksaDeviceReadIndex();
    static bool writeBlumbangReksaDeviceQueue(int index, const unsigned char* device, unsigned size);
    static bool readBlumbangReksaDeviceQueue(int index, unsigned char* device, unsigned int size);
    static const int getBufferSizeBlumbangReksaDeviceQueue(void) {return bufferSizeBlumbangReksaDeviceQueue; };

    static bool writeBlumbangReksaDataQueueIndex(int data);
    static int getBlumbangReksaDataQueueIndex();
    static bool writeBlumbangReksaDataReadIndex(int data);
    static int getBlumbangReksaDataReadIndex();
    static bool writeBlumbangReksaDataQueue(int index, const unsigned char* data, unsigned int size);
    static bool readBlumbangReksaDataQueue(int index, unsigned char* data, unsigned int size);
    static const int getBufferSizeBlumbangReksaDataQueue(void) { return bufferSizeBlumbangReksaDataQueue; };


    static bool setLocale(int lang);
    static unsigned char getLocale(void);

    static bool writeTempIntercept(float slope);
    static float getTempIntercept(void);

    static int getTimeZone(void);
    static void writeTimeZone(int timeZone);
  };

#endif
