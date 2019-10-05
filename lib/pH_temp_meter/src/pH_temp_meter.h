#ifndef PH_TEMP_METER_H_
#define PH_TEMP_METER_H_
#include <Arduino.h>
#include <Stream.h>
#include <SoftwareSerial.h>
#include "statistic.h"
#include <String.h>

class StabilityDetector_
{
private:
  bool stable;
  unsigned char stableCount;
  float precision=0.0125;
  unsigned char index;
  float valueBuffer[10];
public:
  StabilityDetector_ (float precision=0.005f);
  void setPrecision(float precision) { this->precision = precision; };
  void updateValue(float value);
  bool isStable(void) { return stable; };
  unsigned char getStableCount(void) { return stableCount; };
  float getModeValue(void) { return getMode(valueBuffer); };
  float getMedianValue(void) { return getMedian(valueBuffer); };
  float getDeviasionValue(void){return getDeviasion(valueBuffer);};
  void reset(void);
};

class pH_temp_meter//:public SoftwareSerial
{
    private:
        bool pHUpdated, temperatureUpdated;
        SoftwareSerial* com;
        float pH, temperature;
        StabilityDetector_ pHbuffer;
        StabilityDetector_ temperatureBuffer;
        String bufferMsg;
        unsigned long timeoutSystem = 1000;
        void clearBuffer(void);
        bool getResponse(unsigned long timeout__ = 1000);
        bool parsingDataPHTemp(void);
        int available(void);
        void flush(void);
        size_t write(uint8_t c);
        size_t write(const char c);
        size_t write(const char str[]);
        size_t write(const __FlashStringHelper *str);
        size_t readBytes(char buffer[], size_t length);
        int read (void);
        int peek(void);
        bool justSingleRun = false;
    public:
        pH_temp_meter(SoftwareSerial & com_);
        pH_temp_meter&begin(void);
        float getPH(void);
        float getTemperature(void);
        float getPHVoltage(void);
        float getTemperatureResistance(void);
        uint8_t getCalibrationInfo(void);
        bool setSleep(bool sleep_);

        bool pHCalibrationStart(void);
        bool pHCalibrationAction(void);
        bool pHCalibrationCancel(void);
        bool pHCalibrationMid(void);
        bool pHCalibrationLow(void);
        bool pHCalibrationHigh(void);

        bool temperatureCalibration(float temp_ref);

        void singleRunning(void);
        void resetPHStable(void){ pHbuffer.reset();};
        void resetTempStable(void){temperatureBuffer.reset();};
        bool ispHStable(void){return pHbuffer.isStable();};
        bool isTempStable(void){return temperatureBuffer.isStable();};
        void setPrecisionOfPH(float prec){pHbuffer.setPrecision(prec);};
        void setPrecisionOfTemp(float prec){temperatureBuffer.setPrecision(prec);};
        unsigned char getpHStableCount(void){return pHbuffer.getStableCount();};
        unsigned char getTempStableCount(void){return temperatureBuffer.getStableCount();};
};
#endif