/*
 * XBeeTelepathonPowerManager.h
 *
 *  Created on: 20 May 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_XBEETELEPATHONPOWERMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_XBEETELEPATHONPOWERMANAGER_H_
#include <PowerManager.h>
class XBeeTelepathonPowerManager  : public PowerManager{
public:
	TimeManager  timeManager;
	SecretManager  secretManager;
	GeneralFunctions  generalFunctions;
	SDCardManager  sdCardManager;
	HardwareSerial _HardSerial;
	LCDDisplay&  lcd;
	XBeeTelepathonPowerManager(LCDDisplay & l , SecretManager & s, SDCardManager & sd, TimeManager & t, GeneralFunctions  & f, HardwareSerial& serial);

	float getCurrentFromBattery(void);
	float getCurrentInputFromSolarPanel(void);
	float getSolarPanelVoltage();
	void defineState();
	virtual ~XBeeTelepathonPowerManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_XBEETELEPATHONPOWERMANAGER_H_ */
