/*
 * MusicPlayerController.h
 *
 *  Created on: 30 Jul 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_MUSICPLAYERCONTROLLER_H_
#define LIBRARIES_DIGITALSTABLES_MUSICPLAYERCONTROLLER_H_

#include <Arduino.h>




class MusicPlayerController {



	HardwareSerial& _HardSerial;


public:

	MusicPlayerController( HardwareSerial& serial);
	void checkButtonAction(unsigned char pin);
	byte checkVolumeChange(unsigned char pin);
	byte checkStationChange(unsigned char pin);
	void setNumberOfStations(byte n);
	byte  getNumberOfStations();
	virtual ~MusicPlayerController();
};

#endif /* LIBRARIES_DIGITALSTABLES_MUSICPLAYERCONTROLLER_H_ */
