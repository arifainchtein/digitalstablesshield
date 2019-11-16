/*
  SDCardManager.h - Library Managing the high level functions of the Sd Card
  Created by Ari Fainchtein, Feb 13, 2019.
  Released into the public domain.
 */
#ifndef SDCardManager_h
#define SDCardManager_h
#include <WPSSensorRecord.h>
#include <LCDDisplay.h>
#include "Arduino.h"
#include <SD.h>

#include <GeneralFunctions.h>
#include <DataStorageManager.h>
#include <TimeManager.h>
#include <DiscreteRecord.h>
#include <SDCardInitParams.h>

class SDCardManager: public DataStorageManager{

	TimeManager & timeManager;
	
	HardwareSerial& _HardSerial;
	LCDDisplay&  lcdDisplay;
	SDCardInitParams& dataStorageManagerInitParams;
	


public:
	SDCardManager(SDCardInitParams& d, TimeManager & t,  HardwareSerial& serial, LCDDisplay& l);
	bool start();
	bool readUntransferredFileFromSDCardByDate(int moveData, boolean sendToSerial,const char *dirName, int date, int month, int year);
	bool readUntransferredFileFromSDCard(int moveData, boolean sendToSerial, const char *dirName);
	void storeRememberedValue(long time, const char *name, float value, String unit);
	void storeRememberedValue(long time, const char *name, float value, uint8_t operatingStatus);

	void storeDiscreteRecord(const DiscreteRecord &discreteRec);


	bool readDiscreteRecord(uint16_t index,DiscreteRecord& rec);
	bool openDiscreteRecordFile();
	void closeDiscreteRecordFile();

	bool storeEventRecord(const char *EventRecordDirName,const char *eventUnstraferedFileName, const byte *eventData,int eventSize );
	bool readEventRecord(uint16_t index, byte *eventData,int eventSize, boolean moveData);
	int openEventRecordFile(const char *filename, int eventSize);
	void closeEventRecordFile(boolean);
	bool deleteEventRecordFile(const char *filename);

	float searchRememberedValue(const char *label, int date, int month, int year, char *whatToSearchFor);
	void storeLifeCycleEvent(long time, const char *eventType, int eventValue);
	long printDirectory(File dir, int numTabs);
	uint32_t getFreeDiskSpace();
		uint32_t getDiskUsage();

	long getSDCardDiskUse(File dir );
	boolean getHistoricalData(const char *dirName, int date, int month, int year);
	void saveWPSSensorRecord(WPSSensorRecord anWPSSensorRecord);

	//
	// Functions that represent Serial commands
	//
	bool testWPSSensor(float batteryVoltage, float current, int stateOfCharge, uint8_t operatingStatus);
	float listFiles();

private:
	File dataFile;

};

#endif
