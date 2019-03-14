

#include <GeneralConstants.h>
#include <GeneralFunctions.h>
#include <GroveLCD.h>
#include <LCDDisplay.h>
#include <PowerManager.h>
#include <RTCInfoRecord.h>
#include <avr/wdt.h>

#include <SDCardManager.h>
#include <SecretManager.h>
#include <TimeManager.h>
#include <WPSSensorRecord.h>

#include <GeneralConstants.h>
#include <GeneralFunctions.h>
#include <GroveLCD.h>
#include <LCDDisplay.h>
#include <PowerManager.h>
#include <RTCInfoRecord.h>
#include <SDCardManager.h>
#include <SecretManager.h>
#include <TimeManager.h>
#include <WPSSensorRecord.h>



#include <Wire.h>
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
	//
	// Start the Serial Ports
	//
	Serial.begin(9600);
	Serial1.begin(9600);
	Serial2.begin(9600);
	Serial3.begin(9600);



	//
	// Start The Managers
	//
	timeManager.start();
	sdCardManager.start();
	aPowerManager.start();

	long totalDiskUse=sdCardManager.getDiskUsage()/1024;

	//
	// Initialize the LCD Screen
	//
	String line1="Finish Init";




	delay(2000);
	long now = timeManager.getCurrentTimeInSeconds();
	sdCardManager.storeLifeCycleEvent(now, GeneralConstants::LIFE_CYCLE_EVENT_SETUP_COMPLETED, GeneralConstants::LIFE_CYCLE_EVENT_COMMA_VALUE);
}


void loop() {

	wdt_reset();




	//
	// Generate the SensorData String
	String sensorDataString=aPowerManager.getBaseSensorString();
	//
	// now add the teleonome specific sensors
	//


	//
	// end of teleonome specific sensors
	//


	//
	// now define the state its in
	//
	aPowerManager.defineState();
	//
	// the commands
	//
	if( Serial.available() != 0) {
		//lcd.clear();
		String command = Serial.readString();
		groveLCD.setCursor(0, 0);
		groveLCD.print(command);
		boolean commandProcessed = aPowerManager.processDefaultCommands( command, sensorDataString);
		String sensorDataString = aPowerManager.getBaseSensorString();
		//
		// teleonome specific sensors
		//

		//
		// end of teleonome soecific sensor
		//
		if(!commandProcessed){
			//
			// add device specific
			if(command.startsWith("GetSensorData")){
				Serial.println(sensorDataString);
				Serial.flush();
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
