/*
 * Esp32SecretManager.cpp
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#include <Esp32SecretManager.h>
#include <TimeManager.h>



Esp32SecretManager::Esp32SecretManager(TimeManager& t): timeManager(t){

}

Esp32SecretManager::~Esp32SecretManager() {
	// TODO Auto-generated destructor stub
}

