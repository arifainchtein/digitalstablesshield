//#include <MemoryFree.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <SPI.h>
#include <SD.h>
#include "GravityRtc.h"
#include "rgb_lcd.h"

#include "totp.h"
#include "EEPROM.h"
#define LEAP_YEAR(_year) ((_year%4)==0)
static  byte monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
int timeZoneHours=11;
int SECONDOFFSET=10;

int currentCommandCodeHistoryPos=0;
int numberOfCommandCodesInHistory=5;
long commandCodeHistory[5]={999999,999999,999999,99999,99999};

GravityRtc rtc;     //RTC Initialization
char code[7];
int SHARED_SECRET_LENGTH=27;
float capacitorVoltage= 0;
#define LOCK_CAPACITOR_PIN A0
#define SD_PIN 53  // SD Card CS pin

// Arbitrary record definition for this table.
// This should be modified to reflect your record needs.
int wpsPulseFrequencySeconds=60;
long lastWPSRecordSeconds=0L;
boolean waitingForWPSConfirmation=false;
long currentSleepStartTime=0L;
long lastWPSStartUp=0L;



const int LIFE_CYCLE_EVENT_AWAKE_VALUE=3;
const int LIFE_CYCLE_EVENT_WPS_VALUE=2;
const int LIFE_CYCLE_EVENT_COMMA_VALUE=1;


String operatingStatus ="Normal";



String  WPSSensorDataDirName="WPSSensr";
String LifeCycleDataDirName="LifeCycl";
String  RememberedValueDataDirName  = "RememVal";
String  unstraferedFileName ="Untransf.txt";


#define CONST_STR(s) s
//
//#define LIFE_CYCLE_EVENT_FORCED_START_WPS CONST_STR("Forced Start WPS")
//#define LIFE_CYCLE_EVENT_START_WPS    CONST_STR("Start WPS")
//#define LIFE_CYCLE_EVENT_END_WPS      CONST_STR("End WPS")
//#define LIFE_CYCLE_EVENT_START_COMMA  CONST_STR("Start Comma")
//#define LIFE_CYCLE_EVENT_END_COMMA    CONST_STR("End Comma")
//
//#define BATTERY_VOLTAGE_BEFORE_PI_ON  CONST_STR("Battery Voltage Before Turning Pi On")
//#define BATTERY_VOLTAGE_ATER_PI_ON    CONST_STR("Battery Voltage After Turning Pi On")
//#define BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON CONST_STR("Battery Voltage Differential After Turning Pi On")
//#define FORCED_PI_TURN_OFF  CONST_STR("Forced Pi Turn Off")
//#define PI_TURN_OFF CONST_STR("Pi Turn Off")
//
#define DAILY_MINIMUM_BATTERY_VOLTAGE CONST_STR("Daily Minimum Battery Voltage")
#define DAILY_MAXIMUM_BATTERY_VOLTAGE CONST_STR("Daily Maximum Battery Voltage")
#define DAILY_MINIMUM_BATTERY_CURRENT CONST_STR("Daily Minimum Battery Current")
#define DAILY_MAXIMUM_BATTERY_CURRENT CONST_STR("Daily Maximum Battery Current")

#define MAXIMUM_VALUE CONST_STR("Max")
#define MINIMUM_VALUE CONST_STR("Min")
#define AVERAGE_VALUE CONST_STR("Avg")
//
//#define UNIT_VOLT CONST_STR("Volt")
//#define UNIT_MILLI_AMPERES CONST_STR("mA")
//#define UNIT_PERCENTAGE CONST_STR("%")


String UNIT_VOLT ="Volt";
String UNIT_MILLI_AMPERES ="mA";
String UNIT_PERCENTAGE ="%";
String FORCED_PI_TURN_OFF ="Forced Pi Turn Off";
String BATTERY_VOLTAGE_BEFORE_PI_ON ="Battery Voltage Before Turning Pi On";
String BATTERY_VOLTAGE_ATER_PI_ON="Battery Voltage After Turning Pi On";
String BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON ="Battery Voltage Differential After Turning Pi On";
String PI_TURN_OFF ="Pi Turn Off";

String LIFE_CYCLE_EVENT_FORCED_START_WPS ="Forced Start WPS";
String LIFE_CYCLE_EVENT_START_WPS    ="Start WPS";
String LIFE_CYCLE_EVENT_END_WPS      ="End WPS";
String LIFE_CYCLE_EVENT_START_COMMA ="Start Comma";
String LIFE_CYCLE_EVENT_END_COMMA ="End Comma";

String LIFE_CYCLE_EVENT_FORCED_START_WPS= "Forced Start WPS";
String LIFE_CYCLE_EVENT_START_WPS  =  "Start WPS";
String LIFE_CYCLE_EVENT_END_WPS    =  "End WPS";
String LIFE_CYCLE_EVENT_START_COMMA = "Start Comma";
String LIFE_CYCLE_EVENT_END_COMMA    ="End Comma";


//
// Analogs
//
#define batteryVoltagePin A14
#define CURRENT_SENSOR A12

//
// Digital
//
const int switchPin = 3;
const int PI_POWER_PIN=4;




String pulseStartTime="";
String pulseStopTime="";
boolean inPulse=false;
//
// the wps variables
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
//
// the virtual micrcntroller
boolean isHost=true;
String currentIpAddress="No IP";
String currentSSID="No SSID";
// the //lcd at @Sento:Internal:Actuators:LCD
rgb_lcd lcd;

boolean notInPulse=true;
String toReturn="";
int delayTime=1;
boolean showSensorData=true;
int currentViewIndex=0;
int secondsForCommandsToBeExecuted=4;
int currentCommandSecondsCounter=0;
volatile int f_wdt=1;
String faultData="";
long secondsToForcedWPS=60L;
long wpsAlertTime=0L;

int currentDay=0;
int currentMonth=0;
int currentYear=0;
float minBatteryVoltage=0;
float maxBatteryVoltage=0;

float minBatteryCurrent=0;
float maxBatteryCurrent=0;

//Sd2Card card;
//SdVolume volume;
//SdFile root;


//
// devices specific functions
//
void saveSecret(String secret, int numberDigits, int periodSeconds ){
	char someCharArray[SHARED_SECRET_LENGTH];
	secret.toCharArray(someCharArray,SHARED_SECRET_LENGTH);

	for(int i=0;i<SHARED_SECRET_LENGTH ;i++){
		EEPROM.write(i, someCharArray[i]);
	}

	EEPROM.write(SHARED_SECRET_LENGTH, numberDigits);
	EEPROM.write(SHARED_SECRET_LENGTH+1, periodSeconds);
}



void readSecret(char *secretCode){

	if ( EEPROM.read ( 0 ) != 0xff ){
		for (int i = 0; i < SHARED_SECRET_LENGTH; ++i ){
			secretCode [ i ] = EEPROM.read ( i );
		}
	}
}

float stringToFloat(String s){
	char buffer3[10];
	s.toCharArray(buffer3, 10);
	float result = atof(buffer3);
	return result;
}





boolean readUntransferredFileFromSDCardByDate(int moveData, boolean sendToSerial, String dirName, int date, int month, int year){
	//GetRememberedValueData#0
	File uf = SD.open("/" + dirName + "/" + unstraferedFileName, FILE_WRITE);
	File tf;
	boolean result=false;
	if(moveData==1) tf = SD.open("/" + dirName + "/" + date  + "_" +  month + "_" + year + ".txt", FILE_WRITE);
	if (uf) {
		uf.seek(0);
		while (uf.available()){
			//
			// Read each line, send it to the serial port
			// and copy it into today's file
			String line = uf.readStringUntil('\n');
			if(sendToSerial)Serial.print(line);
			if(moveData==1)tf.print(line);
		}
		uf.close(); // close the file
		if(moveData==1){
			tf.close(); // close the file
			//
			// since we just transferred the records and copy them
			// delete the file untransferredFile
			SD.remove("/" + dirName + "/"+unstraferedFileName);
		}
		result=true;
	}
	return result;
}

boolean readUntransferredFileFromSDCard(int moveData, boolean sendToSerial, String dirName){
	rtc.read();
	int year = rtc.year-2000;
	int month = rtc.month-1;
	return readUntransferredFileFromSDCardByDate( moveData,  sendToSerial,  dirName,  rtc.day,  month,  year);
}

void storeRememberedValue(long time, String name, float value, String unit){
	//File untransferredFile = SD.open("/" + RememberedValueDataDirName + "/" + unstraferedFileName, FILE_WRITE);

	char untransferredFileName[24];
	sprintf(untransferredFileName,"/%s/%s",RememberedValueDataDirName.c_str(),unstraferedFileName.c_str());
	File untransferredFile = SD.open(untransferredFileName, FILE_WRITE);



	if (untransferredFile) {
		// Write to file
		untransferredFile.print(time);
		untransferredFile.print("#");
		untransferredFile.print(name);
		untransferredFile.print("#");
		untransferredFile.println(value);
		untransferredFile.print("#");
		untransferredFile.println(unit);
		untransferredFile.close(); // close the file
	}
}


long dateAsSeconds(int year, int month, int date, int hour, int minute, int second){


	// note year argument is full four digit year (or digits since 2000), i.e.1975, (year 8 is 2008

	int i;
	long seconds;

	if(year < 69)
		year+= 2000;
	// seconds from 1970 till 1 jan 00:00:00 this year
	seconds= (year-1970)*(60*60*24L*365);

	// add extra days for leap years
	for (i=1970; i<year; i++) {
		if (LEAP_YEAR(i)) {
			seconds+= 60*60*24L;
		}
	}
	// add days for this year
	for (i=0; i<month; i++) {
		if (i==1 && LEAP_YEAR(year)) {
			seconds+= 60*60*24L*29;
		} else {
			seconds+= 60*60*24L*monthDays[i];
		}
	}
	seconds+= (date-1)*3600*24L;
	seconds+= hour*3600L;
	seconds+= minute*60L;
	seconds -=  timeZoneHours*3600L;
	seconds+=second;

	return seconds;
}



/*
 * this function is called at the beginning of a new day
 */
void dailyTasks(long time, int yesterdayDate, int yesterdayMonth, int yesterdayYear ){

	//
	// move whatever is in untrasferred to the correct date
	boolean result = readUntransferredFileFromSDCardByDate( 1,false, RememberedValueDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear );
	 result = readUntransferredFileFromSDCardByDate( 1,false, WPSSensorDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear);
	 result = readUntransferredFileFromSDCardByDate( 1,false, LifeCycleDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear);

	storeRememberedValue(time,DAILY_MINIMUM_BATTERY_VOLTAGE, minBatteryVoltage, UNIT_VOLT);
	storeRememberedValue(time,DAILY_MAXIMUM_BATTERY_VOLTAGE, maxBatteryVoltage, UNIT_VOLT);

	storeRememberedValue(time,DAILY_MINIMUM_BATTERY_CURRENT, minBatteryCurrent, UNIT_MILLI_AMPERES);
	storeRememberedValue(time,DAILY_MAXIMUM_BATTERY_CURRENT, maxBatteryCurrent, UNIT_MILLI_AMPERES);

	minBatteryVoltage = 9999;
	maxBatteryVoltage = -1;
	minBatteryCurrent = 9999;
	maxBatteryCurrent = -1;

}

void monthlyTasks(long time){

}

void yearlyTasks(long time){

}


long getCurrentTimeInSeconds(){
	rtc.read();
	int month = rtc.month-1;
    long now=dateAsSeconds(rtc.year, month, rtc.day, rtc.hour, rtc.minute, rtc.second);
	if (currentDay != rtc.day) {
		//
		// we are in a new day, so get yesterdays day, month and year
		int yesterdayDate=currentDay;
		int yesterdayMonth=currentMonth;
		int yesterdayYear=currentYear;

		//
		// now reset
		currentDay = rtc.day;
		dailyTasks(now,yesterdayDate, yesterdayMonth, yesterdayYear );
	}

	if (currentMonth !=month) {
		currentMonth=month;
		monthlyTasks(now);
	}

	if (currentYear !=rtc.year) {
		currentYear=rtc.year;
		yearlyTasks(now);
	}

	return now;
}

String getValue(String data, char separator, int index)
{
	int found = 0;
	int strIndex[] = {
			0, -1  };
	int maxIndex = data.length()-1;
	for(int i=0; i<=maxIndex && found<=index; i++){
		if(data.charAt(i)==separator || i==maxIndex){
			found++;
			strIndex[0] = strIndex[1]+1;
			strIndex[1] = (i == maxIndex) ? i+1 : i;
		}
	}
	return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

float searchRememberedValue(String label, int date, int month, int year, String whatToSearchFor){

	float result=-9999;
	String line="";
	String anyLabel;
	float value=0;
	int sampleCount=0;
	int sampleSum=0;
//	char fileName[23]="/";
//	strcat(fileName, *RememberedValueDataDirName);
//	strcat(fileName, "/");
//	strcat(fileName, date);
//	strcat(fileName, "_");
//	strcat(fileName, month);
//	strcat(fileName, "_");
//	strcat(fileName, year);
//	strcat(fileName, ".txt");
//	File todayFile = SD.open(fileName, FILE_WRITE);

	char fileName[32];
	sprintf(fileName,"/%s/%i_%i_%i/.txt",RememberedValueDataDirName.c_str(),date,month,year);
	File todayFile = SD.open(fileName, FILE_WRITE);


	//File todayFile = SD.open("/" + RememberedValueDataDirName + "/" + date + "_" +  month + "_" + year + ".txt", FILE_WRITE);

	if (todayFile) {
		todayFile.seek(0);
		while (todayFile.available()){
			//
			// Read each line, send it to the serial port
			// and copy it into today's file
			line = todayFile.readStringUntil('\n');
			anyLabel = getValue(line, '#', 1);
			if(label == anyLabel){
				value = stringToFloat(getValue(line, '#', 2));
				if(whatToSearchFor == MAXIMUM_VALUE){
					if(value>result)result=value;
				}else if(whatToSearchFor == MINIMUM_VALUE){
					if(value<result)result=value;
				}else if(whatToSearchFor == AVERAGE_VALUE){
					sampleCount++;
					sampleSum+=value;
				}
			}

		}
		todayFile.close(); // close the file
	}
	if(whatToSearchFor == AVERAGE_VALUE && sampleCount>0){
		result = sampleSum/sampleCount;
	}
	return result;
}


long generateCode(){
	rtc.read();
	int seconds = rtc.second+SECONDOFFSET;
	int month = rtc.month-1;

	long timestamp = dateAsSeconds(rtc.year, month, rtc.day, rtc.hour, rtc.minute, seconds);
	char secretCode[SHARED_SECRET_LENGTH];
	readSecret(secretCode);
	TOTP totp = TOTP(secretCode);
	long code=totp. gen_code  (timestamp ) ;
	//
	// now check to see if this code is already in the history
	boolean found=false;
	for(int i=0;i<numberOfCommandCodesInHistory;i++){
		if(commandCodeHistory[i]==code){
			found=true;
		}
	}

	if(!found){
		//
		// now check to see if this value is in the history
		if(currentCommandCodeHistoryPos<numberOfCommandCodesInHistory){
			commandCodeHistory[currentCommandCodeHistoryPos]=code;
			currentCommandCodeHistoryPos++;
		}else{
			for(int i=0;i<numberOfCommandCodesInHistory-1;i++){
				commandCodeHistory[i]=commandCodeHistory[i+1];
			}
			commandCodeHistory[numberOfCommandCodesInHistory-1]=code;
		}
	}

	return code;
}

boolean checkCode(long userCode){
	boolean codeOk=false;
	long code = generateCode();
	// Serial.print("code=");
	//Serial.print(code);

	if(userCode == code){
		codeOk=true;
	}else{
		for(int i=0;i<numberOfCommandCodesInHistory;i++){
			//    Serial.print("codehistory");
			//   Serial.print(commandCodeHistory[i]);

			if(commandCodeHistory[i] == userCode){
				codeOk=true;
			}
		}
	}

	return codeOk;
}

void storeLifeCycleEvent(long time, String eventType, int eventValue){

	char untransferredFileName[24];
	sprintf(untransferredFileName,"/%s/%s",LifeCycleDataDirName.c_str(),unstraferedFileName.c_str());
	File untransferredFile = SD.open(untransferredFileName, FILE_WRITE);


	//File untransferredFile = SD.open("/" + LifeCycleDataDirName + "/" + unstraferedFileName, FILE_WRITE);
	if (untransferredFile) {
		// Write to file
		untransferredFile.print(eventType);
		untransferredFile.print("#");
		untransferredFile.print(time);
		untransferredFile.print("#");
		untransferredFile.println(eventValue);
		untransferredFile.close(); // close the file
	}
}



void testRTCMode(){
	rtc.read();
	//*************************Time********************************
	//  Serial.print("   Year = ");//year
	//  Serial.print(rtc.year);
	//  Serial.print("   Month = ");//month
	//  Serial.print(rtc.month);
	//  Serial.print("   Day = ");//day
	//  Serial.print(rtc.day);
	//  Serial.print("   Week = ");//week
	//  Serial.print(rtc.week);
	//  Serial.print("   Hour = ");//hour
	//  Serial.print(rtc.hour);
	//  Serial.print("   Minute = ");//minute
	//  Serial.print(rtc.minute);
	//  Serial.print("   Second = ");//second
	int seconds = rtc.second+SECONDOFFSET;
	//  Serial.print(seconds);


	//
	int month = rtc.month-1;

	long timestamp = dateAsSeconds(rtc.year, month, rtc.day, rtc.hour, rtc.minute, seconds);
	//Serial.print("milliseconds=");
	//  Serial.println(timestamp);

	char secretCode[SHARED_SECRET_LENGTH];
	readSecret(secretCode);
	TOTP totp = TOTP(secretCode);
	//long code=totp. gen_code  (rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second,timeZoneHours*60 ) ;
	long code=totp.gen_code  (timestamp ) ;
	Serial.print(" secretCode=");
	Serial.print(secretCode);
	Serial.print(" timestamp=");
	Serial.print(timestamp);
	Serial.print(" Code=");
	Serial.println(code);
}

String getElapsedTimeHoursMinutesSecondsString(long elapsedTime) {
	//String seconds = String(elapsedTime % 60);
	long seconds = elapsedTime/1000;
	int minutes = (seconds % 3600) / 60;
	String minP ="";
	if(minutes<10)minP="0";


	int hours = seconds / 3600;
	String hoursS = "";
	if(hours<10)hoursS="0";


	String time =  hoursS + hours + ":" + minP + minutes;// + ":" + seconds;
	return time;
}




int getStatefCharge(double batteryVoltage){
	if(batteryVoltage>12.90)return 100;
	else if(batteryVoltage>=12.84)return 90;
	else if(batteryVoltage>=12.81)return 85;
	else if(batteryVoltage>=12.78)return 80;
	else if(batteryVoltage>=12.74)return 75;

	else if(batteryVoltage>=12.70)return 70;
	else if(batteryVoltage>=12.66)return 65;

	else if(batteryVoltage>=12.61)return 60;
	else if(batteryVoltage>=12.57)return 55;

	else if(batteryVoltage>=12.51)return 50;
	else if(batteryVoltage>=12.45)return 45;

	else if(batteryVoltage>=12.39)return 40;
	else if(batteryVoltage>=12.33)return 35;

	else if(batteryVoltage>=12.26)return 30;
	else if(batteryVoltage>=12.18)return 25;
	else if(batteryVoltage>=12.10)return 20;
	else if(batteryVoltage>=12.06)return 15;

	else if(batteryVoltage>=12.00)return 10;
	else if(batteryVoltage>=11.95)return 5;

	else if(batteryVoltage>=11.90)return 0;
	return 0;
}

float getBatteryVoltage(){
	long  sensorValue=analogRead(batteryVoltagePin);
	long  sum=0;
	for(int i=0;i<10;i++)
	{
		sum=sensorValue+sum;
		sensorValue=analogRead(batteryVoltagePin);
		delay(2);
	}
	sum=sum/10;
	double correction=1.025;
	float value =correction*(10*sum*4.980/1023.00);
	if(value < minBatteryVoltage)minBatteryVoltage=value;
	else if(value > maxBatteryVoltage)maxBatteryVoltage=value;
	float maxBatteryVoltage=0;

	return value;
}

float calculateCurrent(){
	int sensorValue;             //value read from the sensor
	int sensorMax = 0;
	uint32_t start_time = millis();
	while((millis()-start_time) < 1000)//sample for 1000ms
	{
		sensorValue = analogRead(CURRENT_SENSOR);
		if (sensorValue > sensorMax)
		{
			//record the maximum sensor value
			sensorMax = sensorValue;
		}
	}
	//the VCC on the Grove interface of the sensor is 5v
	float amplitude_current=(float)(sensorMax-512)/1024*5/185*1000000;
	float effective_value=amplitude_current/1.414;
	if(effective_value)minBatteryCurrent=effective_value;
	else if(effective_value > maxBatteryCurrent)maxBatteryCurrent=effective_value;
	return effective_value;
}



void initializeWDT(){
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
ISR(WDT_vect)
{
	//lcd.setCursor(0, 1);
	//lcd.print("Waking Up") ;
	//wdt_reset();


	if(f_wdt == 0)
	{
		f_wdt=1;
	}
	else
	{
		//Serial.println("WDT Overrun!!!");
	}
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
void enterSleep(void)
{
	digitalWrite(PI_POWER_PIN, LOW);



	wdt_reset();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
	//sleep_enable();

	/* Now enter sleep mode. */
	sleep_mode();

	/* The program will continue from here after the WDT timeout*/

	//
	// check the voltage of the battery, if its higher than
	// the min for wps then go into wps,
	// otherwise go back to comma
	//
	float batteryVoltage = getBatteryVoltage();
	if(batteryVoltage>minWPSVoltage){
		// STORE a lifecycle comma exit record
		long now = getCurrentTimeInSeconds();
		storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_END_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
		lcd.display();
		lcd.setRGB(255,255,0);
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Out of Comma");
		lcd.setCursor(0,1);

		lcd.print(batteryVoltage);
		lcd.print("V");
		operatingStatus="WPS";
		//lcd.setCursor(0, 1);
		//lcd.print("Awake") ;
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
		enterSleep();
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
void pauseWPS(void)
{
	digitalWrite(PI_POWER_PIN, LOW);
	lcd.noDisplay();
	lcd.setRGB(0,0,0);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
	sleep_enable();

	/* Now enter sleep mode. */
	sleep_mode();

	/* The program will continue from here after the WDT timeout*/

	lcd.display();
	lcd.setRGB(255,255,0);
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Out of Pause");
	lcd.setCursor(0,1);
	float batteryVoltage = getBatteryVoltage();
	lcd.print(batteryVoltage);
	lcd.print("V");
	operatingStatus="WPS";
	//lcd.setCursor(0, 1);
	//lcd.print("Awake") ;
	sleep_disable(); /* First thing to do is disable sleep. */
	/* Re-enable the peripherals. */
	power_all_enable();
}


boolean getHistoricalData(String dirName, int date, int month, int year){
	boolean result=false;
	File todayFile1 = SD.open("/" + dirName + "/" + date + "_" +  month + "_" + year + ".txt", FILE_WRITE);

	if (todayFile1) {
		todayFile1.seek(0);
		while (todayFile1.available()){
			//
			// Read each line, send it to the serial port
			// and copy it into today's file
			String line = todayFile1.readStringUntil('\n');
			Serial.print(line);
		}
		todayFile1.close(); // close the file
		//
		// tell the hypothalamus we are done
		//
		result=true;
	}
	return result;
}

long getSDCardDiskUse(File dir ) {

	long total=0L;
	while(true) {

		File entry =  dir.openNextFile();
		if (! entry) {
			// no more files
			break;
		}
		if (entry.isDirectory()) {
			total+=getSDCardDiskUse(entry);
		} else {
			// files have sizes, directories do not
			total+=entry.size();
		}
		entry.close();
	}
	return total;
}


long printDirectory(File dir, int numTabs) {
	long total=0L;
	while(true) {

		File entry =  dir.openNextFile();
		if (! entry) {
			// no more files
			break;
		}
		for (uint8_t i=0; i<numTabs; i++) {
			Serial.print('\t');
		}
		Serial.print(entry.name());
		if (entry.isDirectory()) {
			Serial.println("/");
			printDirectory(entry, numTabs+1);
		} else {
			// files have sizes, directories do not
			Serial.print("\t\t");
			total+=entry.size();
			Serial.println(entry.size(), DEC);
		}
		entry.close();
	}
	return total;
}

void sendWPSAlert(long time, String faultData, int batteryVoltage){
	waitingForWPSConfirmation=true;
	wpsAlertTime=getCurrentTimeInSeconds();
	lcd.clear();
	lcd.setRGB(225, 225, 0);
	lcd.setCursor(0, 0);
	lcd.print("set alert");
	storeRememberedValue(time,FORCED_PI_TURN_OFF, batteryVoltage, UNIT_VOLT);
}


void turnPiOffForced(long time){
	float batteryVoltageBefore = getBatteryVoltage();
	digitalWrite(PI_POWER_PIN, LOW);
	delay(1000);
	float batteryVoltageAfter = getBatteryVoltage();
	float voltageDifferential = 1-(batteryVoltageBefore/batteryVoltageAfter);
	storeRememberedValue(time,FORCED_PI_TURN_OFF,0 , operatingStatus);
	storeRememberedValue(time,BATTERY_VOLTAGE_BEFORE_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	storeRememberedValue(time,BATTERY_VOLTAGE_ATER_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	storeRememberedValue(time,BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON, voltageDifferential, UNIT_PERCENTAGE);
}

void turnPiOff(long time){
	float batteryVoltageBefore = getBatteryVoltage();
	digitalWrite(PI_POWER_PIN, LOW);
	delay(1000);
	float batteryVoltageAfter = getBatteryVoltage();
	float voltageDifferential = 1-(batteryVoltageBefore/batteryVoltageAfter);
	storeRememberedValue(time,PI_TURN_OFF,0 , operatingStatus);
	storeRememberedValue(time,BATTERY_VOLTAGE_BEFORE_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	storeRememberedValue(time,BATTERY_VOLTAGE_ATER_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	storeRememberedValue(time,BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON, voltageDifferential, UNIT_PERCENTAGE);
}


void turnPiOn(long time){
	float batteryVoltageBefore = getBatteryVoltage();
	digitalWrite(PI_POWER_PIN, HIGH);
	delay(1000);
	float batteryVoltageAfter = getBatteryVoltage();
	float voltageDifferential = 1-(batteryVoltageAfter/batteryVoltageBefore);

	storeRememberedValue(time,BATTERY_VOLTAGE_BEFORE_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	storeRememberedValue(time,BATTERY_VOLTAGE_ATER_PI_ON, batteryVoltageBefore, UNIT_VOLT);
	storeRememberedValue(time,BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON, voltageDifferential, UNIT_PERCENTAGE);
}



void setup() {
	Serial.begin(9600);

	pinMode(52, OUTPUT);
	digitalWrite(52, LOW);
	SPI.begin();
	rtc.setup();

	//Set the RTC time automatically: Calibrate RTC time by your computer time
	//rtc.adjustRtc(F(__DATE__), F(__TIME__));
	pinMode(PI_POWER_PIN, OUTPUT);
	digitalWrite(PI_POWER_PIN, LOW);
	long now = getCurrentTimeInSeconds();
	turnPiOff(now);
	lcd.begin(16,2);
	delay(100); //Allow for serial print to complete.
	initializeWDT();

	if (!SD.begin(SD_PIN)) {
		Serial.println("No SD-card.");
		lcd.setCursor(0, 0);
		lcd.print("No SD-card.") ;
		return;
	}else{
		// Check dir for db files
		if (!SD.exists("/" + WPSSensorDataDirName )) {
			Serial.println("wpsSensorData Dir does not exist, creating...");
			SD.mkdir("/" + WPSSensorDataDirName);
		}
		if (!SD.exists("/" + LifeCycleDataDirName)) {
			SD.mkdir("/" + LifeCycleDataDirName);
		}
		if (!SD.exists("/" + RememberedValueDataDirName)) {
			SD.mkdir("/" + RememberedValueDataDirName);
		}
		File sensorFile = SD.open("/" + WPSSensorDataDirName );
		long totalDiskUse=getSDCardDiskUse(sensorFile);
		File lifeCycleFile = SD.open("/" + LifeCycleDataDirName );
		totalDiskUse+=getSDCardDiskUse(lifeCycleFile);
		File rememberedValueFile = SD.open("/" + RememberedValueDataDirName );
		totalDiskUse+=getSDCardDiskUse(rememberedValueFile);

		lcd.setCursor(0, 0);
		lcd.print("Finish Init") ;
		lcd.setCursor(0, 1);
		lcd.print("SD use ") ;
		lcd.print(totalDiskUse/1024) ;
		lcd.print("Kb") ;
	}

//	wdt_enable(WDTO_8S);


}


void loop() {

	wdt_reset();

	float batteryVoltage = getBatteryVoltage();
	float currentValue = calculateCurrent();

	long  lockCapacitorValue=analogRead(LOCK_CAPACITOR_PIN);
	capacitorVoltage= lockCapacitorValue * (5.0 / 1023.0);



	toReturn="";

	char batteryVoltageStr[15];
	dtostrf(batteryVoltage,4, 1, batteryVoltageStr);
	toReturn.concat(batteryVoltageStr) ;
	toReturn.concat("#") ;

	char currentValueStr[15];
	dtostrf(currentValue,4, 0, currentValueStr);
	toReturn.concat(currentValueStr) ;
	toReturn.concat("#") ;

	int internalBatteryStateOfCharge = getStatefCharge(batteryVoltage);

	toReturn.concat( internalBatteryStateOfCharge);
	toReturn.concat("#") ;
	toReturn.concat( operatingStatus);
	toReturn.concat("#") ;

	File sensorFile = SD.open("/" + WPSSensorDataDirName );
	long totalDiskUse=getSDCardDiskUse(sensorFile);
	File lifeCycleFile = SD.open("/" + LifeCycleDataDirName );
	totalDiskUse+=getSDCardDiskUse(lifeCycleFile);

	File rememberedValueFile = SD.open("/" + RememberedValueDataDirName );
	totalDiskUse+=getSDCardDiskUse(rememberedValueFile);

	toReturn.concat(totalDiskUse/1024);
	toReturn.concat("#");



	//lcd.clear();
	//lcd.setCursor(0, 0);



	if(!inWPS ){
		if(!waitingForWPSConfirmation){
			long now = getCurrentTimeInSeconds();
			if(batteryVoltage>exitWPSVoltage){
				lcd.setRGB(0, 225, 0);

				if(!digitalRead(PI_POWER_PIN))turnPiOn(now);
				operatingStatus="Normal";
			}else if(batteryVoltage>minWPSVoltage && batteryVoltage<exitWPSVoltage){
				inWPS=true;
				lcd.clear();
				if(digitalRead(PI_POWER_PIN)){
					lcd.setRGB(0, 225, 0);
					lcd.setCursor(0, 0);
					lcd.print("Pi On");
					faultData="Enter WPS";
					sendWPSAlert(now, faultData, batteryVoltage);
				}else{
					lcd.setRGB(225, 225, 0);
					lcd.setCursor(0, 0);
					lcd.print("Pi Off");
					lcd.setCursor(0,1);
					lcd.print(batteryVoltage);
					lcd.print("V ");
					lcd.print(internalBatteryStateOfCharge);
					lcd.print("%") ;
					wpsCountdown=true;

				}
			}else if( batteryVoltage<=enterWPSVoltage){
				faultData="Enter WPS";
				sendWPSAlert(now, faultData, batteryVoltage);
			}

		}

	}else if(inWPS && batteryVoltage<minWPSVoltage){
		if(f_wdt == 1){
			/* Toggle the LED */
			/* Don't forget to clear the flag. */
			f_wdt = 0;
			/* Re-enter sleep mode. */
			//
			// STORE a comma enter lifecycle
			//
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("Going into Comma");
			operatingStatus="Comma";
			lcd.setCursor(0,1);
			lcd.print(batteryVoltage);
			lcd.print("V ");
			lcd.print(internalBatteryStateOfCharge);
			lcd.print("%") ;
			delay(2000);
			lcd.setRGB(0,0,0);
			//
			// if we are here, we need to turn the pi off because it the voltage
			// is too low.  this is a bad situation, because the hypothalamus is
			// is not notified, and therefore bad things can haapne, from a corrupt
			// denome file to a corrupt sd card.
			//being here is not a good thing.  it means that the battery is losing voltage
			// two quick.  normally, the voltage drop should be more gradual,
			//and the hypothalamus would finish a pulse shut itself down and the
			// wps would not go into pulse until the voltage come up
			long now = getCurrentTimeInSeconds();
			turnPiOffForced(now);
			lcd.noDisplay();
			storeLifeCycleEvent(now,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
			enterSleep();
		}
	}else if(batteryVoltage>minWPSVoltage && batteryVoltage<exitWPSVoltage){
		lcd.setRGB(255, 225, 0);
		long now = getCurrentTimeInSeconds();
		faultData="Enter WPS";
		sendWPSAlert(now, faultData, batteryVoltage);
		operatingStatus="WPS";

	}else if(batteryVoltage>exitWPSVoltage){
		lcd.setRGB(0, 225, 0);
		long now = getCurrentTimeInSeconds();
		if(!digitalRead(PI_POWER_PIN))turnPiOn(now);
		operatingStatus="Normal";

	}




	//
	// the commands
	//
	if( Serial.available() != 0) {


		//lcd.clear();
		String command = Serial.readString();
		lcd.setCursor(0, 0);
		lcd.print(command);
		if(command=="TestWPSSensor"){
			lastWPSRecordSeconds = getCurrentTimeInSeconds();
			File untransferredFile = SD.open("/" + WPSSensorDataDirName + "/" + unstraferedFileName, FILE_WRITE);
			if (untransferredFile) {
				// Write to file
				float batteryVoltage = getBatteryVoltage();
				float current = calculateCurrent();
				int sc = getStatefCharge(batteryVoltage);

				untransferredFile.print(lastWPSRecordSeconds);
				untransferredFile.print("#");
				untransferredFile.print(batteryVoltage);
				untransferredFile.print("#");
				untransferredFile.print(current);
				untransferredFile.print("#");
				untransferredFile.print(sc);
				untransferredFile.print("#");
				untransferredFile.print(operatingStatus);


				File sensorFile = SD.open("/" + WPSSensorDataDirName );
				long totalDiskUse=getSDCardDiskUse(sensorFile);
				File lifeCycleFile = SD.open("/" + LifeCycleDataDirName );
				totalDiskUse+=getSDCardDiskUse(lifeCycleFile);
				untransferredFile.print("#");
				untransferredFile.println(totalDiskUse/1024);


				untransferredFile.close(); // close the file
			}
			Serial.println("Ok-TestWPSSensor");
			Serial.flush();

		}else if(command=="TestLifeCycle"){
			long now = getCurrentTimeInSeconds();
			storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_END_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
			Serial.println("Ok-TestLifeCycle");
			Serial.flush();

		}else if(command=="ListFiles"){
			Serial.println(" ");
			Serial.println(" ");
			Serial.println("/" + WPSSensorDataDirName);
			File sensorFile = SD.open("/" + WPSSensorDataDirName );
			long totalDiskUse=printDirectory(sensorFile, 1);
			Serial.println(" ");
			Serial.println("/" + LifeCycleDataDirName);
			File lifeCycleFile = SD.open("/" + LifeCycleDataDirName );
			totalDiskUse+=printDirectory(lifeCycleFile, 1);

			Serial.println(" ");
			Serial.println("/" + RememberedValueDataDirName);
			File rememberedValueFile = SD.open("/" + RememberedValueDataDirName );
			totalDiskUse+=printDirectory(rememberedValueFile, 1);



			//			Serial.print("Clusters:          ");
			//			Serial.println(volume.clusterCount());
			//			Serial.print("Blocks x Cluster:  ");
			//			Serial.println(volume.blocksPerCluster());
			//
			//			Serial.print("Total Blocks:      ");
			//			Serial.println(volume.blocksPerCluster() * volume.clusterCount());
			//			Serial.println();

			// print the type and size of the first FAT-type volume
			//			uint32_t volumesize;
			//			Serial.print("Volume type is:    FAT");
			//			Serial.println(volume.fatType(), DEC);
			//
			//			volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
			//			volumesize *= volume.clusterCount();       // we'll have a lot of clusters
			//			volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
			//			Serial.print("Volume size (Kb):  ");
			//			Serial.println(volumesize);
			//			Serial.print("Volume size (Mb):  ");
			//			volumesize /= 1024;
			//			Serial.println(volumesize);
			//			Serial.print("Volume size (Gb):  ");
			//			Serial.println((float)volumesize / 1024.0);

			Serial.println(" ");

			Serial.print("Used (Kb):  ");
			Serial.println((float)totalDiskUse/1024);

			//			Serial.print("Free (Mb):  ");
			//			Serial.println(volumesize-totalDiskUse);
			Serial.println("");
			Serial.println("Ok-ListFiles");
			Serial.flush();
		}else if(command=="Ping"){

			Serial.println("Ok-Ping");
			Serial.flush();
		}else if(command.startsWith("SetTime")){
			int date = getValue(command, '#', 1).toInt();
			int month = getValue(command, '#', 2).toInt();
			int year = getValue(command, '#', 3).toInt();
			int dw = getValue(command, '#', 4).toInt();
			int hour = getValue(command, '#', 5).toInt();
			int min = getValue(command, '#', 6).toInt();
			int sec = getValue(command, '#', 7).toInt();
			rtc.adjustRtc(year,month,date,dw,hour,min,sec);

			rtc.read();
			Serial.print(rtc.day);
			Serial.print("/");//month
			Serial.print(rtc.month);
			Serial.print("/");//month
			Serial.print(rtc.year);
			Serial.print(" ");//month
			Serial.print(rtc.hour);
			Serial.print(":");
			Serial.print(rtc.minute);
			Serial.print(":");
			Serial.println(rtc.second);
			Serial.println("Ok-SetTime");
			Serial.flush();

		}else if(command.startsWith("GetTime")){


			rtc.read();
			Serial.print(rtc.day);
			Serial.print("/");//month
			Serial.print(rtc.month);
			Serial.print("/");//month
			Serial.print(rtc.year);
			Serial.print(" ");//month
			Serial.print(rtc.hour);
			Serial.print(":");
			Serial.print(rtc.minute);
			Serial.print(":");
			Serial.println(rtc.second);
			Serial.println("Ok-GetTime");
			Serial.flush();
		}else if(command.startsWith("VerifyUserCode")){
			String codeInString = getValue(command, '#', 1);
			long userCode = codeInString.toInt();
			boolean validCode = checkCode( userCode);
			String result="Failure-Invalid Code";
			if(validCode)result="Ok-Valid Code";
			Serial.println(result);
			Serial.flush();
			delay(delayTime);
		}else if(command.startsWith("GetCommandCode")){

			long code =generateCode();
			//
			// patch a bug in the totp library
			// if the first digit is a zero, it
			// returns a 5 digit number
			if(code<100000){
				Serial.print("0");
				Serial.println(code);
			}else{
				Serial.println(code);
			}
			Serial.flush();
			delay(delayTime);
		}else if(command.startsWith("GetSecret")){
			if(capacitorVoltage==0){
				//
				// we are in normal operation
				//
				Serial.println("Failure-GetSecret");
				Serial.flush();
			}else{
				char secretCode[SHARED_SECRET_LENGTH];
				readSecret(secretCode);
				Serial.println(secretCode);
				Serial.println("Ok-GetSecret");
				Serial.flush();
				delay(delayTime);
			}


		} else if(command.startsWith("SetSecret")){
			if(capacitorVoltage==0){
				//
				// we are in normal operation
				//
				Serial.println("Failure-SetSecret");
				Serial.flush();
			}else{
				String secret = getValue(command, '#', 1);
				int numberDigits = getValue(command, '#', 2).toInt();
				int periodSeconds = getValue(command, '#', 3).toInt();
				saveSecret(secret, numberDigits, periodSeconds);

				Serial.println("Ok-SetSecret");
				Serial.flush();
			}
			delay(delayTime);
		}else if(command.startsWith("PulseStart")){
			inPulse=true;
			pulseStartTime = getValue(command, '#', 1);
			Serial.println("Ok-PulseStart");
			Serial.flush();
			//lcd.clear();
		}else if(command.startsWith("PulseFinished")){
			pulseStopTime = getValue(command, '#', 1);
			inPulse=false;
			Serial.println("Ok-PulseFinished");
			Serial.flush();
			//lcd.clear();
		}else if(command.startsWith("IPAddr")){
			notInPulse=false;
			currentIpAddress = getValue(command, '#', 1);
			Serial.println("Ok-IPAddr");
			Serial.flush();
			delay(delayTime);
			notInPulse=true;

		}else if(command.startsWith("SSID")){
			notInPulse=false;
			currentSSID = getValue(command, '#', 1);
			Serial.println("Ok-currentSSID");
			Serial.flush();
			delay(delayTime);
			notInPulse=true;

		}else if(command.startsWith("HostMode")){
			Serial.println("Ok-IPAddr");
			Serial.flush();
			delay(delayTime);
			isHost=true;
		}else if(command.startsWith("NetworkMode")){
			Serial.println("Ok-IPAddr");
			Serial.flush();
			delay(delayTime);
			isHost=false;
		}else if(command.startsWith("GetSensorData")){
			Serial.println(toReturn);
			Serial.flush();

		}else if(command.startsWith("EnterWPS")){
			//EnterWPS#10#45#30
			secondsToTurnPowerOff = (long)getValue(command, '#', 1).toInt();
			secondsToNextPiOn = (long)getValue(command, '#', 2).toInt();
			wpsPulseFrequencySeconds = getValue(command, '#', 3).toInt();
			waitingForWPSConfirmation=false;
			wpsCountdown=true;
			operatingStatus="WPS";
			wpsCountDownStartSeconds= getCurrentTimeInSeconds();
			currentSecondsToPowerOff=0L;

			Serial.println("Ok-EnterWPS");
			Serial.flush();
		}else if(command.startsWith("ExitWPS")){

			Serial.println("Ok-ExitWPS");
			Serial.flush();
			inWPS=false;
			operatingStatus="Normal";
			currentSecondsToPowerOff=0L;
			wpsCountdown=false;

		}else if(command.startsWith("UpdateWPSParameters")){
			String minWPSVoltageS = getValue(command, '#', 1);
			char buffer[10];
			minWPSVoltageS.toCharArray(buffer, 10);
			minWPSVoltage = atof(buffer);

			minWPSVoltage = stringToFloat(getValue(command, '#', 1));
			enterWPSVoltage = stringToFloat(getValue(command, '#', 2));
			exitWPSVoltage = stringToFloat(getValue(command, '#', 3));

			secondsToForcedWPS = getValue(command, '#', 4).toInt();
			Serial.println("Ok-UpdateWPSParameters");
			Serial.flush();



		}else if(command.startsWith("GetRememberedValueData")){
			//GetRememberedValueData#0
			int transferData = getValue(command, '#', 1).toInt();
			boolean result = readUntransferredFileFromSDCard( transferData,true, RememberedValueDataDirName);
			if(result){
				Serial.println("Ok-GetRememberedValueData");
			}else {
				Serial.println("Failure-error opening/" + RememberedValueDataDirName + "/" + unstraferedFileName);
			}
			Serial.flush();
		}else if(command.startsWith("GetLifeCycleData")){
			//GetLifeCycleData#0
			int transferData = getValue(command, '#', 1).toInt();
			boolean result = readUntransferredFileFromSDCard( transferData,true, LifeCycleDataDirName);
			if(result){
				Serial.println("Ok-GetLifeCycleData");
			}else {
				Serial.println("Failure-error opening/" + LifeCycleDataDirName + "/" + unstraferedFileName);
			}
			Serial.flush();
		}else if(command.startsWith("GetWPSSensorData")){
			//GetWPSSensorData#0
			//GetLifeCycleData#0
			int transferData = getValue(command, '#', 1).toInt();
			boolean result = readUntransferredFileFromSDCard( transferData,true, WPSSensorDataDirName);
			if(result){
				Serial.println("Ok-GetWPSSensorData");
			}else {
				Serial.println("Failure-error opening/" + WPSSensorDataDirName + "/" + unstraferedFileName);
			}
			Serial.flush();

		}else if(command.startsWith("GetHistoricalWPSSensorData")){

			int date = getValue(command, '#', 1).toInt();
			int month = getValue(command, '#', 2).toInt();
			int year = getValue(command, '#', 3).toInt();
			boolean result  = getHistoricalData( WPSSensorDataDirName,  date,  month,  year);
			if(result){
				Serial.println("Ok-GetWPSSensorDataHistory");
			}else {
				Serial.println("Failure-error opening/" + WPSSensorDataDirName + "/" +unstraferedFileName);
			}
			Serial.flush();
		}else if(command.startsWith("GetHistoricalLifeCycleData")){
			//GetHistoricalLifeCycleData#12#1#19
			int date = getValue(command, '#', 1).toInt();
			int month = getValue(command, '#', 2).toInt();
			int year = getValue(command, '#', 3).toInt();
			boolean result  = getHistoricalData( LifeCycleDataDirName,  date,  month,  year);
			if (result) {
				Serial.println("Ok-GetHistoricalLifeCycleData");
			}else {
				Serial.println("Failure-error opening/" + LifeCycleDataDirName + "/" + unstraferedFileName);
			}
			Serial.flush();
		}else if(command.startsWith("GetHistoricalRememberedValueData")){
			//GetHistoricalLifeCycleData#12#1#19
			int date = getValue(command, '#', 1).toInt();
			int month = getValue(command, '#', 2).toInt();
			int year = getValue(command, '#', 3).toInt();
			boolean result  = getHistoricalData( RememberedValueDataDirName,  date,  month,  year);
			if (result) {
				Serial.println("Ok-GetHistoricalRememberedValueData");
			}else {
				Serial.println("Failure-error opening/" + RememberedValueDataDirName + "/" + unstraferedFileName);
			}
			Serial.flush();
		}else if (command.startsWith("AsyncData") ){

			Serial.print("AsyncCycleUpdate#");
			Serial.println(toReturn);
			Serial.flush();
			delay(delayTime);
		}else if (command.startsWith("FaultData") ){
			//Serial.println(faultData);
			if(faultData=="Enter WPS"){

				Serial.print("Fault#WPS Alert#Enter WPS#");
				Serial.print(generateCode());

				Serial.print("#@On Load:Notify And Shutdown:Voltage At WPS#");
				Serial.println(batteryVoltage);
				waitingForWPSConfirmation=true;

			}else{
				Serial.println("Ok");
			}

			Serial.flush();
			faultData="";
			delay(delayTime);
		}else if (command.startsWith("UserCommand") ){
			//
			// this function is not used in Ra2
			// because Ra2 has no buttons
			// but in the case that a teleonome does have
			//human interface buttons connected to the microcontrller
			// or there is a timer, here is where it will
			Serial.println("Ok-UserCommand");
			Serial.flush();
			delay(delayTime);

		}else if (command.startsWith("TimerStatus") ){
			//
			// this function is not used in Ra2
			// because Ra2 has no btimers
			// but in the case that a teleonome does have
			//human interface buttons connected to the microcontrller
			// or there is a timer, here is where it will be
			Serial.println("Ok-TimerStatus");
			Serial.flush();
			delay(delayTime);

		}else{
			//
			// call read to flush the incoming
			//
			Serial.read();
			Serial.println("Failure-Bad Command " + command);
			Serial.flush();
		}
	}

	long now = getCurrentTimeInSeconds();


	if(waitingForWPSConfirmation){
		//
		// check to see if the current voltage
		// has changed, if so, cancel the wps
		// if not continue
		if(batteryVoltage>exitWPSVoltage){
			wpsSleeping=false;
			operatingStatus="Normal";
			inWPS=false;
			waitingForWPSConfirmation=false;
			lcd.setRGB(0,255,0);
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("pi on Normal mode ");
			lcd.setCursor(0,1);
			lcd.print(batteryVoltage);
			lcd.print(" V");
		}else{
			delay(1000);
			long z = now-wpsAlertTime;
			lcd.clear();
			lcd.setRGB(225,225,0);
			lcd.setCursor(0,0);
			lcd.print("Waiting EnterWPS");
			lcd.setCursor(0,1);
			long remaining = secondsToForcedWPS-z;
			lcd.print(remaining);
			lcd.print("  ");
			lcd.print(batteryVoltage);
			lcd.print("V ");
			//lcd.print(freeMemory());

			inWPS=true;
			if( remaining <= 0  ){
				waitingForWPSConfirmation=false;
				operatingStatus="WPS";
				storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_FORCED_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);

				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("pi off");
				wpsSleeping=true;
				currentSleepStartTime = now;
				currentSecondsToPowerOff=0L;
				turnPiOff(now);
				wpsCountdown=false;
			}
		}




	}else if(wpsCountdown){
		currentSecondsToPowerOff = secondsToTurnPowerOff -( now - wpsCountDownStartSeconds);
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("wps countdown ");
		lcd.setCursor(0,1);
		lcd.print(	currentSecondsToPowerOff);
		if(currentSecondsToPowerOff<=0){
			currentSecondsToPowerOff=0;
			turnPiOff(now);
			storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_START_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
			inWPS=true;
			wpsSleeping=true;
			wpsCountdown=false;
			currentSleepStartTime=now;
		}
	}else if(inWPS){
		if(wpsSleeping){
			//delay(1000);
			//lcd.noDisplay();
			long remaining = secondsToNextPiOn-(now - currentSleepStartTime) ;
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("pi sleep for ");
			lcd.print(remaining);
			if(remaining<=0){
				//
				// check if there is enough battery
				//
				if(batteryVoltage>exitWPSVoltage){
					wpsSleeping=false;
					if(!digitalRead(PI_POWER_PIN))turnPiOn(now);
					//
					// STORE life cycle WPS_END event
					operatingStatus="Normal";
					inWPS=false;
					waitingForWPSConfirmation=false;
					lcd.setRGB(0,255,0);
					storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_END_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
					lcd.clear();
					lcd.setCursor(0,0);
					lcd.print("pi on Normal mode ");
					lcd.setCursor(0,1);
					lcd.print(batteryVoltage);
					lcd.print("V ");
					lcd.print(internalBatteryStateOfCharge);
					lcd.print("%") ;
				}else if(batteryVoltage>minWPSVoltage && batteryVoltage<=exitWPSVoltage){
					wpsSleeping=false;
					inWPS=true;
					if(!digitalRead(PI_POWER_PIN))turnPiOn(now);
					storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_END_WPS, LIFE_CYCLE_EVENT_WPS_VALUE);
					operatingStatus="WPS";



					lcd.clear();
					lcd.display();
					if(batteryVoltage>minWPSVoltage){
						lcd.setRGB(255,255,0);
					}else{
						lcd.setRGB(255,0,0);
					}
					lcd.setCursor(0,0);
					lcd.print("pi ON WPS ");
					lcd.setCursor(0,1);
					lcd.print(batteryVoltage);
					lcd.print("V ");
					lcd.print(internalBatteryStateOfCharge);
					lcd.print("%") ;
					lastWPSStartUp = now;
				}else{
					//
					// there is not enough battery
					// reset currentSecondsSleeping
					// back to zero to start another cycle
					// send the processor to sleep
					//lcd.setCursor(0, 1);
					//lcd.print("Deep Sleep.... ") ;

					if(f_wdt == 1){
						/* Toggle the LED */
						/* Don't forget to clear the flag. */
						f_wdt = 0;
						/* Re-enter sleep mode. */
						//
						// STORE a comma enter lifecycle
						//
						lcd.clear();
						lcd.setCursor(0,0);
						lcd.print("Going into Comma");
						operatingStatus="Comma";
						lcd.setCursor(0,1);
						lcd.print(batteryVoltage);
						lcd.print("V ");
						lcd.print(internalBatteryStateOfCharge);
						lcd.print("%") ;
						delay(2000);
						lcd.setRGB(0,0,0);
						lcd.noDisplay();
						storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
						enterSleep();
					}else{
						/* Do nothing. */
					}
				}

			}else{
				//
				// if we are here is because we are in the
				// sleep period of the wps cycle.
				long z =now-lastWPSRecordSeconds;
				lcd.clear();
				lcd.display();
				lcd.setRGB(255,255,0);
				lcd.setCursor(0,0);
				lcd.print("wps rec in ");
				long remaining = (long)wpsPulseFrequencySeconds-z;

				lcd.print(remaining);
				lcd.setCursor(0,1);
				lcd.print("pi on in ");
				long piremaining = secondsToNextPiOn-(now - currentSleepStartTime) ;
				lcd.print(piremaining);


				//delay(1000);

				if(remaining<=0){
					lcd.clear();
					lcd.display();

					lastWPSRecordSeconds = getCurrentTimeInSeconds();
					File untransferredFile = SD.open("/" + WPSSensorDataDirName + "/" + unstraferedFileName, FILE_WRITE);

					if (untransferredFile) {
						// Write to file
						lcd.setRGB(0,0,255);
						float batteryVoltage = getBatteryVoltage();
						float current = calculateCurrent();
						int sc = getStatefCharge(batteryVoltage);
						untransferredFile.print(lastWPSRecordSeconds);
						untransferredFile.print("#");
						untransferredFile.print(batteryVoltage);
						untransferredFile.print("#");
						untransferredFile.print(current);
						untransferredFile.print("#");
						//
						// calculate the energy used in mAhr
						//
						float energy = wpsPulseFrequencySeconds*current/3600;
						untransferredFile.print(energy);
						untransferredFile.print("#");
						untransferredFile.print(sc);
						untransferredFile.print("#");
						untransferredFile.print(operatingStatus);

						File sensorFile = SD.open("/" + WPSSensorDataDirName );
						long totalDiskUse=getSDCardDiskUse(sensorFile);
						File lifeCycleFile = SD.open("/" + LifeCycleDataDirName );
						totalDiskUse+=getSDCardDiskUse(lifeCycleFile);
						untransferredFile.print("#");
						untransferredFile.println(totalDiskUse/1024);

						untransferredFile.close(); // close the file
						lcd.setCursor(0,0);
						lcd.print("saved wps record");
						lcd.setCursor(0,1);
						lcd.print("to SD card ");


					} else {
						lcd.setRGB(255,255,0);
						lcd.setCursor(0,0);
						lcd.print("error opening");
						lcd.setCursor(0,1);
						lcd.print(WPSSensorDataDirName + "/" + unstraferedFileName);
						Serial.println("error opening/" +WPSSensorDataDirName + "/" + unstraferedFileName);
					}
					lcd.setRGB(255,255,0);
				}else{
					//
					// if we are here is because we are in the sleeping part of the
					// WPS and is not time to take another record
					// now check if there is any reason to keep it from comma
					// ie if its raining and the sensor needs to stay on
					// if not sleep for 8 seconds
					//
					boolean stayAwake=true;

					if(!stayAwake){
						pauseWPS();
					}
				}

				if(batteryVoltage<minWPSVoltage){
					//
					// there is not enough battery
					// reset currentSecondsSleeping
					// back to zero to start another cycle
					// send the processor to sleep
					//lcd.setCursor(0, 1);
					//lcd.print("Deep Sleep.... ") ;

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
						storeLifeCycleEvent(now,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
						enterSleep();
					}else{
						/* Do nothing. */
					}

				}
			}
		}else{
			if(batteryVoltage<minWPSVoltage){
				//
				// if we are here, it means that
				// the pi is on but the battery voltage
				// is below the minimum
				// send an alert to the hypothalamus to shut things down
				// orderly
				sendWPSAlert(now, faultData, batteryVoltage);
			}else{
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("pi ON WPS ");
				lcd.print(batteryVoltage);
				lcd.print(" V");
				lcd.setCursor(0,1);
				lcd.print("Runtime ");
				long secsRunning = now-lastWPSStartUp;
				lcd.print(secsRunning);
			}

		}
	}else{
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
			// currentViewIndex = 0 means show data
			// currentViewIndex = 1 means confirm shutdown
			// currentViewIndex = 2 shutdown in process
			// currentViewIndex = 3 means confirm restart
			// currentViewIndex = 4 restart in process
			// i
			switch(currentViewIndex){
			case 0:
				//
				// alternate between sensor data and
				// network info
				if(showSensorData){
					lcd.clear();
					lcd.setCursor(0, 0);
					lcd.print((int)currentValue);
					lcd.print("mA ") ;
					lcd.print(batteryVoltageStr) ;
					lcd.print("V ") ;
					lcd.print(internalBatteryStateOfCharge);
					lcd.print("%") ;
				}else{
					lcd.clear();
					lcd.setCursor(0, 0);
					lcd.print(currentSSID);
					lcd.setCursor(0, 1);
					lcd.print(currentIpAddress) ;
				}
				delay(1000);
				showSensorData=!showSensorData;
				break;

			case 1:
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("Shutdown");
				lcd.setCursor(0, 1);
				lcd.print("Are You Sure?");
				break;

			case 2:
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("Shutting Down" );
				lcd.setCursor(0, 1);
				lcd.print("See you soon" );
				break;
			}
		}
	}



	if(currentViewIndex>0){
		currentCommandSecondsCounter++;
	}
	if(currentCommandSecondsCounter>secondsForCommandsToBeExecuted){
		currentViewIndex=0;
		currentCommandSecondsCounter=0;
	}


}

