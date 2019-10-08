/*
 * FlowSensorNetworkManager.cpp
 * This code must be generated by the spermatogenesis process
 *  Created on: 7 Oct 2019
 *      Author: arifainchtein
 */
#include <FlowSensorNetworkManager.h>
#include "Arduino.h"



// connect a flow meter to an interrupt pin (see notes on your Arduino model for pin numbers)

TimeManager&  timeManager;
DataStorageManager&  dataStorageManager;
HardwareSerial& _HardSerial;

const unsigned long period = 1000;
bool meter0InEvent = false;
bool meter1InEvent = false;
bool meter2InEvent = false;
bool meter3InEvent = false;
bool meter4InEvent = false;
bool meter10InEvent = false;

uint8_t currentSampleIndexMeter0=-1;
uint8_t currentSampleIndexMeter1=-1;
uint8_t currentSampleIndexMeter2=-1;
uint8_t currentSampleIndexMeter3=-1;
uint8_t currentSampleIndexMeter4=-1;
uint8_t currentSampleIndexMeter5=-1;

long currentMeter0StartTime=0L;

#define flow_0 9
#define flow_1 8
#define flow_2 7
#define flow_3 6
#define flow_4 46
#define flow_10 45

FlowMeter Meter0 = FlowMeter(flow_0);
FlowMeter* Meter1 = nullptr;
FlowMeter* Meter2 = nullptr;
FlowMeter* Meter3 = nullptr;
FlowMeter* Meter4 = nullptr;
FlowMeter* Meter5 = nullptr;

FlowMeterEventData aFlowMeter0EventData;
FlowMeterEventData aFlowMeter1EventData;
FlowMeterEventData aFlowMeter2EventData;
FlowMeterEventData aFlowMeter3EventData;
FlowMeterEventData aFlowMeter4EventData;
FlowMeterEventData aFlowMeter5EventData;
bool withDistributionPoint=false;

FlowSensorNetworkManager::FlowSensorNetworkManager( DataStorageManager& sd, TimeManager& t, HardwareSerial& serial ):  dataStorageManager(sd),timeManager(t), _HardSerial(serial)
{}




void FlowSensorNetworkManager::begin(uint8_t numberOfWaterPoints, bool distr) {
	withDistributionPoint=distr;
	pinMode(flow_0, INPUT);
	attachInterrupt(digitalPinToInterrupt(flow_0), sensor_0, LOW);

	if(numberOfWaterPoints>1){
		Meter1 = new FlowMeter(flow_1);
		pinMode(flow_1, INPUT);
		attachInterrupt(digitalPinToInterrupt(flow_1), sensor_1, LOW);
	}

	if(numberOfWaterPoints>2){
		 Meter2 = new FlowMeter(flow_2);
		pinMode(flow_2, INPUT);
		 attachInterrupt(digitalPinToInterrupt(flow_2), sensor_2, LOW);

	}
	if(numberOfWaterPoints>3){
		Meter3 = new FlowMeter(flow_3);
		pinMode(flow_3, INPUT);
		attachInterrupt(digitalPinToInterrupt(flow_3), sensor_3, LOW);

	}
	if(numberOfWaterPoints>4){
		 Meter4 = new FlowMeter(flow_4);
		pinMode(flow_4, INPUT);;
		attachInterrupt(digitalPinToInterrupt(flow_4), sensor_4, LOW);

	}
	if(numberOfWaterPoints>5){
		Meter5 = new FlowMeter(flow_10);
		pinMode(flow_10, INPUT);
		attachInterrupt(digitalPinToInterrupt(flow_10), sensor_10, LOW);
	}
}

void FlowSensorNetworkManager::updateValues(){
	updateMeter(Meter0, meter0InEvent, aFlowMeter0EventData,currentSampleIndexMeter0, withDistributionPoint);
	if(Meter1 != nullptr)updateMeter(*Meter1, meter1InEvent, aFlowMeter1EventData,currentSampleIndexMeter1, false);
	if(Meter2 != nullptr)updateMeter(*Meter2, meter2InEvent, aFlowMeter2EventData,currentSampleIndexMeter2, false);
	if(Meter3 != nullptr)updateMeter(*Meter3, meter3InEvent, aFlowMeter3EventData, currentSampleIndexMeter3, false);
	if(Meter4 != nullptr)updateMeter(*Meter4, meter4InEvent, aFlowMeter4EventData, currentSampleIndexMeter4, false);
	if(Meter5 != nullptr)updateMeter(*Meter5, meter10InEvent, aFlowMeter5EventData, currentSampleIndexMeter5, false);

}
float FlowSensorNetworkManager::getMeterCurrentFlow(uint8_t meterIndex){

	float flowRate = 0.0;

	switch(meterIndex){

		case 0:
			flowRate = Meter0.getCurrentFlowrate();
			break;
		case 1:
			if(Meter1 != nullptr)flowRate = Meter1->getCurrentFlowrate();
			break;
		case 2:
			if(Meter2 != nullptr)flowRate = Meter2->getCurrentFlowrate();
			break;
		case 3:
			if(Meter3 != nullptr)flowRate = Meter3->getCurrentFlowrate();
			break;
		case 4:
			if(Meter4 != nullptr)flowRate = Meter4->getCurrentFlowrate();
			break;
		case 10:
			if(Meter5 != nullptr)flowRate = Meter5->getCurrentFlowrate();
			break;

		default:
			flowRate=0.0;
			break;
	}
	return flowRate;
}


float FlowSensorNetworkManager::getMeterCurrentVolume(uint8_t meterIndex){

	float currentVolume = 0.0;

	switch(meterIndex){

		case 0:
			currentVolume = Meter0.getCurrentFlowrate();
			break;
		case 1:
			if(Meter1 != nullptr)currentVolume = Meter1->getCurrentVolume();
			break;
		case 2:
			if(Meter2 != nullptr)currentVolume = Meter2->getCurrentVolume();
			break;
		case 3:
			if(Meter3 != nullptr)currentVolume = Meter3->getCurrentVolume();
			break;
		case 4:
			if(Meter4 != nullptr)currentVolume = Meter4->getCurrentVolume();
			break;
		case 10:
			if(Meter5 != nullptr)currentVolume = Meter5->getCurrentVolume();
			break;

		default:
			currentVolume=0.0;
			break;
	}
	return currentVolume;
}

void FlowSensorNetworkManager::updateMeter(FlowMeter & meter, bool & meterInEvent, FlowMeterEventData & aFlowMeterEventData, uint8_t & currentSampleIndexMeter, bool dist){

	//
	// if water is not running in any of the meters
	// then if the event is going, close the event
	meter.tick(period);
	if(meter.getCurrentFrequency()>0){
		long currentTime = timeManager.getCurrentTimeInSeconds();

		if(!meterInEvent){
			//
			// if we are here it means that the flow meter
			// just detected a new starting event
			meterInEvent=true;
			aFlowMeterEventData.startTime = currentTime;
		}
		currentSampleIndexMeter++;
		float flowRate = meter.getCurrentFlowrate();


		aFlowMeterEventData.flowMeterId=0;
		if(dist){
			currentMeter0StartTime=currentTime;
			aFlowMeterEventData.eventGroupStartTime=currentMeter0StartTime;
		}else{
			aFlowMeterEventData.eventGroupStartTime=currentTime;
		}


		aFlowMeterEventData.totalVolume+=meter.getCurrentVolume();
		aFlowMeterEventData.samples[currentSampleIndexMeter].sampleTime=currentTime;
		aFlowMeterEventData.samples[currentSampleIndexMeter].flow=flowRate;

	}else{
		if(meterInEvent){
			//
			// if we are here it means that there is no
			// water flowing through the meter now
			// but since we were in an event
			// it means that event is finished so
			// create a aFlowMeterEventData and store it
			//
			// since the last time check,
			// this means that the event is finished
			//
			aFlowMeterEventData.endTime=timeManager.getCurrentTimeInSeconds();
			aFlowMeterEventData.averageflow
			aFlowMeterEventData.numberOfSamples=currentSampleIndexMeter+1;
			aFlowMeterEventData.sampleFrequencySeconds
			meter0InEvent=false;
			//sdCardSaveRecord(aFlowMeterEventData);
		}else{
			//
			// if we are here it means that the current flow is 0
			// and we are not in an event, this means that
			// the flow meter is idle
			aFlowMeterEventData.reset();
			currentSampleIndexMeter=-1;
			currentMeter0StartTime=0;
			meter.reset();
		}
	}
}


static void FlowSensorNetworkManager::sensor_0(){
	 Meter0.count();
}

static void FlowSensorNetworkManager::sensor_1(){
	if(Meter1 != nullptr)Meter1->count();;
}

static void FlowSensorNetworkManager::sensor_2(){
	if(Meter2 != nullptr)Meter2->count();
}

static void FlowSensorNetworkManager::sensor_3(){
	if(Meter3 != nullptr)Meter3->count();
}

static void FlowSensorNetworkManager::sensor_4(){
	if(Meter4 != nullptr)Meter4->count();
}

static void FlowSensorNetworkManager::sensor_10(){

	if(Meter5 != nullptr)Meter5->count();
}



FlowSensorNetworkManager::~FlowSensorNetworkManager() {
	// TODO Auto-generated destructor stub
}

