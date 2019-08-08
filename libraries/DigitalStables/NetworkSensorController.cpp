/*
 * NetworkSensorController.cpp
 *
 *  Created on: 8 Aug 2019
 *      Author: arifainchtein
 */

#include <NetworkSensorController.h>

unsigned char qualityOfServicePin=23;


NetworkSensorController::NetworkSensorController() {
	// TODO Auto-generated constructor stub

}

void start(unsigned char qosp){
	qualityOfServicePin=qosp;
}


void checkStatus(){

}


NetworkSensorController::~NetworkSensorController() {
	// TODO Auto-generated destructor stub
}

