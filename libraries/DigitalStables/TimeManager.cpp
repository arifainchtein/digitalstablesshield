/*
 * TimeManager.cpp
 *
 *  Created on: 13 Feb. 2019
 *      Author: arifainchtein
 */
#include "Arduino.h"
#include <TimeManager.h>

static const byte monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};




TimeManager::TimeManager( HardwareSerial& serial): _HardSerial(serial){

}

String TimeManager::getElapsedTimeHoursMinutesSecondsString(long elapsedTime) {
	//String seconds = String(elapsedTime % 60);
	long seconds = elapsedTime/1000;
	int minutes = (seconds % 3600) / 60;
	String minP ="";
	if(minutes<10)minP="0";


	int hours = seconds / 3600;
	String hoursS = "";
	if(hours<10)hoursS="0";


	String time =  hoursS + hours + ":" + minP + minutes;// + ":" + seconds;
	return time;
}

void TimeManager::hourlyTasks(long time, int previousHour ){

}
void TimeManager::dailyTasks(long time, int yesterdayDate, int yesterdayMonth, int yesterdayYear ){

}
void TimeManager::monthlyTasks(long time){

}
void TimeManager::yearlyTasks(long time){

}

bool TimeManager::setTime(String command){

	return true;

}

bool TimeManager::printTimeToSerial(){
	String displayTime =  "";
		return true;
}
//
// End of Functions that represents commands received via the serial port
//


void TimeManager::start(){


}


long TimeManager::dateAsSeconds(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second){


	// note year argument is full four digit year (or digits since 2000), i.e.1975, (year 8 is 2008

	int i;


	if(year < 69){
		year+= 2000;
	}

	// seconds from 1970 till 1 jan 00:00:00 this year
	long seconds= (year-1970)*(60*60*24L*365);

	// add extra days for leap years
	for (i=1970; i<year; i++) {
		if (LEAP_YEAR(i)) {
			seconds+= 60*60*24L;
		}
	}
	// add days for this year
	for (i=0; i<month; i++) {
		if (i==1 && LEAP_YEAR(year)) {
			seconds+= 60*60*24L*29;
		} else {
			seconds+= 60*60*24L*monthDays[i];
		}
	}

	seconds+= (date-1)*3600*24L;
	seconds+= hour*3600L;
	seconds+= minute*60L;
	seconds -=  timeZoneHours*3600L;
	seconds+=second;
	return seconds;
}

String TimeManager::getCurrentTimeForDisplay(bool showSecs){
	String displayTime =  "";



	return displayTime;
}

String TimeManager::getCurrentDateTimeForDisplay(){
	return "";
}


String TimeManager::getCurrentDateForDisplay(){
	return "";
}


long TimeManager::getTimeForCodeGeneration(){

	RTCInfoRecord anRTCInfoRecord =getCurrentDateTime();
	uint8_t seconds = anRTCInfoRecord.second+SECONDOFFSET;
	uint8_t month = anRTCInfoRecord.month-1;
	uint16_t year = anRTCInfoRecord.year;


	//uint8_t date = rtc.day;
	//uint8_t hour = rtc.hour;
	//uint8_t minute=rtc.minute;
//	_HardSerial.print("getTimeForCodeGeneration,about to call dateAsSeconds, year=");
//	_HardSerial.println(year);
//	_HardSerial.flush();
	return dateAsSeconds(year, month, anRTCInfoRecord.date, anRTCInfoRecord.hour, anRTCInfoRecord.minute, anRTCInfoRecord.second );
}



RTCInfoRecord TimeManager::getCurrentDateTime(){
	RTCInfoRecord aRTCInfoRecord;
	return aRTCInfoRecord;
}

long TimeManager::getCurrentTimeInSeconds(){
	return 0;
}

String TimeManager::getElapsedTimeHoursMinutesSecondsString(long elapsedTime) {
	//String seconds = String(elapsedTime % 60);
	long seconds = elapsedTime/1000;
	int minutes = (seconds % 3600) / 60;
	String minP ="";
	if(minutes<10)minP="0";


	int hours = seconds / 3600;
	String hoursS = "";
	if(hours<10)hoursS="0";


	String time =  hoursS + hours + ":" + minP + minutes;// + ":" + seconds;
	return time;
}

TimeManager::~TimeManager( ){

}


