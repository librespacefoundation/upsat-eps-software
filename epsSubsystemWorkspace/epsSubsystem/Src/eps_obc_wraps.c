/*
 * eps_obc_wraps.c
 *
 *  Created on: May 21, 2016
 *      Author: ariolis
 */

#include "eps_obc_wraps.h"
#include "stm32l1xx_hal.h"


extern UART_HandleTypeDef huart3;
extern uint8_t uart_temp[200];

EPS_soft_error_status EPS_obc_communication_init(void){

	EPS_soft_error_status bootsequence_status = EPS_SOFT_ERROR_OBC_COMM_INIT;


 	HAL_reset_source(&sys_data.rsrc);

	pkt_pool_INIT();
	sprintf((char*)uart_temp, "Hello\n");
	HAL_UART_Transmit(&huart3, uart_temp, 6 , 10000);
#ifdef EPS_DEBUG_MODE
	HAL_sys_delay(5000);//xreiazetai afto to delay re man?
#endif
	uint16_t size = 0;

	event_crt_pkt_api(uart_temp, "EPS STARTED", 666, 666, "", &size, SATR_OK);
	HAL_uart_tx(DBG_APP_ID, (uint8_t *)uart_temp, size);

	/*Uart inits*/
	HAL_UART_Receive_IT(&huart3, eps_data.obc_uart.uart_buf, UART_BUF_SIZE);

	bootsequence_status = EPS_SOFT_ERROR_OK;
	return bootsequence_status;

}

EPS_soft_error_status EPS_obc_communication_service(void){

	EPS_soft_error_status obc_com_status = EPS_SOFT_ERROR_OBC_COMM_SERVICE;

	import_pkt(OBC_APP_ID, &eps_data.obc_uart);

	obc_com_status = EPS_SOFT_ERROR_OK;
	return obc_com_status;
}
