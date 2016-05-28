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

extern volatile EPS_soft_error_status error_status;/* global software error status - in the interrupt is called  the soft error handling.*/
extern volatile EPS_umbilical_status EPS_umbilical_mode;

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

	EPS_soft_error_status bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_POWEROFF_ALL_RAILS;

	EPS_set_rail_switch(COMM, EPS_SWITCH_RAIL_OFF, state);
	EPS_set_rail_switch(OBC, EPS_SWITCH_RAIL_OFF, state);
	EPS_set_rail_switch(ADCS, EPS_SWITCH_RAIL_OFF, state);
	EPS_set_rail_switch(SU, EPS_SWITCH_RAIL_OFF, state);
	EPS_set_rail_switch(TEMP_SENSOR, EPS_SWITCH_RAIL_OFF, state);

	bootsequence_status = EPS_SOFT_ERROR_OK;

	return bootsequence_status;
}

/**
  * @brief Turn on all subsystems, except SU subsystem, with 500ms delay to avoid current spikes.
  * @param  state: the eps state structure containing central info of the EPS subsystem.
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_bootseq_poweron_all_rails(volatile EPS_State *state){

	EPS_soft_error_status bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_POWERON_ALL_RAILS;

	/*Power up all voltage rails except SU */
	EPS_set_rail_switch(TEMP_SENSOR, EPS_SWITCH_RAIL_ON, state);
	HAL_sys_delay(500);
	EPS_set_rail_switch(OBC, EPS_SWITCH_RAIL_ON, state);
	HAL_sys_delay(500);
	EPS_set_rail_switch(ADCS, EPS_SWITCH_RAIL_ON, state);
	HAL_sys_delay(500);
	EPS_set_rail_switch(COMM, EPS_SWITCH_RAIL_ON, state);
	HAL_sys_delay(500);
 	EPS_set_rail_switch(SU, EPS_SWITCH_RAIL_OFF, state);

	bootsequence_status = EPS_SOFT_ERROR_OK;

	return bootsequence_status;
}


/**
  * @brief Check 10 times  the umbilical pin and enter the appropriate mode
  * @param  state: the eps state structure containing central info of the EPS subsystem.
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_bootseq_umbilical_check(volatile EPS_State *state){

	EPS_soft_error_status bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_UMBILICAL_CHECK;

	state->umbilical_switch = 0xff;//true

	for (uint8_t var = 0; var < 10; ++var) {

		GPIO_PinState umbilical_pin =HAL_GPIO_ReadPin(GPIO_UMBILICAL_GPIO_Port, GPIO_UMBILICAL_Pin);
		HAL_sys_delay(10);
		if(umbilical_pin == GPIO_PIN_SET){
			state->umbilical_switch = 0x00;//false
		}
	}


	if (state->umbilical_switch==0x00){//false
		//umbilical is not connected = high

		//nominal mode.
		EPS_umbilical_mode = UMBILICAL_NOT_CONNECTED;
	}
	else if(state->umbilical_switch==0xff){//true
		//umbilical is connected = low

		//debug mode.
		EPS_umbilical_mode = UMBILICAL_CONNECTED;
	}
	else{
		//error handling for undefined umbilical behavior.
		error_status = EPS_SOFT_ERROR_UMBILICAL_UNPREDICTED;
		EPS_umbilical_mode = UMBILICAL_CONNECTOR_UNDEFINED_STATE;
	}

	bootsequence_status = EPS_SOFT_ERROR_OK;

	return bootsequence_status;
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

	EPS_soft_error_status bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_STAGE;

	//perfrom deployment check and if needed enter deployment mode.
	if(EPS_check_deployment_status() == DEPLOYMENT_NOT){


		/* power of all rails */
		bootsequence_status = EPS_bootseq_poweroff_all_rails(state);


		/*Wait for 30 minutes...(make it less for debug) */
		HAL_Delay(60000);
		HAL_Delay(60000);
		HAL_Delay(60000);
		HAL_Delay(60000);
		HAL_Delay(60000);




		bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_LEFT;
		EPS_set_control_switch(DEPLOY_LEFT, EPS_SWITCH_CONTROL_ON, state);
		HAL_sys_delay(DEPLOY_BURNOUT_DELAY);
		EPS_set_control_switch(DEPLOY_LEFT, EPS_SWITCH_CONTROL_OFF, state);

		bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_RIGHT;
		EPS_set_control_switch(DEPLOY_RIGHT, EPS_SWITCH_CONTROL_ON, state);
		HAL_sys_delay(DEPLOY_BURNOUT_DELAY);
		EPS_set_control_switch(DEPLOY_RIGHT, EPS_SWITCH_CONTROL_OFF, state);

		bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_BOTTOM;
		EPS_set_control_switch(DEPLOY_BOTTOM, EPS_SWITCH_CONTROL_ON, state);
		HAL_sys_delay(DEPLOY_BURNOUT_DELAY);
		EPS_set_control_switch(DEPLOY_BOTTOM, EPS_SWITCH_CONTROL_OFF, state);

		bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_TOP;
		EPS_set_control_switch(DEPLOY_TOP, EPS_SWITCH_CONTROL_ON, state);
		HAL_sys_delay(DEPLOY_BURNOUT_DELAY);
		EPS_set_control_switch(DEPLOY_TOP, EPS_SWITCH_CONTROL_OFF, state);


		/* set deployment flag that deployment is completed */
		uint32_t memory_write_value;
		bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_A;
		memory_write_value = DEPLOYMENT_KEY_A;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_A, &memory_write_value );
		HAL_sys_delay(1);

		bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_B;
		memory_write_value = DEPLOYMENT_KEY_B;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_B, &memory_write_value );
		HAL_sys_delay(1);

		bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_C;
		memory_write_value = DEPLOYMENT_KEY_C;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_C, &memory_write_value );
		HAL_sys_delay(1);

		bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_D;
		memory_write_value = DEPLOYMENT_KEY_D;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_D, &memory_write_value );
		HAL_sys_delay(1);

		bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_E;
		memory_write_value = DEPLOYMENT_KEY_E;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_E, &memory_write_value );
		HAL_sys_delay(1);

		bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_F;
		memory_write_value = DEPLOYMENT_KEY_F;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_F, &memory_write_value );
		HAL_sys_delay(1);
		bootsequence_status = EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_G;
		memory_write_value = DEPLOYMENT_KEY_G;
		EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_G, &memory_write_value );
		HAL_sys_delay(1);
	}

	bootsequence_status = EPS_SOFT_ERROR_OK;
	return bootsequence_status;

}

/**
  * @}
  */

