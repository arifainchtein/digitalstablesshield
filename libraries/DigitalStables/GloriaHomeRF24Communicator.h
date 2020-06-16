/*
 * RF24Communicator.h
 *
 *  Created on: 17 Nov 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_GLORIAHOMERF24COMMUNICATOR_H_
#define LIBRARIES_DIGITALSTABLES_GLORIAHOMERF24COMMUNICATOR_H_

#include <Communicator.h>
#include <GloriaData.h>

class GloriaHomeRF24Communicator {
public:
	HardwareSerial& _HardSerial;

	GloriaHomeRF24Communicator(HardwareSerial& serial);
	void start(GloriaCommData& commData);
	void scan( );
	bool publish(GloriaHomeBaseData& gloriaHomeBaseData  );
    bool receive(GloriaBaseData& gloriaBaseData,GloriaFlowData& gloriaFlowData);
	virtual ~GloriaHomeRF24Communicator();
};

#endif /* LIBRARIES_DIGITALSTABLES_GLORIAHOMERF24COMMUNICATOR_H_ */
