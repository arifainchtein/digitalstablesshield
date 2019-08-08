/*
 * ControlPanelController.h
 *
 *  Created on: 7 Aug 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_CONTROLPANELCONTROLLER_H_
#define LIBRARIES_DIGITALSTABLES_CONTROLPANELCONTROLLER_H_
#include <Arduino.h>
#include <LCDDisplay.h>
#include <PowerManager.h>

class ControlPanelController {

	HardwareSerial& _HardSerial;
	LCDDisplay&  lcd;
	PowerManager& aPowerManager;
public:
	ControlPanelController(LCDDisplay & l , PowerManager & p, HardwareSerial& serial);
	void start(unsigned char bp);
	void checkStatus();
	virtual ~ControlPanelController();

private:
	void checkButtonAction();
};

#endif /* LIBRARIES_DIGITALSTABLES_CONTROLPANELCONTROLLER_H_ */
