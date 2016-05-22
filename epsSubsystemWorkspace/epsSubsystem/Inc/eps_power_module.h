/*
 * eps_power_module.h
 *
 *  Created on: May 2, 2016
 *      Author: ariolis
 */

#ifndef INC_EPS_POWER_MODULE_H_
#define INC_EPS_POWER_MODULE_H_

#include "stm32l1xx_hal.h"
#include "eps_configuration.h"
#include "eps_soft_error_handling.h"



typedef struct {
	uint16_t voltage; /*average voltage at each mppt step*/
	uint16_t current; /*average curret at each mppt step*/
	uint32_t previous_power; /*average power at previous mppt step*/
	uint32_t previous_voltage; /*average voltage input at previous mppt step*/
	uint8_t incremennt_flag;/*flag for mppt algorithm must be initialized to 1*/
	uint32_t pwm_duty_cycle; /*duty cycle of power module pwm output*/
	TIM_HandleTypeDef *htim_pwm;/*assign wich timer is assigned for this pwm output*/
	uint32_t timChannel;/*assign the proper timer channel assigned to module pwm output*/
	ADC_HandleTypeDef *hadc_power_module;/*adc handle for voltage and current measurements for each power module*/
	uint32_t ADC_channel_current;
	uint32_t ADC_channel_voltage;

}EPS_PowerModule;


void EPS_PowerModule_init(EPS_PowerModule *module_X, uint32_t starting_pwm_dutycycle, TIM_HandleTypeDef *htim, uint32_t timer_channel, ADC_HandleTypeDef *hadc_power_module, uint32_t ADC_channel_current, uint32_t ADC_channel_voltage);
void EPS_update_power_module_state(EPS_PowerModule *power_module);
void EPS_PowerModule_mppt_update_pwm(EPS_PowerModule *moduleX);
void EPS_PowerModule_mppt_apply_pwm(EPS_PowerModule *moduleX);
EPS_soft_error_status EPS_PowerModule_init_ALL(EPS_PowerModule *module_top, EPS_PowerModule *module_bottom, EPS_PowerModule *module_left, EPS_PowerModule *module_right);



#endif /* INC_EPS_POWER_MODULE_H_ */
