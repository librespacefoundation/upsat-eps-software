#include "tc74_temp_sensor.h"


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


//#include "tc74_temp_sensor.h"
//
//
// TC_74_STATUS read_device_temperature(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address, int8_t *receive_word ) {
//
//	 TC_74_STATUS device_status;
//	 /*Read temperature*/
//	 /*master sends the slave's temperature register adrress to read back*/
//	 uint8_t transmit_packet = TEMPERATURE_REGISTER;
//	 if (HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, (uint8_t*)&transmit_packet, 1, 100) != HAL_OK) {
//		 //soft error handle
//		 device_status = DEVICE_ERROR;
//	 }
//
// 	 if (HAL_I2C_Master_Receive(h_i2c, device_i2c_address, receive_word, 1, 100) != HAL_OK) {
//		 //soft error handle
// 		device_status = DEVICE_ERROR;
//	 }
//
// 	 if(device_status!=DEVICE_ERROR){
// 		 device_status = read_device_status(h_i2c, device_i2c_address);
// 	 }
//
//	 return device_status;
// }
//
//
// TC_74_STATUS device_wake_up(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address) {
//
//	 TC_74_STATUS device_status = DEVICE_NORMAL;
//	 HAL_StatusTypeDef res;
//	 /*wake up */
//	 uint8_t transmit_packet[2] = { CONFIGURATION_REGISTER, NORMAL };
//	 if ((res = HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, transmit_packet, 2,	100)) != HAL_OK) {
//		 //soft error handle
//		 device_status = DEVICE_ERROR;
//	 }
//
// 	 if(device_status!=DEVICE_ERROR){
// 		 device_status = read_device_status(h_i2c, device_i2c_address);
// 	 }
//
//	 return device_status;
// }
//
//
// TC_74_STATUS device_sleep(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address) {
//
//	 TC_74_STATUS device_status;
//	 /*put device to sleep*/
//	 uint8_t transmit_packet[2] = { CONFIGURATION_REGISTER, STANDBY };
//	 if (HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, transmit_packet, 2, 100) != HAL_OK) {
//		 //soft error handle
//		 device_status = DEVICE_ERROR;
//	 }
//
// 	 if(device_status!=DEVICE_ERROR){
// 		 device_status = read_device_status(h_i2c, device_i2c_address);
// 	 }
//
//	 return device_status;
// }
//
// TC_74_STATUS read_device_status(I2C_HandleTypeDef *h_i2c, uint8_t device_i2c_address ) {
//
//	 TC_74_STATUS device_status = DEVICE_NORMAL;
//	 HAL_StatusTypeDef res;
//	/*Read control regiter*/
//	/*master sends the slave's temperature register adrress to read back*/
//	uint8_t transmit_packet[2] = { CONFIGURATION_REGISTER, NORMAL };
//	if ((res=HAL_I2C_Master_Transmit(h_i2c, device_i2c_address, transmit_packet, 1, 100) )!= HAL_OK) {
//		//soft error handle
//		device_status = DEVICE_ERROR;
//	}
//
//	uint8_t receive_word;
// 	if ((res=HAL_I2C_Master_Receive(h_i2c, device_i2c_address, &receive_word, 1, 100)) != HAL_OK) {
//		//soft error handle
// 		device_status = DEVICE_ERROR;
//	}
//
// 	if(device_status!=DEVICE_ERROR){
// 		if ((receive_word&0b10111111) == STANDBY){
// 			device_status = DEVICE_STANDBY;
// 		}
// 		else if((receive_word&0b10111111) == NORMAL){
// 			device_status = DEVICE_NORMAL;
// 			//TODO: implement device normal data not ready state!
// 		}
// 		else{
// 			device_status = DEVICE_ERROR;
// 		}
// 	}
//
//
//	return device_status;
//}
