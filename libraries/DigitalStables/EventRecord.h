/*
 * This file is generated in the spermatogenesis process
 */
#ifndef EVENTRECORD_H
 #define EVENTRECORD_H
struct EventRecord{
	long startTime;
	long endTime;
	long eventGroupStartTime;

	void reset(){
			::memset(this,0,sizeof(this));
		}

};
#endif
