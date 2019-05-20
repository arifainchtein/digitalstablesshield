/*
 * SolarPowerManager.cpp
 *
 *  Created on: 19 May 2019
 *      Author: arifainchtein
 */

#include <SolarPowerManager.h>

#define BATTERY_OUPUT_CURRENT_SENSOR A2
#define SOLAR_PANEL_VOLTAGE_PIN A3
#define SOLAR_PANEL_INPUT_CURRENT_SENSOR A4

float amplitude_current;               //amplitude current
float effective_value;

SolarPowerManager::SolarPowerManager(){}

SolarPowerManager::SolarPowerManager(LCDDisplay& l, SecretManager& s, SDCardManager& sd, TimeManager& t, GeneralFunctions& f,HardwareSerial& serial ): lcd(l),secretManager(s), sdCardManager(sd),timeManager(t), generalFunctions(f), _HardSerial(serial)
{}


float SolarPowerManager::getCurrentInputFromSolarPanel(void){
	int sensorValue;             //value read from the sensor
		int sensorMax = 0;
		uint32_t start_time = millis();
		while((millis()-start_time) < 100)//sample for 1000ms
		{
			sensorValue = analogRead(SOLAR_PANEL_INPUT_CURRENT_SENSOR);
			if (sensorValue > sensorMax)
			{
				//record the maximum sensor value
				sensorMax = sensorValue;
			}
		}

		//the VCC on the Grove interface of the sensor is 5v
		amplitude_current=(float)(sensorMax-512)/1024*5/185*1000000;
		effective_value=amplitude_current/1.414;
		return effective_value;
}


float SolarPowerManager::getSolarPanelVoltage(){
	long  sensorValue=analogRead(SOLAR_PANEL_VOLTAGE_PIN);
	long  sum=0;
	for(int i=0;i<10;i++)
	{
		sum=sensorValue+sum;
		sensorValue=analogRead(SOLAR_PANEL_VOLTAGE_PIN);
		delay(2);
	}
	sum=sum/10;
	float value =(10*sum*4.980/1023.00);
	return value;
}

float SolarPowerManager::getCurrentFromBattery(void){
	int sensorValue;             //value read from the sensor
	int sensorMax = 0;
	uint32_t start_time = millis();
	while((millis()-start_time) < 100)//sample for 1000ms
	{
		sensorValue = analogRead(BATTERY_OUPUT_CURRENT_SENSOR);
		if (sensorValue > sensorMax)
		{
			//record the maximum sensor value
			sensorMax = sensorValue;
		}
	}

	//the VCC on the Grove interface of the sensor is 5v
	amplitude_current=(float)(sensorMax-512)/1024*5/185*1000000;
	effective_value=amplitude_current/1.414;
	return effective_value;
}

SolarPowerManager::~SolarPowerManager() {
	// TODO Auto-generated destructor stub
}

