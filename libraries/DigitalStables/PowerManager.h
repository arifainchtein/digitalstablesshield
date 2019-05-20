/*
  PowerManager.h - Library Managing the Power Management
  Created by Ari Fainchtein, March 13, 2019.
  Released into the public domain.
 */
#include "Arduino.h"

#ifndef PowerManager_h
#define PowerManager_h
#include <SDCardManager.h>
#include <LCDDisplay.h>
#include <WPSSensorRecord.h>
#include <SD.h>
#include <GeneralFunctions.h>
#include <TimeManager.h>
#include <SecretManager.h>





class PowerManager{


public:

	String operatingStatus ="Normal";


	int currentViewIndex=0;

	boolean isHost=true;
	long poweredDownInLoopSeconds;
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
	long secondsToTurnPowerOff = 30;
	long secondsToNextPiOn=90L;
	long currentSecondsToPowerOff=0L;
	boolean wpsCountdown=false;
	boolean wpsSleeping=false;
	boolean inWPS=false;
	float minWPSVoltage=12.4;
	float enterWPSVoltage=12.6;
	float exitWPSVoltage=12.8;
	long lastWPSStartUp=0L;
	long lastWPSRecordSeconds=0L;
	int wpsPulseFrequencySeconds=60;
	boolean waitingForWPSConfirmation=false;
	long currentSleepStartTime=0L;
	volatile int f_wdt=1;
	long wpsCountDownStartSeconds=0L;

	TimeManager  timeManager;
	SecretManager  secretManager;
	GeneralFunctions  generalFunctions;
	SDCardManager  sdCardManager;
	HardwareSerial _HardSerial;
	LCDDisplay&  lcd;

	static const char *UNIT_VOLT ="Volt";
	static const char *UNIT_SECONDS="sec";
	static const char *UNIT_MILLI_AMPERES ="mA";
	static const char *UNIT_MILLI_AMPERES_HOURS ="mAh";
	static const char *UNIT_PERCENTAGE ="%";
	static const char *FORCED_PI_TURN_OFF ="FPTO";
	static const char *BATTERY_VOLTAGE_BEFORE_PI_ON ="BVBTPO";
	static const char *BATTERY_VOLTAGE_ATER_PI_ON="BVATPO";
	static const char *BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON ="BVDATPO";
	static const char *PI_TURN_OFF ="Pi Turn Off";
	static const char *UNIT_NO_UNIT =" ";


	static const char *LIFE_CYCLE_EVENT_FORCED_START_WPS ="FSWPS";
	static const char *LIFE_CYCLE_MANUAL_SHUTDOWN    ="MS";
	static const char *LIFE_CYCLE_EVENT_START_WPS    ="SW";
	static const char *LIFE_CYCLE_EVENT_END_WPS     ="EWS";
	static const char *LIFE_CYCLE_EVENT_START_COMMA ="SC";
	static const char *LIFE_CYCLE_EVENT_END_COMMA ="EC";
	static const int LIFE_CYCLE_EVENT_AWAKE_VALUE=3;
	static const int LIFE_CYCLE_EVENT_WPS_VALUE=2;
	static const int LIFE_CYCLE_EVENT_COMMA_VALUE=1;

	static const char *DAILY_STATS_TIMESTAMP="DST";
	static const char *DAILY_MINIMUM_BATTERY_VOLTAGE="DMiBV";
	static const char *DAILY_MAXIMUM_BATTERY_VOLTAGE="DMaBV";
	static const char *DAILY_MINIMUM_BATTERY_CURRENT="DMiBC";
	static const char *DAILY_MAXIMUM_BATTERY_CURRENT="DMaBC";
	static const char *DAILY_ENERGY="DE";
	static const char *DAILY_POWERED_DOWN_IN_LOOP_SECONDS="DPDInLS";
	static const char *HOURLY_ENERGY="HE";
	static const char *HOURLY_POWERED_DOWN_IN_LOOP_SECONDS="HPDILS";
	static const char *HOURLY_OPERATING_IN_LOOP_SECONDS="HOILS";
	const char  *WPSSensorDataDirName="WPSSensr";
	const char  *EventsDirName="Events";
	const char  *DiscreteDirName="Discrete";
	const char  *LifeCycleDataDirName="LifeCycl";
	const char  *RememberedValueDataDirName  = "RememVal";
	const char  *unstraferedFileName ="Untransf.txt";
	PowerManager();
	PowerManager(LCDDisplay & l , SecretManager & s, SDCardManager & sd, TimeManager & t, GeneralFunctions  & f, HardwareSerial& serial);
	void start();
	void hourlyTasks(long time, int previousHour );
	void dailyTasks(long time, int yesterdayDate, int yesterdayMonth, uint16_t yesterdayYear );
	void monthlyTasks(long time);
	void yearlyTasks(long time);
	float getCurrentFromBattery(void);
	float getCurrentInputFromSolarPanel(void);
	float getSolarPanelVoltage();
	float getBatteryVoltage();
	void initializeWDT();
	void enterArduinoSleep(void);
	void pauseWPS();
	void sendWPSAlert(long time, char *faultData, int batteryVoltage);
	void turnPiOffForced(long time);
	void turnPiOff(long time);
	void turnPiOn(long time);
	void defineState();
	boolean processDefaultCommands(String command);
	void endOfLoopProcessing();
	float getLockCapacitorVoltage();
	void toggleWDT();
	String getBaseSensorString();

protected:


private:


};

#endif
