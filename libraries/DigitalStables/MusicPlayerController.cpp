/*
 * MusicPlayerController.cpp
 *
 *  Created on: 30 Jul 2019
 *      Author: arifainchtein
 */

#include <MusicPlayerController.h>
#include <LCDDisplay.h>


// rewind button 1Kohm
int REWIND_BUTTON_VALUE_MIN = 920;
int REWIND_BUTTON_VALUE_MAX = 1020;

// play/pause button 10Kohm
int PLAY_BUTTON_VALUE_MIN = 850;
int PLAY_BUTTON_VALUE_MAX = 919;
//forward button 22Kohm
int FORWARD_BUTTON_VALUE_MIN = 750;
int FORWARD_BUTTON_VALUE_MAX = 849;
// mark button 47Kohm
int MARK_BUTTON_VALUE_MIN = 681;
int MARK_BUTTON_VALUE_MAX = 749;
int musicbuttonValue=0;

boolean playingMusic=false;
int lastPreviousValue=-1;
uint8_t musicControllerCurrentFunction=0;

int musicControllerCurrentFunctionRewind=1;
int musicControllerCurrentFunctionPlay=2;
int musicControllerCurrentFunctionForward=3;
int musicControllerCurrentFunctionMark=4;

int previousVolume=0;
int VOLUME_TOLERANCE=25;
uint8_t numberOfStations=1;
uint8_t previousStation=0;
uint8_t volumeLevel=0;
uint8_t currentStation=0;
unsigned char volumePin=0;
unsigned char stationSelectorPin=1;
unsigned char musciButtonPin=2;

MusicPlayerController::MusicPlayerController(HardwareSerial& serial, LCDDisplay& l ):  _HardSerial(serial),  lcd(l){

}

void MusicPlayerController::start(unsigned char vp, unsigned char sp, unsigned char bp, int ns){
	volumePin=vp;
	musciButtonPin=bp;
	stationSelectorPin=sp;
	numberOfStations=ns;
	int volumePinValue=analogRead(volumePin);
	volumeLevel = volumePinValue/10;

	/////////////////
	int valueBetweenStation = 1023/numberOfStations;
	int stations[numberOfStations];
	stations[0]=0;
	for (byte i = 1; i < numberOfStations; i = i + 1) {
		stations[i]=i*valueBetweenStation;		
	}
	int stationPinValue=analogRead(stationSelectorPin);
	//
	// now check to see if the user has changed
	// the station
	
	for (byte i = 0; i < numberOfStations-1; i = i + 1) {
		if(stationPinValue>=stations[i] && stationPinValue<stations[i+1]){
			currentStation=i;
		}
	}
			
	refreshLCD();
	
}
 void MusicPlayerController::checkStatus( ){
	 checkButtonAction(  );
	   delay(10);
	   uint8_t volume =  checkVolumeChange();
	   delay(10);
	   uint8_t station  = checkStationChange( );
	   delay(10);
	   refreshLCD();
 }

 void MusicPlayerController::setNumberOfStations(uint8_t n){
 	numberOfStations=n;
 }

 uint8_t MusicPlayerController::getNumberOfStations(){
 	return numberOfStations;
 }

void MusicPlayerController::checkButtonAction( ){

	// TODO Auto-generated destructor stub
	musicbuttonValue =analogRead(musciButtonPin);
	
	if(musicbuttonValue<=REWIND_BUTTON_VALUE_MAX && musicbuttonValue>=REWIND_BUTTON_VALUE_MIN){
		
		if(musicControllerCurrentFunction!=musicControllerCurrentFunctionRewind){
			musicControllerCurrentFunction = musicControllerCurrentFunctionRewind;
			_HardSerial.println("Rewind");
		}
	}else if(musicbuttonValue<=PLAY_BUTTON_VALUE_MAX && musicbuttonValue>=PLAY_BUTTON_VALUE_MIN){
		
		if(musicControllerCurrentFunction!=musicControllerCurrentFunctionPlay){
			musicControllerCurrentFunction = musicControllerCurrentFunctionPlay;
			_HardSerial.println("Play");
		}
	}else if(musicbuttonValue<=FORWARD_BUTTON_VALUE_MAX && musicbuttonValue>=FORWARD_BUTTON_VALUE_MIN){
		
		if(musicControllerCurrentFunction!=musicControllerCurrentFunctionForward){
			musicControllerCurrentFunction = musicControllerCurrentFunctionForward;
			_HardSerial.println("Forward");
			//_HardSerial.println(musicbuttonValue);
		}
	}else if(musicbuttonValue<=MARK_BUTTON_VALUE_MAX && musicbuttonValue>=MARK_BUTTON_VALUE_MIN){
		
		if(musicControllerCurrentFunction!=musicControllerCurrentFunctionMark){
			musicControllerCurrentFunction = musicControllerCurrentFunctionMark;
			_HardSerial.println("Mark");
			//_HardSerial.println(musicbuttonValue);
		}
	}else{
		musicControllerCurrentFunction=0;
	}
}
/**
 * the adc produces a number between 0 and 1023
 * divide by 10 to turn it into a percentage
 */
uint8_t MusicPlayerController::checkVolumeChange( ){

	// TODO Auto-generated destructor stub
	int value=analogRead(volumePin);
	int change = previousVolume-value;
	volumeLevel = value/10;
	if(abs(change)>VOLUME_TOLERANCE){
		previousVolume = value;
		_HardSerial.print("VolumeChange#");
		_HardSerial.print(volumeLevel);
		_HardSerial.print("#");
		_HardSerial.print(value);
		_HardSerial.print("#");
		_HardSerial.print(previousVolume);
		_HardSerial.print("#");
		_HardSerial.println(change);
		
	}
	return volumeLevel;
}

/**
 * the adc produces a number between 0 and 1023
 * divide by number of stations
 */
uint8_t MusicPlayerController::checkStationChange( ){

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
	
	for (int i = 0; i < numberOfStations-1; i = i + 1) {
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



void MusicPlayerController::refreshLCD(){
	lcd.clear();
   		
	if(musicControllerCurrentFunction==0){
		lcd.setCursor(0,0);
  		lcd.print("Vol:") ;
  		lcd.print(volumeLevel) ;
  		lcd.print(" Sta:") ;
		lcd.print(currentStation) ;
		lcd.print(" ") ;
		lcd.print(musicbuttonValue);
		lcd.setCursor(0,1);
		lcd.print("Spotify") ;
		lcd.print("                ") ;
	}else if(musicControllerCurrentFunction==musicControllerCurrentFunctionRewind){
		
   		lcd.setCursor(0,0);		   
  		lcd.print("Rewind           ") ;
        lcd.setCursor(0,1);		   
  		lcd.print("                 ") ;
	    //lcd.print(musicbuttonValue);
	}else if(musicControllerCurrentFunction==musicControllerCurrentFunctionPlay){
		
   		lcd.setCursor(0,0);		   
  		lcd.print("Play             ") ;
        lcd.setCursor(0,1);		   
  		lcd.print("                 ") ;
		//lcd.print(musicbuttonValue);
	}else if(musicControllerCurrentFunction==musicControllerCurrentFunctionForward){
		
   		lcd.setCursor(0,0);		   
  		lcd.print("Forward           ") ;
        lcd.setCursor(0,1);		   
  		lcd.print("                  ") ;
		//lcd.print(musicbuttonValue);
	}else if(musicControllerCurrentFunction==musicControllerCurrentFunctionMark){
		
   		lcd.setCursor(0,0);		   
  		lcd.print("Mark             ") ;
        lcd.setCursor(0,1);		   
  		lcd.print("                 ") ;
		//lcd.print(musicbuttonValue);
	}
}
 

MusicPlayerController::~MusicPlayerController() {


}

