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
	byte checkVolumeChange();
	byte checkStationChange();
	void setNumberOfStations(byte n);
	byte  getNumberOfStations();
	void refreshLCD();
	virtual ~MusicPlayerController();
};

#endif /* LIBRARIES_DIGITALSTABLES_MUSICPLAYERCONTROLLER_H_ */
