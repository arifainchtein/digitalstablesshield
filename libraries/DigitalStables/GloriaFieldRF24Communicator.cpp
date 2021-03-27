/*
 * GloriaFieldRF24Communicator.cpp
 *
 *  Created on: 22 Jun 2020
 *      Author: arifainchtein
 */

#include <GloriaFieldRF24Communicator.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>




//
// Channel info
//

//
// variables for the scan function
//
const int num_reps = 100;
const uint8_t num_channels = 128;
uint8_t gloriaFieldValues[num_channels];
RF24* radio;

unsigned long lastReading=0L;
unsigned int gloriaDataCounter;
unsigned long lastRead2;


long lastReceptionFromBase=0L;
long currentTime = 0L;
bool sendBaseData=false;


GloriaFieldRF24Communicator::GloriaFieldRF24Communicator(LCDDisplay & l, TimeManager & t) : lcd(l), timeManager(t){
	// TODO Auto-generated constructor stub

}




void GloriaFieldRF24Communicator::start(GloriaCommData& gloriaCommData){
	//gloriaCommData=commDat;
	radio = new RF24(gloriaCommData.CHIP_ENABLE, gloriaCommData.CHIP_SELECT);
	radio->begin();
	radio->setDataRate( RF24_250KBPS );

	// optionally, increase the delay between retries & # of retries
	radio->setRetries(15,15);

	// optionally, reduce the payload size.  seems to
	// improve reliability
	// radio->setPayloadSize(32);
	radio->enableDynamicPayloads();
	//radio->setAutoAck(1);
	//radio->enableAckPayload();

	//radio->openWritingPipe(gloriaCommData.readingPipe);//writingPipe);
	//radio->openReadingPipe(1,gloriaCommData.writingPipe);//readingPipe);
	const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
	radio->openWritingPipe(pipes[1]);
	radio->openReadingPipe(1,pipes[0]);


	//	radio->openWritingPipe(PIPE1);
	//	radio->openReadingPipe(1,PIPE0);

	//
	// Start listening
	//

	radio->startListening();

	//
	// Dump the configuration of the rf unit for debugging
	//

	radio->printDetails();
}

bool GloriaFieldRF24Communicator::receive(GloriaHomeBaseData& gloriaHomeBaseData){
	bool toReturn=false;
	radio->startListening ();
	delay(10);
	if ( radio->available() ){
		//blink(pixelsLayer2.Color(248, 128, 23));
		uint8_t bytesReceivedLength = radio->getDynamicPayloadSize();
		lcd.print ("bytesReceivedLength = ");
		lcd.print (bytesReceivedLength);
		// Dump the payloads until we've gotten everything



		bool done = false;
		while (!done)
		{
			// Fetch the payload, and see if this was the last one.

			if(bytesReceivedLength == sizeof gloriaHomeBaseData){
				done = radio->read( &gloriaHomeBaseData, sizeof gloriaHomeBaseData);
				//Serial.print ("new pump status = ");
				//Serial.println (gloriaHomeBaseData.pumpState);
			}else {
				//blink(pixelsLayer2.Color(0 ,255, 0));
				done=false;
			}

			lastReceptionFromBase = timeManager.getCurrentTimeInSeconds();
		}  // end reading the payload
	}


	radio->stopListening ();
	return toReturn;
}





//
bool GloriaFieldRF24Communicator::sendSensorData (const GloriaFieldSensorData& sensorData)
{
	// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
	//ower_all_enable();
	digitalWrite (SS, HIGH);
	// SPI.begin ();
	//digitalWrite (CHIP_ENABLE, LOW);
	//digitalWrite (CHIP_SELECT, HIGH);
	//  //Serial.println("Sending 2");
	//
	// Setup and configure rf radio
	//

	// optionally, increase the delay between retries & # of retries
	///                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               radio.setRetries(15, 15);

	// optionally, reduce the payload size.  seems to improve reliability
	// radio.setPayloadSize(32);


	//  //Serial.println("Sending 4");

	delay (10);
	//Serial.print("Sending sensorData:");

	bool  ok = radio->write (&sensorData, sizeof sensorData);
	if(ok){
		//Serial.println("sensorData Data was sent ok");
	}else{
		//Serial.println("sensorData       Data was NOT sent ok");
	}
}

bool GloriaFieldRF24Communicator::sendFlowData (const GloriaFieldFlowData& flowData)
{
	// Set up nRF24L01 radio on SPI bus plus pins 9 & 10


	//ower_all_enable();
	digitalWrite (SS, HIGH);
	// SPI.begin ();
	//digitalWrite (CHIP_ENABLE, LOW);
	//digitalWrite (CHIP_SELECT, HIGH);
	//  //Serial.println("Sending 2");
	//
	// Setup and configure rf radio
	//

	// optionally, increase the delay between retries & # of retries
	///                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               radio.setRetries(15, 15);

	// optionally, reduce the payload size.  seems to improve reliability
	// radio.setPayloadSize(32);


	//  //Serial.println("Sending 4");

	delay (10);
	//Serial.print("Sending flow:");

	bool  ok = radio->write (&flowData, sizeof flowData);
	if(ok){
		//Serial.println("Flow Data was sent ok");
	}else{
		//Serial.println("Flow       Data was NOT sent ok");
	}
}
bool GloriaFieldRF24Communicator::sendPowerData (const GloriaFieldPowerData& data)
{
	// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
	radio->stopListening();

	//ower_all_enable();
	//digitalWrite (SS, HIGH);
	// SPI.begin ();
	//digitalWrite (CHIP_ENABLE, LOW);
	//digitalWrite (CHIP_SELECT, HIGH);
	//  //Serial.println("Sending 2");
	//
	// Setup and configure rf radio
	//

	// optionally, increase the delay between retries & # of retries
	///                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               radio.setRetries(15, 15);

	// optionally, reduce the payload size.  seems to improve reliability
	// radio.setPayloadSize(32);


	//  //Serial.println("Sending 4");

	delay (10);

	bool ok = radio->write (&data, sizeof data);
	if(ok){
		//Serial.println("Base Data was sent ok");
	}else{
		//Serial.println("Base Data was NOT sent ok");
	}


	//  power_all_disable();
	radio->startListening();
	return ok;
}





void GloriaFieldRF24Communicator::scan(void)
{
	// Clear measurement gloriaFieldValues
	memset(gloriaFieldValues,0,sizeof(gloriaFieldValues));

	// Scan all channels num_reps times
	int rep_counter = num_reps;
	while (rep_counter--)
	{
		int i = num_channels;
		while (i--)
		{
			// Select this channel
			//      radio->setChannel(i);
			//
			//      // Listen for a little
			//      radio->startListening();
			//      delayMicroseconds(128);
			//      radio->stopListening();
			//
			//      // Did we get a carrier?
			//      if ( radio->testCarrier() )
			//        ++gloriaFieldValues[i];
		}
	}

	// Print out channel measurements, clamped to a single hex digit
	int i = 0;
	while ( i < num_channels )
	{
		printf("%x",min(0xf,gloriaFieldValues[i]&0xf));
		++i;
	}
	printf("\n\r");
}
GloriaFieldRF24Communicator::~GloriaFieldRF24Communicator() {
	// TODO Auto-generated destructor stub
}

