/*
 * GertrudizControlPanelController.cpp
 *
 *  Created on: 7 Aug 2019
 *      Author: arifainchtein
 */
#include <Arduino.h>
#include <GertrudizControlPanelController.h>

// rewind button 1Kohm
int SELECT_BUTTON_VALUE_MIN = 950;
int SELECT_BUTTON_VALUE_MAX = 1020;

// play/pause button 10Kohm
int ACT_BUTTON_VALUE_MIN = 850;
int ACT_BUTTON_VALUE_MAX = 949;

//forward button 22Kohm
int CONFIRM_BUTTON_VALUE_MIN = 750;
int CONFIRM_BUTTON_VALUE_MAX = 849;

int controlPanelButtonValue=0;

int GertrudizControlPanelControllerCurrentFunctionSelect=1;
int GertrudizControlPanelControllerCurrentFunctionAct=2;
int GertrudizControlPanelControllerCurrentFunctionConfirm=3;
int GertrudizControlPanelControllerCurrentFunction=0;

#define PHONE_CHARGER_POWER_ENABLE_PIN        1
#define PHONE_CHARGER_LED_PIN                18
#define SPEAKER_POWER_LED_PIN                19
#define ASYNC_CYCLE_LED_PIN                  11
#define SYNC_CYCLE_LED_PIN                   12
#define SPEAKER_POWER_ENABLE_PIN             22

#define CONTROL_PANEL_BUTTON_PIN             A7

void GertrudizControlPanelController(LCDDisplay & l , PowerManager & p, HardwareSerial& serial);

GertrudizControlPanelController::GertrudizControlPanelController(LCDDisplay& l, PowerManager & p ,HardwareSerial& serial ):lcd(l),aPowerManager(p),  _HardSerial(serial)
{}

void GertrudizControlPanelController::start(){


	//
	// Phone USB Port
	//
	//
	// the phone charger uses Pololu D24V25F5
	// which needs to be driven low to be off
	pinMode(PHONE_CHARGER_POWER_ENABLE_PIN, OUTPUT);
	pinMode(PHONE_CHARGER_LED_PIN, OUTPUT);
	turnPhoneUSBPortOn();

	//
		// Speaker USB Port
		//
	pinMode(SPEAKER_POWER_ENABLE_PIN, OUTPUT);
	pinMode(SPEAKER_POWER_LED_PIN, OUTPUT);
	turnSpeakerUSBPortOn();


}

void GertrudizControlPanelController::turnPhoneUSBPortOn(){
	digitalWrite(PHONE_CHARGER_POWER_ENABLE_PIN, HIGH);
	digitalWrite(PHONE_CHARGER_LED_PIN, HIGH);
}
void GertrudizControlPanelController::turnPhoneUSBPortOff(){
	digitalWrite(PHONE_CHARGER_POWER_ENABLE_PIN, LOW);
	digitalWrite(PHONE_CHARGER_LED_PIN, LOW);
}

boolean GertrudizControlPanelController::getPhonePortStatus(){
	return digitalRead(PHONE_CHARGER_POWER_ENABLE_PIN);
}

void GertrudizControlPanelController::turnSpeakerUSBPortOn(){
	digitalWrite(SPEAKER_POWER_ENABLE_PIN, HIGH);
	digitalWrite(SPEAKER_POWER_LED_PIN, HIGH);
}
void GertrudizControlPanelController::turnSpeakerUSBPortOff(){
	digitalWrite(SPEAKER_POWER_ENABLE_PIN, LOW);
	digitalWrite(SPEAKER_POWER_LED_PIN, LOW);
}

boolean GertrudizControlPanelController::getSpeakerUSBPortStatus(){
	return digitalRead(SPEAKER_POWER_ENABLE_PIN);
}

void GertrudizControlPanelController::checkStatus( ){
	checkButtonAction( );
}

void GertrudizControlPanelController::checkButtonAction( ){

	// TODO Auto-generated destructor stub
	controlPanelButtonValue =analogRead(CONTROL_PANEL_BUTTON_PIN);
	int currentViewIndex = aPowerManager.getCurrentViewIndex();
	//_HardSerial.println("controlPanelButtonValue:");
	//_HardSerial.println(controlPanelButtonValue);

	if(controlPanelButtonValue<=SELECT_BUTTON_VALUE_MAX && controlPanelButtonValue>=SELECT_BUTTON_VALUE_MIN){

		if(GertrudizControlPanelControllerCurrentFunction!=GertrudizControlPanelControllerCurrentFunctionSelect){
			GertrudizControlPanelControllerCurrentFunction = GertrudizControlPanelControllerCurrentFunctionSelect;

			if(currentViewIndex<3){
				currentViewIndex++;
				aPowerManager.setCurrentViewIndex(currentViewIndex);
			}else if(currentViewIndex==3){
				aPowerManager.setCurrentViewIndex(0);
			}else if(currentViewIndex==10){
				//
				// if we are here it means the user generated a new password
				// so next time the user toggles the select button
				// go back to the order
				//currentViewIndex=2;
				aPowerManager.setCurrentViewIndex(2);
			}else if(currentViewIndex==30){
				//
				// if we are here it means the user choose
				// to turn off the power supply
				// go back to the order
				//currentViewIndex=2;
				aPowerManager.setCurrentViewIndex(2);

			}else{
				//currentViewIndex=0;
				aPowerManager.setCurrentViewIndex(0);
			}

			//_HardSerial.println("Select");
		}
	}else if(controlPanelButtonValue<=ACT_BUTTON_VALUE_MAX && controlPanelButtonValue>=ACT_BUTTON_VALUE_MIN){

		if(GertrudizControlPanelControllerCurrentFunction!=GertrudizControlPanelControllerCurrentFunctionAct){
			GertrudizControlPanelControllerCurrentFunction = GertrudizControlPanelControllerCurrentFunctionAct;
			//_HardSerial.println("Act");
		}
	}else if(controlPanelButtonValue<=CONFIRM_BUTTON_VALUE_MAX && controlPanelButtonValue>=CONFIRM_BUTTON_VALUE_MIN){

		if(GertrudizControlPanelControllerCurrentFunction!=GertrudizControlPanelControllerCurrentFunctionConfirm){
			GertrudizControlPanelControllerCurrentFunction = GertrudizControlPanelControllerCurrentFunctionConfirm;
			//_HardSerial.println("Confirm");
			//_HardSerial.println(controlPanelButtonValue);
		}
	}else{
		GertrudizControlPanelControllerCurrentFunction=0;
	}
}


GertrudizControlPanelController::~GertrudizControlPanelController() {
	// TODO Auto-generated destructor stub
}

