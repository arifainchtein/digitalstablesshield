/*
 * SolarPowerManager.cpp
 *
 *  Created on: 19 May 2019
 *      Author: arifainchtein
 */

#include <SolarPowerManager.h>

//#ifdef SOLAR_PANEL_VOLTAGE_PIN
//#undef SOLAR_PANEL_VOLTAGE_PIN
//#define SOLAR_PANEL_VOLTAGE_PIN 0
//#endif
//
//#ifdef BATTERY_VOLTAGE_PIN
//#undef BATTERY_VOLTAGE_PIN
//#define BATTERY_VOLTAGE_PIN 1
//#endif
//
//#ifdef SOLAR_PANEL_INPUT_CURRENT_SENSOR
//#undef SOLAR_PANEL_INPUT_CURRENT_SENSOR
//#define SOLAR_PANEL_INPUT_CURRENT_SENSOR 2
//#endif
//
//#ifdef BATTERY_OUPUT_CURRENT_SENSOR
//#undef BATTERY_OUPUT_CURRENT_SENSOR
//#define BATTERY_OUPUT_CURRENT_SENSOR 3
//#endif
//
//#ifdef RS485_SOURCE_VOLTAGE_PIN
//#undef RS485_SOURCE_VOLTAGE_PIN
//#define RS485_SOURCE_VOLTAGE_PIN 4
//#endif
//
//#ifdef LOCK_CAPACITOR_PIN
//#undef LOCK_CAPACITOR_PIN
//#define LOCK_CAPACITOR_PIN 5
//#endif

#define SOLAR_PANEL_VOLTAGE_PIN 0
#define BATTERY_VOLTAGE_PIN 1
#define SOLAR_PANEL_INPUT_CURRENT_SENSOR 2
#define BATTERY_OUPUT_CURRENT_SENSOR 3
#define RS485_SOURCE_VOLTAGE_PIN 4
#define LOCK_CAPACITOR_PIN 5
float amplitude_current;               //amplitude current
float effective_value;


SolarPowerManager::SolarPowerManager(LCDDisplay& l, SecretManager& s, DataStorageManager& sd, TimeManager& t,HardwareSerial& serial ):PowerManager(l,  s,  sd,  t, serial ), lcd(l),secretManager(s), dataStorageManager(sd),timeManager(t), _HardSerial(serial)
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

