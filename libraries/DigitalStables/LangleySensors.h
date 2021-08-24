/*
 * LangleySensors.h
 *
 *  Created on: 24 Aug 2021
 *      Author: arifainchtein
 */
#include <Arduino.h>


#ifndef LIBRARIES_DIGITALSTABLES_LANGLEYSENSORS_H_
#define LIBRARIES_DIGITALSTABLES_LANGLEYSENSORS_H_

class LangleySensors {
public:
	LangleySensors();
	void start(uint8_t a[]);
	float getTemperature();//{
	virtual ~LangleySensors();
};

#endif /* LIBRARIES_DIGITALSTABLES_LANGLEYSENSORS_H_ */
