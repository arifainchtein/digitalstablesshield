#include <TelepathonData.h>

struct GloriaBaseData: public TelepathonData{
  bool relayState;
  float temperature;
  float voltage;
  long secondsTime;
  float tankHeight;
};

struct GloriaFlowData: public TelepathonData{
   long secondsTime;
  float flowRate1;
  float volume1;
  float flowRate2;
  float volume2;

};
