/*
 * eps_configuration.c
 *
 *  Created on: May 2, 2016
 *      Author: Aris Stathakis
 *
 */

#include "eps_configuration.h"

extern TIM_HandleTypeDef htim6;

/* TIM6 init function */
EPS_soft_error_status kick_TIM6_timed_interrupt(uint32_t period_in_uicroseconds) {

	/*configure tim6 peripheral to trigger interrupt every microseconds*/

	EPS_soft_error_status kick_timer_status = EPS_SOFT_ERROR_KICK_TIMER;

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

	kick_timer_status = EPS_SOFT_ERROR_KICK_TIMER;
	return kick_timer_status;
}

