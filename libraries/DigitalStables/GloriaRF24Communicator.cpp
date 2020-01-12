/*
 * RF24Communicator.cpp
 *
 *  Created on: 17 Nov 2019
 *      Author: arifainchtein
 */
#include <Arduino.h>
#include <GloriaRF24Communicator.h>
#include <GloriaData.h>
#include <TelepathonData.h>


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <TimeManager.h>
//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
/**
 * Constructor
 *
 * Creates a new instance of this driver.  Before using, you create an instance
 * and send in the unique pins that this chip is connected to.
 *
 * @param _cepin The pin attached to Chip Enable on the RF module
 * @param _cspin The pin attached to Chip Select
 */

//RF24& radio;

//
// Channel info
//

//
// variables for the scan function
//
const int num_reps = 100;
const uint8_t num_channels = 128;
uint8_t values[num_channels];

RF24& radio;
RF24CommunicatorInitParams  aRF24CommunicatorInitParams;
GloriaRF24Communicator::GloriaRF24Communicator() {
	// TODO Auto-generated constructor stub

}

void GloriaRF24Communicator::start(RF24CommunicatorInitParams p){
	aRF24CommunicatorInitParams =p;
	radio = new RF24(p.cepin, p.cspin);
	radio.begin();

	// optionally, increase the delay between retries & # of retries
	radio.setRetries(15,15);

	// optionally, reduce the payload size.  seems to
	// improve reliability
	// radio.setPayloadSize(32);
	radio.enableDynamicPayloads();

	radio.openWritingPipe(p.pipe1);
	radio.openReadingPipe(1,p.pipe0);

	//
	// Start listening
	//

//	radio.startListening();

	//
	// Dump the configuration of the rf unit for debugging
	//

	radio.printDetails();
}

void GloriaRF24Communicator::publish(TelepathonData& telepathonData[]){
	//RF24 radio(aRF24CommunicatorInitParams.cepin, aRF24CommunicatorInitParams.cspin);
	digitalWrite (SS, HIGH);
	SPI.begin ();
	digitalWrite (aRF24CommunicatorInitParams.cepin, LOW);
	digitalWrite (aRF24CommunicatorInitParams.cspin, HIGH);
	//radio.begin();
	radio.powerUp();

	radio.setRetries(15, 15);
	radio.enableDynamicPayloads();
	radio.openWritingPipe (aRF24CommunicatorInitParams.pipe0);
	radio.setPALevel(RF24_PA_MIN);
	radio.openReadingPipe (1, aRF24CommunicatorInitParams.pipe1);

	radio.startListening ();
	delay (10);
	radio.stopListening ();
	delay (10);
	GloriaBaseData data = telepathonData[0];
	GloriaFlowData gloriaFlowData = telepathonData[1];
	bool ok = radio.write (&data, sizeof data);
	ok = radio.write (&gloriaFlowData, sizeof gloriaFlowData);
	radio.startListening ();
	radio.powerDown ();

	SPI.end ();

}

TelepathonData * GloriaRF24Communicator::receive(){

	TelepathonData data[] new TelepathonData[2];
	GloriaBaseData baseData;
	data[0]=baseData;
	GloriaFlowData gloriaFlowData;
	data[1]=gloriaFlowData;

	return data;
}

void GloriaRF24Communicator::scan(void)
{
	// Clear measurement values
	memset(values,0,sizeof(values));

	// Scan all channels num_reps times
	int rep_counter = num_reps;
	while (rep_counter--)
	{
		int i = num_channels;
		while (i--)
		{
			// Select this channel
			//      radio.setChannel(i);
			//
			//      // Listen for a little
			//      radio.startListening();
			//      delayMicroseconds(128);
			//      radio.stopListening();
			//
			//      // Did we get a carrier?
			//      if ( radio.testCarrier() )
			//        ++values[i];
		}
	}

	// Print out channel measurements, clamped to a single hex digit
	int i = 0;
	while ( i < num_channels )
	{
		printf("%x",min(0xf,values[i]&0xf));
		++i;
	}
	printf("\n\r");
}

GloriaRF24Communicator::GloriaRF24Communicator() {
	// TODO Auto-generated destructor stub
}

