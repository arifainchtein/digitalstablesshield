/*
 * GeneralFunctions.h
 *
 *  Created on: 13 Feb. 2019
 *      Author: arifainchtein
 */

#ifndef GENERALFUNCTIONS_H_
#define GENERALFUNCTIONS_H_
#include "Arduino.h"

class GeneralFunctions{
public:
	GeneralFunctions();
	static String getValue(String data, char separator, int index);
	static int getStateOfCharge(double batteryVoltage);
	static float stringToFloat(String s);
	static char * generatePassword();
};
#endif /* GENERALFUNCTIONS_H_ */

