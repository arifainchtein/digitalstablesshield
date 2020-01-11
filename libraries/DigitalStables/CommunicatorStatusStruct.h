/*
 * BaseSensorStruct.h
 *
 *  Created on: 12 Oct 2019
 *      Author: arifainchtein
 */
#include <Arduino.h>
#include <DiscreteRecord.h>
#ifndef LIBRARIES_DIGITALSTABLES_COMMUNICATORSTATUSSTRUCT_H_
#define LIBRARIES_DIGITALSTABLES_COMMUNICATORSTATUSSTRUCT_H_

struct CommunicatorStatusStruct :public DiscreteRecord{


		boolean signalStatus;  // 0=Solar 1=Grid
			};


typedef union CommunicatorStatusStructUnion{
	CommunicatorStatusStruct aPowerStatusStruct;
  byte aPowerStatusStructBytes[sizeof(CommunicatorStatusStruct)];
};



#endif /* LIBRARIES_DIGITALSTABLES_POWERSTATUSSTRUCT_H_ */
