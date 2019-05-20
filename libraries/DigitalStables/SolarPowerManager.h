/*
 * SolarPowerManager.h
 *
 *  Created on: 19 May 2019
 *      Author: arifainchtein
 */

#include "Arduino.h"
#include <PowerManager.h>

#ifndef LIBRARIES_DIGITALSTABLES_SOLARPOWERMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_SOLARPOWERMANAGER_H_

class SolarPowerManager : public PowerManager{
public:
	TimeManager  timeManager;
	SecretManager  secretManager;
	GeneralFunctions  generalFunctions;
	SDCardManager  sdCardManager;
	HardwareSerial _HardSerial;
	LCDDisplay&  lcd;
	SolarPowerManager();
	SolarPowerManager(LCDDisplay & l , SecretManager & s, SDCardManager & sd, TimeManager & t, GeneralFunctions  & f, HardwareSerial& serial);
	float getCurrentFromBattery(void);
	float getCurrentInputFromSolarPanel(void);
	float getSolarPanelVoltage();
	virtual ~SolarPowerManager();


};

#endif /* LIBRARIES_DIGITALSTABLES_SOLARPOWERMANAGER_H_ */
