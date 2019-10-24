/*
 * SDCardInitParams.h
 *
 *  Created on: 24 Oct 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_SDCARDINITPARAMS_H_
#define LIBRARIES_DIGITALSTABLES_SDCARDINITPARAMS_H_

struct SDCardInitParams : public DataStorageManagerInitParams{

  uint8_t  sdPin=19;
};



#endif /* LIBRARIES_DIGITALSTABLES_SDCARDINITPARAMS_H_ */
