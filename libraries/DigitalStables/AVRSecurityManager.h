/*
 * AVRSecurityManager.h
 *
 *  Created on: 6 Dec 2021
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_AVRSECURITYMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_AVRSECURITYMANAGER_H_
#include <SecretManager.h>

class AVRSecurityManager  : public SecretManager{
public:
	AVRSecurityManager();
	void saveSecret(String secret, int numberDigits, int periodSeconds );
	String readSecret();


	virtual ~AVRSecurityManager();
};

#endif /* LIBRARIES_DIGITALSTABLES_AVRSECURITYMANAGER_H_ */
