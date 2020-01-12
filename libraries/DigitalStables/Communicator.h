/*
 * Communicator.h
 *
 *  Created on: 17 Nov 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_COMMUNICATOR_H_
#define LIBRARIES_DIGITALSTABLES_COMMUNICATOR_H_
#include <CommunicatorInitParams.h>
#include <TelepathonData.h>

class Communicator {

public:

	struct TelepathonData{

	};

	Communicator();
	virtual void start(CommunicatorInitParams p)=0;
	virtual void process()=0;
	virtual void publish(TelepathonData& data )=0;
	virtual TelepathonData * receive()=0;
	virtual ~Communicator();
};

#endif /* LIBRARIES_DIGITALSTABLES_COMMUNICATOR_H_ */
