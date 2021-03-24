/*
 * GloriaFieldSensors.cpp
 *
 *  Created on: 22 Jun 2020
 *      Author: arifainchtein
 */
#include <Arduino.h>
#include <GloriaFieldSensors.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <GloriaPinDefinition.h>

OneWire oneWire(DS18B20Z_DQ);
DallasTemperature tempSensor(&oneWire);


double tankHeight;
double tankPerimiter;

GloriaFieldSensors::GloriaFieldSensors() {

}

void GloriaFieldSensors::start(){
	//tempSensor.begin();
}
float GloriaFieldSensors::getTemperature(){
	tempSensor.requestTemperatures();
	delay(100);
	return tempSensor.getTempCByIndex(0);
}


//float GloriaFieldSensors::getTemperature(){
//
//
//	  //
//	      // the temperature
//	      //
//	      byte present;
//	        byte i;
//	        byte type_s;
//	        byte data[12];
//	        byte addr[8];
//	        float celsius;
//
//	        if ( !oneWire.search(addr)) {
//	          //Serial.println("No more addresses.");
//	        //  Serial.println();
//	        	oneWire.reset_search();
//	          delay(250);
//	        }
//
//
//
//	       	oneWire.reset();
//	       	oneWire.select(addr);
//	       	oneWire.write(0x44, 1);        // start conversion, with parasite power on at the end
//
//	        delay(750);;     // maybe 750ms is enough, maybe not
//	        // we might do a ds.depower() here, but the reset will take care of it.
//
//	        present = oneWire.reset();
//	        oneWire.select(addr);
//	        oneWire.write(0xBE);         // Read Scratchpad
//	        for ( i = 0; i < 9; i++) {           // we need 9 bytes
//	          data[i] = oneWire.read();
//	        }
//	        // Convert the data to actual temperature
//	        // because the result is a 16 bit signed integer, it should
//	        // be stored to an "int16_t" type, which is always 16 bits
//	        // even when compiled on a 32 bit processor.
//	        int16_t raw = (data[1] << 8) | data[0];
//	        if (type_s) {
//	          raw = raw << 3; // 9 bit resolution default
//	          if (data[7] == 0x10) {
//	            // "count remain" gives full 12 bit resolution
//	            raw = (raw & 0xFFF0) + 12 - data[6];
//	          }
//	        } else {
//	          byte cfg = (data[4] & 0x60);
//	          // at lower res, the low bits are undefined, so let's zero them
//	          if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
//	          else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
//	          else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
//	          //// default is 12 bit resolution, 750 ms conversion time
//	        }
//	         celsius = (float)raw / 16.0;
//	        return celsius;
//	}


void GloriaFieldSensors::setTankHeight(double d){
	tankHeight=d;
}
void GloriaFieldSensors::setTankPerimiter(double d){
	tankPerimiter=d;
}

double GloriaFieldSensors::getTankWaterCurrentVolume(void){

}

double GloriaFieldSensors::getTankWaterLevel(void){

}
//
// sourcez; https://forum.arduino.cc/index.php?topic=204429.0
//
float GloriaFieldSensors::getSourceVoltage(void) // Returns actual value of Vcc (x 100)
    {

     // For 1284/644
     const long InternalReferenceVoltage = 1065L;  // Adjust this value to your boards specific internal BG voltage x1000
        // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc reference
        // MUX4 MUX3 MUX2 MUX1 MUX0  --> 11110 1.1V (VBG)         -Selects channel 30, bandgap voltage, to measure
     ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR)| (1<<MUX4) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);

     delay(50);  // Let mux settle a little to get a more stable A/D conversion
        // Start a conversion
     ADCSRA |= _BV( ADSC );
        // Wait for it to complete
     while( ( (ADCSRA & (1<<ADSC)) != 0 ) );
        // Scale the value
     int results = (((InternalReferenceVoltage * 1023L) / ADC) + 5L) / 10L; // calculates for straight line value
     float voltage = results/100.0;
  return voltage;

    }

GloriaFieldSensors::~GloriaFieldSensors() {
	// TODO Auto-generated destructor stub
}

