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
#include <LCDDisplay.h>
class GloriaHomeRF24Communicator {
public:
	LCDDisplay& lcd;
	GloriaHomeRF24Communicator(LCDDisplay & l);
	void start(GloriaCommData& commData);
	void scan( );
	bool publish(GloriaHomeBaseData& gloriaHomeBaseData  );
    bool receive(GloriaBaseData& gloriaBaseData,GloriaFlowData& gloriaFlowData);
	virtual ~GloriaHomeRF24Communicator();
};

#endif /* LIBRARIES_DIGITALSTABLES_GLORIAHOMERF24COMMUNICATOR_H_ */
