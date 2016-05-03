#include "tc74_temp_sensor.h"


 TC_74_STATUS read_device_temperature(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address, uint8_t *receive_word ) {

	 TC_74_STATUS device_status;
	 /*Read temperature*/
	 /*master sends the slave's temperature register adrress to read back*/
	 uint8_t transmit_packet = TEMPERATURE_REGISTER;
	 if (HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, &transmit_packet, 1, 1000) != HAL_OK) {
		 //soft error handle
		 device_status = DEVICE_ERROR;
	 }

 	 if (HAL_I2C_Master_Receive(h_i2c, device_i2c_address, receive_word, 1, 1000) != HAL_OK) {
		 //soft error handle
 		device_status = DEVICE_ERROR;
	 }

 	 if(device_status!=DEVICE_ERROR){
 		 device_status = read_device_status(h_i2c, device_i2c_address);
 	 }

	 return device_status;
 }


 TC_74_STATUS device_wake_up(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address) {

	 TC_74_STATUS device_status;
	 /*wake up */
	 uint8_t transmit_packet[2] = { CONFIGURATION_REGISTER, NORMAL };
	 if (HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, transmit_packet, 2,
			 1000) != HAL_OK) {
		 //soft error handle
		 device_status = DEVICE_ERROR;
	 }

 	 if(device_status!=DEVICE_ERROR){
 		 device_status = read_device_status(h_i2c, device_i2c_address);
 	 }

	 return device_status;
 }


 TC_74_STATUS device_sleep(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address) {

	 TC_74_STATUS device_status;
	 /*put device to sleep*/
	 uint8_t transmit_packet[2] = { CONFIGURATION_REGISTER, STANDBY };
	 if (HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, transmit_packet, 2, 1000) != HAL_OK) {
		 //soft error handle
		 device_status = DEVICE_ERROR;
	 }

 	 if(device_status!=DEVICE_ERROR){
 		 device_status = read_device_status(h_i2c, device_i2c_address);
 	 }

	 return device_status;
 }

 TC_74_STATUS read_device_status(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address ) {

	 TC_74_STATUS device_status;
	/*Read control regiter*/
	/*master sends the slave's temperature register adrress to read back*/
	uint8_t transmit_packet[2] = { CONFIGURATION_REGISTER, NORMAL };
	if (HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, transmit_packet, 1, 1000) != HAL_OK) {
		//soft error handle
		device_status = DEVICE_ERROR;
	}

	uint8_t receive_word;
 	if (HAL_I2C_Master_Receive(h_i2c, device_i2c_address, &receive_word, 1, 1000) != HAL_OK) {
		//soft error handle
 		device_status = DEVICE_ERROR;
	}

 	if(device_status!=DEVICE_ERROR){
 		if (receive_word == STANDBY){
 			device_status = DEVICE_STANDBY;
 		}
 		else if(receive_word == NORMAL){
 			device_status = DEVICE_NORMAL;
 		}
 		else{
 			device_status = DEVICE_ERROR;
 		}
 	}


	return device_status;
}
