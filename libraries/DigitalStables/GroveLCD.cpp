/*
 * GroveLCD.cpp
 *
 *  Created on: 14 Mar. 2019
 *      Author: arifainchtein
 */

#include <GroveLCD.h>
#include "rgb_lcd.h"
#include <LCDDisplay.h>
rgb_lcd lcd;

GroveLCD::GroveLCD() {
	// TODO Auto-generated constructor stub

}

void GroveLCD::begin(int c,int r){
	lcd.begin(c,r);
}

void GroveLCD::clear(){
	lcd.clear();
}
void GroveLCD::setRGB(int r, int g, int b){
	lcd.setRGB(r,g,b);

}
void GroveLCD::setCursor(uint8_t c, uint8_t r){
	lcd.setCursor(c, r);
}
void GroveLCD::print(const String & s){
	lcd.print(s);

}
void GroveLCD::noDisplay(){
	lcd.noDisplay();
}

void GroveLCD::display(){
	lcd.display();
}


GroveLCD::~GroveLCD() {
	// TODO Auto-generated destructor stub
}

