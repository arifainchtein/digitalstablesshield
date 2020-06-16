/*
 * XBeeTelepathonPowerManager.h
 *
 *  Created on: 20 May 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_GLORIABASEPOWERMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_GLORIABASEPOWERMANAGER_H_
#include <PowerManager.h>
#include <DataStorageManager.h>
#include <TimeManager.h>
#include <SecretManager.h>
#include <PowerManager.h>
class GloriaBasePowerManager  : public PowerManager{
public:

	SecretManager  secretManager;
	GeneralFunctions  generalFunctions;
	DataStorageManager  &dataStorageManager;
	HardwareSerial& _HardSerial;
	TimeManager & timeManager;
	LCDDisplay&  lcd;
	GloriaBasePowerManager(LCDDisplay & l , SecretManager & s, DataStorageManager & sd, TimeManager & t,  HardwareSerial& serial);

	uint8_t getEnergySourceType();  // 0=Solar 1=Grid
	uint8_t getEnergyStorageType(); //0=Battery 1=Capacitor
	float getEnergySourceVoltage();
	float getEnergyStorageVoltage();
	float getCurrentFromEnergyStorage(void);
	float getCurrentInputFromEnergySource(void);

	void start();

	bool canPublish();

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

	virtual ~GloriaBasePowerManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_GLORIABASEDPOWERMANAGER_H_ */
