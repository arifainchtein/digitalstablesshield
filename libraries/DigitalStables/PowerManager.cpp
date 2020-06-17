#include <Arduino.h>
#include <LCDDisplay.h>
#include <PowerManager.h>
#include <DataStorageManager.h>
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
#define BATTERY_VOLTAGE_PIN A1
#define CURRENT_SENSOR A4
#define LOCK_CAPACITOR_PIN A5
#define VOLTAGE_REGULATOR_OUTPUT_SENSOR A6




float capacitorVoltage= 0;


// the battery voltage always in A1
// for compatibility with Gloria, Wally and Valentino
//


    //effective current
bool powerSupplyOn=false;

//
// current view index
// Controls what the user sees in the lcdnow
// it starts with a value of 99 which means is locked


bool showingAct=false;

int currentHour=0;
int currentDay=0;
int currentMonth=0;
int currentYear=0;




float Aref = 1.075;
unsigned int total;
float voltage;

//
// the virtual micrcntroller













PowerManager::PowerManager(LCDDisplay& l, SecretManager& s, DataStorageManager& sd, TimeManager& t, HardwareSerial& serial ): lcd(l),secretManager(s), dataStorageManager(sd),timeManager(t), _HardSerial(serial)
{}

PowerManager::~PowerManager() {
	// TODO Auto-generated destructor stub
}




void PowerManager::start(){
	// pinMode(52, OUTPUT);
	// digitalWrite(52, LOW);
	 SPI.begin();


	pinMode(PI_POWER_PIN, OUTPUT);
	pinMode(CURRENT_SENSOR, INPUT);
	pinMode(BATTERY_VOLTAGE_PIN, INPUT);
	pinMode(VOLTAGE_REGULATOR_OUTPUT_SENSOR, INPUT);
	
	lcd.print("init power manager");
	long now = timeManager.getCurrentTimeInSeconds();
	turnPiOff(now);
	initializeWDT();
	lcd.print("finish init power manager");
}



float PowerManager::getCurrentInputFromEnergySource(void){
	return 0.0;
}



float PowerManager::getVoltageRegulatorOutput(){
  
  int NUM_SAMPLES=10;
  int sample_count=0;
  float sum=0;
  while (sample_count < NUM_SAMPLES) {
        sum += analogRead(VOLTAGE_REGULATOR_OUTPUT_SENSOR);
        sample_count++;
        delay(10);
    }
    float voltage = .986*((float)sum / (float)NUM_SAMPLES * 5.015) / 1024.0;
    return voltage;
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
	float batteryVoltage = getEnergyStorageVoltage();
	float regulatorOutput = getVoltageRegulatorOutput();
	if(batteryVoltage>minWPSVoltage){
		// STORE a lifecycle comma exit record
		long now = timeManager.getCurrentTimeInSeconds();
		dataStorageManager.storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_END_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
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

		operatingStatus=2;
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
	float batteryVoltage = getEnergyStorageVoltage();
	lcd.print(batteryVoltage);
	lcd.print("V ");
	lcd.print(pauseDuringWPS);

	operatingStatus=2;
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
	operatingStatus=2;
	wpsAlertTime=timeManager.getCurrentTimeInSeconds();
	dataStorageManager.storeRememberedValue(time,faultData, batteryVoltage, UNIT_VOLT);
}

void PowerManager::turnPiOffForced(long time){
	float batteryVoltageBefore = getEnergyStorageVoltage();
	
	digitalWrite(PI_POWER_PIN, LOW);
	delay(1000);
	float batteryVoltageAfter = getEnergyStorageVoltage();
	float regulatorVoltageAfter = getVoltageRegulatorOutput();
	float voltageDifferential = 1-(batteryVoltageBefore/batteryVoltageAfter);
	dataStorageManager.storeRememberedValue(time,FORCED_PI_TURN_OFF,0 , operatingStatus);
	dataStorageManager.storeRememberedValue(time,BATTERY_VOLTAGE_BEFORE_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	dataStorageManager.storeRememberedValue(time,BATTERY_VOLTAGE_ATER_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	dataStorageManager.storeRememberedValue(time,BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON, voltageDifferential, UNIT_PERCENTAGE);
}

void PowerManager::turnPiOff(long time){
	float batteryVoltageBefore = getEnergyStorageVoltage();
	digitalWrite(PI_POWER_PIN, LOW);
	delay(1000);
	float batteryVoltageAfter = getEnergyStorageVoltage();
	float voltageDifferential = 1-(batteryVoltageBefore/batteryVoltageAfter);
	dataStorageManager.storeRememberedValue(time,PI_TURN_OFF,0 , operatingStatus);
	dataStorageManager.storeRememberedValue(time,BATTERY_VOLTAGE_BEFORE_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	dataStorageManager.storeRememberedValue(time,BATTERY_VOLTAGE_ATER_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	dataStorageManager.storeRememberedValue(time,BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON, voltageDifferential, UNIT_PERCENTAGE);
}


void PowerManager::turnPiOn(long time){
	float batteryVoltageBefore = getEnergyStorageVoltage();
	digitalWrite(PI_POWER_PIN, HIGH);
	delay(1000);
	float batteryVoltageAfter = getEnergyStorageVoltage();
	float voltageDifferential = 1-(batteryVoltageAfter/batteryVoltageBefore);

	dataStorageManager.storeRememberedValue(time,BATTERY_VOLTAGE_BEFORE_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	dataStorageManager.storeRememberedValue(time,BATTERY_VOLTAGE_ATER_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	dataStorageManager.storeRememberedValue(time,BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON, voltageDifferential, UNIT_PERCENTAGE);
}

int PowerManager::getCurrentViewIndex(){
	return currentViewIndex;
}

void PowerManager::setCurrentViewIndex(int i){
	currentViewIndex=i;
}
bool PowerManager::getHypothalamusStatus(){
	return hypothalamusStatus;
}


bool PowerManager::processDefaultCommands(String command){
	bool processed=false;
	capacitorVoltage = getLockCapacitorVoltage();
	if(command=="TestWPSSensor"){
		float batteryVoltage = getEnergyStorageVoltage();
		float regulatorVoltage = getVoltageRegulatorOutput();
		
		float current = getCurrentFromEnergyStorage();
		int stateOfCharge= GeneralFunctions::getStateOfCharge(batteryVoltage);
		bool result = dataStorageManager.testWPSSensor( batteryVoltage,  current,  stateOfCharge,  operatingStatus);
		if(result){
			_HardSerial.println("Ok-TestWPSSensor");
		}else{
			_HardSerial.println("Failure-TestWPSSensor");
		}
		_HardSerial.flush();
		processed=true;
	}else if(command=="TestLifeCycle"){
		long now = timeManager.getCurrentTimeInSeconds();
		dataStorageManager.storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_END_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
		_HardSerial.println("Ok-TestLifeCycle");
		_HardSerial.flush();

	}else if(command=="ListFiles"){
		_HardSerial.println(" ");
		_HardSerial.println(" ");
		
		float total = dataStorageManager.listFiles();


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
		_HardSerial.print("capacitorVoltage=");
		_HardSerial.print(capacitorVoltage);

		if(capacitorVoltage>4.8){
			//
			// we are in normal operation
			//
			_HardSerial.println("Failure-SetTime");
			_HardSerial.flush();

		}else{
			bool result = timeManager.setTime(command);
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
		String codeInString = GeneralFunctions::getValue(command, '#', 1);
		long userCode = codeInString.toInt();
		bool validCode = secretManager.checkCode( userCode);
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
		char secretCode[SecretManager::SHARED_SECRET_LENGTH];
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
		if(capacitorVoltage>4.85){
			//
			// we are in normal operation
			//
			_HardSerial.println("Failure-GetSecret");
			_HardSerial.flush();
		}else{
			char secretCode[SecretManager::SHARED_SECRET_LENGTH];
			secretManager.readSecret(secretCode);
			_HardSerial.println(secretCode);
			_HardSerial.println("Ok-GetSecret");
			_HardSerial.flush();
			delay(delayTime);
		}

		processed=true;
	} else if(command.startsWith("SetSecret")){
		if(capacitorVoltage<4.8){
			//
			// we are in normal operation
			//
			_HardSerial.println("Failure-SetSecret");
			_HardSerial.flush();
		}else{
			String secret = GeneralFunctions::getValue(command, '#', 1);
			int numberDigits = GeneralFunctions::getValue(command, '#', 2).toInt();
			int periodSeconds = GeneralFunctions::getValue(command, '#', 3).toInt();
			secretManager.saveSecret(secret, numberDigits, periodSeconds);

			_HardSerial.println("Ok-SetSecret");
			_HardSerial.flush();
		}
		delay(delayTime);
		processed=true;
	}else if(command.startsWith("PulseStart")){
		inPulse=true;
		waitingManualPiStart=false;
		pulseStartTime = GeneralFunctions::getValue(command, '#', 1);
		_HardSerial.println("Ok-PulseStart");
		_HardSerial.flush();

		processed=true;
	}else if(command.startsWith("PulseFinished")){
		pulseStopTime = GeneralFunctions::getValue(command, '#', 1);
		inPulse=false;
		_HardSerial.println("Ok-PulseFinished");
		_HardSerial.flush();

		processed=true;


	}else if(command.startsWith("IPAddr")){
		currentIpAddress = GeneralFunctions::getValue(command, '#', 1);
		_HardSerial.println("Ok-IPAddr");
		_HardSerial.flush();
		delay(delayTime);
		processed=true;
	}else if(command.startsWith("SSID")){
		currentSSID = GeneralFunctions::getValue(command, '#', 1);
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
		secondsToTurnPowerOff = (long)GeneralFunctions::getValue(command, '#', 1).toInt();
		secondsToNextPiOn = (long)GeneralFunctions::getValue(command, '#', 2).toInt();
		wpsPulseFrequencySeconds = GeneralFunctions::getValue(command, '#', 3).toInt();
		int pauseDuringWPSi = GeneralFunctions::getValue(command, '#', 4).toInt();
		if(pauseDuringWPSi==1)pauseDuringWPS=true;
		else pauseDuringWPS=false;
		waitingForWPSConfirmation=false;
		wpsCountdown=true;
		operatingStatus=2;
		wpsCountDownStartSeconds= timeManager.getCurrentTimeInSeconds();
		currentSecondsToPowerOff=0L;

		_HardSerial.println("Ok-EnterWPS");
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("ExitWPS")){

		_HardSerial.println("Ok-ExitWPS");
		_HardSerial.flush();
		inWPS=false;
		operatingStatus=3;
		currentSecondsToPowerOff=0L;
		wpsCountdown=false;
		processed=true;
	}else if(command.startsWith("UpdateWPSParameters")){
		String minWPSVoltageS = GeneralFunctions::getValue(command, '#', 1);
		char buffer[10];
		minWPSVoltageS.toCharArray(buffer, 10);
		minWPSVoltage = atof(buffer);

		minWPSVoltage = GeneralFunctions::stringToFloat(GeneralFunctions::getValue(command, '#', 1));
		enterWPSVoltage = GeneralFunctions::stringToFloat(GeneralFunctions::getValue(command, '#', 2));
		exitWPSVoltage = GeneralFunctions::stringToFloat(GeneralFunctions::getValue(command, '#', 3));

		secondsToForcedWPS = GeneralFunctions::getValue(command, '#', 4).toInt();
		_HardSerial.println("Ok-UpdateWPSParameters");
		_HardSerial.flush();

		processed=true;

	}else if(command.startsWith("GetRememberedValueData")){
		//GetRememberedValueData#0
		int transferData = GeneralFunctions::getValue(command, '#', 1).toInt();
		bool result = dataStorageManager.readUntransferredFileFromSDCard( transferData,true, RememberedValueDataDirName);
		if(result){
			_HardSerial.println("Ok-GetRememberedValueData");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", RememberedValueDataDirName, unstraferedFileName);
			_HardSerial.println(text);
		}
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetLifeCycleData")){
		//GetLifeCycleData#0
		int transferData = GeneralFunctions::getValue(command, '#', 1).toInt();
		bool result = dataStorageManager.readUntransferredFileFromSDCard( transferData,true, LifeCycleDataDirName);
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
		int transferData = GeneralFunctions::getValue(command, '#', 1).toInt();
		bool result = dataStorageManager.readUntransferredFileFromSDCard( transferData,true, WPSSensorDataDirName);
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

		int date = GeneralFunctions::getValue(command, '#', 1).toInt();
		int month = GeneralFunctions::getValue(command, '#', 2).toInt();
		int year = GeneralFunctions::getValue(command, '#', 3).toInt();
		bool result  =dataStorageManager.getHistoricalData( WPSSensorDataDirName,  date,  month,  year);
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
		int date = GeneralFunctions::getValue(command, '#', 1).toInt();
		int month = GeneralFunctions::getValue(command, '#', 2).toInt();
		int year = GeneralFunctions::getValue(command, '#', 3).toInt();
		bool result  = dataStorageManager.getHistoricalData( LifeCycleDataDirName,  date,  month,  year);
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
		int date = GeneralFunctions::getValue(command, '#', 1).toInt();
		int month = GeneralFunctions::getValue(command, '#', 2).toInt();
		int year = GeneralFunctions::getValue(command, '#', 3).toInt();
		bool result  = dataStorageManager.getHistoricalData( RememberedValueDataDirName,  date,  month,  year);
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
			_HardSerial.println(getEnergyStorageVoltage());
			waitingForWPSConfirmation=true;

		}else{
			_HardSerial.println("Ok");
		}

		_HardSerial.flush();
		faultData="";
		delay(delayTime);
		processed=true;
	}else if (command.startsWith("TimerStatus") ){
		//
		// in the case that a teleonome does have
		//human interface buttons connected to the microcontrller
		// or there is a timer, here is where it will be
		_HardSerial.println("Ok-TimerStatus");
		_HardSerial.flush();
		delay(delayTime);
		processed=true;

	}
	return processed;
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

//PowerStatusStruct PowerManager::getPowerStatusStruct(){
//	float batteryVoltage = getEnergyStorageVoltage();
//	int currentValue = getCurrentFromEnergyStorage();
//	float capacitorVoltage= getLockCapacitorVoltage();
//	byte internalEnergyStorageStateOfCharge = GeneralFunctions::getStateOfCharge(batteryVoltage);
//	//float regulatorVoltage = getVoltageRegulatorOutput();
//	//long totalDiskUse=dataStorageManager.getDiskUsage()/1024;
//
//	aPowerStatusStruct.sampleTime = timeManager.getCurrentTimeInSeconds();
//	aPowerStatusStruct.batteryVoltage= batteryVoltage;
//	aPowerStatusStruct.solarPanelVoltage= getSolarPanelVoltage();
//	aPowerStatusStruct.currentFromEnergyStorage=currentValue;
//	aPowerStatusStruct.capacitorVoltage=capacitorVoltage;
//	aPowerStatusStruct.internalEnergyStorageStateOfCharge=internalEnergyStorageStateOfCharge;
//	//aPowerStatusStruct.regulatorVoltage=regulatorVoltage;
//	aPowerStatusStruct.operatingStatus=operatingStatus;
//
//	return aPowerStatusStruct;
//}
//
//PowerStatisticsStruct PowerManager::getPowerStatisticsStruct(){
////	long totalDiskUse=dataStorageManager.getDiskUsage()/1024;
//
//	aPowerStatisticsStruct.sampleTime = timeManager.getCurrentTimeInSeconds();
//	aPowerStatisticsStruct.dailyMinEnergyStorageVoltage=dailyMinEnergyStorageVoltage;
//	aPowerStatisticsStruct.dailyMaxEnergyStorageVoltage=dailyMaxEnergyStorageVoltage;
//	aPowerStatisticsStruct.dailyMinEnergyStorageCurrent=dailyMinEnergyStorageCurrent;
//	aPowerStatisticsStruct.dailyMaxEnergyStorageCurrent=dailyMaxEnergyStorageCurrent;
//	aPowerStatisticsStruct.dailyEnergyStorageOutEnergy=dailyEnergyStorageOutEnergy;
//	aPowerStatisticsStruct.dailyPoweredDownInLoopSeconds=dailyPoweredDownInLoopSeconds;
//	aPowerStatisticsStruct.hourlyEnergyStorageOutEnergy=hourlyEnergyStorageOutEnergy;
//	aPowerStatisticsStruct.hourlyPoweredDownInLoopSeconds=hourlyPoweredDownInLoopSeconds;
//
//	//	aPowerStatusStruct.totalDiskUse=totalDiskUse;
//
//	return aPowerStatisticsStruct;
//}

void PowerManager::printPowerStatusStructToSerialPort(){
	_HardSerial.print("in PowerManager printPowerStatusStructToSerialPort" ) ;
	float batteryVoltage = getEnergyStorageVoltage();
	byte internalEnergyStorageStateOfCharge = GeneralFunctions::getStateOfCharge(batteryVoltage);
	//int currentValue = getCurrentFromEnergyStorage();
	float capacitorVoltage= getLockCapacitorVoltage();

	//
	// Sensor Request Queue Position 1
	//
	char batteryVoltageStr[15];
	dtostrf(batteryVoltage,4, 1, batteryVoltageStr);
	_HardSerial.print(batteryVoltageStr) ;
	_HardSerial.print("#") ;
	
//	//
//	// Sensor Request Queue Position 2
//	//
//	char currentValueStr[15];
//	dtostrf(currentValue,4, 0, currentValueStr);
//	_HardSerial.print(currentValueStr) ;
//	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 2
	//
	char capacitorVoltageStr[15];
	dtostrf(capacitorVoltage,2, 1, capacitorVoltageStr);
	_HardSerial.print(capacitorVoltageStr) ;
	_HardSerial.print("#") ;


	//
	// Sensor Request Queue Position 3
	//
	_HardSerial.print( internalEnergyStorageStateOfCharge);
	_HardSerial.print("#") ;

//	//
//	// Sensor Request Queue Position 5
//	//
//	float regulatorVoltage = getVoltageRegulatorOutput();
//	char regulatorVoltageStr[15];
//	dtostrf(regulatorVoltage,2, 2, regulatorVoltageStr);
//	_HardSerial.print( regulatorVoltageStr);
//	_HardSerial.print("#") ;


	//
	// Sensor Request Queue Position 4
	//

	_HardSerial.print( operatingStatus);
	_HardSerial.print("#") ;

	
}

void PowerManager::printPowerStatisticsStructToSerialPort(){
	//
	// Sensor Request Queue Position 7
	//

	char dailyMinEnergyStorageVoltageStr[15];
	dtostrf(dailyMinEnergyStorageVoltage,4, 0, dailyMinEnergyStorageVoltageStr);
	_HardSerial.print(dailyMinEnergyStorageVoltageStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 8
	//

	char dailyMaxEnergyStorageVoltageStr[15];
	dtostrf(dailyMaxEnergyStorageVoltage,4, 0, dailyMaxEnergyStorageVoltageStr);
	_HardSerial.print(dailyMaxEnergyStorageVoltageStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 9
	//

	char dailyMinEnergyStorageCurrentStr[15];
	dtostrf(dailyMinEnergyStorageCurrent,4, 0, dailyMinEnergyStorageCurrentStr);
	_HardSerial.print(dailyMinEnergyStorageCurrentStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 10
	//

	char dailyMaxEnergyStorageCurrentStr[15];
	dtostrf(dailyMaxEnergyStorageCurrent,4, 0, dailyMaxEnergyStorageCurrentStr);
	_HardSerial.print(dailyMaxEnergyStorageCurrentStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 11
	//

	char dailyEnergyStorageOutEnergyStr[15];
	dtostrf(dailyEnergyStorageOutEnergy,4, 0, dailyEnergyStorageOutEnergyStr);
	_HardSerial.print(dailyEnergyStorageOutEnergyStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 12
	//

	char dailyPoweredDownInLoopSecondsStr[15];
	dtostrf(dailyPoweredDownInLoopSeconds,4, 0, dailyPoweredDownInLoopSecondsStr);
	_HardSerial.print(dailyPoweredDownInLoopSecondsStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 13
	//

	char hourlyEnergyStorageOutEnergyStr[15];
	dtostrf(hourlyEnergyStorageOutEnergy,4, 0, hourlyEnergyStorageOutEnergyStr);
	_HardSerial.print(hourlyEnergyStorageOutEnergyStr) ;
	_HardSerial.print("#") ;
	//
	// Sensor Request Queue Position 14
	//

	char hourlyPoweredDownInLoopSecondsStr[15];
	dtostrf(hourlyPoweredDownInLoopSeconds,4, 0, hourlyPoweredDownInLoopSecondsStr);
	_HardSerial.print(hourlyPoweredDownInLoopSecondsStr) ;
	_HardSerial.print("#") ;


}
