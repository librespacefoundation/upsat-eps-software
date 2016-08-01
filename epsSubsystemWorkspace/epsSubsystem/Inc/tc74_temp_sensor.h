#ifndef __TC74_TEMP_SENSOR_H
#define __TC74_TEMP_SENSOR_H

#include "stm32l1xx_hal.h"

/** @addtogroup tc74_device_driver
  * @{
  */

/* tc74 register addresses */
#define TC74_CONFIGURATION_REGISTER	0x01 /**< */
#define TC74_TEMPERATURE_REGISTER	0x00 /**< */

// configuration register commands
#define TC74_STANDBY_COMMAND 	0b10000000/**< */
#define TC74_AWAKE_COMMAND 	0b00000000/**< */
#define TC74_DATA_READY_FLAG 0b01000000/**< */

/**< tc74 i2c address constants*/
#define TC74_A0	0b10010000
#define TC74_A1	0b10010010
#define TC74_A2	0b10010100
#define TC74_A3	0b10010110
#define TC74_A4	0b10011000
#define TC74_A5	0b10011010
#define TC74_A6	0b10011100
#define TC74_A7	0b10011110

/**
 * @brief TC_74_STATUS.
 *
 * status flag for the tc74 device.
 */
typedef enum TC_74_STATUS{
	DEVICE_ERROR,
	DEVICE_STANDBY,
	DEVICE_NORMAL_DATA_READY,
	DEVICE_NORMAL_DATA_NOT_READY,
    DEVICE_STATUS_LAST_VALUE
}TC_74_STATUS;

/*get temperature reading from the tc74 sensor with the specified device address - returns status*/
TC_74_STATUS TC74_read_device_temperature(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address, int8_t *receive_word );
/*wake up sensor*/
TC_74_STATUS TC74_device_wake_up(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address);
/*put sensor to sleep*/
TC_74_STATUS TC74_device_sleep(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address);
/*get sensor status*/
TC_74_STATUS TC74_read_device_status(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address);


#endif

/**
  * @}
  */
