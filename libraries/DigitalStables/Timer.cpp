/*
 * Timer.cpp
 *
 *  Created on: 18 Jun 2020
 *      Author: arifainchtein
 */

#include <Timer.h>
bool status=false;
uint16_t current=0;
uint16_t duration;
bool active=false;

Timer::Timer(uint16_t d) {
	duration=d;
}
void Timer::start(){

	active=true;
}
void Timer::stop(){
	active=false;
}
void Timer::reset(){
	current=0;
}
void Timer::tick(){
	if(active)current++;
}
bool Timer::status(){
	if(active && current>=duration)return true;
	else return false;
}

Timer::~Timer() {
	// TODO Auto-generated destructor stub
}

