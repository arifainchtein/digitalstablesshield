/*
 * CapacitorPowerManager.h
 *
 *  Created on: 12 Nov 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_CAPACITORPOWERMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_CAPACITORPOWERMANAGER_H_

#include <PowerManager.h>
#include <DataStorageManager.h>
#include <TimeManager.h>


class CapacitorPowerManager : public PowerManager{
public:
		SecretManager  secretManager;
		DataStorageManager  &dataStorageManager;
		HardwareSerial _HardSerial;
		LCDDisplay&  lcd;
		TimeManager & timeManager;
		GeneralFunctions generalFunctions;

		CapacitorPowerManager(LCDDisplay & l , SecretManager & s, DataStorageManager & sd, TimeManager & t, HardwareSerial& serial);
		void start();
		bool canPublish();
		uint8_t getEnergySourceType();  // 0=Solar 1=Grid
		uint8_t getEnergyStorageType(); //0=Battery 1=Capacitor
		float getEnergySourceVoltage();
		float getEnergyStorageVoltage();
		float getCurrentFromEnergyStorage();
		float getCurrentInputFromEnergySource();

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

	virtual ~CapacitorPowerManager();


};

#endif /* LIBRARIES_DIGITALSTABLES_CAPACITORPOWERMANAGER_H_ */
