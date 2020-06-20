/*
 * FlowSensorNetworkConfigParams.h
 *
 *  Created on: 20 Jun 2020
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKCONFIGPARAMS_H_
#define LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKCONFIGPARAMS_H_
#include <Arduino.h>
#include <FlowMeterDefinition.h>

class FlowSensorNetworkConfigParams {
public:
	uint8_t numberSensors;
	int sampleFrequencySeconds=10;
	bool withDistributionPoint=false;
	bool withCentralWaste=false;

	FlowMeterDefinition & sensor_0;
	FlowMeterDefinition & sensor_1;
	FlowMeterDefinition & sensor_2;
	FlowMeterDefinition & sensor_3;
	FlowMeterDefinition & sensor_4;
	FlowMeterDefinition & sensor_5;

	FlowSensorNetworkConfigParams(uint8_t n,int s,bool wd,bool wc,
			FlowMeterDefinition& s0,FlowMeterDefinition & s1,FlowMeterDefinition & s2,FlowMeterDefinition & s3,FlowMeterDefinition & s4,FlowMeterDefinition & s5);
	virtual ~FlowSensorNetworkConfigParams();
};

#endif /* LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKCONFIGPARAMS_H_ */
