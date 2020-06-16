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


void CapacitorPowerManager::printPowerStatisticsStructToSerialPort(){
	//
	// Sensor Request Queue Position 1
	//
	PowerStatisticsStruct aPowerStatisticsStruct=getPowerStatisticsStruct();
	char dailyMinEnergyStorageVoltageStr[15];
	dtostrf(aPowerStatisticsStruct.dailyMinEnergyStorageVoltage,4, 0, dailyMinEnergyStorageVoltageStr);
	this->_HardSerial.print(dailyMinEnergyStorageVoltageStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 2
	//

	char dailyMaxEnergyStorageVoltageStr[15];
	dtostrf(aPowerStatisticsStruct.dailyMaxEnergyStorageVoltage,4, 0, dailyMaxEnergyStorageVoltageStr);
	this->_HardSerial.print(dailyMaxEnergyStorageVoltageStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 3
	//

	char dailyMinEnergyStorageCurrentStr[15];
	dtostrf(aPowerStatisticsStruct.dailyMinEnergyStorageCurrent,4, 0, dailyMinEnergyStorageCurrentStr);
	this->_HardSerial.print(dailyMinEnergyStorageCurrentStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 4
	//

	char dailyMaxEnergyStorageCurrentStr[15];
	dtostrf(dailyMaxEnergyStorageCurrent,4, 0, dailyMaxEnergyStorageCurrentStr);
	this->_HardSerial.print(dailyMaxEnergyStorageCurrentStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 5
	//

	char dailyEnergyStorageOutEnergyStr[15];
	dtostrf(dailyEnergyStorageOutEnergy,4, 2, dailyEnergyStorageOutEnergyStr);
	this->_HardSerial.print(dailyEnergyStorageOutEnergyStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 6
	//

	char dailyPoweredDownInLoopSecondsStr[15];
	dtostrf(dailyPoweredDownInLoopSeconds,4, 0, dailyPoweredDownInLoopSecondsStr);
	this->_HardSerial.print(dailyPoweredDownInLoopSecondsStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 7
	//

	char hourlyEnergyStorageOutEnergyStr[15];
	dtostrf(hourlyEnergyStorageOutEnergy,4, 2, hourlyEnergyStorageOutEnergyStr);
	this->_HardSerial.print(hourlyEnergyStorageOutEnergyStr) ;
	this->_HardSerial.print("#") ;
	//
	// Sensor Request Queue Position 8
	//

	char hourlyPoweredDownInLoopSecondsStr[15];
	dtostrf(aPowerStatisticsStruct.hourlyPoweredDownInLoopSeconds,4, 0, hourlyPoweredDownInLoopSecondsStr);
	this->_HardSerial.print(hourlyPoweredDownInLoopSecondsStr) ;
	this->_HardSerial.print("#") ;


}

void CapacitorPowerManager::printPowerStatusStructToSerialPort(){
	PowerStatusStruct aPowerStatusStruct =  getPowerStatusStruct();
	_HardSerial.print(aPowerStatusStruct.sampleTime) ;
	_HardSerial.print("#") ;
	//
	// Sensor Request Queue Position 1
	//
	char solarPanelVoltageStr[15];
	dtostrf(aPowerStatusStruct.energySourceVoltage,4, 1, solarPanelVoltageStr);
	_HardSerial.print(solarPanelVoltageStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 2
	//
	char batteryVoltageStr[15];
	dtostrf(aPowerStatusStruct.energyStorageVoltage,4, 1, batteryVoltageStr);
	_HardSerial.print(batteryVoltageStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 3
	//
	char solarCurrentValueStr[15];
	dtostrf(aPowerStatusStruct.currentFromEnergySource,4, 0, solarCurrentValueStr);
	_HardSerial.print(solarCurrentValueStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 4
	//
	char batteryOutputCurrentValueStr[15];
	dtostrf(aPowerStatusStruct.currentFromEnergyStorage,4, 0, batteryOutputCurrentValueStr);
	_HardSerial.print(solarCurrentValueStr) ;
	_HardSerial.print("#") ;


	//
	// Sensor Request Queue Position 5
	//
	char capacitorVoltageStr[15];
	dtostrf(aPowerStatusStruct.lockCapacitorVoltage,2, 1, capacitorVoltageStr);
	_HardSerial.print(capacitorVoltageStr) ;
	_HardSerial.print("#") ;


	//
	// Sensor Request Queue Position 6
	//
	_HardSerial.print( aPowerStatusStruct.internalEnergyStorageStateOfCharge);
	_HardSerial.print("#") ;



	_HardSerial.print(aPowerStatusStruct. operatingStatus);
	_HardSerial.print("#") ;
}

void CapacitorPowerManager::hourlyTasks(long time, int previousHour ){

	dataStorageManager.storeRememberedValue(time,HOURLY_ENERGY, hourlyEnergyStorageOutEnergy, UNIT_MILLI_AMPERES_HOURS);
	dataStorageManager.storeRememberedValue(time,HOURLY_POWERED_DOWN_IN_LOOP_SECONDS, hourlyPoweredDownInLoopSeconds, UNIT_SECONDS);
	dataStorageManager.storeRememberedValue(time,HOURLY_OPERATING_IN_LOOP_SECONDS, 3600-hourlyPoweredDownInLoopSeconds, UNIT_SECONDS);
	hourlyEnergyStorageOutEnergy=0;
	hourlyPoweredDownInLoopSeconds=0;
}

/*
 * this function is called at the beginning of a new day
 */
void CapacitorPowerManager::dailyTasks(long time, int yesterdayDate, int yesterdayMonth, uint16_t yesterdayYear ){
	//
	// move whatever is in untrasferred to the correct date
	boolean result = dataStorageManager.readUntransferredFileFromSDCardByDate( 1,false, RememberedValueDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear );
	result = dataStorageManager.readUntransferredFileFromSDCardByDate( 1,false, WPSSensorDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear);
	result = dataStorageManager.readUntransferredFileFromSDCardByDate( 1,false, LifeCycleDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear);
	long yesterdayDateSeconds = timeManager.dateAsSeconds(yesterdayYear,yesterdayMonth,yesterdayDate, 0, 0, 0);
	dataStorageManager.storeRememberedValue(time,DAILY_STATS_TIMESTAMP, yesterdayDateSeconds, UNIT_NO_UNIT);
	dataStorageManager.storeRememberedValue(time,DAILY_MINIMUM_BATTERY_VOLTAGE, dailyMinEnergyStorageVoltage, UNIT_VOLT);
	dataStorageManager.storeRememberedValue(time,DAILY_MAXIMUM_BATTERY_VOLTAGE, dailyMaxEnergyStorageVoltage, UNIT_VOLT);
	dataStorageManager.storeRememberedValue(time,DAILY_MINIMUM_BATTERY_CURRENT, dailyMinEnergyStorageCurrent, UNIT_MILLI_AMPERES);
	dataStorageManager.storeRememberedValue(time,DAILY_MAXIMUM_BATTERY_CURRENT, dailyMaxEnergyStorageCurrent, UNIT_MILLI_AMPERES);
	dataStorageManager.storeRememberedValue(time,DAILY_ENERGY, dailyEnergyStorageOutEnergy, UNIT_MILLI_AMPERES_HOURS);
	dataStorageManager.storeRememberedValue(time,DAILY_POWERED_DOWN_IN_LOOP_SECONDS, dailyPoweredDownInLoopSeconds, UNIT_SECONDS);
	dailyMinEnergyStorageVoltage = 9999.0;
	dailyMaxEnergyStorageVoltage = -1.0;
	dailyMinEnergyStorageCurrent = 9999.0;
	dailyMaxEnergyStorageCurrent = -1.0;
	dailyEnergyStorageOutEnergy=0.0;
	dailyPoweredDownInLoopSeconds=0.0;

}

void CapacitorPowerManager::monthlyTasks(long time){

}

void CapacitorPowerManager::yearlyTasks(long time){

}

void CapacitorPowerManager::endOfLoopProcessing(){

	//	endOfLoopProcessing,
	//now=63099
	//currentFromBattery=99.00
	//loopStartingSeconds=1569113725
	//poweredDownInLoopSeconds=0
	//loopConsumingPowerSeconds=12286


	long now = timeManager.getCurrentTimeInSeconds();
	int loopConsumingPowerSeconds = now -loopStartingSeconds -poweredDownInLoopSeconds;
	float currentFromEnergyStorage = getCurrentFromEnergyStorage();
	float currentFromSolarPanel = getCurrentInputFromEnergySource();

	dailyEnergyStorageOutEnergy+= loopConsumingPowerSeconds*currentFromEnergyStorage/3600;
	hourlyEnergyStorageOutEnergy+= loopConsumingPowerSeconds*currentFromEnergyStorage/3600;
	dailyEnergyStorageInEnergy+= loopConsumingPowerSeconds*currentFromSolarPanel/3600;
	hourlyEnergyStorageInEnergy+= loopConsumingPowerSeconds*currentFromSolarPanel/3600;
	dailyPoweredDownInLoopSeconds+=poweredDownInLoopSeconds;
	hourlyPoweredDownInLoopSeconds+=poweredDownInLoopSeconds;
//	_HardSerial.print("dailyEnergyStorageOutEnergy=");
//	_HardSerial.print(dailyEnergyStorageOutEnergy);
//
//	_HardSerial.print("dailyEnergyStorageInEnergy=");
//	_HardSerial.print(dailyEnergyStorageInEnergy);
//
//
//	_HardSerial.print(" poweredDownInLoopSeconds=");
//	_HardSerial.print(poweredDownInLoopSeconds);
//	_HardSerial.print(" loopConsumingPowerSeconds=");
//	_HardSerial.println(loopConsumingPowerSeconds);
//
//	_HardSerial.print(" endOfLoopProcessing, now=");
//	_HardSerial.print(now);
//	_HardSerial.print(" currentFromEnergyStorage=");
//	_HardSerial.print(currentFromEnergyStorage);
//
//	_HardSerial.print(" loopStartingSeconds=");
//	_HardSerial.print(loopStartingSeconds);
//
//	_HardSerial.print(" poweredDownInLoopSeconds=");
//	_HardSerial.print(poweredDownInLoopSeconds);
//	_HardSerial.print(" loopConsumingPowerSeconds=");
//	_HardSerial.println(loopConsumingPowerSeconds);
}

PowerStatisticsStruct CapacitorPowerManager::getPowerStatisticsStruct(){
	//	long totalDiskUse=dataStorageManager.getDiskUsage()/1024;
	PowerStatisticsStruct powerStatisticsStruct;
	powerStatisticsStruct.sampleTime = timeManager.getCurrentTimeInSeconds();
	powerStatisticsStruct.dailyMinEnergyStorageVoltage=this->dailyMinEnergyStorageVoltage;
	powerStatisticsStruct.dailyMaxEnergyStorageVoltage=this->dailyMaxEnergyStorageVoltage;
	powerStatisticsStruct.dailyMinEnergyStorageCurrent=this->dailyMinEnergyStorageCurrent;
	powerStatisticsStruct.dailyMaxEnergyStorageCurrent=this->dailyMaxEnergyStorageCurrent;
	powerStatisticsStruct.dailyEnergyStorageOutEnergy=this->dailyEnergyStorageOutEnergy;
	powerStatisticsStruct.dailyPoweredDownInLoopSeconds=this->dailyPoweredDownInLoopSeconds;
	powerStatisticsStruct.hourlyEnergyStorageOutEnergy=this->hourlyEnergyStorageOutEnergy;
	powerStatisticsStruct.hourlyPoweredDownInLoopSeconds=this->hourlyPoweredDownInLoopSeconds;

	//	aPowerStatusStruct.totalDiskUse=totalDiskUse;

	return powerStatisticsStruct;
}


PowerStatusStruct CapacitorPowerManager::getPowerStatusStruct(){
	float batteryVoltage = getEnergyStorageVoltage();

	float capacitorVoltage= 0.0;//getLockCapacitorVoltage();
	byte internalEnergyStorageStateOfCharge = GeneralFunctions::getStateOfCharge(batteryVoltage);
	//float regulatorVoltage = getVoltageRegulatorOutput();
	//long totalDiskUse=dataStorageManager.getDiskUsage()/1024;

	aPowerStatusStruct.sampleTime = timeManager.getCurrentTimeInSeconds();
	aPowerStatusStruct.energyStorageVoltage= batteryVoltage;
	aPowerStatusStruct.energySourceVoltage= getEnergySourceVoltage();
	aPowerStatusStruct.currentFromEnergySource= getCurrentInputFromEnergySource();
	aPowerStatusStruct.currentFromEnergyStorage=getCurrentFromEnergyStorage();
	aPowerStatusStruct.lockCapacitorVoltage=capacitorVoltage;
	aPowerStatusStruct.internalEnergyStorageStateOfCharge=internalEnergyStorageStateOfCharge;
	//aPowerStatusStruct.regulatorVoltage=regulatorVoltage;
	aPowerStatusStruct.operatingStatus=operatingStatus;

	return aPowerStatusStruct;
}


uint8_t CapacitorPowerManager::getEnergySourceType(){
	return SOLAR_ENERGY_SOURCE;
}

uint8_t CapacitorPowerManager::getEnergyStorageType(){
	return CAPACITOR_ENERGY_STORAGE;
}

float CapacitorPowerManager::getCurrentInputFromEnergySource(void){
	return -1.0;
}

float CapacitorPowerManager::getCurrentFromEnergyStorage(void){
	return -1.0;
}

float CapacitorPowerManager::getEnergySourceVoltage(){
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



//
// source; https://forum.arduino.cc/index.php?topic=204429.0
//
float CapacitorPowerManager::getEnergyStorageVoltage(){ // Returns actual value of Vcc (x 100)


#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
     // For mega boards
     Serial.println(F("Using 1280/2560 Bandgap for calculations "));
     Serial.println();
     const long InternalReferenceVoltage = 1100L;  // Adjust this value to your boards specific internal BG voltage x1000
        // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc reference
        // MUX4 MUX3 MUX2 MUX1 MUX0  --> 11110 1.1V (VBG)         -Selects channel 30, bandgap voltage, to measure
     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR)| (0<<MUX5) | (1<<MUX4) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__)
     // For 1284/644
     Serial.println(F("Using 1284/644 Bandgap for calculations "));
     Serial.println();
     const long InternalReferenceVoltage = 1115L;  // Adjust this value to your boards specific internal BG voltage x1000
        // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc reference
        // MUX4 MUX3 MUX2 MUX1 MUX0  --> 11110 1.1V (VBG)         -Selects channel 30, bandgap voltage, to measure
     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR)| (1<<MUX4) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
#else
     // For 168/328 boards
     Serial.println(F("Using 168/328 Bandgap for calculations"));
     Serial.println();
     const long InternalReferenceVoltage = 1100L;  // Adjust this value to your boards specific internal BG voltage x1000
        // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc external reference
        // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)         -Selects channel 14, bandgap voltage, to measure
     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);

#endif
     delay(50);  // Let mux settle a little to get a more stable A/D conversion
        // Start a conversion
     ADCSRA |= _BV( ADSC );
        // Wait for it to complete
     while( ( (ADCSRA & (1<<ADSC)) != 0 ) );
        // Scale the value
     float  results = (((InternalReferenceVoltage * 1023L) / ADC) + 5L) / 10L; // calculates for straight line value
     return results;

    }

// Returns actual value of Vcc (x 100){









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
					anWPSSensorRecord.hourlyEnergyStorageOutEnergy=hourlyEnergyStorageOutEnergy;
					anWPSSensorRecord.dailyEnergyStorageOutEnergy=dailyEnergyStorageOutEnergy;
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
				lcd.print(capacitorVoltage);
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

