/*
 * NoLCD.h
 *
 *  Created on: 14 Mar 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_NOLCD_H_
#define LIBRARIES_DIGITALSTABLES_NOLCD_H_

#include <Arduino.h>

class NoLCD {
public:
	NoLCD();
	void begin();
	void display();
	void clear();
	void setRGB(int, int, int);
	void setCursor(uint8_t, uint8_t);
	void print( String );
	void print(float);
	void noDisplay();

	virtual ~NoLCD();
};

#endif /* LIBRARIES_DIGITALSTABLES_NOLCD_H_ */
