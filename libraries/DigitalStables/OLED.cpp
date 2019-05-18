/*
 * OLED.cpp
 *
 *  Created on: 2 May 2019
 *      Author: arifainchtein
 */

#include <OLED.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiAvrI2c oled;


OLED::OLED() {
	// TODO Auto-generated constructor stub

}

void OLED::begin(){
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
  // Call oled.setI2cClock(frequency) to change from the default frequency.

  oled.setFont(Arial14);

  uint32_t m = micros();
  oled.clear();
  oled.println("Hello world!");
  oled.println("A long line may be truncated");
  oled.println();
  oled.set2X();
  oled.println("2X demo");
  oled.set1X();
  oled.print("\nmicros: ");
  oled.print(micros() - m);
  oled.
}

void OLED::clear(){

}
void OLED::setRGB(int r, int g, int b){


}
void OLED::setCursor(uint8_t c, uint8_t r){

}
void OLED::print(const String s){
	oled.print(s);
}

void OLED::print(float s){
	oled.print(s);
}

void OLED::noDisplay(){

}

void OLED::display(){

}


OLED::~OLED() {
	// TODO Auto-generated destructor stub
}

