/*
 * Grovegrove.cpp
 *
 *  Created on: 14 Mar. 2019
 *      Author: arifainchtein
 */
#include "Arduino.h"
#include <GroveLCD.h>
#include <rgb_lcd.h>

rgb_lcd grove;

GroveLCD::GroveLCD() {
	// TODO Auto-generated constructor stub

}

void GroveLCD::begin(){
	grove.begin(16,2);

}

void GroveLCD::backlight(){
    
}

void GroveLCD::clear(){
	grove.clear();
}
void GroveLCD::setRGB(int r, int g, int b){
	grove.setRGB(r,g,b);

}
void GroveLCD::setCursor(uint8_t c, uint8_t r){
	grove.setCursor(c, r);
}
void GroveLCD::print( String  s){
	grove.print(s);

}

void GroveLCD::println( String  s){
	grove.print(s);

}

void GroveLCD::println(float s){
	grove.print(s);
}

void GroveLCD::print(float s){
	grove.print(s);
}

void GroveLCD::println(long s){
	grove.print(s);
}

void GroveLCD::print(long s){
	grove.print(s);
}
void GroveLCD::println(double s){
	grove.print(s);
}

void GroveLCD::print(double s){
	grove.print(s);
}
void GroveLCD::noDisplay(){
	grove.noDisplay();
}

void GroveLCD::display(){
	grove.display();
}

void GroveLCD::print(int s){
	grove.print(s);
}

void GroveLCD::println(int s){
	grove.print(s);

}

void GroveLCD::set2X(){

}
void GroveLCD::set1X(){

}

GroveLCD::~GroveLCD() {
	// TODO Auto-generated destructor stub
}

