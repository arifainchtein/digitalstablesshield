#include <GroveLCD.h>
#include <NoLCD.h>


#include <avr/wdt.h>
#include <PowerManager.h>


/**
 * teleonome speciifc libraries
 */


/**
 * teleonome speciifc variables
 */
GroveLCD groveLCD;
NoLCD noLCD;

static const char *LIFE_CYCLE_EVENT_SETUP_COMPLETED="Setup Method Completed";
static const int LIFE_CYCLE_EVENT_COMMA_VALUE=1;





GeneralFunctions generalFunctions;
TimeManager timeManager(generalFunctions, Serial);
SecretManager secretManager(timeManager);
SDCardManager sdCardManager(timeManager, generalFunctions, Serial, groveLCD );
PowerManager aPowerManager(groveLCD , secretManager , sdCardManager , timeManager, generalFunctions, Serial);


int counter;







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

	/*
	 * Initialize the LCD Screen
	 */
	groveLCD.begin(totalDiskUse);
	//
	// end of initializing lcd
	//
	delay(1000);
	long now = timeManager.getCurrentTimeInSeconds();

	sdCardManager.storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_SETUP_COMPLETED, LIFE_CYCLE_EVENT_COMMA_VALUE);
}


void loop() {

	wdt_reset();


counter++;

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
	aPowerManager.defineState(counter);
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
		/*
		 * teleonome specific sensors
		 */

		/*
		 * end f teleonome specific sensors
		 */
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
