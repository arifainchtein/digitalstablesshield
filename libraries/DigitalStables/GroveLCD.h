/*
 * GroveLCD.h
 *
 *  Created on: 14 Mar. 2019
 *      Author: arifainchtein
 */
#include <Arduino.h>
#include <LCDDisplay.h>

#ifndef ARDUINO_LIBRARIES_DIGITALSTABLES_GROVELCD_H_
#define ARDUINO_LIBRARIES_DIGITALSTABLES_GROVELCD_H_

class GroveLCD: public LCDDisplay {
public:
	GroveLCD();
		virtual void begin(long);
		virtual void clear();
		virtual void setRGB(int, int, int);
		virtual void setCursor(uint8_t, uint8_t);
		virtual void print(const String &);
		virtual void print(float);
		virtual void noDisplay();
		virtual void display();

	virtual ~GroveLCD();

};

#endif /* ARDUINO_LIBRARIES_DIGITALSTABLES_GROVELCD_H_ */
