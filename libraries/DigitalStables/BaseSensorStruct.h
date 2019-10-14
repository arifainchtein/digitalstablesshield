/*
 * BaseSensorStruct.h
 *
 *  Created on: 12 Oct 2019
 *      Author: arifainchtein
 */
#include <Arduino.h>
#ifndef LIBRARIES_DIGITALSTABLES_BASESENSORSTRUCT_H_
#define LIBRARIES_DIGITALSTABLES_BASESENSORSTRUCT_H_

struct BaseSensorStruct :public DiscreteRecord{

		float batteryVoltage ;
		int currentValue;
		float capacitorVoltage;
		byte internalBatteryStateOfCharge;
		float regulatorVoltage;
		uint8_t operatingStatus;
		float dailyMinBatteryVoltage;
		float dailyMaxBatteryVoltage;
		float dailyMinBatteryCurrent;
		float dailyMaxBatteryCurrent;
		float dailyBatteryOutEnergy;
		float dailyPoweredDownInLoopSeconds;
		float hourlyBatteryOutEnergy;
		float hourlyPoweredDownInLoopSeconds;
		long totalDiskUse;
		long totalDiskAvailable;
		byte pauseDuringWPS;
	};


typedef union BaseSensorStructUnion{
  BaseSensorStruct aBaseSensorStruct;
  byte aBaseSensorStructBytes[sizeof(BaseSensorStruct)];
};



#endif /* LIBRARIES_DIGITALSTABLES_BASESENSORSTRUCT_H_ */
