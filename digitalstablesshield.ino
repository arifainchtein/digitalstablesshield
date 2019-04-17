#include <GroveLCD.h>
#include <NoLCD.h>


#include <avr/wdt.h>
//#include <PowerManager.h>


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





//GeneralFunctions generalFunctions;
//TimeManager timeManager(generalFunctions, Serial);
//SecretManager secretManager(timeManager);
//SDCardManager sdCardManager(timeManager, generalFunctions, Serial, groveLCD );
//PowerManager aPowerManager(groveLCD , secretManager , sdCardManager , timeManager, generalFunctions, Serial);


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

	// uncomment
	//	aPowerManager.toggleWDT();



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

	//	//
	//	// Start The Managers
	//	//
	//	groveLCD.setCursor(0, 0);
	//	groveLCD.clear();
	//	groveLCD.print("Init Time Manager") ;
	//	timeManager.start();
	//
	//	groveLCD.clear();
	//	groveLCD.print("Init SDCard Manager") ;
	//	sdCardManager.start();
	//
	//	groveLCD.clear();
	//	groveLCD.print("Init Power Manager") ;
	//	aPowerManager.start();
	//
	//	long totalDiskUse=sdCardManager.getDiskUsage()/1024;

	/*
	 * Initialize the LCD Screen
	 */
	groveLCD.clear();
	groveLCD.setCursor(0, 0);
	groveLCD.print("Init Finished") ;
	groveLCD.setCursor(0, 1);
	//groveLCD.begin(totalDiskUse);
	//
	// end of initializing lcd
	//
	//delay(1000);
	//long now = timeManager.getCurrentTimeInSeconds();

	//sdCardManager.storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_SETUP_COMPLETED, LIFE_CYCLE_EVENT_COMMA_VALUE);
}


void loop() {

	wdt_reset();


	counter++;
	groveLCD.clear();
	groveLCD.setCursor(0, 0);
	groveLCD.print("counter ") ;
	groveLCD.print(counter) ;
	delay(1000)
	//
	// Generate the SensorData String


	//
	// now define the state its in
	//
	//aPowerManager.defineState(counter);
	//
	// the commands
	//
	//	if( Serial.available() != 0) {
	//		//lcd.clear();
	//
	//		String command = Serial.readString();
	//		//
	//		// end of teleonome specific sensors
	//		//
	//
	//
	//
	//		boolean commandProcessed = true;// aPowerManager.processDefaultCommands( command);
	//
	//		/*
	//		 * teleonome specific sensors
	//		 */
	//
	//		/*
	//		 * end f teleonome specific sensors
	//		 */
	//		if(!commandProcessed){
	//			//
	//			// add device specific
	//			if(command.startsWith("GetSensorData")){
	//
	//				groveLCD.clear();
	//
	//				groveLCD.setCursor(0, 0);
	//				groveLCD.print("b:");
	//				long b = millis();
	//				groveLCD.print(b);
	//				String sensorDataString= "";//aPowerManager.getBaseSensorString();
	//				int now = (int)(millis() - b);
	//				//
	//				// now add the teleonome specific sensors
	//				//
	//
	//				groveLCD.setCursor(0, 1);
	//				groveLCD.print("e:");
	//				groveLCD.print(now);
	//
	//
	//				Serial.println(sensorDataString);
	//				Serial.flush();
	//			}else{
	//				//
	//				// call read to flush the incoming
	//				//
	//				Serial.read();
	//				Serial.println("Failure-Bad Command " + command);
	//				Serial.flush();
	//			}
	//		}
	//
	// this is the end of the loop, to calculate the energy spent on this loop
	// take the time substract the time at the beginning of the loop (the now variable defined above)
	// and also substract the seconds spent in powerdownMode
	// finally add the poweredDownInLoopSeconds to the daily total

	//aPowerManager.endOfLoopProcessing();

	//}
}
