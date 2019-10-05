#include "Config.h"
#include "Indicating.h"
#include "Sensor.h"
#include "Storage.h"
#include "lang.h"
#include "System.h" 

const float elevation_device = 50;    // m
float getPressure(float temp, float elevation)
{
	temp += 273.15;
	const float Po = 101325;		// in Pa
	const double M = 0.02896968;	// kg/mol
	const float Ro = 8.314462618;	// J/(mol.K)
	const float g  = 9.80665;		// m/s^2
	
	float pressure = Po * exp( -(g*elevation*(float)M)/(temp*Ro));
	return pressure;
	
}

#if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
  #if PH_SERIAL_TYPE == SOFTWARE_SERIAL
    SoftwareSerial Sensor::Water::pHSerial(PH_SERIAL_RX_PIN, PH_SERIAL_TX_PIN);
  #endif
#endif

bool Sensor::Water::sleep = false;
bool Sensor::Water::sleepBuffer = false;

unsigned char Sensor::Water::powerSwitchState = HIGH;

#if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_ONEWIRE
  OneWire Sensor::Water::oneWire(ONEWIRE_DS_PIN);
  DallasTemperature Sensor::Water::ds(&oneWire);
  DeviceAddress Sensor::Water::deviceAddress;
#elif TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE
  //RTD_I2C Sensor::Water::temperature_I2C(TEMPERATURE_I2C_SDA_PIN, TEMPERATURE_I2C_SCL_PIN, TEMPERATURE_ADDR);
  OneWire Sensor::Water::oneWire(ONEWIRE_DS_PIN);
  DallasTemperature Sensor::Water::ds(&oneWire);
  DeviceAddress Sensor::Water::deviceAddress;
  RTD_I2C Sensor::Water::temperature_I2C(TEMPERATURE_ADDR);
#else
  OneWire Sensor::Water::oneWire(ONEWIRE_DS_PIN);
  DallasTemperature Sensor::Water::ds(&oneWire);
  DeviceAddress Sensor::Water::deviceAddress;
#endif

#if DO_SENSOR_SERIAL_ENABLE
  #if DO_SENSOR_SERIAL == SOFTWARE_SERIAL
    SoftwareSerial Sensor::Water::DO_Serial(DO_SERIAL_RX_PIN, DO_SERIAL_TX_PIN);
    DO_EZO Sensor::Water::DOSensor(DO_Serial,DO_SENSOR_SERIAL_DEBUG);
  #else
    DO_EZO Sensor::Water::DOSensor(DO_SENSOR_SERIAL, DO_SENSOR_SERIAL_DEBUG);
  #endif
#endif

#if EC_SENSOR_SERIAL == SOFTWARE_SERIAL
  SoftwareSerial Sensor::Water::EC_Serial(EC_SERIAL_RX_PIN, EC_SERIAL_TX_PIN);
#endif

#if EC_SENSOR_SERIAL_ENABLE
  #if EC_SENSOR_SERIAL == SOFTWARE_SERIAL
    EC_EZO Sensor::Water::ECSensor(EC_Serial,EC_SENSOR_SERIAL_DEBUG);
  #else
    EC_EZO Sensor::Water::ECSensor(EC_SENSOR_SERIAL, EC_SENSOR_SERIAL_DEBUG);
  #endif
#endif



// preprocesor for pH sensor
#if PH_SENSOR_SERIAL_ENABLE
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
    pH_temp_meter Sensor::Water::pHsensor_(PH_SENSOR_SERIAL);//pH_EZO Sensor::Water::pHSensor(PH_SENSOR_SERIAL, PH_SENSOR_SERIAL_DEBUG);
  #endif
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_I2C
    //analogPH   pHSensor( PH_I2C_addr, defaultAtpH4, defaultAtpH7);
    softI2C   Sensor::Water::pH_2Wire(PH_I2C_SDA_PIN, PH_I2C_SCL_PIN);
    analogPH  Sensor::Water::pH_temp_Sensor(pH_line);
    pHCalibrationBuffer Sensor::Water::pHCalibrationValue;
    float Sensor::Water::temperatureCalibrationValue;
  #endif
#endif
  

#if ORP_SENSOR_SERIAL_ENABLE
ORP_EZO Sensor::Water::ORPSensor(ORP_SENSOR_SERIAL, ORP_SENSOR_SERIAL_DEBUG);
#endif
float Sensor::Water::temperature = 0.0f;
bool Sensor::Water::tempStat = false;


void Sensor::Water::setup(void) {
  pinMode(19, INPUT_PULLUP);
  pinMode(18, OUTPUT);
  pinMode(15, INPUT_PULLUP);
  pinMode(14, OUTPUT);
  #if SYSTEM_DEBUG
    Serial.println(F("set pin Serial"));
    Serial.flush();
  #endif

  pinMode(POWER_ISO_PIN, OUTPUT);
 // digitalWrite(POWER_ISO_PIN,LOW);
  digitalWritePowerIso(HIGH);
  delay(10);
    
#if DO_SENSOR_SERIAL_ENABLE
  #if SYSTEM_DEBUG
    Serial.println(F("set DO port!"));
  #endif 

  #if DO_SENSOR_SERIAL == SOFTWARE_SERIAL
    DO_Serial.begin(9600);
    pinMode(DO_SERIAL_TX_PIN, OUTPUT);
    digitalWrite(DO_SERIAL_TX_PIN, HIGH);
  #else
    DO_SENSOR_SERIAL.begin(9600);
    if(!DO_SENSOR_SERIAL) {delay(50);  DO_SENSOR_SERIAL.begin(9600);}
  #endif

  #if SYSTEM_DEBUG
    Serial.println(F(" DO sensor is waking up !"));
  #endif
  dissolvedOxygenInit();
#endif

#if EC_SENSOR_SERIAL_ENABLE
  #if SYSTEM_DEBUG
    Serial.println(F("set EC port!"));
  #endif

  #if EC_SENSOR_SERIAL == SOFTWARE_SERIAL
    EC_Serial.begin(9600);
    pinMode(EC_SERIAL_TX_PIN, OUTPUT);
    digitalWrite(EC_SERIAL_TX_PIN, HIGH);
  #else
    EC_SENSOR_SERIAL.begin(9600);
    if(!EC_SENSOR_SERIAL){delay(50); EC_SENSOR_SERIAL.begin(9600);}
  #endif

  #if SYSTEM_DEBUG
    Serial.println(F(" EC sensor is waking up !"));
  #endif
  electricalConductivityInit();
#endif


#if PH_SENSOR_SERIAL_ENABLE
  #if SYSTEM_DEBUG
    Serial.println(F(" pH sensor is waking up !"));
  #endif
  pHInit();
#endif

  temperatureInit();
}

void Sensor::Water::loop(void) {
//static unsigned long timeAccess;
 digitalWritePowerIso(HIGH);
 wdt_reset();
  if (sleepBuffer != sleep) 
  {
    if (sleep) 
    {
      digitalWritePowerIso(HIGH);
#if PH_SENSOR_SERIAL_ENABLE
        pHInit();
#endif
#if EC_SENSOR_SERIAL_ENABLE
        electricalConductivityInit();
#endif
#if DO_SENSOR_SERIAL_ENABLE
        dissolvedOxygenInit();
#endif


#if PH_SENSOR_SERIAL_ENABLE
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
     // pHSensor.sleep();
  #endif
#endif
      sleep = sleepBuffer;
    } 
    else 
    {
#if DO_SENSOR_SERIAL_ENABLE
      DOSensor.sleep();
#endif
#if EC_SENSOR_SERIAL_ENABLE
      ECSensor.sleep();
#endif

      digitalWritePowerIso(LOW);
      sleep = sleepBuffer;
    }
  }

  static unsigned long tCompensate = millis();
  if ((millis() - tCompensate >= 1000ul)) 
  {
    if (0.1f < temperature && temperature < 45.0f) 
    {
      #if PH_SENSOR_SERIAL_ENABLE
           // pHSensor.temperatureCompensation(temperature);
      #endif
      #if EC_SENSOR_SERIAL_ENABLE
            if(ECSensor.getStatusDev())
              ECSensor.temperatureCompensation(temperature);
      #endif
      #if DO_SENSOR_SERIAL_ENABLE
            if(DOSensor.getStatusDev())
            {
              DOSensor.pressureCompensation(getPressure(temperature,elevation_device)/1000);
              DOSensor.temperatureCompensation(temperature);
            }
              
      #endif
     // pHSensor.temperatureCompensation(temperature);
    }
        
    #if EC_SENSOR_SERIAL_ENABLE
        if (ECSensor.getElectricalConductivity() < 100000.0f) 
        {
          #if DO_SENSOR_SERIAL_ENABLE
            DOSensor.salinityCompensation((long)ECSensor.getElectricalConductivity());
          #endif
        }
    #endif
        tCompensate = millis();
  }

  #if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE
    temperature = getTemperatureValue();
  #else
  static unsigned long tTemperature = millis();
  if ((millis() - tTemperature) >= 850ul) 
  {
    #if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_SERIAL
      pH_temp_Sensor.singleReading();
      temperature = pH_temp_Sensor.getTemperature();
      //pHsensor_.getPH();
    #else
      ds.requestTemperaturesByAddress(deviceAddress);
      temperature = ds.getTempC(deviceAddress);
    #endif
    //getTemperatureValue();
    if (temperature < 0.1f) temperature = 0.0f;
    tTemperature = millis();
  }
  #endif

static unsigned int ezoT = Indicating::RTC::getSecondTime();
if ((Indicating::RTC::getSecondTime() - ezoT) > 60ul)
{
  #if DO_SENSOR_SERIAL_ENABLE
      if(!DOSensor.getStatusDev())
        dissolvedOxygenInit();
  #endif
  #if EC_SENSOR_SERIAL_ENABLE
      if(!ECSensor.getStatusDev())
        electricalConductivityInit();
  #endif
  ezoT = Indicating::RTC::getSecondTime();
}

#if DO_SENSOR_SERIAL_ENABLE
  DOSensor.continuousReading(EZO_Continuous_reading);
#endif
#if EC_SENSOR_SERIAL_ENABLE
  ECSensor.continuousReading(EZO_Continuous_reading);
#endif

#if PH_SENSOR_SERIAL_ENABLE
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
    pHsensor_.singleRunning();
    //pH_ = pHsensor_.getPH();//
    //pHSensor.continuousReading(false);
  #endif
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_I2C
    pH_temp_Sensor.singleReading();
  #endif
#endif
}

pHCalibrationBuffer Sensor::Water::loadPHCalibrationValue() {
  pHCalibrationBuffer calibrationValue;
  Storage::Eeprom::readPHCalibrationValue((unsigned char *)&calibrationValue, sizeof(pHCalibrationBuffer));
  return calibrationValue;
}

pHCalibrationBuffer Sensor::Water::savePHCalibrationValue( pHCalibrationBuffer calibrationValue) {
  Storage::Eeprom::writePHCalibrationValue((const unsigned char *)&calibrationValue, sizeof(pHCalibrationBuffer));
  return calibrationValue;
}

void Sensor::Water::digitalWritePowerIso(unsigned char state) {
  powerSwitchState = state;
  if(digitalRead(POWER_ISO_PIN) == state) return;
  else
    if(state == HIGH)
      digitalWrite(POWER_ISO_PIN,HIGH);//POWER_ISO_PORT |=POWER_ISO_PIN_; 
    else
      digitalWrite(POWER_ISO_PIN,LOW);//POWER_ISO_PORT &=~POWER_ISO_PIN_;
  delay(50);
}

void Sensor::Water::temperatureInit(void) {
  pH_temp_Sensor.singleReading();
  pH_temp_Sensor.setTempPrecision(precision_of_temp);
  pH_temp_Sensor.getTemperature();
  for(uint8_t ui = 0; ui < 10; ui++)
  {
    pH_temp_Sensor.singleReading();
    delay(50);
  }
  pH_temp_Sensor.initValueTemperature(Storage::Eeprom::getTempIntercept());

  #if CALIBRATION_TEMPERATURE
    pH_temp_Sensor.setTempPrecision(0.035);
    while(!pH_temp_Sensor.isTempStable())
    {
      if(pH_temp_Sensor.pHStableCount()>5) break;
      pH_temp_Sensor.getTemperature();
      pH_temp_Sensor.singleReading();
    }
    temperature = pH_temp_Sensor.getTemperature();
    pH_temp_Sensor.calibrationTemperature(temperature_ref_val);
    Storage::Eeprom::writeTempIntercept(pH_temp_Sensor.getTemperatureCalibrationValue());
    delay(50);
  #endif 
}

bool Sensor::Water::getStatusTemperature(void)
{
  #if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_ONEWIRE
    return ds.isConnected(deviceAddress);
  #elif TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE
    return temperature_I2C.getTempStatus();
  #else 
    return true;
  #endif

}

#if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE
bool Sensor::Water::temperatureCalibration(void)
{
  delay(100);
  ds.begin();
  ds.getAddress(deviceAddress, 0);
  ds.setResolution(deviceAddress, 12);
  ds.setWaitForConversion(false);
  ds.requestTemperaturesByAddress(deviceAddress);

  for (uint8_t ui = 0; ui < 3; ui++) {
    if (ds.isConnected(deviceAddress)) break;
    delay(100);
  }
  if (!ds.isConnected(deviceAddress)) return false;
  temperature_I2C.setSlope(0);
  for( uint8_t ii = 0; ii< 250; ii++)
    temperature_I2C.readTemperature();
  temperature = temperature_I2C.readTemperature();
  if ( temperature_I2C.readTemperature() < 0 )return false;
  temperature_I2C.setAdjust( ds.getTempC(deviceAddress));
  if( !Storage::Eeprom::writeTempIntercept( temperature_I2C.getSlope() ) ) return false;
  temperature = temperature_I2C.readTemperature();
  char buffer[2][17], valueBuffer[10];
  sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getTemperature(), 3, 2, valueBuffer));
  Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
  delay(500);
  if( sqrt( pow(temperature_I2C.readTemperature() - ds.getTempC(deviceAddress), 2) ) < 0.1 )
    return true;
  else
    return false;
}
#elif TEMPERATURE_SENSOR_INTERFACE ==_INTERFACE_SERIAL
bool Sensor::Water::temperatureCalibration(void)
{
  delay(100);
  ds.begin();
  ds.getAddress(deviceAddress, 0);
  ds.setResolution(deviceAddress, 12);
  ds.setWaitForConversion(false);
  ds.requestTemperaturesByAddress(deviceAddress);

  for (uint8_t ui = 0; ui < 3; ui++) {
    if (ds.isConnected(deviceAddress)) break;
    delay(100);
  }
  if (!ds.isConnected(deviceAddress)) return false;
  for( uint8_t ii = 0; ii< 255; ii++)
  {
    static unsigned long run_time = millis();
    if(millis() - run_time >= 500UL)
    {
      run_time = millis();
      pH_temp_Sensor.singleReading();
      pH_temp_Sensor.getTemperature();
    }; 
    if(pH_temp_Sensor.isTempStable() || (pH_temp_Sensor.tempStableCount()>5))
      break;
    if( ii == 250 )
      return false;
  }
  temperature = pH_temp_Sensor.getTemperature();
  if ( temperature < 0 )return false;
  if ( pH_temp_Sensor.calibrationTemperature(ds.getTempC(deviceAddress)) )
  {
  it_works:
    pH_temp_Sensor.singleReading();
    char buffer[2][17], valueBuffer[10];
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(pH_temp_Sensor.getTemperature(), 3, 2, valueBuffer));
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
    delay(500);
    if( sqrt( pow(pH_temp_Sensor.getTemperature() - ds.getTempC(deviceAddress), 2) ) < 0.1 )
      return true;
    else
      return false;
  }
  else
  {
    if (pH_temp_Sensor.calibrationTemperature(ds.getTempC(deviceAddress))) goto it_works;     
  }
  return false;
}
#endif

/*float Sensor::Water::getTemperatureValue(void)
{
  static float tempP;
  static float temporaryDum[4];
  static unsigned long tsTemperature = millis();
  // rolling manually
  #if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_ONEWIRE
    //if(getStatusTemperature())
    //{
      tempP = ds.getTempC(deviceAddress);
      ds.requestTemperaturesByAddress(deviceAddress);
      return tempP;
    //}
    //else
        //return sqrt (-1);
  #elif TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE
    tempP = temperature_I2C.readTemperature();
  //#elif TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_SERIAL
    float tempP_ = pH_temp_Sensor.getTemperature();
  #else
    pH_temp_Sensor.singleReading();
    tempP = pH_temp_Sensor.getTemperature();
  #endif
  if( millis() - tsTemperature >= 850 ) // every 500 millis, updating reading sensor
  {
    temporaryDum[3] = temporaryDum[2];
    temporaryDum[2] = temporaryDum[1];
    temporaryDum[1] = temporaryDum[0];
    temporaryDum[0] =  tempP;
    if(getDeviasion(temporaryDum)>1)
      tempP = getLowest(temporaryDum);
    else
      tempP = getMedian(temporaryDum);
    tsTemperature = millis();
  }
  return tempP;
}
*/

void Sensor::Water::electricalConductivityInit(void) {
  #if SYSTEM_DEBUG
    Serial.println(F(" EC init !"));
  #endif
  ECSensor.initialize();
  if(!ECSensor.getStatusDev()) return;
  if (ECSensor.getLedEnabled() != EC_SENSOR_SERIAL_DEBUG) ECSensor.ledEnable(EC_SENSOR_SERIAL_DEBUG);
  if (!ECSensor.getOutputElectricalConductivityEnabled()) ECSensor.outputElectricalConductivityEnable(true);
  if (!ECSensor.getOutputTotalDissolveSolidsEnabled()) ECSensor.outputTotalDissolveSolidsEnable(true);
  if (!ECSensor.getOutputSalinityEnabled()) ECSensor.outputSalinityEnable(true);
  if (!ECSensor.getOutputSpecificGravityOfSeaWaterEnabled()) ECSensor.outputSpecificGravityOfSeaWaterEnable(true);
  if (!ECSensor.getContinuousReadingEnabled()) ECSensor.continuousReadingEnable(true);
  ECSensor.setElectricalConductivityPrecision(50);  //250
}

void Sensor::Water::dissolvedOxygenInit(void) {
  DOSensor.initialize();
  if(!DOSensor.getStatusDev()) return;
  if (DOSensor.getLedEnabled() != DO_SENSOR_SERIAL_DEBUG) DOSensor.ledEnable(DO_SENSOR_SERIAL_DEBUG);
  if (!DOSensor.getOutputPercentageEnabled()) DOSensor.outputPercentageEnable(true);
  if (!DOSensor.getOutputDissolvedOxygenEnabled()) DOSensor.outputDissolvedOxygenEnable(true);
  if (!DOSensor.getContinuousReadingEnabled()) DOSensor.continuousReadingEnable(true);
  DOSensor.pressureCompensation(101.325);
  DOSensor.setDissolvedOxygenPrecision(0.01);

}

void Sensor::Water::dissolvedOxygenCalibratingZero(void)
{
  char buffer[2][17], valueBuffer[10];
  if(!DOSensor.getStatusDev())
  {
    wdt_reset();
    sprintf_P(buffer[1], Indicating::Translation::get(PLEASE_CHECK_SENSOR), (const char*)F("DO"));
    Indicating::Display::info(buffer[1]);
    delay(2000);
    return;
  };
  
  wdt_reset();
  Indicating::Display::info(F(""), Indicating::Translation::get(CALIBRATION_WITH_DOTS), F(""), F(""));
  DOSensor.setDissolvedOxygenPrecision(0.01);
  //===================== to get temperature compensation ====================
  unsigned int timeoutCalibration = Indicating::RTC::getSecondTime();
  // warming up sensor
  do {
    wdt_reset();
    DOSensor.continuousReading(EZO_Continuous_reading);
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getDissolvedOxygen(), 6, 3, valueBuffer));
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
  } while (Indicating::RTC::getSecondTime() - timeoutCalibration < 60);
  
  DOSensor.clearStableCount();
  timeoutCalibration = Indicating::RTC::getSecondTime();
  // starting to calibrate
  do {
    wdt_reset();
    DOSensor.continuousReading(EZO_Continuous_reading);
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getDissolvedOxygen(), 6, 3, valueBuffer));
    sprintf_P(buffer[1], (const char *)F("           %3d%%%%"), DOSensor.getDissolvedOxygenStableCount() * 10);
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], buffer[1]);
  } while (!isDissolvedOxygenStable() || (Indicating::RTC::getSecondTime() - timeoutCalibration < 25));
  if( getDissolvedOxygen() < 1 )
    DOSensor.calibrationZero();
  else  
  {
    DOSensor.setDissolvedOxygenPrecision(0.05);
    return;
  }
  timeoutCalibration = Indicating::RTC::getSecondTime();
  do {
    wdt_reset();
    DOSensor.continuousReading(EZO_Continuous_reading);
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getDissolvedOxygen(), 6, 3, valueBuffer));
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
  } while (Indicating::RTC::getSecondTime() - timeoutCalibration < 5);
  DOSensor.setDissolvedOxygenPrecision(0.05);
}


void Sensor::Water::dissolvedOxygenCalibratingAtmospheric(void) {
  //unsigned long t = 0;
  char buffer[2][17], valueBuffer[10];
  if(!DOSensor.getStatusDev())
  {
    wdt_reset();
    sprintf_P(buffer[1], Indicating::Translation::get(PLEASE_CHECK_SENSOR), (const char*)F("DO"));
    Indicating::Display::info(buffer[1]);
    delay(2000);
    return;
  };
  
  wdt_reset();
  Indicating::Display::info(F(""), Indicating::Translation::get(CALIBRATION_WITH_DOTS), F(""), F(""));
  //DOSensor.factoryReset();
  //DOSensor.calibrationClear();
  //dissolvedOxygenInit();
  //===================== to get temperature compensation ====================
    pH_temp_Sensor.singleReading();
    temperature =  pH_temp_Sensor.getTemperature();
  DOSensor.temperatureCompensation(temperature);

  unsigned int timeoutCalibration = Indicating::RTC::getSecondTime();
  // warming up sensor
  do {
    wdt_reset();
    DOSensor.continuousReading(EZO_Continuous_reading);
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getDissolvedOxygen(), 6, 3, valueBuffer));
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
  } while (Indicating::RTC::getSecondTime() - timeoutCalibration < 60);
  
  DOSensor.clearStableCount();
  timeoutCalibration = Indicating::RTC::getSecondTime();
  // starting to calibrate
  do {
    wdt_reset();
    DOSensor.continuousReading(EZO_Continuous_reading);
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getDissolvedOxygen(), 6, 3, valueBuffer));
    sprintf_P(buffer[1], (const char *)F("           %3d%%%%"), DOSensor.getDissolvedOxygenStableCount() * 10);
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], buffer[1]);
  } while (!isDissolvedOxygenStable() || (Indicating::RTC::getSecondTime() - timeoutCalibration < 25));
  DOSensor.calibrationAtmospheric();
  timeoutCalibration = Indicating::RTC::getSecondTime();
  do {
    wdt_reset();
    DOSensor.continuousReading(EZO_Continuous_reading);
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getDissolvedOxygen(), 6, 3, valueBuffer));
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
  } while (Indicating::RTC::getSecondTime() - timeoutCalibration < 5);
}


void Sensor::Water::electricalConductivityCalibratingDry(void) {
  if(!ECSensor.getStatusDev()) return;
  ECSensor.setElectricalConductivityPrecision(250.0f);    //default 500
  char buffer[2][17], valueBuffer[10];
  wdt_reset();
  Indicating::Display::info(F(""), Indicating::Translation::get(CALIBRATION_WITH_DOTS), F(""), F(""));
  ECSensor.factoryReset();
  ECSensor.calibrationClear();
  wdt_reset();
  electricalConductivityInit();
  //===================== to get temperature compensation ====================
    pH_temp_Sensor.singleReading();
    temperature =  pH_temp_Sensor.getTemperature();

  ECSensor.temperatureCompensation(temperature);
  unsigned int timeoutCalibration = Indicating::RTC::getSecondTime();
  do {
    wdt_reset();
    ECSensor.continuousReading(EZO_Continuous_reading);
    wdt_reset();
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getElectricalConductivity(), 6, 3, valueBuffer));
    sprintf_P(buffer[1], (const char *)F("           %3d%%%%"), ECSensor.getElectricalConductivityStableCount() * 10);
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], buffer[1]);
  } while (!isElectricalConductivityStable() && (Indicating::RTC::getSecondTime() - timeoutCalibration < 10));
  wdt_reset();
  ECSensor.calibrationDry();
  timeoutCalibration = Indicating::RTC::getSecondTime();
  do {
    wdt_reset();
    ECSensor.continuousReading(EZO_Continuous_reading);
    wdt_reset();
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getElectricalConductivity(), 6, 3, valueBuffer));
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
  } while (Indicating::RTC::getSecondTime() - timeoutCalibration < 5);
}

void Sensor::Water::electricalConductivityCalibratingLow(void) {
  wdt_reset();
  if(!ECSensor.getStatusDev()) return;
  char buffer[2][17], valueBuffer[10];
  ECSensor.clearStableCount();
  unsigned int timeoutCalibration = Indicating::RTC::getSecondTime();
  do {
    wdt_reset();
    ECSensor.continuousReading(EZO_Continuous_reading);
    wdt_reset();
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getElectricalConductivity(), 6, 3, valueBuffer));
    sprintf_P(buffer[1], (const char *)F("           %3d%%%%"), ECSensor.getElectricalConductivityStableCount() * 10);
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], buffer[1]);
    delay(100);
  } while (!isElectricalConductivityStable() && (Indicating::RTC::getSecondTime() - timeoutCalibration < 25));
  if (ECSensor.getElectricalConductivity() < 5000.0f) {
    ECSensor.probeType(10.0f);
  }
  wdt_reset();
  ECSensor.calibrationLow(12880);
  timeoutCalibration = Indicating::RTC::getSecondTime();
  do {
    wdt_reset();
    ECSensor.continuousReading(EZO_Continuous_reading);
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getElectricalConductivity(), 6, 3, valueBuffer));
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
    delay(100);
  } while (Indicating::RTC::getSecondTime() - timeoutCalibration < 5);
}

void Sensor::Water::electricalConductivityCalibratingHigh(void) {
  wdt_reset();
  if(!ECSensor.getStatusDev()) return;
  //unsigned long timeoutCalib = 0;
  char buffer[2][17], valueBuffer[10];
  ECSensor.clearStableCount();
  unsigned int timeoutCalibration = Indicating::RTC::getSecondTime();
  do {
    wdt_reset();
    ECSensor.continuousReading(EZO_Continuous_reading);
    wdt_reset();
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getElectricalConductivity(), 6, 3, valueBuffer));
    sprintf_P(buffer[1], (const char *)F("           %3d%%%%"), ECSensor.getElectricalConductivityStableCount() * 10);
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], buffer[1]);
    delay(100);
  } while (!isElectricalConductivityStable() && (Indicating::RTC::getSecondTime() - timeoutCalibration < 25));
  wdt_reset();
  ECSensor.calibrationHigh(80000);
  timeoutCalibration = Indicating::RTC::getSecondTime();
  do {
    wdt_reset();
    ECSensor.continuousReading(EZO_Continuous_reading);
    wdt_reset();
    sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getElectricalConductivity(), 6, 3, valueBuffer));
    Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
  } while (Indicating::RTC::getSecondTime() - timeoutCalibration < 5);

  ECSensor.setElectricalConductivityPrecision(100.0f);
}

void Sensor::Water::pHInit(void) {
  //char textBuff[10];
  pH_temp_Sensor.begin();
  pHCalibrationValue = loadPHCalibrationValue();
  pH_temp_Sensor.initValuePH(pHCalibrationValue);
  pH_temp_Sensor.setPHPrecision(0.05f);
}

boolean Sensor::Water::pHCalibratingMid(void) {
  unsigned long te = 0;
  char buffer[2][17], valueBuffer[10];
  Indicating::Display::info(F(""), Indicating::Translation::get(CALIBRATION_WITH_DOTS), F(""), F(""));
#if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_I2C
  pH_temp_Sensor.resetParam();
  pH_temp_Sensor.singleReading();
  pH_temp_Sensor.setPHPrecision(pHCalibrationPrecision);
  pH_temp_Sensor.resetPHStable();
#else
  pHsensor_.setPrecisionOfPH(std_deviasion);
  pHsensor_.resetPHStable();
#endif

  unsigned int timeoutCalibration = Indicating::RTC::getSecondTime();
  do {
        if( millis() - te > pHCalibrationInterval)
        {
          te = millis();
          temperature = pH_temp_Sensor.getTemperature();
          #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
              pHsensor_.singleRunning();//pHSensor.continuousReading();
          #elif PH_SENSOR_INTERFACE == SENSOR_INTERFACE_I2C
              pH_temp_Sensor.singleReading();
          #endif
          sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getpH(), 6, 2, valueBuffer));
          Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
          delay(10);
          wdt_reset();
        };
  } while  (Indicating::RTC::getSecondTime() - timeoutCalibration < 10);
  
  pH_temp_Sensor.resetPHStable();
  timeoutCalibration = Indicating::RTC::getSecondTime();
  te = millis();
  do {
    if( millis() - te > pHCalibrationInterval)
    {
      te = millis();
          pH_temp_Sensor.singleReading();
          temperature = pH_temp_Sensor.getTemperature();
      sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getpH(), 6, 2, valueBuffer));
      sprintf_P(buffer[1], (const char *)F("           %3d%%%%"), pH_temp_Sensor.pHStableCount() * 5);
      Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], buffer[1]);
      wdt_reset();
      if(isnan(getpH())) break;
    }
    System::SoftPower::loop(false);
    if(System::SoftPower::isPressed())
    {
      Indicating::Display::info(t(PH_CALIBRATION_CANCELLED));
      pH_temp_Sensor.initValuePH(pHCalibrationValue);
      delay(1500);
      return false;
    }
  } while (!ispHStable() && (Indicating::RTC::getSecondTime() - timeoutCalibration < 300));
  
  if(ispHStable() || (pH_temp_Sensor.pHStableCount() > 5) )
  {
  
     if (! pH_temp_Sensor.calibrationPH(MID_PH, 7.00f ) ) 
     {
       pH_temp_Sensor.singleReading();
       if (! pH_temp_Sensor.calibrationPH(MID_PH, 7.00f ))
       {
          Indicating::Display::info(t(PH_CALIBRATION_FAILED));
          pH_temp_Sensor.initValuePH(pHCalibrationValue);
          delay(1200);
          return false;
       };
     };
     pH_temp_Sensor.resetPHStable();
     pH_temp_Sensor.singleReading();
     temperature = pH_temp_Sensor.getTemperature();
    timeoutCalibration = Indicating::RTC::getSecondTime();
    te = millis();
    do {
        if( millis() - te >= pHCalibrationInterval)
        {
          te = millis();
          #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
            pHsensor_.singleRunning();
            //t_ph =pHsensor_.getPH();//pHSensor.continuousReading();
          #elif PH_SENSOR_INTERFACE == SENSOR_INTERFACE_I2C
            pH_temp_Sensor.singleReading();
          #endif
          sprintf_P(buffer[0], (const char *)F("    %6s     "), dtostrf(getpH(), 6, 2, valueBuffer));
          Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
          wdt_reset();
        }
    } while  ( (pH_temp_Sensor.pHStableCount() < 1 ) || (Indicating::RTC::getSecondTime() - timeoutCalibration < 10) );
    delay(1500);
    return true;    
  }
  else
  { 
    Indicating::Display::info(t(PH_CALIBRATION_FAILED));
    pH_temp_Sensor.initValuePH(pHCalibrationValue);
    delay(1200);
    return false;
  }
    
}

#if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
  void Sensor::Water::pHCalibratingLow(void) {
    unsigned long t = 0;
    char buffer[2][17], valueBuffer[10];
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_ANALOG_I2C
  //===================== to get temperature compensation ====================
    #if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_ONEWIRE
      temperature = ds.getTempC(deviceAddress);
      ds.requestTemperaturesByAddress(deviceAddress);
    #elif TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE
      temperature = temperature_I2C.readTemperature();
    #endif
    pHSensor.temperatureCompensation(temperature);
  #endif
    t = millis();
    do {
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
      //pHsensor_.getPH();//pHSensor.continuousReading();
  #elif PH_SENSOR_INTERFACE == SENSOR_INTERFACE_ANALOG_I2C
      pHSensor.singleReading();
      delay(pHCalibrationInterval);
  #endif
      sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getpH(), 6, 2, valueBuffer));
      sprintf_P(buffer[1], (const char *)F("           %3d%%%%"), pHsensor_.getpHStableCount() * 10);
      Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], buffer[1]);
    } while (!ispHStable() || (millis() - t < 10000));
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
    pHsensor_.pHCalibrationLow();//pHSensor.calibrationLow(4.010f);//, pHSensor.getTemperature());
  #elif PH_SENSOR_INTERFACE == SENSOR_INTERFACE_ANALOG_I2C
    pHSensor.calibrationLow(4.010f, pHSensor.getTemperature());
    savePHCalibrationValue(pHSensor.getCalibrationValue());
  #endif
    t = millis();
    do {
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
      //pHsensor_.getPH();//pHSensor.continuousReading();
  #elif PH_SENSOR_INTERFACE == SENSOR_INTERFACE_ANALOG_I2C
      pHSensor.singleReading();
      delay(pHCalibrationInterval);
  #endif
      sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getpH(), 6, 4, valueBuffer));
      Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
    } while (millis() - t < 3500);
    //===================== to get temperature compensation ====================
      #if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_ONEWIRE
        temperature = ds.getTempC(deviceAddress);
        ds.requestTemperaturesByAddress(deviceAddress);
      #elif TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE
        temperature = temperature_I2C.readTemperature();
      #endif
      //pHSensor.temperatureCompensation(temperature);
  }

  void Sensor::Water::pHCalibratingHigh(void) {
    unsigned long t = 0;
    char buffer[2][17], valueBuffer[10];
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_ANALOG_I2C

  //===================== to get temperature compensation ====================
    #if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_ONEWIRE
      temperature = ds.getTempC(deviceAddress);
      ds.requestTemperaturesByAddress(deviceAddress);
    #elif TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE
      temperature = temperature_I2C.readTemperature();
    #endif

    pHSensor.temperatureCompensation(temperature);
  #endif

    t = millis();
    do {
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
      pHsensor_.getPH();//pHSensor.continuousReading();
  #elif PH_SENSOR_INTERFACE == SENSOR_INTERFACE_ANALOG_I2C
      pHSensor.singleReading();
      delay(pHCalibrationInterval);
  #endif
      sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getpH(), 6, 2, valueBuffer));
      sprintf_P(buffer[1], (const char *)F("           %3d%%%%"), pHsensor_.getpHStableCount() * 10);
      Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], buffer[1]);
    } while (!ispHStable() || (millis() - t < 10000));

  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
    pHsensor_.pHCalibrationHigh();//calibrationHigh(10.010f);
  #elif PH_SENSOR_INTERFACE == SENSOR_INTERFACE_ANALOG_I2C
    pHSensor.calibrationHigh(10.010f, pHSensor.getTemperature());
    savePHCalibrationValue(pHSensor.getCalibrationValue());
  #endif

    t = millis();
    do {
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
      pHsensor_.getPH();//pHSensor.continuousReading();
  #elif PH_SENSOR_INTERFACE == SENSOR_INTERFACE_ANALOG_I2C
      pHSensor.singleReading();
      delay(pHCalibrationInterval);
  #endif
      sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getpH(), 6, 2, valueBuffer));
      Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
    } while (millis() - t < 3000);
  }
#endif

bool Sensor::Water::pHCalibratingLowHigh(void) {
  unsigned long te = 0;
  char buffer[2][17], valueBuffer[10];
  Indicating::Display::info(F(""), Indicating::Translation::get(CALIBRATION_WITH_DOTS), F(""), F(""));
#if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_I2C
  pH_temp_Sensor.singleReading();
  pH_temp_Sensor.resetPHStable();
  pH_temp_Sensor.setPHPrecision(pHCalibrationPrecision);
#endif
  unsigned int timeoutCalibration = Indicating::RTC::getSecondTime();
  // warming up()
  do {
        if( millis() - te > pHCalibrationInterval)
        {
          te = millis();
          temperature = pH_temp_Sensor.getTemperature();
          #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
              pHsensor_.singleRunning();//pHSensor.continuousReading();
          #elif PH_SENSOR_INTERFACE == SENSOR_INTERFACE_I2C
              pH_temp_Sensor.singleReading();
          #endif
          sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getpH(), 6, 2, valueBuffer));
          Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
          delay(10);
          wdt_reset();
        };
  } while  (Indicating::RTC::getSecondTime() - timeoutCalibration < 10);
  
  pH_temp_Sensor.resetPHStable();
  timeoutCalibration = Indicating::RTC::getSecondTime();
  te = millis();
  do {
        if( millis() - te > pHCalibrationInterval)
        {
          te = millis();
          temperature = pH_temp_Sensor.getTemperature();
          #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
              pHsensor_.singleRunning();//pHSensor.continuousReading();
          #elif PH_SENSOR_INTERFACE == SENSOR_INTERFACE_I2C
              pH_temp_Sensor.singleReading();
          #endif
          sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getpH(), 6, 2, valueBuffer));
          sprintf_P(buffer[1], (const char *)F("           %3d%%%%"), pH_temp_Sensor.pHStableCount() * 5);
          Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], buffer[1]);
          delay(10);
          wdt_reset();
          if(isnan(getpH())) break;
        };
        
        System::SoftPower::loop(false);
        if(System::SoftPower::isPressed())
        {
          Indicating::Display::info(t(PH_CALIBRATION_CANCELLED));
          pH_temp_Sensor.initValuePH(pHCalibrationValue);
          delay(1500);
          return false;
        }
  } while (!ispHStable() && (Indicating::RTC::getSecondTime() - timeoutCalibration < 600));
  if(ispHStable() || (pH_temp_Sensor.pHStableCount() > 4) )
  {
  
    #if PH_SENSOR_INTERFACE != SENSOR_INTERFACE_I2C
      if (getpH() < 7.5) {
        pHsensor_.pHCalibrationLow();//pHSensor.calibrationLow(4.001f);
      } else {
        pHsensor_.pHCalibrationHigh();//pHSensor.calibrationHigh(10.001f);
      }
      delay(100);
      pHsensor_.pHCalibrationAction();
    #else
      if (getpH() < 7.01 ) {
        pH_temp_Sensor.calibrationPH(LOW_PH, 4.01f);//calibrationLow(4.01f, temperature);
      } else {
        pH_temp_Sensor.calibrationPH(HIGH_PH, 10.00f);
      }
    #endif
  }
  else
  {
    Indicating::Display::info(t(PH_CALIBRATION_FAILED));
    pH_temp_Sensor.initValuePH(pHCalibrationValue);
    delay(1200);
    return false;
  }
      
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_I2C
    pH_temp_Sensor.resetPHStable();
    pH_temp_Sensor.setPHPrecision(0.05f);
    savePHCalibrationValue(pH_temp_Sensor.getPHCalibrationValue());
   // pH_temp_Sensor.initValuePH(loadPHCalibrationValue());
  #endif

  timeoutCalibration = Indicating::RTC::getSecondTime();
  te = millis();
  do {
        if( millis() - te >= pHCalibrationInterval)
        {
          te = millis();
          #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
              pHsensor_.singleRunning();
          #elif PH_SENSOR_INTERFACE == SENSOR_INTERFACE_I2C
              pH_temp_Sensor.singleReading();
          #endif
          sprintf_P(buffer[0], (const char *)F("     %6s     "), dtostrf(getpH(), 6, 2, valueBuffer));
          Indicating::Display::info(F(""), Indicating::Translation::get(PLEASE_WAIT), buffer[0], F(""));
          wdt_reset();
        };
        System::SoftPower::loop(false);
        if(System::SoftPower::isPressed())
            break;
  } while (  (pH_temp_Sensor.pHStableCount() < 1 ) || (Indicating::RTC::getSecondTime() - timeoutCalibration < 10) );
  delay(1500);
  return true;
}


bool Sensor::Water::isSleep(void) {
  return sleep;
}

bool Sensor::Water::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void Sensor::Water::setSleep(bool sleep) {
  sleepBuffer = sleep;
}


void Sensor::Water::procFlow(uint8_t idx)
{
  switch(idx)
  {
    case 0 :  //pHSensor.singleReading();
              break;
    case 1 :  
              if(ECSensor.getStatusDev())
                ECSensor.continuousReading();
              break;
    case 2 :  
              if(DOSensor.getStatusDev())
                DOSensor.continuousReading();
              break;

    case 3 :  
              if (ds.isConnected(deviceAddress))
              {
                temperature = ds.getTempC(deviceAddress);
                ds.requestTemperaturesByAddress(deviceAddress);
              }
              else
                  temperature = sqrt (-1); 
              break;
    default:
              break;
  }
}