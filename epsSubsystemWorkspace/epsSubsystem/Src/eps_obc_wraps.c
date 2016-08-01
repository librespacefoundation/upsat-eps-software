/*
 * eps_obc_wraps.c
 *
 *  Created on: May 21, 2016
 *      Author: ariolis
 */

#include "eps_obc_wraps.h"
#include "stm32l1xx_hal.h"


extern UART_HandleTypeDef huart3;/**/
extern uint8_t uart_temp[200];



/** @addtogroup obc_wrapper_Functions
  * @{
  */

/**
  * @brief Initialize communication with obc. Must be called once before the while loop.
  * @param  void.
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_obc_communication_init(void){

	EPS_soft_error_status bootsequence_status = EPS_SOFT_ERROR_OBC_COMM_INIT;

	pkt_pool_INIT();

	uint16_t size = 0;

	event_boot(0, 0);

	/*Uart inits*/
	HAL_UART_Receive_IT(&huart3, eps_data.obc_uart.uart_buf, UART_BUF_SIZE);

	bootsequence_status = EPS_SOFT_ERROR_OK;
	return bootsequence_status;

}

/**
  * @brief service obc communication. Must be called every timed interrupt to repsond to obc messages.
  * @param  void.
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_obc_communication_service(void){

	EPS_soft_error_status obc_com_status = EPS_SOFT_ERROR_OBC_COMM_SERVICE;

	import_pkt(OBC_APP_ID, &eps_data.obc_uart);

	check_timeouts();

	export_pkt(OBC_APP_ID, &eps_data.obc_uart);

	obc_com_status = EPS_SOFT_ERROR_OK;
	return obc_com_status;
}
/**
  * @}
  */
