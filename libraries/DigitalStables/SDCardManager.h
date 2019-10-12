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
#include <File.cpp>

#include <GeneralFunctions.h>
#include <DataStorageManager.h>
#include <LCDDisplay.h>
#include <TimeManager.h>
#include <DiscreteRecord.h>
#include <DataStorageManagerInitParams.h>

class SDCardManager: public DataStorageManager{

	TimeManager & timeManager;
	
	HardwareSerial& _HardSerial;
	LCDDisplay&  lcdDisplay;
	DataStorageManagerInitParams& dataStorageManagerInitParams;
	


public:
	SDCardManager(DataStorageManagerInitParams& d, TimeManager & t,  HardwareSerial& serial, LCDDisplay& l);
	bool start();
	bool readUntransferredFileFromSDCardByDate(int moveData, boolean sendToSerial,const char *dirName, int date, int month, int year);
	bool readUntransferredFileFromSDCard(int moveData, boolean sendToSerial, const char *dirName);
	void storeRememberedValue(long time, const char *name, float value, String unit);

	void storeDiscreteRecord(const DiscreteRecord &discreteRec);
	void storeRememberedValue(long time, const char *name, float value, uint8_t operatingStatus);

	bool readDiscreteRecord(uint16_t index,DiscreteRecord& rec);
	bool openDiscreteRecordFile();
	void closeDiscreteRecordFile();

	void storeEventRecord(const char *EventRecordDirName, const byte *eventData,int eventSize );
	bool readEventRecord(uint16_t index, byte *eventData,int eventSize, boolean moveData);
	bool openEventRecordFile(const char *filename);
	void closeEventRecordFile(boolean);

	float searchRememberedValue(const char *label, int date, int month, int year, char *whatToSearchFor);
	void storeLifeCycleEvent(long time, const char *eventType, int eventValue);
	long printDirectory(File dir, int numTabs);
	long getDiskUsage();
	long getSDCardDiskUse(File dir );
	boolean getHistoricalData(const char *dirName, int date, int month, int year);
	void saveWPSSensorRecord(WPSSensorRecord anWPSSensorRecord);

	//
	// Functions that represent Serial commands
	//
	bool testWPSSensor(float batteryVoltage, float current, int stateOfCharge, uint8_t operatingStatus);
	float listFiles();
	uint32_t getFreeDiskSpace();

private:
	File dataFile;

};

#endif
