/*
 * EEPROMDataStorageManager.h
 *
 *  Created on: 2 May 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_NODATASTORAGEMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_NODATASTORAGEMANAGER_H_

#include <DataStorageManager.h>
#include <DataStorageManagerInitParams.h>

class NoDataStorageManager: public DataStorageManager {

	TimeManager & timeManager;
	HardwareSerial& _HardSerial;
	LCDDisplay&  lcdDisplay;
	DataStorageManagerInitParams& dataStorageManagerInitParams;
public:
	NoDataStorageManager();
	virtual ~NoDataStorageManager();
	
	NoDataStorageManager(DataStorageManagerInitParams& d, TimeManager & t,  HardwareSerial& serial, LCDDisplay& l);
	bool start();
	bool readUntransferredFileFromSDCardByDate(int moveData, bool sendToSerial,const char *dirName, int date, int month, int year);
	bool readUntransferredFileFromSDCard(int moveData, bool sendToSerial, const char *dirName);
	void storeRememberedValue(long time, const char *name, float value, String unit);

	void storeRememberedValue(long time, const char *name, float value, uint8_t operatingStatus);

	void storeDiscreteRecord(const DiscreteRecord &discreteRec);
	bool readDiscreteRecord(uint16_t index,DiscreteRecord& rec);
	bool openDiscreteRecordFile();
	void closeDiscreteRecordFile();

	bool storeEventRecord(const char *EventRecordDirName,const char *eventUnstraferedFileName, const byte *eventData,int eventSize );
	bool readEventRecord(uint16_t index, byte *eventData,int eventSize, bool moveData);
	int openEventRecordFile(const char *filename, int eventSize);
	void closeEventRecordFile(bool);
	bool deleteEventRecordFile(const char *filename);

	float searchRememberedValue(const char *label, int date, int month, int year, char *whatToSearchFor);
	void storeLifeCycleEvent(long time, const char *eventType, int eventValue);
	long printDirectory(File dir, int numTabs);
	uint32_t getFreeDiskSpace();
	uint32_t getDiskUsage();

	bool getHistoricalData(const char *dirName, int date, int month, int year);
	void saveWPSSensorRecord(WPSSensorRecord anWPSSensorRecord);

	//
	// Functions that represent Serial commands
	//
	bool testWPSSensor(float batteryVoltage, float current, int stateOfCharge, String operatingStatus);
	float listFiles();
	bool setTime(String);
	bool testWPSSensor(float batteryVoltage, float current, int stateOfCharge, uint8_t operatingStatus);

	

};

#endif /* LIBRARIES_DIGITALSTABLES_NODATASTORAGEMANAGER_H_ */
