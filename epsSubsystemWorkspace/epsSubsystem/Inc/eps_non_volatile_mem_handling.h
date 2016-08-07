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

/** @addtogroup eps_non_volatile_mem_handling
  * @{
  */


/*ATTENTION:
 * on startup the device MUST be properly initialized.
 * this means that the final firmware must have the
 * correct values set to all persistent variables
 * and perform a memory corruption test. i.e if the readed
 * value is considered corrupt, use a default value instead.
 * */

#define DATA_EEPROM_BASE_ADDRESS 0x08080000/**< Data EEPROM addresses:  0x0808 0000 - 0x0808 1FFF  size: 8 Kbytes - cat3 device memory map from stm32l datasheet*/

//**< This word is written in the deployment flag address and read back to check if deployment stage has occured.this memory address mut be cleared the firt time the eps progam launches*/
#define SATELLITE_ARM_KEY_A 0x1312ACAB
#define SATELLITE_ARM_KEY_B 0x27BCD4FC
#define SATELLITE_ARM_KEY_C 0xFB54E21D
#define SATELLITE_ARM_KEY_D 0x312A8D65
#define SATELLITE_ARM_KEY_E 0x64F83D8A
#define SATELLITE_ARM_KEY_F 0xE1AB3FE9
#define SATELLITE_ARM_KEY_G 0x6E4A7320


//this word is written in the deployment flag address if no deployment has occurred.
#define SATELLITE_DISARM_KEY_A 0xE8AB9C66
#define SATELLITE_DISARM_KEY_B 0x2E3DF504
#define SATELLITE_DISARM_KEY_C 0x9CD19D03
#define SATELLITE_DISARM_KEY_D 0x80A932ED
#define SATELLITE_DISARM_KEY_E 0x660C1871
#define SATELLITE_DISARM_KEY_F 0xE281C7B0
#define SATELLITE_DISARM_KEY_G 0x1FFE271A

#define DEPLOYMENT_FLAG_ADDRESS_OFFSET 64/**< FLASH MEMORY  ADDRESS OFFSET */

#define DEPLOYMENT_FLAG_ADDRESS_A (DATA_EEPROM_BASE_ADDRESS+4)
#define DEPLOYMENT_FLAG_ADDRESS_B (DEPLOYMENT_FLAG_ADDRESS_A+DEPLOYMENT_FLAG_ADDRESS_OFFSET)
#define DEPLOYMENT_FLAG_ADDRESS_C (DEPLOYMENT_FLAG_ADDRESS_B+DEPLOYMENT_FLAG_ADDRESS_OFFSET)
#define DEPLOYMENT_FLAG_ADDRESS_D (DEPLOYMENT_FLAG_ADDRESS_C+DEPLOYMENT_FLAG_ADDRESS_OFFSET)
#define DEPLOYMENT_FLAG_ADDRESS_E (DEPLOYMENT_FLAG_ADDRESS_D+DEPLOYMENT_FLAG_ADDRESS_OFFSET)
#define DEPLOYMENT_FLAG_ADDRESS_F (DEPLOYMENT_FLAG_ADDRESS_E+DEPLOYMENT_FLAG_ADDRESS_OFFSET)
#define DEPLOYMENT_FLAG_ADDRESS_G (DEPLOYMENT_FLAG_ADDRESS_F+DEPLOYMENT_FLAG_ADDRESS_OFFSET)

#define BOOT_COUNTER_ADDRESS (DATA_EEPROM_BASE_ADDRESS+8)

#define LIMIT_BATTERY_LOW_ADDRESS (DATA_EEPROM_BASE_ADDRESS+16)/**< FLASH MEMORY  ADDRESS OF BATTERY LOW THRESHOLD */
#define LIMIT_BATTERY_HIGH_ADDRESS (DATA_EEPROM_BASE_ADDRESS+20)/**< FLASH MEMORY  ADDRESS OF BATTERY HIGH THRESHOLD */
#define LIMIT_BATTERY_CRITICAL_ADDRESS (DATA_EEPROM_BASE_ADDRESS+24)/**< FLASH MEMORY  ADDRESS OF BATTERY CRITICAL THRESHOLD */
#define LIMIT_BATTERY_TEMPERATURE_LOW_ADDRESS (DATA_EEPROM_BASE_ADDRESS+28)/**< FLASH MEMORY  ADDRESS OF TEMPERATURE LOW THRESHOLD */
#define LIMIT_BATTERY_TEMPERATURE_HIGH_ADDRESS (DATA_EEPROM_BASE_ADDRESS+32)/**< FLASH MEMORY  ADDRESS OF TEMPERATURE HIGH THRESHOLD */

#define SOFT_ERROR_STATUS_ADDRESS (DATA_EEPROM_BASE_ADDRESS+64)/**< FLASH MEMORY  ADDRESS OF SOFT ERROR STATUS */


/**
 * @brief Deployment flag status.
 *
 * This datatype is used to define the status of the umbilical connector.
 */
typedef enum {
	DEPLOYMENT_SAT_ARMED,/**< deployment has not happened but satellite is properly armed */
	DEPLOYMENT_SAT_DISARMED,/**< deployment has  happened but satellite is properly disarmed */
	DEPLOYMENT_UNDEFINED,/**< deployment sequence droped in unresolved situation */
	DEPLOYMENT_LAST_VALUE/**< deployment status non valid. */
}EPS_deployment_status;

/*please delete this please...*/
void EPS_erase_deployment_flags(void);
void EPS_set_flash_memory_initial_values(void);

//check if deployment has already occurred. if not signal deployment and change the deployment status at the end of deployment process. also sets boot counter to zero to be incremented to 1 in the first boot.
EPS_deployment_status EPS_check_deployment_status(void);

//increments boot counter. This function is called once after each reset.
void EPS_startup_increment_bootcounter(void);


//set persistent limit values: changes safety limits and reset. Intention of this function is to allow for remote manipulation of critical safety limits.
void EPS_set_persistent_safety_limits(EPS_safety_limits *limits);

//get word from a specific memory address: remote debug session.
void EPS_get_memory_word(uint32_t memory_address, uint32_t *memory_data );
//set word from a specific memory address: remote debug session.
void EPS_set_memory_word(uint32_t memory_address, uint32_t *memory_data);

#endif /* INC_EPS_NON_VOLATILE_MEM_HANDLING_H_ */
/**
  * @}
  */
