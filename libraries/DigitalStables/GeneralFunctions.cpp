/*
 * GeneralFunctions.cpp
 *
 *  Created on: 13 Feb. 2019
 *      Author: arifainchtein
 */

#include "Arduino.h"
#include "GeneralFunctions.h"

#define PASSLENGTH 8
char *alphabeth = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'?^&/%$£-+";
const long alphabethLength = 70;//sizeof(alphabeth);


GeneralFunctions::GeneralFunctions(){

}

char* GeneralFunctions::generatePassword(){
	static char pass[PASSLENGTH+1];
	for (int n = 0; n < PASSLENGTH; n++){
		pass[n] = alphabeth[random(0, alphabethLength)];
	}
	pass[PASSLENGTH] = '\0';
	return pass;
}


float GeneralFunctions::stringToFloat(String s){
	char buffer3[10];
	s.toCharArray(buffer3, 10);
	float result = atof(buffer3);
	return result;
}

String GeneralFunctions::getValue(String data, char separator, int index)
{
	int found = 0;
	int strIndex[] = {
			0, -1  };
	int maxIndex = data.length()-1;
	for(int i=0; i<=maxIndex && found<=index; i++){
		if(data.charAt(i)==separator || i==maxIndex){
			found++;
			strIndex[0] = strIndex[1]+1;
			strIndex[1] = (i == maxIndex) ? i+1 : i;
		}
	}
	return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int GeneralFunctions::getStateOfCharge(double batteryVoltage){
	if(batteryVoltage>12.90)return 100;
	else if(batteryVoltage>=12.84)return 90;
	else if(batteryVoltage>=12.81)return 85;
	else if(batteryVoltage>=12.78)return 80;
	else if(batteryVoltage>=12.74)return 75;

	else if(batteryVoltage>=12.70)return 70;
	else if(batteryVoltage>=12.66)return 65;

	else if(batteryVoltage>=12.61)return 60;
	else if(batteryVoltage>=12.57)return 55;

	else if(batteryVoltage>=12.51)return 50;
	else if(batteryVoltage>=12.45)return 45;

	else if(batteryVoltage>=12.39)return 40;
	else if(batteryVoltage>=12.33)return 35;

	else if(batteryVoltage>=12.26)return 30;
	else if(batteryVoltage>=12.18)return 25;
	else if(batteryVoltage>=12.10)return 20;
	else if(batteryVoltage>=12.06)return 15;

	else if(batteryVoltage>=12.00)return 10;
	else if(batteryVoltage>=11.95)return 5;

	else if(batteryVoltage>=11.90)return 0;
	return 0;
}

void GeneralFunctions::printPowerStatusStructToSerial(HardwareSerial& serial, const PowerStatusStruct powerStatusStruct){
	serial.print(powerStatusStruct.sampleTime );
	serial.print("#" );

	serial.print(powerStatusStruct.batteryVoltage );
	serial.print("#" );
	serial.print(powerStatusStruct.currentFromBattery);
	serial.print("#" );
	serial.print(powerStatusStruct.capacitorVoltage);
	serial.print("#" );
	serial.print(powerStatusStruct.internalBatteryStateOfCharge);
	serial.print("#" );
	serial.print(powerStatusStruct.regulatorVoltage);
	serial.print("#" );
	serial.print(powerStatusStruct.operatingStatus);
	serial.print("#" );
//
//	serial.print(powerStatusStruct.totalDiskUse);
//	serial.print(powerStatusStruct.totalDiskAvailable);

}

void GeneralFunctions::printPowerStatisticsStructToSerial(HardwareSerial& serial, const PowerStatisticsStruct powerStatisticsStruct){
	serial.print(powerStatisticsStruct.sampleTime );
	serial.print("#" );
	serial.print(powerStatisticsStruct.dailyMinBatteryVoltage);
	serial.print("#" );
	serial.print(powerStatisticsStruct.dailyMaxBatteryVoltage);
	serial.print("#" );
	serial.print(powerStatisticsStruct.dailyMinBatteryCurrent);
	serial.print("#" );

	serial.print(powerStatisticsStruct.dailyMaxBatteryCurrent);
	serial.print("#" );
	serial.print(powerStatisticsStruct.dailyBatteryOutEnergy);
	serial.print("#" );
	serial.print(powerStatisticsStruct.dailyPoweredDownInLoopSeconds);
	serial.print("#" );
	serial.print(powerStatisticsStruct.hourlyBatteryOutEnergy);
	serial.print("#" );
	serial.print(powerStatisticsStruct.hourlyPoweredDownInLoopSeconds);
	serial.print("#" );
//	serial.print(powerStatusStruct.totalDiskUse);
//	serial.print(powerStatusStruct.totalDiskAvailable);

}
