/*
 * LangleyData.h
 *
 *  Created on: 24 Aug 2021
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_LANGLEYDATA_H_
#define LIBRARIES_DIGITALSTABLES_LANGLEYDATA_H_


#include <Arduino.h>


#ifndef LANGLEYDATA_H
#define LANGLEYDATA_H
struct LangleyData{

	uint8_t langleyId[8];
	uint8_t fieldId=0;
	long secondsTime=0L;
	float capacitorVoltage=0.0;
	float externalBatteryVoltage=0.0;
	float temperature=0.0;
	float fenceVoltage=0.0;
	int rssi=0;
	int snr=0;
};
#endif


#endif /* LIBRARIES_DIGITALSTABLES_LANGLEYDATA_H_ */
