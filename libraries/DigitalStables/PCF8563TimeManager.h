/*
 * PCF8563TimeManager.h
 *
 *  Created on: 21 Mar 2021
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_PCF8563TIMEMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_PCF8563TIMEMANAGER_H_

#include <TimeManager.h>

#include <RTCInfoRecord.h>

class PCF8563TimeManager  : public TimeManager {


public:

	//	String days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

	PCF8563TimeManager( HardwareSerial& serial);
	void start();
	long dateAsSeconds(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second);
	void hourlyTasks(long time, int previousHour );
	void dailyTasks(long time, int yesterdayDate, int yesterdayMonth, int yesterdayYear );
	void monthlyTasks(long time);
	void yearlyTasks(long time);
	long getCurrentTimeInSeconds();
	long getCurrentTimeInSeconds(RTCInfoRecord r);
    bool printTimeToSerial(RTCInfoRecord aRTCInfoRecord);
	RTCInfoRecord now();
	long getTimeForCodeGeneration();
	//
	// Functions that represent Serial commands
	//
	bool printTimeToSerial();
	bool setTime(String);
	bool setTime(RTCInfoRecord);
	void setPCF8563alarm(byte am, byte ah);
	void PCF8563alarmOff();
	bool checkPCF8563alarm();
	//
	// oscillator functions
	//
	void PCF8563oscOFF();
	void PCF8563osc1Hz();
	void PCF8563osc32Hz();
	void PCF8563osc1024kHz();
	void PCF8563osc32768kHz();

	virtual ~PCF8563TimeManager();

private:
	byte bcdToDec(byte value);
	byte decToBcd(byte value);


};

#endif /* LIBRARIES_DIGITALSTABLES_PCF8563TIMEMANAGER_H_ */
