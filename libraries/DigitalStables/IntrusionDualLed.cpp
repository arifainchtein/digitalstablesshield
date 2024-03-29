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
#define DELAYVAL 500
Adafruit_NeoPixel intrusionleds = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB);


IntrusionDualLed::IntrusionDualLed() {
	// TODO Auto-generated constructor stub

}

void IntrusionDualLed::start(){
	intrusionleds.begin();
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
	//delay(DELAYVAL);
}

void IntrusionDualLed::refreshValue(uint8_t pin,uint8_t red,uint8_t green, uint8_t blue ){

	intrusionleds.setPixelColor(pin, intrusionleds.Color(green,red, blue));
	intrusionleds.show();
}

void IntrusionDualLed::blink(unsigned char pin, String color){
	for(int i=0;i<2;i++){
		setStatusColor(pin, color);
		delay(DELAYVAL);
		setStatusColor(pin, "off");

	}
}

void IntrusionDualLed::loopOverPrimaries(unsigned char pin){
	setStatusColor(pin, "danger");
	delay(DELAYVAL);
	setStatusColor(pin, "warning");
	delay(DELAYVAL);
	setStatusColor(pin, "success");
	delay(DELAYVAL);
	setStatusColor(pin, "off");
	delay(DELAYVAL);
}

void IntrusionDualLed::loopOverPrimaries(){
	setStatusColor(0, "danger");
	setStatusColor(1, "danger");
	delay(DELAYVAL);
	setStatusColor(0, "warning");
	setStatusColor(1, "warning");
	delay(DELAYVAL);
	setStatusColor(0, "success");
	setStatusColor(1, "success");
	delay(DELAYVAL);
	setStatusColor(0, "off");
	setStatusColor(1, "off");

	delay(DELAYVAL);


}

IntrusionDualLed::~IntrusionDualLed() {
	// TODO Auto-generated destructor stub
}

