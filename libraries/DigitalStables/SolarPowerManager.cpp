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

#define SOLAR_PANEL_VOLTAGE_PIN A0
#define BATTERY_VOLTAGE_PIN A1
#define SOLAR_PANEL_INPUT_CURRENT_SENSOR A2
#define BATTERY_OUPUT_CURRENT_SENSOR A3
#define RS485_SOURCE_VOLTAGE_PIN A4
#define LOCK_CAPACITOR_PIN A5
float amplitude_current;               //amplitude current
float effective_value;



SolarPowerManager::SolarPowerManager(LCDDisplay& l, SecretManager& s, DataStorageManager& sd, TimeManager& t,HardwareSerial& serial ):PowerManager(l,  s,  sd,  t, serial ), lcd(l),secretManager(s), dataStorageManager(sd),timeManager(t)
{}

void SolarPowerManager::endOfLoopProcessing(){

	//	endOfLoopProcessing,
	//now=63099
	//currentFromBattery=99.00
	//loopStartingSeconds=1569113725
	//poweredDownInLoopSeconds=0
	//loopConsumingPowerSeconds=12286


	long now = timeManager.getCurrentTimeInSeconds();
	int loopConsumingPowerSeconds = now -loopStartingSeconds -poweredDownInLoopSeconds;
	float currentFromBattery = getCurrentFromBattery();
	 float currentFromSolarPanel = getCurrentInputFromSolarPanel();






	dailyBatteryOutEnergy+= loopConsumingPowerSeconds*currentFromBattery/3600;
	hourlyBatteryOutEnergy+= loopConsumingPowerSeconds*currentFromBattery/3600;
	dailyBatteryInEnergy+= loopConsumingPowerSeconds*currentFromSolarPanel/3600;
	hourlyBatteryInEnergy+= loopConsumingPowerSeconds*currentFromSolarPanel/3600;
	dailyPoweredDownInLoopSeconds+=poweredDownInLoopSeconds;
	hourlyPoweredDownInLoopSeconds+=poweredDownInLoopSeconds;
	_HardSerial.print("dailyBatteryOutEnergy=");
	_HardSerial.print(dailyBatteryOutEnergy);

	_HardSerial.print("dailyBatteryInEnergy=");
	_HardSerial.print(dailyBatteryInEnergy);


	_HardSerial.print(" poweredDownInLoopSeconds=");
	_HardSerial.print(poweredDownInLoopSeconds);
	_HardSerial.print(" loopConsumingPowerSeconds=");
	_HardSerial.println(loopConsumingPowerSeconds);

	_HardSerial.print(" endOfLoopProcessing, now=");
	_HardSerial.print(now);
	_HardSerial.print(" currentFromBattery=");
	_HardSerial.print(currentFromBattery);

	_HardSerial.print(" loopStartingSeconds=");
	_HardSerial.print(loopStartingSeconds);

	_HardSerial.print(" poweredDownInLoopSeconds=");
	_HardSerial.print(poweredDownInLoopSeconds);
	_HardSerial.print(" loopConsumingPowerSeconds=");
	_HardSerial.println(loopConsumingPowerSeconds);
}

void SolarPowerManager::defineState(){
	poweredDownInLoopSeconds=0;
	loopStartingSeconds = timeManager.getCurrentTimeInSeconds();

	float batteryVoltage = getBatteryVoltage();
	//_HardSerial.print("staring define state, batteryVoltage =");
	//_HardSerial.println(batteryVoltage);

	float regulatorVoltage = getVoltageRegulatorOutput();
	int internalBatteryStateOfCharge = GeneralFunctions::getStateOfCharge(batteryVoltage);
	float currentFromBattery = getCurrentFromBattery();
	float inputFromSOlarPanel =  getCurrentInputFromSolarPanel();
	float solarPanelVolltage = getSolarPanelVoltage();
	float lockCapacitor = getLockCapacitorVoltage();



	if(batteryVoltage<this->dailyMinBatteryVoltage)this->dailyMinBatteryVoltage = batteryVoltage;
	if(batteryVoltage>this->dailyMaxBatteryVoltage)this->dailyMaxBatteryVoltage = batteryVoltage;
	if(currentFromBattery<this->dailyMinBatteryCurrent)this->dailyMinBatteryCurrent = currentFromBattery;
	if(currentFromBattery>this->dailyMaxBatteryCurrent)this->dailyMaxBatteryCurrent = currentFromBattery;


	hypothalamusStatus = digitalRead(PI_POWER_PIN);

	if(shuttingDownPiCountdown){
		currentSecondsToPowerOff = secondsToTurnPowerOff -( loopStartingSeconds - shutDownRequestedseconds );
		lcd.clear();
		lcd.setCursor(0,0);
		String s = "Shutting down";
		lcd.print(s);
		lcd.setCursor(0,1);

		if(currentSecondsToPowerOff<=0){
			shuttingDownPiCountdown=false;
			manualShutdown=true;
			inPulse=false;
			turnPiOff(loopStartingSeconds);
			dataStorageManager.storeLifeCycleEvent(loopStartingSeconds, LIFE_CYCLE_MANUAL_SHUTDOWN, LIFE_CYCLE_EVENT_COMMA_VALUE);
			lcd.print("Pi is OFF");
			currentViewIndex=3;
		}else{
			lcd.print("in ");
			lcd.print(	currentSecondsToPowerOff);
		}
	}else if(batteryVoltage>exitWPSVoltage){
		if(!hypothalamusStatus && !manualShutdown)turnPiOn(loopStartingSeconds);
		operatingStatus=3;
		lcd.setRGB(0, 225, 0);
		operatingStatus=3;
		wpsCountdown=false;
		wpsSleeping=false;
		inWPS=false;
		waitingForWPSConfirmation=false;

		if(inPulse){
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("Executing Pulse" );
			lcd.setCursor(0, 1);
			lcd.print( "Started at " );
			lcd.print(  pulseStartTime );
		}else{
			//
			// if we are here it means
			// that we are not in pulse and not in wps
			// so display user data according to the value of
			// currentViewIndex
			// currentViewIndex = 0 means show main data
			// currentViewIndex = 1 means Generate Password
			// currentViewIndex = 2 show Network info
			// currentViewIndex = 3 means shutdown request
			// currentViewIndex = 4 shutdown in process
			// i
			switch(currentViewIndex){
			case 0:
				//
				// this is the most
				// common state so as to
				//avoid flickering, only refresh once per second
				if((millis()-previousUpdate) >1000){
					previousUpdate = millis();
					lcd.clear();
					lcd.setCursor(0,0);
					lcd.print(batteryVoltage) ;
					lcd.print("V ") ;


					if(lockCapacitor>4.85){
						lcd.print("R");
					}else{
						lcd.print("P");
					}
					lcd.print(lockCapacitor);
					lcd.print(" ") ;
					lcd.print((int)currentFromBattery);
					lcd.print("mA ") ;
					lcd.setCursor(0, 1);
					lcd.print(timeManager.getCurrentDateTimeForDisplay());
					lcd.print(" " );
					lcd.print(regulatorVoltage );
					lcd.print("V" );

					lcd.setCursor(0, 2);
					long freeDiskSpace=dataStorageManager.getFreeDiskSpace()/1024;
					long totalDiskUse=dataStorageManager.getDiskUsage()/1024;
					lcd.print(totalDiskUse) ;
					lcd.print("kb F:") ;
					lcd.print(freeDiskSpace) ;
					lcd.print("kb H:") ;
					if(hypothalamusStatus){
						lcd.print("1 ");
					}else{
						lcd.print("0 ");
					}


				}

				break;

			case 1:
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("Create Password");
				lcd.setCursor(0, 1);
				lcd.print(" ");
				break;
			case 2:
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print(currentSSID);
				lcd.setCursor(0, 1);
				lcd.print(currentIpAddress);
				break;
			case 3:
				lcd.clear();
				lcd.setCursor(0, 0);
				if(manualShutdown){
					lcd.print("Pi is Off");
					lcd.setCursor(0, 1);
					lcd.print("Turn On Pi?");
				}else if(waitingManualPiStart){
					lcd.print("Waiting for Pi" );
					lcd.setCursor(0, 1);
					lcd.print("To Start" );

				}else{
					lcd.print("Turn Off Pi");
					lcd.setCursor(0, 1);
					lcd.print("Are You Sure?");
				}

				break;

			case 30:
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("Shutting Down Pi" );
				lcd.setCursor(0, 1);
				lcd.print(" " );
				break;


			case 35:
				// the pi was just turned on
				// manually
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("Waiting for Pi" );
				lcd.setCursor(0, 1);
				lcd.print("To Start" );
				currentViewIndex=3;
				waitingManualPiStart=true;
				break;
			}
		}
	}else if(batteryVoltage>enterWPSVoltage && batteryVoltage<=exitWPSVoltage){
		if(wpsSleeping){
			//delay(1000);
			//lcd.noDisplay();
			long piSleepingRemaining = secondsToNextPiOn-(loopStartingSeconds - currentSleepStartTime) ;
			lcd.clear();
			lcd.display();
			lcd.setCursor(0,0);
			lcd.setRGB(255,255,0);

			if(piSleepingRemaining<=0){
				wpsSleeping=false;
				if(!digitalRead(PI_POWER_PIN))turnPiOn(loopStartingSeconds);
				dataStorageManager.storeLifeCycleEvent(loopStartingSeconds, LIFE_CYCLE_EVENT_END_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);

				lcd.print("Pi ON WPS ");
				lcd.setCursor(0,1);
				lcd.print(batteryVoltage);
				lcd.print("V ");
				lcd.print(internalBatteryStateOfCharge);
				lcd.print("%") ;
				lastWPSStartUp = loopStartingSeconds;
			}else{
				//
				// if we are here is because we are in the
				// sleep period of the wps cycle.
				// check to see if we need to store a record in the sd card
				//
				long z =loopStartingSeconds-lastWPSRecordSeconds;
				lcd.print("wps rec in ");
				long netWPSRecordIn = (long)wpsPulseFrequencySeconds-z;

				lcd.print(netWPSRecordIn);
				lcd.setCursor(0,1);
				lcd.print("pi on in ");
				long piremaining = secondsToNextPiOn-(loopStartingSeconds - currentSleepStartTime) ;
				lcd.print(piremaining);


				//delay(1000);

				if(netWPSRecordIn<=0){
					lcd.clear();
					lcd.display();

					lastWPSRecordSeconds = timeManager.getCurrentTimeInSeconds();
					WPSSensorRecord anWPSSensorRecord;
					anWPSSensorRecord.batteryVoltage= getBatteryVoltage();
					anWPSSensorRecord.current = getCurrentFromBattery();
					anWPSSensorRecord.stateOfCharge = GeneralFunctions::getStateOfCharge(batteryVoltage);
					anWPSSensorRecord.lastWPSRecordSeconds=lastWPSRecordSeconds;
					anWPSSensorRecord.hourlyBatteryOutEnergy=hourlyBatteryOutEnergy;
					anWPSSensorRecord.dailyBatteryOutEnergy=dailyBatteryOutEnergy;
					anWPSSensorRecord.hourlyPoweredDownInLoopSeconds=hourlyPoweredDownInLoopSeconds;
					anWPSSensorRecord.dailyPoweredDownInLoopSeconds=dailyPoweredDownInLoopSeconds;
					anWPSSensorRecord.pauseDuringWPS=pauseDuringWPS;
					anWPSSensorRecord.operatingStatus=operatingStatus;
					anWPSSensorRecord.totalDiskUse= 0; dataStorageManager.getDiskUsage();


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
		}else if(hypothalamusStatus){
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("pi ON WPS ");
			lcd.print(batteryVoltage);
			lcd.print(" V");
			lcd.setCursor(0,1);
			lcd.print("Runtime ");
			long secsRunning = loopStartingSeconds-lastWPSStartUp;
			lcd.print(secsRunning);
		}
	}else if(batteryVoltage>minWPSVoltage && batteryVoltage<=enterWPSVoltage){
		if(!inWPS){
			faultData="Enter WPS";
			sendWPSAlert(loopStartingSeconds, faultData, batteryVoltage);
			lcd.clear();
			lcd.setRGB(225, 225, 0);
			lcd.setCursor(0, 0);
			lcd.print("WPS Alert Sent");

		}else{
			if(waitingForWPSConfirmation){
				delay(1000);
				long z = loopStartingSeconds-wpsAlertTime;
				long remaining = secondsToForcedWPS-z;
				lcd.clear();
				lcd.setRGB(225,225,0);
				lcd.setCursor(0,0);

				if( remaining <= 0  ){
					waitingForWPSConfirmation=false;
					operatingStatus=2;
					dataStorageManager.storeLifeCycleEvent(loopStartingSeconds, LIFE_CYCLE_EVENT_FORCED_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
					lcd.print("pi off");
					wpsSleeping=true;
					currentSleepStartTime = loopStartingSeconds;
					currentSecondsToPowerOff=0L;
					turnPiOff(loopStartingSeconds);
					wpsCountdown=false;
				}else{
					lcd.print("Waiting EnterWPS");
					lcd.setCursor(0,1);
					long remaining = secondsToForcedWPS-z;
					lcd.print(remaining);
					lcd.print("  ");
					lcd.print(batteryVoltage);
					lcd.print("V ");
				}
			}else if(wpsCountdown){
				currentSecondsToPowerOff = secondsToTurnPowerOff -( loopStartingSeconds - wpsCountDownStartSeconds);
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("wps countdown ");
				lcd.setCursor(0,1);
				lcd.print(	currentSecondsToPowerOff);
				if(currentSecondsToPowerOff<=0){
					currentSecondsToPowerOff=0;
					turnPiOff(loopStartingSeconds);
					dataStorageManager.storeLifeCycleEvent(loopStartingSeconds, LIFE_CYCLE_EVENT_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
					wpsSleeping=true;
					wpsCountdown=false;
					currentSleepStartTime=loopStartingSeconds;
				}
			}else if(wpsSleeping){
				//delay(1000);
				//lcd.noDisplay();
				long piSleepingRemaining = secondsToNextPiOn-(loopStartingSeconds - currentSleepStartTime) ;
				lcd.clear();
				lcd.display();
				lcd.setCursor(0,0);
				lcd.setRGB(255,255,0);

				if(piSleepingRemaining<=0){
					wpsSleeping=false;
					if(!digitalRead(PI_POWER_PIN))turnPiOn(loopStartingSeconds);
					dataStorageManager.storeLifeCycleEvent(loopStartingSeconds, LIFE_CYCLE_EVENT_END_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);

					lcd.print("Pi ON WPS ");
					lcd.setCursor(0,1);
					lcd.print(batteryVoltage);
					lcd.print("V ");
					lcd.print(internalBatteryStateOfCharge);
					lcd.print("%") ;
					lastWPSStartUp = loopStartingSeconds;
				}else{
					//
					// if we are here is because we are in the
					// sleep period of the wps cycle.
					// check to see if we need to store a record in the sd card
					//
					long z =loopStartingSeconds-lastWPSRecordSeconds;
					lcd.print("WPS rec in ");
					long netWPSRecordIn = (long)wpsPulseFrequencySeconds-z;

					lcd.print(netWPSRecordIn);
					lcd.setCursor(0,1);
					lcd.print("pi on in ");
					long piremaining = secondsToNextPiOn-(loopStartingSeconds - currentSleepStartTime) ;
					lcd.print(piremaining);


					//delay(1000);

					if(netWPSRecordIn<=0){
						lcd.clear();
						lcd.display();

						lastWPSRecordSeconds = timeManager.getCurrentTimeInSeconds();
						WPSSensorRecord anWPSSensorRecord;
						anWPSSensorRecord.batteryVoltage= getBatteryVoltage();
						anWPSSensorRecord.current = getCurrentFromBattery();
						anWPSSensorRecord.stateOfCharge = GeneralFunctions::getStateOfCharge(batteryVoltage);
						anWPSSensorRecord.lastWPSRecordSeconds=lastWPSRecordSeconds;
						anWPSSensorRecord.hourlyBatteryOutEnergy=hourlyBatteryOutEnergy;
						anWPSSensorRecord.dailyBatteryOutEnergy=dailyBatteryOutEnergy;
						anWPSSensorRecord.hourlyPoweredDownInLoopSeconds=hourlyPoweredDownInLoopSeconds;
						anWPSSensorRecord.dailyPoweredDownInLoopSeconds=dailyPoweredDownInLoopSeconds;
						anWPSSensorRecord.pauseDuringWPS=pauseDuringWPS;
						anWPSSensorRecord.operatingStatus=operatingStatus;
						anWPSSensorRecord.totalDiskUse=989; dataStorageManager.getDiskUsage();

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
			}else{
				if(hypothalamusStatus){
					lcd.clear();
					lcd.setCursor(0,0);
					lcd.print("pi ON WPS ");
					lcd.print(batteryVoltage);
					lcd.print(" V");
					lcd.setCursor(0,1);
					lcd.print("Runtime ");
					long secsRunning = loopStartingSeconds-lastWPSStartUp;
					lcd.print(secsRunning);
				}else{

				}

			}
		}

	}else if(batteryVoltage<minWPSVoltage ){
		if(!inWPS ){
			faultData="Enter WPS";
			sendWPSAlert(loopStartingSeconds, faultData, batteryVoltage);
			lcd.clear();
			lcd.setRGB(225, 0, 0);
			lcd.setCursor(0, 0);
			lcd.print("Comma Alert Sent");

		}else{
			if(waitingForWPSConfirmation){
				delay(1000);
				long z = loopStartingSeconds-wpsAlertTime;
				long remaining = secondsToForcedWPS-z;
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.setRGB(255,0,0);
				lcd.setCursor(0,0);
				if( remaining <= 0  ){
					waitingForWPSConfirmation=false;
					operatingStatus=2;
					dataStorageManager.storeLifeCycleEvent(loopStartingSeconds, LIFE_CYCLE_EVENT_FORCED_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
					wpsSleeping=false;
					currentSecondsToPowerOff=0L;
					if(hypothalamusStatus)turnPiOff(loopStartingSeconds);
					wpsCountdown=false;

					if(f_wdt == 1){
						/* Don't forget to clear the flag. */
						f_wdt = 0;
						/* Re-enter sleep mode. */
						lcd.print("Enter Comma");
						operatingStatus=1;
						lcd.setCursor(0,1);
						lcd.print(batteryVoltage);
						lcd.print(" V");
						delay(2000);
						lcd.setRGB(0,0,0);
						lcd.noDisplay();
						dataStorageManager.storeLifeCycleEvent(loopStartingSeconds,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
						enterArduinoSleep();
					}
				}else{
					lcd.print("Waiting EnterWPS");
					lcd.setCursor(0,1);
					long remaining = secondsToForcedWPS-z;
					lcd.print(remaining);
					lcd.print("  ");
					lcd.print(batteryVoltage);
					lcd.print("V ");
				}
			}else if(wpsCountdown){
				currentSecondsToPowerOff = secondsToTurnPowerOff -( loopStartingSeconds - wpsCountDownStartSeconds);
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("wps countdown ");
				lcd.setCursor(0,1);
				lcd.print(	currentSecondsToPowerOff);
				if(currentSecondsToPowerOff<=0){
					currentSecondsToPowerOff=0;
					if(hypothalamusStatus)turnPiOff(loopStartingSeconds);
					dataStorageManager.storeLifeCycleEvent(loopStartingSeconds, LIFE_CYCLE_EVENT_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
					wpsSleeping=false;
					wpsCountdown=false;
					if(f_wdt == 1){
						/* Don't forget to clear the flag. */
						f_wdt = 0;
						/* Re-enter sleep mode. */
						lcd.print("Enter Comma");
						operatingStatus=1;
						lcd.setCursor(0,1);
						lcd.print(batteryVoltage);
						lcd.print(" V");
						delay(2000);
						lcd.setRGB(0,0,0);
						lcd.noDisplay();
						dataStorageManager.storeLifeCycleEvent(loopStartingSeconds,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
						enterArduinoSleep();
					}
				}
			}else if(wpsSleeping){
				//
				// if the pi is asleep then go into a comma
				//
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
					dataStorageManager.storeLifeCycleEvent(loopStartingSeconds,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
					enterArduinoSleep();
				}
			}else if(hypothalamusStatus){
				//
				// i we are here it means the pi is n
				// and voltage has dropped into
				// comma range so
				faultData="Enter WPS";
				sendWPSAlert(loopStartingSeconds, faultData, batteryVoltage);
				lcd.clear();
				lcd.setRGB(225, 0, 0);
				lcd.setCursor(0, 0);
				lcd.print("Comma Alert Sent");
			}
		}
	}

	//_HardSerial.println("ending define state");
}

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




void SolarPowerManager::printPowerStatusStructToSerialPort(){
	PowerStatusStruct aPowerStatusStruct =  getPowerStatusStruct();
	_HardSerial.print(aPowerStatusStruct.sampleTime) ;
	_HardSerial.print("#") ;
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


PowerStatisticsStruct SolarPowerManager::getPowerStatisticsStruct(){
	//	long totalDiskUse=dataStorageManager.getDiskUsage()/1024;
	PowerStatisticsStruct powerStatisticsStruct;
	powerStatisticsStruct.sampleTime = timeManager.getCurrentTimeInSeconds();
	powerStatisticsStruct.dailyMinBatteryVoltage=this->dailyMinBatteryVoltage;
	powerStatisticsStruct.dailyMaxBatteryVoltage=this->dailyMaxBatteryVoltage;
	powerStatisticsStruct.dailyMinBatteryCurrent=this->dailyMinBatteryCurrent;
	powerStatisticsStruct.dailyMaxBatteryCurrent=this->dailyMaxBatteryCurrent;
	powerStatisticsStruct.dailyBatteryOutEnergy=this->dailyBatteryOutEnergy;
	powerStatisticsStruct.dailyPoweredDownInLoopSeconds=this->dailyPoweredDownInLoopSeconds;
	powerStatisticsStruct.hourlyBatteryOutEnergy=this->hourlyBatteryOutEnergy;
	powerStatisticsStruct.hourlyPoweredDownInLoopSeconds=this->hourlyPoweredDownInLoopSeconds;

	//	aPowerStatusStruct.totalDiskUse=totalDiskUse;

	return powerStatisticsStruct;
}

void SolarPowerManager::printPowerStatisticsStructToSerialPort(){
	//
	// Sensor Request Queue Position 1
	//
	PowerStatisticsStruct aPowerStatisticsStruct=getPowerStatisticsStruct();
	char dailyMinBatteryVoltageStr[15];
	dtostrf(aPowerStatisticsStruct.dailyMinBatteryVoltage,4, 0, dailyMinBatteryVoltageStr);
	this->_HardSerial.print(dailyMinBatteryVoltageStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 2
	//

	char dailyMaxBatteryVoltageStr[15];
	dtostrf(aPowerStatisticsStruct.dailyMaxBatteryVoltage,4, 0, dailyMaxBatteryVoltageStr);
	this->_HardSerial.print(dailyMaxBatteryVoltageStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 3
	//

	char dailyMinBatteryCurrentStr[15];
	dtostrf(aPowerStatisticsStruct.dailyMinBatteryCurrent,4, 0, dailyMinBatteryCurrentStr);
	this->_HardSerial.print(dailyMinBatteryCurrentStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 4
	//

	char dailyMaxBatteryCurrentStr[15];
	dtostrf(dailyMaxBatteryCurrent,4, 0, dailyMaxBatteryCurrentStr);
	this->_HardSerial.print(dailyMaxBatteryCurrentStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 5
	//

	char dailyBatteryOutEnergyStr[15];
	dtostrf(dailyBatteryOutEnergy,4, 2, dailyBatteryOutEnergyStr);
	this->_HardSerial.print(dailyBatteryOutEnergyStr) ;
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

	char hourlyBatteryOutEnergyStr[15];
	dtostrf(hourlyBatteryOutEnergy,4, 2, hourlyBatteryOutEnergyStr);
	this->_HardSerial.print(hourlyBatteryOutEnergyStr) ;
	this->_HardSerial.print("#") ;
	//
	// Sensor Request Queue Position 8
	//

	char hourlyPoweredDownInLoopSecondsStr[15];
	dtostrf(aPowerStatisticsStruct.hourlyPoweredDownInLoopSeconds,4, 0, hourlyPoweredDownInLoopSecondsStr);
	this->_HardSerial.print(hourlyPoweredDownInLoopSecondsStr) ;
	this->_HardSerial.print("#") ;


}

void SolarPowerManager::hourlyTasks(long time, int previousHour ){

	dataStorageManager.storeRememberedValue(time,HOURLY_ENERGY, hourlyBatteryOutEnergy, UNIT_MILLI_AMPERES_HOURS);
	dataStorageManager.storeRememberedValue(time,HOURLY_POWERED_DOWN_IN_LOOP_SECONDS, hourlyPoweredDownInLoopSeconds, UNIT_SECONDS);
	dataStorageManager.storeRememberedValue(time,HOURLY_OPERATING_IN_LOOP_SECONDS, 3600-hourlyPoweredDownInLoopSeconds, UNIT_SECONDS);
	hourlyBatteryOutEnergy=0;
	hourlyPoweredDownInLoopSeconds=0;
}

/*
 * this function is called at the beginning of a new day
 */
void SolarPowerManager::dailyTasks(long time, int yesterdayDate, int yesterdayMonth, uint16_t yesterdayYear ){
	//
	// move whatever is in untrasferred to the correct date
	boolean result = dataStorageManager.readUntransferredFileFromSDCardByDate( 1,false, RememberedValueDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear );
	result = dataStorageManager.readUntransferredFileFromSDCardByDate( 1,false, WPSSensorDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear);
	result = dataStorageManager.readUntransferredFileFromSDCardByDate( 1,false, LifeCycleDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear);
	long yesterdayDateSeconds = timeManager.dateAsSeconds(yesterdayYear,yesterdayMonth,yesterdayDate, 0, 0, 0);
	dataStorageManager.storeRememberedValue(time,DAILY_STATS_TIMESTAMP, yesterdayDateSeconds, UNIT_NO_UNIT);
	dataStorageManager.storeRememberedValue(time,DAILY_MINIMUM_BATTERY_VOLTAGE, dailyMinBatteryVoltage, UNIT_VOLT);
	dataStorageManager.storeRememberedValue(time,DAILY_MAXIMUM_BATTERY_VOLTAGE, dailyMaxBatteryVoltage, UNIT_VOLT);
	dataStorageManager.storeRememberedValue(time,DAILY_MINIMUM_BATTERY_CURRENT, dailyMinBatteryCurrent, UNIT_MILLI_AMPERES);
	dataStorageManager.storeRememberedValue(time,DAILY_MAXIMUM_BATTERY_CURRENT, dailyMaxBatteryCurrent, UNIT_MILLI_AMPERES);
	dataStorageManager.storeRememberedValue(time,DAILY_ENERGY, dailyBatteryOutEnergy, UNIT_MILLI_AMPERES_HOURS);
	dataStorageManager.storeRememberedValue(time,DAILY_POWERED_DOWN_IN_LOOP_SECONDS, dailyPoweredDownInLoopSeconds, UNIT_SECONDS);
	dailyMinBatteryVoltage = 9999.0;
	dailyMaxBatteryVoltage = -1.0;
	dailyMinBatteryCurrent = 9999.0;
	dailyMaxBatteryCurrent = -1.0;
	dailyBatteryOutEnergy=0.0;
	dailyPoweredDownInLoopSeconds=0.0;

}

void SolarPowerManager::monthlyTasks(long time){

}

void SolarPowerManager::yearlyTasks(long time){

}


SolarPowerManager::~SolarPowerManager() {
	// TODO Auto-generated destructor stub
}

