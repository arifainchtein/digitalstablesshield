/*
 * LifeEvents.h
 *
 *  Created on: 14 Mar. 2019
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_GENERALCONSTANTS_H_
#define LIBRARIES_DIGITALSTABLES_GENERALCONSTANTS_H_

class GeneralConstants{


public:

	static const char  *WPSSensorDataDirName="WPSSensr";
	static const char  *LifeCycleDataDirName="LifeCycl";
	static const char  *RememberedValueDataDirName  = "RememVal";
	static const char  *unstraferedFileName ="Untransf.txt";

	static const char *DAILY_STATS_TIMESTAMP="Daily Timestamp";
	static const char *DAILY_MINIMUM_BATTERY_VOLTAGE="Daily Minimum Battery Voltage";
	static const char *DAILY_MAXIMUM_BATTERY_VOLTAGE="Daily Maximum Battery Voltage";
	static const char *DAILY_MINIMUM_BATTERY_CURRENT="Daily Minimum Battery Current";
	static const char *DAILY_MAXIMUM_BATTERY_CURRENT="Daily Maximum Battery Current";
	static const char *DAILY_ENERGY="Daily Energy";
	static const char *DAILY_POWERED_DOWN_IN_LOOP_SECONDS="Hourly Powered Down In Loop Seconds";
	static const char *HOURLY_ENERGY="Hourly Energy";
	static const char *HOURLY_POWERED_DOWN_IN_LOOP_SECONDS="Hourly Powered Down In Loop Seconds";
	static const char *HOURLY_OPERATING_IN_LOOP_SECONDS="Hourly Operating In Loop Seconds";

	static const char *MAXIMUM_VALUE="Max";
	static const char *MINIMUM_VALUE="Min";
	static const char *AVERAGE_VALUE="Avg";

	static const char *UNIT_NO_UNIT =" ";
	static const char *UNIT_VOLT ="Volt";
	static const char *UNIT_SECONDS="seconds";
	static const char *UNIT_MILLI_AMPERES ="mA";
	static const char *UNIT_MILLI_AMPERES_HOURS ="mAh";
	static const char *UNIT_PERCENTAGE ="%";
	static const char *FORCED_PI_TURN_OFF ="Forced Pi Turn Off";
	static const char *BATTERY_VOLTAGE_BEFORE_PI_ON ="Battery Voltage Before Turning Pi On";
	static const char *BATTERY_VOLTAGE_ATER_PI_ON="Battery Voltage After Turning Pi On";
	static const char *BATTERY_VOLTAGE_DIFFERENTIAL_AFTER_PI_ON ="Battery Voltage Differential After Turning Pi On";
	static const char *PI_TURN_OFF ="Pi Turn Off";

	static const char *LIFE_CYCLE_EVENT_FORCED_START_WPS ="Forced Start WPS";
	static const char *LIFE_CYCLE_MANUAL_SHUTDOWN    ="Manual Shutdown";
	static const char *LIFE_CYCLE_EVENT_START_WPS    ="Start WPS";
	static const char *LIFE_CYCLE_EVENT_END_WPS     ="End WPS";
	static const char *LIFE_CYCLE_EVENT_START_COMMA ="Start Comma";

	static const char *LIFE_CYCLE_EVENT_END_COMMA ="End Comma";
	static const char *LIFE_CYCLE_EVENT_SETUP_COMPLETED="Setup Method Completed";

	static const int LIFE_CYCLE_EVENT_AWAKE_VALUE=3;
	static const int LIFE_CYCLE_EVENT_WPS_VALUE=2;
	static const int LIFE_CYCLE_EVENT_COMMA_VALUE=1;

};

#endif /* LIBRARIES_DIGITALSTABLES_GENERALCONSTANTS_H_ */
