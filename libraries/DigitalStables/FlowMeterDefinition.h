/*
 * FlowMeterDefinition.h
 *
 *  Created on: 20 Jun 2020
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_FLOWMETERDEFINITION_H_
#define LIBRARIES_DIGITALSTABLES_FLOWMETERDEFINITION_H_
#include <Arduino.h>

class FlowMeterDefinition {
public:
	uint8_t interruptNumber;
	double qfactor;
	static uint8_t NOT_USED;

	FlowMeterDefinition(uint8_t i,double q);
	virtual ~FlowMeterDefinition();
};

#endif /* LIBRARIES_DIGITALSTABLES_FLOWMETERDEFINITION_H_ */
