/*
 * Timer.h
 *
 *  Created on: 18 Jun 2020
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_TIMER_H_
#define LIBRARIES_DIGITALSTABLES_TIMER_H_
#include <Arduino.h>

class Timer {
public:
	Timer(uint16_t duration);
	void start();
	bool status();
	void tick();
	void reset();
	void stop();
	virtual ~Timer();
};

#endif /* LIBRARIES_DIGITALSTABLES_TIMER_H_ */
