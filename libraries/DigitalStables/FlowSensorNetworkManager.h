/*
 * FlowSensorNetworkManager.h
 *
 *  Created on: 7 Oct 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKMANAGER_H_

#include <FlowMeterEventData.h>
#include <DataStorageManager.h>
#include <TimeManager.h>
#include <PowerManager.h>
#include <FlowMeter.h>


class FlowSensorNetworkManager {
	PowerManager & powerManager;
	TimeManager & timeManager;
	DataStorageManager & dataStorageManager;

public:
	FlowSensorNetworkManager();
	FlowSensorNetworkManager(PowerManager& p,DataStorageManager & sd, TimeManager & t);
	void begin(uint8_t n, bool withDistributionPoint);
	static void sensor_0();
	static void sensor_1();
	static void sensor_2();
	static void sensor_3();
	static void sensor_4();
	static void sensor_10();
	bool updateValues();
	float getMeterCurrentFlow(uint8_t meterIndex);
	float getMeterCurrentVolume(uint8_t meterIndex);
	virtual ~FlowSensorNetworkManager();

private:
	boolean updateMeter( FlowMeter & meter, bool & meterInEvent, FlowMeterEventData & aFlowMeterEventData, uint8_t & currentSampleIndexMeter, bool dist);

};

#endif /* LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKMANAGER_H_ */
