/*
 * FlowMeterEventData.h
 *
 *  Created on: 6 May 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_FLOWMETEREVENTFAULT_H_
#define LIBRARIES_DIGITALSTABLES_FLOWMETEREVENTFAULT_H_

#include "Arduino.h"
#include <FaultRecord.h>

typedef struct FlowMeterEventFault: public FaultRecord{
	float flow;
	uint8_t faultCode;
};



#endif /*IBRARIES_DIGITALSTABLES_FLOWMETEREVENTFAULT_H_ */
