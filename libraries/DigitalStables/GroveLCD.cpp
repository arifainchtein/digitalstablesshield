/*
 * GroveLCD.cpp
 *
 *  Created on: 14 Mar. 2019
 *      Author: arifainchtein
 */
#include "Arduino.h"
#include <GroveLCD.h>
#include <rgb_lcd.h>

rgb_lcd lcd;

GroveLCD::GroveLCD() {
	// TODO Auto-generated constructor stub

}

void GroveLCD::begin(long totalDiskUse){
	lcd.begin(16,2);
	lcd.setCursor(0, 0);
	lcd.print("Finish Init") ;
	lcd.setCursor(0, 1);
	lcd.print("SD use ") ;
	lcd.print(totalDiskUse) ;
	lcd.print("Kb") ;
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
void GroveLCD::print( String  s){
	lcd.print(s);

}

void GroveLCD::print(float s){
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

