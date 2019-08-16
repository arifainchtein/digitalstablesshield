/*
 * ControlPanelController.h
 *
 *  Created on: 7 Aug 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_GERTRUDIZCONTROLPANELCONTROLLER_H_
#define LIBRARIES_DIGITALSTABLES_GERTRUDIZCONTROLPANELCONTROLLER_H_
#include <Arduino.h>
#include <LCDDisplay.h>
#include <PowerManager.h>

class GertrudizControlPanelController {

	HardwareSerial& _HardSerial;
	LCDDisplay&  lcd;
	PowerManager& aPowerManager;
public:
	GertrudizControlPanelController(LCDDisplay & l , PowerManager & p, HardwareSerial& serial);
	void start();
	void checkStatus();


	void turnPhoneUSBPortOn();
	void turnPhoneUSBPortOff();
	boolean getPhonePortStatus();

	void turnSpeakerUSBPortOn();
	void turnSpeakerUSBPortOff();
	boolean getSpeakerUSBPortStatus();

	virtual ~GertrudizControlPanelController();

private:
	void checkButtonAction();
};

#endif /* LIBRARIES_DIGITALSTABLES_GERTRUDIZCONTROLPANELCONTROLLER_H_ */
