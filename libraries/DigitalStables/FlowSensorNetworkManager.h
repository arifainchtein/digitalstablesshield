/*
 * FlowSensorNetworkManager.h
 *
 *  Created on: 7 Oct 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKMANAGER_H_

#include <FlowMeter.h>
#include <FlowMeterEventData.h>
#include <DataStorageManager.h>
#include <TimeManager.h>

class FlowSensorNetworkManager {
public:
	FlowSensorNetworkManager(DataStorageManager & sd, TimeManager & t, HardwareSerial& serial);
	void begin(uint8_t n, bool withDistributionPoint);
	void sensor_0();
	void sensor_1();
	void sensor_2();
	void sensor_3();
	void sensor_4();
	void sensor_10();
	void updateValues();

private:
	void updateMeter(FlowMeter & meter, bool & meterInEvent, FlowMeterEventData & aFlowMeterEventData, uint8_t & currentSampleIndexMeter, bool dist);
	virtual ~FlowSensorNetworkManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKMANAGER_H_ */
