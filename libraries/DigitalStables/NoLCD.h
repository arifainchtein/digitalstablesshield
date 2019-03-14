/*
 * NoLCD.h
 *
 *  Created on: 14 Mar 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_NOLCD_H_
#define LIBRARIES_DIGITALSTABLES_NOLCD_H_

#include <Arduino.h>
#include <LCDDisplay.h>

class NoLCD: public LCDDisplay {
public:
	NoLCD();
	void begin(long);
	void display();
	void clear();
	void setRGB(int, int, int);
	void setCursor(uint8_t, uint8_t);
	void print(const String &);
	void print(float);
	void noDisplay();
	void display();
	virtual ~NoLCD();
};

#endif /* LIBRARIES_DIGITALSTABLES_NOLCD_H_ */
