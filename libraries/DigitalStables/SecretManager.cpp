#include "Arduino.h"

#include <totp.h>
#include <TimeManager.h>
#include <SecretManager.h>


char code[7];
int currentCommandCodeHistoryPos=0;
int numberOfCommandCodesInHistory=5;
long commandCodeHistory[5]={999999,999999,999999,99999,99999};

SecretManager::SecretManager(TimeManager& t): timeManager(t){

}



long SecretManager::generateCode(){



	long timestamp = timeManager.getTimeForCodeGeneration();

	char secretCode[SHARED_SECRET_LENGTH];
	String secret = readSecret();
	TOTP totp = TOTP(secret);
	long code=totp. gen_code  (timestamp ) ;
	//
	// now check to see if this code is already in the history
	boolean found=false;
	for(int i=0;i<numberOfCommandCodesInHistory;i++){
		if(commandCodeHistory[i]==code){
			found=true;
		}
	}

	if(!found){
		//
		// now check to see if this value is in the history
		if(currentCommandCodeHistoryPos<numberOfCommandCodesInHistory){
			commandCodeHistory[currentCommandCodeHistoryPos]=code;
			currentCommandCodeHistoryPos++;
		}else{
			for(int i=0;i<numberOfCommandCodesInHistory-1;i++){
				commandCodeHistory[i]=commandCodeHistory[i+1];
			}
			commandCodeHistory[numberOfCommandCodesInHistory-1]=code;
		}
	}

	return code;
}

boolean SecretManager::checkCode(long userCode){
	boolean codeOk=false;
	long code = generateCode();
	// Serial.print("code=");
	//Serial.print(code);

	if(userCode == code){
		codeOk=true;
	}else{
		for(int i=0;i<numberOfCommandCodesInHistory;i++){
			//    Serial.print("codehistory");
			//   Serial.print(commandCodeHistory[i]);

			if(commandCodeHistory[i] == userCode){
				codeOk=true;
			}
		}
	}

	return codeOk;
}
