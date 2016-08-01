/*
 * eps_debug.c
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */
#include "eps_debug.h"
#include "stm32l1xx_hal.h"

extern UART_HandleTypeDef huart1;

/** @addtogroup eps_debug
  * @{
  */

/**
  * @brief write data to debug uart.
  *        TODO: implement this! for uart debug record.
  * @param  void.
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_flush_debug_data_to_uart(void){

	EPS_soft_error_status bootsequence_status = EPS_SOFT_ERROR_DEBUG_UART_FLUSH;


	bootsequence_status = EPS_SOFT_ERROR_OK;
	return bootsequence_status;
}

/**
  * @brief Initialize uart debug.
  *        TODO: implement this! for uart debug record.
  * @param  void.
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_debug_uart_init(void){

	EPS_soft_error_status bootsequence_status = EPS_SOFT_ERROR_DEBUG_UART_INIT;

	/*debug UART starting string test*/
	uint8_t uart_output_buffer[15];
	sprintf(uart_output_buffer, "UART test ...\n");
	if(HAL_UART_Transmit_DMA(&huart1, (uint8_t*)uart_output_buffer, 15)!= HAL_OK)
	{
		/* Transfer error in transmission process */
		//Error_Handler();
		HAL_Delay(1);
	}

	bootsequence_status = EPS_SOFT_ERROR_OK;
	return bootsequence_status;
}
/**
  * @}
  */
