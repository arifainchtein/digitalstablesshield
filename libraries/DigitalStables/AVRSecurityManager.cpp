/*
 * AVRSecurityManager.cpp
 *
 *  Created on: 6 Dec 2021
 *      Author: arifainchtein
 */

#include <AVRSecurityManager.h>
#include <EEPROM.h>

AVRSecurityManager::AVRSecurityManager() {
	// TODO Auto-generated constructor stub

}


void AVRSecurityManager::saveSecret(String secret, int numberDigits, int periodSeconds ){
	char someCharArray[SHARED_SECRET_LENGTH];
	secret.toCharArray(someCharArray,SHARED_SECRET_LENGTH);

	for(int i=0;i<SHARED_SECRET_LENGTH ;i++){
		EEPROM.write(i, someCharArray[i]);
	}

	EEPROM.write(SHARED_SECRET_LENGTH, numberDigits);
	EEPROM.write(SHARED_SECRET_LENGTH+1, periodSeconds);
}


String AVRSecurityManager::readSecret(){
	char *secretCode[SHARED_SECRET_LENGTH];
	if ( EEPROM.read ( 0 ) != 0xff ){
		for (int i = 0; i < SHARED_SECRET_LENGTH; ++i ){
			secretCode [ i ] = EEPROM.read ( i );
		}
	}
	return secretCode;
}

AVRSecurityManager::~AVRSecurityManager() {
	// TODO Auto-generated destructor stub
}

