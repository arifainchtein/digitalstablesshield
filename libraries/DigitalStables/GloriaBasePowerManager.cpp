/*
 * GloriaBasePowerManager.cpp
 *
 *  Created on: 20 May 2019
 *      Author: arifainchtein
 */

#include <GloriaBasePowerManager.h>
#include <PowerManager.h>
#include <SPI.h>



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


//#define  Vin_solar   0//ADC_0
//#define  Vin_battery 1//ADC_1
//#define  Cin_solar   2//ADC_2
//#define  Cin_battery 3//ADC_3
//#define  Vin_485     4//ADC_4
//#define  Vin_temp    5//ADC_5

//
// using the Gertrudiz V1.2 board
//
#define PI_POWER_ENABLE 1
#define BATTERY_VOLTAGE_PIN A1
#define CURRENT_SENSOR A4
#define LOCK_CAPACITOR_PIN A5
#define VOLTAGE_REGULATOR_OUTPUT_SENSOR A6




GloriaBasePowerManager::GloriaBasePowerManager(LCDDisplay& l, SecretManager& s, DataStorageManager& sd, TimeManager& t,HardwareSerial& serial ):PowerManager(l,  s,  sd,  t, serial ), lcd(l),secretManager(s), dataStorageManager(sd),timeManager(t), _HardSerial(serial)
{}


uint8_t GloriaBasePowerManager::getEnergySourceType(){
	return GRID_ENERGY_SOURCE;
}

uint8_t GloriaBasePowerManager::getEnergyStorageType(){
	return BATTERY_ENERGY_STORAGE;
}


void GloriaBasePowerManager::start(){
	// pinMode(52, OUTPUT);
	// digitalWrite(52, LOW);
//	 SPI.begin();

	pinMode(BATTERY_VOLTAGE_PIN, INPUT);
	//pinMode(RS485_SOURCE_VOLTAGE_PIN, INPUT);

//	initializeWDT();
}


bool GloriaBasePowerManager::canPublish(){
	if(getEnergyStorageVoltage()>12.4){
		return true;
	}else{
		return false;
	}
}


void GloriaBasePowerManager::endOfLoopProcessing(){

	//	endOfLoopProcessing,
	//now=63099
	//currentFromBattery=99.00
	//loopStartingSeconds=1569113725
	//poweredDownInLoopSeconds=0
	//loopConsumingPowerSeconds=12286


	long now = timeManager.getCurrentTimeInSeconds();
	int loopConsumingPowerSeconds = now -loopStartingSeconds -poweredDownInLoopSeconds;
	float currentFromEnergyStorage = 0;//getCurrentFromEnergyStorage();
	 //float currentFromSolarPanel = getCurrentInputFromEnergySource();






	dailyEnergyStorageOutEnergy+= 0;//loopConsumingPowerSeconds*currentFromEnergyStorage/3600;
	hourlyEnergyStorageOutEnergy+= 0;//loopConsumingPowerSeconds*currentFromEnergyStorage/3600;
	//dailyEnergyStorageInEnergy+= loopConsumingPowerSeconds*currentFromSolarPanel/3600;
	//hourlyEnergyStorageInEnergy+= loopConsumingPowerSeconds*currentFromSolarPanel/3600;

	dailyPoweredDownInLoopSeconds+=poweredDownInLoopSeconds;
	hourlyPoweredDownInLoopSeconds+=poweredDownInLoopSeconds;

}

float GloriaBasePowerManager::getEnergyStorageVoltage(){
	return 13.4;
//	int NUM_SAMPLES=10;
//	int sample_count=0;
//	float sum=0;
//	while (sample_count < NUM_SAMPLES) {
//		sum += analogRead(BATTERY_VOLTAGE_PIN);
//		sample_count++;
//		delay(10);
//	}
//	float avg = (float)sum / (float)NUM_SAMPLES;
//	float voltage = 11*avg * 5.0 / 1024.0;
//
//
//	return voltage;
}


float GloriaBasePowerManager::getCurrentInputFromEnergySource(void){

	return -1.0;
}



float GloriaBasePowerManager::getEnergySourceVoltage(){

//#define volt_ref= 500
//#define volt_mult=   57
//#define adc_res  = 1024
//#define curr_ref_min 24900
//#define curr_ref_tip = 25000
//#define curr_ref_max 25100
//#define curr_mul_max  82
//#define curr_mul_tip  80
//#define curr_mul_min  78

		int volt_ref= 500;
		int volt_mult=   57;
		int adc_res  = 1024;
		int curr_ref_min =24900;
		int curr_ref_tip = 25000;
		int32_t prosc;
		int  curr_mul_tip = 80;
		prosc = analogRead(A2);
	   prosc *= volt_ref;
	  prosc *= 100;
	  prosc /= adc_res;
	  prosc -= curr_ref_tip;
	  prosc *= 100;
	  prosc /= curr_mul_tip;
	  return prosc;
}

//float GloriaBasePowerManager::getSolarPanelVoltage(){
//	long  sensorValue=analogRead(SOLAR_PANEL_VOLTAGE_PIN);
//	long  sum=0;
//	for(int i=0;i<10;i++)
//	{
//		sum=sensorValue+sum;
//		sensorValue=analogRead(SOLAR_PANEL_VOLTAGE_PIN);
//		delay(2);
//	}
//	sum=sum/10;
//	float value =(10*sum*4.980/1023.00);
//	return value;
//}

float GloriaBasePowerManager::getCurrentFromEnergyStorage(void){
	return 1;
}


void GloriaBasePowerManager::defineState(){
	this->poweredDownInLoopSeconds=0;
	long time = timeManager.getCurrentTimeInSeconds();

	float batteryVoltage = getEnergyStorageVoltage();
	int internalBatteryStateOfCharge = generalFunctions.getStateOfCharge(batteryVoltage);
	float currentFromBattery = getCurrentFromEnergyStorage();
	float inputFromSOlarPanel =  getCurrentInputFromEnergySource();
	//float solarPanelVolltage = getEnergySourceVoltage();



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

				dataStorageManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_END_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);

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
					anWPSSensorRecord.energyStorageVoltage= getEnergyStorageVoltage();
					anWPSSensorRecord.current = getCurrentFromEnergyStorage();
					anWPSSensorRecord.stateOfCharge = generalFunctions.getStateOfCharge(batteryVoltage);
					anWPSSensorRecord.lastWPSRecordSeconds=lastWPSRecordSeconds;
					anWPSSensorRecord.hourlyEnergyStorageOutEnergy=hourlyEnergyStorageOutEnergy;
					anWPSSensorRecord.dailyEnergyStorageOutEnergy=dailyEnergyStorageOutEnergy;
					anWPSSensorRecord.hourlyPoweredDownInLoopSeconds=hourlyPoweredDownInLoopSeconds;
					anWPSSensorRecord.dailyPoweredDownInLoopSeconds=dailyPoweredDownInLoopSeconds;
					anWPSSensorRecord.pauseDuringWPS=pauseDuringWPS;
					anWPSSensorRecord.operatingStatus=operatingStatus;
					anWPSSensorRecord.totalDiskUse=989; //dataStorageManager.getDiskUsage();

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
				lcd.print(batteryVoltage);
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

PowerStatusStruct GloriaBasePowerManager::getPowerStatusStruct(){
	float batteryVoltage =getEnergyStorageVoltage();
	//_HardSerial.print("line 318") ;
	float capacitorVoltage= getLockCapacitorVoltage();
	//_HardSerial.print("line 320") ;
	byte internalEnergyStorageStateOfCharge = GeneralFunctions::getStateOfCharge(batteryVoltage);
//	_HardSerial.print("line 322") ;
	//float regulatorVoltage = getVoltageRegulatorOutput();
	//long totalDiskUse=dataStorageManager.getDiskUsage()/1024;

	aPowerStatusStruct.sampleTime = timeManager.getCurrentTimeInSeconds();
	aPowerStatusStruct.energyStorageVoltage= batteryVoltage;
	//aPowerStatusStruct.energySourceVoltage= getEnergySourceVoltage();
	//aPowerStatusStruct.currentFromEnergySource= getCurrentInputFromEnergySource();
	//aPowerStatusStruct.currentFromEnergyStorage=getCurrentFromEnergyStorage();
	aPowerStatusStruct.lockCapacitorVoltage=capacitorVoltage;
	aPowerStatusStruct.internalEnergyStorageStateOfCharge=internalEnergyStorageStateOfCharge;
	//aPowerStatusStruct.regulatorVoltage=regulatorVoltage;
	aPowerStatusStruct.operatingStatus=operatingStatus;
	//_HardSerial.print("line 335") ;
	return aPowerStatusStruct;
}




void GloriaBasePowerManager::printPowerStatusStructToSerialPort(){
	PowerStatusStruct aPowerStatusStruct =  getPowerStatusStruct();
	_HardSerial.print(aPowerStatusStruct.sampleTime) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 2
	//
	char batteryVoltageStr[15];
	dtostrf(aPowerStatusStruct.energyStorageVoltage,4, 1, batteryVoltageStr);
	_HardSerial.print(batteryVoltageStr) ;
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
	_HardSerial.print(aPowerStatusStruct.internalEnergyStorageStateOfCharge);
	_HardSerial.print("#") ;



	_HardSerial.print(aPowerStatusStruct. operatingStatus);
	_HardSerial.print("#") ;
}


PowerStatisticsStruct GloriaBasePowerManager::getPowerStatisticsStruct(){
	// long totalDiskUse=dataStorageManager.getDiskUsage()/1024;
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

void GloriaBasePowerManager::printPowerStatisticsStructToSerialPort(){
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

void GloriaBasePowerManager::hourlyTasks(long time, int previousHour ){

	dataStorageManager.storeRememberedValue(time,HOURLY_ENERGY, hourlyEnergyStorageOutEnergy, UNIT_MILLI_AMPERES_HOURS);
	dataStorageManager.storeRememberedValue(time,HOURLY_POWERED_DOWN_IN_LOOP_SECONDS, hourlyPoweredDownInLoopSeconds, UNIT_SECONDS);
	dataStorageManager.storeRememberedValue(time,HOURLY_OPERATING_IN_LOOP_SECONDS, 3600-hourlyPoweredDownInLoopSeconds, UNIT_SECONDS);
	hourlyEnergyStorageOutEnergy=0;
	hourlyPoweredDownInLoopSeconds=0;
}

/*
 * this function is called at the beginning of a new day
 */
void GloriaBasePowerManager::dailyTasks(long time, int yesterdayDate, int yesterdayMonth, uint16_t yesterdayYear ){
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

void GloriaBasePowerManager::monthlyTasks(long time){

}

void GloriaBasePowerManager::yearlyTasks(long time){

}

GloriaBasePowerManager::~GloriaBasePowerManager() {
	// TODO Auto-generated destructor stub
}

