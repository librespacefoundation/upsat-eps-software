/*
 * eps_safety.c
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */
#include "eps_safety.h"
#include "eps_non_volatile_mem_handling.h"
#include "eps_power_module.h"

extern EPS_PowerModule power_module_top, power_module_bottom, power_module_left, power_module_right;


EPS_soft_error_status EPS_perform_safety_checks(EPS_State *state, EPS_safety_limits *limits){



	EPS_soft_error_status safety_check_status = EPS_SOFT_ERROR_SAFETY_CHECK;

	/* turn on (if previously closed or keep them on - solar cell power modules)*/
	power_module_top.module_state = POWER_MODULE_ON;
	power_module_bottom.module_state = POWER_MODULE_ON;
	power_module_left.module_state = POWER_MODULE_ON;
	power_module_right.module_state = POWER_MODULE_ON;

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
		/* send message to obc that battery is in critical point and it should turn off non critical loads. */



	}
	else if( (state->battery_voltage<limits->limit_battery_high)&&(state->battery_voltage>limits->limit_battery_critical) ){
		/*normal bat - nominal state*/
		state->EPS_safety_battery_mode = EPS_SAFETY_MODE_BATTERY_NORMAL;

		//let all loads to last state - if on keep the on if off keep them off.
//
//		EPS_set_rail_switch(COMM, EPS_SWITCH_RAIL_ON, state);
//		EPS_set_rail_switch(OBC, EPS_SWITCH_RAIL_ON, state);
//		EPS_set_rail_switch(ADCS, EPS_SWITCH_RAIL_ON, state);
//		/* EPS_set_rail_switch(SU, EPS_SWITCH_RAIL_ON, state); */
//		EPS_set_rail_switch(TEMP_SENSOR, EPS_SWITCH_RAIL_ON, state);

	}
	else if( (state->battery_voltage>limits->limit_battery_high) ){
		/*overvoltage bat - overvoltage to the battery bus - shutdown all mppt modules - this is a battery protection fault*/
		power_module_top.module_state = POWER_MODULE_OFF;
		power_module_top.pwm_duty_cycle = MPPT_STARTUP_PWM_DUTYCYCLE;/*set pwm to initial so as to reset mppt when it will startup again.*/

		power_module_bottom.module_state = POWER_MODULE_OFF;
		power_module_bottom.pwm_duty_cycle = MPPT_STARTUP_PWM_DUTYCYCLE;

		power_module_left.module_state = POWER_MODULE_OFF;
		power_module_left.pwm_duty_cycle = MPPT_STARTUP_PWM_DUTYCYCLE;

		power_module_right.module_state = POWER_MODULE_OFF;
		power_module_right.pwm_duty_cycle = MPPT_STARTUP_PWM_DUTYCYCLE;


		state->EPS_safety_battery_mode = EPS_SAFETY_MODE_BATTERY_OVERVOLTAGE;
	}
	else{
		/*undefined state - must resolve this in error handling*/
		state->EPS_safety_battery_mode = EPS_SAFETY_MODE_BATTERY_NOT_SET;
		safety_check_status = EPS_SOFT_ERROR_SAFETY_CHECK_BATTERY_VOLTAGE_UNPREDICTED;

		/*turn off solar power modules to avoid fault at overvoltage*/
		power_module_top.module_state = POWER_MODULE_OFF;
		power_module_bottom.module_state = POWER_MODULE_OFF;
		power_module_left.module_state = POWER_MODULE_OFF;
		power_module_right.module_state = POWER_MODULE_OFF;
	}



	/* check battery temperature limits */
	if(state->battery_temp<=limits->limit_battery_temperature_low){
		/* too cold -turn heaters on */
		state->EPS_safety_temperature_mode = EPS_SAFETY_MODE_TEMPERATURE_LOW;
		EPS_set_control_switch(BATTERY_HEATERS, EPS_SWITCH_CONTROL_ON, state);

	}
	else if(state->battery_temp>=limits->limit_battery_temperature_high){
		/* too hot - close subsystems to reduce heat dissipation */
		state->EPS_safety_temperature_mode = EPS_SAFETY_MODE_TEMPERATURE_HIGH;
		EPS_set_control_switch(BATTERY_HEATERS, EPS_SWITCH_CONTROL_OFF, state);
	}
	else if( (state->battery_temp>limits->limit_battery_temperature_low)&&(state->battery_temp<limits->limit_battery_temperature_high) ){
		/* temperature within range */
		state->EPS_safety_temperature_mode = EPS_SAFETY_MODE_TEMPERATURE_NORMAL;
		EPS_set_control_switch(BATTERY_HEATERS, EPS_SWITCH_CONTROL_OFF, state);
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
