/*
 * Communicator.h
 *
 *  Created on: 17 Nov 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_COMMUNICATOR_H_
#define LIBRARIES_DIGITALSTABLES_COMMUNICATOR_H_
#include <CommunicatorInitParams.h>
class Communicator {
public:
	Communicator();
	virtual void start(CommunicatorInitParams p)=0;
	virtual void process()=0;

	virtual ~Communicator();
};

#endif /* LIBRARIES_DIGITALSTABLES_COMMUNICATOR_H_ */
