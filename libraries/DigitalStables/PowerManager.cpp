#include <Arduino.h>
#include <PowerManager.h>
#include <SDCardManager.h>
#include <GeneralFunctions.h>
#include <GeneralConstants.h>
#include <TimeManager.h>

#include <SecretManager.h>
#include <LCDDisplay.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <totp.h>
#include "GeneralConstants.h"


//
// the wps variables
#define LOCK_CAPACITOR_PIN A2
long secondsToTurnPowerOff = 30;
long secondsToNextPiOn=90L;
long currentSecondsToPowerOff=0L;
boolean wpsCountdown=false;
boolean wpsSleeping=false;
boolean inWPS=false;
float minWPSVoltage=12.4;
float enterWPSVoltage=12.6;
float exitWPSVoltage=12.8;

long wpsCountDownStartSeconds=0L;

char *faultData;
long secondsToForcedWPS=60L;
long wpsAlertTime=0L;
int wpsPulseFrequencySeconds=60;
long lastWPSRecordSeconds=0L;
boolean waitingForWPSConfirmation=false;

long currentSleepStartTime=0L;
long lastWPSStartUp=0L;

float capacitorVoltage= 0;


char remFileName[10];

char sensorDirName[10];
char lifeCycleFileName[10];
long shutDownRequestedseconds= 0L;
boolean shuttingDownPiCountdown=false;
boolean manualShutdown=false;
boolean waitingManualPiStart=false;

// the battery voltage
//
#define batteryVoltagePin A1
#define CURRENT_SENSOR A3
float amplitude_current;               //amplitude current
float effective_value;       //effective current
boolean powerSupplyOn=false;

//
// current view index
// Controls what the user sees in the lcdnow
// it starts with a value of 99 which means is locked


boolean showingAct=false;

int currentHour=0;
int currentDay=0;
int currentMonth=0;
int currentYear=0;


TimeManager & timeManager;
SecretManager & secretManager;

GeneralFunctions & generalFunctions;
SDCardManager & sdCardManager;
HardwareSerial& _HardSerial;
LCDDisplay & lcd;



//
// the virtual micrcntroller

String currentIpAddress="No IP";
String currentSSID="No SSID";
int delayTime=1;
volatile int f_wdt=1;


extern int SHARED_SECRET_LENGTH;

PowerManager::PowerManager(LCDDisplay& l, SecretManager& s, SDCardManager& sd, TimeManager& t, GeneralFunctions& f,HardwareSerial& serial ): lcd(l),secretManager(s), sdCardManager(sd),timeManager(t), generalFunctions(f), _HardSerial(serial)
{}

void PowerManager::start(){
	pinMode(PI_POWER_PIN, OUTPUT);
	digitalWrite(PI_POWER_PIN, LOW);
}
void PowerManager::hourlyTasks(long time, int previousHour ){

	sdCardManager.storeRememberedValue(time,GeneralConstants::HOURLY_ENERGY, hourlyBatteryOutEnergy, GeneralConstants::UNIT_MILLI_AMPERES_HOURS);
	sdCardManager.storeRememberedValue(time,GeneralConstants::HOURLY_POWERED_DOWN_IN_LOOP_SECONDS, hourlyPoweredDownInLoopSeconds, GeneralConstants::UNIT_SECONDS);
	sdCardManager.storeRememberedValue(time,GeneralConstants::HOURLY_OPERATING_IN_LOOP_SECONDS, 3600-hourlyPoweredDownInLoopSeconds, GeneralConstants::UNIT_SECONDS);
	hourlyBatteryOutEnergy=0;
	hourlyPoweredDownInLoopSeconds=0;
}

/*
 * this function is called at the beginning of a new day
 */
void PowerManager::dailyTasks(long time, int yesterdayDate, int yesterdayMonth, uint16_t yesterdayYear ){
	//
	// move whatever is in untrasferred to the correct date
	boolean result = sdCardManager.readUntransferredFileFromSDCardByDate( 1,false, RememberedValueDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear );
	result = sdCardManager.readUntransferredFileFromSDCardByDate( 1,false, WPSSensorDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear);
	result = sdCardManager.readUntransferredFileFromSDCardByDate( 1,false, LifeCycleDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear);
	long yesterdayDateSeconds = timeManager.dateAsSeconds(yesterdayYear,yesterdayMonth,yesterdayDate, 0, 0, 0);
	sdCardManager.storeRememberedValue(time,GeneralConstants::DAILY_STATS_TIMESTAMP, yesterdayDateSeconds, GeneralConstants::UNIT_NO_UNIT);
	sdCardManager.storeRememberedValue(time,GeneralConstants::DAILY_MINIMUM_BATTERY_VOLTAGE, dailyMinBatteryVoltage, GeneralConstants::UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,GeneralConstants::DAILY_MAXIMUM_BATTERY_VOLTAGE, dailyMaxBatteryVoltage, GeneralConstants::UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,GeneralConstants::DAILY_MINIMUM_BATTERY_CURRENT, dailyMinBatteryCurrent, GeneralConstants::UNIT_MILLI_AMPERES);
	sdCardManager.storeRememberedValue(time,GeneralConstants::DAILY_MAXIMUM_BATTERY_CURRENT, dailyMaxBatteryCurrent, GeneralConstants::GeneralConstants::UNIT_MILLI_AMPERES);
	sdCardManager.storeRememberedValue(time,GeneralConstants::DAILY_ENERGY, dailyBatteryOutEnergy, GeneralConstants::UNIT_MILLI_AMPERES_HOURS);
	sdCardManager.storeRememberedValue(time,GeneralConstants::DAILY_POWERED_DOWN_IN_LOOP_SECONDS, dailyPoweredDownInLoopSeconds, GeneralConstants::UNIT_SECONDS);
	dailyMinBatteryVoltage = 9999.0;
	dailyMaxBatteryVoltage = -1.0;
	dailyMinBatteryCurrent = 9999.0;
	dailyMaxBatteryCurrent = -1.0;
	dailyBatteryOutEnergy=0.0;
	dailyPoweredDownInLoopSeconds=0.0;

}

void PowerManager::monthlyTasks(long time){

}

void PowerManager::yearlyTasks(long time){

}



float PowerManager::getCurrentValue(void){
	int sensorValue;             //value read from the sensor
	int sensorMax = 0;
	uint32_t start_time = millis();
	while((millis()-start_time) < 100)//sample for 1000ms
	{
		sensorValue = analogRead(CURRENT_SENSOR);
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

float PowerManager::getBatteryVoltage(){
	long  sensorValue=analogRead(batteryVoltagePin);
	long  sum=0;
	for(int i=0;i<10;i++)
	{
		sum=sensorValue+sum;
		sensorValue=analogRead(batteryVoltagePin);
		delay(2);
	}
	sum=sum/10;
	float value =(10*sum*4.980/1023.00);
	return value;
}

void PowerManager::initializeWDT(){
	/*** Setup the WDT ***/

	/* Clear the reset flag. */
	MCUSR &= ~(1<<WDRF);

	/* In order to change WDE or the prescaler, we need to
	 * set WDCE (This will allow updates for 4 clock cycles).
	 */
	WDTCSR |= (1<<WDCE) | (1<<WDE);

	/* set new watchdog timeout prescaler value */
	WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */

	/* Enable the WD interrupt (note no reset). */
	WDTCSR |= _BV(WDIE);

}




/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Enters the arduino into sleep mode.
 *
 ***************************************************/
void PowerManager::enterArduinoSleep(void)
{
	digitalWrite(PI_POWER_PIN, LOW);



	wdt_reset();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
	//sleep_enable();
	long currentSleepSeconds = timeManager.getCurrentTimeInSeconds();
	/* Now enter sleep mode. */
	sleep_mode();

	/* The program will continue from here after the WDT timeout*/

	//
	// check the voltage of the battery, if its higher than
	// the min for wps then go into wps,
	// otherwise go back to comma
	//
	long lastSleepSeconds = timeManager.getCurrentTimeInSeconds()-currentSleepSeconds ;
	poweredDownInLoopSeconds+=lastSleepSeconds;
	float batteryVoltage = getBatteryVoltage();
	if(batteryVoltage>minWPSVoltage){
		// STORE a lifecycle comma exit record
		long now = timeManager.getCurrentTimeInSeconds();
		//sdCardManager.storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_END_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
		lcd.display();
		lcd.setRGB(255,255,0);
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Out of Comma");
		lcd.setCursor(0,1);
		lcd.print(batteryVoltage);
		lcd.print("V ");
		lcd.print(lastSleepSeconds);
		lcd.print("V ");

		operatingStatus="WPS";
		currentSleepStartTime = now;
		wpsSleeping=true;
		inWPS=true;
		sleep_disable(); /* First thing to do is disable sleep. */
		/* Re-enable the peripherals. */
		power_all_enable();
	}else{
		lcd.display();
		lcd.setRGB(255,0,0);
		lcd.clear();
		lcd.print(batteryVoltage);
		lcd.print("V");
		delay(500);
		lcd.noDisplay();
		lcd.setRGB(0,0,0);
		enterArduinoSleep();
	}

}

/***************************************************
 *  Name:        pauseWPS
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: This method is similar to enterSleep except that is called
 *               during the time where the voltage is whithin the wps range
 *               and is a way to save power
 *               it is different than the comma because it does not recursively
 *               call itself and does not write lifecycle events
 *
 ***************************************************/
void PowerManager::pauseWPS(void)
{
	digitalWrite(PI_POWER_PIN, LOW);
	lcd.noDisplay();
	lcd.setRGB(0,0,0);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
	sleep_enable();

	long currentSleepSeconds = timeManager.getCurrentTimeInSeconds();
	/* Now enter sleep mode. */
	sleep_mode();

	/* The program will continue from here after the WDT timeout*/

	//
	// check the voltage of the battery, if its higher than
	// the min for wps then go into wps,
	// otherwise go back to comma
	//
	long lastSleepSeconds = timeManager.getCurrentTimeInSeconds()-currentSleepSeconds ;
	poweredDownInLoopSeconds+=lastSleepSeconds;

	lcd.display();
	lcd.setRGB(255,255,0);
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Out of Pause");
	lcd.setCursor(0,1);
	float batteryVoltage = getBatteryVoltage();
	lcd.print(batteryVoltage);
	lcd.print("V ");
	lcd.print(pauseDuringWPS);

	operatingStatus="WPS";
	//lcd.setCursor(0, 1);
	//lcd.print("Awake") ;
	sleep_disable(); /* First thing to do is disable sleep. */
	/* Re-enable the peripherals. */
	power_all_enable();
}

void PowerManager::sendWPSAlert(long time, char *faultData, int batteryVoltage){
	waitingForWPSConfirmation=true;
	wpsCountdown=false;
	inWPS=true;
	operatingStatus="WPS";
	wpsAlertTime=timeManager.getCurrentTimeInSeconds();
	//sdCardManager.storeRememberedValue(time,faultData, batteryVoltage, UNIT_VOLT);
}

void PowerManager::turnPiOffForced(long time){
	float batteryVoltageBefore = getBatteryVoltage();
	digitalWrite(PI_POWER_PIN, LOW);
	delay(1000);
	float batteryVoltageAfter = getBatteryVoltage();
	float voltageDifferential = 1-(batteryVoltageBefore/batteryVoltageAfter);
	sdCardManager.storeRememberedValue(time,GeneralConstants::FORCED_PI_TURN_OFF,0 , operatingStatus);
	sdCardManager.storeRememberedValue(time,GeneralConstants::BATTERY_VOLTAGE_BEFORE_PI_ON, batteryVoltageBefore, GeneralConstants::UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,GeneralConstants::BATTERY_VOLTAGE_ATER_PI_ON, batteryVoltageBefore, GeneralConstants::UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,GeneralConstants::BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON, voltageDifferential, GeneralConstants::UNIT_PERCENTAGE);
}

void PowerManager::turnPiOff(long time){
	float batteryVoltageBefore = getBatteryVoltage();
	digitalWrite(PI_POWER_PIN, LOW);
	delay(1000);
	float batteryVoltageAfter = getBatteryVoltage();
	float voltageDifferential = 1-(batteryVoltageBefore/batteryVoltageAfter);
	sdCardManager.storeRememberedValue(time,GeneralConstants::PI_TURN_OFF,0 , operatingStatus);
	sdCardManager.storeRememberedValue(time,GeneralConstants::BATTERY_VOLTAGE_BEFORE_PI_ON, batteryVoltageBefore, GeneralConstants::UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,GeneralConstants::BATTERY_VOLTAGE_ATER_PI_ON, batteryVoltageBefore, GeneralConstants::UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,GeneralConstants::BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON, voltageDifferential, GeneralConstants::UNIT_PERCENTAGE);
}


void PowerManager::turnPiOn(long time){
	float batteryVoltageBefore = getBatteryVoltage();
	digitalWrite(PI_POWER_PIN, HIGH);
	delay(1000);
	float batteryVoltageAfter = getBatteryVoltage();
	float voltageDifferential = 1-(batteryVoltageAfter/batteryVoltageBefore);

	sdCardManager.storeRememberedValue(time,GeneralConstants::BATTERY_VOLTAGE_BEFORE_PI_ON, batteryVoltageBefore, GeneralConstants::UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,GeneralConstants::BATTERY_VOLTAGE_ATER_PI_ON, batteryVoltageBefore, GeneralConstants::UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,GeneralConstants::BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON, voltageDifferential, GeneralConstants::UNIT_PERCENTAGE);
}



void PowerManager::defineState(long time, float batteryVoltage,int internalBatteryStateOfCharge, float currentValue, boolean piIsOn){
	poweredDownInLoopSeconds=0;
	if(shuttingDownPiCountdown){
		currentSecondsToPowerOff = secondsToTurnPowerOff -( time - shutDownRequestedseconds );
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Shutting down");
		lcd.setCursor(0,1);

		if(currentSecondsToPowerOff<=0){
			shuttingDownPiCountdown=false;
			manualShutdown=true;
			inPulse=false;
			turnPiOff(time);
			sdCardManager.storeLifeCycleEvent(time, GeneralConstants::LIFE_CYCLE_MANUAL_SHUTDOWN, GeneralConstants::LIFE_CYCLE_EVENT_COMMA_VALUE);
			lcd.print("Pi is OFF");
			currentViewIndex=3;
		}else{
			lcd.print("in ");
			lcd.print(	currentSecondsToPowerOff);
		}
	}else if(batteryVoltage>exitWPSVoltage){
		if(!piIsOn && !manualShutdown)turnPiOn(time);
		operatingStatus="Normal";
		lcd.setRGB(0, 225, 0);
		operatingStatus="Normal";
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
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print((int)currentValue);
				lcd.print("mA ") ;
				lcd.print(batteryVoltage) ;
				lcd.print("V ") ;
				lcd.print(internalBatteryStateOfCharge);
				lcd.print("%") ;
				lcd.setCursor(0, 1);
				lcd.print(timeManager.getCurrentDateTimeForDisplay());
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
			long piSleepingRemaining = secondsToNextPiOn-(time - currentSleepStartTime) ;
			lcd.clear();
			lcd.display();
			lcd.setCursor(0,0);
			lcd.setRGB(255,255,0);

			if(piSleepingRemaining<=0){
				wpsSleeping=false;
				if(!digitalRead(PI_POWER_PIN))turnPiOn(time);
				//sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_END_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);

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
				lcd.print("wps rec in ");
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
					anWPSSensorRecord.current = getCurrentValue();
					anWPSSensorRecord.stateOfCharge = generalFunctions.getStateOfCharge(batteryVoltage);
					anWPSSensorRecord.lastWPSRecordSeconds=lastWPSRecordSeconds;
					anWPSSensorRecord.hourlyBatteryOutEnergy=hourlyBatteryOutEnergy;
					anWPSSensorRecord.dailyBatteryOutEnergy=dailyBatteryOutEnergy;
					anWPSSensorRecord.hourlyPoweredDownInLoopSeconds=hourlyPoweredDownInLoopSeconds;
					anWPSSensorRecord.dailyPoweredDownInLoopSeconds=dailyPoweredDownInLoopSeconds;
					anWPSSensorRecord.pauseDuringWPS=pauseDuringWPS;
					anWPSSensorRecord.operatingStatus=operatingStatus;
					anWPSSensorRecord.totalDiskUse= 0; //sdCardManager.getDiskUsage();


					//sdCardManager.saveWPSSensorRecord( anWPSSensorRecord);
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
		}else if(piIsOn){
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("pi ON WPS ");
			lcd.print(batteryVoltage);
			lcd.print(" V");
			lcd.setCursor(0,1);
			lcd.print("Runtime ");
			long secsRunning = time-lastWPSStartUp;
			lcd.print(secsRunning);
		}
	}else if(batteryVoltage>minWPSVoltage && batteryVoltage<=enterWPSVoltage){
		if(!inWPS){
			faultData="Enter WPS";
			sendWPSAlert(time, faultData, batteryVoltage);
			lcd.clear();
			lcd.setRGB(225, 225, 0);
			lcd.setCursor(0, 0);
			lcd.print("WPS Alert Sent");

		}else{
			if(waitingForWPSConfirmation){
				delay(1000);
				long z = time-wpsAlertTime;
				long remaining = secondsToForcedWPS-z;
				lcd.clear();
				lcd.setRGB(225,225,0);
				lcd.setCursor(0,0);

				if( remaining <= 0  ){
					waitingForWPSConfirmation=false;
					operatingStatus="WPS";
					//sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_FORCED_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
					lcd.print("pi off");
					wpsSleeping=true;
					currentSleepStartTime = time;
					currentSecondsToPowerOff=0L;
					turnPiOff(time);
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
				currentSecondsToPowerOff = secondsToTurnPowerOff -( time - wpsCountDownStartSeconds);
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("wps countdown ");
				lcd.setCursor(0,1);
				lcd.print(	currentSecondsToPowerOff);
				if(currentSecondsToPowerOff<=0){
					currentSecondsToPowerOff=0;
					turnPiOff(time);
					//sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
					wpsSleeping=true;
					wpsCountdown=false;
					currentSleepStartTime=time;
				}
			}else if(wpsSleeping){
				//delay(1000);
				//lcd.noDisplay();
				long piSleepingRemaining = secondsToNextPiOn-(time - currentSleepStartTime) ;
				lcd.clear();
				lcd.display();
				lcd.setCursor(0,0);
				lcd.setRGB(255,255,0);

				if(piSleepingRemaining<=0){
					wpsSleeping=false;
					if(!digitalRead(PI_POWER_PIN))turnPiOn(time);
					//sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_END_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);

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
						anWPSSensorRecord.current = getCurrentValue();
						anWPSSensorRecord.stateOfCharge = generalFunctions.getStateOfCharge(batteryVoltage);
						anWPSSensorRecord.lastWPSRecordSeconds=lastWPSRecordSeconds;
						anWPSSensorRecord.hourlyBatteryOutEnergy=hourlyBatteryOutEnergy;
						anWPSSensorRecord.dailyBatteryOutEnergy=dailyBatteryOutEnergy;
						anWPSSensorRecord.hourlyPoweredDownInLoopSeconds=hourlyPoweredDownInLoopSeconds;
						anWPSSensorRecord.dailyPoweredDownInLoopSeconds=dailyPoweredDownInLoopSeconds;
						anWPSSensorRecord.pauseDuringWPS=pauseDuringWPS;
						anWPSSensorRecord.operatingStatus=operatingStatus;
						anWPSSensorRecord.totalDiskUse=989; //sdCardManager.getDiskUsage();

						//sdCardManager.saveWPSSensorRecord( anWPSSensorRecord);
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
				if(piIsOn){
					lcd.clear();
					lcd.setCursor(0,0);
					lcd.print("pi ON WPS ");
					lcd.print(batteryVoltage);
					lcd.print(" V");
					lcd.setCursor(0,1);
					lcd.print("Runtime ");
					long secsRunning = time-lastWPSStartUp;
					lcd.print(secsRunning);
				}else{

				}

			}
		}

	}else if(batteryVoltage<minWPSVoltage ){
		if(!inWPS ){
			faultData="Enter WPS";
			sendWPSAlert(time, faultData, batteryVoltage);
			lcd.clear();
			lcd.setRGB(225, 0, 0);
			lcd.setCursor(0, 0);
			lcd.print("Comma Alert Sent");

		}else{
			if(waitingForWPSConfirmation){
				delay(1000);
				long z = time-wpsAlertTime;
				long remaining = secondsToForcedWPS-z;
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.setRGB(255,0,0);
				lcd.setCursor(0,0);
				if( remaining <= 0  ){
					waitingForWPSConfirmation=false;
					operatingStatus="WPS";
					//sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_FORCED_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
					wpsSleeping=false;
					currentSecondsToPowerOff=0L;
					if(piIsOn)turnPiOff(time);
					wpsCountdown=false;

					if(f_wdt == 1){
						/* Don't forget to clear the flag. */
						f_wdt = 0;
						/* Re-enter sleep mode. */
						lcd.print("Enter Comma");
						operatingStatus="Comma";
						lcd.setCursor(0,1);
						lcd.print(batteryVoltage);
						lcd.print(" V");
						delay(2000);
						lcd.setRGB(0,0,0);
						lcd.noDisplay();
						//sdCardManager.storeLifeCycleEvent(time,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
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
				currentSecondsToPowerOff = secondsToTurnPowerOff -( time - wpsCountDownStartSeconds);
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("wps countdown ");
				lcd.setCursor(0,1);
				lcd.print(	currentSecondsToPowerOff);
				if(currentSecondsToPowerOff<=0){
					currentSecondsToPowerOff=0;
					if(piIsOn)turnPiOff(time);
					//sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
					wpsSleeping=false;
					wpsCountdown=false;
					if(f_wdt == 1){
						/* Don't forget to clear the flag. */
						f_wdt = 0;
						/* Re-enter sleep mode. */
						lcd.print("Enter Comma");
						operatingStatus="Comma";
						lcd.setCursor(0,1);
						lcd.print(batteryVoltage);
						lcd.print(" V");
						delay(2000);
						lcd.setRGB(0,0,0);
						lcd.noDisplay();
						//sdCardManager.storeLifeCycleEvent(time,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
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
					operatingStatus="Comma";
					lcd.setCursor(0,1);
					lcd.print(batteryVoltage);
					lcd.print(" V");
					delay(2000);
					lcd.setRGB(0,0,0);
					lcd.noDisplay();
					//sdCardManager.storeLifeCycleEvent(time,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
					enterArduinoSleep();
				}
			}else if(piIsOn){
				//
				// i we are here it means the pi is n
				// and voltage has dropped into
				// comma range so
				faultData="Enter WPS";
				sendWPSAlert(time, faultData, batteryVoltage);
				lcd.clear();
				lcd.setRGB(225, 0, 0);
				lcd.setCursor(0, 0);
				lcd.print("Comma Alert Sent");
			}
		}
	}
}

boolean PowerManager::processDefaultCommands(String command, String sensorDataString){
	boolean processed=false;
	if(command=="TestWPSSensor"){
		float batteryVoltage = getBatteryVoltage();
		float current = getCurrentValue();
		int stateOfCharge= generalFunctions.getStateOfCharge(batteryVoltage);
		boolean result = true;//sdCardManager.testWPSSensor( batteryVoltage,  current,  stateOfCharge,  operatingStatus);
		if(result){
			_HardSerial.println("Ok-TestWPSSensor");
		}else{
			_HardSerial.println("Failure-TestWPSSensor");
		}
		_HardSerial.flush();
		processed=true;
	}else if(command=="TestLifeCycle"){
		long now = timeManager.getCurrentTimeInSeconds();
		//sdCardManager.storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_END_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
		_HardSerial.println("Ok-TestLifeCycle");
		_HardSerial.flush();

	}else if(command=="ListFiles"){
		_HardSerial.println(" ");
		_HardSerial.println(" ");
		_HardSerial.println(sensorDirName);
		float total = 0.0;//sdCardListFiles();


		_HardSerial.println(" ");

		_HardSerial.print("Used (Kb):  ");
		_HardSerial.println(total);

		_HardSerial.println("");
		_HardSerial.println("Ok-ListFiles");
		_HardSerial.flush();
		processed=true;
	}else if(command=="Ping"){

		_HardSerial.println("Ok-Ping");
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("SetTime")){

		if(capacitorVoltage==0){
			//
			// we are in normal operation
			//
			_HardSerial.println("Failure-SetTime");
			_HardSerial.flush();

		}else{
			boolean result = timeManager.setTime(command);
			if(result){
				_HardSerial.println("Ok-SetTime");
			}else{
				_HardSerial.println("Failure-SetTime");
			}

			_HardSerial.flush();
		}
		processed=true;

	}else if(command.startsWith("GetTime")){
		String time = timeManager.getCurrentDateTimeForDisplay();
		_HardSerial.println(time);
		_HardSerial.flush();
		_HardSerial.println("Ok-GetTime");
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("VerifyUserCode")){
		String codeInString = generalFunctions.getValue(command, '#', 1);
		long userCode = codeInString.toInt();
		boolean validCode = secretManager.checkCode( userCode);
		String result="Failure-Invalid Code";
		if(validCode)result="Ok-Valid Code";
		_HardSerial.println(result);
		_HardSerial.flush();
		delay(delayTime);
		processed=true;
	}else if(command.startsWith("GetCommandCodeGenerationTime")){

		long secOrig =timeManager.getTimeForCodeGeneration();

		_HardSerial.print("secOrig=");
		_HardSerial.println(secOrig);
		_HardSerial.flush();
		char secretCode[SHARED_SECRET_LENGTH];
		secretManager.readSecret(secretCode);
		_HardSerial.print("secretCode=");
		_HardSerial.println(secretCode);
		_HardSerial.flush();

		TOTP totp = TOTP(secretCode);
		long code=totp. gen_code  (secOrig ) ;


		//long code =secretManager.generateCode();
		_HardSerial.print("code=");
		_HardSerial.println(code);
		_HardSerial.println("Ok-GetCommandCodeGenerationTime");
		_HardSerial.flush();
		delay(delayTime);
		processed=true;
	}else if(command.startsWith("GetCommandCode")){

		long code =secretManager.generateCode();
		//
		// patch a bug in the totp library
		// if the first digit is a zero, it
		// returns a 5 digit number
		if(code<100000){
			_HardSerial.print("0");
			_HardSerial.println(code);
		}else{
			_HardSerial.println(code);
		}
		_HardSerial.flush();
		delay(delayTime);
		processed=true;
	}else if(command.startsWith("GetSecret")){
		if(capacitorVoltage==0){
			//
			// we are in normal operation
			//
			_HardSerial.println("Failure-GetSecret");
			_HardSerial.flush();
		}else{
			char secretCode[SHARED_SECRET_LENGTH];
			secretManager.readSecret(secretCode);
			_HardSerial.println(secretCode);
			_HardSerial.println("Ok-GetSecret");
			_HardSerial.flush();
			delay(delayTime);
		}

		processed=true;
	} else if(command.startsWith("SetSecret")){
		if(capacitorVoltage==0){
			//
			// we are in normal operation
			//
			_HardSerial.println("Failure-SetSecret");
			_HardSerial.flush();
		}else{
			String secret = generalFunctions.getValue(command, '#', 1);
			int numberDigits = generalFunctions.getValue(command, '#', 2).toInt();
			int periodSeconds = generalFunctions.getValue(command, '#', 3).toInt();
			secretManager.saveSecret(secret, numberDigits, periodSeconds);

			_HardSerial.println("Ok-SetSecret");
			_HardSerial.flush();
		}
		delay(delayTime);
		processed=true;
	}else if(command.startsWith("PulseStart")){
		inPulse=true;
		waitingManualPiStart=false;
		pulseStartTime = generalFunctions.getValue(command, '#', 1);
		_HardSerial.println("Ok-PulseStart");
		_HardSerial.flush();
		lcd.clear();
		lcd.setRGB(255,0,0);
		processed=true;
	}else if(command.startsWith("PulseFinished")){
		pulseStopTime = generalFunctions.getValue(command, '#', 1);
		inPulse=false;
		_HardSerial.println("Ok-PulseFinished");
		_HardSerial.flush();
		lcd.clear();
		lcd.setRGB(255,255,255);
		processed=true;


	}else if(command.startsWith("IPAddr")){
		currentIpAddress = generalFunctions.getValue(command, '#', 1);
		_HardSerial.println("Ok-IPAddr");
		_HardSerial.flush();
		delay(delayTime);
		processed=true;
	}else if(command.startsWith("SSID")){
		currentSSID = generalFunctions.getValue(command, '#', 1);
		_HardSerial.println("Ok-currentSSID");
		_HardSerial.flush();
		delay(delayTime);
		processed=true;
	}else if(command.startsWith("HostMode")  ){
		_HardSerial.println("Ok-HostMode");
		_HardSerial.flush();
		delay(delayTime);
		isHost=true;
		processed=true;
	}else if(command.startsWith("NetworkMode")   ){
		_HardSerial.println("Ok-NetworkMode");
		_HardSerial.flush();
		delay(delayTime);
		isHost=false;
		processed=true;
	}else if(command.startsWith("GetSensorData")){
		_HardSerial.println(sensorDataString);
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("EnterWPS")){
		//EnterWPS#10#45#30#1
		secondsToTurnPowerOff = (long)generalFunctions.getValue(command, '#', 1).toInt();
		secondsToNextPiOn = (long)generalFunctions.getValue(command, '#', 2).toInt();
		wpsPulseFrequencySeconds = generalFunctions.getValue(command, '#', 3).toInt();
		int pauseDuringWPSi = generalFunctions.getValue(command, '#', 4).toInt();
		if(pauseDuringWPSi==1)pauseDuringWPS=true;
		else pauseDuringWPS=false;
		waitingForWPSConfirmation=false;
		wpsCountdown=true;
		operatingStatus="WPS";
		wpsCountDownStartSeconds= timeManager.getCurrentTimeInSeconds();
		currentSecondsToPowerOff=0L;

		_HardSerial.println("Ok-EnterWPS");
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("ExitWPS")){

		_HardSerial.println("Ok-ExitWPS");
		_HardSerial.flush();
		inWPS=false;
		operatingStatus="Normal";
		currentSecondsToPowerOff=0L;
		wpsCountdown=false;
		processed=true;
	}else if(command.startsWith("UpdateWPSParameters")){
		String minWPSVoltageS = generalFunctions.getValue(command, '#', 1);
		char buffer[10];
		minWPSVoltageS.toCharArray(buffer, 10);
		minWPSVoltage = atof(buffer);

		minWPSVoltage = generalFunctions.stringToFloat(generalFunctions.getValue(command, '#', 1));
		enterWPSVoltage = generalFunctions.stringToFloat(generalFunctions.getValue(command, '#', 2));
		exitWPSVoltage = generalFunctions.stringToFloat(generalFunctions.getValue(command, '#', 3));

		secondsToForcedWPS = generalFunctions.getValue(command, '#', 4).toInt();
		_HardSerial.println("Ok-UpdateWPSParameters");
		_HardSerial.flush();

		processed=true;

	}else if(command.startsWith("GetRememberedValueData")){
		//GetRememberedValueData#0
		int transferData = generalFunctions.getValue(command, '#', 1).toInt();
		boolean result = true;//sdCardManager.readUntransferredFileFromSDCard( transferData,true, RememberedValueDataDirName);
		if(result){
			_HardSerial.println("Ok-GetRememberedValueData");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", remFileName, unstraferedFileName);
			_HardSerial.println(text);
		}
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetLifeCycleData")){
		//GetLifeCycleData#0
		int transferData = generalFunctions.getValue(command, '#', 1).toInt();
		boolean result = true;//sdCardManager.readUntransferredFileFromSDCard( transferData,true, LifeCycleDataDirName);
		if(result){
			_HardSerial.println("Ok-GetLifeCycleData");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", LifeCycleDataDirName, unstraferedFileName);
			_HardSerial.println(text);
		}
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetWPSSensorData")){
		//GetWPSSensorData#0
		//GetLifeCycleData#0
		int transferData = generalFunctions.getValue(command, '#', 1).toInt();
		boolean result = true;//sdCardManager.readUntransferredFileFromSDCard( transferData,true, WPSSensorDataDirName);
		if(result){
			_HardSerial.println("Ok-GetWPSSensorData");
		}else {

			char text[44];
			snprintf(text, sizeof text, "Failure-error opening /%s/%s", WPSSensorDataDirName, unstraferedFileName);
			_HardSerial.println(text);

		}
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetHistoricalWPSSensorData")){

		int date = generalFunctions.getValue(command, '#', 1).toInt();
		int month = generalFunctions.getValue(command, '#', 2).toInt();
		int year = generalFunctions.getValue(command, '#', 3).toInt();
		boolean result  = true;//sdCardManager.getHistoricalData( WPSSensorDataDirName,  date,  month,  year);
		if(result){
			_HardSerial.println("Ok-GetWPSSensorDataHistory");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", WPSSensorDataDirName, unstraferedFileName);

			_HardSerial.println(text);
		}
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetHistoricalLifeCycleData")){
		//GetHistoricalLifeCycleData#12#1#19
		int date = generalFunctions.getValue(command, '#', 1).toInt();
		int month = generalFunctions.getValue(command, '#', 2).toInt();
		int year = generalFunctions.getValue(command, '#', 3).toInt();
		boolean result  = true;//sdCardManager.getHistoricalData( LifeCycleDataDirName,  date,  month,  year);
		if (result) {
			_HardSerial.println("Ok-GetHistoricalLifeCycleData");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", LifeCycleDataDirName, unstraferedFileName);
			_HardSerial.println(text);
		}
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetHistoricalRememberedValueData")){
		//GetHistoricalLifeCycleData#12#1#19
		int date = generalFunctions.getValue(command, '#', 1).toInt();
		int month = generalFunctions.getValue(command, '#', 2).toInt();
		int year = generalFunctions.getValue(command, '#', 3).toInt();
		boolean result  = true;//sdCardManager.getHistoricalData( RememberedValueDataDirName,  date,  month,  year);
		if (result) {
			_HardSerial.println("Ok-GetHistoricalRememberedValueData");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", RememberedValueDataDirName, unstraferedFileName);

			_HardSerial.println(text);
		}
		_HardSerial.flush();
		processed=true;
	}else if (command == "AsyncData" ){
		_HardSerial.println("Ok-No Data");
		_HardSerial.flush();
		processed=true;
	}else if (command.startsWith("FaultData") ){
		//_HardSerial.println(faultData);
		if(faultData=="Enter WPS"){

			_HardSerial.print("Fault#WPS Alert#Enter WPS#");
			_HardSerial.print(secretManager.generateCode());

			_HardSerial.print("#@On Load:Notify And Shutdown:Voltage At WPS#");
			_HardSerial.println(getBatteryVoltage());
			waitingForWPSConfirmation=true;

		}else{
			_HardSerial.println("Ok");
		}

		_HardSerial.flush();
		faultData="";
		delay(delayTime);
		processed=true;
	}else if (command.startsWith("UserCommand") ){
		//
		// this function is not used in Ra2
		// because Ra2 has no buttons
		// but in the case that a teleonome does have
		//human interface buttons connected to the microcontrller
		// or there is a timer, here is where it will
		_HardSerial.println("Ok-UserCommand");
		_HardSerial.flush();
		delay(delayTime);
		processed=true;
	}else if (command.startsWith("TimerStatus") ){
		//
		// this function is not used in Ra2
		// because Ra2 has no btimers
		// but in the case that a teleonome does have
		//human interface buttons connected to the microcontrller
		// or there is a timer, here is where it will be
		_HardSerial.println("Ok-TimerStatus");
		_HardSerial.flush();
		delay(delayTime);
		processed=true;

	}
	return processed;
}
void PowerManager::endOfLoopProcessing(){
	long now = timeManager.getCurrentTimeInSeconds();
	int loopConsumingPowerSeconds = timeManager.getCurrentTimeInSeconds()-now -poweredDownInLoopSeconds;
	dailyBatteryOutEnergy+= loopConsumingPowerSeconds*getCurrentValue()/3600;
	hourlyBatteryOutEnergy+= loopConsumingPowerSeconds*getCurrentValue()/3600;
	dailyPoweredDownInLoopSeconds+=poweredDownInLoopSeconds;
	hourlyPoweredDownInLoopSeconds+=poweredDownInLoopSeconds;
}

float getLockCapacitorVoltage(){
	long  lockCapacitorValue = analogRead(LOCK_CAPACITOR_PIN);
	float capacitorVoltage= lockCapacitorValue * (5.0 / 1023.0);
	return capacitorVoltage;
}
void toggleWDT(){
	if(f_wdt == 0)
		{
			f_wdt=1;
		}
		else
		{
			//_HardSerial.println("WDT Overrun!!!");
		}
}
