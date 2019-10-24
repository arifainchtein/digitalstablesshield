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


float SolarPowerManager::getBatteryVoltage(){
	return 13.4;
	int NUM_SAMPLES=10;
  int sample_count=0;
  float sum=0;
  while (sample_count < NUM_SAMPLES) {
        sum += analogRead(BATTERY_VOLTAGE_PIN);
        sample_count++;
        delay(10);
    }
    float avg = (float)sum / (float)NUM_SAMPLES;
    float voltage = 11*avg * 5.0 / 1024.0;


    return voltage;
}

float SolarPowerManager::getCurrentInputFromSolarPanel(void){

	return 150;

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
	return 19.5;

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
	return 99;
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

PowerStatusStruct SolarPowerManager::getPowerStatusStruct(){
	float batteryVoltage = getBatteryVoltage();

	float capacitorVoltage= 0.0;//getLockCapacitorVoltage();
	byte internalBatteryStateOfCharge = GeneralFunctions::getStateOfCharge(batteryVoltage);
	//float regulatorVoltage = getVoltageRegulatorOutput();
	//long totalDiskUse=dataStorageManager.getDiskUsage()/1024;

	aPowerStatusStruct.sampleTime = timeManager.getCurrentTimeInSeconds();
	aPowerStatusStruct.batteryVoltage= batteryVoltage;
	aPowerStatusStruct.solarPanelVoltage= getSolarPanelVoltage();
	aPowerStatusStruct.currentFromSolarPanel= getCurrentInputFromSolarPanel();
	aPowerStatusStruct.currentFromBattery=getCurrentFromBattery();
	aPowerStatusStruct.capacitorVoltage=capacitorVoltage;
	aPowerStatusStruct.internalBatteryStateOfCharge=internalBatteryStateOfCharge;
	//aPowerStatusStruct.regulatorVoltage=regulatorVoltage;
	aPowerStatusStruct.operatingStatus=operatingStatus;

	return aPowerStatusStruct;
}

PowerStatisticsStruct SolarPowerManager::getPowerStatisticsStruct(){
//	long totalDiskUse=dataStorageManager.getDiskUsage()/1024;

	aPowerStatisticsStruct.sampleTime = timeManager.getCurrentTimeInSeconds();
	aPowerStatisticsStruct.dailyMinBatteryVoltage=dailyMinBatteryVoltage;
	aPowerStatisticsStruct.dailyMaxBatteryVoltage=dailyMaxBatteryVoltage;
	aPowerStatisticsStruct.dailyMinBatteryCurrent=dailyMinBatteryCurrent;
	aPowerStatisticsStruct.dailyMaxBatteryCurrent=dailyMaxBatteryCurrent;
	aPowerStatisticsStruct.dailyBatteryOutEnergy=dailyBatteryOutEnergy;
	aPowerStatisticsStruct.dailyPoweredDownInLoopSeconds=dailyPoweredDownInLoopSeconds;
	aPowerStatisticsStruct.hourlyBatteryOutEnergy=hourlyBatteryOutEnergy;
	aPowerStatisticsStruct.hourlyPoweredDownInLoopSeconds=hourlyPoweredDownInLoopSeconds;

	//	aPowerStatusStruct.totalDiskUse=totalDiskUse;

	return aPowerStatisticsStruct;
}


void SolarPowerManager::printPowerStatusStructToSerialPort(){
	PowerStatusStruct aPowerStatusStruct =  getPowerStatusStruct();
	this->_HardSerial.print(aPowerStatusStruct.sampleTime) ;
	this->  _HardSerial.print("#") ;
	  //
	  // Sensor Request Queue Position 1
	  //
	  char solarPanelVoltageStr[15];
	  dtostrf(aPowerStatusStruct.solarPanelVoltage,4, 1, solarPanelVoltageStr);
	  _HardSerial.print(solarPanelVoltageStr) ;
	  _HardSerial.print("#") ;

	  //
	  // Sensor Request Queue Position 2
	  //
	  char batteryVoltageStr[15];
	  dtostrf(aPowerStatusStruct.batteryVoltage,4, 1, batteryVoltageStr);
	  _HardSerial.print(batteryVoltageStr) ;
	  _HardSerial.print("#") ;

	  //
	  // Sensor Request Queue Position 3
	  //
	  char solarCurrentValueStr[15];
	  dtostrf(aPowerStatusStruct.currentFromSolarPanel,4, 0, solarCurrentValueStr);
	  _HardSerial.print(solarCurrentValueStr) ;
	  _HardSerial.print("#") ;

	  //
	  // Sensor Request Queue Position 4
	  //
	  char batteryOutputCurrentValueStr[15];
	  dtostrf(aPowerStatusStruct.currentFromBattery,4, 0, batteryOutputCurrentValueStr);
	  _HardSerial.print(solarCurrentValueStr) ;
	  _HardSerial.print("#") ;


	  //
	  // Sensor Request Queue Position 5
	  //
	  char capacitorVoltageStr[15];
	  dtostrf(aPowerStatusStruct.capacitorVoltage,2, 1, capacitorVoltageStr);
	  _HardSerial.print(capacitorVoltageStr) ;
	  _HardSerial.print("#") ;


	  //
	  // Sensor Request Queue Position 6
	  //
	  _HardSerial.print( aPowerStatusStruct.internalBatteryStateOfCharge);
	  _HardSerial.print("#") ;



	  _HardSerial.print(aPowerStatusStruct. operatingStatus);
	  _HardSerial.print("#") ;
}

//void SolarPowerManager::printPowerStatusStructToSerialPort(){
//	_HardSerial.print("point 0") ;
//	float batteryVoltage = getBatteryVoltage();
//	_HardSerial.print("point 1, batteryVoltage") ;
//	float solarPanelVoltage = getSolarPanelVoltage();
//	_HardSerial.print("point 1, solarVoltage");
//	byte internalBatteryStateOfCharge = GeneralFunctions::getStateOfCharge(batteryVoltage);
//	_HardSerial.print("point 1, internalBatteryStateOfCharge");
//	int solarPanelCurrentValue = getCurrentInputFromSolarPanel();
//	_HardSerial.print("point 1, solarPanelCurrentValue");
//	int batteryOutputCurrentValue = getCurrentFromBattery();
//	_HardSerial.print("point 1, batteryOutputCurrentValue");
//	float capacitorVoltage= 0;//getLockCapacitorVoltage();
//	_HardSerial.print("point 1") ;
//	//
//	// Sensor Request Queue Position 1
//	//
//	char solarPanelVoltageStr[15];
//	dtostrf(solarPanelVoltage,4, 1, solarPanelVoltageStr);
//	_HardSerial.print(solarPanelVoltageStr) ;
//	_HardSerial.print("#") ;
//
//	//
//	// Sensor Request Queue Position 2
//	//
//	char batteryVoltageStr[15];
//	dtostrf(batteryVoltage,4, 1, batteryVoltageStr);
//	_HardSerial.print(batteryVoltageStr) ;
//	_HardSerial.print("#") ;
//
//	//
//	// Sensor Request Queue Position 3
//	//
//	char solarCurrentValueStr[15];
//	dtostrf(solarPanelCurrentValue,4, 0, solarCurrentValueStr);
//	_HardSerial.print(solarCurrentValueStr) ;
//	_HardSerial.print("#") ;
//
//	//
//	// Sensor Request Queue Position 4
//	//
//	char batteryOutputCurrentValueStr[15];
//	dtostrf(batteryOutputCurrentValue,4, 0, batteryOutputCurrentValueStr);
//	_HardSerial.print(solarCurrentValueStr) ;
//	_HardSerial.print("#") ;
//
//
//	//
//	// Sensor Request Queue Position 5
//	//
//	char capacitorVoltageStr[15];
//	dtostrf(capacitorVoltage,2, 1, capacitorVoltageStr);
//	_HardSerial.print(capacitorVoltageStr) ;
//	_HardSerial.print("#") ;
//
//
//	//
//	// Sensor Request Queue Position 6
//	//
//	_HardSerial.print( internalBatteryStateOfCharge);
//	_HardSerial.print("#") ;
//
//
//
//	_HardSerial.print( operatingStatus);
//	_HardSerial.print("#") ;
//
//
//}

void SolarPowerManager::printPowerStatisticsStructToSerialPort(){
	//
	// Sensor Request Queue Position 1
	//

	char dailyMinBatteryVoltageStr[15];
	dtostrf(dailyMinBatteryVoltage,4, 0, dailyMinBatteryVoltageStr);
	_HardSerial.print(dailyMinBatteryVoltageStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 2
	//

	char dailyMaxBatteryVoltageStr[15];
	dtostrf(dailyMaxBatteryVoltage,4, 0, dailyMaxBatteryVoltageStr);
	_HardSerial.print(dailyMaxBatteryVoltageStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 3
	//

	char dailyMinBatteryCurrentStr[15];
	dtostrf(dailyMinBatteryCurrent,4, 0, dailyMinBatteryCurrentStr);
	_HardSerial.print(dailyMinBatteryCurrentStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 4
	//

	char dailyMaxBatteryCurrentStr[15];
	dtostrf(dailyMaxBatteryCurrent,4, 0, dailyMaxBatteryCurrentStr);
	_HardSerial.print(dailyMaxBatteryCurrentStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 5
	//

	char dailyBatteryOutEnergyStr[15];
	dtostrf(dailyBatteryOutEnergy,4, 0, dailyBatteryOutEnergyStr);
	_HardSerial.print(dailyBatteryOutEnergyStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 6
	//

	char dailyPoweredDownInLoopSecondsStr[15];
	dtostrf(dailyPoweredDownInLoopSeconds,4, 0, dailyPoweredDownInLoopSecondsStr);
	_HardSerial.print(dailyPoweredDownInLoopSecondsStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 7
	//

	char hourlyBatteryOutEnergyStr[15];
	dtostrf(hourlyBatteryOutEnergy,4, 0, hourlyBatteryOutEnergyStr);
	_HardSerial.print(hourlyBatteryOutEnergyStr) ;
	_HardSerial.print("#") ;
	//
	// Sensor Request Queue Position 8
	//

	char hourlyPoweredDownInLoopSecondsStr[15];
	dtostrf(hourlyPoweredDownInLoopSeconds,4, 0, hourlyPoweredDownInLoopSecondsStr);
	_HardSerial.print(hourlyPoweredDownInLoopSecondsStr) ;
	_HardSerial.print("#") ;


}

SolarPowerManager::~SolarPowerManager() {
	// TODO Auto-generated destructor stub
}

