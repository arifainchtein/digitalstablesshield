/*
 * GravityTimeManager.h
 *
 *  Created on: 7 Oct 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_GRAVITYTIMEMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_GRAVITYTIMEMANAGER_H_

#include <TimeManager.h>
#include <GravityRtc.h>

class GravityTimeManager : public TimeManager {
	GravityRtc rtc;

public:
	GravityTimeManager();
	GravityTimeManager( HardwareSerial& serial);
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
	String getCurrentTimeForDisplay(boolean showSec);
	String getCurrentDateTimeForDisplay();
	String getCurrentDateForDisplay();
	//
	// Functions that represent Serial commands
	//
	boolean printTimeToSerial();
	boolean setTime(String);

	virtual ~GravityTimeManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_GRAVITYTIMEMANAGER_H_ */
