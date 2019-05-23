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
	void begin();
	void display();
	void clear();
	void setRGB(int, int, int);
	void setCursor(uint8_t, uint8_t);
	void print( String );
	void print(float);
	void println( String );
	void println(float);
	void noDisplay();
	void print(long)=0;
	void println(long)=0;
	void print(double)=0;
	void println(double)=0;
	virtual ~NoLCD();
};

#endif /* LIBRARIES_DIGITALSTABLES_NOLCD_H_ */
