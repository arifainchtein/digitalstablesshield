/*
 * GravityTimeManager.cpp
 *
 *  Created on: 13 Feb. 2019
 *      Author: arifainchtein
 */
#include "Arduino.h"
#include <GravityTimeManager.h>
#include <RTCInfoRecord.h>

#include <GravityRtc.h>

#include <GeneralFunctions.h>

static const byte monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};

GravityTimeManager::GravityTimeManager( HardwareSerial& serial):TimeManager(serial ){

}

GravityTimeManager::~GravityTimeManager( ){}

//
// Functions that represents commands received via the serial port
//



//SetTime#23#05#2019#4#05#48#20

 void GravityTimeManager::hourlyTasks(long time, int previousHour ){

 }
 void GravityTimeManager::dailyTasks(long time, int yesterdayDate, int yesterdayMonth, int yesterdayYear ){

 }
 void GravityTimeManager::monthlyTasks(long time){

 }
 void GravityTimeManager::yearlyTasks(long time){

 }

bool GravityTimeManager::setTime(String command){
	//SetTime#24#10#19#4#17#32#00
	int date = GeneralFunctions::getValue(command, '#', 1).toInt();
	int month = GeneralFunctions::getValue(command, '#', 2).toInt();
	int year = GeneralFunctions::getValue(command, '#', 3).toInt();
	int dw = GeneralFunctions::getValue(command, '#', 4).toInt();
	int hour = GeneralFunctions::getValue(command, '#', 5).toInt();
	int min = GeneralFunctions::getValue(command, '#', 6).toInt();
	int sec = GeneralFunctions::getValue(command, '#', 7).toInt();

	_HardSerial.print(rtc.day);
		_HardSerial.print("/");
		_HardSerial.print(rtc.month);
		_HardSerial.print("/");
		_HardSerial.print(rtc.year);
		_HardSerial.print(" ");
		_HardSerial.print(rtc.hour);
		_HardSerial.print(":");
		_HardSerial.print(rtc.minute);
		_HardSerial.print(":");
		_HardSerial.print(rtc.second);


	rtc.adjustRtc(year,month,date,dw,hour,min,sec);




	//	setTime(hour,min,sec,date,month,year);
	//	RTC.set(now());
	//getTime();
	return true;

}

bool GravityTimeManager::printTimeToSerial(){

	rtc.read();
	String displayTime =  "";
	_HardSerial.print(rtc.day);
	_HardSerial.print("/");
	_HardSerial.print(rtc.month);
	_HardSerial.print("/");
	_HardSerial.print(rtc.year);
	_HardSerial.print(" ");
	_HardSerial.print(rtc.hour);
	_HardSerial.print(":");
	_HardSerial.print(rtc.minute);
	_HardSerial.print(":");
	_HardSerial.print(rtc.second);


	return true;
}
//
// End of Functions that represents commands received via the serial port
//


void GravityTimeManager::start(){
	rtc.setup();

}


long GravityTimeManager::dateAsSeconds(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second){


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



long GravityTimeManager::getTimeForCodeGeneration(){

	RTCInfoRecord anRTCInfoRecord =now();
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



RTCInfoRecord GravityTimeManager::now(){
	RTCInfoRecord aRTCInfoRecord;
	rtc.read();
	aRTCInfoRecord.date=rtc.day;
	aRTCInfoRecord.month=rtc.month;
	aRTCInfoRecord.year=rtc.year;
	aRTCInfoRecord.hour=rtc.hour;
	aRTCInfoRecord.minute=rtc.minute;
	aRTCInfoRecord.second=rtc.second;

	return aRTCInfoRecord;
}

long GravityTimeManager::getCurrentTimeInSeconds(){
	rtc.read();
	int month = rtc.month-1;
	long now=dateAsSeconds(rtc.year, month, rtc.day, rtc.hour, rtc.minute, rtc.second);
	return now;
}


