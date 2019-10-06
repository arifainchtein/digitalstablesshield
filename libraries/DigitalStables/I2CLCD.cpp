/*
 * I2Ci2clcd.cpp
 *
 *  Created on: 14 Mar. 2019
 *      Author: arifainchtein
 */
#include "Arduino.h"
#include <I2CLCD.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C i2clcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

I2CLCD::I2CLCD() {
	// TODO Auto-generated constructor stub

}

void I2CLCD::begin(){

	i2clcd.begin(16,2);
  i2clcd.backlight();

}

void I2CLCD::backlight(){
    i2clcd.backlight();
}

void I2CLCD::clear(){
	i2clcd.clear();
}
void I2CLCD::setRGB(int r, int g, int b){
	

}
void I2CLCD::setCursor(uint8_t c, uint8_t r){
	i2clcd.setCursor(c, r);
}
void I2CLCD::print( String  s){
	i2clcd.print(s);

}

void I2CLCD::println( String  s){
	i2clcd.print(s);

}

void I2CLCD::println(float s){
	i2clcd.print(s);
}

void I2CLCD::print(float s){
	i2clcd.print(s);
}

void I2CLCD::println(long s){
	i2clcd.print(s);
}

void I2CLCD::print(long s){
	i2clcd.print(s);
}
void I2CLCD::println(double s){
	i2clcd.print(s);
}

void I2CLCD::print(double s){
	i2clcd.print(s);
}
void I2CLCD::noDisplay(){
	i2clcd.noDisplay();
}

void I2CLCD::display(){
	i2clcd.display();
}

void I2CLCD::print(int s){
    i2clcd.print(s);
}

void I2CLCD::println(int s){
    i2clcd.print(s);
}

void I2CLCD::set2X(){

}
void I2CLCD::set1X(){

}

I2CLCD::~I2CLCD() {
	// TODO Auto-generated destructor stub
}

