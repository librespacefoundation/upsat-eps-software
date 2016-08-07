/*
 * eps_configuration.c
 *
 *  Created on: May 2, 2016
 *      Author: Aris Stathakis
 *
 */

#include "eps_configuration.h"

extern TIM_HandleTypeDef htim6;/* timer 6 peripheral, the timer used for the timed schedule.*/


volatile EPS_umbilical_status EPS_umbilical_mode = UMBILICAL_CONNECTED;/* initialize global umbilical flag to connected - When umbillical is connected no deployment stage occurs.*/


/** @addtogroup eps_configuration
  * @{
  */

/**
  * @brief  Initializes timer 6 and starts counting generating an interrupt every time the requested amount of microseconds has elapsed.
  * @param  period_in_uicroseconds: period of timer6 interrupt
  * @retval EPS_soft_error_status.
  */
EPS_soft_error_status kick_TIM6_timed_interrupt(uint32_t period_in_uicroseconds) {

	/*configure tim6 peripheral to trigger interrupt every microseconds*/

	error_status = EPS_SOFT_ERROR_KICK_TIMER;

	TIM_MasterConfigTypeDef sMasterConfig;

	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 32;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = period_in_uicroseconds;
	HAL_TIM_Base_Init(&htim6);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig);

	/*kick timer interupt for timed threads.*/
	HAL_TIM_Base_Start_IT(&htim6);

	return EPS_SOFT_ERROR_KICK_TIMER_COMPLETE;
}



/**
  * @brief  Changes countdown time the watchdog timer will be energized - new_reload*(1/(37Khz/new_precaler)) = time to watchdog
  * @param  new_precaler: must be a valid prescaler value such as IWDG_PRESCALER_4 - IWDG_PRESCALER_256
  * @param  new_reload: must be a valid reloas value in the range of 0-4095.
  * @retval EPS_soft_error_status.
  */
EPS_soft_error_status IWDG_change_reset_time(IWDG_HandleTypeDef *new_hiwdg, uint32_t new_precaler, uint32_t new_reload) {

	error_status = EPS_SOFT_ERROR_IWDG_CHANGE;
	/*Refresh previous seting IWDG*/
	HAL_IWDG_Refresh(new_hiwdg);

	new_hiwdg->Instance = IWDG;
	new_hiwdg->Init.Prescaler = new_precaler;
	new_hiwdg->Init.Reload = new_reload;/*1900 *(1/(37Khz/Prescaler)) = time to watchdog =205.409msec*/
	HAL_IWDG_Init(new_hiwdg);

	/*Start Counting*/
	HAL_IWDG_Start(new_hiwdg);

	return EPS_SOFT_ERROR_IWDG_CHANGE_COMPLETE;
}






/**
  * @}
  */
