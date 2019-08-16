/*
 * NetworkSensorController.h
 *
 *  Created on: 8 Aug 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_NETWORKSENSORCONTROLLER_H_
#define LIBRARIES_DIGITALSTABLES_NETWORKSENSORCONTROLLER_H_

#include <static_str.h>

class NetworkSensorController {

	HardwareSerial& _HardSerial;

public:
	NetworkSensorController(HardwareSerial& serial);
	void start();
	void setNetworkStatusColor(unsigned char pin, String color );
	void setNetworkStatusColor(unsigned char pin, uint8_t red, uint8_t green, uint8_t blue );
	void setDownloadQoS(uint8_t value);
	void setUploadQoS(uint8_t value);
	void setQoS(uint8_t download, uint8_t upload);
	virtual ~NetworkSensorController();
};

#endif /* LIBRARIES_DIGITALSTABLES_NETWORKSENSORCONTROLLER_H_ */
