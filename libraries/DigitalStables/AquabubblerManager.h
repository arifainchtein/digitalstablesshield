/*
 * AquabubblerManager.h
 *
 *  Created on: 7 Oct 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_AQUABUBBLERMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_AQUABUBBLERMANAGER_H_

#include <FlowMeter.h>

class AquabubblerManager {
public:
	AquabubblerManager();
	void begin();
	void sensor_0();
	void sensor_1();
	void sensor_2();
	void sensor_3();
	void sensor_4();
	void sensor_10();
	void updateValues();



	virtual ~AquabubblerManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_AQUABUBBLERMANAGER_H_ */
