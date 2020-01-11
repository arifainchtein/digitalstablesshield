/*
 * SDCardInitParams.h
 *
 *  Created on: 24 Oct 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_RF24COMMUNICATORINITPARAMS_H_
#define LIBRARIES_DIGITALSTABLES_RF24COMMUNICATORINITPARAMSS_H_
#include <Arduino.h>
#include <CommunicatorInitParams.h>


struct RF24CommunicatorInitParams : public CommunicatorInitParams{

	RF24CommunicatorInitParams(uint8_t ce, uint8_t cs):cepin(ce), cspin(cs){}

		uint8_t cepin=12;
		uint8_t cspin=4;
};



#endif /* LIBRARIES_DIGITALSTABLES_SDCARDINITPARAMS_H_ */
