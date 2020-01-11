/*
 * RF24Communicator.h
 *
 *  Created on: 17 Nov 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_RF24COMMUNICATOR_H_
#define LIBRARIES_DIGITALSTABLES_RF24COMMUNICATOR_H_

#include <Communicator.h>
#include <RF24CommunicatorInitParams.h>
class RF24Communicator: public Communicator {
public:
	RF24Communicator();
	void start(RF24CommunicatorInitParams p);
	void scan( );
	virtual ~RF24Communicator();
};

#endif /* LIBRARIES_DIGITALSTABLES_RF24COMMUNICATOR_H_ */
