/*
 * RF24Communicator.h
 *
 *  Created on: 17 Nov 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_GLORIARF24COMMUNICATOR_H_
#define LIBRARIES_DIGITALSTABLES_GLORIARF24COMMUNICATOR_H_

#include <Communicator.h>
#include <TelepathonData.h>
class GloriaRF24Communicator: public Communicator {
public:
	GloriaRF24Communicator();
	void start();
	void scan( );
	bool publish(const TelepathonData& data );
    bool receive(TelepathonData& data);
	virtual ~GloriaRF24Communicator();
};

#endif /* LIBRARIES_DIGITALSTABLES_GLORIARF24COMMUNICATOR_H_ */
