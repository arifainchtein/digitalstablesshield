/*
 * PCF8563TimeManager.cpp
 *
 *  Created on: 21 Mar 2021
 *      Author: arifainchtein
 */

#include <PCF8563TimeManager.h>
#include <Wire.h>

static const byte monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
#define PCF8563address 0x51



PCF8563TimeManager::PCF8563TimeManager( HardwareSerial& serial):TimeManager(serial ){
	 Wire.begin();
}

void PCF8563TimeManager::hourlyTasks(long time, int previousHour ){

 }
 void PCF8563TimeManager::dailyTasks(long time, int yesterdayDate, int yesterdayMonth, int yesterdayYear ){

 }
 void PCF8563TimeManager::monthlyTasks(long time){

 }
 void PCF8563TimeManager::yearlyTasks(long time){

 }
 byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

bool PCF8563TimeManager::setTime(String command){
	//SetTime#24#10#19#4#17#32#00
	int date = GeneralFunctions::getValue(command, '#', 1).toInt();
	int month = GeneralFunctions::getValue(command, '#', 2).toInt();
	int year = GeneralFunctions::getValue(command, '#', 3).toInt();
	int dayOfWeek = GeneralFunctions::getValue(command, '#', 4).toInt();
	int hour = GeneralFunctions::getValue(command, '#', 5).toInt();
	int minute = GeneralFunctions::getValue(command, '#', 6).toInt();
	int second = GeneralFunctions::getValue(command, '#', 7).toInt();

	_HardSerial.print(date);
	_HardSerial.print("/");
		_HardSerial.print(month);
		_HardSerial.print("/");
		_HardSerial.print(year);
		_HardSerial.print(" ");
		_HardSerial.print(hour);
		_HardSerial.print(":");
		_HardSerial.print(minute);
		_HardSerial.print(":");
		_HardSerial.print(second);


		Wire.beginTransmission(PCF8563address);
		  Wire.write(0x02);
		  Wire.write(decToBcd(second));
		  Wire.write(decToBcd(minute));
		  Wire.write(decToBcd(hour));
		  Wire.write(decToBcd(date));
		  Wire.write(decToBcd(dayOfWeek));
		  Wire.write(decToBcd(month));
		  Wire.write(decToBcd(year));
		  Wire.endTransmission();




	//	setTime(hour,min,sec,date,month,year);
	//	RTC.set(now());
	//getTime();
	return true;

}

RTCInfoRecord PCF8563TimeManager::getCurrentDateTime(){

		Wire.beginTransmission(PCF8563address);
		Wire.write(0x02);
		Wire.endTransmission();
		Wire.requestFrom(PCF8563address, 7);
		RTCInfoRecord aRTCInfoRecord;
		aRTCInfoRecord.second     = bcdToDec(Wire.read() & B01111111); // remove VL error bit
		aRTCInfoRecord.minute     = bcdToDec(Wire.read() & B01111111); // remove unwanted bits from MSB
		aRTCInfoRecord.hour       = bcdToDec(Wire.read() & B00111111);
		aRTCInfoRecord.date = bcdToDec(Wire.read() & B00111111);
		aRTCInfoRecord.dayOfWeek  = bcdToDec(Wire.read() & B00000111);
		aRTCInfoRecord.month      = bcdToDec(Wire.read() & B00011111);  // remove century bit, 1999 is over
		aRTCInfoRecord.year       = bcdToDec(Wire.read());

		return aRTCInfoRecord;
	}


bool PCF8563TimeManager::printTimeToSerial(){

	RTCInfoRecord aRTCInfoRecord = getCurrentDateTime();
	String displayTime =  "";
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


void PCF8563TimeManager::start(){

	 Wire.begin();
}


long PCF8563TimeManager::dateAsSeconds(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second){


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



String PCF8563TimeManager::getCurrentTimeForDisplay(bool showSecs){
	RTCInfoRecord aRTCInfoRecord  = getCurrentDateTime();
	String displayTime =  "";

	displayTime.concat(aRTCInfoRecord.hour);
	displayTime.concat(":");
	displayTime.concat(aRTCInfoRecord.minute);
	if(showSecs){
		displayTime.concat(":");
		displayTime.concat(aRTCInfoRecord.second);
	}

	return displayTime;
}

String PCF8563TimeManager::getCurrentDateTimeForDisplay(){
	RTCInfoRecord aRTCInfoRecord  = getCurrentDateTime();

	String displayTime =  "";
	displayTime.concat(aRTCInfoRecord.date);
	displayTime.concat("/");
	displayTime.concat(aRTCInfoRecord.month);
	displayTime.concat("/");
	int year = aRTCInfoRecord.year-2000;
	displayTime.concat(year);
	displayTime.concat(" ");
	displayTime.concat(aRTCInfoRecord.hour);
	displayTime.concat(":");
	displayTime.concat(aRTCInfoRecord.minute);
	displayTime.concat(":");
	displayTime.concat(aRTCInfoRecord.second);

	return displayTime;
}

String PCF8563TimeManager::getCurrentDateForDisplay(){
	RTCInfoRecord aRTCInfoRecord  = getCurrentDateTime();

	String displayTime =  "";
	displayTime.concat(aRTCInfoRecord.date);
	displayTime.concat("/");
	displayTime.concat(aRTCInfoRecord.month);
	displayTime.concat("/");
	int year = aRTCInfoRecord.year-2000;
	displayTime.concat(year);

	return displayTime;
}


long PCF8563TimeManager::getTimeForCodeGeneration(){

	RTCInfoRecord anRTCInfoRecord =getCurrentDateTime();
	uint8_t seconds = anRTCInfoRecord.second+SECONDOFFSET;
	uint8_t month = anRTCInfoRecord.month-1;
	uint16_t year = anRTCInfoRecord.year;

	return dateAsSeconds(year, month, anRTCInfoRecord.date, anRTCInfoRecord.hour, anRTCInfoRecord.minute, anRTCInfoRecord.second );
}





long PCF8563TimeManager::getCurrentTimeInSeconds(){
	RTCInfoRecord anRTCInfoRecord =getCurrentDateTime();

	int month = anRTCInfoRecord.month-1;
	long now=dateAsSeconds(anRTCInfoRecord.year, month, anRTCInfoRecord.date, anRTCInfoRecord.hour, anRTCInfoRecord.minute, anRTCInfoRecord.second);
	return now;
}

String PCF8563TimeManager::getElapsedTimeHoursMinutesSecondsString(long elapsedTime) {
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


	byte PCF8563TimeManager::bcdToDec(byte value)
	{
	  return ((value / 16) * 10 + value % 16);
	}

	byte PCF8563TimeManager::decToBcd(byte value){
	  return (value / 10 * 16 + value % 10);
	}





PCF8563TimeManager::~PCF8563TimeManager() {
	// TODO Auto-generated destructor stub
}
