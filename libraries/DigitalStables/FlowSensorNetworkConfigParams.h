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
	FlowMeterDefinition * sensor_1;
	FlowMeterDefinition * sensor_2;
	FlowMeterDefinition * sensor_3;
	FlowMeterDefinition * sensor_4;
	FlowMeterDefinition * sensor_5;

	FlowSensorNetworkConfigParams(uint8_t n,int s,bool wd,bool wc,
			FlowMeterDefinition& s0,FlowMeterDefinition * s1=nullptr,FlowMeterDefinition* s2=nullptr,FlowMeterDefinition * s3=nullptr,FlowMeterDefinition * s4=nullptr,FlowMeterDefinition * s5=nullptr);
	virtual ~FlowSensorNetworkConfigParams();
};

#endif /* LIBRARIES_DIGITALSTABLES_FLOWSENSORNETWORKCONFIGPARAMS_H_ */
