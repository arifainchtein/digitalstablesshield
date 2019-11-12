/*
 * This file is generated in the spermatogenesis process
 */
#ifndef EVENTRECORD_H
 #define EVENTRECORD_H
#include "Arduino.h"

struct EventRecord{
	long startTime;

//	uint32_t highAddress;
//	uint32_t lowAddress;
	uint16_t senderId;
	long endTime;
	long eventGroupStartTime;



};
#endif
