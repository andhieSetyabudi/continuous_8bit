# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [ 19.04.02] - 2019 - 04 - 18
- Display controller
- Token extractor
- Support hologram by SIM-card name
- Update strength of GPRS and signal if device is registered on network
- update APN & IMEI-provider from Thai and vietnam


## [0.10.0] - 2018-12-17
- Add get token from server
- Send data with token
- Add firmware version in device data

## [0.9.4] - 2018-11-21
- Add translation.
- Add prepocessor condition when soft power was not defined.
- Update config to support EZO pH.
- shifted button pin, added preprocessor if continous mode disable RFID.
- Change structure of config.
- Add prepocessor condition for enable/disable calibration button.

## [0.9.3] - 2018-08-29
- change some code to support Hologram without CCID.
- Omit CCID number in config.

## [0.9.2] - 2018-08-25
- Add config for enable or disable sensor.
- Add code to support ORP sensor.
- Fixed bug when entering DEBUG mode.

## [0.9.1] - 2018-08-04
- Add APN for singtel.
- change some code to support Hologram.
- Change logic which is roaming and can connect to GPRS network.

## [0.9.0] - 2018-05-09
- Change end point API.
- Adjust some link.
- Softpower class.
- Disable font.

## [0.7.2] - 2018-04-24
- Fixed bug when pressed the button prolonged for confirmation data pending.
- Edit version in develop branch.
- Bump version.

## [0.7.1] - 2018-04-06
- Change develop version.
- Fix bug high salinty calibration.

## [0.6.6] - 2018-04-05
- Fix bug sleep when data queue aren't empty.
- Fix some missmatch program between develop and master.

## [0.6.5] - 2018-04-02
- bug fix data send via sms failed to store to mmc

## [0.6.4] - 2018-04-02
- Add feature of CI gitlab.
- Change salinity precission.
- Set ID DEBUG when press front button at booting.

## [0.6.3] - 2018-01-24
- Add flag sms get number is false.

## [0.6.2] - 2018-01-15
- Hot fix set env_default for platformio

## [0.6.1] - 2018-01-15
- Add dev sign on version.
- Bug fix buffer size overflow on toCSV/implode methode.

## [0.6.0] - 2018-01-12
- Add parse sms massage timestamp.
- Remove 'test' and dummy sms unknown.
- Splitting Indicating File.
- Change calibration low and high value.
- Separated program in Storage class.
- Separated program in UserInput class.
- Separated program in System class.
- Separated program in Sensor class.
- Move position of loop and setup after declaration variable.
- Seperated perogram in communication class.
- Update APN database.
- Separated function of http task into new subclass internet.
- Fixing error calibration and change point calibration 7 to 10.
- add config file for jala board v0.9.1.
- Change platformio.ini to building 3 board version in one time.
- Change name filepath of data dummy and make logger for data will be sent.
- Update depedencies and README.md.

## [0.5.0] - 2017-10-13
- Edit time reset if continous cennecting.
- Add logic to search operator APN.
- Add condition if timeout and connection is open.
- Add roaming condition.
- Add timezone format.
- Add timezone parsing data.
- Fix timezone format use integer not character.
- Edit size of array for atom time format.
- Set default to low to trigger RFID hardware reset.
- Add imei to struct data and make function EEPROM.
- Edit Start write index from 'index' not 'index-1'.
- Add condition if battery is low then sleep.
- Move reading battery voltage before sleep condition.
- Add condition will to openconnection if any data has came.
- Send pending data move to EEPROM.
- Add the input probe EC type and can change from config.
- Add condition if soft switch long press then power off.
- Add condition for detection type of EC probe and then adjust automatically.
- Add program in setup for checking battery voltage provided low batt then stuck in while looping.
- Add an indication when the system is charging.
- Make ussd request via sms and respon back to requster with sms ashynchronous.
- Add  program to detected  when was charging, add cmd at+clts at init, and improve soft power display.
- Improvement program for softpower button.
- Add program to verify data in eeprom with crc.
- Removing program to allow get timestamp (AT+CLTS) which had been added before and move it to library.
- Add program to write and read pH calibration value to eeprom and pH library.
- Add program for getting and requesting own number from Jack.
- Change mode recieving sms which change to urc mode.
- Revision pogram to get number from jack & add the program to get off when empty battery.
- Storage the data to the SD card providing the data failed send by sms.
- Change configuration file program which can change old or the slim one.
- Move mode and interval time,which is before in subconfig file, to main config program.
- Add waiting timeout to select send or delete queue data menus.
- Changing low and mid pH calibration value, Comment calibration value for ommiting pH 10 calibration point.
- Add program to give feedback when device id is empty..
- Solved bug for button and add wake up from sleep if non-continous mode.
- Breaking Change, using AnalogPHMeter v0.2.0.

## [0.4.0] - 2017-6-14
- Change from wait to stable become timeout.
- Fixing compensation..
- Make init after factory and calibration reset.
- Make pulse stop when sleep, even not stable.
- Refactor and add workaround for compiler error.
- Change Logo to JALA.
- Give condition flag to change index data queue.
- Move turn on device to asyncTask.
- Add connection retry, reset after 2 times.
- Uncomment smsTask, with new command RESET GSM.
- On resetProperties also Reset ussdRequester.
- Adding get location asynchronous.
- Add Get Lcation Flag reset if timeout.
- Reset if not registered in 90 second.
- add function openConnectionAsync.
- Fix forgot to add startWaiting millis().

## [0.3.0] - 2017-6-14
- Remove unused library
- Add Sleep parameter in Sensor Class
- Add Sleep method and property in UserInput
- Fix, sometime seconds error on server
- Add Sleep Method and Property in Indicating
- Add Sleep Method and Property to Communication
- Rewrite Sleep method and property in System Class
- Bug fix, should be inverse condition.
- change Not disable pin change interrupt.
- Fix that Forget to return to normal condition after testing
- Add Sleep condition in main program
- Add Sleep condition is depend on other sleep condition
- Add Display Firmware Version
- Add Delay
- Fix Wrong placement Sleep Program
- Fix wrong condition
- Move Displaying Firmware to System.
- Add continue sign
- Add program to wake when pressed
- Add notification when sending via SMS
- Add Limit temperature error reading from 0.000 to 0.1
- Fix allowToConnect Condition

## [0.2.0] - 2017-5-19
- Delete Config.h
- Add macro to disable DHT
- Add SoftPower Class in System
- Change UserInput::Button Class. Optimizing
- Fix forbidden 'operator' name
- Fix Button behaviour, and Indicating
- Communication optimize, add Buffer Device status. etc
- Rewriting System Library flow
- Rewriting UserInput Library flow
- Rewritring Indicating Library, support PROGMEM
- Fixed Button should be input, dammit!
- For safety always enable Serial
- Change Jack SMS Number
- Change info to in flash
- Add Splash Screen in Indicating
- Fix library not downloaded
- Refactor program
- Pin Initialization Optimizing
- Also update device on update interval continuous
- Add Sleep Class in System Class
- Add Splash screen
- Change variable name.
- Edit Config for Charlie 2
- Add Firmware Version

## [0.1.0] - 2017-5-19
- Initial Commit
