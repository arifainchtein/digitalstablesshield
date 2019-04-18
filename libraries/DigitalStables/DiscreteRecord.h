 #ifndef DISCRETERECORD_H
 #define DISCRETERECORD_H
struct DiscreetRecrd{

    float lat
	float lon;
    uint16_t adc2;
    long timestamp;
    uint16_t panID;
    uint8_t tagId;
    uint8_t signalStrength;

};
#endif