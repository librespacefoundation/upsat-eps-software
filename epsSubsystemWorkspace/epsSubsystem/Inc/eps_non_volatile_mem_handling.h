/*
 * eps_non_volatile_mem_handling.h
 *
 *  Created on: May 9, 2016
 *      Author: ariolis
 */

#ifndef INC_EPS_NON_VOLATILE_MEM_HANDLING_H_
#define INC_EPS_NON_VOLATILE_MEM_HANDLING_H_

#include "stm32l1xx_hal.h"
#include "eps_safety.h"

/*ATTENTION:
 * on startup the device MUST be properly initialized.
 * this means that the final firmware must have the
 * correct values set to all persistent variables
 * and perform a memory corruption test. i.e if the readed
 * value is considered corrupt, use a default value instead.
 * */


//cat3 device memory map from stm32l datasheet
//Data EEPROM addresses:  0x0808 0000 - 0x0808 1FFF  size: 8 Kbytes
#define DATA_EEPROM_BASE_ADDRESS 0x08080000

//this word is written in the deployment flag address and read back to check if deployment stage has occured.this memory address mut be cleared the firt time the eps progam launches
#define DEPLOYMENT_KEY 0xDEADBEEF

#define DEPLOYMENT_FLAG_ADDRESS (DATA_EEPROM_BASE_ADDRESS+4)
#define BOOT_COUNTER_ADDRESS (DATA_EEPROM_BASE_ADDRESS+8)

#define LIMIT_BATTERY_LOW_ADDRESS (DATA_EEPROM_BASE_ADDRESS+16)
#define LIMIT_BATTERY_HIGH_ADDRESS (DATA_EEPROM_BASE_ADDRESS+20)
#define LIMIT_BATTERY_CRITICAL_ADDRESS (DATA_EEPROM_BASE_ADDRESS+24)
#define LIMIT_BATTERY_TEMPERATURE_LOW_ADDRESS (DATA_EEPROM_BASE_ADDRESS+28)

typedef enum {
	DEPLOYMENT_NOT =0,
	DEPLOYMENT_OK
}EPS_deployment_status;

//check if deployment has already occurred. if not signal deployment and change the deployment status at the end of deployment process. also sets boot counter to zero to be incremented to 1 in the first boot.
EPS_deployment_status EPS_check_deployment_status(void);

//increments boot counter. This function is called once after each reset.
void EPS_startup_increment_bootcounter(void);




//set persistent limit values: changes safety limits and reset. Intention of this function is to allow for remote manipulation of critical safety limits.
void EPS_set_persistent_safety_limits(EPS_safety_limits *limits);

//get word from a specific memory address: remote debug session.
void EPS_get_memory_word(uint32_t memory_address);
//set word from a specific memory address: remote debug session.
void EPS_set_memory_word(uint32_t memory_address);

#endif /* INC_EPS_NON_VOLATILE_MEM_HANDLING_H_ */
