/*
 * RF24Communicator.h
 *
 *  Created on: 17 Nov 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_GLORIARF24COMMUNICATOR_H_
#define LIBRARIES_DIGITALSTABLES_GLORIARF24COMMUNICATOR_H_

#include <Communicator.h>
#include <GloriaRF24CommunicatorInitParams.h>
#include <TelepathonData.h>
class GloriaRF24Communicator: public Communicator {
public:
	GloriaRF24Communicator();
	void start(RF24CommunicatorInitParams p);
	void scan( );
	void publish(TelepathonData& data[] );
    TelepathonData * receive();
	virtual ~GloriaRF24Communicator();
};

#endif /* LIBRARIES_DIGITALSTABLES_GLORIARF24COMMUNICATOR_H_ */
