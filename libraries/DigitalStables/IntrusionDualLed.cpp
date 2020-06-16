/*
 * IntrusionDualLed.cpp
 *
 *  Created on: 9 Jun 2020
 *      Author: arifainchtein
 */

#include <IntrusionDualLed.h>
#include <Adafruit_NeoPixel.h>
#define LED_COUNT      2
#define LED_PIN 14

Adafruit_NeoPixel intrusionleds = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB);


IntrusionDualLed::IntrusionDualLed() {
	// TODO Auto-generated constructor stub

}

void IntrusionDualLed::start(HardwareSerial& serial){
	intrusionleds.begin();
	uint8_t red=0;
	uint8_t green=255;
	uint8_t blue=255;

	intrusionleds.setPixelColor(0, intrusionleds.Color(green,red, blue));
	intrusionleds.setPixelColor(1, intrusionleds.Color(green,red, blue));
	intrusionleds.show();
	serial.println("after showing");
}

void IntrusionDualLed::setStatusColor(unsigned char pin, String color ){

		if(color.equals("success")){
			setColor( pin,  0,255,0 );
		}else if(color.equals("primary")){
			setColor(pin,  0,0, 255);
		}else if(color.equals("warning")){
			setColor(pin, 255, 255,  0);
		}else if(color.equals("danger")){
			setColor(pin, 255, 0,  0);
		}else if(color.equals("default")){
			setColor(pin, 255, 255, 255);
		}else if(color.equals("crisis")){
			setColor(pin, 255,165,0);
		}else if(color.equals("stale")){
			setColor(pin, 148,0, 211);
		}else if(color.equals("off")){
			setColor(pin, 0, 0, 0);
		}

}

void IntrusionDualLed::setColor(unsigned char pin, uint8_t red, uint8_t green, uint8_t blue ){

	intrusionleds.setPixelColor(pin, intrusionleds.Color(green,red, blue));
	intrusionleds.show();
	delay(500);
}

void IntrusionDualLed::refreshValue(uint8_t pin,uint8_t red,uint8_t green, uint8_t blue ){

	intrusionleds.setPixelColor(pin, intrusionleds.Color(green,red, blue));
	intrusionleds.show();
}

IntrusionDualLed::~IntrusionDualLed() {
	// TODO Auto-generated destructor stub
}

