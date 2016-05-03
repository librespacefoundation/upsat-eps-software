#include "tc74_temp_sensor.h"


 uint8_t read_device_temperature(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address) {

	 /*Read temperature*/
	 /*master sends the slave's temperature register adrress to read back*/
	 uint8_t transmit_packet = TEMPERATURE_REGISTER;
	 if (HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, &transmit_packet, 1, 1000) != HAL_OK) {
		 //soft error handle
	 }

	 uint8_t receive_word = 0;
	 if (HAL_I2C_Master_Receive(h_i2c, device_i2c_address, &receive_word, 1, 1000) != HAL_OK) {
		 //soft error handle
	 }

	 return receive_word;
 }


 TC_74_STATUS device_wake_up(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address) {

	 /*wake up */
	 uint8_t transmit_packet[2] = { CONFIGURATION_REGISTER, NORMAL };
	 if (HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, transmit_packet, 2,
			 1000) != HAL_OK) {
		 //soft error handle
	 }

	 return DEVICE_OK;
 }


 TC_74_STATUS device_sleep(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address) {

	 /*put device to sleep*/
	 uint8_t transmit_packet[2] = { CONFIGURATION_REGISTER, STANDBY };
	 if (HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, transmit_packet, 2, 1000) != HAL_OK) {
		 //soft error handle
	 }

	 return DEVICE_OK;
 }

uint8_t read_device_status(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address) {
	/*Read control regiter*/
	/*master sends the slave's temperature register adrress to read back*/
	uint8_t transmit_packet[2] = { CONFIGURATION_REGISTER, NORMAL };
	if (HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, transmit_packet, 1, 1000) != HAL_OK) {
		//soft error handle
	}

	uint8_t receive_word = 0xFF;
	if (HAL_I2C_Master_Receive(h_i2c, device_i2c_address, &receive_word, 1, 1000) != HAL_OK) {
		//soft error handle
	}

	return receive_word;
}
