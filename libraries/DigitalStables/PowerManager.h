/*
  PowerManager.h - Library Managing the Power Management
  Created by Ari Fainchtein, March 13, 2019.
  Released into the public domain.
 */
#ifndef PowerManager_h
#define PowerManager_h
#include <WPSSensorRecord.h>

#include "Arduino.h"
#include <SD.h>
#include <GeneralFunctions.h>
#include <SDCardManager.h>
#include <TimeManager.h>
#include <SecretManager.h>
#include <LCDDisplay.h>



class PowerManager{


public:

	String operatingStatus ="Normal";

	const char  *WPSSensorDataDirName="WPSSensr";
	const char  *LifeCycleDataDirName="LifeCycl";
	const char  *RememberedValueDataDirName  = "RememVal";
	const char  *unstraferedFileName ="Untransf.txt";
	int currentViewIndex=0;
	int PI_POWER_PIN=8;
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


	PowerManager(LCDDisplay & l , SecretManager & s, SDCardManager & sd, TimeManager & t, GeneralFunctions  & f, HardwareSerial& serial);
	void start();
	void hourlyTasks(long time, int previousHour );
	void dailyTasks(long time, int yesterdayDate, int yesterdayMonth, uint16_t yesterdayYear );
	void monthlyTasks(long time);
	void yearlyTasks(long time);
	float getCurrentValue(void);
	float getBatteryVoltage();
	void initializeWDT();
	void enterArduinoSleep(void);
	void pauseWPS();
	void sendWPSAlert(long time, char *faultData, int batteryVoltage);
	void turnPiOffForced(long time);
	void turnPiOff(long time);
	void turnPiOn(long time);
	void defineState(long time, float batteryVoltage,int internalBatteryStateOfCharge, float currentValue, boolean piIsOn);
	boolean processDefaultCommands(String command, String sensorDataString);
	void endOfLoopProcessing();
	float getLockCapacitorVoltage();
	void toggleWDT();
private:


};

#endif
