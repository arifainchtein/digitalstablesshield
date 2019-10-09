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
#include <SDCardManager.h>
#include <LCDDisplay.h>
#include <WPSSensorRecord.h>
#include <SD.h>
#include <GeneralFunctions.h>

#include <SecretManager.h>


class SolarPowerManager : public PowerManager{
public:
	TimeManager  timeManager;
	SecretManager  secretManager;
	GeneralFunctions  generalFunctions;
	DataStorageManager  &dataStorageManager;
	HardwareSerial _HardSerial;
	LCDDisplay&  lcd;

	SolarPowerManager(LCDDisplay & l , SecretManager & s, DataStorageManager & sd, TimeManager & t, GeneralFunctions  & f, HardwareSerial& serial);
	float getCurrentFromBattery(void);
	float getCurrentInputFromSolarPanel(void);
	float getSolarPanelVoltage();
	virtual ~SolarPowerManager();


};

#endif /* LIBRARIES_DIGITALSTABLES_SOLARPOWERMANAGER_H_ */
