/*
 * GloriaFieldRF24Communicator.h
 *
 *  Created on: 22 Jun 2020
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_GLORIAFIELDRF24COMMUNICATOR_H_
#define LIBRARIES_DIGITALSTABLES_GLORIAFIELDRF24COMMUNICATOR_H_
#include <Communicator.h>
#include <Arduino.h>
#include <GloriaData.h>
#include <LCDDisplay.h>
#include <TimeManager.h>

class GloriaFieldRF24Communicator {


public:

	LCDDisplay & lcd;
	TimeManager &  timeManager;


	GloriaFieldRF24Communicator( LCDDisplay & l ,  TimeManager & t);

	void start(GloriaCommData& commData);
	void scan( );
	bool receive(GloriaHomeBaseData& gloriaHomeBaseData  );
	virtual ~GloriaFieldRF24Communicator();

	bool sendPowerData (const GloriaFieldPowerData& data, HardwareSerial& serial);
	bool sendFlowData (const GloriaFieldFlowData& flowData);
	bool sendSensorData (const GloriaFieldSensorData& sensorData);
};

#endif /* LIBRARIES_DIGITALSTABLES_GLORIAFIELDRF24COMMUNICATOR_H_ */
