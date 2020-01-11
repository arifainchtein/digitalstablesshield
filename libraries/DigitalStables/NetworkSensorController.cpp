/*
 * NetworkSensorController.cpp
 *
 *  Created on: 8 Aug 2019
 *      Author: arifainchtein
 */
#include <Arduino.h>
#include <NetworkSensorController.h>
#include <Adafruit_NeoPixel.h>
#include <static_str.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define NETWORK_STATUS_LED_PIN 23
#define NETWORK_QOS_DOWNLOAD_PIN 13
#define NETWORK_QOS_UPLOAD_PIN 14
#define LED_COUNT      2

//Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, NETWORK_STATUS_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, NETWORK_STATUS_LED_PIN, NEO_GRB);


NetworkSensorController::NetworkSensorController(HardwareSerial& serial ):  _HardSerial(serial){
	// TODO Auto-generated constructor stub

}

void NetworkSensorController::start(){
	//
	// the Intrusion LED, set them to data stale at the begining
	//
	leds.begin();  // Call this to start up the LED strip.
	setNetworkStatusColor( 0, "stale" );
	setNetworkStatusColor( 1, "stale" );

	leds.show();
	//
	// the QoS meters
	//
	pinMode(NETWORK_QOS_DOWNLOAD_PIN, OUTPUT);
	pinMode(NETWORK_QOS_UPLOAD_PIN, OUTPUT);
	//
	//
}

void NetworkSensorController::setQoS(uint8_t download, uint8_t upload){
	analogWrite(NETWORK_QOS_DOWNLOAD_PIN, map(download, 0, 100, 0, 255));
	analogWrite(NETWORK_QOS_UPLOAD_PIN, map(upload, 0, 100, 0, 255));
}


void NetworkSensorController::setDownloadQoS(uint8_t value){
	analogWrite(NETWORK_QOS_DOWNLOAD_PIN, value);
}

void NetworkSensorController::setUploadQoS(uint8_t value){
	analogWrite(NETWORK_QOS_UPLOAD_PIN, value);
}

void NetworkSensorController::setNetworkStatusColor(unsigned char pin, String color ){

		if(color.equals("success")){
			setNetworkStatusColor( pin,  0,255,0 );
		}else if(color.equals("primary")){
			setNetworkStatusColor(pin,  0,0, 255);
		}else if(color.equals("warning")){
			setNetworkStatusColor(pin, 255, 255,  0);
		}else if(color.equals("danger")){
			setNetworkStatusColor(pin, 255, 0,  0);
		}else if(color.equals("default")){
			setNetworkStatusColor(pin, 255, 255, 255);
		}else if(color.equals("crisis")){
			setNetworkStatusColor(pin, 255,165,0);
		}else if(color.equals("stale")){
			setNetworkStatusColor(pin, 148,0, 211);
		}else if(color.equals("off")){
			setNetworkStatusColor(pin, 0, 0, 0);
		}else {
			_HardSerial.print("bad color:");
			_HardSerial.print(color);
			_HardSerial.print("#");
			_HardSerial.println(color.length());

			setNetworkStatusColor(pin, 0, 0, 0);
		}

}

void NetworkSensorController::setNetworkStatusColor(unsigned char pin, uint8_t red, uint8_t green, uint8_t blue ){

	leds.setPixelColor(pin, leds.Color(green,red, blue));
	leds.show();
	delay(500);
}




NetworkSensorController::~NetworkSensorController() {
	// TODO Auto-generated destructor stub
}

