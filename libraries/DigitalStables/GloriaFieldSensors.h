/*
 * GloriaFieldSensors.h
 *
 *  Created on: 22 Jun 2020
 *      Author: arifainchtein
 */
#include <Arduino.h>

#ifndef LIBRARIES_DIGITALSTABLES_GLORIAFIELDSENSORS_H_
#define LIBRARIES_DIGITALSTABLES_GLORIAFIELDSENSORS_H_

class GloriaFieldSensors {
public:
	GloriaFieldSensors( );

	void setTankPerimiter(double);
	void setTankHeight(double);
	double getTankWaterCurrentVolume();
	void start(uint8_t a[]);
	float getTemperature();//{
	float getSourceVoltage(void);// Returns actual value of Vcc (x 100)
	double getTankWaterLevel();
	virtual ~GloriaFieldSensors();
};

#endif /* LIBRARIES_DIGITALSTABLES_GLORIAFIELDSENSORS_H_ */
