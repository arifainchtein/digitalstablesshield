/*
 * GloriaFieldLayer2NeoPixel.h
 *
 *  Created on: 20 Jun 2020
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_GLORIAFIELDLAYER2NEOPIXEL_H_
#define LIBRARIES_DIGITALSTABLES_GLORIAFIELDLAYER2NEOPIXEL_H_
#include <Arduino.h>

class GloriaFieldLayer2NeoPixel {
public:
	int DELAYVAL =500;

	GloriaFieldLayer2NeoPixel(int d);
	void start();
	void  refreshValue(uint8_t id,uint8_t red,uint8_t green, uint8_t blue );
	void setStatusColor(unsigned char pin, String color );
	void setColor(unsigned char pin, uint8_t red, uint8_t green, uint8_t blue );
	void loopOverPrimaries();
	void blink(uint8_t color);
	virtual ~GloriaFieldLayer2NeoPixel();
};

#endif /* LIBRARIES_DIGITALSTABLES_GLORIAFIELDLAYER2NEOPIXEL_H_ */
