/*
 * TimeManager.cpp
 *
 *  Created on: 13 Feb. 2019
 *      Author: arifainchtein
 */
#include "Arduino.h"
#include <TimeManager.h>
#include <RTCInfoRecord.h>
#include <GeneralFunctions.h>
static const byte monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};

TimeManager::TimeManager( HardwareSerial& serial): _HardSerial(serial){

}

//
// Functions that represents commands received via the serial port
//



//SetTime#23#05#2019#4#05#48#20


boolean TimeManager::setTime(String command){

	return true;

}

boolean TimeManager::printTimeToSerial(){
	return true;
}
////
//// End of Functions that represents commands received via the serial port
////
//
//
void TimeManager::start(){
}
//
//
long TimeManager::dateAsSeconds(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second){

}

String TimeManager::getCurrentTimeForDisplay(boolean showSecs){

}
//
String TimeManager::getCurrentDateTimeForDisplay(){

}
//
String TimeManager::getCurrentDateForDisplay(){

}
//
//
long TimeManager::getTimeForCodeGeneration(){

}
//
//
//
RTCInfoRecord TimeManager::getCurrentDateTime(){

}
//
long TimeManager::getCurrentTimeInSeconds(){

}

String TimeManager::getElapsedTimeHoursMinutesSecondsString(long elapsedTime) {

}
