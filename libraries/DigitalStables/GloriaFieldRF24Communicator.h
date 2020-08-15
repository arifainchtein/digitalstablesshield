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
	bool publish(GloriaBaseData& gloriaBaseData,GloriaFlowData& gloriaFlowData);
	virtual ~GloriaFieldRF24Communicator();

private:
	bool sendBase (const GloriaBaseData& data);
	bool sendFlow (const GloriaFlowData& flowData);
};

#endif /* LIBRARIES_DIGITALSTABLES_GLORIAFIELDRF24COMMUNICATOR_H_ */
