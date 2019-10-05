#ifndef analogPH_h
#define analogPH_h

#include <Arduino.h>
#include <Wire.h>
#include <Stream.h>
#include "softI2C.h"
#include "statistic.h"
#include "ADS1112.h"

#define calibrationPointMAX         3
#define LOW_PH                      4
#define MID_PH                      7
#define HIGH_PH                     10



struct phADC
{
    float reference;        
    float ph_signal;
    float rtd_signal;
};

struct pHCalibrationBuffer
{
  float slope,
        intercept;
  uint8_t point;
};

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

class analogPH:public ADS1112
{
    protected:
        float reverse_funct(float pH_, float temp_);
        
    private:
        StabilityDetector_ phStableCount;
        StabilityDetector_ pHVoltStable;
        StabilityDetector_ tempStable;
        softI2C *line;
        ADS1112 ADCLine;
        
        uint8_t cal_level_buffer = 0;
        float intercept, slope;
        float pH          = 0,
              temperature = 25.00f,
              resistance_RTD = 1000,
              RTD_offset = -30.3077736;
        unsigned long run_time_ph = millis(),
                      run_time_rtd= millis();
        float ema_a_ph, ema_b_ph, ema_c_ph;
        float ema_a_rtd,ema_b_rtd, ema_c_rtd;
        phADC voltage;
        pHCalibrationBuffer calibrationVal;
        float defaultLow, defaultMid;
        float temporaryVLow, temporaryVMid;
        float temporaryMid;
        
        float reverse_factor;
        
        

        void readTemperature(void);
        void readPH(void);
        float readPHVoltage(void);
        float getRefVoltage(void);
        float getPHSignal(void);
        float getRTDSignal(void);
        analogPH&calibrationMid(float mid);
        analogPH&calibrationLow(float low);
        analogPH&calibrationHigh(float high);
        
    public:
        analogPH(softI2C &line, float volt_low = 0.153f, float volt_mid = 0.008f);
        bool begin();
        
        analogPH&resetParam(void);
        
        analogPH&initValuePH(pHCalibrationBuffer calibrationBuffer){this->calibrationVal = calibrationBuffer; return *this;};
        analogPH&initValueTemperature(float bufferValue){this->RTD_offset = bufferValue; return *this;};

        pHCalibrationBuffer getPHCalibrationValue(void){return this->calibrationVal;};
        float               getTemperatureCalibrationValue(void){return this->RTD_offset;};
        
        analogPH&singleReading(void);
        
        float getPH(void){return this->pH;};
        float getTemperature(void){return this->temperature;};
        
        bool calibrationPH(uint8_t level_ph, float pH_val);
        bool calibrationTemperature(const float temp_Ref);
        
        void setPHPrecision(float prec){this->phStableCount.setPrecision(prec);};
        bool isPHStable(void){return this->phStableCount.isStable();};
        unsigned char pHStableCount(void){return this->phStableCount.getStableCount();};
        void resetPHStable(void){this->phStableCount.reset();};

        void setPHVoltPrecision(float prec){this->pHVoltStable.setPrecision(prec);};
        bool isPHVoltStable(void){return this->pHVoltStable.isStable();};
        unsigned char pHVoltStableCount(void){return this->pHVoltStable.getStableCount();};
        void resetPHVoltStable(void){this->pHVoltStable.reset();};

        void setTempPrecision(float prec){this->tempStable.setPrecision(prec);};
        bool isTempStable(void){return this->tempStable.isStable();};
        unsigned char tempStableCount(void){return this->tempStable.getStableCount();};
        void resetTempStable(void){this->tempStable.reset();};
};
#endif
