/*
 * MusicPlayerController.h
 *
 *  Created on: 30 Jul 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_MUSICPLAYERCONTROLLER_H_
#define LIBRARIES_DIGITALSTABLES_MUSICPLAYERCONTROLLER_H_

#include <Arduino.h>
#include <LCDDisplay.h>


class MusicPlayerController {



	HardwareSerial& _HardSerial;
		LCDDisplay&  lcd;

public:

	MusicPlayerController( HardwareSerial& serial, LCDDisplay& lcd);
	void start(unsigned char vp, unsigned char sp, unsigned char bp, int ns);
	void checkButtonAction();
	uint8_t checkVolumeChange();
	uint8_t checkStationChange();
	void setNumberOfStations(uint8_t n);
	uint8_t  getNumberOfStations();
	void refreshLCD();
	virtual ~MusicPlayerController();
};

#endif /* LIBRARIES_DIGITALSTABLES_MUSICPLAYERCONTROLLER_H_ */
