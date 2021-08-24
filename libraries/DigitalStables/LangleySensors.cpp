/*
 * LangleySensors.cpp
 *
 *  Created on: 24 Aug 2021
 *      Author: arifainchtein
 */

#include <LangleySensors.h>
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LangleyPinDefinition.h>

OneWire oneWire1(DS18B20Z);
DallasTemperature tempSensor1(&oneWire1);

LangleySensors::LangleySensors() {
	// TODO Auto-generated constructor stub

}

void LangleySensors::start(uint8_t address[]){
	tempSensor1.begin();
	tempSensor1.getAddress(address, 0);
}

float LangleySensors::getTemperature(){
	tempSensor1.requestTemperatures();
	delay(100);
	return tempSensor1.getTempCByIndex(0);
}

LangleySensors::~LangleySensors() {
	// TODO Auto-generated destructor stub
}

