/*
 * AquabubblerTelephatonPowerManager.h
 *
 *  Created on: 21 Oct 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_AQUABUBBLERTELEPHATONPOWERMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_AQUABUBBLERTELEPHATONPOWERMANAGER_H_

#include <PowerManager.h>
#include <DataStorageManager.h>
#include <TimeManager.h>

class AquabubblerTelephatonPowerManager  : public PowerManager{
public:
	AquabubblerTelephatonPowerManager();
		SecretManager  secretManager;
		GeneralFunctions  generalFunctions;
		DataStorageManager  &dataStorageManager;
		HardwareSerial _HardSerial;
		TimeManager & timeManager;
		LCDDisplay&  lcd;

		AquabubblerTelephatonPowerManager(LCDDisplay & l , SecretManager & s, DataStorageManager & sd, TimeManager & t,  HardwareSerial& serial);

		float getEnergySourceVoltage();
		float getEnergyStorageVoltage();
		float getCurrentFromEnergyStorage(void);
		float getCurrentInputFromEnergySource(void);
		uint8_t getEnergySourceType();  // 0=Solar 1=Grid
		uint8_t getEnergyStorageType(); //0=Battery 1=Capacitor
		void printPowerStatusStructToSerialPort();
		void printPowerStatisticsStructToSerialPort();

		void start();
		void defineState();
		bool canPublish();
		PowerStatusStruct getPowerStatusStruct();
		PowerStatisticsStruct getPowerStatisticsStruct();
		void digitalWrite(int port, byte state);//PinWrite(int port, byte state);
		void pinMode(int port, byte state);
		boolean processDefaultCommands(String command);
		void endOfLoopProcessing();
		void hourlyTasks(long time, int previousHour );
		void dailyTasks(long time, int yesterdayDate, int yesterdayMonth, uint16_t yesterdayYear);
		void monthlyTasks(long time);
		void yearlyTasks(long time);
		void printSOS();

	virtual ~AquabubblerTelephatonPowerManager();

private:
	void blinkLed(uint16_t delay);

};

#endif /* LIBRARIES_DIGITALSTABLES_AQUABUBBLERTELEPHATONPOWERMANAGER_H_ */
