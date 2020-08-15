/*
 * GloriaFieldSensors.cpp
 *
 *  Created on: 22 Jun 2020
 *      Author: arifainchtein
 */
#include <Arduino.h>
#include <GloriaFieldSensors.h>

int TMP36_PIN=A0;
int TANK_WATER_LEVEL_PIN=A1;
double tankHeight;
double tankPerimiter;

GloriaFieldSensors::GloriaFieldSensors() {
	// TODO Auto-generated constructor stub

}


float GloriaFieldSensors::getTemperature(){
  int reading = analogRead(TMP36_PIN);
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  return temperatureC;
}

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

