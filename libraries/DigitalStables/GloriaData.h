#include <CommunicatorInitParams.h>

#ifndef GLORIABASEDATA_H
 #define GLORIABASEDATA_H
struct GloriaBaseData{
	 long secondsTime=0L;
  bool relayState=false;
  float temperature=0.0;
  float voltage=0.0;

  float tankHeight=0.0;
};
#endif

#ifndef GLORIAFLOWDATA_H
 #define GLORIAFLOWDATA_H
struct GloriaFlowData{
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
	uint64_t writingPipe=0xF0F0F0F0E1LL; //pipes[0]
	uint64_t readingPipe=0xF0F0F0F0D2LL; //pipes[1]

	uint64_t readingPipePipe=0xF0F0F0F0E1LL; //pipes[0]
	uint64_t writing=0xF0F0F0F0D2LL; //pipes[1]

};
#endif
