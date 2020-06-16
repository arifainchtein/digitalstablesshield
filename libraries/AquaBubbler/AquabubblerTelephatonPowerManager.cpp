/*
 * AquabubblerTelephatonPowerManager.cpp
 *
 *  Created on: 21 Oct 2019
 *      Author: arifainchtein
 */


#include <AquabubblerTelephatonPowerManager.h>
#include <PowerManager.h>
#include <SPI.h>




#define BIT0  0x01
#define BIT1  0x02
#define BIT2  0x04
#define BIT3  0x08
#define BIT4  0x10
#define BIT5  0x20
#define BIT6  0x40
#define BIT7  0x80
#define mascara_pin 0x00ff

#define PA  0x0000
#define PB  0x0100
#define PC  0x0200
#define PD  0x0300
#define PE  0x0400
#define PF  0x0500
#define PL  0x0600
#define PK  0x0700
#define PJ  0x0800
#define PH  0x0900
#define PG  0x1000
#define mascara_puerto  0xff00

//////// pin asignation
//#define flow_1  PE | BIT7 //9 //int7
//#define flow_2  PE | BIT6 //8 //int6
//#define flow_3  PE | BIT5 //7 //int5
//#define flow_4  PE | BIT4 //6 //int4
//#define flow_5  PD | BIT3 //46  //int3
//#define flow_6  PD | BIT2 //45  //int2
/*
  #define flow_1_int  INT7
  #define flow_2_int  INT6
  #define flow_3_int  INT5
  #define flow_4_int  INT4
  #define flow_5_int  INT3
  #define flow_6_int  INT2
 */
#define  ena_50  PL | BIT0 // 35
#define  ena_33  PL | BIT1 // 36

#define  usbpwren  PK | BIT0 // 89
#define  rtc_int  PK | BIT1 // 88
#define  nfc_int  PK | BIT2 // 87
#define  sd_det  PK | BIT3 // 86
#define  sw_stat  PK | BIT4 // 85

#define  nfc_ena  PL | BIT2 // 37
#define  sw_ena  PB | BIT5 // 24
#define   spi_ss  19//const byte  spi_ss[2]={PB,BIT0};// 19
#define  spi_ss_2 PB | BIT0

#define  en_485  PJ | BIT2 // 65
#define  en_logic  PH | BIT2 // 14

#define  led1  PB | BIT6 // 25
#define  led2  PB | BIT7 // 26

#define adc_0  PF | BIT0
#define adc_1  PF | BIT1
#define adc_2  PF | BIT2
#define adc_3  PF | BIT3
#define adc_4  PF | BIT4
#define adc_5  PF | BIT5




#define volt_ref  500
#define volt_mult   57
#define adc_res   1024
#define curr_ref_min 24900
#define curr_ref_tip  25000
#define curr_ref_max 25100
#define curr_mul_max  82
#define curr_mul_tip  80
#define curr_mul_min  78



//#define  Vin_solar   0//ADC_0
//#define  Vin_battery 1//ADC_1
//#define  Cin_solar   2//ADC_2
//#define  Cin_battery 3//ADC_3
//#define  Vin_485     4//ADC_4
//#define  Vin_temp    5//ADC_5

#define SOLAR_PANEL_VOLTAGE_PIN A0
#define BATTERY_VOLTAGE_PIN A1
#define SOLAR_PANEL_INPUT_CURRENT_SENSOR A2
#define BATTERY_OUPUT_CURRENT_SENSOR A3
#define RS485_SOURCE_VOLTAGE_PIN A4
//#define LOCK_CAPACITOR_PIN 5
//amplitude current


#define mascara_pin 0x00ff
#define PA  0x0000
#define PB  0x0100
#define PC  0x0200
#define PD  0x0300
#define PE  0x0400
#define PF  0x0500
#define PL  0x0600
#define PK  0x0700
#define PJ  0x0800
#define PH  0x0900
#define PG  0x1000

#define mascara_puerto  0xff00


/*
  #define flow_1_int  INT7
  #define flow_2_int  INT6
  #define flow_3_int  INT5
  #define flow_4_int  INT4
  #define flow_5_int  INT3
  #define flow_6_int  INT2
 */
#define  ena_50  PL | BIT0 // 35
#define  ena_33  PL | BIT1 // 36

#define  usbpwren  PK | BIT0 // 89
#define  rtc_int  PK | BIT1 // 88
#define  nfc_int  PK | BIT2 // 87
#define  sd_det  PK | BIT3 // 86
#define  sw_stat  PK | BIT4 // 85

#define  nfc_ena  PL | BIT2 // 37
#define  sw_ena  PB | BIT5 // 24
#define   spi_ss  19//const byte  spi_ss[2]={PB,BIT0};// 19
#define  spi_ss_2 PB | BIT0

#define  en_485  PJ | BIT2 // 65
#define  en_logic  PH | BIT2 // 14

#define  led1  PB | BIT6 // 25
#define  led2  PB | BIT7 // 26

#define adc_0  PF | BIT0
#define adc_1  PF | BIT1
#define adc_2  PF | BIT2
#define adc_3  PF | BIT3
#define adc_4  PF | BIT4
#define adc_5  PF | BIT5

#define  Vin_solar   0//ADC_0
#define  Vin_battery 1//ADC_1
#define  Cin_solar   2//ADC_2
#define  Cin_battery 3//ADC_3
#define  Vin_485     4//ADC_4
#define  Vin_temp    5//ADC_5


long wpsAlertTime=0L;
char *faultData;
int delayTime=1;

AquabubblerTelephatonPowerManager::AquabubblerTelephatonPowerManager(LCDDisplay& l, SecretManager& s, DataStorageManager& sd, TimeManager& t,HardwareSerial& serial ):PowerManager(l,  s,  sd,  t, serial ), lcd(l),secretManager(s), dataStorageManager(sd),timeManager(t)
{}




void AquabubblerTelephatonPowerManager::start(){


	pinMode(led1, OUTPUT);
	pinMode(led2, OUTPUT);

	pinMode(adc_0, INPUT);
	pinMode(adc_1, INPUT);
	pinMode(adc_2, INPUT);
	pinMode(adc_3, INPUT);
	pinMode(adc_4, INPUT);
	pinMode(adc_5, INPUT);

//	pinMode(flow_1, INPUT);
//	pinMode(flow_2, INPUT);
//	pinMode(flow_3, INPUT);
//	pinMode(flow_4, INPUT);
//	pinMode(flow_5, INPUT);
//	pinMode(flow_6, INPUT);

	_HardSerial.print("before interru=");

	//    //interrupciones
	  //    /*Table 15-3. Interrupt Sense Control(1)
	  //      ISCn1 ISCn0 Description
	  //      0 0 The low level of INTn generates an interrupt request
	  //      0 1 Any logical change on INTn generates an interrupt request
	  //      1 0 The falling edge between two samples of INTn generates an interrupt request
	  //      1 1 The rising edge between two samples of INTn generates an interrupt request
	  //    */
	//      EICRA = (1 << ISC31) | (1 << ISC21);
	//      EICRB = (1 << ISC71) | (1 << ISC61) | (1 << ISC51) | (1 << ISC41);
	//      EIMSK = (1 << INT7) | (1 << INT6) | (1 << INT5) | (1 << INT4) | (1 << INT3) | (1 << INT2);
	  //
	  //
	  //    ///////// Comunication configuration
	  //    //serial configuration

		_HardSerial.print("after interr=");

	pinMode(en_logic, OUTPUT);
	digitalWrite(en_logic, HIGH);

	pinMode(nfc_ena,OUTPUT);
	digitalWrite(nfc_ena,LOW);

	delay(300);
	digitalWrite(en_logic, LOW);
	digitalWrite(nfc_ena,HIGH);
	pinMode(spi_ss_2, OUTPUT);

	digitalWrite(led1, HIGH);
	digitalWrite(led2, HIGH);
	delay(2000);

	digitalWrite(led1, LOW);
	digitalWrite(led2, LOW);


}

void AquabubblerTelephatonPowerManager::blinkLed(uint16_t time){
	digitalWrite(led1, HIGH);
	digitalWrite(led2, LOW);
	delay(time);
	digitalWrite(led1, LOW);
	digitalWrite(led2, LOW);
}

PowerStatusStruct AquabubblerTelephatonPowerManager::getPowerStatusStruct(){
	float batteryVoltage = getEnergyStorageVoltage();

	float capacitorVoltage= 0.0;//getLockCapacitorVoltage();
	byte internalEnergyStorageStateOfCharge = GeneralFunctions::getStateOfCharge(batteryVoltage);
	//float regulatorVoltage = getVoltageRegulatorOutput();
	//long totalDiskUse=dataStorageManager.getDiskUsage()/1024;

	aPowerStatusStruct.sampleTime = timeManager.getCurrentTimeInSeconds();
	aPowerStatusStruct.energyStorageVoltage= batteryVoltage;
	aPowerStatusStruct.energySourceVoltage= getEnergySourceVoltage();
	aPowerStatusStruct.currentFromEnergySource= getCurrentInputFromEnergySource();
	aPowerStatusStruct.currentFromEnergyStorage=getCurrentFromEnergyStorage();
	aPowerStatusStruct.lockCapacitorVoltage=capacitorVoltage;
	aPowerStatusStruct.internalEnergyStorageStateOfCharge=internalEnergyStorageStateOfCharge;
	//aPowerStatusStruct.regulatorVoltage=regulatorVoltage;
	aPowerStatusStruct.operatingStatus=operatingStatus;

	return aPowerStatusStruct;
}




void AquabubblerTelephatonPowerManager::printPowerStatusStructToSerialPort(){
	PowerStatusStruct aPowerStatusStruct =  getPowerStatusStruct();
	_HardSerial.print(aPowerStatusStruct.sampleTime) ;
	_HardSerial.print("#") ;
	//
	// Sensor Request Queue Position 1
	//
	char solarPanelVoltageStr[15];
	dtostrf(aPowerStatusStruct.energySourceVoltage,4, 1, solarPanelVoltageStr);
	_HardSerial.print(solarPanelVoltageStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 2
	//
	char batteryVoltageStr[15];
	dtostrf(aPowerStatusStruct.energyStorageVoltage,4, 1, batteryVoltageStr);
	_HardSerial.print(batteryVoltageStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 3
	//
	char solarCurrentValueStr[15];
	dtostrf(aPowerStatusStruct.currentFromEnergySource,4, 0, solarCurrentValueStr);
	_HardSerial.print(solarCurrentValueStr) ;
	_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 4
	//
	char batteryOutputCurrentValueStr[15];
	dtostrf(aPowerStatusStruct.currentFromEnergyStorage,4, 0, batteryOutputCurrentValueStr);
	_HardSerial.print(solarCurrentValueStr) ;
	_HardSerial.print("#") ;


	//
	// Sensor Request Queue Position 5
	//
	char capacitorVoltageStr[15];
	dtostrf(aPowerStatusStruct.lockCapacitorVoltage,2, 1, capacitorVoltageStr);
	_HardSerial.print(capacitorVoltageStr) ;
	_HardSerial.print("#") ;


	//
	// Sensor Request Queue Position 6
	//
	_HardSerial.print( aPowerStatusStruct.internalEnergyStorageStateOfCharge);
	_HardSerial.print("#") ;



	_HardSerial.print(aPowerStatusStruct. operatingStatus);
	_HardSerial.print("#") ;
}


PowerStatisticsStruct AquabubblerTelephatonPowerManager::getPowerStatisticsStruct(){
	//	long totalDiskUse=dataStorageManager.getDiskUsage()/1024;
	PowerStatisticsStruct powerStatisticsStruct;
	powerStatisticsStruct.sampleTime = timeManager.getCurrentTimeInSeconds();
	powerStatisticsStruct.dailyMinEnergyStorageVoltage=this->dailyMinEnergyStorageVoltage;
	powerStatisticsStruct.dailyMaxEnergyStorageVoltage=this->dailyMaxEnergyStorageVoltage;
	powerStatisticsStruct.dailyMinEnergyStorageCurrent=this->dailyMinEnergyStorageCurrent;
	powerStatisticsStruct.dailyMaxEnergyStorageCurrent=this->dailyMaxEnergyStorageCurrent;
	powerStatisticsStruct.dailyEnergyStorageOutEnergy=this->dailyEnergyStorageOutEnergy;
	powerStatisticsStruct.dailyPoweredDownInLoopSeconds=this->dailyPoweredDownInLoopSeconds;
	powerStatisticsStruct.hourlyEnergyStorageOutEnergy=this->hourlyEnergyStorageOutEnergy;
	powerStatisticsStruct.hourlyPoweredDownInLoopSeconds=this->hourlyPoweredDownInLoopSeconds;

	//	aPowerStatusStruct.totalDiskUse=totalDiskUse;

	return powerStatisticsStruct;
}

void AquabubblerTelephatonPowerManager::printPowerStatisticsStructToSerialPort(){
	//
	// Sensor Request Queue Position 1
	//
	PowerStatisticsStruct aPowerStatisticsStruct=getPowerStatisticsStruct();
	char dailyMinEnergyStorageVoltageStr[15];
	dtostrf(aPowerStatisticsStruct.dailyMinEnergyStorageVoltage,4, 0, dailyMinEnergyStorageVoltageStr);
	this->_HardSerial.print(dailyMinEnergyStorageVoltageStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 2
	//

	char dailyMaxEnergyStorageVoltageStr[15];
	dtostrf(aPowerStatisticsStruct.dailyMaxEnergyStorageVoltage,4, 0, dailyMaxEnergyStorageVoltageStr);
	this->_HardSerial.print(dailyMaxEnergyStorageVoltageStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 3
	//

	char dailyMinEnergyStorageCurrentStr[15];
	dtostrf(aPowerStatisticsStruct.dailyMinEnergyStorageCurrent,4, 0, dailyMinEnergyStorageCurrentStr);
	this->_HardSerial.print(dailyMinEnergyStorageCurrentStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 4
	//

	char dailyMaxEnergyStorageCurrentStr[15];
	dtostrf(dailyMaxEnergyStorageCurrent,4, 0, dailyMaxEnergyStorageCurrentStr);
	this->_HardSerial.print(dailyMaxEnergyStorageCurrentStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 5
	//

	char dailyEnergyStorageOutEnergyStr[15];
	dtostrf(dailyEnergyStorageOutEnergy,4, 2, dailyEnergyStorageOutEnergyStr);
	this->_HardSerial.print(dailyEnergyStorageOutEnergyStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 6
	//

	char dailyPoweredDownInLoopSecondsStr[15];
	dtostrf(dailyPoweredDownInLoopSeconds,4, 0, dailyPoweredDownInLoopSecondsStr);
	this->_HardSerial.print(dailyPoweredDownInLoopSecondsStr) ;
	this->_HardSerial.print("#") ;

	//
	// Sensor Request Queue Position 7
	//

	char hourlyEnergyStorageOutEnergyStr[15];
	dtostrf(hourlyEnergyStorageOutEnergy,4, 2, hourlyEnergyStorageOutEnergyStr);
	this->_HardSerial.print(hourlyEnergyStorageOutEnergyStr) ;
	this->_HardSerial.print("#") ;
	//
	// Sensor Request Queue Position 8
	//

	char hourlyPoweredDownInLoopSecondsStr[15];
	dtostrf(aPowerStatisticsStruct.hourlyPoweredDownInLoopSeconds,4, 0, hourlyPoweredDownInLoopSecondsStr);
	this->_HardSerial.print(hourlyPoweredDownInLoopSecondsStr) ;
	this->_HardSerial.print("#") ;


}
uint8_t AquabubblerTelephatonPowerManager::getEnergySourceType(){
	return SOLAR_ENERGY_SOURCE;
}

uint8_t AquabubblerTelephatonPowerManager::getEnergyStorageType(){
	return BATTERY_ENERGY_STORAGE;
}


void AquabubblerTelephatonPowerManager::printSOS()
{
	for(int i=0;i<3;i++){
		blinkLed(500);
	}

	for(int i=0;i<3;i++){
		blinkLed(2000);
	}

	for(int i=0;i<3;i++){
		blinkLed(500);
	}

}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
/*
   Codigo agregado para hacer compatible la configuracion de puertos
   La salida digitales y entradas


 */
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void AquabubblerTelephatonPowerManager::digitalWrite(int port, byte state)//PinWrite(int port, byte state)
{

	int port_2;

	port_2 = port & mascara_puerto;

	if (port_2 == PA) {
		if (state == HIGH) {
			PORTA |= port & mascara_pin;
		}
		else {
			PORTA &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PB) {
		if (state == HIGH) {
			PORTB |= port & mascara_pin;
		}
		else {
			PORTB &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PC) {
		if (state == HIGH) {
			PORTC |= port & mascara_pin;
		}
		else {
			PORTC &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PD) {
		if (state == HIGH) {
			PORTD |= port & mascara_pin;
		}
		else {
			PORTD &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PE) {
		if (state == HIGH) {
			PORTE |= port & mascara_pin;
		}
		else {
			PORTE &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PF) {
		if (state == HIGH) {
			PORTF |= port & mascara_pin;
		}
		else {
			PORTF &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PL) {
		if (state == HIGH) {
			PORTL |= port & mascara_pin;
		}
		else {
			PORTL &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PK) {
		if (state == HIGH) {
			PORTK |= port & mascara_pin;
		}
		else {
			PORTK &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PJ) {
		if (state == HIGH) {
			PORTJ |= port & mascara_pin;
		}
		else {
			PORTJ &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PH) {
		if (state == HIGH) {
			PORTH |= port & mascara_pin;
		}
		else {
			PORTH &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PG) {
		if (state == HIGH) {
			PORTG |= port & mascara_pin;
		}
		else {
			PORTG &= ~(port & mascara_pin);
		}
	}





}

void AquabubblerTelephatonPowerManager::pinMode(int port, byte state)//PortMode(int port, byte state)
{
	int port_2;

	port_2 = port & mascara_puerto;

	if (port_2 == PA) {
		if (state == OUTPUT) {
			DDRA |= port & mascara_pin;
		}
		else {
			DDRA &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PB) {
		if (state == OUTPUT) {
			DDRB |= port & mascara_pin;
		}
		else {
			DDRB &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PC) {
		if (state == OUTPUT) {
			DDRC |= port & mascara_pin;
		}
		else {
			DDRC &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PD) {
		if (state == OUTPUT) {
			DDRD |= port & mascara_pin;
		}
		else {
			DDRD &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PE) {
		if (state == OUTPUT) {
			DDRE |= port & mascara_pin;
		}
		else {
			DDRE &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PF) {
		if (state == OUTPUT) {
			DDRF |= port & mascara_pin;
		}
		else {
			DDRF &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PL) {
		if (state == OUTPUT) {
			DDRL |= port & mascara_pin;
		}
		else {
			DDRL &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PK) {
		if (state == OUTPUT) {
			DDRK |= port & mascara_pin;
		}
		else {
			DDRK &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PJ) {
		if (state == OUTPUT) {
			DDRJ |= port & mascara_pin;
		}
		else {
			DDRJ &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PH) {
		if (state == OUTPUT) {
			DDRH |= port & mascara_pin;
		}
		else {
			DDRH &= ~(port & mascara_pin);
		}
	}

	if (port_2 == PG) {
		if (state == OUTPUT) {
			DDRG |= port & mascara_pin;
		}
		else {
			DDRG &= ~(port & mascara_pin);
		}
	}


}

////////////////////////////////////////////
/*
    Fin de codigo adicional para hacer compatible al micro con arduino nativo

 */
///////////////////////////////////////////

void AquabubblerTelephatonPowerManager::hourlyTasks(long time, int previousHour ){

	dataStorageManager.storeRememberedValue(time,HOURLY_ENERGY, hourlyEnergyStorageOutEnergy, UNIT_MILLI_AMPERES_HOURS);
	dataStorageManager.storeRememberedValue(time,HOURLY_POWERED_DOWN_IN_LOOP_SECONDS, hourlyPoweredDownInLoopSeconds, UNIT_SECONDS);
	dataStorageManager.storeRememberedValue(time,HOURLY_OPERATING_IN_LOOP_SECONDS, 3600-hourlyPoweredDownInLoopSeconds, UNIT_SECONDS);
	hourlyEnergyStorageOutEnergy=0;
	hourlyPoweredDownInLoopSeconds=0;
}

/*
 * this function is called at the beginning of a new day
 */
void AquabubblerTelephatonPowerManager::dailyTasks(long time, int yesterdayDate, int yesterdayMonth, uint16_t yesterdayYear ){
	//
	// move whatever is in untrasferred to the correct date
	boolean result = dataStorageManager.readUntransferredFileFromSDCardByDate( 1,false, RememberedValueDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear );
	result = dataStorageManager.readUntransferredFileFromSDCardByDate( 1,false, WPSSensorDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear);
	result = dataStorageManager.readUntransferredFileFromSDCardByDate( 1,false, LifeCycleDataDirName,yesterdayDate, yesterdayMonth, yesterdayYear);
	long yesterdayDateSeconds = timeManager.dateAsSeconds(yesterdayYear,yesterdayMonth,yesterdayDate, 0, 0, 0);
	dataStorageManager.storeRememberedValue(time,DAILY_STATS_TIMESTAMP, yesterdayDateSeconds, UNIT_NO_UNIT);
	dataStorageManager.storeRememberedValue(time,DAILY_MINIMUM_BATTERY_VOLTAGE, dailyMinEnergyStorageVoltage, UNIT_VOLT);
	dataStorageManager.storeRememberedValue(time,DAILY_MAXIMUM_BATTERY_VOLTAGE, dailyMaxEnergyStorageVoltage, UNIT_VOLT);
	dataStorageManager.storeRememberedValue(time,DAILY_MINIMUM_BATTERY_CURRENT, dailyMinEnergyStorageCurrent, UNIT_MILLI_AMPERES);
	dataStorageManager.storeRememberedValue(time,DAILY_MAXIMUM_BATTERY_CURRENT, dailyMaxEnergyStorageCurrent, UNIT_MILLI_AMPERES);
	dataStorageManager.storeRememberedValue(time,DAILY_ENERGY, dailyEnergyStorageOutEnergy, UNIT_MILLI_AMPERES_HOURS);
	dataStorageManager.storeRememberedValue(time,DAILY_POWERED_DOWN_IN_LOOP_SECONDS, dailyPoweredDownInLoopSeconds, UNIT_SECONDS);
	dailyMinEnergyStorageVoltage = 9999.0;
	dailyMaxEnergyStorageVoltage = -1.0;
	dailyMinEnergyStorageCurrent = 9999.0;
	dailyMaxEnergyStorageCurrent = -1.0;
	dailyEnergyStorageOutEnergy=0.0;
	dailyPoweredDownInLoopSeconds=0.0;

}

void AquabubblerTelephatonPowerManager::monthlyTasks(long time){

}

void AquabubblerTelephatonPowerManager::yearlyTasks(long time){

}


bool AquabubblerTelephatonPowerManager::canPublish(){
	if(getEnergyStorageVoltage()>12.4){
		return true;
	}else{
		return false;
	}
}




float AquabubblerTelephatonPowerManager::getEnergySourceVoltage(){


	int32_t  prosc = analogRead(A0);
	prosc *= volt_ref;
	prosc *= volt_mult;
	prosc /= adc_res;
	prosc /= 1000;
	return prosc;
}


float AquabubblerTelephatonPowerManager::getEnergyStorageVoltage(){


	int32_t  prosc = analogRead(A1);
	prosc *= volt_ref;
	prosc *= volt_mult;
	prosc /= adc_res;
	prosc /= 1000;
	return prosc;

}

float AquabubblerTelephatonPowerManager::getCurrentInputFromEnergySource(void){

	int sensorValue;             //value read from the sensor
	int sensorMax = 0;
	uint32_t start_time = millis();
	while((millis()-start_time) < 100)//sample for 1000ms
	{
		sensorValue = analogRead(A2);
		if (sensorValue > sensorMax)
		{
			//record the maximum sensor value
			sensorMax = sensorValue;
		}
	}

	//the VCC on the Grove interface of the sensor is 5v
	float solar_amplitude_current=(float)(sensorMax-512)/1024*5/185*1000000;
	float effective_value=solar_amplitude_current/1.414;
	return effective_value;
}

float AquabubblerTelephatonPowerManager::getCurrentFromEnergyStorage(void){

	int sensorValue;             //value read from the sensor
	int sensorMax = 0;
	uint32_t start_time = millis();
	while((millis()-start_time) < 100)//sample for 1000ms
	{
		sensorValue = analogRead(A3);
		if (sensorValue > sensorMax)
		{
			//record the maximum sensor value
			sensorMax = sensorValue;
		}
	}

	//the VCC on the Grove interface of the sensor is 5v
	float amplitude_current=(float)(sensorMax-512)/1024*5/185*1000000;
	float effective_value=amplitude_current/1.414;
	return effective_value;

	//	int32_t prosc = analogRead(A3);//BATTERY_OUPUT_CURRENT_SENSOR);
	//
	////	  prosc *= volt_ref;
	////	  prosc *= 100;
	////	  prosc /= adc_res;
	////	  prosc -= curr_ref_tip;
	////	  prosc *= 100;
	////	  prosc /= curr_mul_tip;
	//	 return (float)prosc;



}

PowerStatusStruct AquabubblerTelephatonPowerManager::getPowerStatusStruct(){
	float batteryVoltage = getEnergyStorageVoltage();
	int currentValue = getCurrentFromEnergyStorage();
	float capacitorVoltage= 0.0;//getLockCapacitorVoltage();
	byte internalEnergyStorageStateOfCharge = GeneralFunctions::getStateOfCharge(batteryVoltage);
	//float regulatorVoltage = getVoltageRegulatorOutput();
	//long totalDiskUse=dataStorageManager.getDiskUsage()/1024;

	aPowerStatusStruct.sampleTime = timeManager.getCurrentTimeInSeconds();
	aPowerStatusStruct.energyStorageVoltage= batteryVoltage;
	aPowerStatusStruct.energySourceVoltage= getEnergySourceVoltage();
	aPowerStatusStruct.currentFromEnergyStorage=currentValue;
	aPowerStatusStruct.currentFromEnergySource=getCurrentInputFromEnergySource();
	aPowerStatusStruct.lockCapacitorVoltage=capacitorVoltage;
	aPowerStatusStruct.internalEnergyStorageStateOfCharge=internalEnergyStorageStateOfCharge;
	//aPowerStatusStruct.regulatorVoltage=0.0;//regulatorVoltage;
	aPowerStatusStruct.operatingStatus=operatingStatus;

	return aPowerStatusStruct;
}

PowerStatisticsStruct AquabubblerTelephatonPowerManager::getPowerStatisticsStruct(){
	//	long totalDiskUse=dataStorageManager.getDiskUsage()/1024;

	aPowerStatisticsStruct.sampleTime = timeManager.getCurrentTimeInSeconds();
	aPowerStatisticsStruct.dailyMinEnergyStorageVoltage=dailyMinEnergyStorageVoltage;
	aPowerStatisticsStruct.dailyMaxEnergyStorageVoltage=dailyMaxEnergyStorageVoltage;
	aPowerStatisticsStruct.dailyMinEnergyStorageCurrent=dailyMinEnergyStorageCurrent;
	aPowerStatisticsStruct.dailyMaxEnergyStorageCurrent=dailyMaxEnergyStorageCurrent;
	aPowerStatisticsStruct.dailyEnergyStorageOutEnergy=dailyEnergyStorageOutEnergy;
	aPowerStatisticsStruct.dailyPoweredDownInLoopSeconds=dailyPoweredDownInLoopSeconds;
	aPowerStatisticsStruct.hourlyEnergyStorageOutEnergy=hourlyEnergyStorageOutEnergy;
	aPowerStatisticsStruct.hourlyPoweredDownInLoopSeconds=hourlyPoweredDownInLoopSeconds;

	//	aPowerStatusStruct.totalDiskUse=totalDiskUse;

	return aPowerStatisticsStruct;
}

void AquabubblerTelephatonPowerManager::endOfLoopProcessing(){

	long now = millis();//timeManager.getCurrentTimeInSeconds();
	int loopConsumingPowerMilliSeconds = now -loopStartingSeconds -poweredDownInLoopSeconds;
	float currentFromEnergyStorage = getCurrentFromEnergyStorage();
	float currentFromSolarPanel = getCurrentInputFromEnergySource();

//	_HardSerial.println(" ");
//	//
//	_HardSerial.print(" endOfLoopProcessing, now=");
//	_HardSerial.print(now);
//	_HardSerial.print(" currentFromEnergyStorage=");
//	_HardSerial.print(currentFromEnergyStorage);
//	//
//	_HardSerial.print(" loopStartingSeconds=");
//	_HardSerial.print(loopStartingSeconds);
//	//
//	//
//	//  _Hard//serial.print(" poweredDownInLoopSeconds=");
//	//  _Hard//serial.print(poweredDownInLoopSeconds);
//	_HardSerial.print(" loopConsumingPowerSeconds=");
//	_HardSerial.println(loopConsumingPowerMilliSeconds);
//
//


	dailyEnergyStorageOutEnergy+= loopConsumingPowerMilliSeconds*currentFromEnergyStorage/3600000;
	hourlyEnergyStorageOutEnergy+= loopConsumingPowerMilliSeconds*currentFromEnergyStorage/3600000;
	dailyEnergyStorageInEnergy+= loopConsumingPowerMilliSeconds*currentFromSolarPanel/3600000;
	hourlyEnergyStorageInEnergy+= loopConsumingPowerMilliSeconds*currentFromSolarPanel/3600000;
	dailyPoweredDownInLoopSeconds+=poweredDownInLoopSeconds;
	hourlyPoweredDownInLoopSeconds+=poweredDownInLoopSeconds;
//	_HardSerial.print("dailyEnergyStorageOutEnergy=");
//	_HardSerial.println(dailyEnergyStorageOutEnergy);

//	_HardSerial.print(" dailyBatterInEnergy=");
//	_HardSerial.println(dailyEnergyStorageInEnergy);
	digitalWrite(led1, LOW);
	digitalWrite(led2, LOW);
}


boolean AquabubblerTelephatonPowerManager::processDefaultCommands(String command){
	this->_HardSerial.println("starting process default ");
	this->_HardSerial.println(command);

	boolean processed=false;
	if(command=="TestWPSSensor"){
		float batteryVoltage = getEnergyStorageVoltage();
		float regulatorVoltage = getVoltageRegulatorOutput();

		float current = getCurrentFromEnergyStorage();
		int stateOfCharge= GeneralFunctions::getStateOfCharge(batteryVoltage);
		boolean result = dataStorageManager.testWPSSensor( batteryVoltage,  current,  stateOfCharge,  operatingStatus);
		if(result){
			this->_HardSerial.println("Ok-TestWPSSensor");
		}else{
			this->_HardSerial.println("Failure-TestWPSSensor");
		}
		this->_HardSerial.flush();
		processed=true;
	}else if(command=="TestLifeCycle"){
		long now = timeManager.getCurrentTimeInSeconds();
		dataStorageManager.storeLifeCycleEvent(now, LIFE_CYCLE_EVENT_END_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
		_HardSerial.println("Ok-TestLifeCycle");
		_HardSerial.flush();
		processed=true;
	}else if(command=="ListFiles"){
		this->_HardSerial.println(" ");
		this->_HardSerial.println(" ");

		float total = dataStorageManager.listFiles();


		this->_HardSerial.println(" ");

		this->_HardSerial.print("Used (Kb):  ");
		this->_HardSerial.println(total);

		this->_HardSerial.println("");
		this->	_HardSerial.println("Ok-ListFiles");
		this->_HardSerial.flush();
		processed=true;
	}else if(command=="Ping"){

		this->_HardSerial.println("Ok-Ping");
		this->_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("SetTime")){


		boolean result = timeManager.setTime(command);
		if(result){
			this->_HardSerial.println("Ok-SetTime");
		}else{
			this->_HardSerial.println("Failure-SetTime");
		}

		_HardSerial.flush();

		processed=true;

	}else if(command.startsWith("GetTime")){
		String time = timeManager.getCurrentDateTimeForDisplay();
		this->_HardSerial.println(time);
		this->_HardSerial.flush();
		this->_HardSerial.println("Ok-GetTime");
		this->_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("EnterWPS")){
		//EnterWPS#10#45#30#1
		secondsToTurnPowerOff = (long)GeneralFunctions::getValue(command, '#', 1).toInt();
		secondsToNextPiOn = (long)GeneralFunctions::getValue(command, '#', 2).toInt();
		wpsPulseFrequencySeconds = GeneralFunctions::getValue(command, '#', 3).toInt();
		int pauseDuringWPSi = GeneralFunctions::getValue(command, '#', 4).toInt();
		if(pauseDuringWPSi==1)pauseDuringWPS=true;
		else pauseDuringWPS=false;
		waitingForWPSConfirmation=false;
		wpsCountdown=true;
		operatingStatus=2;
		wpsCountDownStartSeconds= timeManager.getCurrentTimeInSeconds();
		currentSecondsToPowerOff=0L;

		this->_HardSerial.println("Ok-EnterWPS");
		this->_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("ExitWPS")){

		this->_HardSerial.println("Ok-ExitWPS");
		this->_HardSerial.flush();
		inWPS=false;
		operatingStatus=3;
		currentSecondsToPowerOff=0L;
		wpsCountdown=false;
		processed=true;
	}else if(command.startsWith("UpdateWPSParameters")){
		String minWPSVoltageS = GeneralFunctions::getValue(command, '#', 1);
		char buffer[10];
		minWPSVoltageS.toCharArray(buffer, 10);
		minWPSVoltage = atof(buffer);

		minWPSVoltage = GeneralFunctions::stringToFloat(GeneralFunctions::getValue(command, '#', 1));
		enterWPSVoltage = GeneralFunctions::stringToFloat(GeneralFunctions::getValue(command, '#', 2));
		exitWPSVoltage = GeneralFunctions::stringToFloat(GeneralFunctions::getValue(command, '#', 3));

		this->secondsToForcedWPS = GeneralFunctions::getValue(command, '#', 4).toInt();
		_HardSerial.println("Ok-UpdateWPSParameters");
		_HardSerial.flush();

		processed=true;

	}else if(command.startsWith("GetRememberedValueData")){
		//GetRememberedValueData#0
		int transferData = GeneralFunctions::getValue(command, '#', 1).toInt();
		boolean result = dataStorageManager.readUntransferredFileFromSDCard( transferData,true, RememberedValueDataDirName);
		if(result){
			_HardSerial.println("Ok-GetRememberedValueData");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", RememberedValueDataDirName, unstraferedFileName);
			_HardSerial.println(text);
		}
		_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetLifeCycleData")){
		//GetLifeCycleData#0
		int transferData = GeneralFunctions::getValue(command, '#', 1).toInt();
		boolean result = dataStorageManager.readUntransferredFileFromSDCard( transferData,true, LifeCycleDataDirName);
		if(result){
			this->_HardSerial.println("Ok-GetLifeCycleData");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", LifeCycleDataDirName, unstraferedFileName);
			this->_HardSerial.println(text);
		}
		this->_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetWPSSensorData")){
		//GetWPSSensorData#0
		//GetLifeCycleData#0
		int transferData = GeneralFunctions::getValue(command, '#', 1).toInt();
		boolean result = dataStorageManager.readUntransferredFileFromSDCard( transferData,true, WPSSensorDataDirName);
		if(result){
			this->_HardSerial.println("Ok-GetWPSSensorData");
		}else {

			char text[44];
			snprintf(text, sizeof text, "Failure-error opening /%s/%s", WPSSensorDataDirName, unstraferedFileName);
			this->_HardSerial.println(text);

		}
		this->_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetHistoricalWPSSensorData")){

		int date = GeneralFunctions::getValue(command, '#', 1).toInt();
		int month = GeneralFunctions::getValue(command, '#', 2).toInt();
		int year = GeneralFunctions::getValue(command, '#', 3).toInt();
		boolean result  =dataStorageManager.getHistoricalData( WPSSensorDataDirName,  date,  month,  year);
		if(result){
			this->_HardSerial.println("Ok-GetWPSSensorDataHistory");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", WPSSensorDataDirName, unstraferedFileName);

			this->_HardSerial.println(text);
		}
		this->_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetHistoricalLifeCycleData")){
		//GetHistoricalLifeCycleData#12#1#19
		int date = GeneralFunctions::getValue(command, '#', 1).toInt();
		int month = GeneralFunctions::getValue(command, '#', 2).toInt();
		int year = GeneralFunctions::getValue(command, '#', 3).toInt();
		boolean result  = dataStorageManager.getHistoricalData( LifeCycleDataDirName,  date,  month,  year);
		if (result) {
			this->_HardSerial.println("Ok-GetHistoricalLifeCycleData");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", LifeCycleDataDirName, unstraferedFileName);
			this->_HardSerial.println(text);
		}
		this->_HardSerial.flush();
		processed=true;
	}else if(command.startsWith("GetHistoricalRememberedValueData")){
		//GetHistoricalLifeCycleData#12#1#19
		int date = GeneralFunctions::getValue(command, '#', 1).toInt();
		int month = GeneralFunctions::getValue(command, '#', 2).toInt();
		int year = GeneralFunctions::getValue(command, '#', 3).toInt();
		boolean result  = dataStorageManager.getHistoricalData( RememberedValueDataDirName,  date,  month,  year);
		if (result) {
			this->_HardSerial.println("Ok-GetHistoricalRememberedValueData");
		}else {
			char text[44];
			snprintf(text, sizeof text, "Failure-error opening %s/%s", RememberedValueDataDirName, unstraferedFileName);

			this->_HardSerial.println(text);
		}
		this->_HardSerial.flush();
		processed=true;
	}else if (command == "AsyncData" ){
		this->_HardSerial.println("Ok-No Data");
		this->_HardSerial.flush();
		processed=true;
	}else if (command.startsWith("FaultData") ){
		//_HardSerial.println(faultData);
		if(faultData=="Enter WPS"){

			this->_HardSerial.print("Fault#WPS Alert#Enter WPS#");
			this->_HardSerial.print(secretManager.generateCode());

			this->_HardSerial.print("#@On Load:Notify And Shutdown:Voltage At WPS#");
			this->_HardSerial.println(getEnergyStorageVoltage());
			waitingForWPSConfirmation=true;

		}else{
			this->_HardSerial.println("Ok");
		}

		this->_HardSerial.flush();
		faultData="";
		delay(delayTime);
		processed=true;
	}else if (command.startsWith("TimerStatus") ){
		//
		// in the case that a teleonome does have
		//human interface buttons connected to the microcontrller
		// or there is a timer, here is where it will be
		this->_HardSerial.println("Ok-TimerStatus");
		this->_HardSerial.flush();
		delay(delayTime);
		processed=true;

	}
	return processed;
}

void AquabubblerTelephatonPowerManager::defineState(){
	this->poweredDownInLoopSeconds=0;
	this->loopStartingSeconds = millis();//timeManager.getCurrentTimeInSeconds();

	float batteryVoltage = getEnergyStorageVoltage();
	int internalEnergyStorageStateOfCharge = generalFunctions.getStateOfCharge(batteryVoltage);
	float currentFromEnergyStorage = getCurrentFromEnergyStorage();
	float inputFromSOlarPanel =  getCurrentInputFromEnergySource();
	float solarPanelVolltage = getEnergySourceVoltage();

	digitalWrite(led1, HIGH);
	digitalWrite(led2, HIGH);



	if(batteryVoltage>=minWPSVoltage ){
		if(wpsSleeping){
			//delay(1000);
			//lcd.noDisplay();
			long piSleepingRemaining = secondsToNextPiOn-(loopStartingSeconds - currentSleepStartTime) ;



		}


	}else{
		if(wpsSleeping){

			if(f_wdt == 1){
				/* Don't forget to clear the flag. */
				f_wdt = 0;
				/* Re-enter sleep mode. */
				lcd.clear();
				lcd.setRGB(255,0,0);
				lcd.setCursor(0,0);
				lcd.print("Enter Comma");
				operatingStatus=1;
				lcd.setCursor(0,1);
				lcd.print(batteryVoltage);
				lcd.print(" V");
				delay(2000);
				lcd.setRGB(0,0,0);
				lcd.noDisplay();
				dataStorageManager.storeLifeCycleEvent(loopStartingSeconds,LIFE_CYCLE_EVENT_START_COMMA, LIFE_CYCLE_EVENT_COMMA_VALUE);
				enterArduinoSleep();
			}
		}
	}


}




AquabubblerTelephatonPowerManager::~AquabubblerTelephatonPowerManager() {
	// TODO Auto-generated destructor stub
}

