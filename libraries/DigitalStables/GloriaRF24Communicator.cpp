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

uint8_t CHIP_ENABLE=22;
uint8_t CHIP_SELECT=23;
uint64_t PIPE0=0xF0F0F0F0E1LL;
uint64_t PIPE1=0xF0F0F0F0D2LL;

RF24 radio(CHIP_ENABLE, CHIP_SELECT);

GloriaRF24Communicator::GloriaRF24Communicator() {
	// TODO Auto-generated constructor stub

}

void GloriaRF24Communicator::start(){

	radio.begin();

	// optionally, increase the delay between retries & # of retries
	radio.setRetries(15,15);

	// optionally, reduce the payload size.  seems to
	// improve reliability
	// radio.setPayloadSize(32);
	radio.enableDynamicPayloads();

	radio.openWritingPipe(PIPE1);
	radio.openReadingPipe(1,PIPE0);

	//
	// Start listening
	//

	//	radio.startListening();

	//
	// Dump the configuration of the rf unit for debugging
	//

	radio.printDetails();
}

bool GloriaRF24Communicator::publish(const TelepathonData& telepathonData){
	//RF24 radio(aRF24CommunicatorInitParams.cepin, aRF24CommunicatorInitParams.cspin);
	digitalWrite (SS, HIGH);
	SPI.begin ();
	digitalWrite (CHIP_ENABLE, LOW);
	digitalWrite (CHIP_SELECT, HIGH);
	//radio.begin();
	radio.powerUp();

	radio.setRetries(15, 15);
	radio.enableDynamicPayloads();
	radio.openWritingPipe (PIPE0);
	radio.setPALevel(RF24_PA_MIN);
	radio.openReadingPipe (1, PIPE1);

	radio.startListening ();
	delay (10);
	radio.stopListening ();
	delay (10);
	GloriaBaseData data = telepathonData[0];
	GloriaFlowData gloriaFlowData = telepathonData[1];
	bool toReturn=false;
	toReturn = radio.write (&data, sizeof data);
	toReturn = radio.write (&gloriaFlowData, sizeof gloriaFlowData);
	radio.startListening ();
	radio.powerDown ();

	SPI.end ();
	return toReturn;
}

unsigned long lastReading;
unsigned int counter;

bool GloriaRF24Communicator::receive(TelepathonData& telepathonData){

	TelepathonData data[] =  new TelepathonData[2];
	GloriaBaseData baseData;
	data[0]=baseData;
	GloriaFlowData gloriaFlowData;
	data[1]=gloriaFlowData;
	bool toReturn=false;
	//
	//  the code to receive
	//
	if ( radio.available() ){

		uint8_t bytesReceivedLength = radio.getDynamicPayloadSize();
		Serial.print ("bytesReceivedLength = ");
		Serial.print (bytesReceivedLength);
		// Dump the payloads until we've gotten everything



		bool done = false;
		while (!done)
		{
			// Fetch the payload, and see if this was the last one.
			if(bytesReceivedLength == sizeof baseData){
				done = radio.read( &baseData, sizeof baseData);
				if (counter > 9999)
					counter = 0;


				// float results = InternalReferenceVoltage / float (voltage + 0.5) * 1024.0;
				int elapsedTime = (millis () - lastReading) / 1000;
				long timestamp = baseData.secondsTime;
				toReturn=true;


			}else if(bytesReceivedLength == sizeof gloriaFlowData){
				done = radio.read( &gloriaFlowData, sizeof gloriaFlowData);
				if (counter > 9999)
					counter = 0;


				// float results = InternalReferenceVoltage / float (voltage + 0.5) * 1024.0;
				int elapsedTime = (millis () - lastReading) / 1000;
				long timestamp = gloriaFlowData.secondsTime;

				Serial.print ("Reading = ");
				Serial.print (timestamp);
				Serial.print ("   ");
				// String timeString = GeneralFunctions::getElapsedTimeHoursMinutesSecondsString(timestamp);
				//      Serial.print (timeString);
				Serial.print ("   ");


				Serial.print (",  flow1=");
				Serial.print (gloriaFlowData.flowRate1);
				Serial.print (",  volume1=");
				Serial.print (gloriaFlowData.volume1);
				Serial.print (",  flow2=");
				Serial.print (gloriaFlowData.flowRate2);
				Serial.print (",  volume2=");
				Serial.println (gloriaFlowData.volume2);
				toReturn=true;
			}
			lastReading = millis ();
		}  // end reading the payload
	}









	return true;
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

