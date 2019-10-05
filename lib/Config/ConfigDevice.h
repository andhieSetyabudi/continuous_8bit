#ifndef CONFIGDEVICE_H_
#define CONFIGDEVICE_H_

#define VERSION "19.08.01"
/* Constant */
#define HARDWARE_SERIAL             0
#define SOFTWARE_SERIAL             1
#define SENSOR_INTERFACE_EZO        0
#define SENSOR_INTERFACE_ANALOG     1
#define SENSOR_INTERFACE_I2C        2

#define _INTERFACE_ONEWIRE          0
#define _INTERFACE_TWOWIRE          1
#define _INTERFACE_SERIAL           2


#define SYSTEM_SERIAL           Serial
#define SYSTEM_SERIAL_BAUDRATE  115200


//============================================= GSM predefined - pin confs
#define GSM_REGULATOR_PIN             27
#define GSM_POWER_PIN                 26
#define GSM_SERIAL                    Serial2
#define GSM_SERIAL_TYPE               HARDWARE_SERIAL
#define GSM_SERIAL_BAUDRATE           19200//115200 
#if GSM_SERIAL_TYPE == SOFTWARE_SERIAL
  #define GSM_SERIAL_RX_PIN 10
  #define GSM_SERIAL_TX_PIN 11
#elif GSM_SERIAL == Serial2
  #define GSM_SERIAL_RX_PIN           16
  #define GSM_SERIAL_TX_PIN           17
#endif
#define END_RESET_GSM_PIN             3
#define RI_GSM_PIN                    A11
#define LDTR_GSM_PIN                  A10


//======================================= Sensor predefined pins
#define PH_SENSOR_INTERFACE           SENSOR_INTERFACE_I2C
#define PH_SERIAL_TYPE                SOFTWARE_SERIAL
#define TEMPERATURE_SENSOR_INTERFACE _INTERFACE_SERIAL


#define EZO_Continuous_reading          false
#if DO_SENSOR_SERIAL_ENABLE
  #define DO_SENSOR_SERIAL              Serial3    // default Serial3
  #if DO_SENSOR_SERIAL == SOFTWARE_SERIAL     // pH port was used
    #define DO_SERIAL_RX_PIN            11
    #define DO_SERIAL_TX_PIN            10
  #endif
#endif
#if EC_SENSOR_SERIAL_ENABLE
  #define EC_SENSOR_SERIAL              Serial1    // default Serial1
  #if EC_SENSOR_SERIAL == SOFTWARE_SERIAL     // pH port was used
    #define EC_SERIAL_RX_PIN            11
    #define EC_SERIAL_TX_PIN            10
  #endif
#endif

#if PH_SENSOR_SERIAL_ENABLE
  #if PH_SENSOR_INTERFACE == SENSOR_INTERFACE_EZO
    #if PH_SERIAL_TYPE == HARDWARE_SERIAL
      #define PH_SENSOR_SERIAL Serial3
    #elif PH_SERIAL_TYPE == SOFTWARE_SERIAL
      #define pHCalibrationInterval   950     // in ms
      #define std_deviasion           0.002f//0.0025f
      #define PH_SERIAL_RX_PIN 11
      #define PH_SERIAL_TX_PIN 10
      #define PH_SENSOR_SERIAL Sensor::Water::pHSerial
      
    #endif
  #elif PH_SENSOR_INTERFACE == SENSOR_INTERFACE_I2C
    #define PH_I2C_SDA_PIN 11
    #define PH_I2C_SCL_PIN 10
    #define pHCalibrationInterval   800ul     // in ms
    #define pHCalibrationPrecision  0.0025f//0.002f   // floating-point for black probe : 75; blue : 0.00685f
    #define defaultAtpH7            1.220f//1.130f//1.220f
    #define defaultAtpH4            1.558f//1.336f//1.540f
    #define pH_line                 Sensor::Water::pH_2Wire
  #endif
#endif

#define SMS_JACK_NUMBER       "+6285700799004"
#define SMS_DISPOSAL_NUMBER   "+6285878175157"


//============================ hardware I/O port-pins predefined
#define FORCE_SHUTDOWN_PIN   28

#define BUTTON_PIN_PORT 10 // PJ
#define BUTTON_PIN_BIT _BV(2)
#define BUTTON_PIN      70

#if CALIBRATION_BUTTON_ENABLE == true
  #define CALIBRATION_BUTTON_PIN_PORT 10 // PJ
  #define CALIBRATION_BUTTON_PIN_BIT _BV(4)
  #define CALIBRATION_BUTTON_PIN 72
#endif

#if CHARGING_DETECTION == true
  //#define CHARGE_STAT_PIN_PORT 10 // PJ
  //#define CHARGE_STAT_PIN_BIT _BV(6)
  #define CHARGE_STAT_PIN 74
#endif

#define CHG_CE                37

#define POWER_ISO_PIN         29 
#define POWER_ISO_PIN_PORT    11 // PK
#define POWER_ISO_PIN_BIT     _BV(1)
#define POWER_ISO_PIN_        _BV(PK1)
#define POWER_ISO_PORT        PORTK

#define RTC_SQWO_PIN          A14
#define U8GLIB_TYPE           U8GLIB_SSD1306_128X64


#if TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_ONEWIRE
  #define ONEWIRE_DS_PIN 41
  #define DHT_USED false
    #if DHT_USED
      #define DHT_PIN A15
      #define DHT_TYPE DHT22
    #endif
#elif TEMPERATURE_SENSOR_INTERFACE == _INTERFACE_TWOWIRE
  #define TEMPERATURE_ADDR  0x49
  #define TEMPERATURE_I2C_SDA_PIN 52
  #define TEMPERATURE_I2C_SCL_PIN 50
  #define ONEWIRE_DS_PIN    41
#else
  #define ONEWIRE_DS_PIN    41
  #define precision_of_temp 0.15f
#endif

//#define SOFT_POWER_CONTROL A12
//#define SOFT_POWER_CONTROL_PORT PORTK
//#define SOFT_POWER_CONTROL_PIN  _BV(PK4)
#define SOFT_POWER_BUTTON  A15

//#define CHARGER_DETECT_PIN 38

#define BATTERY_ADC_PIN     A13
#define CHARGER_STATUS_PIN  A12

#define charger_port        10 // PJ
#define charger_pin         _BV(6)
#define charger_pin_stat    6

#define BUZZER_PIN 5
#define min_power             720
#define max_power             830
#define min_percentage_power  10
#define min_percentage_off    5

#define bahasa                1
#define english               0
#define Language          bahasa
    #if Language == bahasa
      #define Linguistic        english
    #else
      #define Linguistic        bahasa
    #endif


#define CHG_ADDR  0x6B
#define CHG_CE    37
#define CHG_SDA   36
#define CHG_SCL   35
#define BAT_ADDR  0x23


#define microSD   false
#include <avr/wdt.h>
#endif


#define CALIBRATION_TEMPERATURE false 
#define temperature_ref_val     27.0