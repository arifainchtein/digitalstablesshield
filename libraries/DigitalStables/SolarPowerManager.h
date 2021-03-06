/*
 * SolarPowerManager.h
 *
 *  Created on: 19 May 2019
 *      Author: arifainchtein
 */


#ifndef LIBRARIES_DIGITALSTABLES_SOLARPOWERMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_SOLARPOWERMANAGER_H_

#include "Arduino.h"
#include <PowerManager.h>



class SolarPowerManager : public PowerManager{
public:
	SecretManager  secretManager;
	DataStorageManager  &dataStorageManager;
	//HardwareSerial _HardSerial;
	LCDDisplay&  lcd;
	TimeManager & timeManager;


	SolarPowerManager(LCDDisplay & l , SecretManager & s, DataStorageManager & sd, TimeManager & t, HardwareSerial& serial);

	uint8_t getEnergySourceType();  // 0=Solar 1=Grid
	uint8_t getEnergyStorageType(); //0=Battery 1=Capacitor
	float getEnergySourceVoltage();
	float getEnergyStorageVoltage();
	float getCurrentFromEnergyStorage(void);
	float getCurrentInputFromEnergySource(void);

	void printPowerStatusStructToSerialPort();
	void printPowerStatisticsStructToSerialPort();
	PowerStatusStruct getPowerStatusStruct();
	PowerStatisticsStruct getPowerStatisticsStruct();
	void endOfLoopProcessing();
	void defineState();
	void hourlyTasks(long time, int previousHour );
	void dailyTasks(long time, int yesterdayDate, int yesterdayMonth, uint16_t yesterdayYear);
	void monthlyTasks(long time);
	void yearlyTasks(long time);

	virtual ~SolarPowerManager();


};

#endif /* LIBRARIES_DIGITALSTABLES_SOLARPOWERMANAGER_H_ */
