/*
 * GloriaFieldLayer2NeoPixel.cpp
 *
 *  Created on: 20 Jun 2020
 *      Author: arifainchtein
 */

#include <GloriaFieldLayer2NeoPixel.h>
#include <Adafruit_NeoPixel.h>


// How many NeoPixels are attached to the Arduino?
#define LAYER2_NUMPIXELS 3
#define LAYER2_NEOPIXEL_PIN 4


Adafruit_NeoPixel pixelsLayer2(LAYER2_NUMPIXELS, LAYER2_NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);


GloriaFieldLayer2NeoPixel::GloriaFieldLayer2NeoPixel(int d):DELAYVAL(d) {
	// TODO Auto-generated constructor stub

}

void GloriaFieldLayer2NeoPixel::start(){
	pixelsLayer2.begin();
	pixelsLayer2.setPixelColor(0, pixelsLayer2.Color(255, 255, 0));
	pixelsLayer2.setPixelColor(1, pixelsLayer2.Color(255, 255, 0));
	pixelsLayer2.setPixelColor(2, pixelsLayer2.Color(255, 255, 0));
	pixelsLayer2.show();   // Send the updated pixel colors to the hardware.

}
void GloriaFieldLayer2NeoPixel::blink(uint8_t color){
	for(int i=0;i<2;i++){
		pixelsLayer2.setPixelColor(0, color);
		pixelsLayer2.setPixelColor(1, color);
		pixelsLayer2.setPixelColor(2, color);
		pixelsLayer2.show();
		delay(DELAYVAL);
		pixelsLayer2.setPixelColor(0, pixelsLayer2.Color(0 ,0, 0));
		pixelsLayer2.setPixelColor(1, pixelsLayer2.Color(0 ,0, 0));
		pixelsLayer2.setPixelColor(2, pixelsLayer2.Color(0 ,0, 0));
		pixelsLayer2.show();
		delay(DELAYVAL);
	}
}

void GloriaFieldLayer2NeoPixel::setStatusColor(unsigned char pin, String color ){

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

void GloriaFieldLayer2NeoPixel::setColor(unsigned char pin, uint8_t red, uint8_t green, uint8_t blue ){

	pixelsLayer2.setPixelColor(pin, pixelsLayer2.Color(green,red, blue));
	pixelsLayer2.show();
	delay(DELAYVAL);
}

void GloriaFieldLayer2NeoPixel::refreshValue(uint8_t pin,uint8_t red,uint8_t green, uint8_t blue ){

	pixelsLayer2.setPixelColor(pin, pixelsLayer2.Color(green,red, blue));
	pixelsLayer2.show();
}

GloriaFieldLayer2NeoPixel::~GloriaFieldLayer2NeoPixel() {
	// TODO Auto-generated destructor stub
}

void GloriaFieldLayer2NeoPixel::loopOverPrimaries(){
	// pixelsLayer2.clear(); // Set all pixel colors to 'off'
	pixelsLayer2.setPixelColor(0, pixelsLayer2.Color(0, 255, 0));
	pixelsLayer2.show();   // Send the updated pixel colors to the hardware.
	delay(DELAYVAL); // Pause before next pass through loop

	pixelsLayer2.setPixelColor(1, pixelsLayer2.Color(255, 0, 0));
	pixelsLayer2.show();   // Send the updated pixel colors to the hardware.
	delay(DELAYVAL); // Pause before next pass through loop


	pixelsLayer2.setPixelColor(2, pixelsLayer2.Color(0,0, 255));
	pixelsLayer2.show();   // Send the updated pixel colors to the hardware.
	delay(DELAYVAL); // Pause before next pass through loop


	// pixelsLayer2.clear(); // Set all pixel colors to 'off'
	pixelsLayer2.setPixelColor(0, pixelsLayer2.Color(0,0, 0));
	pixelsLayer2.show();
}

