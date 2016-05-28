/*
 *
  ******************************************************************************
  * @file    eps_bootsequence.h
  * @author   Aris Stathakis
  * @version V1.0
  * @date    May 21, 2016
  * @brief   Header file for bootsequence module.
  ******************************************************************************
 *
 */

#ifndef INC_EPS_BOOTSEQUENCE_H_
#define INC_EPS_BOOTSEQUENCE_H_

#include "stm32l1xx_hal.h"
#include "eps_state.h"
#include "eps_configuration.h"
#include "eps_soft_error_handling.h"

EPS_soft_error_status EPS_bootseq_poweroff_all_rails(volatile EPS_State *state);

EPS_soft_error_status EPS_bootseq_umbilical_check(volatile EPS_State *state);

EPS_soft_error_status EPS_bootseq_enter_deployment_stage(volatile EPS_State *state);

EPS_soft_error_status EPS_bootseq_poweron_all_rails(volatile EPS_State *state);

#endif /* INC_EPS_BOOTSEQUENCE_H_ */
