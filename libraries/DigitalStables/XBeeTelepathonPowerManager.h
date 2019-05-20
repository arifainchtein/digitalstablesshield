/*
 * XBeeTelepathonPowerManager.h
 *
 *  Created on: 20 May 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_XBEETELEPATHONPOWERMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_XBEETELEPATHONPOWERMANAGER_H_

class XBeeTelepathonPowerManager {
public:
	XBeeTelepathonPowerManager();
	float getCurrentFromBattery(void);
	float getCurrentInputFromSolarPanel(void);
	float getSolarPanelVoltage();
	void defineState();
	virtual ~XBeeTelepathonPowerManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_XBEETELEPATHONPOWERMANAGER_H_ */
