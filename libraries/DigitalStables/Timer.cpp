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
Timer::Timer() {
	// TODO Auto-generated constructor stub

}
void Timer::start(uint16_t d){
	duration=d;
}
void Timer::reset(){
	current=0;
}
void Timer::tick(){
	current++;
}
bool Timer::status(){
	if(current>duration)return true;
	else return false;
}

Timer::~Timer() {
	// TODO Auto-generated destructor stub
}

