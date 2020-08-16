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





TimeManager::~TimeManager( ){

}


