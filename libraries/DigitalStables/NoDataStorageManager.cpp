/*
 * NoDataStorageManager.cpp
 *
 *  Created on: 2 May 2019
 *      Author: arifainchtein
 */

#include <NoDataStorageManager.h>
#include <WPSSensorRecord.h>

#include "Arduino.h"

#include <GeneralFunctions.h>
#include <DataStorageManager.h>
#include <DataStorageManagerInitParams.h>
#include <LCDDisplay.h>
#include <TimeManager.h>
#include <DiscreteRecord.h>
#include <Wire.h>





NoDataStorageManager::NoDataStorageManager(DataStorageManagerInitParams& d, TimeManager& t, HardwareSerial& serial, LCDDisplay& l ): dataStorageManagerInitParams(d), timeManager(t), _HardSerial(serial), lcdDisplay(l)
{}

bool NoDataStorageManager::start(){

	

}

bool NoDataStorageManager::readUntransferredFileFromSDCardByDate(int moveData, bool sendToSerial,const char *dirName, int date, int month, int year){

}
bool NoDataStorageManager::readUntransferredFileFromSDCard(int moveData, bool sendToSerial, const char *dirName){

}
void NoDataStorageManager::storeRememberedValue(long time, const char *name, float value, String unit){

}
bool NoDataStorageManager::testWPSSensor(float batteryVoltage, float current, int stateOfCharge, uint8_t operatingStatus){

}

void NoDataStorageManager::storeRememberedValue(long time, const char *name, float value, uint8_t operatingStatus){

}


void NoDataStorageManager::storeDiscreteRecord(const DiscreteRecord &discreteRec){

}

bool NoDataStorageManager::readDiscreteRecord(uint16_t index,DiscreteRecord& rec){

}
bool NoDataStorageManager::openDiscreteRecordFile(){

}
void NoDataStorageManager::closeDiscreteRecordFile(){

}

bool NoDataStorageManager::storeEventRecord(const char *EventRecordDirName,const char *eventUnstraferedFileName, const byte *eventData,int eventSize ){

}

bool NoDataStorageManager::deleteEventRecordFile(const char *filename)
{

}


bool NoDataStorageManager::readEventRecord(uint16_t index, byte *eventData,int eventSize, bool moveData){

}
int NoDataStorageManager::openEventRecordFile(const char *filename, int eventSize){
	return -1;
}
void NoDataStorageManager::closeEventRecordFile(bool){

}

uint32_t NoDataStorageManager::getFreeDiskSpace(){
	return 0;
}

uint32_t NoDataStorageManager::getDiskUsage(){
	return 0;
}

float NoDataStorageManager::searchRememberedValue(const char *label, int date, int month, int year, char *whatToSearchFor){

}
void NoDataStorageManager::storeLifeCycleEvent(long time, const char *eventType, int eventValue){

}
long NoDataStorageManager::printDirectory(File dir, int numTabs){

}

bool NoDataStorageManager::getHistoricalData(const char *dirName, int date, int month, int year){

}
void NoDataStorageManager::saveWPSSensorRecord(WPSSensorRecord anWPSSensorRecord){

}

//
// Functions that represent Serial commands
//
bool NoDataStorageManager::testWPSSensor(float batteryVoltage, float current, int stateOfCharge, String operatingStatus){

}
float NoDataStorageManager::listFiles(){

}
bool NoDataStorageManager::setTime(String){

}





NoDataStorageManager::~NoDataStorageManager() {
	// TODO Auto-generated destructor stub
}

