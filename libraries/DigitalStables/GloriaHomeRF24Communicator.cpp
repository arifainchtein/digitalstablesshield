/*
 * RF24Communicator.cpp
 *
 *  Created on: 17 Nov 2019
 *      Author: arifainchtein
 */
#include <Arduino.h>
#include <GloriaHomeRF24Communicator.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <TimeManager.h>
//
// Hardware configuration
//

// Set up nRF24L01 radio33 on SPI bus plus pins 9 & 10
/**
 * Constructor
 *
 * Creates a new instance of this driver.  Before using, you create an instance
 * and send in the unique pins that this chip is connected to.
 *
 * @param _cepin The pin attached to Chip Enable on the RF module
 * @param _cspin The pin attached to Chip Select
 */

/*

NRF24L01     Uno
----------   ----------------

  VCC         --> 3.3V pin
  Gnd         Gnd
  CSN         D10 (SS)
  CE          D9
  MOSI        D11 (MOSI)
  SCK         D13 (SCK)
  IRQ         N/C
  MISO        D12 (MISO)

 */


//
// Channel info
//

//
// variables for the scan function
//
const int num_reps = 100;
const uint8_t num_channels = 128;
uint8_t values[num_channels];

//uint8_t CHIP_ENABLE=13;//22;
//uint8_t CHIP_SELECT=4;//23;
//uint64_t PIPE0=0xF0F0F0F0E1LL;
//uint64_t PIPE1=0xF0F0F0F0D2LL;

//RF24 radio33(CHIP_ENABLE, CHIP_SELECT);
RF24* radio33;

unsigned long gloriaHomeLastReading=0L;
unsigned int gloriaHomeDataCounter;
#define REG3V3PIN 0
int CHIP_ENABLE=4;
int CHIP_SELECT=23;


GloriaHomeRF24Communicator::GloriaHomeRF24Communicator(LCDDisplay& l):lcd(l) {
	// TODO Auto-generated constructor stub

}

void GloriaHomeRF24Communicator::start(GloriaCommData& gloriaCommData){

	radio33 = new RF24(gloriaCommData.CHIP_ENABLE, gloriaCommData.CHIP_SELECT);
	CHIP_ENABLE = gloriaCommData.CHIP_ENABLE;
	CHIP_SELECT= gloriaCommData.CHIP_SELECT;
	radio33->begin();
	radio33->setDataRate( RF24_250KBPS );

	// optionally, increase the delay between retries & # of retries
	radio33->setRetries(15,15);

	// optionally, reduce the payload size.  seems to
	// improve reliability
	// radio33->setPayloadSize(32);
	radio33->enableDynamicPayloads();
	//radio33->setAutoAck(1);
	//radio33->enableAckPayload();

	//radio33->openWritingPipe(gloriaCommData.readingPipe);//writingPipe);
	//radio33->openReadingPipe(1,gloriaCommData.writingPipe);//readingPipe);
	const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
	radio33->openWritingPipe(pipes[1]);
	radio33->openReadingPipe(1,pipes[0]);


	//	radio33->openWritingPipe(PIPE1);
	//	radio33->openReadingPipe(1,PIPE0);

	//
	// Start listening
	//

	radio33->startListening();

	//
	// Dump the configuration of the rf unit for debugging
	//

	radio33->printDetails();
}

bool GloriaHomeRF24Communicator::publish(GloriaHomeBaseData& gloriaHomeBaseData){
	//RF24 radio33(aRF24CommunicatorInitParams.cepin, aRF24CommunicatorInitParams.cspin);
	digitalWrite (SS, HIGH);
	//	SPI.begin ();
	digitalWrite (CHIP_ENABLE, LOW);
	digitalWrite (CHIP_SELECT, HIGH);
	//radio33->begin();
	//	radio33->powerUp();

	radio33->setRetries(15, 15);
	radio33->enableDynamicPayloads();
//	radio33->openWritingPipe (gloriaCommData.writingPipe);
	radio33->setPALevel(RF24_PA_MAX);
//	radio33->openReadingPipe (1, gloriaCommData.readingPipe);

//	radio33->startListening ();
//	delay (10);
	radio33->stopListening ();
	delay (10);

	bool toReturn=false;
	toReturn = radio33->write (&gloriaHomeBaseData, sizeof gloriaHomeBaseData);
	radio33->startListening ();
	//radio33->powerDown ();

	//SPI.end ();
	return toReturn;
}



bool GloriaHomeRF24Communicator::receive(GloriaBaseData& gloriaBaseData,GloriaFlowData& gloriaFlowData){


	bool toReturn=false;
	//
	//  the code to receive
	//


	if ( radio33->available() ){


	//	lcd.clear();
		//while (!doneBase && !doneFlow)
		//{
			uint8_t bytesReceivedLength = radio33->getDynamicPayloadSize();
			//lcd.println ("Rcv Gloria 1");
			// Fetch the payload, and see if this was the last one.
			if(bytesReceivedLength == sizeof gloriaBaseData){
				  radio33->read( &gloriaBaseData, sizeof gloriaBaseData);

			//	delay(100);
				toReturn=true;


			}else if(bytesReceivedLength == sizeof gloriaFlowData){
				radio33->read( &gloriaFlowData, sizeof gloriaFlowData);
			//	delay(100);

				toReturn=true;
			}

			gloriaHomeLastReading = millis ();

			//        char voltageBuf [8];
			//        dtostrf (results, 4, 2, voltageBuf);  // number, width, decimal places, buffer
			//        char buf [9];
			//        sprintf (buf, "%3s %4i", voltageBuf, counter);

			//  display.sendString (buf);

		//}


		// end reading the payload
	}


	return toReturn;
}

void GloriaHomeRF24Communicator::scan(void)
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
			//      radio33->setChannel(i);
			//
			//      // Listen for a little
			//      radio33->startListening();
			//      delayMicroseconds(128);
			//      radio33->stopListening();
			//
			//      // Did we get a carrier?
			//      if ( radio33->testCarrier() )
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

GloriaHomeRF24Communicator::~GloriaHomeRF24Communicator() {
	// TODO Auto-generated destructor stub
}

