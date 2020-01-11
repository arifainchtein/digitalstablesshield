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

String GeneralFunctions::getElapsedTimeHoursMinutesSecondsString(long elapsedTime) {
	//String seconds = String(elapsedTime % 60);
	long seconds = elapsedTime/1000;
	int minutes = (seconds % 3600) / 60;
	String minP ="";
	if(minutes<10)minP="0";


	int hours = seconds / 3600;
	String hoursS = "";
	if(hours<10)hoursS="0";


	String time =  hoursS + hours + ":" + minP + minutes;// + ":" + seconds;
	return time;
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

uint8_t GeneralFunctions::getStateOfCharge(float batteryVoltage){
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


uint8_t GeneralFunctions::getCapacitorStateOfCharge(float capacitorVoltage){
	if(capacitorVoltage>5.0)return 100;
		else if(capacitorVoltage>=4.84)return 90;
		else if(capacitorVoltage>=4.81)return 85;
		else if(capacitorVoltage>=4.78)return 80;
		else if(capacitorVoltage>=4.74)return 75;

		else if(capacitorVoltage>=4.70)return 70;
		else if(capacitorVoltage>=4.66)return 65;

		else if(capacitorVoltage>=4.61)return 60;
		else if(capacitorVoltage>=4.57)return 55;

		else if(capacitorVoltage>=4.51)return 50;
		else if(capacitorVoltage>=4.45)return 45;

		else if(capacitorVoltage>=4.39)return 40;
		else if(capacitorVoltage>=4.33)return 35;

		else if(capacitorVoltage>=4.26)return 30;
		else if(capacitorVoltage>=4.18)return 25;
		else if(capacitorVoltage>=4.10)return 20;
		else if(capacitorVoltage>=4.06)return 15;

		else if(capacitorVoltage>=4.00)return 10;
		else if(capacitorVoltage>=3.95)return 5;

		else if(capacitorVoltage>=11.90)return 0;
		return 0;
}



void GeneralFunctions::printPowerStatusStructToSerial(HardwareSerial& serial, const PowerStatusStruct powerStatusStruct){
	serial.print(powerStatusStruct.sampleTime );
	serial.print("#" );
	serial.print(powerStatusStruct.energySourceVoltage );
	serial.print("#" );
	serial.print(powerStatusStruct.energyStorageVoltage );
	serial.print("#" );
	serial.print(powerStatusStruct.currentFromEnergySource);
	serial.print("#" );
	serial.print(powerStatusStruct.currentFromEnergyStorage);
	serial.print("#" );
	serial.print(powerStatusStruct.lockCapacitorVoltage);
	serial.print("#" );
	serial.print(powerStatusStruct.internalEnergyStorageStateOfCharge);
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
	serial.print(powerStatisticsStruct.dailyMinEnergyStorageVoltage);
	serial.print("#" );
	serial.print(powerStatisticsStruct.dailyMaxEnergyStorageVoltage);
	serial.print("#" );
	serial.print(powerStatisticsStruct.dailyMinEnergyStorageCurrent);
	serial.print("#" );

	serial.print(powerStatisticsStruct.dailyMaxEnergyStorageCurrent);
	serial.print("#" );
	serial.print(powerStatisticsStruct.dailyEnergyStorageOutEnergy);
	serial.print("#" );
	serial.print(powerStatisticsStruct.dailyPoweredDownInLoopSeconds);
	serial.print("#" );
	serial.print(powerStatisticsStruct.hourlyEnergyStorageOutEnergy);
	serial.print("#" );
	serial.print(powerStatisticsStruct.hourlyPoweredDownInLoopSeconds);
	serial.print("#" );
//	serial.print(powerStatusStruct.totalDiskUse);
//	serial.print(powerStatusStruct.totalDiskAvailable);

}
