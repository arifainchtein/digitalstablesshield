/*
 * EEPROMDataStorageManager.cpp
 *
 *  Created on: 2 May 2019
 *      Author: arifainchtein
 */

#include <EEPROMDataStorageManager.h>
#include <WPSSensorRecord.h>
#include <LCDDisplay.h>
#include "Arduino.h"
#include <SD.h>
#include <GeneralFunctions.h>
#include <DataStorageManager.h>
#include <DataStorageManagerInitParams.h>
#include <LCDDisplay.h>
#include <TimeManager.h>
#include <DiscreteRecord.h>
#include <Wire.h>

uint8_t  pageSize=128;
uint8_t wpSensorDataFolderSize;
uint8_t lifeCycleFolderSize;
uint8_t discreteRecordFolderSize;
uint8_t eventFolderSize;
uint8_t rememberedValueFolderSize;

EEPROMDataStorageManager::EEPROMDataStorageManager() {
	// TODO Auto-generated constructor stub

}

EEPROMDataStorageManager::EEPROMDataStorageManager(DataStorageManagerInitParams& d, TimeManager& t, GeneralFunctions& f,HardwareSerial& serial, LCDDisplay& l ): dataStorageManagerInitParams(d), timeManager(t), generalFunctions(f), _HardSerial(serial), lcdDisplay(l)
{}

boolean EEPROMDataStorageManager::start(){

	pageSize = dataStorageManagerInitParams.pageSize;
	wpSensorDataFolderSize= dataStorageManagerInitParams.wpSensorDataFolderSize;
	lifeCycleFolderSize= dataStorageManagerInitParams.lifeCycleFolderSize;
	discreteRecordFolderSize= dataStorageManagerInitParams.discreteRecordFolderSize;
	eventFolderSize= dataStorageManagerInitParams.eventFolderSize;
	rememberedValueFolderSize= dataStorageManagerInitParams.rememberedValueFolderSize;

}

boolean EEPROMDataStorageManager::readUntransferredFileFromSDCardByDate(int moveData, boolean sendToSerial,const char *dirName, int date, int month, int year){

}
boolean EEPROMDataStorageManager::readUntransferredFileFromSDCard(int moveData, boolean sendToSerial, const char *dirName){

}
void EEPROMDataStorageManager::storeRememberedValue(long time, const char *name, float value, String unit){

}
void EEPROMDataStorageManager::storeDiscreteRecord( DiscreteRecord &discreteRec){

}
boolean EEPROMDataStorageManager::readDiscreteRecord(uint16_t index,DiscreteRecord& rec){

}
boolean EEPROMDataStorageManager::openDiscreteRecordFile(){

}

void EEPROMDataStorageManager::closeDiscreteRecordFile(){

}
float EEPROMDataStorageManager::searchRememberedValue(const char *label, int date, int month, int year, char *whatToSearchFor){

}
void EEPROMDataStorageManager::storeLifeCycleEvent(long time, const char *eventType, int eventValue){

}
long EEPROMDataStorageManager::printDirectory(File dir, int numTabs){

}
long EEPROMDataStorageManager::getDiskUsage(){

}
long EEPROMDataStorageManager::getSDCardDiskUse(File dir ){

}
boolean EEPROMDataStorageManager::getHistoricalData(const char *dirName, int date, int month, int year){

}
void EEPROMDataStorageManager::saveWPSSensorRecord(WPSSensorRecord anWPSSensorRecord){

}

//
// Functions that represent Serial commands
//
boolean EEPROMDataStorageManager::testWPSSensor(float batteryVoltage, float current, int stateOfCharge, String operatingStatus){

}
float EEPROMDataStorageManager::listFiles(){

}
boolean EEPROMDataStorageManager::setTime(String){

}

void EEPROMDataStorageManager::writeEEPROM(int deviceaddress, unsigned int eeaddress, char* data)
{
	// Uses Page Write for 24LC256
	// Allows for page boundary defined in pageize
	// Splits string into max 16 byte writes
	unsigned char i=0, counter=0;
	unsigned int  address;
	unsigned int  page_space;
	unsigned int  page=0;
	unsigned int  num_writes;
	unsigned int  data_len=0;
	unsigned char first_write_size;
	unsigned char last_write_size;
	unsigned char write_size;

	// Calculate length of data
	do{ data_len++; } while(data[data_len]);

	// Calculate space available in first page
	page_space = int(((eeaddress/pageSize) + 1)*pageSize)-eeaddress;

	// Calculate first write size
	if (page_space>16){
		first_write_size=page_space-((page_space/16)*16);
		if (first_write_size==0) first_write_size=16;
	}
	else
		first_write_size=page_space;

	// calculate size of last write
	if (data_len>first_write_size)
		last_write_size = (data_len-first_write_size)%16;

	// Calculate how many writes we need
	if (data_len>first_write_size)
		num_writes = ((data_len-first_write_size)/16)+2;
	else
		num_writes = 1;

	i=0;
	address=eeaddress;
	for(page=0;page<num_writes;page++)
	{
		if(page==0) write_size=first_write_size;
		else if(page==(num_writes-1)) write_size=last_write_size;
		else write_size=16;

		Wire.beginTransmission(deviceaddress);
		Wire.write((int)((address) >> 8));   // MSB
		Wire.write((int)((address) & 0xFF)); // LSB
		counter=0;
		do{
			Wire.write((byte) data[i]);
			i++;
			counter++;
		} while((data[i]) && (counter<write_size));
		Wire.endTransmission();
		address+=write_size;   // Increment address for next write

		delay(6);  // needs 5ms for page write
	}
}

void EEPROMDataStorageManager::readEEPROM(int deviceaddress, unsigned int eeaddress,
		unsigned char* data, unsigned int num_chars)
{
	unsigned char i=0;
	Wire.beginTransmission(deviceaddress);
	Wire.write((int)(eeaddress >> 8));   // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();

	Wire.requestFrom(deviceaddress,num_chars);

	while(Wire.available()) data[i++] = Wire.read();

}

EEPROMDataStorageManager::~EEPROMDataStorageManager() {
	// TODO Auto-generated destructor stub
}

