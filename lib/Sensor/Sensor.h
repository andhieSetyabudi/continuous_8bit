#ifndef SENSOR_H_
#define SENSOR_H_

#include <Arduino.h>
//#include <SoftwareSerial.h>
#include <DallasTemperature.h>
#include <EZO.h>
#include <OneWire.h>
#include "Config.h"
#include "Indicating.h"
#include "analogPH.h"

#if DHT_USED
  #include <DHT.h>
#endif

const char *t(int key);

class Sensor {
 private:
  static bool sleep;
  static bool sleepBuffer;

 public:
  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);

  static void setup(void);
  static void loop(void);

  class Water;
#if DHT_USED
  class Ambient;
#endif
  class Battery;
};

class Sensor::Water {
 private:
  //static unsigned long timeoutCalib;
#if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
  #if PH_SERIAL_TYPE == SOFTWARE_SERIAL
    static SoftwareSerial pHSerial;
    static pH_RTD pHsensor_;
  #endif
#elif  PH_SENSOR_INTERFACE == SENSOR_INTERFACE_I2C
  static analogPH  pH_temp_Sensor;
  static softI2C   pH_2Wire;
  static pHCalibrationBuffer pHCalibrationValue;
  static float temperatureCalibrationValue;
#endif


  static bool sleep;
  static bool sleepBuffer;
  static unsigned char powerSwitchState;

#if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_ONEWIRE
  static OneWire oneWire;
  static DallasTemperature ds;
  static DeviceAddress deviceAddress;
#elif TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE
  static RTD_I2C temperature_I2C;
  static OneWire oneWire;
  static DallasTemperature ds;
  static DeviceAddress deviceAddress;
#elif TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_SERIAL
  static OneWire oneWire;
  static DallasTemperature ds;
  static DeviceAddress deviceAddress;
#endif


#if DO_SENSOR_SERIAL == SOFTWARE_SERIAL
  static SoftwareSerial DO_Serial;
#endif
  static DO_EZO DOSensor;
#if EC_SENSOR_SERIAL == SOFTWARE_SERIAL
  static SoftwareSerial EC_Serial;
#endif
  static EC_EZO ECSensor;



  

  

//================== temperature variable
  static float temperature;
  static bool tempStat;

  static pHCalibrationBuffer loadPHCalibrationValue();
  static pHCalibrationBuffer savePHCalibrationValue(struct pHCalibrationBuffer calibrationValue);

 public:
  static void digitalWritePowerIso(unsigned char state);

  static bool getStatusTemperature(void);
  static float getTemperature(void) { pH_temp_Sensor.singleReading(); temperature = pH_temp_Sensor.getTemperature(); return temperature; };
  
  static float getDissolvedOxygen(void) { return DOSensor.getDissolvedOxygen(); };
  static float getPercentageDissolvedOxygen(void) { return DOSensor.getPercentage(); };
  static float getElectricalConductivity(void) { return ECSensor.getElectricalConductivity(); };
  static float getTotalDissolvedSolids(void) { return ECSensor.getTotalDissolveSolids(); };
  static float getSalinity(void) { return ECSensor.getSalinity(); };
  static float getSpecificGravityOfSeaWater(void) { return ECSensor.getOutputSpecificGravityOfSeaWaterEnabled(); };
  static float getpH() { return roundF(pH_temp_Sensor.getPH(),2);};
 
  static bool isDissolvedOxygenStable(void) { return DOSensor.isDissolvedOxygenStable(); };
  static bool isElectricalConductivityStable(void) { return ECSensor.isElectricalConductivityStable(); };
  static bool ispHStable(void) { return pH_temp_Sensor.isPHStable(); };
  static bool isDOAvailable(void){return DOSensor.getStatusDev();};
  static bool isECAvailable(void){return ECSensor.getStatusDev();};

  static void temperatureInit(void);
  #if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE
  static bool isTemperatureStable(void) { return temperature_I2C.isStable(); };
  static bool temperatureCalibration(void);
  #elif TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_SERIAL
  static bool isTemperatureStable(void) { return pH_temp_Sensor.isTempStable(); };
  static bool temperatureCalibration(void);
  #endif

  //static float getTemperatureValue(void);
  static void dissolvedOxygenInit(void);
  static void dissolvedOxygenFactoryReset(void) { DOSensor.factoryReset(); };
  static unsigned char dissolvedOxygenCalibrationPoint(void) { return DOSensor.getCalibrationPoint() - '0'; };
  static void dissolvedOxygenCalibratingAtmospheric(void);
  static void dissolvedOxygenCalibratingZero(void);

  static void electricalConductivityInit(void);
  static void electricalConductivityFactoryReset(void) { ECSensor.factoryReset(); };
  static unsigned char electricalConductivityCalibrationPoint(void) { return ECSensor.getCalibrationPoint() - '0'; };
  static void electricalConductivityCalibratingDry(void);
  static void electricalConductivityCalibratingLow(void);
  static void electricalConductivityCalibratingHigh(void);

  static void pHInit(void);
  static void pHFactoryReset(void) { /*pHSensor.factoryReset();*/ };
  static boolean pHCalibratingMid(void);
  static void pHCalibratingLow(void);
  static void pHCalibratingHigh(void);
  static bool pHCalibratingLowHigh(void);

  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);

  static void procFlow(uint8_t idx);
  static void setup(void);
  static void loop(void);
};

#if DHT_USED
class Sensor::Ambient {
 private:
  static bool sleep;
  static bool sleepBuffer;
  static DHT dht;

  static float temperature;
  static float humidity;

 public:
  static float getTemperature(void) { return temperature; };
  static float getHumidity(void) { return humidity; };

  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);

  static void setup(void);
  static void loop(void);
};
#endif

class Sensor::Battery {
 private:
  static bool sleep;
  static bool sleepBuffer;
  static bool lowStatus;
  static bool lowEvent;

  static unsigned int voltage;

 public:
  static bool isLowStatus(void) { return lowStatus; }
  static bool isLowEvent(void) { return lowEvent; }

  static uint8_t getPercentage(void);
  static float getVoltage(void) { return (float)voltage/100; };

  static bool isSleep(void);
  static bool isGoingToSleep(void);
  static void setSleep(bool sleep);

  static void setup(void);
  static void loop(void);
};

#endif
