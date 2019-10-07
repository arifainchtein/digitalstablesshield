/*
 * AquabubblerManager.h
 *
 *  Created on: 7 Oct 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_AQUABUBBLERMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_AQUABUBBLERMANAGER_H_

class AquabubblerManager {
public:
	AquabubblerManager();
	void begin();
	void updateValues();
	virtual ~AquabubblerManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_AQUABUBBLERMANAGER_H_ */
