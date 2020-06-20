/*
 * FlowSensorNetworkConfigParams.cpp
 *
 *  Created on: 20 Jun 2020
 *      Author: arifainchtein
 */
#include <Arduino.h>
#include <FlowSensorNetworkConfigParams.h>


FlowSensorNetworkConfigParams::FlowSensorNetworkConfigParams(uint8_t n, int s,bool wd,bool wc,
		FlowMeterDefinition &s0,FlowMeterDefinition & s1,FlowMeterDefinition & s2,FlowMeterDefinition & s3,FlowMeterDefinition & s4,FlowMeterDefinition & s5):
		 numberSensors(n),sampleFrequencySeconds(s),withDistributionPoint(wd),withCentralWaste(wc),
		 sensor_0(s0),sensor_1(s1),sensor_2(s2),sensor_3(s3),sensor_4(s4),sensor_5(s5){

}




FlowSensorNetworkConfigParams::~FlowSensorNetworkConfigParams() {
	// TODO Auto-generated destructor stub
}

