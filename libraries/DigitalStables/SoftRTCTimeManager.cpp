/*
 * SoftRTCTimeManager.cpp
 *
 *  Created on: 24 Aug 2021
 *      Author: arifainchtein
 */

#include <SoftRTCTimeManager.h>

#include <swRTC.h>

static const byte monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};



SoftRTCTimeManager::SoftRTCTimeManager( HardwareSerial& serial):TimeManager(serial ){
	rtc = new swRTC();

}

void SoftRTCTimeManager::hourlyTasks(long time, int previousHour ){

 }
 void SoftRTCTimeManager::dailyTasks(long time, int yesterdayDate, int yesterdayMonth, int yesterdayYear ){

 }
 void SoftRTCTimeManager::monthlyTasks(long time){

 }
 void SoftRTCTimeManager::yearlyTasks(long time){

 }


bool SoftRTCTimeManager::setTime(String command){
	//SetTime#24#10#19#4#17#32#00
	int date = GeneralFunctions::getValue(command, '#', 1).toInt();
	int month = GeneralFunctions::getValue(command, '#', 2).toInt();
	int year = GeneralFunctions::getValue(command, '#', 3).toInt();
	int dayOfWeek = GeneralFunctions::getValue(command, '#', 4).toInt();
	int hour = GeneralFunctions::getValue(command, '#', 5).toInt();
	int minute = GeneralFunctions::getValue(command, '#', 6).toInt();
	int second = GeneralFunctions::getValue(command, '#', 7).toInt();

	rtc->stopRTC(); //stop the RTC
	rtc->setTime(hour,minute,second); //set the time here
	rtc->setDate(date,month,year); //set the date here
	rtc->startRTC(); //start the RTC

	return true;

}

RTCInfoRecord SoftRTCTimeManager::now(){

		RTCInfoRecord aRTCInfoRecord;
		aRTCInfoRecord.second     = rtc->getSeconds();
		aRTCInfoRecord.minute     = rtc->getMinutes();
		aRTCInfoRecord.hour       = rtc->getHours();
		aRTCInfoRecord.date 	= rtc->getDay();
		aRTCInfoRecord.dayOfWeek  = rtc->getWeekDay();
		aRTCInfoRecord.month      = rtc->getMonth();
		aRTCInfoRecord.year       = rtc->getYear();
		return aRTCInfoRecord;
	}


bool SoftRTCTimeManager::printTimeToSerial(){

	RTCInfoRecord aRTCInfoRecord = now();

	_HardSerial.print(aRTCInfoRecord.date);
	_HardSerial.print("/");
	_HardSerial.print(aRTCInfoRecord.month);
	_HardSerial.print("/");
	_HardSerial.print(aRTCInfoRecord.year);
	_HardSerial.print(" ");
	_HardSerial.print(aRTCInfoRecord.hour);
	_HardSerial.print(":");
	_HardSerial.print(aRTCInfoRecord.minute);
	_HardSerial.print(":");
	_HardSerial.print(aRTCInfoRecord.second);


	return true;
}
//
// End of Functions that represents commands received via the serial port
//


void SoftRTCTimeManager::start(){

	rtc->startRTC();
}


long SoftRTCTimeManager::dateAsSeconds(uint16_t y, uint8_t m, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second){


	// note year argument is full four digit year (or digits since 2000), i.e.1975, (year 8 is 2008

	int i;


	if(y < 69){
		y+= 2000;
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
	for (i=0; i<m; i++) {
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





long SoftRTCTimeManager::getTimeForCodeGeneration(){

	RTCInfoRecord anRTCInfoRecord =now();
	uint8_t seconds = anRTCInfoRecord.second+SECONDOFFSET;
	uint8_t m = anRTCInfoRecord.month-1;
	uint16_t y = anRTCInfoRecord.year;

	return dateAsSeconds(y, m, anRTCInfoRecord.date, anRTCInfoRecord.hour, anRTCInfoRecord.minute, anRTCInfoRecord.second );
}





long SoftRTCTimeManager::getCurrentTimeInSeconds(){
	RTCInfoRecord anRTCInfoRecord =now();

	int m = anRTCInfoRecord.month-1;
	long now=dateAsSeconds(anRTCInfoRecord.year, m, anRTCInfoRecord.date, anRTCInfoRecord.hour, anRTCInfoRecord.minute, anRTCInfoRecord.second);
	return now;
}


SoftRTCTimeManager::~SoftRTCTimeManager() {
	// TODO Auto-generated destructor stub
}

