/*
 * TimeManager.h
 *
 *  Created on: 13 Feb. 2019
 *      Author: arifainchtein
 */

#ifndef TIMEMANAGER_H_
#define TIMEMANAGER_H_
#include <Arduino.h>
#include <GravityRtc.h>
#include <RTCInfoRecord.h>
#include <GeneralFunctions.h>

class TimeManager{
	HardwareSerial& _HardSerial;

	#define LEAP_YEAR(_year) ((_year%4)==0)

	int timeZoneHours=11;
	int SECONDOFFSET=10;
	

	public:
		TimeManager( HardwareSerial& serial);
        void start()=0;
		long dateAsSeconds(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second)=0;
		void hourlyTasks(long time, int previousHour )=0;
		void dailyTasks(long time, int yesterdayDate, int yesterdayMonth, int yesterdayYear )=0;
		void monthlyTasks(long time)=0;
		void yearlyTasks(long time)=0;
		long getCurrentTimeInSeconds()=0;
		String getElapsedTimeHoursMinutesSecondsString(long elapsedTime)=0;
		RTCInfoRecord getCurrentDateTime()=0;
		long getTimeForCodeGeneration()=0;
		String getCurrentTimeForDisplay(boolean showSec)=0;
		String getCurrentDateTimeForDisplay()=0;
		String getCurrentDateForDisplay()=0;
		//
		// Functions that represent Serial commands
		//
		boolean printTimeToSerial()=0;
		boolean setTime(String)=0;


	private:


	

	};
#endif /* TIMEMANAGER_H_ */
