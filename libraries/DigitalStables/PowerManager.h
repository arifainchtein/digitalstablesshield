/*
  PowerManager.h - Library Managing the Power Management
  Created by Ari Fainchtein, March 13, 2019.
  Released into the public domain.
 */
#include "Arduino.h"

#ifndef PowerManager_h
#define PowerManager_h
#include <DataStorageManager.h>
#include <LCDDisplay.h>
#include <WPSSensorRecord.h>
#include <SD.h>
#include <GeneralFunctions.h>
#include <TimeManager.h>
#include <SecretManager.h>
#include <PowerStatusStruct.h>




class PowerManager{


public:
	long previousUpdate;
	boolean hypothalamusStatus=false;
	long shutDownRequestedseconds= 0L;
	boolean shuttingDownPiCountdown=false;
	boolean manualShutdown=false;
	boolean waitingManualPiStart=false;
	String currentIpAddress="No IP";
	String currentSSID="No SSID";
	PowerStatusStruct aPowerStatusStruct;
	PowerStatisticsStruct aPowerStatisticsStruct;

	//
	// power variables
	//
	// sources  0=Solar 1=Grid
	// storage  0=Battery 1=Capacitor

	constexpr static const uint8_t BATTERY_ENERGY_STORAGE = 0;
	constexpr static const uint8_t CAPACITOR_ENERGY_STORAGE =1;
	constexpr static const uint8_t SOLAR_ENERGY_SOURCE =0;
	constexpr static const uint8_t GRID_ENERGY_SOURCE =1;

	//
	// the operating status can be:
	//
	// 3 Normal all systems go
	// 2 WPS
	// 1 Comma
	uint8_t operatingStatus =3;

	char *faultData;
	long secondsToForcedWPS=60L;
	long wpsAlertTime=0L;
	int delayTime=1;
	int currentViewIndex=0;

	boolean isHost=true;
	long poweredDownInLoopSeconds=0L;
	float dailyMinBatteryVoltage=0;
	float dailyMaxBatteryVoltage=0;

	float dailyMinBatteryCurrent=0;
	float dailyMaxBatteryCurrent=0;

	float dailyBatteryOutEnergy=0;
	float dailyBatteryInEnergy;
	float hourlyBatteryInEnergy;
	float hourlyBatteryOutEnergy=0;

	float dailyPoweredDownInLoopSeconds=0;

	float hourlyPoweredDownInLoopSeconds=0;
	boolean pauseDuringWPS=false;
	boolean inPulse=false;
	String pulseStartTime="";
	String pulseStopTime="";

	#define PI_POWER_PIN 0
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
	long loopStartingSeconds=0L;

	TimeManager&  timeManager;
	SecretManager&  secretManager;
	DataStorageManager&  dataStorageManager;
	HardwareSerial& _HardSerial;
	LCDDisplay&  lcd;

	constexpr static const char *UNIT_VOLT ="Volt";
	constexpr static const char *UNIT_SECONDS="sec";
	constexpr static const char *UNIT_MILLI_AMPERES ="mA";
	constexpr static const char *UNIT_MILLI_AMPERES_HOURS ="mAh";
	constexpr static const char *UNIT_PERCENTAGE ="%";
	constexpr static const char *FORCED_PI_TURN_OFF ="FPTO";
	constexpr static const char *BATTERY_VOLTAGE_BEFORE_PI_ON ="BVBTPO";
	constexpr static const char *BATTERY_VOLTAGE_ATER_PI_ON="BVATPO";
	constexpr static const char *BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON ="BVDATPO";
	constexpr static const char *PI_TURN_OFF ="Pi Turn Off";
	constexpr static const char *UNIT_NO_UNIT =" ";

	constexpr static const char *LIFE_CYCLE_EVENT_SETUP_COMPLETED="SMC"; //Setup Method Completed
	constexpr static const char *LIFE_CYCLE_EVENT_FORCED_START_WPS ="FSWPS";
	constexpr static const char *LIFE_CYCLE_MANUAL_SHUTDOWN    ="MS";
	constexpr static const char *LIFE_CYCLE_EVENT_START_WPS    ="SW";
	constexpr static const char *LIFE_CYCLE_EVENT_END_WPS     ="EWS";
	constexpr static const char *LIFE_CYCLE_EVENT_START_COMMA ="SC";
	constexpr static const char *LIFE_CYCLE_EVENT_END_COMMA ="EC";
	constexpr static const int LIFE_CYCLE_EVENT_AWAKE_VALUE=3;
	constexpr static const int LIFE_CYCLE_EVENT_WPS_VALUE=2;
	constexpr static const int LIFE_CYCLE_EVENT_COMMA_VALUE=1;

	constexpr static const char * LIFE_CYCLE_EVENT_START_EXTENDED_OPERON_EXECUTION="SEOE";
	constexpr static const char * LIFE_CYCLE_EVENT_END_EXTENDED_OPERON_EXECUTION="EEOE";
	constexpr static const char * LIFE_CYCLE_EVENT_UPDATE_EXTENDED_OPERON_EXECUTION="UEOE";

	constexpr static const char *DAILY_STATS_TIMESTAMP="DST";
	constexpr static const char *DAILY_MINIMUM_BATTERY_VOLTAGE="DMiBV";
	constexpr static const char *DAILY_MAXIMUM_BATTERY_VOLTAGE="DMaBV";
	constexpr static const char *DAILY_MINIMUM_BATTERY_CURRENT="DMiBC";
	constexpr static const char *DAILY_MAXIMUM_BATTERY_CURRENT="DMaBC";
	constexpr static const char *DAILY_ENERGY="DE";
	constexpr static const char *DAILY_POWERED_DOWN_IN_LOOP_SECONDS="DPDInLS";
	constexpr static const char *HOURLY_ENERGY="HE";
	constexpr static const char *HOURLY_POWERED_DOWN_IN_LOOP_SECONDS="HPDILS";
	constexpr static const char *HOURLY_OPERATING_IN_LOOP_SECONDS="HOILS";
	const char  *WPSSensorDataDirName="WPSSensr";

	const char  *DiscreteDirName="Discrete";
	const char  *LifeCycleDataDirName="LifeCycl";
	const char  *RememberedValueDataDirName  = "RememVal";
	const char  *unstraferedFileName ="Untransf.txt";
	PowerManager();
	PowerManager(LCDDisplay & l , SecretManager & s, DataStorageManager & sd, TimeManager & t, HardwareSerial& serial);
	void start();
	virtual void hourlyTasks(long time, int previousHour )=0;
	virtual void dailyTasks(long time, int yesterdayDate, int yesterdayMonth, uint16_t yesterdayYear) =0;
	virtual void monthlyTasks(long time)=0;
	virtual void yearlyTasks(long time)=0;
	virtual uint8_t getEnergySourceType()=0;  // 0=Solar 1=Grid
	virtual uint8_t getEnergyStorageType()=0; //0=Battery 1=Capacitor

	float getCurrentFromEnergyStorage(void);
	float getCurrentInputFromEnergySource(void);
	virtual float getEnergySourceVoltage()=0;
	virtual float getEnergyStorageVoltage()=0;
	void initializeWDT();
	void enterArduinoSleep(void);
	void pauseWPS();
	void sendWPSAlert(long time, char *faultData, int batteryVoltage);
	void turnPiOffForced(long time);
	void turnPiOff(long time);
	void turnPiOn(long time);
	virtual void defineState()=0;
	bool processDefaultCommands(String command);
	virtual void endOfLoopProcessing()=0;
	float getLockCapacitorVoltage();
	void toggleWDT();
	virtual PowerStatusStruct getPowerStatusStruct()=0;
	virtual PowerStatisticsStruct getPowerStatisticsStruct()=0;

	virtual void printPowerStatusStructToSerialPort()=0;
	virtual void printPowerStatisticsStructToSerialPort()=0;
	bool getHypothalamusStatus();
	float getVoltageRegulatorOutput();
	void setCurrentViewIndex(int);
	int getCurrentViewIndex();
	virtual ~PowerManager();
	protected:


private:
	

};

#endif
