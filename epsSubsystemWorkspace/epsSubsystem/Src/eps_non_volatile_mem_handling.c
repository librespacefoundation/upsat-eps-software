/*
 * eps_non_volatile_mem_handling.c
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */

#include "eps_non_volatile_mem_handling.h"


//check if deployment ha happened already.
EPS_deployment_status EPS_check_deployment_status(void){

	EPS_deployment_status return_status;
	uint32_t memory_read_value;
	EPS_get_memory_word( DEPLOYMENT_FLAG_ADDRESS, &memory_read_value );

	if(memory_read_value==DEPLOYMENT_KEY){
		return_status = DEPLOYMENT_OK;
	}
	else{
		return_status = DEPLOYMENT_NOT;
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
