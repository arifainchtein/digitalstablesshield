/*
 * XBeeTelepathonPowerManager.h
 *
 *  Created on: 20 May 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_XBEETELEPATHONPOWERMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_XBEETELEPATHONPOWERMANAGER_H_
#include <PowerManager.h>
#include <DataStorageManager.h>
#include <TimeManager.h>

class XBeeTelepathonPowerManager  : public PowerManager{
public:

	SecretManager  secretManager;
	GeneralFunctions  generalFunctions;
	DataStorageManager  &dataStorageManager;
	HardwareSerial _HardSerial;
	TimeManager & timeManager;
	LCDDisplay&  lcd;
	XBeeTelepathonPowerManager(LCDDisplay & l , SecretManager & s, DataStorageManager & sd, TimeManager & t,  HardwareSerial& serial);

	uint8_t getEnergySourceType();  // 0=Solar 1=Grid
	uint8_t getEnergyStorageType(); //0=Battery 1=Capacitor
	float getEnergySourceVoltage();
	float getEnergyStorageVoltage();
	float getCurrentFromEnergyStorage(void);
	float getCurrentInputFromEnergySource(void);

	void start();
	void defineState();
	bool canPublish();
	virtual ~XBeeTelepathonPowerManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_XBEETELEPATHONPOWERMANAGER_H_ */
