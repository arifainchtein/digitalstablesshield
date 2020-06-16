/*
 * IntrusionDualLed.h
 *
 *  Created on: 9 Jun 2020
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_INTRUSIONDUALLED_H_
#define LIBRARIES_DIGITALSTABLES_INTRUSIONDUALLED_H_
#include "Arduino.h"

class IntrusionDualLed {
public:
	IntrusionDualLed();
	void start(HardwareSerial& serial);
	void  refreshValue(uint8_t id,uint8_t red,uint8_t green, uint8_t blue );
	void setStatusColor(unsigned char pin, String color );
	void setColor(unsigned char pin, uint8_t red, uint8_t green, uint8_t blue );

	virtual ~IntrusionDualLed();
};

#endif /* LIBRARIES_DIGITALSTABLES_INTRUSIONDUALLED_H_ */
