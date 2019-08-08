/*
 * I2CLCD.h
 *
 *  Created on: 14 Mar. 2019
 *      Author: arifainchtein
 */
#include "Arduino.h"
#include <LCDDisplay.h>

#ifndef ARDUINO_LIBRARIES_DIGITALSTABLES_I2CLCD_H_
#define ARDUINO_LIBRARIES_DIGITALSTABLES_I2CLCD_H_

class I2CLCD: public LCDDisplay {
public:
	I2CLCD();


	void begin();
    void backlight();
	void display();
	void clear();
	void setRGB(int, int, int);
	void setCursor(uint8_t, uint8_t);
	void print( String );
	void print(float);
	void println( String );
	void println(float);
	void noDisplay();
	void print(long);
	void println(long);
	void print(double);
	void println(double);
	void print(int);
		void println(int);
		void set2X();
	void set1X();
	virtual ~I2CLCD();

};

#endif /* ARDUINO_LIBRARIES_DIGITALSTABLES_I2CLCD_H_ */
