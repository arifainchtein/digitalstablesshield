/*
 * FlowSensorStatus.h
 *
 *  Created on: 28 Oct 2019
 *      Author: arifainchtein
 */
#include <Arduino.h>
#include <DiscreteRecord.h>

#ifndef LIBRARIES_DIGITALSTABLES_FLOWSENSORSTATUS_H_
#define LIBRARIES_DIGITALSTABLES_FLOWSENSORSTATUS_H_


typedef struct FlowSensorStatus: public DiscreteRecord{
		  uint8_t meterId;
		  float currentFlow;
		  float currentVolume;

		};


typedef union FlowSensorStatusUnion{
	FlowSensorStatus aFlowSensorStatus;
  byte aFlowSensorStatusBytes[sizeof(FlowSensorStatus)];
};


#endif /* LIBRARIES_DIGITALSTABLES_FLOWSENSORSTATUS_H_ */
