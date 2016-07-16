/*
 * eps_non_volatile_mem_handling.c
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */

#include "eps_non_volatile_mem_handling.h"
#include "eps_safety.h"


//TODO: delete this or yo're in deep shift.
void EPS_erase_deployment_flags(void){

	/* zero out all deploy flags from flash */
	uint32_t memory_write_value = DEPLOYMENT_NOT_KEY_A;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_A, &memory_write_value );

 }



void EPS_set_flash_memory_initial_values(void){

	/*set deployment keys to not deployed: ARMS THE SATELITE FOR DEPLOYMENT*/
	uint32_t memory_write_value = DEPLOYMENT_NOT_KEY_A;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_A, &memory_write_value );

	memory_write_value = DEPLOYMENT_NOT_KEY_B;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_B, &memory_write_value );

	memory_write_value = DEPLOYMENT_NOT_KEY_C;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_A, &memory_write_value );

	memory_write_value = DEPLOYMENT_NOT_KEY_C;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_A, &memory_write_value );

	memory_write_value = DEPLOYMENT_NOT_KEY_D;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_D, &memory_write_value );

	memory_write_value = DEPLOYMENT_NOT_KEY_E;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_E, &memory_write_value );

	memory_write_value = DEPLOYMENT_NOT_KEY_F;
	EPS_set_memory_word( DEPLOYMENT_FLAG_ADDRESS_F, &memory_write_value );

	memory_write_value = DEPLOYMENT_NOT_KEY_G;
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


}





//check if deployment has already happened.
EPS_deployment_status EPS_check_deployment_status(void){

	EPS_deployment_status return_status = DEPLOYMENT_UNDEFINED;

	/* read all deploy flags from flash */
	uint32_t memory_read_valueA;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_A, &memory_read_valueA );
	uint32_t memory_read_valueB;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_B, &memory_read_valueB );
	uint32_t memory_read_valueC;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_C, &memory_read_valueC );
	uint32_t memory_read_valueD;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_D, &memory_read_valueD );
	uint32_t memory_read_valueE;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_E, &memory_read_valueE );
	uint32_t memory_read_valueF;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_F, &memory_read_valueF );
	uint32_t memory_read_valueG;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS_G, &memory_read_valueG );



	if((memory_read_valueA==DEPLOYMENT_KEY_A)||(memory_read_valueB==DEPLOYMENT_KEY_B)||(memory_read_valueC==DEPLOYMENT_KEY_C)||(memory_read_valueD==DEPLOYMENT_KEY_D)||(memory_read_valueE==DEPLOYMENT_KEY_E)||(memory_read_valueF==DEPLOYMENT_KEY_F)||(memory_read_valueG==DEPLOYMENT_KEY_G)){

		//DEPLOYMENT KEY HAS HAPENNED
		return_status = DEPLOYMENT_OK;

	}
	else if((memory_read_valueA==DEPLOYMENT_NOT_KEY_A)||(memory_read_valueB==DEPLOYMENT_NOT_KEY_B)||(memory_read_valueC==DEPLOYMENT_NOT_KEY_C)||(memory_read_valueD==DEPLOYMENT_NOT_KEY_D)||(memory_read_valueE==DEPLOYMENT_NOT_KEY_E)||(memory_read_valueF==DEPLOYMENT_NOT_KEY_F)||(memory_read_valueG==DEPLOYMENT_NOT_KEY_G)){

		//DEPLOYMENT KEY HAS NOT HAPENNED
		return_status = DEPLOYMENT_NOT;
	}
	else{

		//THIS SHIT IS MORE SERIOUS THAN WE CAN POSSIBLY HANDLE
	}



	return return_status;
}


//increment boot counter
void EPS_startup_increment_bootcounter(void){

	uint32_t memory_read_value;
	EPS_get_memory_word( BOOT_COUNTER_ADDRESS, &memory_read_value );
	memory_read_value = memory_read_value+1;
	EPS_set_memory_word(BOOT_COUNTER_ADDRESS, &memory_read_value);

}



//get word from a specific memory address: remote debug session.
void EPS_get_memory_word(uint32_t memory_address, uint32_t *memory_data ){

	  uint32_t flash_read_value;
	  flash_read_value = *( (uint32_t *)memory_address);
	  *memory_data  = flash_read_value;

}

//set word from a specific memory address: remote debug session.
void EPS_set_memory_word(uint32_t memory_address, uint32_t *memory_data){


	uint32_t flash_write_value = *memory_data;

	/* write to memory the proper status true value */
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(TYPEPROGRAM_WORD, memory_address, flash_write_value);
	HAL_FLASH_Lock();

}
