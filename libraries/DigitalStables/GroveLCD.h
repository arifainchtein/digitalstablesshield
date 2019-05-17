/*
 * GroveLCD.h
 *
 *  Created on: 14 Mar. 2019
 *      Author: arifainchtein
 */
#include "Arduino.h"
#include <LCDDisplay.h>

#ifndef ARDUINO_LIBRARIES_DIGITALSTABLES_GROVELCD_H_
#define ARDUINO_LIBRARIES_DIGITALSTABLES_GROVELCD_H_

class GroveLCD: public LCDDisplay {
public:
	GroveLCD();


	void begin();
	void display();
	void clear();
	void setRGB(int, int, int);
	void setCursor(uint8_t, uint8_t);
	void print( String );
	void print(float);
	void noDisplay();

	virtual ~GroveLCD();

};

#endif /* ARDUINO_LIBRARIES_DIGITALSTABLES_GROVELCD_H_ */
