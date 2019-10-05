#ifndef MAIN_H_
#define MAIN_H_



void sensorCalibration(void);
BlumbangReksaData getCurrentBlumbangReksaData(void);
BlumbangReksaDevice getCurrentBlumbangReksaDevice(void);
bool confirmation(const char *confirmationText, const char *yesText, const char *noText, bool defaultChoose = true, unsigned long timeout = 30000UL);
void sendPendingDataConfirmation(void);

static bool displayed = false;

void displayData(uint8_t dataDispType, unsigned int timeDisplay);
bool getDisplayStatus(void);
bool setDisplayedStatus(bool status);
void setSystem(bool sleepS);
//void chargingState(void);

#if CALIBRATION_BUTTON_ENABLE == true
    void calibrationMenu(void);
#endif

const char *t(int key);

//=================================== mapping type of data to be displayed ===================
#define measurementData             1
#define notifBatteryLowbat          2
#define notifBatteryChargePlugged   3
#define notifBatteryChargeRelease   4
#define notifTapID                  5
#define notifSendData               6
#define notifHoldToSend             7
#define notifDataQueueFull          8
#define displayIdCardData           9
#define sleepDisplay               10
#define prepareAfterSleep          11


#endif
