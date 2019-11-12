/*
 * CapacitorPowerManager.cpp
 *
 *  Created on: 12 Nov 2019
 *      Author: arifainchtein
 */

#include <CapacitorPowerManager.h>
#define CAPACITOR_VOLTAGE_PIN A0
#define ENERGY_SOURCE_VOLTAGE_PIN A2

float publishMinimumVoltage=2.4;






CapacitorPowerManager::CapacitorPowerManager(LCDDisplay& l, SecretManager& s, DataStorageManager& sd, TimeManager& t,HardwareSerial& serial ):PowerManager(l,  s,  sd,  t, serial ), lcd(l),secretManager(s), dataStorageManager(sd),timeManager(t), _HardSerial(serial)
{}

uint8_t CapacitorPowerManager::getEnergySourceType(){
	return SOLAR_ENERGY_SOURCE;
}

uint8_t CapacitorPowerManager::getEnergyStorageType(){
	return CAPACITOR_ENERGY_STORAGE;
}

void float CapacitorPowerManager::getEnergySourceVoltage(){
	float total=0.0;
	uint8_t numberOfSamples=10;
	for(int i=0;i<numberOfSamples;i++){
		total += analogRead(ENERGY_SOURCE_VOLTAGE_PIN);
	}
		return total/numberOfSamples;
}


void CapacitorPowerManager::start(){
	pinMode(CAPACITOR_VOLTAGE_PIN, INPUT);
//	initializeWDT();
}


float CapacitorPowerManager::getEnergyStorageVoltage(){
	float total=0.0;
	uint8_t numberOfSamples=10;
	for(int i=0;i<numberOfSamples;i++){
		total += analogRead(CAPACITOR_VOLTAGE_PIN);
	}
	return total/numberOfSamples;
}

bool CapacitorPowerManager::canPublish(){
	if(getEnergyStorageVoltage()>publishMinimumVoltage){
		return true;
	}else{
		return false;
	}
}



void CapacitorPowerManager::defineState(){
	this->poweredDownInLoopSeconds=0;
	long time = timeManager.getCurrentTimeInSeconds();

	float capacitorVoltage = getEnergyStorageVoltage();
	int internalCapacitorStateOfCharge = generalFunctions.getCapacitorStateOfCharge(capacitorVoltage);



	if(capacitorVoltage>=minWPSVoltage ){
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

				dataStorageManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_END_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);

				lcd.print("Pi ON WPS ");
				lcd.setCursor(0,1);
				lcd.print(capacitorVoltage);
				lcd.print("V ");
				lcd.print(internalCapacitorStateOfCharge);
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
					anWPSSensorRecord.energyStorageVoltage= getEnergyStorageVoltage();
					//anWPSSensorRecord.current = getCurrentFromBattery();
					anWPSSensorRecord.stateOfCharge = generalFunctions.getCapacitorStateOfCharge(anWPSSensorRecord.energyStorageVoltage);
					anWPSSensorRecord.lastWPSRecordSeconds=lastWPSRecordSeconds;
					anWPSSensorRecord.hourlyEnergyStorageOutEnergy=hourlyBatteryOutEnergy;
					anWPSSensorRecord.dailyEnergyStorageOutEnergy=dailyBatteryOutEnergy;
					anWPSSensorRecord.hourlyPoweredDownInLoopSeconds=hourlyPoweredDownInLoopSeconds;
					anWPSSensorRecord.dailyPoweredDownInLoopSeconds=dailyPoweredDownInLoopSeconds;
					anWPSSensorRecord.pauseDuringWPS=pauseDuringWPS;
					anWPSSensorRecord.operatingStatus=operatingStatus;
					anWPSSensorRecord.totalDiskUse=dataStorageManager.getDiskUsage();

					dataStorageManager.saveWPSSensorRecord( anWPSSensorRecord);
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
				operatingStatus=1;
				lcd.setCursor(0,1);
				lcd.print(energyStorgeVoltage);
				lcd.print(" V");
				delay(2000);
				lcd.setRGB(0,0,0);
				lcd.noDisplay();
				dataStorageManager.storeLifeCycleEvent(time,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
				enterArduinoSleep();
			}
		}
	}


}

CapacitorPowerManager::~CapacitorPowerManager() {
	// TODO Auto-generated destructor stub
}

