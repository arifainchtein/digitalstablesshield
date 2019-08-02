/*
 * DataStorageManagerInitParams.h
 *
 *  Created on: 2 May 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_EEPROMDATASTORAGEMANAGERINITPARAMS_H_
#define LIBRARIES_EEPRMDATASTORAGEMANAGERINITPARAMS_H_

#include <DataStorageManagerInitParams.h>
struct EEPROMDataStorageManagerInitParams : public DataStorageManagerInitParams{
	uint8_t  pageSize;
	uint8_t wpSensorDataFolderSize;
	uint8_t lifeCycleFolderSize;
	uint8_t discreteRecordFolderSize;
	uint8_t eventFolderSize;
	uint8_t rememberedValueFolderSize;

};





#endif /* LIBRARIES_DATASTORAGEMANAGERINITPARAMS_H_ */
