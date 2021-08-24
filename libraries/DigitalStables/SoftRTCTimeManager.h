/*
 * SoftRTC.h
 *
 *  Created on: 24 Aug 2021
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_SOFTRTCTIMEMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_SOFTRTCTIMEMANAGER_H_

#include <TimeManager.h>
#include <swRTC.h>
#include <RTCInfoRecord.h>

class SoftRTCTimeManager  : public TimeManager {

	swRTC rtc;

public:
	SoftRTCTimeManager(HardwareSerial& serial);
	void start();
	long dateAsSeconds(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second);
	void hourlyTasks(long time, int previousHour );
	void dailyTasks(long time, int yesterdayDate, int yesterdayMonth, int yesterdayYear );
	void monthlyTasks(long time);
	void yearlyTasks(long time);
	long getCurrentTimeInSeconds();
	RTCInfoRecord now();
	long getTimeForCodeGeneration();
	//
	// Functions that represent Serial commands
	//
	bool printTimeToSerial();
	bool setTime(String);
	virtual ~SoftRTCTimeManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_SOFTRTCTIMEMANAGER_H_ */
