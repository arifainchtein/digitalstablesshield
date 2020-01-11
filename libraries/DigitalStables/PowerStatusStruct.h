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


		uint8_t energySourceType;  // 0=Solar 1=Grid
		uint8_t energyStorageType;  // 0=battery 1=capacitor
		float energyStorageVoltage ;
		float energySourceVoltage;
		int currentFromEnergyStorage;
		int currentFromEnergySource;
		float lockCapacitorVoltage;
		byte internalEnergyStorageStateOfCharge;
		//float regulatorVoltage;
		uint8_t operatingStatus;
		//long totalDiskUse;
		//long totalDiskAvailable;


		//PowerStatusStruct():energySourceType(0), energyStorageType(0), energyStorageVoltage(-1.0) , energySourceVoltage(-1.0),
		//currentFromEnergyStorage(-1),  currentFromEnergySource(-1), lockCapacitorVoltage(-10), internalEnergyStorageStateOfCharge(-1), operatingStatus(-1)
		//{}
	};

struct PowerStatisticsStruct :public DiscreteRecord{


		uint8_t energySourceType;  // 0=Solar 1=Grid
		uint8_t energyStorageType;  // 0=battery 1=capacitor

		float dailyMinEnergyStorageVoltage;
		float dailyMaxEnergyStorageVoltage;
		float dailyMinEnergyStorageCurrent;
		float dailyMaxEnergyStorageCurrent;
		float dailyEnergyStorageOutEnergy;
		float dailyPoweredDownInLoopSeconds;
		float hourlyEnergyStorageOutEnergy;
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
