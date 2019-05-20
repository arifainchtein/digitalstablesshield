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
	SolarPowerManager();
	float getCurrentFromBattery(void);
	float getCurrentInputFromSolarPanel(void);
	float getSolarPanelVoltage();
	virtual ~SolarPowerManager();


};

#endif /* LIBRARIES_DIGITALSTABLES_SOLARPOWERMANAGER_H_ */
