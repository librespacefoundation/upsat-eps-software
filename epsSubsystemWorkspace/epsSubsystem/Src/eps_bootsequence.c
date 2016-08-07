/*
 *
  ******************************************************************************
  * @file    eps_bootsequence.c
  * @author  Aris Stathakis
  * @version V1.0
  * @date    May 21, 2016
  * @brief   Source file for bootsequence module.
  *          In this module all functions utilized during boot sequence are
  *          defined and implemented.
  ******************************************************************************
 *
 */
#include "eps_bootsequence.h"
#include "stm32l1xx_hal.h"
#include "eps_non_volatile_mem_handling.h"
#include "eps_configuration.h"
#include "eps_soft_error_handling.h"

extern EPS_State eps_board_state;/*global eps subsystem state.*/
extern IWDG_HandleTypeDef hiwdg;

/** @addtogroup bootsequence_Functions
  * @{
  */

/**
  * @brief Turn off all subsystem power rails. pmos mosfets are used  so on startup the connected subsystems are powered up.
  *        This is something that all the subsystems must handle(soft delay on startup)
  * @param  state: the eps state structure containing central info of the EPS subsystem.
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_bootseq_poweroff_all_rails(volatile EPS_State *state){

	error_status = EPS_SOFT_ERROR_BOOTSEQ_POWEROFF_ALL_RAILS;

	EPS_set_rail_switch(COMM, EPS_SWITCH_RAIL_OFF, state);
	EPS_set_rail_switch(OBC, EPS_SWITCH_RAIL_OFF, state);
	EPS_set_rail_switch(ADCS, EPS_SWITCH_RAIL_OFF, state);
	EPS_set_rail_switch(SU, EPS_SWITCH_RAIL_OFF, state);
	EPS_set_rail_switch(TEMP_SENSOR, EPS_SWITCH_RAIL_OFF, state);

	return EPS_SOFT_ERROR_BOOTSEQ_POWEROFF_ALL_RAILS_COMPLETE;
}

/**
  * @brief Turn on all subsystems, except SU subsystem, with 50ms delay to avoid current spikes.
  * @param  state: the eps state structure containing central info of the EPS subsystem.
  * @retval Error status for handling and debugging.
  *
  * TODO: when recovering from startup should I turn on coms or turn only obc and wait for oobc to tell me to turn adcs and comms on?
  */
EPS_soft_error_status EPS_bootseq_poweron_all_rails(volatile EPS_State *state){

	error_status = EPS_SOFT_ERROR_BOOTSEQ_POWERON_ALL_RAILS;

	/*Power up all voltage rails except SU */
	EPS_set_rail_switch(TEMP_SENSOR, EPS_SWITCH_RAIL_ON, state);
	HAL_Delay(50);
	EPS_set_rail_switch(COMM, EPS_SWITCH_RAIL_ON, state);
	HAL_Delay(50);
	EPS_set_rail_switch(OBC, EPS_SWITCH_RAIL_ON, state);
	HAL_Delay(50);
	EPS_set_rail_switch(ADCS, EPS_SWITCH_RAIL_ON, state);
	HAL_sys_delay(50);
 	EPS_set_rail_switch(SU, EPS_SWITCH_RAIL_OFF, state);

	return EPS_SOFT_ERROR_BOOTSEQ_POWERON_ALL_RAILS_COMPLETE;
}


/**
  * @brief Check 10 times  the umbilical pin and enter the appropriate mode
  * @param  state: the eps state structure containing central info of the EPS subsystem.
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_bootseq_umbilical_check(volatile EPS_State *state) {

	error_status = EPS_SOFT_ERROR_BOOTSEQ_UMBILICAL_CHECK;

	state->umbilical_switch = 0xff; //true

	for (uint8_t var = 0; var < 10; ++var) {

		GPIO_PinState umbilical_pin = HAL_GPIO_ReadPin(GPIO_UMBILICAL_GPIO_Port,
		        GPIO_UMBILICAL_Pin);
		HAL_Delay(10);
		if (umbilical_pin == GPIO_PIN_SET) {
			state->umbilical_switch = 0x00; //false
			break;
		}
	}

	if (state->umbilical_switch == 0x00) { //false
		/* umbilical is not connected = high */

		/* nominal mode. */
		EPS_umbilical_mode = UMBILICAL_NOT_CONNECTED;

		/* deployment stage*/
		error_status = EPS_bootseq_enter_deployment_stage(&eps_board_state);

	} else if (state->umbilical_switch == 0xff) { //true
		/* umbilical is connected = low */

		/* debug mode. */
		EPS_umbilical_mode = UMBILICAL_CONNECTED;
	} else {
		//error handling for undefined umbilical behavior.
		error_status = EPS_SOFT_ERROR_UMBILICAL_UNPREDICTED;
		EPS_umbilical_mode = UMBILICAL_CONNECTOR_UNDEFINED_STATE;
		/*if we are in an undefined situation favor deployment since a failed deployment equals a dead satellite*/
		EPS_bootseq_enter_deployment_stage(&eps_board_state);
	}

	return EPS_SOFT_ERROR_BOOTSEQ_UMBILICAL_CHECK_COMPLETE;
}


/**
  * @brief Check deployment status and if needed enter the appropriate state. A set of 7 keys are read from flash memory to verify
  *        if deployment has occurred. If any of them is already set, deployment has already happened and is skipped. If This is the
  *        first time or it has not complete deployment stage already once, The system waits for 30 minutes, deploys all deployable devices
  *        and writes the appropriate keys to flash memory.
  * @param  state: the eps state structure containing central info of the EPS subsystem.
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_bootseq_enter_deployment_stage(volatile EPS_State *state){

	error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_STAGE;

	EPS_deployment_status curret_deployment_status = EPS_check_deployment_status();
	//perfrom deployment check and if needed enter deployment mode.
	if(curret_deployment_status == DEPLOYMENT_SAT_ARMED){


		/* power of all rails */
		error_status = EPS_bootseq_poweroff_all_rails(state);


		/*Wait for 30 minutes...(make it less for debug)  30*  (60*1000) msec  = 30* 6 * (10*1000)msec */
		for (int var = 0; var < 180; ++var) {

			HAL_IWDG_Refresh(&hiwdg);
			HAL_Delay(10000);/*wait for 10sec*/
		}

		error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_ANT1;
		EPS_set_control_switch(DEPLOY_ANT1, EPS_SWITCH_CONTROL_ON, state);
		HAL_Delay(DEPLOY_BURNOUT_DELAY);
		EPS_set_control_switch(DEPLOY_ANT1, EPS_SWITCH_CONTROL_OFF, state);
		HAL_IWDG_Refresh(&hiwdg);

		error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_LEFT;
		EPS_set_control_switch(DEPLOY_LEFT, EPS_SWITCH_CONTROL_ON, state);
		HAL_Delay(DEPLOY_BURNOUT_DELAY);
		EPS_set_control_switch(DEPLOY_LEFT, EPS_SWITCH_CONTROL_OFF, state);
		HAL_IWDG_Refresh(&hiwdg);


		error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_RIGHT;
		EPS_set_control_switch(DEPLOY_RIGHT, EPS_SWITCH_CONTROL_ON, state);
		HAL_Delay(DEPLOY_BURNOUT_DELAY);
		EPS_set_control_switch(DEPLOY_RIGHT, EPS_SWITCH_CONTROL_OFF, state);
		HAL_IWDG_Refresh(&hiwdg);

		error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_BOTTOM;
		EPS_set_control_switch(DEPLOY_BOTTOM, EPS_SWITCH_CONTROL_ON, state);
		HAL_Delay(DEPLOY_BURNOUT_DELAY);
		EPS_set_control_switch(DEPLOY_BOTTOM, EPS_SWITCH_CONTROL_OFF, state);
		HAL_IWDG_Refresh(&hiwdg);

		error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_TOP;
		EPS_set_control_switch(DEPLOY_TOP, EPS_SWITCH_CONTROL_ON, state);
		HAL_Delay(DEPLOY_BURNOUT_DELAY);
		EPS_set_control_switch(DEPLOY_TOP, EPS_SWITCH_CONTROL_OFF, state);
		HAL_IWDG_Refresh(&hiwdg);

		/* set deployment flag that deployment is completed */
		uint32_t memory_write_value;
		error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_A;
		memory_write_value = SATELLITE_DISARM_KEY_A;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_A, &memory_write_value );
		HAL_Delay(1);

		error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_B;
		memory_write_value = SATELLITE_DISARM_KEY_B;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_B, &memory_write_value );
		HAL_Delay(1);

		error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_C;
		memory_write_value = SATELLITE_DISARM_KEY_C;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_C, &memory_write_value );
		HAL_Delay(1);

		error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_D;
		memory_write_value = SATELLITE_DISARM_KEY_D;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_D, &memory_write_value );
		HAL_Delay(1);

		error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_E;
		memory_write_value = SATELLITE_DISARM_KEY_E;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_E, &memory_write_value );
		HAL_Delay(1);

		error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_F;
		memory_write_value = SATELLITE_DISARM_KEY_F;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_F, &memory_write_value );
		HAL_Delay(1);
		error_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_G;
		memory_write_value = SATELLITE_DISARM_KEY_G;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_G, &memory_write_value );
		HAL_Delay(1);
	}
	else if(curret_deployment_status == DEPLOYMENT_SAT_DISARMED){
		/* Deployment has successfully occurred.*/
	}
	else{
		/*TON PINOUME*/
	}

	return EPS_SOFT_ERROR_DEPLOYMENT_COMPLETE;

}

/**
  * @}
  */
