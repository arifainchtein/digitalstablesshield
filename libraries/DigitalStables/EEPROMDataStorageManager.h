/*
 * EEPROMDataStorageManager.h
 *
 *  Created on: 2 May 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_EEPROMDATASTORAGEMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_EEPROMDATASTORAGEMANAGER_H_
#include <Arduino.h>
#include <DataStorageManager.h>
#include <EEPROMDataStorageManagerInitParams.h>

class EEPROMDataStorageManager: public DataStorageManager {

	TimeManager & timeManager;
	
	HardwareSerial& _HardSerial;
	LCDDisplay&  lcdDisplay;
	EEPROMDataStorageManagerInitParams& dataStorageManagerInitParams;

	uint8_t  pageSize=128;
uint8_t wpSensorDataFolderSize;
uint8_t lifeCycleFolderSize;
uint8_t discreteRecordFolderSize;
uint8_t eventFolderSize;
uint8_t rememberedValueFolderSize;
public:
	EEPROMDataStorageManager();
	virtual ~EEPROMDataStorageManager();

	EEPROMDataStorageManager(EEPROMDataStorageManagerInitParams& d, TimeManager & t, HardwareSerial& serial, LCDDisplay& l);
	bool start();
	bool readUntransferredFileFromSDCardByDate(int moveData, boolean sendToSerial,const char *dirName, int date, int month, int year);
	bool readUntransferredFileFromSDCard(int moveData, boolean sendToSerial, const char *dirName);
	void storeRememberedValue(long time, const char *name, float value, String unit);
	void storeDiscreteRecord( DiscreteRecord &discreteRec);
	bool readDiscreteRecord(uint16_t index,DiscreteRecord& rec);
	bool openDiscreteRecordFile();
	void closeDiscreteRecordFile();

	bool storeEventRecord(const char *EventRecordDirName,const char *eventUnstraferedFileName, const byte *eventData,int eventSize );
	bool readEventRecord(uint16_t index, byte *eventData,int eventSize, boolean moveData);
	bool openEventRecordFile(const char *filename);
	void closeEventRecordFile(boolean);


	float searchRememberedValue(const char *label, int date, int month, int year, char *whatToSearchFor);
	void storeLifeCycleEvent(long time, const char *eventType, int eventValue);
	long printDirectory(File dir, int numTabs);
	long getDiskUsage();
	long getSDCardDiskUse(File dir );
	bool getHistoricalData(const char *dirName, int date, int month, int year);
	void saveWPSSensorRecord(WPSSensorRecord anWPSSensorRecord);

	//
	// Functions that represent Serial commands
	//
	boolean testWPSSensor(float batteryVoltage, float current, int stateOfCharge, String operatingStatus);
	float listFiles();
	
	

private:
	void writeEEPROM(int deviceaddress, unsigned int eeaddress, char* data);
	void readEEPROM(int deviceaddress, unsigned int eeaddress, unsigned char* data, unsigned int num_chars);
};

#endif /* LIBRARIES_DIGITALSTABLES_EEPROMDATASTORAGEMANAGER_H_ */
