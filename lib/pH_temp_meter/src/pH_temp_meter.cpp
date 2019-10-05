#include "pH_temp_meter.h"

/* StabilityDetector Class */
StabilityDetector_::StabilityDetector_(float precision)
{
  this->stable = false;
  this->stableCount = 0;
  this->index = 0;
  for (size_t i = 0; i < arrayLength(valueBuffer); i++)
    this->valueBuffer[i] = 0.00f;
  this->precision = precision;
}

void StabilityDetector_::updateValue(float value) {
  valueBuffer[index] = value;
  if(++index == 10) index = 0;
  if( getDeviasion(valueBuffer) <= precision ) stableCount++;
  else stableCount = 0;
  stable = (stableCount > 20);
  if (stable) stableCount = 20;
}

void StabilityDetector_::reset(void)
{
    this->stable = false;
  this->stableCount = 0;
  this->index = 0;
  for (size_t i = 0; i < arrayLength(valueBuffer); i++)
    this->valueBuffer[i] = 0.00f;
}


int pH_temp_meter::available(void) {
  return this->com->available();
}


void pH_temp_meter::flush(void) {
  this->com->flush();
}

size_t pH_temp_meter::write(uint8_t c)
{
  return this->com->write(c);
}

size_t pH_temp_meter::write(const char c) {
  return this->com->write(c);
}

size_t pH_temp_meter::write(const char str[]) {
  return this->com->write(str);
}

size_t pH_temp_meter::write(const __FlashStringHelper *str) {
  char buffer[strlen_P((const char *)str) + 1];
  strcpy_P(buffer, (const char *)str);
  return write(buffer);
}

size_t pH_temp_meter::readBytes(char buffer[], size_t length) {
  return this->com->readBytes(buffer, length);
}

int pH_temp_meter::read (void)
{
  return this->com->read();  
}

int pH_temp_meter::peek(void)
{
  return this->com->peek();
}

pH_temp_meter::pH_temp_meter(SoftwareSerial &com_)
{
    this->com = &com_;
    //this->com->setTimeout(1000);
}

pH_temp_meter&pH_temp_meter::begin(void)
{
    this->bufferMsg.reserve(100);
    this->com->flush();
    return *this;
}

void pH_temp_meter::clearBuffer(void)
{
    this->bufferMsg="";
}

bool pH_temp_meter::getResponse(unsigned long timeout__)
{
    unsigned long time_out = millis();
    if(!this->com->isListening())
        this->com->listen();

    while(true)
    {
        while(available())
        {
            char inChar = (char)read();
            this->bufferMsg += inChar;
            if ( (inChar == '\n') || (inChar == '\r') ) 
            {
              if(this->bufferMsg.length() < 2)
              {
                this->bufferMsg = "";
                return false;
              }
              this->bufferMsg.trim();
              this->bufferMsg.toLowerCase();
              Serial.println(" receive msg : "+this->bufferMsg);
              return true;
            }
        }
        if(millis() - time_out >= timeout__)
            return false;
    };
}

bool pH_temp_meter::parsingDataPHTemp(void)
{
  if(!this->bufferMsg.length() > 0) return false;
  int len = this->bufferMsg.length()+1;
  char buff[len];
  this->bufferMsg.trim(); 
  this->bufferMsg.toLowerCase();
  this->bufferMsg.toCharArray(buff,len);
  if(strstr_P((const char *)buff, (const char *)F("p,")) != NULL)        // get data ph
  {
    char *pointer = strstr_P((const char *)buff,(const char *)F("p")) + strlen_P((const char *)F("p"));
    char *str = strtok_P(pointer, (const char *)F(","));
    for (uint8_t i = 0; i < 2 && str != NULL; i++) {
      if(i==0)
      {
        this->pH = atof(str);
        this->pHUpdated = true;
        return true;
      }
      str = strtok_P(NULL, (const char *)F(","));
    };
  }

  if(strstr_P((const char *)buff, (const char *)F("t,")) != NULL)        // get data temperature
  {
    char *pointer = strstr_P((const char *)buff,(const char *)F("t")) + strlen_P((const char *)F("t"));
    char *str = strtok_P(pointer, (const char *)F(","));
    for (uint8_t i = 0; i < 2 && str != NULL; i++) {
      if(i==0)
      {
        this->temperature = atof(str);
        this->temperatureUpdated = true;
        return true;
      }
      str = strtok_P(NULL, (const char *)F(","));
    };
  }

  if(strstr_P((const char *)buff, (const char *)F("a,")) != NULL)        // get data temperature
  {
    char *pointer = strstr_P((const char *)buff,(const char *)F("a")) + strlen_P((const char *)F("a"));
    char *str = strtok_P(pointer, (const char *)F(","));
    for (uint8_t i = 0; i < 2 && str != NULL; i++) {
      if( i == 1)
      {
        this->temperature = atof(str);
        this->temperatureUpdated = true;
      }
      if(i == 0)
      {
        this->pH = atof(str);
        this->pHUpdated = true;
      }
      if (this->temperatureUpdated && this->pHUpdated ) return true;
      str = strtok_P(NULL, (const char *)F(","));
    };
  }

  return false;
}

float pH_temp_meter::getPH(void)
{
  if(this->justSingleRun )
  {
    this->justSingleRun = false;
    Serial.println("nilai pH : "+String(this->pH)+"\t deviasi :"+String(this->pHbuffer.getDeviasionValue(),5));
    return this->pH;
  }
    this->pHUpdated = false;
    //Serial.println("request pH value");
    clearBuffer();
    write(F("read,ph\n"));
    if(!getResponse()) return this->pH;
    if(!parsingDataPHTemp()) return this->pH;
    if(!this->pHUpdated) return this->pH;
    this->pHbuffer.updateValue(this->pH);
    Serial.println("nilai pH : "+String(this->pH)+"\t deviasi :"+String(this->pHbuffer.getDeviasionValue(),5));
    return this->pH;
}

float pH_temp_meter::getTemperature(void)
{
  if(this->justSingleRun )
  {
    this->justSingleRun = false;
    //Serial.println("nilai temperature : "+String(this->temperature)+"\t deviasi :"+String(this->temperatureBuffer.getDeviasionValue(),5));
    return this->temperature;
  }
    this->temperatureUpdated = false;
    //Serial.println("request temperature value");
    clearBuffer();
    write(F("read,temp\n"));
    if(!getResponse()) return this->temperature;
    if(!parsingDataPHTemp()) return this->temperature;
    if(!this->temperatureUpdated) return this->temperature;
    this->temperatureBuffer.updateValue(this->temperature);
    //Serial.println("nilai temperature : "+String(this->temperature)+"\t deviasi :"+String(this->temperatureBuffer.getDeviasionValue(),5));
    return this->temperature;
}

float pH_temp_meter::getPHVoltage(void)
{
    float voltage = 0;
    clearBuffer();
    write(F("read,voltage\n"));
    if(!getResponse()) return voltage;
    voltage = this->bufferMsg.toFloat(); 
    return voltage;
}

float pH_temp_meter::getTemperatureResistance(void)
{
    float resistance = -1;
    clearBuffer();
    write(F("read,resistance\n"));
    if(!getResponse()) return resistance;
    resistance = this->bufferMsg.toFloat(); 
    return resistance;
}

uint8_t pH_temp_meter::getCalibrationInfo(void)
{
    uint8_t info_ = 1;
    clearBuffer();
    write(F("cal_ph,?\n"));
    if(!getResponse()) return info_;
    info_ = this->bufferMsg.toInt(); 
    return info_;
}

bool pH_temp_meter::setSleep(bool sleep_)
{
    clearBuffer();
    if(sleep_)
        write(F("set_sleep,on\n"));
    else
        write(F("set_sleep,off\n"));
    if(!getResponse()) return false;
    return true;
}

bool pH_temp_meter::pHCalibrationStart(void)
{
    clearBuffer();
    write(F("cal_ph,start\n"));
    if(!getResponse()) return false;
    char tmp[this->bufferMsg.length()+1];
    this->bufferMsg.toCharArray(tmp,this->bufferMsg.length()+1);
    if (strstr_P((const char *)tmp, (const char *)F("calibration open")) != NULL)
        return true;
    else
        return false;
}

bool pH_temp_meter::pHCalibrationAction(void)
{
    clearBuffer();
    write(F("cal_ph,action\n"));
    if(!getResponse()) return false;
    char tmp[this->bufferMsg.length()+1];
    this->bufferMsg.toCharArray(tmp,this->bufferMsg.length()+1);
    if (strstr_P((const char *)tmp, (const char *)F("ok")) != NULL)
        return true;
    else
        return false;
}

bool pH_temp_meter::pHCalibrationCancel(void)
{
  clearBuffer();
    write(F("cal_ph,cancel\n"));
    if(!getResponse()) return false;
    char tmp[this->bufferMsg.length()+1];
    this->bufferMsg.toCharArray(tmp,this->bufferMsg.length()+1);
    if (strstr_P((const char *)tmp, (const char *)F("ok")) != NULL)
        return true;
    else
        return false;
}

bool pH_temp_meter::pHCalibrationMid(void)
{
    clearBuffer();
    write(F("cal_ph,mid\n"));
    if(!getResponse()) return false;
    char tmp[this->bufferMsg.length()+1];
    this->bufferMsg.toCharArray(tmp,this->bufferMsg.length()+1);
    if (strstr_P((const char *)tmp, (const char *)F("ok")) != NULL)
        return true;
    else
        return false;
}

bool pH_temp_meter::pHCalibrationLow(void)
{
    clearBuffer();
    write(F("cal_ph,low\n"));
    if(!getResponse()) return false;
    char tmp[this->bufferMsg.length()+1];
    this->bufferMsg.toCharArray(tmp,this->bufferMsg.length()+1);
    if (strstr_P((const char *)tmp, (const char *)F("ok")) != NULL)
        return true;
    else
        return false;
}

bool pH_temp_meter::pHCalibrationHigh(void)
{
    clearBuffer();
    write(F("cal_ph,high\n"));
    if(!getResponse()) return false;
    char tmp[this->bufferMsg.length()+1];
    this->bufferMsg.toCharArray(tmp,this->bufferMsg.length()+1);
    if (strstr_P((const char *)tmp, (const char *)F("ok")) != NULL)
        return true;
    else
        return false;
}

bool pH_temp_meter::temperatureCalibration(float temp_ref)
{
    clearBuffer();
    char data[24];
    char valueBuffer[10];
      sprintf_P(data, (const char *)F("cal_temp,%s\n"), dtostrf(temp_ref, 6, 2, valueBuffer));
    write(data); 
    Serial.println(data);
    if(!getResponse()) return false;
    char tmp[this->bufferMsg.length()+1];
    this->bufferMsg.toCharArray(tmp,this->bufferMsg.length()+1);
    Serial.println(this->bufferMsg);
    if (strstr_P((const char *)tmp, (const char *)F("ok")) != NULL)
        return true;
    else
        return false;
}

void pH_temp_meter::singleRunning(void)
{
  this->temperatureUpdated = false;
  this->pHUpdated          = false;
  //Serial.println("request pH & temperature value");
    clearBuffer();
    write(F("read,all\n"));
    if(!getResponse()) return;
    if(!parsingDataPHTemp()) return;
    if(!this->temperatureUpdated && !this->pHUpdated) return;
    this->temperatureBuffer.updateValue(this->temperature);
    this->pHbuffer.updateValue(this->pH);
    this->justSingleRun = true;
}