/*
 * Esp32SecretManager.h
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_ESP32SECRETMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_ESP32SECRETMANAGER_H_
#include <SecretManager.h>
#include <TimeManager.h>

class Esp32SecretManager   : public SecretManager{
	TimeManager & timeManager;

public:

	Esp32SecretManager(TimeManager & t) ;
	void saveSecret(String secret, int numberDigits, int periodSeconds );
	void readSecret(char *secretCode);
	String readSecret();

	virtual ~Esp32SecretManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_ESP32SECRETMANAGER_H_ */
