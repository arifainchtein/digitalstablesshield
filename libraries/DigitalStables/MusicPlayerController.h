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
	void start(int ns);
	void setNumberOfStations(uint8_t n);
	uint8_t  getNumberOfStations();
	void checkStatus();
	uint8_t getVolumeLevel();
	uint8_t getCurrentStation();
	boolean getNextUserCommand();

	uint8_t musicControllerCurrentFunctionRewind=1;
	uint8_t musicControllerCurrentFunctionPlay=2;
	uint8_t musicControllerCurrentFunctionForward=3;
	uint8_t musicControllerCurrentFunctionMark=4;
	uint8_t musicControllerCurrentFunctionVolumeChange=5;
	uint8_t musicControllerCurrentFunctionStationChange=6;

	virtual ~MusicPlayerController();

private:

	void checkButtonAction();
	uint8_t checkVolumeChange();
	uint8_t checkStationChange();
	void refreshLCD();


};

#endif /* LIBRARIES_DIGITALSTABLES_MUSICPLAYERCONTROLLER_H_ */
