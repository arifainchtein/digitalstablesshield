/*
 * Timer.cpp
 *
 *  Created on: 18 Jun 2020
 *      Author: arifainchtein
 */

#include <Timer.h>
bool status=false;
long startTime;
uint16_t duration;
Timer::Timer() {
	// TODO Auto-generated constructor stub

}
void Timer::start(uint16_t d){
	startTime = millis();
	duration=d;
}

bool Timer::status(){
	if(millis()>startTime+duration)return true;
	else return false;
}

Timer::~Timer() {
	// TODO Auto-generated destructor stub
}

