/*
 * FlowSensorNetworkManager.cpp
 * This code must be generated by the spermatogenesis process
 *  Created on: 7 Oct 2019
 *      Author: arifainchtein
 */
#include "Arduino.h"
#include <FlowSensorNetworkManager.h>



// connect a flow meter to an interrupt pin (see notes on your Arduino model for pin numbers)
#define mascara_pin 0x00ff
#define PA  0x0000
#define PB  0x0100
#define PC  0x0200
#define PD  0x0300
#define PE  0x0400
#define PF  0x0500
#define PL  0x0600
#define PK  0x0700
#define PJ  0x0800
#define PH  0x0900
#define PG  0x1000

#define mascara_puerto  0xff00


const unsigned long period = 1000;
bool meter0InEvent = false;
bool meter1InEvent = false;
bool meter2InEvent = false;
bool meter3InEvent = false;
bool meter4InEvent = false;
bool meter5InEvent = false;

uint8_t currentSampleIndexMeter0=-1;
uint8_t currentSampleIndexMeter1=-1;
uint8_t currentSampleIndexMeter2=-1;
uint8_t currentSampleIndexMeter3=-1;
uint8_t currentSampleIndexMeter4=-1;
uint8_t currentSampleIndexMeter5=-1;

long currentMeterStartTime=0L;
//
// the flow variables represent the actual interrupt not the pin
// so flow_0 is interrupt 0 which in the 1284p is in pin 10
// so flow_2 is interrupt 2 which in the 1284p is in pin 2
//
//uint8_t flow_0=0;
//uint8_t flow_1=0;
//uint8_t flow_2=0;
//uint8_t flow_3=0;
//uint8_t flow_4=0;
//uint8_t flow_5=0;

FlowMeter Meter0;// = nullptr;
FlowMeter Meter1;// = nullptr;
FlowMeter Meter2;// = nullptr;
FlowMeter Meter3;// = nullptr;
FlowMeter Meter4;// = nullptr;
FlowMeter Meter5;// = nullptr;

FlowMeterEventDataUnion aFlowMeter0EventDataUnion;
FlowMeterEventDataUnion aFlowMeter1EventDataUnion;
FlowMeterEventDataUnion aFlowMeter2EventDataUnion;
FlowMeterEventDataUnion aFlowMeter3EventDataUnion;
FlowMeterEventDataUnion aFlowMeter4EventDataUnion;
FlowMeterEventDataUnion aFlowMeter5EventDataUnion;

bool withDistributionPoint=false;
bool canPublishAsync=false;



FlowSensorNetworkManager::FlowSensorNetworkManager(DataStorageManager& sd, TimeManager& t, FlowSensorNetworkConfigParams f ):
				 dataStorageManager(sd),timeManager(t), aFlowSensorNetworkConfigParams(f)
{}




void FlowSensorNetworkManager::begin() {
	//	//    //interrupciones
	//			  //    /*Table 15-3. Interrupt Sense Control(1)
	//			  //      ISCn1 ISCn0 Description
	//			  //      0 0 The low level of INTn generates an interrupt request
	//			  //      0 1 Any logical change on INTn generates an interrupt request
	//			  //      1 0 The falling edge between two samples of INTn generates an interrupt request
	//			  //      1 1 The rising edge between two samples of INTn generates an interrupt request
	//			  //    */
	//			      EICRA = (1 << ISC31) | (1 << ISC21);
	//			      EICRB = (1 << ISC71) | (1 << ISC61) | (1 << ISC51) | (1 << ISC41);
	//			      EIMSK = (1 << INT7) | (1 << INT6) | (1 << INT5) | (1 << INT4) | (1 << INT3) | (1 << INT2);
	//
	//			  //
	//			  //    ///////// Comunication configuration
	//			  //    //seria
	uint8_t numberOfWaterPoints=aFlowSensorNetworkConfigParams.numberSensors;
	sampleFrequencySeconds=aFlowSensorNetworkConfigParams.sampleFrequencySeconds;
	withDistributionPoint=aFlowSensorNetworkConfigParams.withDistributionPoint;
	Meter0 = FlowMeter(aFlowSensorNetworkConfigParams.sensor_0.interruptNumber);
	pinMode(aFlowSensorNetworkConfigParams.sensor_0.interruptNumber, INPUT);
	attachInterrupt(digitalPinToInterrupt(aFlowSensorNetworkConfigParams.sensor_0.interruptNumber), sensor_0, RISING);

	if(numberOfWaterPoints>1){
		uint8_t flow_1 = aFlowSensorNetworkConfigParams.sensor_1.interruptNumber;
		Meter1 = new FlowMeter(flow_1);
		pinMode(flow_1, INPUT);
		//attachInterrupt(digitalPinToInterrupt(flow_1), sensor_1, RISING);
		attachInterrupt(flow_1, sensor_1, RISING);
		Meter1.reset();
	}

	if(numberOfWaterPoints>2){
		unsigned int flow_2 = aFlowSensorNetworkConfigParams.sensor_2.interruptNumber;
		Meter2 = new FlowMeter(flow_2);
		pinMode(flow_2, INPUT);
		// attachInterrupt(digitalPinToInterrupt(flow_2), sensor_2, RISING);
		attachInterrupt(flow_2, sensor_2, RISING);
		Meter2.reset();

	}
	if(numberOfWaterPoints>3){
		uint8_t flow_3 = aFlowSensorNetworkConfigParams.sensor_3.interruptNumber;
		Meter3 = new FlowMeter(flow_3);
		pinMode(flow_3, INPUT);
		//attachInterrupt(digitalPinToInterrupt(flow_3), sensor_3, RISING);
		attachInterrupt(flow_3, sensor_3, RISING);
		Meter3.reset();

	}
	if(numberOfWaterPoints>4){
		uint8_t flow_4 = aFlowSensorNetworkConfigParams.sensor_4.interruptNumber;
		Meter4 = new FlowMeter(flow_4);
		pinMode(18, INPUT);;
		//		attachInterrupt(INT3,sensor_4, RISING);
		//	attachInterrupt(digitalPinToInterrupt(18), sensor_10, RISING);
		attachInterrupt(flow_4, sensor_4, RISING);
		Meter4.reset();

	}
	if(numberOfWaterPoints>5){
		uint8_t flow_5 = aFlowSensorNetworkConfigParams.sensor_5.interruptNumber;
		Meter5 = new FlowMeter(flow_5);
		pinMode(flow_5, INPUT);
		//	attachInterrupt(digitalPinToInterrupt(19), sensor_10, RISING);
		attachInterrupt(flow_5, sensor_5, RISING);
		Meter5.reset();
	}




}

void FlowSensorNetworkManager::setSampleFrequencySeconds(int s){
	sampleFrequencySeconds=s;
}
bool FlowSensorNetworkManager::updateValues(){
	bool meter0Status=false;
	bool meter1Status=false;
	bool meter2Status=false;
	bool meter3Status=false;
	bool meter4Status=false;
	bool meter5Status=false;
	//serial.println("in updatevalues flowsensor");
	meter0Status = updateMeter(Meter0, 0,meter0InEvent, aFlowMeter0EventDataUnion,currentSampleIndexMeter0, withDistributionPoint);
	if(aFlowSensorNetworkConfigParams.sensor_1.interruptNumber != FlowMeterDefinition::NOT_USED)meter1Status=updateMeter(Meter1,1, meter1InEvent, aFlowMeter1EventDataUnion,currentSampleIndexMeter1, false);


	if(aFlowSensorNetworkConfigParams.sensor_1.interruptNumber != FlowMeterDefinition::NOT_USED)meter1Status=updateMeter(Meter1,1, meter1InEvent, aFlowMeter1EventDataUnion,currentSampleIndexMeter1, false);
	if(aFlowSensorNetworkConfigParams.sensor_2.interruptNumber != FlowMeterDefinition::NOT_USED)meter1Status=updateMeter(Meter2,1, meter2InEvent, aFlowMeter2EventDataUnion,currentSampleIndexMeter2, false);
	if(aFlowSensorNetworkConfigParams.sensor_3.interruptNumber != FlowMeterDefinition::NOT_USED)meter1Status=updateMeter(Meter3,1, meter3InEvent, aFlowMeter3EventDataUnion,currentSampleIndexMeter3, false);
	if(aFlowSensorNetworkConfigParams.sensor_4.interruptNumber != FlowMeterDefinition::NOT_USED)meter1Status=updateMeter(Meter4,1, meter4InEvent, aFlowMeter4EventDataUnion,currentSampleIndexMeter4, false);
	if(aFlowSensorNetworkConfigParams.sensor_5.interruptNumber != FlowMeterDefinition::NOT_USED)meter1Status=updateMeter(Meter5,1, meter5InEvent, aFlowMeter5EventDataUnion,currentSampleIndexMeter4, false);


	bool ret =  meter0Status || meter1Status || meter2Status|| meter3Status || meter4Status || meter5Status;
	//serial.print("in  flowsensor, ret=");
	//	serial.println(ret);
	return ret;
}






bool FlowSensorNetworkManager::updateMeter(FlowMeter & meter, uint8_t meterId,bool & meterInEvent, FlowMeterEventDataUnion & aFlowMeterEventDataUnion, uint8_t & currentSampleIndexMeter, bool dist){

	//
	// if water is not running
	// then if the event is going, close the event
	bool toReturn=false;
	meter.tick(period);
	//	serial.print("meter freq=");
	//    serial.println(meter.getCurrentFrequency());

	if(meter.getCurrentFrequency()>0){
		long currentTime = timeManager.getCurrentTimeInSeconds();
		toReturn=true;
		float flowRate = meter.getCurrentFlowrate();
		//		serial.print("flowRate=");
		//		serial.print(flowRate);
		//		serial.print(" meterInEvent=");
		//	    serial.println(meterInEvent);
		if(!meterInEvent){
			//
			// if we are here it means that the flow meter
			// just detected a new starting event
			meterInEvent=true;
			aFlowMeterEventDataUnion.aFlowMeterEventData.startTime = currentTime;
			aFlowMeterEventDataUnion.aFlowMeterEventData.flowMeterId=meterId;
			if(dist){
				currentMeterStartTime=currentTime;
				aFlowMeterEventDataUnion.aFlowMeterEventData.eventGroupStartTime=currentMeterStartTime;
			}else{
				aFlowMeterEventDataUnion.aFlowMeterEventData.eventGroupStartTime=currentTime;
			}
		}else{
			//
			// if we are here it means that we are in the
			// middle of an event


		}
		currentSampleIndexMeter++;

		//serial.print("flow rate=");
		//serial.println(flowRate);

		aFlowMeterEventDataUnion.aFlowMeterEventData.totalVolume+=meter.getCurrentVolume();
		//serial.print("volume=");
		//serial.println(aFlowMeterEventDataUnion.aFlowMeterEventData.totalVolume);




		//aFlowMeterEventDataUnion.aFlowMeterEventData.samples[currentSampleIndexMeter].sampleTime=currentTime;
		//aFlowMeterEventDataUnion.aFlowMeterEventData.samples[currentSampleIndexMeter].flow=flowRate;

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
			aFlowMeterEventDataUnion.aFlowMeterEventData.endTime=timeManager.getCurrentTimeInSeconds();
			aFlowMeterEventDataUnion.aFlowMeterEventData.averageflow=meter.getTotalFlowrate();
			//	aFlowMeterEventDataUnion.aFlowMeterEventData.numberOfSamples=currentSampleIndexMeter+1;
			aFlowMeterEventDataUnion.aFlowMeterEventData.sampleFrequencySeconds=sampleFrequencySeconds;
			meterInEvent=false;
			//
			// now ask the powermanager for permission to transmit
			//
			const byte* eventData = reinterpret_cast<const byte*>(&aFlowMeterEventDataUnion.aFlowMeterEventData);
			const char  *flowMeterEventUnstraferedFileName ="FMEUF";
			const char  *EventsDirName="Events";
			dataStorageManager.openEventRecordFile(flowMeterEventUnstraferedFileName, sizeof(aFlowMeterEventDataUnion.aFlowMeterEventData));
			bool savedOk = dataStorageManager.storeEventRecord(EventsDirName,flowMeterEventUnstraferedFileName, eventData, sizeof(aFlowMeterEventDataUnion.aFlowMeterEventData) );

//			serial.print("saving flow event with averageflow=");
//			serial.println(aFlowMeterEventDataUnion.aFlowMeterEventData.averageflow);
//			//			serial.print(" savedOk=");
			//			serial.println(savedOk);

			aFlowMeterEventDataUnion.aFlowMeterEventData.reset();
			currentSampleIndexMeter=-1;
			currentMeterStartTime=0;
			meter.reset();
		}else{
			//
			// if we are here it means that the current flow is 0
			// and we are not in an event, this means that
			// the flow meter is idle
			aFlowMeterEventDataUnion.aFlowMeterEventData.reset();
			currentSampleIndexMeter=-1;
			currentMeterStartTime=0;
			meter.reset();
		}
	}
	return toReturn;
}

float FlowSensorNetworkManager::getMeterCurrentFlow(uint8_t meterIndex){

	float flowRate = 0.0;

	switch(meterIndex){

	case 0:
		flowRate = Meter0.getCurrentFlowrate();
		break;
	case 1:
		if(aFlowSensorNetworkConfigParams.sensor_1.interruptNumber != FlowMeterDefinition::NOT_USED)flowRate = Meter1.getCurrentFlowrate();
		break;
	case 2:
		if(aFlowSensorNetworkConfigParams.sensor_2.interruptNumber != FlowMeterDefinition::NOT_USED)flowRate = Meter2.getCurrentFlowrate();
		break;
	case 3:
		if(aFlowSensorNetworkConfigParams.sensor_3.interruptNumber != FlowMeterDefinition::NOT_USED)flowRate = Meter3.getCurrentFlowrate();
		break;
	case 4:
		if(aFlowSensorNetworkConfigParams.sensor_4.interruptNumber != FlowMeterDefinition::NOT_USED)flowRate = Meter4.getCurrentFlowrate();
		break;
	case 10:
		if(aFlowSensorNetworkConfigParams.sensor_5.interruptNumber != FlowMeterDefinition::NOT_USED)flowRate = Meter5.getCurrentFlowrate();
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
		if(aFlowSensorNetworkConfigParams.sensor_1.interruptNumber != FlowMeterDefinition::NOT_USED)currentVolume = Meter1.getCurrentVolume();
		break;
	case 2:
		if(aFlowSensorNetworkConfigParams.sensor_2.interruptNumber != FlowMeterDefinition::NOT_USED)currentVolume = Meter2.getCurrentVolume();
		break;
	case 3:
		if(aFlowSensorNetworkConfigParams.sensor_3.interruptNumber != FlowMeterDefinition::NOT_USED)currentVolume = Meter3.getCurrentVolume();
		break;
	case 4:
		if(aFlowSensorNetworkConfigParams.sensor_4.interruptNumber != FlowMeterDefinition::NOT_USED)currentVolume = Meter4.getCurrentVolume();
		break;
	case 10:
		if(aFlowSensorNetworkConfigParams.sensor_5.interruptNumber != FlowMeterDefinition::NOT_USED)currentVolume = Meter5.getCurrentVolume();
		break;

	default:
		currentVolume=0.0;
		break;
	}
	return currentVolume;
}

void  FlowSensorNetworkManager::sensor_0(){
	Meter0.count();
}

void FlowSensorNetworkManager::sensor_1(){
	if(aFlowSensorNetworkConfigParams.sensor_1.interruptNumber != FlowMeterDefinition::NOT_USED)Meter1.count();
}

void FlowSensorNetworkManager::sensor_2(){
	if(aFlowSensorNetworkConfigParams.sensor_2.interruptNumber != FlowMeterDefinition::NOT_USED)Meter2.count();
}

void FlowSensorNetworkManager::sensor_3(){
	if(aFlowSensorNetworkConfigParams.sensor_3.interruptNumber != FlowMeterDefinition::NOT_USED)Meter3.count();
}

void FlowSensorNetworkManager::sensor_4(){
	if(aFlowSensorNetworkConfigParams.sensor_4.interruptNumber != FlowMeterDefinition::NOT_USED)Meter4.count();
}

void FlowSensorNetworkManager::sensor_5(){

	if(aFlowSensorNetworkConfigParams.sensor_5.interruptNumber != FlowMeterDefinition::NOT_USED)Meter5.count();
}



FlowSensorNetworkManager::~FlowSensorNetworkManager() {
	// TODO Auto-generated destructor stub
}

