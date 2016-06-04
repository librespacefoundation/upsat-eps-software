/*
 * eps_safety.c
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */
#include "eps_safety.h"
#include "eps_non_volatile_mem_handling.h"




EPS_soft_error_status EPS_perform_safety_checks(EPS_State *state, EPS_safety_limits *limits){



	EPS_soft_error_status safety_check_status = EPS_SOFT_ERROR_SAFETY_CHECK;

	/* check battery voltage limits */
	if(state->battery_voltage<limits->limit_battery_low){
		/*low bat - shutdown everything*/
		state->EPS_safety_battery_mode = EPS_SAFETY_MODE_BATTERY_DEAD;

		EPS_set_rail_switch(COMM, EPS_SWITCH_RAIL_OFF, state);
		EPS_set_rail_switch(OBC, EPS_SWITCH_RAIL_OFF, state);
		EPS_set_rail_switch(ADCS, EPS_SWITCH_RAIL_OFF, state);
		EPS_set_rail_switch(SU, EPS_SWITCH_RAIL_OFF, state);
		EPS_set_rail_switch(TEMP_SENSOR, EPS_SWITCH_RAIL_OFF, state);


	}
	else if( (state->battery_voltage<limits->limit_battery_critical)&&(state->battery_voltage>limits->limit_battery_low) ){
		/*bat critical - keep alive only basic subsystems*/
		state->EPS_safety_battery_mode = EPS_SAFETY_MODE_BATTERY_CRITICAL;

		EPS_set_rail_switch(COMM, EPS_SWITCH_RAIL_OFF, state);
		EPS_set_rail_switch(OBC, EPS_SWITCH_RAIL_OFF, state);
	}
	else if( (state->battery_voltage<limits->limit_battery_high)&&(state->battery_voltage>limits->limit_battery_critical) ){
		/*normal bat - nominal state*/
		state->EPS_safety_battery_mode = EPS_SAFETY_MODE_BATTERY_NORMAL;

		EPS_set_rail_switch(COMM, EPS_SWITCH_RAIL_ON, state);
		EPS_set_rail_switch(OBC, EPS_SWITCH_RAIL_ON, state);
		EPS_set_rail_switch(ADCS, EPS_SWITCH_RAIL_ON, state);
		/* EPS_set_rail_switch(SU, EPS_SWITCH_RAIL_ON, state); */
		EPS_set_rail_switch(TEMP_SENSOR, EPS_SWITCH_RAIL_ON, state);

	}
	else if( (state->battery_voltage>limits->limit_battery_high) ){
		/*overvoltage bat - overvoltage to the battery bus - shutdown all mppt modules - this is a battery protection fault*/
		state->EPS_safety_battery_mode = EPS_SAFETY_MODE_BATTERY_OVERVOLTAGE;
	}
	else{
		/*undefined state - must resolve this in error handling*/
		state->EPS_safety_battery_mode = EPS_SAFETY_MODE_BATTERY_NOT_SET;
		safety_check_status = EPS_SOFT_ERROR_SAFETY_CHECK_BATTERY_VOLTAGE_UNPREDICTED;
	}



	/* check battery temperature limits */
	if(state->battery_temp<=limits->limit_battery_temperature_low){
		/* too cold -turn heaters on */
		state->EPS_safety_temperature_mode = EPS_SAFETY_MODE_TEMPERATURE_LOW;
	}
	else if(state->battery_temp>=limits->limit_battery_temperature_high){
		/* too hot - close subsystems to reduce heat dissipation */
		state->EPS_safety_temperature_mode = EPS_SAFETY_MODE_TEMPERATURE_HIGH;
	}
	else if( (state->battery_temp>limits->limit_battery_temperature_low)&&(state->battery_temp<limits->limit_battery_temperature_high) ){
		/* temperature within range */
		state->EPS_safety_temperature_mode = EPS_SAFETY_MODE_TEMPERATURE_NORMAL;
		}
	else{
		/* undefined temperature state handle with soft error handling*/
		state->EPS_safety_temperature_mode = EPS_SAFETY_MODE_TEMPERATURE_NOT_SET;
		safety_check_status = EPS_SOFT_ERROR_SAFETY_CHECK_BATTERY_TEMPERATURE_UNPREDICTED;
	}



	safety_check_status = EPS_SOFT_ERROR_OK;
	return safety_check_status;
}

//load limits from memory at startup. this is done so if needed to change them for safety issues, then the values wont be again problematic on reset and startup.
EPS_soft_error_status EPS_load_safety_limits_from_memory(EPS_safety_limits *limits){

	EPS_soft_error_status limits_status = EPS_SOFT_ERROR_LOAD_SAFETY_LIMITS;

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

	limits_status = EPS_SOFT_ERROR_OK;
	return limits_status;

}
