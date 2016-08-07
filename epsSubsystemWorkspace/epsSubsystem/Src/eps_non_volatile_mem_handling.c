/*
 * eps_non_volatile_mem_handling.c
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */

#include "eps_non_volatile_mem_handling.h"
#include "eps_safety.h"

/** @addtogroup eps_non_volatile_mem_handling
  * @{
  */

/**
  * @brief supposed to erase memory adresses having been reserved to arm the satelite
  *        //TODO: delete this or yo're in deep shift.
   * @retval none.
  */
void EPS_erase_deployment_flags(void){

	/* set to disarm all deploy flags from flash */
	uint32_t memory_write_value = SATELLITE_DISARM_KEY_A;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_A, &memory_write_value );
	memory_write_value = SATELLITE_DISARM_KEY_B;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_B, &memory_write_value );
	memory_write_value = SATELLITE_DISARM_KEY_C;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_C, &memory_write_value );
	memory_write_value = SATELLITE_DISARM_KEY_D;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_D, &memory_write_value );
	memory_write_value = SATELLITE_DISARM_KEY_E;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_E, &memory_write_value );
	memory_write_value = SATELLITE_DISARM_KEY_F;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_F, &memory_write_value );
	memory_write_value = SATELLITE_DISARM_KEY_G;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_G, &memory_write_value );

 }


/**
  * @brief Initialize flash memory: deployment keys and limit values
  *
  *        This is a debug only utility by no means this is to be done in proper runtime.
  *
  * @retval none.
  */
void EPS_set_flash_memory_initial_values(void){

	/*set deployment keys to not deployed: ARMS THE SATELITE FOR DEPLOYMENT*/
	uint32_t memory_write_value = SATELLITE_ARM_KEY_A;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_A, &memory_write_value );

	memory_write_value = SATELLITE_ARM_KEY_B;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_B, &memory_write_value );

	memory_write_value = SATELLITE_ARM_KEY_C;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_C, &memory_write_value );

	memory_write_value = SATELLITE_ARM_KEY_D;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_D, &memory_write_value );

	memory_write_value = SATELLITE_ARM_KEY_E;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_E, &memory_write_value );

	memory_write_value = SATELLITE_ARM_KEY_F;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_F, &memory_write_value );

	memory_write_value = SATELLITE_ARM_KEY_G;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_G, &memory_write_value );

	/*set safety limits*/
	memory_write_value = LIMIT_BATTERY_VOLTAGE_LOW_DEFAULT;
	EPS_set_memory_word( LIMIT_BATTERY_LOW_ADDRESS, &memory_write_value );

	memory_write_value = LIMIT_BATTERY_VOLTAGE_HIGH_DEFAULT;
	EPS_set_memory_word( LIMIT_BATTERY_HIGH_ADDRESS, &memory_write_value );

	memory_write_value = LIMIT_BATTERY_VOLTAGE_CRITICAL_DEFAULT;
	EPS_set_memory_word( LIMIT_BATTERY_CRITICAL_ADDRESS, &memory_write_value );

	memory_write_value = LIMIT_BATTERY_TEMPERATURE_LOW_DEFAULT;
	EPS_set_memory_word( LIMIT_BATTERY_TEMPERATURE_LOW_ADDRESS, &memory_write_value );

	memory_write_value = LIMIT_BATTERY_TEMPERATURE_HIGH_DEFAULT;
	EPS_set_memory_word( LIMIT_BATTERY_TEMPERATURE_HIGH_ADDRESS, &memory_write_value );

	memory_write_value = (uint32_t)EPS_SOFT_ERROR_OK;
	EPS_set_memory_word( SOFT_ERROR_STATUS_ADDRESS, &memory_write_value );

}


/**
  * @brief Check if deployment has already happened.
  *
  *       If any of the arm keys is in place then the satellite is armed
  *       If any of the disarm keys is in place the satellite is disarmed.
  *
  * @retval EPS_deployment_status.
  */
EPS_deployment_status EPS_check_deployment_status(void) {

	EPS_deployment_status return_status = DEPLOYMENT_UNDEFINED;

	/* read all deploy flags from flash */
	uint32_t memory_read_valueA;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_A, &memory_read_valueA);
	uint32_t memory_read_valueB;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_B, &memory_read_valueB);
	uint32_t memory_read_valueC;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_C, &memory_read_valueC);
	uint32_t memory_read_valueD;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_D, &memory_read_valueD);
	uint32_t memory_read_valueE;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_E, &memory_read_valueE);
	uint32_t memory_read_valueF;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_F, &memory_read_valueF);
	uint32_t memory_read_valueG;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_G, &memory_read_valueG);

	if ((memory_read_valueA == SATELLITE_ARM_KEY_A)
	        || (memory_read_valueB == SATELLITE_ARM_KEY_B)
	        || (memory_read_valueC == SATELLITE_ARM_KEY_C)
	        || (memory_read_valueD == SATELLITE_ARM_KEY_D)
	        || (memory_read_valueE == SATELLITE_ARM_KEY_E)
	        || (memory_read_valueF == SATELLITE_ARM_KEY_F)
	        || (memory_read_valueG == SATELLITE_ARM_KEY_G)) {

		//DEPLOYMENT  HAS NOT HAPENNED
		return_status = DEPLOYMENT_SAT_ARMED;

	} else if ((memory_read_valueA == SATELLITE_DISARM_KEY_A)
	        || (memory_read_valueB == SATELLITE_DISARM_KEY_B)
	        || (memory_read_valueC == SATELLITE_DISARM_KEY_C)
	        || (memory_read_valueD == SATELLITE_DISARM_KEY_D)
	        || (memory_read_valueE == SATELLITE_DISARM_KEY_E)
	        || (memory_read_valueF == SATELLITE_DISARM_KEY_F)
	        || (memory_read_valueG == SATELLITE_DISARM_KEY_G)) {

		//DEPLOYMENT KEY HAS HAPENNED
		return_status = DEPLOYMENT_SAT_DISARMED;
	} else {

		//THIS SHIT IS MORE SERIOUS THAN WE CAN POSSIBLY HANDLE
		return_status = DEPLOYMENT_SAT_ARMED;/*give priority to deployment*/
	}

	return return_status;
}

/**
  * @brief Increment boot counter.
  *
  *       Sadly not in use.
  *
  * @retval None.
  */
void EPS_startup_increment_bootcounter(void){

	uint32_t memory_read_value;
	EPS_get_memory_word( BOOT_COUNTER_ADDRESS, &memory_read_value );
	memory_read_value = memory_read_value+1;
	EPS_set_memory_word(BOOT_COUNTER_ADDRESS, &memory_read_value);

}

/**
  * @brief Get word from a specific memory address.
  *
  * @param memory_address: memory address - good idea to pass only defined values and avoid mishapsmemory_address
  * @param memory_data:  32bit word just read from the specified memory address.
  *
  * @retval None.
  */
void EPS_get_memory_word(uint32_t memory_address, uint32_t *memory_data ){

	  uint32_t flash_read_value;
	  flash_read_value = *( (uint32_t *)memory_address);
	  *memory_data  = flash_read_value;

}

/**
  * @brief Set word from a specific memory address.
  *
  * @param memory_address: memory address - good idea to pass only defined values and avoid mishapsmemory_address
  * @param memory_data: payload, 32bit word to write at the specified memory address.
  *
  * @retval None.
  */
void EPS_set_memory_word(uint32_t memory_address, uint32_t *memory_data){


	uint32_t flash_write_value = *memory_data;

	/* write to memory the proper status true value */
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(TYPEPROGRAM_WORD, memory_address, flash_write_value);
	HAL_FLASH_Lock();

}

/**
  * @brief Updates flash memory soft error value - THis is supposed to occur only
  *        when the subsystem is reset via the watchdog timer.
  *
  * @retval none.
  */
void EPS_update_flash_memory_soft_error_value(void){
	uint32_t memory_write_value = (uint32_t)error_status;
	EPS_set_memory_word( SOFT_ERROR_STATUS_ADDRESS, &memory_write_value );
}

/**
  * @}
  */
