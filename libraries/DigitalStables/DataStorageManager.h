/*
 * DataStorageManager.h
 *
 *  Created on: 30 Apr 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_DATASTORAGEMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_DATASTORAGEMANAGER_H_
#include "Arduino.h"
#include <SD.h>
#include <GeneralFunctions.h>
#include <LCDDisplay.h>
#include <TimeManager.h>
#include <DiscreteRecord.h>
#include <DataStorageManagerInitParams.h>
#include <WPSSensorRecord.h>
#include <LCDDisplay.h>

class DataStorageManager {
public:
	DataStorageManager();

	DataStorageManager(DataStorageManagerInitParams&  d, TimeManager& t, HardwareSerial& serial, LCDDisplay& l);
	virtual bool start()=0;
	virtual bool readUntransferredFileFromSDCardByDate(int moveData, bool sendToSerial,const char *dirName, int date, int month, int year)=0;
	virtual bool readUntransferredFileFromSDCard(int moveData, bool sendToSerial, const char *dirName)=0;
	virtual void storeRememberedValue(long time, const char *name, float value, String unit)=0;
	virtual void storeRememberedValue(long time, const char *name, float value, uint8_t operatingStatus)=0;

	virtual void storeDiscreteRecord(const DiscreteRecord& discreteRec)=0;
	virtual bool readDiscreteRecord(uint16_t index,DiscreteRecord& rec)=0;
	virtual bool openDiscreteRecordFile()=0;
	virtual void closeDiscreteRecordFile()=0;

	virtual bool storeEventRecord(const char *EventRecordDirName,const char *eventUnstraferedFileName, const byte *eventData,int eventSize )=0;
	virtual bool readEventRecord(uint16_t index, byte *eventData,int eventSize, bool moveData)=0;
	virtual int openEventRecordFile(const char *filename, int eventSize)=0;
	virtual void closeEventRecordFile(bool)=0;
	virtual bool deleteEventRecordFile(const char *filename)=0;

	virtual float searchRememberedValue(const char *label, int date, int month, int year, char *whatToSearchFor)=0;
	virtual void storeLifeCycleEvent(long time, const char *eventType, int eventValue)=0;
	virtual long printDirectory(File dir, int numTabs)=0;
	virtual bool getHistoricalData(const char *dirName, int date, int month, int year)=0;
	virtual void saveWPSSensorRecord(WPSSensorRecord anWPSSensorRecord)=0;

	//
	// Functions that represent Serial commands
	//
	virtual bool testWPSSensor(float batteryVoltage, float current, int stateOfCharge, uint8_t operatingStatus)=0;
	virtual float listFiles()=0;
	virtual uint32_t getFreeDiskSpace()=0;
	virtual uint32_t getDiskUsage()=0;

	virtual ~DataStorageManager();
};



#endif /* LIBRARIES_DIGITALSTABLES_DATASTORAGEMANAGER_H_ */
