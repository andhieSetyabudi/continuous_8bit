#include "analogPH.h"
#define prec    4

#define alp     0.25f


#define alp_max 0.1f
#define alp_min 0.01f

#define ts_max  1500
#define ts_min  500

float EMA_function(float alpha, float latest, float stored){
  return roundF(alpha*latest,6) + roundF((1-alpha)*stored,6);
}

float Calculate_Temp(float R)
{
  const float A = 0.0039083;
  const float B = -0.0000005775;
  float T ;//= - ( sqrt( -0.00232f * R + 17.59246f)-3.908f ) / 0.00116f;
  T = 0.0 - A;
  T += sqrt((A * A)-4.0 * B * (1.0 - R));
  T /= 2.0 * B;
  return T;
}

float getAlpha(unsigned long mills, float cut_off)
{
  float dt   = (float) mills/1000.0;
  if ( (1/dt) < cut_off) cut_off = (1/dt)/2; 
  float RC   = 1/(2*PI*cut_off);
  float alpha = dt/(dt+RC);
  alpha /= 8;
  return roundF(alpha,4);
}

float getSlope(float x1, float x2, float val1, float val2)
{
  float zigY = val1 + val2;
  float zigX  = x1 +  x2;
  float zigXY= (val1*x1)+(val2*x2);
  float zigX2 = pow(x1,2)+pow(x2,2);
  float slope_ = ( 2*zigXY - zigY*zigX )/ ( 2*zigX2 - pow(zigX, 2) );
  return roundF(slope_,5);
}

float getIntercept(float x1, float x2, float val1, float val2)
{
  float zigY  = val1 +  val2;
  float zigX  = x1 +  x2;
  float zigXY= (val1*x1)+(val2*x2);
  float zigX2 = pow(x1,2)+pow(x2,2);
  float intercept_ = ( zigY*zigX2 - zigX*zigXY )/ ( 2*zigX2 - pow(zigX, 2) );
  return  roundF(intercept_,5);
}


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
    //Serial.println("==================== deviasion : "+String(getDeviasion(valueBuffer),5)+"\t stable count : "+String(stableCount));
}

void StabilityDetector_::reset(void)
{
    this->stable = false;
	this->stableCount = 0;
	this->index = 0;
	for (size_t i = 0; i < arrayLength(valueBuffer); i++)
		this->valueBuffer[i] = 0.00f;
}

// constructor 
analogPH::analogPH(softI2C  &line, float volt_low, float volt_mid)
{
    this->line = &line;
    this->defaultLow = volt_low;
    this->defaultMid = volt_mid;
    ADCLine.init(line, ADCLine.L, ADCLine.L);
    resetParam();
}

bool analogPH::begin()
{
    line->beginTransmission(ADCLine.getAddress());
    this->ema_a_rtd = 1.8416f;
    this->ema_b_rtd = 1.8416f;
    
    this->ema_a_ph = 0.0143f;
    this->ema_b_ph = 0.0143f;

    if(line->endTransmission() ==0) 
    {
        this->resetParam();
        return true ;
    }
    else 
      return false;
}

analogPH & analogPH::resetParam(void)
{
    
    this->phStableCount.reset();
    this->pHVoltStable.reset();
    this->tempStable.reset();

    this->phStableCount.setPrecision(0.0075f);
    this->pHVoltStable.setPrecision(0.00025f);
    this->tempStable.setPrecision(0.1f);
 
    this->intercept = getIntercept(this->defaultMid,this->defaultLow,7.00,4.01);
    this->slope = getSlope(this->defaultMid,this->defaultLow,7.00,4.01);
    this->calibrationVal={this->slope, this->intercept,  2};
    return *this;
}

float analogPH::getPHSignal(void)
{
    ADCLine.selectChannel(ADCLine.CHANNEL_0, ADCLine.GAIN_1, ADCLine.MODE_UNIPOLAR);
    delayMicroseconds(10);
    float alpha_ = getAlpha(millis() - this->run_time_ph, 8);
    this->run_time_ph = millis();
    this->ema_a_ph = EMA_function(alpha_,roundF((float)ADCLine.readADC(),prec), this->ema_a_ph);    // basic EMA
    this->ema_b_ph = EMA_function(alpha_,this->ema_a_ph, this->ema_b_ph);
    //this->ema_c_ph = EMA_function(alpha_,this->ema_b_ph, this->ema_c_ph);
    this->voltage.ph_signal = roundF(2*this->ema_a_ph - this->ema_b_ph, prec);    // DEMA filter
    //this->voltage.ph_signal = roundF(3*this->ema_a_ph - 3*this->ema_b_ph + this->ema_c_ph, prec); // TEMA filter
    return this->voltage.ph_signal;
}

float analogPH::getRTDSignal(void)
{
    ADCLine.selectChannel(ADCLine.CHANNEL_1, ADCLine.GAIN_1, ADCLine.MODE_UNIPOLAR);
    delayMicroseconds(10);
    float alpha_ = getAlpha(millis() - this->run_time_rtd, 100);
    this->run_time_rtd = millis();

    this->ema_a_rtd = EMA_function(alpha_,roundF((float)ADCLine.readADC(),prec), this->ema_a_rtd);    // basic EMA
    this->ema_b_rtd = EMA_function(alpha_,this->ema_a_rtd, this->ema_b_rtd);
    //this->ema_c_rtd = EMA_function(alpha_,this->ema_b_rtd, this->ema_c_rtd);
    
    this->voltage.rtd_signal = roundF(2*this->ema_a_rtd - this->ema_b_rtd, prec);   // DEMA filter
    //this->voltage.rtd_signal = roundF(3*this->ema_a_rtd  - 3*this->ema_b_rtd + this->ema_c_rtd, prec);
    Serial.println(this->voltage.rtd_signal,5);
    Serial.flush();
    return this->voltage.rtd_signal;
}

float analogPH::getRefVoltage(void)
{
    ADCLine.selectChannel(ADCLine.CHANNEL_2, ADCLine.GAIN_1, ADCLine.MODE_UNIPOLAR);
    delayMicroseconds(10);
    this->voltage.reference =  roundF((float)ADCLine.readADC(),prec);
    return this->voltage.reference;
}

float analogPH::readPHVoltage(void)
{
  float voltPH = 0;
  float gain = 10000.0/(10000.0);
        gain +=1.05;
  voltPH= roundF(((getPHSignal()/gain) - getRefVoltage()),prec);
  //Serial.println("pH voltage : "+String(voltPH,5));
  return voltPH;
}

analogPH & analogPH::singleReading(void)
{
   readTemperature();
   readPH();
   return *this;
}

bool analogPH::calibrationPH(uint8_t level_ph, float pH_val)
{
   if(isPHVoltStable() || (pHStableCount() >= 5))
   {
      switch(level_ph)
        {
            case LOW_PH  :
                            if(this->calibrationVal.point < 1) return false;
                            calibrationLow(pH_val);
                            return true;
                            break;
            case MID_PH  :
                            calibrationMid(pH_val);
                            return true;
                            break;
            case HIGH_PH : 
                            if(this->calibrationVal.point < 1) return false;
                            calibrationHigh(pH_val);
                            return true;
                            break;
            default : return false;
        };
   } 
   else
    return false;
}


analogPH &analogPH::calibrationMid(float mid) {
  this->temporaryVMid = readPHVoltage(); 
  this->temporaryMid = mid;
  this->calibrationVal.point = 1;
  float adcAt4 = this->defaultLow + this->temporaryVMid - this->defaultMid;
  this->calibrationVal.slope      = getSlope(this->temporaryVMid,adcAt4,mid,4.01);
  this->calibrationVal.intercept  = getIntercept(this->temporaryVMid,adcAt4,mid,4.01);
  return *this;
}

analogPH&analogPH::calibrationLow(float low)
{
  this->temporaryVLow = readPHVoltage(); 
  this->calibrationVal.point=2;
  this->calibrationVal.slope      = getSlope(this->temporaryVMid,this->temporaryVLow,this->temporaryMid,low);
  this->calibrationVal.intercept  = getIntercept(this->temporaryVMid,this->temporaryVLow,this->temporaryMid,low);
  return *this;
}

analogPH&analogPH::calibrationHigh(float high)
{
  this->temporaryVLow = readPHVoltage(); 
  this->calibrationVal.point=2;
  this->calibrationVal.slope      = getSlope(this->temporaryVMid,this->temporaryVLow,this->temporaryMid,high);
  this->calibrationVal.intercept  = getIntercept(this->temporaryVMid,this->temporaryVLow,this->temporaryMid,high);
}

void analogPH::readPH(void)
{
  float pH_v = readPHVoltage();
  this->pHVoltStable.updateValue(pH_v);
  this->pH = 0;
  this->pH = this->calibrationVal.intercept+(this->calibrationVal.slope*pH_v);
  this->pH = roundF(this->pH, 3);
  this->phStableCount.updateValue(this->pH);
  this->pH = (this->pH > 14)?14:(this->pH<0)?0:this->pH;
  if(this->tempStable.isStable() || (this->tempStable.getStableCount() > 6 ))
  {
    float compensation = ( (this->temperature+273.15) / (25+273.15) );
          compensation =  isnan(compensation)?1:(compensation<0)?0.001:compensation;
    this->pH = 7.00 +  (( this->pH - 7.00 ) * compensation);
    this->pH = (this->pH > 14)?14:(this->pH<0)?0:this->pH;
  }
  Serial.println("values : "+String(this->pH,4)+"\t"+String(this->phStableCount.getDeviasionValue(),4)+"\t"+String(pH_v,4));
  Serial.flush();
  return;
}

void analogPH::readTemperature(void)
{
  this->temperature = -1;
  float rtd_v = getRTDSignal();
  float ref_v = (49900.0 + 10000.0) / 10000.0;
        ref_v*= getRefVoltage();
  if ( ref_v <= 0.0 ) return;
  else
  {
      this->resistance_RTD  = ( roundF(ref_v,4) - roundF(rtd_v,5) ) * 1400.00 / roundF(rtd_v,5);
      this->resistance_RTD -= 1400;
      this->resistance_RTD -= this->RTD_offset;
      this->temperature = Calculate_Temp(roundF(this->resistance_RTD,3)/1000.0);
      this->temperature = (isnan(this->temperature))?0:this->temperature;
      this->temperature = roundF(this->temperature,2);
      this->tempStable.updateValue(this->temperature);
  };
}

bool analogPH::calibrationTemperature(const float temp_Ref)
{
  readTemperature();
  float voltage_reference  = (49900.0 + 10000.0) / 10000.0;
          voltage_reference *= this->voltage.reference;
    if ( voltage_reference <= 0.1 )
      return false;
  float temp_ =((pow(-(temp_Ref*0.00116)+3.908,2)-17.59246)/-0.00232);
  float dataTemporary = this->resistance_RTD + this->RTD_offset - roundF(temp_,3);
  this->RTD_offset = isnan (dataTemporary)?0: roundF(dataTemporary,3);
  return true;
}
