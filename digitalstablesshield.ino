
#include <Wire.h>
#include <Arduino.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <SPI.h>
#include <SD.h>
#include "rgb_lcd.h"

#include <RTCInfoRecord.h>
#include <WPSSensorRecord.h>

#include <TimeManager.h>
#include <GeneralFunctions.h>
#include <PowerManager.h>

#include <SecretManager.h>
#include <SDCardManager.h>
#include <GroveLCD.h>

#include <OneWire.h>

#include <totp.h>
#include "libraries/DigitalStables/GeneralConstants.h"

int SHARED_SECRET_LENGTH=27;
GroveLCD groveLCD;
GeneralFunctions generalFunctions;
TimeManager timeManager(generalFunctions, Serial);
SecretManager secretManager(timeManager);
SDCardManager sdCardManager(timeManager, generalFunctions, Serial);
PowerManager aPowerManager(groveLCD , secretManager , sdCardManager , timeManager, generalFunctions, Serial);

const char  *WPSSensorDataDirName="WPSSensr";
const char  *LifeCycleDataDirName="LifeCycl";
const char  *RememberedValueDataDirName  = "RememVal";
const char  *unstraferedFileName ="Untransf.txt";





/***************************************************
 *  Name:        ISR(WDT_vect)
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Watchdog Interrupt Service. This
 *               is executed when watchdog timed out.
 *
 ***************************************************/
ISR(WDT_vect){
	//lcd.setCursor(0, 1);
	//lcd.print("Waking Up") ;
	//wdt_reset();

	aPowerManager.toggleWDT();
//	if(f_wdt == 0)
//	{
//		f_wdt=1;
//	}
//	else
//	{
//		//_HardSerial.println("WDT Overrun!!!");
//	}
}

void setup() {
	Serial.begin(9600);

	pinMode(52, OUTPUT);
	digitalWrite(52, LOW);
	SPI.begin();


	timeManager.start();
	sdCardManager.start();
	long now = timeManager.getCurrentTimeInSeconds();
	sdCardManager.storeLifeCycleEvent(now, GeneralConstants::LIFE_CYCLE_EVENT_SETUP_COMPLETED, GeneralConstants::LIFE_CYCLE_EVENT_COMMA_VALUE);

	//Set the RTC time automatically: Calibrate RTC time by your computer time
	//rtc.adjustRtc(F(__DATE__), F(__TIME__));

	long now = timeManager.getCurrentTimeInSeconds();
	aPowerManager.turnPiOff(now);
	groveLCD.begin(16,2);
	delay(100); //Allow for serial print to complete.
	aPowerManager.initializeWDT();
	long totalDiskUse=sdCardManager.getDiskUsage();


	groveLCD.setCursor(0, 0);
	groveLCD.print("Finish Init") ;
	groveLCD.setCursor(0, 1);
	groveLCD.print("SD use ") ;
	groveLCD.print(totalDiskUse/1024) ;
	groveLCD.print("Kb") ;

	long now = timeManager.getCurrentTimeInSeconds();
	sdCardManager.storeLifeCycleEvent(now, GeneralConstants::LIFE_CYCLE_EVENT_SETUP_COMPLETED, GeneralConstants::LIFE_CYCLE_EVENT_COMMA_VALUE);

}

//	wdt_enable(WDTO_8S);





void loop() {

	wdt_reset();

	float batteryVoltage = aPowerManager.getBatteryVoltage();
	int internalBatteryStateOfCharge = generalFunctions.getStateOfCharge(batteryVoltage);
	float currentValue = aPowerManager.getCurrentValue();
	float capacitorVoltage= aPowerManager.getLockCapacitorVoltage();
	boolean piIsOn = digitalRead(aPowerManager.PI_POWER_PIN);



	//
	// Generate the SensorData String
	String sensorDataString="";
	//
	// Sensor Request Queue Position 1
	//
	char batteryVoltageStr[15];
	dtostrf(batteryVoltage,4, 1, batteryVoltageStr);
	sensorDataString.concat(batteryVoltageStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 2
	//
	char currentValueStr[15];
	dtostrf(currentValue,4, 0, currentValueStr);
	sensorDataString.concat(currentValueStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 3
	//
	char capacitorVoltageStr[15];
	dtostrf(capacitorVoltage,2, 1, capacitorVoltageStr);
	sensorDataString.concat(capacitorVoltageStr) ;
	sensorDataString.concat("#") ;


	//
	// Sensor Request Queue Position 4
	//
	sensorDataString.concat( internalBatteryStateOfCharge);
	sensorDataString.concat("#") ;
	//
	// Sensor Request Queue Position 5
	//

	sensorDataString.concat( aPowerManager.operatingStatus);
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 6
	//

	char dailyMinBatteryVoltageStr[15];
	dtostrf(aPowerManager.dailyMinBatteryVoltage,4, 0, dailyMinBatteryVoltageStr);
	sensorDataString.concat(dailyMinBatteryVoltageStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 7
	//

	char dailyMaxBatteryVoltageStr[15];
	dtostrf(aPowerManager.dailyMaxBatteryVoltage,4, 0, dailyMaxBatteryVoltageStr);
	sensorDataString.concat(dailyMaxBatteryVoltageStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 8
	//

	char dailyMinBatteryCurrentStr[15];
	dtostrf(aPowerManager.dailyMinBatteryCurrent,4, 0, dailyMinBatteryCurrentStr);
	sensorDataString.concat(dailyMinBatteryCurrentStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 9
	//

	char dailyMaxBatteryCurrentStr[15];
	dtostrf(aPowerManager.dailyMaxBatteryCurrent,4, 0, dailyMaxBatteryCurrentStr);
	sensorDataString.concat(dailyMaxBatteryCurrentStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 10
	//

	char dailyBatteryOutEnergyStr[15];
	dtostrf(aPowerManager.dailyBatteryOutEnergy,4, 0, dailyBatteryOutEnergyStr);
	sensorDataString.concat(dailyBatteryOutEnergyStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 11
	//

	char dailyPoweredDownInLoopSecondsStr[15];
	dtostrf(aPowerManager.dailyPoweredDownInLoopSeconds,4, 0, dailyPoweredDownInLoopSecondsStr);
	sensorDataString.concat(dailyPoweredDownInLoopSecondsStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 12
	//

	char hourlyBatteryOutEnergyStr[15];
	dtostrf(aPowerManager.hourlyBatteryOutEnergy,4, 0, hourlyBatteryOutEnergyStr);
	sensorDataString.concat(hourlyBatteryOutEnergyStr) ;
	sensorDataString.concat("#") ;
	//
	// Sensor Request Queue Position 13
	//

	char hourlyPoweredDownInLoopSecondsStr[15];
	dtostrf(aPowerManager.hourlyPoweredDownInLoopSeconds,4, 0, hourlyPoweredDownInLoopSecondsStr);
	sensorDataString.concat(hourlyPoweredDownInLoopSecondsStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 14
	//

	long totalDiskUse=sdCardManager.getDiskUsage();
	sensorDataString.concat(totalDiskUse/1024);
	sensorDataString.concat("#");
	//
	// Sensor Request Queue Position 15
	//

	sensorDataString.concat(aPowerManager.pauseDuringWPS);
	sensorDataString.concat("#");


	//lcd.clear();
	//lcd.setCursor(0, 0);

	long now = timeManager.getCurrentTimeInSeconds();
	aPowerManager.poweredDownInLoopSeconds=0;
	aPowerManager.defineState(now,  batteryVoltage, internalBatteryStateOfCharge, currentValue, piIsOn);
	//
	// the commands
	//
	if( Serial.available() != 0) {
		//lcd.clear();
		String command = Serial.readString();
		groveLCD.setCursor(0, 0);
		groveLCD.print(command);
		boolean commandProcessed = aPowerManager.processDefaultCommands( command, sensorDataString);
		if(!commandProcessed){
			if(){
			}else{
				//
				// call read to flush the incoming
				//
				Serial.read();
				Serial.println("Failure-Bad Command " + command);
				Serial.flush();
			}
		}
		//
		// this is the end of the loop, to calculate the energy spent on this loop
		// take the time substract the time at the beginning of the loop (the now variable defined above)
		// and also substract the seconds spent in powerdownMode
		// finally add the poweredDownInLoopSeconds to the daily total

		aPowerManager.endOfLoopProcessing();

	}
}
