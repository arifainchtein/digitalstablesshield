/*
 * FlowSensorNetworkManager.h
 *
 *  Created on: 7 Oct 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKMANAGER_H_
#include "Arduino.h"
#include <FlowMeterEventData.h>
#include <DataStorageManager.h>
#include <TimeManager.h>
#include <PowerManager.h>
#include <FlowMeter.h>
#include <FlowSensorNetworkConfigParams.h>

class FlowSensorNetworkManager {
	TimeManager & timeManager;
	DataStorageManager & dataStorageManager;

	int sampleFrequencySeconds;
	FlowSensorNetworkConfigParams & aFlowSensorNetworkConfigParams;



public:


	struct FlowSensorStatusStruct{
		  uint8_t meterId;
		  float currentFlow;
		  float currentVolume;

		};

	FlowSensorNetworkManager();
	FlowSensorNetworkManager(DataStorageManager & sd, TimeManager & t, FlowSensorNetworkConfigParams f);
	void begin();
	static void sensor_0();
	static void sensor_1();
	static void sensor_2();
	static void sensor_3();
	static void sensor_4();
	static void sensor_5();
	void setSampleFrequencySeconds(int s);
	bool updateValues();
	float getMeterCurrentFlow(uint8_t meterIndex);
	float getMeterCurrentVolume(uint8_t meterIndex);
//	void  pinMode(int port, byte state);
//	void digitalWrite(int port, byte state);



	virtual ~FlowSensorNetworkManager();

private:
	bool updateMeter( FlowMeter & meter,uint8_t meterId,  bool & meterInEvent, FlowMeterEventDataUnion & aFlowMeterEventDataUnion, uint8_t & currentSampleIndexMeter, bool dist);

};

#endif /* LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKMANAGER_H_ */
