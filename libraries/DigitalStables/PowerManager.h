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

	TimeManager  timeManager;
	SecretManager  secretManager;
	GeneralFunctions  generalFunctions;
	SDCardManager  sdCardManager;
	HardwareSerial _HardSerial;
	LCDDisplay&  lcd;


	const char  *WPSSensorDataDirName="WPSSensr";
	const char  *EventsDirName="Events";
	const char  *DiscreteDirName="Discrete";
	const char  *LifeCycleDataDirName="LifeCycl";
	const char  *RememberedValueDataDirName  = "RememVal";
	const char  *unstraferedFileName ="Untransf.txt";

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
private:


};

#endif
