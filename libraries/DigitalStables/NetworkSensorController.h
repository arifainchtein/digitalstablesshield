/*
 * NetworkSensorController.h
 *
 *  Created on: 8 Aug 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_NETWORKSENSORCONTROLLER_H_
#define LIBRARIES_DIGITALSTABLES_NETWORKSENSORCONTROLLER_H_

class NetworkSensorController {
public:
	NetworkSensorController();
	void start(unsigned char qosp);
	void checkStatus();
	virtual ~NetworkSensorController();
};

#endif /* LIBRARIES_DIGITALSTABLES_NETWORKSENSORCONTROLLER_H_ */
