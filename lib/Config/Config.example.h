#ifndef CONFIG_H_
#define CONFIG_H_

#include "Version.h"

// #include "Config912.h"
#define DO_SENSOR_SERIAL_ENABLE true
#define EC_SENSOR_SERIAL_ENABLE true
#define PH_SENSOR_SERIAL_ENABLE true
#define ORP_SENSOR_SERIAL_ENABLE false

#define CALIBRATION_BUTTON_ENABLE true

/* Configuration file for board version 0.3.2 */
#ifdef VERSION_0_3_2
#include "Config032.h"
#endif

/* Configuration file for board version 0.9.0 (slim board)  */
#ifdef VERSION_0_9_0
#include "Config090.h"
#endif

/* Configuration file for board version 0.9.1 (slim board)  */
#ifdef VERSION_0_9_1
#include "Config091.h"
#endif

/*configuration for device ID and mode */
#define DEVICE_ID ""
#define CONTINUOUS_MODE false
#define CONTINUOUS_MINUTE_INTERVAL 30

#define GSM_SERIAL_DEBUG true
#define DO_SENSOR_SERIAL_DEBUG false
#define EC_SENSOR_SERIAL_DEBUG false
#define PH_SENSOR_SERIAL_DEBUG false
#define ORP_SENSOR_SERIAL_DEBUG false
#define SYSTEM_DEBUG false

#endif
