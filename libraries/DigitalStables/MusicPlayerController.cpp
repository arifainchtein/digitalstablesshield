/*
 * MusicPlayerController.cpp
 *
 *  Created on: 30 Jul 2019
 *      Author: arifainchtein
 */

#include <MusicPlayerController.h>

// rewind button 1Kohm
int REWIND_BUTTON_VALUE_MIN = 970;
int REWIND_BUTTON_VALUE_MAX = 1020;

// play/pause button 10Kohm
int PLAY_BUTTON_VALUE_MIN = 910;
int PLAY_BUTTON_VALUE_MAX = 950;
//forward button 22Kohm
int FORWARD_BUTTON_VALUE_MIN = 820;
int FORWARD_BUTTON_VALUE_MAX = 870;
// mark button 47Kohm
int MARK_BUTTON_VALUE_MIN = 681;
int MARK_BUTTON_VALUE_MAX = 740;


boolean playingMusic=false;
int lastPreviousValue=-1;
byte currentFunction=0;

byte currentFunctionRewind=1;
byte currentFunctionPlay=1;
byte currentFunctionForward=1;
byte currentFunctionMark=1;

int previousVolume=0;
int VOLUME_TOLERANCE=25;
int numberOfStations=1;
byte previousStation=0;

MusicPlayerController::MusicPlayerController(HardwareSerial& serial ):  _HardSerial(serial){

}

void MusicPlayerController::checkButtonAction( unsigned char musicPlayerPin){

	// TODO Auto-generated destructor stub
	int value=analogRead(musicPlayerPin);
	if(value<=REWIND_BUTTON_VALUE_MAX && value>=REWIND_BUTTON_VALUE_MIN){
		if(currentFunction!=currentFunctionRewind){
			currentFunction = currentFunctionRewind;
			_HardSerial.print("Rewind#");
			_HardSerial.println(value);
		}
	}else if(value<=PLAY_BUTTON_VALUE_MAX && value>=PLAY_BUTTON_VALUE_MIN){
		if(currentFunction!=currentFunctionPlay){
			currentFunction = currentFunctionPlay;
			_HardSerial.println("Play");
		}
	}else if(value<=FORWARD_BUTTON_VALUE_MAX && value>=FORWARD_BUTTON_VALUE_MIN){
		if(currentFunction!=currentFunctionForward){
			currentFunction = currentFunctionForward;
			_HardSerial.print("Forward#");
			_HardSerial.println(value);
		}
	}else if(value<=MARK_BUTTON_VALUE_MAX && value>=MARK_BUTTON_VALUE_MIN){
		if(currentFunction!=currentFunctionMark){
			currentFunction = currentFunctionMark;
			_HardSerial.print("Mark#");
			_HardSerial.println(value);
		}
	}else{
		currentFunction=0;
	}
}
/**
 * the adc produces a number between 0 and 1023
 * divide by 10 to turn it into a percentage
 */
byte MusicPlayerController::checkVolumeChange( unsigned char volumePin){

	// TODO Auto-generated destructor stub
	int value=analogRead(volumePin);
	int change = previousVolume-value;
	byte volumeLevel = value/10;
	if(abs(change)>VOLUME_TOLERANCE){
		_HardSerial.print("VolumeChange#");
		_HardSerial.println(value/10);
		//_HardSerial.print("#");
		//_HardSerial.println(change);
		previousVolume=value;
	}
	return volumeLevel;
}

/**
 * the adc produces a number between 0 and 1023
 * divide by number of stations
 */
byte MusicPlayerController::checkStationChange( unsigned char stationSelectorPin){

	int valueBetweenStation = 1023/numberOfStations;
	//
	// since the first station is always 0 and the last station is always 1000
	// if for exampll numberOfStations = 5 then we need to find the markings for each
	// station in between
	int stations[numberOfStations];
	stations[0]=0;
	for (byte i = 1; i < numberOfStations; i = i + 1) {
		stations[i]=i*valueBetweenStation;
//		_HardSerial.print("i=");
//		_HardSerial.print(i);
//		_HardSerial.print("sta=");
//		_HardSerial.println(stations[i]);
	}
//	stations[numberOfStations-1]=1023;
	int value=analogRead(stationSelectorPin);
	//
	// now check to see if the user has changed
	// the station
	byte currentStation=0;
	for (byte i = 0; i < numberOfStations-1; i = i + 1) {
		if(value>=stations[i] && value<stations[i+1]){
			currentStation=i;
		}
	}

	if(value>=stations[ numberOfStations-1] ){
				currentStation=numberOfStations-1;
			}
	if(currentStation!=previousStation){
		_HardSerial.print("StationChange#");
		_HardSerial.println(currentStation);
//		_HardSerial.print("#");
//		_HardSerial.print(value);
//		_HardSerial.print("#");
//		_HardSerial.print(previousStation);
//		_HardSerial.print("#");
//		_HardSerial.println(valueBetweenStation);
		previousStation=currentStation;
	}
	return currentStation;
}

void MusicPlayerController::setNumberOfStations(byte n){
	numberOfStations=n;
}

byte MusicPlayerController::getNumberOfStations(){
	return numberOfStations;
}

MusicPlayerController::~MusicPlayerController() {


}

