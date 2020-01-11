/*
 * XBeeCommunicator.h
 *
 *  Created on: 17 Nov 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_XBEECOMMUNICATOR_H_
#define LIBRARIES_DIGITALSTABLES_XBEECOMMUNICATOR_H_

#include <Communicator.h>
#include <XBee.h>

class XBeeCommunicator: public Communicator {
public:
	XBeeCommunicator();
	void start();
	virtual ~XBeeCommunicator();
};

#endif /* LIBRARIES_DIGITALSTABLES_XBEECOMMUNICATOR_H_ */
