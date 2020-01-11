/*
 * RF24Communicator.cpp
 *
 *  Created on: 17 Nov 2019
 *      Author: arifainchtein
 */

#include <RF24Communicator.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

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

RF24Communicator::RF24Communicator() {
	// TODO Auto-generated constructor stub

}

void RF24Communicator::start(RF24CommunicatorInitParams p){
//	radio=new RF24(p.cepin,p.cspin);
//	radio.begin();
//	radio.setAutoAck(false);
//
//	// Get into standby mode
//	radio.startListening();
//	radio.stopListening();
	// Print out header, high then low digit
	int i = 0;
//	while ( i < num_channels )
//	{
//		printf("%x",i>>4);
//		++i;
//	}
//	printf("\n\r");
//	i = 0;
//	while ( i < num_channels )
//	{
//		printf("%x",i&0xf);
//		++i;
//	}
//	printf("\n\r");

}


void RF24Communicator::scan(void)
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

RF24Communicator::~RF24Communicator() {
	// TODO Auto-generated destructor stub
}

