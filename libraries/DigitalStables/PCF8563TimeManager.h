/*
 * PCF8563TimeManager.h
 *
 *  Created on: 21 Mar 2021
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_PCF8563TIMEMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_PCF8563TIMEMANAGER_H_

#include <TimeManager.h>
#include <GravityRtc.h>
#include <RTCInfoRecord.h>

class PCF8563TimeManager  : public TimeManager {
	PCF8563TimeManager rtc;

public:

	//	String days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

	PCF8563TimeManager( HardwareSerial& serial);
	PCF8563TimeManager();
	void start();
	long dateAsSeconds(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second);
	void hourlyTasks(long time, int previousHour );
	void dailyTasks(long time, int yesterdayDate, int yesterdayMonth, int yesterdayYear );
	void monthlyTasks(long time);
	void yearlyTasks(long time);
	long getCurrentTimeInSeconds();
	String getElapsedTimeHoursMinutesSecondsString(long elapsedTime);
	RTCInfoRecord getCurrentDateTime();
	long getTimeForCodeGeneration();
	String getCurrentTimeForDisplay(bool showSec);
	String getCurrentDateTimeForDisplay();
	String getCurrentDateForDisplay();
	//
	// Functions that represent Serial commands
	//
	bool printTimeToSerial();
	bool setTime(String);

private:
	byte bcdToDec(byte value);
	byte decToBcd(byte value);

	virtual ~PCF8563TimeManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_PCF8563TIMEMANAGER_H_ */
