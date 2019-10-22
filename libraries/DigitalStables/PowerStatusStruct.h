/*
 * BaseSensorStruct.h
 *
 *  Created on: 12 Oct 2019
 *      Author: arifainchtein
 */
#include <Arduino.h>
#include <DiscreteRecord.h>
#ifndef LIBRARIES_DIGITALSTABLES_POWERSTATUSSTRUCT_H_
#define LIBRARIES_DIGITALSTABLES_POWERSTATUSSTRUCT_H_

struct PowerStatusStruct :public DiscreteRecord{

		float batteryVoltage ;
		float solarPanelVoltage;
		int currentFromBattery;
		int currentFromSolarPanel;
		float capacitorVoltage;
		byte internalBatteryStateOfCharge;
		float regulatorVoltage;
		uint8_t operatingStatus;
		//long totalDiskUse;
		//long totalDiskAvailable;
	};

struct PowerStatisticsStruct :public DiscreteRecord{


		float dailyMinBatteryVoltage;
		float dailyMaxBatteryVoltage;
		float dailyMinBatteryCurrent;
		float dailyMaxBatteryCurrent;
		float dailyBatteryOutEnergy;
		float dailyPoweredDownInLoopSeconds;
		float hourlyBatteryOutEnergy;
		float hourlyPoweredDownInLoopSeconds;
		//long totalDiskUse;
		//long totalDiskAvailable;
	};

typedef union PowerStatusStructUnion{
	PowerStatusStruct aPowerStatusStruct;
  byte aPowerStatusStructBytes[sizeof(PowerStatusStruct)];
};

typedef union PowerStatisticsStructUnion{
	PowerStatisticsStruct aPowerStatisticsStruct;
  byte aPowerStatisticsStructBytes[sizeof(PowerStatisticsStruct)];
};


#endif /* LIBRARIES_DIGITALSTABLES_POWERSTATUSSTRUCT_H_ */
