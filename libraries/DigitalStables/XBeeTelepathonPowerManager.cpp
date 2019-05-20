/*
 * XBeeTelepathonPowerManager.cpp
 *
 *  Created on: 20 May 2019
 *      Author: arifainchtein
 */

#include <XBeeTelepathonPowerManager.h>

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


XBeeTelepathonPowerManager::XBeeTelepathonPowerManager(LCDDisplay& l, SecretManager& s, SDCardManager& sd, TimeManager& t, GeneralFunctions& f,HardwareSerial& serial ): lcd(l),secretManager(s), sdCardManager(sd),timeManager(t), generalFunctions(f), _HardSerial(serial)
{}

float XBeeTelepathonPowerManager::getCurrentInputFromSolarPanel(void){
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


float XBeeTelepathonPowerManager::getSolarPanelVoltage(){
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

float XBeeTelepathonPowerManager::getCurrentFromBattery(void){
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
void defineState(){
	poweredDownInLoopSeconds=0;
	long time = timeManager.getCurrentTimeInSeconds();

	float batteryVoltage = getBatteryVoltage();
	int internalBatteryStateOfCharge = generalFunctions.getStateOfCharge(batteryVoltage);
	float currentFromBattery = getCurrentFromBattery();
	float inputFromSOlarPanel =  getCurrentInputFromSolarPanel();
	float solarPanelVolltage = getSolarPanelVoltage();



	if(batteryVoltage>=minWPSVoltage ){
		if(wpsSleeping){
			//delay(1000);
			//lcd.noDisplay();
			long piSleepingRemaining = secondsToNextPiOn-(time - currentSleepStartTime) ;
			lcd.clear();
			lcd.display();
			lcd.setCursor(0,0);
			lcd.setRGB(255,255,0);

			if(piSleepingRemaining<=0){
				wpsSleeping=false;

				sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_END_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);

				lcd.print("Pi ON WPS ");
				lcd.setCursor(0,1);
				lcd.print(batteryVoltage);
				lcd.print("V ");
				lcd.print(internalBatteryStateOfCharge);
				lcd.print("%") ;
				lastWPSStartUp = time;
			}else{
				//
				// if we are here is because we are in the
				// sleep period of the wps cycle.
				// check to see if we need to store a record in the sd card
				//
				long z =time-lastWPSRecordSeconds;
				lcd.print("WPS rec in ");
				long netWPSRecordIn = (long)wpsPulseFrequencySeconds-z;

				lcd.print(netWPSRecordIn);
				lcd.setCursor(0,1);
				lcd.print("pi on in ");
				long piremaining = secondsToNextPiOn-(time - currentSleepStartTime) ;
				lcd.print(piremaining);


				//delay(1000);

				if(netWPSRecordIn<=0){
					lcd.clear();
					lcd.display();

					lastWPSRecordSeconds = timeManager.getCurrentTimeInSeconds();
					WPSSensorRecord anWPSSensorRecord;
					anWPSSensorRecord.batteryVoltage= getBatteryVoltage();
					anWPSSensorRecord.current = getCurrentFromBattery();
					anWPSSensorRecord.stateOfCharge = generalFunctions.getStateOfCharge(batteryVoltage);
					anWPSSensorRecord.lastWPSRecordSeconds=lastWPSRecordSeconds;
					anWPSSensorRecord.hourlyBatteryOutEnergy=hourlyBatteryOutEnergy;
					anWPSSensorRecord.dailyBatteryOutEnergy=dailyBatteryOutEnergy;
					anWPSSensorRecord.hourlyPoweredDownInLoopSeconds=hourlyPoweredDownInLoopSeconds;
					anWPSSensorRecord.dailyPoweredDownInLoopSeconds=dailyPoweredDownInLoopSeconds;
					anWPSSensorRecord.pauseDuringWPS=pauseDuringWPS;
					anWPSSensorRecord.operatingStatus=operatingStatus;
					anWPSSensorRecord.totalDiskUse=989; sdCardManager.getDiskUsage();

					sdCardManager.saveWPSSensorRecord( anWPSSensorRecord);
					lcd.setRGB(255,255,0);
				}else{
					//
					// if we are here is because we are in the sleeping part of the
					// WPS and is not time to take another record
					// now check if there is any reason to keep it from comma
					// ie if its raining and the sensor needs to stay on
					// if not sleep for 8 seconds
					//

					if(pauseDuringWPS){
						pauseWPS();
					}
				}
			}
		}


	}else{
		if(wpsSleeping){

			if(f_wdt == 1){
				/* Don't forget to clear the flag. */
				f_wdt = 0;
				/* Re-enter sleep mode. */
				lcd.clear();
				lcd.setRGB(255,0,0);
				lcd.setCursor(0,0);
				lcd.print("Enter Comma");
				operatingStatus="Comma";
				lcd.setCursor(0,1);
				lcd.print(batteryVoltage);
				lcd.print(" V");
				delay(2000);
				lcd.setRGB(0,0,0);
				lcd.noDisplay();
				sdCardManager.storeLifeCycleEvent(time,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
				enterArduinoSleep();
			}
		}
	}


}
XBeeTelepathonPowerManager::~XBeeTelepathonPowerManager() {
	// TODO Auto-generated destructor stub
}

