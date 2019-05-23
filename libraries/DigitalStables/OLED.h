/*
 * OLED.h
 *
 *  Created on: 2 May 2019
 *      Author: arifainchtein
 */
#include "Arduino.h"

#ifndef LIBRARIES_DIGITALSTABLES_OLED_H_
#define LIBRARIES_DIGITALSTABLES_OLED_H_

#include <LCDDisplay.h>

class OLED: public LCDDisplay {
public:
	OLED();
	void begin();
	void display();
	void clear();
	void setRGB(int, int, int);
	void print( String);
	void print(float);
	void println( String);
	void println(float);
	void noDisplay();
	void setCursor(uint8_t, uint8_t);
	void print(long)=0;
	void println(long)=0;
	void print(double)=0;
	void println(double)=0;
	virtual ~OLED();

};

#endif /* LIBRARIES_DIGITALSTABLES_OLED_H_ */
