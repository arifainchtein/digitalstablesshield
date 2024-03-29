#include <Arduino.h>
#include <CommunicatorInitParams.h>

#ifndef GLORIAFIELDPOWERDATA_H
#define GLORIAFIELDPOWERDATA_H
struct GloriaFieldPowerData{

	uint8_t gloriaId[8];
	long secondsTime=0L;
	float inputVoltage=0.0;
	float mcuVoltage=0.0;
	float rtcVoltage=0.0;
};
#endif
//
//fdev_setup_stream(&serial_stdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);
//stdout = &serial_stdout;
//printf("This no2w goes to stdout: %d\n", 3);

//FILE serial_stdout;
//int serial_putchar(char c, FILE* f) {
 //  if (c == '\n') serial_putchar('\r', f);
  // return Serial.write(c) == 1? 0 : 1;
//}


#ifndef GLORIAFIELDSENSORDATA_H
#define GLORIAFIELDSENSORDATA_H
struct GloriaFieldSensorData{

	uint8_t gloriaId[8];
	long secondsTime=0L;
	bool relayState=false;
	float temperature=0.0;
	float tankHeight=0.0;

};
#endif


#ifndef GLORIAFIELDFLOWDATA_H
#define GLORIAFIELDFLOWDATA_H
struct GloriaFieldFlowData{
	uint8_t gloriaId[8];
	long secondsTime=0L;
	float flowRate1=0.0;
	float volume1=0.0;
	float flowRate2=0.0;
	float volume2=0.0;

};
#endif


#ifndef GLORIAHOMEBASEDATA_H
#define GLORIAHOMEBASEDATA_H
struct GloriaHomeBaseData{
	bool pumpState=false;
};
#endif

#ifndef GLORIACOMMDATA_H
#define GLORICOMMDATA_H
struct GloriaCommData{
	//	uint64_t writingPipe=0xF0F0F0F0E1LL; //pipes[0]
	//	uint64_t readingPipe=0xF0F0F0F0D2LL; //pipes[1]
	//
	//	uint64_t readingPipePipe=0xF0F0F0F0E1LL; //pipes[0]
	//	uint64_t writing=0xF0F0F0F0D2LL; //pipes[1]

	uint64_t readingPipe; //pipes[0]
	uint64_t writingPipe; //pipes[1]
	uint8_t CHIP_ENABLE=13;//22;
	uint8_t CHIP_SELECT=4;

	GloriaCommData(uint64_t w, uint64_t r,uint8_t ce, uint8_t cs ):writingPipe(w), readingPipe(r),CHIP_ENABLE(ce),CHIP_SELECT(cs) {};

};
#endif
