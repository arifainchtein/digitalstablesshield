/*
  SecretManager.h - Library Managing the TOTP
  Created by Ari Fainchtein, Feb 13, 2019.
  Released into the public domain.
*/
#ifndef SecretManager_h
#define SecretManager_h
#include "Arduino.h"
#include <TimeManager.h>


class SecretManager{
		TimeManager & timeManager;



	public:
		SecretManager(TimeManager & t) ;
		virtual void saveSecret(String secret, int numberDigits, int periodSeconds )=0;
		virtual String readSecret()=0;
		long generateCode();
		boolean checkCode(long userCode);
		static constexpr int SHARED_SECRET_LENGTH=27;

	private:

};

#endif
