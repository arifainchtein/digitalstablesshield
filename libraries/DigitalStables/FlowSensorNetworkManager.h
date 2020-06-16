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


class FlowSensorNetworkManager {
	PowerManager & powerManager;
	TimeManager & timeManager;
	DataStorageManager & dataStorageManager;
	HardwareSerial& serial;
	int sampleFrequencySeconds;

public:


	struct FlowSensorStatusStruct{
		  uint8_t meterId;
		  float currentFlow;
		  float currentVolume;

		};

	FlowSensorNetworkManager();
	FlowSensorNetworkManager(HardwareSerial& s, PowerManager& p,DataStorageManager & sd, TimeManager & t);
	void begin(uint8_t n, int sampleFrequencySeconds, bool withDistributionPoint);
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
