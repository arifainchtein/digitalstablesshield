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
	boolean start();
	boolean readUntransferredFileFromSDCardByDate(int moveData, boolean sendToSerial,const char *dirName, int date, int month, int year);
	boolean readUntransferredFileFromSDCard(int moveData, boolean sendToSerial, const char *dirName);
	void storeRememberedValue(long time, const char *name, float value, String unit);
	void storeDiscreteRecord( DiscreteRecord &discreteRec);
	boolean readDiscreteRecord(uint16_t index,DiscreteRecord& rec);
	boolean openDiscreteRecordFile();
	void closeDiscreteRecordFile();

	bool storeEventRecord(const char *EventRecordDirName,const char *eventUnstraferedFileName, const byte *eventData,int eventSize );
	bool readEventRecord(uint16_t index, byte *eventData,int eventSize, boolean moveData);
	bool openEventRecordFile(const char *filename);
	void closeEventRecordFile(boolean);
	bool deleteEventRecordFile(const char *filename);

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
	boolean testWPSSensor(float batteryVoltage, float current, int stateOfCharge, String operatingStatus);
	float listFiles();
	boolean setTime(String);

	

};

#endif /* LIBRARIES_DIGITALSTABLES_NODATASTORAGEMANAGER_H_ */
