/*
 * eps_safety.c
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */
#include "eps_safety.h"
#include "eps_non_volatile_mem_handling.h"

EPS_soft_error_status EPS_perform_safety_checks(EPS_State *state, EPS_safety_limits *limits){


	//check if battery voltage is lower than bat low limit
	if(state->battery_voltage<limits->limit_battery_low){
		//shutdown everything
	}

	//check if battery voltage is lower than bat critical limit
	if(state->battery_voltage<limits->limit_battery_critical){
		//get in critical mode. leave running only mission critical modules.
	}


	//check if battery voltage is higher than bat high limit
	if(state->battery_voltage>limits->limit_battery_high){
		//stop charging
	}

	//check if battery temperature is lower than bat temperature limit
	if(state->battery_temp<limits->limit_battery_temperature_low){
		//turn heaters on
	}

	//check if battery temperature is higher than bat temperature limit
	if(state->battery_temp>limits->limit_battery_temperature_high){
		//turn heaters off
	}

	return EPS_SOFT_ERROR_OK;
}

//load limits from memory at startup. this is done so if needed to change them for safety issues, then the values wont be again problematic on reset and startup.
void EPS_load_safety_limits_from_memory(EPS_safety_limits *limits){

	uint32_t memory_read_value;

	EPS_get_memory_word( LIMIT_BATTERY_LOW_ADDRESS, &memory_read_value );
	limits->limit_battery_low = memory_read_value;
	EPS_get_memory_word( LIMIT_BATTERY_HIGH_ADDRESS, &memory_read_value );
	limits->limit_battery_high = memory_read_value;
	EPS_get_memory_word( LIMIT_BATTERY_CRITICAL_ADDRESS, &memory_read_value );
	limits->limit_battery_critical = memory_read_value;
	EPS_get_memory_word( LIMIT_BATTERY_TEMPERATURE_LOW_ADDRESS, &memory_read_value );
	limits->limit_battery_temperature_low = memory_read_value;
	EPS_get_memory_word( LIMIT_BATTERY_TEMPERATURE_HIGH_ADDRESS, &memory_read_value );
	limits->limit_battery_temperature_high = memory_read_value;

}
