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

uint8_t CHIP_ENABLE=13;//22;
uint8_t CHIP_SELECT=4;//23;
uint64_t PIPE0=0xF0F0F0F0E1LL;
uint64_t PIPE1=0xF0F0F0F0D2LL;

RF24 radio(CHIP_ENABLE, CHIP_SELECT);
unsigned long lastReading=0L;
unsigned int gloriaDataCounter;
#define REG3V3PIN 0
GloriaCommData gloriaCommData;
unsigned long lastRead2;

GloriaHomeRF24Communicator::GloriaHomeRF24Communicator(LCDDisplay& l):lcd(l) {
	// TODO Auto-generated constructor stub

}

void GloriaHomeRF24Communicator::start(GloriaCommData& commDat){
	gloriaCommData=commDat;

	radio.begin();
	radio.setDataRate( RF24_250KBPS );

	// optionally, increase the delay between retries & # of retries
	radio.setRetries(15,15);

	// optionally, reduce the payload size.  seems to
	// improve reliability
	// radio.setPayloadSize(32);
	radio.enableDynamicPayloads();
	//radio.setAutoAck(1);
	//radio.enableAckPayload();

	//radio.openWritingPipe(gloriaCommData.readingPipe);//writingPipe);
	//radio.openReadingPipe(1,gloriaCommData.writingPipe);//readingPipe);
	const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1,pipes[0]);


	//	radio.openWritingPipe(PIPE1);
	//	radio.openReadingPipe(1,PIPE0);

	//
	// Start listening
	//

	radio.startListening();

	//
	// Dump the configuration of the rf unit for debugging
	//

	radio.printDetails();
}

bool GloriaHomeRF24Communicator::publish(GloriaHomeBaseData& gloriaHomeBaseData){
	//RF24 radio(aRF24CommunicatorInitParams.cepin, aRF24CommunicatorInitParams.cspin);
	digitalWrite (SS, HIGH);
	//	SPI.begin ();
	digitalWrite (CHIP_ENABLE, LOW);
	digitalWrite (CHIP_SELECT, HIGH);
	//radio.begin();
	//	radio.powerUp();

	radio.setRetries(15, 15);
	radio.enableDynamicPayloads();
//	radio.openWritingPipe (gloriaCommData.writingPipe);
	radio.setPALevel(RF24_PA_MAX);
//	radio.openReadingPipe (1, gloriaCommData.readingPipe);

//	radio.startListening ();
//	delay (10);
	radio.stopListening ();
	delay (10);

	bool toReturn=false;
	toReturn = radio.write (&gloriaHomeBaseData, sizeof gloriaHomeBaseData);
	radio.startListening ();
	//radio.powerDown ();

	//SPI.end ();
	return toReturn;
}



bool GloriaHomeRF24Communicator::receive(GloriaBaseData& gloriaBaseData,GloriaFlowData& gloriaFlowData){


	bool toReturn=false;
	//
	//  the code to receive
	//


	if ( radio.available() ){

		bool doneBase = false;
		bool doneFlow = false;
		lcd.clear();
		//while (!doneBase && !doneFlow)
		//{
			uint8_t bytesReceivedLength = radio.getDynamicPayloadSize();
			lcd.println ("Rcv Gloria 1");
			// Fetch the payload, and see if this was the last one.
			if(bytesReceivedLength == sizeof gloriaBaseData){
				  radio.read( &gloriaBaseData, sizeof gloriaBaseData);
				doneBase=true;
			//	delay(100);
				toReturn=true;


			}else if(bytesReceivedLength == sizeof gloriaFlowData){
				radio.read( &gloriaFlowData, sizeof gloriaFlowData);
				doneFlow=true;
			//	delay(100);

				toReturn=true;
			}

			lastRead2 = millis ();

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

GloriaHomeRF24Communicator::~GloriaHomeRF24Communicator() {
	// TODO Auto-generated destructor stub
}

