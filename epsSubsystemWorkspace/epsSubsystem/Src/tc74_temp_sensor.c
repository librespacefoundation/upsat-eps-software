#include "tc74_temp_sensor.h"

/** @addtogroup tc74_device_driver
  * @{
  */

/**
  * @brief Reads the word in temperature measurement register of the device and returns the current status of the specified address.
  * @param  h_i2c: pointer to the i2c bus peripheral handle that the temperature sensor ics are connected.
  * @param  device_i2c_address: i2c adress of the sensor to request measurement.
  * @param  receive_word: pointer to the word where the measurement register will be returned.
  * @retval TC_74_STATUS.
  */
 TC_74_STATUS TC74_read_device_temperature(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address, int8_t *receive_word ) {

	 TC_74_STATUS device_status;
	 /*Read temperature*/
	 /*master sends the slave's temperature register adrress to read back*/
	 uint8_t transmit_packet = TC74_TEMPERATURE_REGISTER;
	 if (HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, (uint8_t*)&transmit_packet, 1, 100) != HAL_OK) {
		 //soft error handle
		 device_status = DEVICE_ERROR;
	 }

 	 if (HAL_I2C_Master_Receive(h_i2c, device_i2c_address, receive_word, 1, 100) != HAL_OK) {
		 //soft error handle
 		device_status = DEVICE_ERROR;
	 }

 	 if(device_status!=DEVICE_ERROR){
 		 device_status = TC74_read_device_status(h_i2c, device_i2c_address);
 	 }

	 return device_status;
 }

 /**
   * @brief Wake up request to the sensor on the specified address.
   * @param  h_i2c: pointer to the i2c bus peripheral handle that the temperature sensor ics are connected.
   * @param  device_i2c_address: i2c address of the sensor to request measurement.
   * @retval TC_74_STATUS.
   */
 TC_74_STATUS TC74_device_wake_up(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address) {

	 TC_74_STATUS device_status = DEVICE_STATUS_LAST_VALUE;
	 HAL_StatusTypeDef res;
	 /*wake up */
	 uint8_t transmit_packet[2] = { TC74_CONFIGURATION_REGISTER, TC74_AWAKE_COMMAND };
	 if ((res = HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, transmit_packet, 2,	100)) != HAL_OK) {
		 //soft error handle
		 device_status = DEVICE_ERROR;
	 }

 	 if(device_status!=DEVICE_ERROR){
 		 device_status = TC74_read_device_status(h_i2c, device_i2c_address);
 	 }

	 return device_status;
 }

 /**
   * @brief Sleep device request to the sensor on the specified address.
   * @param  h_i2c: pointer to the i2c bus peripheral handle that the temperature sensor ics are connected.
   * @param  device_i2c_address: i2c address of the sensor to request measurement.
   * @retval TC_74_STATUS.
   */
 TC_74_STATUS TC74_device_sleep(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address) {

	 TC_74_STATUS device_status;
	 /*put device to sleep*/
	 uint8_t transmit_packet[2] = { TC74_CONFIGURATION_REGISTER, TC74_STANDBY_COMMAND };
	 if (HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, transmit_packet, 2, 100) != HAL_OK) {
		 //soft error handle
		 device_status = DEVICE_ERROR;
	 }

 	 if(device_status!=DEVICE_ERROR){
 		 device_status = TC74_read_device_status(h_i2c, device_i2c_address);
 	 }

	 return device_status;
 }

 /**
   * @brief Reads the word in device status register of the device and returns the current status of the specified address.
   * @param  h_i2c: pointer to the i2c bus peripheral handle that the temperature sensor ics are connected.
   * @param  device_i2c_address: i2c adress of the sensor to request measurement.
   * @param  receive_word: pointer to the word where the measurement register will be returned.
   * @retval TC_74_STATUS.
   */
 TC_74_STATUS TC74_read_device_status(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address ) {

	 TC_74_STATUS device_status = DEVICE_STATUS_LAST_VALUE;
	 HAL_StatusTypeDef res;
	/*Read control regiter*/
	/*master sends the slave's temperature register adrress to read back*/
	uint8_t transmit_packet[2] = { TC74_CONFIGURATION_REGISTER, TC74_AWAKE_COMMAND };
	if ((res=HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, transmit_packet, 1, 100) )!= HAL_OK) {
		//soft error handle
		device_status = DEVICE_ERROR;
	}

	uint8_t receive_word;
 	if ((res=HAL_I2C_Master_Receive(h_i2c, device_i2c_address, &receive_word, 1, 100)) != HAL_OK) {
		//soft error handle
 		device_status = DEVICE_ERROR;
	}

 	if(device_status!=DEVICE_ERROR){
 		if ((receive_word&0b10111111) == TC74_STANDBY_COMMAND){
 			device_status = DEVICE_STANDBY;
 		}
 		else if((receive_word&0b10111111) == TC74_AWAKE_COMMAND){
 			device_status = DEVICE_NORMAL_DATA_NOT_READY;
 			/*check data ready bit*/
 			if ((receive_word&0b11011111) == TC74_DATA_READY_FLAG){
 				device_status = DEVICE_NORMAL_DATA_READY;
 			}

 		}
 		else{
 			device_status = DEVICE_STATUS_LAST_VALUE;
 		}
 	}


	return device_status;
}
 /**
   * @}
   */
