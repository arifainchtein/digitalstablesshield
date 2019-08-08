/*
 * ControlPanelController.cpp
 *
 *  Created on: 7 Aug 2019
 *      Author: arifainchtein
 */

#include <ControlPanelController.h>

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
unsigned char controlPanelButtonPin=2;
int controlPanelControllerCurrentFunctionSelect=1;
int controlPanelControllerCurrentFunctionAct=2;
int controlPanelControllerCurrentFunctionConfirm=3;
int controlPanelControllerCurrentFunction=0;



void ControlPanelController(LCDDisplay & l , PowerManager & p, HardwareSerial& serial);

ControlPanelController::ControlPanelController(LCDDisplay& l, PowerManager & p ,HardwareSerial& serial ):lcd(l),aPowerManager(p),  _HardSerial(serial)
{}

void ControlPanelController::start( unsigned char bp){
	controlPanelButtonPin=bp;
}

void ControlPanelController::checkStatus( ){
	checkButtonAction( );
}

void ControlPanelController::checkButtonAction( ){

	// TODO Auto-generated destructor stub
	controlPanelButtonValue =analogRead(controlPanelButtonPin);
	int currentViewIndex = aPowerManager.getCurrentViewIndex();
	//_HardSerial.println("controlPanelButtonValue:");
	//_HardSerial.println(controlPanelButtonValue);

	if(controlPanelButtonValue<=SELECT_BUTTON_VALUE_MAX && controlPanelButtonValue>=SELECT_BUTTON_VALUE_MIN){

		if(controlPanelControllerCurrentFunction!=controlPanelControllerCurrentFunctionSelect){
			controlPanelControllerCurrentFunction = controlPanelControllerCurrentFunctionSelect;

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

		if(controlPanelControllerCurrentFunction!=controlPanelControllerCurrentFunctionAct){
			controlPanelControllerCurrentFunction = controlPanelControllerCurrentFunctionAct;
			//_HardSerial.println("Act");
		}
	}else if(controlPanelButtonValue<=CONFIRM_BUTTON_VALUE_MAX && controlPanelButtonValue>=CONFIRM_BUTTON_VALUE_MIN){

		if(controlPanelControllerCurrentFunction!=controlPanelControllerCurrentFunctionConfirm){
			controlPanelControllerCurrentFunction = controlPanelControllerCurrentFunctionConfirm;
			//_HardSerial.println("Confirm");
			//_HardSerial.println(controlPanelButtonValue);
		}
	}else{
		controlPanelControllerCurrentFunction=0;
	}
}


ControlPanelController::~ControlPanelController() {
	// TODO Auto-generated destructor stub
}

