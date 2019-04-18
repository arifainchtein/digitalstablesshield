#include "Arduino.h"
#include <LCDDisplay.h>
#include <PowerManager.h>
#include <SDCardManager.h>
#include <GeneralFunctions.h>
#include <TimeManager.h>

#include <SecretManager.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <totp.h>

#include <SPI.h>
#include <SD.h>

//
// the wps variables
#define LOCK_CAPACITOR_PIN A0
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
#define batteryVoltagePin A14
#define CURRENT_SENSOR A12
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





//
// the virtual micrcntroller

String currentIpAddress="No IP";
String currentSSID="No SSID";
int delayTime=1;
volatile int f_wdt=1;


int SHARED_SECRET_LENGTH;
float dailyMinBatteryVoltage=0;
float dailyMaxBatteryVoltage=0;

float dailyMinBatteryCurrent=0;
float dailyMaxBatteryCurrent=0;
float dailyBatteryOutEnergy=0;
float dailyPoweredDownInLoopSeconds=0;

float hourlyBatteryOutEnergy=0;
float hourlyPoweredDownInLoopSeconds=0;
boolean pauseDuringWPS=false;
boolean inPulse=false;
String pulseStartTime="";
String pulseStopTime="";
int PI_POWER_PIN=4;


static const char *UNIT_VOLT ="Volt";
static const char *UNIT_SECONDS="seconds";
static const char *UNIT_MILLI_AMPERES ="mA";
static const char *UNIT_MILLI_AMPERES_HOURS ="mAh";
static const char *UNIT_PERCENTAGE ="%";
static const char *FORCED_PI_TURN_OFF ="Forced Pi Turn Off";
static const char *BATTERY_VOLTAGE_BEFORE_PI_ON ="Battery Voltage Before Turning Pi On";
static const char *BATTERY_VOLTAGE_ATER_PI_ON="Battery Voltage After Turning Pi On";
static const char *BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON ="Battery Voltage Differential After Turning Pi On";
static const char *PI_TURN_OFF ="Pi Turn Off";
static const char *UNIT_NO_UNIT =" ";


static const char *LIFE_CYCLE_EVENT_FORCED_START_WPS ="Forced Start WPS";
static const char *LIFE_CYCLE_MANUAL_SHUTDOWN    ="Manual Shutdown";
static const char *LIFE_CYCLE_EVENT_START_WPS    ="Start WPS";
static const char *LIFE_CYCLE_EVENT_END_WPS     ="End WPS";
static const char *LIFE_CYCLE_EVENT_START_COMMA ="Start Comma";

static const char *LIFE_CYCLE_EVENT_END_COMMA ="End Comma";


static const int LIFE_CYCLE_EVENT_AWAKE_VALUE=3;
static const int LIFE_CYCLE_EVENT_WPS_VALUE=2;
static const int LIFE_CYCLE_EVENT_COMMA_VALUE=1;

static const char *DAILY_STATS_TIMESTAMP="Daily Timestamp";
static const char *DAILY_MINIMUM_BATTERY_VOLTAGE="Daily Minimum Battery Voltage";
static const char *DAILY_MAXIMUM_BATTERY_VOLTAGE="Daily Maximum Battery Voltage";
static const char *DAILY_MINIMUM_BATTERY_CURRENT="Daily Minimum Battery Current";
static const char *DAILY_MAXIMUM_BATTERY_CURRENT="Daily Maximum Battery Current";
static const char *DAILY_ENERGY="Daily Energy";
static const char *DAILY_POWERED_DOWN_IN_LOOP_SECONDS="Hourly Powered Down In Loop Seconds";
static const char *HOURLY_ENERGY="Hourly Energy";
static const char *HOURLY_POWERED_DOWN_IN_LOOP_SECONDS="Hourly Powered Down In Loop Seconds";
static const char *HOURLY_OPERATING_IN_LOOP_SECONDS="Hourly Operating In Loop Seconds";
long previousUpdate;

PowerManager::PowerManager(LCDDisplay& l, SecretManager& s, SDCardManager& sd, TimeManager& t, GeneralFunctions& f,HardwareSerial& serial ): lcd(l),secretManager(s), sdCardManager(sd),timeManager(t), generalFunctions(f), _HardSerial(serial)
{}

void PowerManager::start(){
	pinMode(52, OUTPUT);
	digitalWrite(52, LOW);
	SPI.begin();
	pinMode(PI_POWER_PIN, OUTPUT);
	long now = timeManager.getCurrentTimeInSeconds();
	turnPiOff(now);
	initializeWDT();
}
void PowerManager::hourlyTasks(long time, int previousHour ){

	sdCardManager.storeRememberedValue(time,HOURLY_ENERGY, hourlyBatteryOutEnergy, UNIT_MILLI_AMPERES_HOURS);
	sdCardManager.storeRememberedValue(time,HOURLY_POWERED_DOWN_IN_LOOP_SECONDS, hourlyPoweredDownInLoopSeconds, UNIT_SECONDS);
	sdCardManager.storeRememberedValue(time,HOURLY_OPERATING_IN_LOOP_SECONDS, 3600-hourlyPoweredDownInLoopSeconds, UNIT_SECONDS);
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
	sdCardManager.storeRememberedValue(time,DAILY_STATS_TIMESTAMP, yesterdayDateSeconds, UNIT_NO_UNIT);
	sdCardManager.storeRememberedValue(time,DAILY_MINIMUM_BATTERY_VOLTAGE, dailyMinBatteryVoltage, UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,DAILY_MAXIMUM_BATTERY_VOLTAGE, dailyMaxBatteryVoltage, UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,DAILY_MINIMUM_BATTERY_CURRENT, dailyMinBatteryCurrent, UNIT_MILLI_AMPERES);
	sdCardManager.storeRememberedValue(time,DAILY_MAXIMUM_BATTERY_CURRENT, dailyMaxBatteryCurrent, UNIT_MILLI_AMPERES);
	sdCardManager.storeRememberedValue(time,DAILY_ENERGY, dailyBatteryOutEnergy, UNIT_MILLI_AMPERES_HOURS);
	sdCardManager.storeRememberedValue(time,DAILY_POWERED_DOWN_IN_LOOP_SECONDS, dailyPoweredDownInLoopSeconds, UNIT_SECONDS);
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
		sdCardManager.storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_END_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
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
	sdCardManager.storeRememberedValue(time,faultData, batteryVoltage, UNIT_VOLT);
}

void PowerManager::turnPiOffForced(long time){
	float batteryVoltageBefore = getBatteryVoltage();
	digitalWrite(PI_POWER_PIN, LOW);
	delay(1000);
	float batteryVoltageAfter = getBatteryVoltage();
	float voltageDifferential = 1-(batteryVoltageBefore/batteryVoltageAfter);
	sdCardManager.storeRememberedValue(time,FORCED_PI_TURN_OFF,0 , operatingStatus);
	sdCardManager.storeRememberedValue(time,BATTERY_VOLTAGE_BEFORE_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,BATTERY_VOLTAGE_ATER_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON, voltageDifferential, UNIT_PERCENTAGE);
}

void PowerManager::turnPiOff(long time){
	float batteryVoltageBefore = getBatteryVoltage();
	digitalWrite(PI_POWER_PIN, LOW);
	delay(1000);
	float batteryVoltageAfter = getBatteryVoltage();
	float voltageDifferential = 1-(batteryVoltageBefore/batteryVoltageAfter);
	sdCardManager.storeRememberedValue(time,PI_TURN_OFF,0 , operatingStatus);
	sdCardManager.storeRememberedValue(time,BATTERY_VOLTAGE_BEFORE_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,BATTERY_VOLTAGE_ATER_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON, voltageDifferential, UNIT_PERCENTAGE);
}


void PowerManager::turnPiOn(long time){
	float batteryVoltageBefore = getBatteryVoltage();
	digitalWrite(PI_POWER_PIN, HIGH);
	delay(1000);
	float batteryVoltageAfter = getBatteryVoltage();
	float voltageDifferential = 1-(batteryVoltageAfter/batteryVoltageBefore);

	sdCardManager.storeRememberedValue(time,BATTERY_VOLTAGE_BEFORE_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,BATTERY_VOLTAGE_ATER_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	sdCardManager.storeRememberedValue(time,BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON, voltageDifferential, UNIT_PERCENTAGE);
}



void PowerManager::defineState(){
	poweredDownInLoopSeconds=0;
	long time = timeManager.getCurrentTimeInSeconds();

	float batteryVoltage = getBatteryVoltage();
	int internalBatteryStateOfCharge = generalFunctions.getStateOfCharge(batteryVoltage);
	float currentValue = getCurrentValue();

	boolean piIsOn = digitalRead(PI_POWER_PIN);

	if(shuttingDownPiCountdown){
		currentSecondsToPowerOff = secondsToTurnPowerOff -( time - shutDownRequestedseconds );
		lcd.clear();
		lcd.setCursor(0,0);
		String s = "Shutting down";
		lcd.print(s);
		lcd.setCursor(0,1);

		if(currentSecondsToPowerOff<=0){
			shuttingDownPiCountdown=false;
			manualShutdown=true;
			inPulse=false;
			turnPiOff(time);
			sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_MANUAL_SHUTDOWN, LIFE_CYCLE_EVENT_COMMA_VALUE);
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
				//
				// this is the most
				// common state so as to
				//avoid flickering, only refresh once per second
				if((millis()-previousUpdate) >1000){
					previousUpdate = millis();
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
			long piSleepingRemaining = secondsToNextPiOn-(time - currentSleepStartTime) ;
			lcd.clear();
			lcd.display();
			lcd.setCursor(0,0);
			lcd.setRGB(255,255,0);

			if(piSleepingRemaining<=0){
				wpsSleeping=false;
				if(!digitalRead(PI_POWER_PIN))turnPiOn(time);
				sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_END_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);

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
					anWPSSensorRecord.totalDiskUse= 0; sdCardManager.getDiskUsage();


					sdCardManager.saveWPSSensorRecord( anWPSSensorRecord);
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
					sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_FORCED_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
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
					sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
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
					sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_END_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);

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
						anWPSSensorRecord.totalDiskUse=989; sdCardManager.getDiskUsage();

						sdCardManager.saveWPSSensorRecord( anWPSSensorRecord);
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
					sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_FORCED_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
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
						sdCardManager.storeLifeCycleEvent(time,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
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
					sdCardManager.storeLifeCycleEvent(time, LIFE_CYCLE_EVENT_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
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
						sdCardManager.storeLifeCycleEvent(time,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
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
					sdCardManager.storeLifeCycleEvent(time,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
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

boolean PowerManager::processDefaultCommands(String command){
	boolean processed=false;
	if(command=="TestWPSSensor"){
		float batteryVoltage = getBatteryVoltage();
		float current = getCurrentValue();
		int stateOfCharge= generalFunctions.getStateOfCharge(batteryVoltage);
		boolean result = sdCardManager.testWPSSensor( batteryVoltage,  current,  stateOfCharge,  operatingStatus);
		if(result){
			_HardSerial.println("Ok-TestWPSSensor");
		}else{
			_HardSerial.println("Failure-TestWPSSensor");
		}
		_HardSerial.flush();
		processed=true;
	}else if(command=="TestLifeCycle"){
		long now = timeManager.getCurrentTimeInSeconds();
		sdCardManager.storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_END_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
		_HardSerial.println("Ok-TestLifeCycle");
		_HardSerial.flush();

	}else if(command=="ListFiles"){
		_HardSerial.println(" ");
		_HardSerial.println(" ");
		_HardSerial.println(sensorDirName);
		float total = sdCardManager.listFiles();


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
		boolean result = sdCardManager.readUntransferredFileFromSDCard( transferData,true, RememberedValueDataDirName);
		if(result){
			_HardSerial.println("Ok-GetRememberedValueData");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", remFileName, unstraferedFileName);
			_HardSerial.println(text);
		}
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetDiscreteData")){
		//GetDiscreteData#0
		int transferData = generalFunctions.getValue(command, '#', 1).toInt();
		boolean result = sdCardManager.readUntransferredFileFromSDCard( transferData,true, DiscreteDirName);
		if(result){
			_HardSerial.println("Ok-GetDiscreteData");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", remFileName, unstraferedFileName);
			_HardSerial.println(text);
		}
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetEventData")){
		//GetEventData#0
		int transferData = generalFunctions.getValue(command, '#', 1).toInt();
		boolean result = sdCardManager.readUntransferredFileFromSDCard( transferData,true, EventsDirName);
		if(result){
			_HardSerial.println("Ok-GetDiscreteData");
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
		boolean result = sdCardManager.readUntransferredFileFromSDCard( transferData,true, LifeCycleDataDirName);
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
		boolean result = sdCardManager.readUntransferredFileFromSDCard( transferData,true, WPSSensorDataDirName);
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
		boolean result  =sdCardManager.getHistoricalData( WPSSensorDataDirName,  date,  month,  year);
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
		boolean result  = sdCardManager.getHistoricalData( LifeCycleDataDirName,  date,  month,  year);
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
		boolean result  = sdCardManager.getHistoricalData( RememberedValueDataDirName,  date,  month,  year);
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

float PowerManager::getLockCapacitorVoltage(){
	long  lockCapacitorValue = analogRead(LOCK_CAPACITOR_PIN);
	float capacitorVoltage= lockCapacitorValue * (5.0 / 1023.0);
	return capacitorVoltage;
}
void PowerManager::toggleWDT(){
	if(f_wdt == 0)
	{
		f_wdt=1;
	}
	else
	{
		//_HardSerial.println("WDT Overrun!!!");
	}
}

String PowerManager::getBaseSensorString(){
	lcd.clear();
	lcd.setCursor(0,0);
	long now = millis();
	float batteryVoltage = getBatteryVoltage();
	lcd.print("S1:");
	long dur = millis()-now;
	lcd.print(dur);

	int internalBatteryStateOfCharge = generalFunctions.getStateOfCharge(batteryVoltage);


	now = millis();


	float currentValue = getCurrentValue();
	lcd.print(" S2:");
	 dur = millis()-now;
		lcd.print(dur);
	lcd.setCursor(0,1);
	now = millis();
	float capacitorVoltage= getLockCapacitorVoltage();
	lcd.print("S3:");
	 dur = millis()-now;
		lcd.print(dur);

	//boolean piIsOn = digitalRead(PI_POWER_PIN);
	// Generate the SensorData String
	String sensorDataString="";
	//
	// Sensor Request Queue Position 1
	//
	now = millis();
	char batteryVoltageStr[15];
	dtostrf(batteryVoltage,4, 1, batteryVoltageStr);
	sensorDataString.concat(batteryVoltageStr) ;
	sensorDataString.concat("#") ;
	lcd.print("S4:");
	 dur = millis()-now;
		lcd.print(dur);
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

	sensorDataString.concat( operatingStatus);
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 6
	//

	char dailyMinBatteryVoltageStr[15];
	dtostrf(dailyMinBatteryVoltage,4, 0, dailyMinBatteryVoltageStr);
	sensorDataString.concat(dailyMinBatteryVoltageStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 7
	//

	char dailyMaxBatteryVoltageStr[15];
	dtostrf(dailyMaxBatteryVoltage,4, 0, dailyMaxBatteryVoltageStr);
	sensorDataString.concat(dailyMaxBatteryVoltageStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 8
	//

	char dailyMinBatteryCurrentStr[15];
	dtostrf(dailyMinBatteryCurrent,4, 0, dailyMinBatteryCurrentStr);
	sensorDataString.concat(dailyMinBatteryCurrentStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 9
	//

	char dailyMaxBatteryCurrentStr[15];
	dtostrf(dailyMaxBatteryCurrent,4, 0, dailyMaxBatteryCurrentStr);
	sensorDataString.concat(dailyMaxBatteryCurrentStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 10
	//

	char dailyBatteryOutEnergyStr[15];
	dtostrf(dailyBatteryOutEnergy,4, 0, dailyBatteryOutEnergyStr);
	sensorDataString.concat(dailyBatteryOutEnergyStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 11
	//

	char dailyPoweredDownInLoopSecondsStr[15];
	dtostrf(dailyPoweredDownInLoopSeconds,4, 0, dailyPoweredDownInLoopSecondsStr);
	sensorDataString.concat(dailyPoweredDownInLoopSecondsStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 12
	//

	char hourlyBatteryOutEnergyStr[15];
	dtostrf(hourlyBatteryOutEnergy,4, 0, hourlyBatteryOutEnergyStr);
	sensorDataString.concat(hourlyBatteryOutEnergyStr) ;
	sensorDataString.concat("#") ;
	//
	// Sensor Request Queue Position 13
	//

	char hourlyPoweredDownInLoopSecondsStr[15];
	dtostrf(hourlyPoweredDownInLoopSeconds,4, 0, hourlyPoweredDownInLoopSecondsStr);
	sensorDataString.concat(hourlyPoweredDownInLoopSecondsStr) ;
	sensorDataString.concat("#") ;

	//
	// Sensor Request Queue Position 14
	//
	now = millis();
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("S5:");
	long totalDiskUse=sdCardManager.getDiskUsage();
	dur = millis()-now;

	lcd.print(dur);
	sensorDataString.concat(totalDiskUse/1024);
	sensorDataString.concat("#");
	//
	// Sensor Request Queue Position 15
	//

	sensorDataString.concat(pauseDuringWPS);
	sensorDataString.concat("#");
	return sensorDataString;

}
