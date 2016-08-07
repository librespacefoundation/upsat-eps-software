/*
 * eps_power_module.c
 *
 *  Created on: May 2, 2016
 *      Author: Aris Stathakis
 */

#include "eps_power_module.h"
#include "eps_state.h"


extern ADC_HandleTypeDef hadc;
extern TIM_HandleTypeDef htim3;

/** @addtogroup eps_power_module
  * @{
  */

/**
  * @brief Initializes power module instance.
  * @param  module_X: pointer to the power module instance to set.
  * @param  starting_pwm_dutycycle: initial duty cycle at which the power module will start the mppt !0 is not a good start.
  * @param  htim: pointer to timer peripheral handle for pwm generation.
  * @param  timer_channel: timer peripheral channel used for this pwm module.
  * @param  hadc_power_module: pointer to adc peripheral handle that holds the current/voltage measurements.
  * @param  ADC_channel_current: pointer to adc channel with the current measurement.
  * @param  ADC_channel_voltage:  pointer to adc channel with the voltage measurement.
  * @retval None.
  */
void EPS_PowerModule_init(EPS_PowerModule *module_X, uint32_t starting_pwm_dutycycle, TIM_HandleTypeDef *htim, uint32_t timer_channel, ADC_HandleTypeDef *hadc_power_module, uint32_t ADC_channel_current, uint32_t ADC_channel_voltage){

	//initialize properly all power module entries.
	module_X->module_state = POWER_MODULE_ON;
	module_X->current =0;
	module_X->voltage =0;
	module_X->previous_power =0;
	module_X->previous_voltage =0;
	module_X->pwm_duty_cycle = starting_pwm_dutycycle;
	module_X->htim_pwm = htim;
	module_X->timChannel = timer_channel;
	module_X->incremennt_flag = 1;//start by incrementing
	module_X->hadc_power_module = hadc_power_module;
	module_X->ADC_channel_current = ADC_channel_current;
	module_X->ADC_channel_voltage = ADC_channel_voltage;

	/*Start pwm with initialized from cube mx pwm duty cycle for timerX at timer_channel.*/
	HAL_TIM_PWM_Start(htim, timer_channel);

}

/**
  * @brief Initializes all satellite power module instances.
  * @param  module_top: pointer to top solar panel side power module.
  * @param  module_bottom: pointer to bottom solar panel side power module.
  * @param  module_left: pointer to left solar panel side power module.
  * @param  module_right: pointer to right solar panel side power module
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_PowerModule_init_ALL(EPS_PowerModule *module_top, EPS_PowerModule *module_bottom, EPS_PowerModule *module_left, EPS_PowerModule *module_right){

	error_status = EPS_SOFT_ERROR_POWER_MODULE_INIT_ALL;

	/*start timer3 pwm base generation (initialized pwm duty cycle from mx must be 0) */
	HAL_TIM_Base_Start(&htim3);

	/* initialize all power modules and dem mppt cotrol blocks.*/
	error_status = EPS_SOFT_ERROR_POWER_MODULE_INIT_TOP;
	EPS_PowerModule_init(module_top, MPPT_STARTUP_PWM_DUTYCYCLE, &htim3, PWM_TIM_CHANNEL_TOP, &hadc, ADC_CURRENT_CHANNEL_TOP, ADC_VOLTAGE_CHANNEL_TOP);
	error_status = EPS_SOFT_ERROR_POWER_MODULE_INIT_BOTTOM;
	EPS_PowerModule_init(module_bottom, MPPT_STARTUP_PWM_DUTYCYCLE, &htim3, PWM_TIM_CHANNEL_BOTTOM, &hadc, ADC_CURRENT_CHANNEL_BOTTOM, ADC_VOLTAGE_CHANNEL_BOTTOM);
	error_status = EPS_SOFT_ERROR_POWER_MODULE_INIT_LEFT;
	EPS_PowerModule_init(module_left, MPPT_STARTUP_PWM_DUTYCYCLE, &htim3, PWM_TIM_CHANNEL_LEFT, &hadc, ADC_CURRENT_CHANNEL_LEFT, ADC_VOLTAGE_CHANNEL_LEFT);
	error_status = EPS_SOFT_ERROR_POWER_MODULE_INIT_RIGHT;
	EPS_PowerModule_init(module_right, MPPT_STARTUP_PWM_DUTYCYCLE, &htim3, PWM_TIM_CHANNEL_RIGHT, &hadc, ADC_CURRENT_CHANNEL_RIGHT, ADC_VOLTAGE_CHANNEL_RIGHT);

	return EPS_SOFT_ERROR_POWER_MODULE_INIT_ALL_COMPLETE;
}

/**
  * @brief Update power module instance adc measurements.
  * @param  power_module: pointer to  power module instance for which to get adc measurements.
  * @retval None.
  */
void EPS_update_power_module_state(EPS_PowerModule *power_module){

	/*initialize adc handle*/
	power_module->hadc_power_module->Init.NbrOfConversion = 2;
	//power_module->hadc_power_module->NbrOfConversionRank = 2;
	HAL_ADC_Init(power_module->hadc_power_module);

	/*setup conversion sequence for */
	ADC_ChannelConfTypeDef sConfig;
	sConfig.SamplingTime = ADC_SAMPLETIME_192CYCLES;

	/*power module current*/
	sConfig.Channel = power_module->ADC_channel_current ;
	sConfig.Rank = 1;
	HAL_ADC_ConfigChannel(power_module->hadc_power_module, &sConfig);

	/*power module voltage*/
	sConfig.Channel = power_module->ADC_channel_voltage ;
	sConfig.Rank = 2;
	HAL_ADC_ConfigChannel(power_module->hadc_power_module, &sConfig);



	/*start dma transfer from adc to memory.*/

	uint32_t adc_measurement_dma_power_modules[67]= { 0 };//2*64 +1

	adc_reading_complete = ADC_TRANSFER_NOT_COMPLETED;//external global flag defined in main and shared with the adc complete transfer interrupt handler.
	HAL_ADC_Start_DMA(power_module->hadc_power_module, adc_measurement_dma_power_modules, 66);

	/*Process Measurements*/
	uint32_t voltage_avg =0;
	uint32_t current_avg =0;

	/*Wait till DMA ADC sequence transfer is ready*/
	while(adc_reading_complete==ADC_TRANSFER_NOT_COMPLETED){
		//wait for dma transfer complete.
	}
	/*ADC must be stopped in the adc dma transfer complete callback.*/

	HAL_ADC_Stop_DMA(power_module->hadc_power_module);

	/*de-interleave and sum voltage and current measurements.*/
	for (int sum_index = 2; sum_index < 66; sum_index+=2) {
		/*top*/
		current_avg = current_avg + adc_measurement_dma_power_modules[sum_index];
		voltage_avg = voltage_avg + adc_measurement_dma_power_modules[sum_index+1];
	}

	/*filter ting*/
	/*average of 16 concecutive adc measurements.skip the first to avoid adc power up distortion.*/
	power_module->voltage = voltage_avg>>5;
	power_module->current = current_avg>>5;


}

/**
  * @brief Update power module instance MPPT next step pwm duty cycle.
  * @param  moduleX: pointer to  power module instance for which to calculate pwm duty cycle based on P&O (protrube and observe) mppt sstrategy algorithm.
  * @retval None.
  */
void EPS_PowerModule_mppt_update_pwm(EPS_PowerModule *moduleX){



	/*power calculation*/

	volatile uint32_t power_now_avg = moduleX->voltage * moduleX->current;
	uint32_t duty_cycle = moduleX->pwm_duty_cycle;


	/*if solar cell voltage is below threshold, reset mppt point search starting from startup dutycycle*/
	if(moduleX->voltage<MPPT_VOLTAGE_THRESHOLD){

		duty_cycle = MPPT_STARTUP_PWM_DUTYCYCLE;

	}
	else{

		uint32_t step_size = MPPT_STEP_SIZE;

		/*decide duty cycle orientation - set increment flag.*/
		if (power_now_avg  <(moduleX->previous_power)){

			if (moduleX->incremennt_flag>0){

				if(moduleX->voltage  <(moduleX->previous_voltage -5)){
					moduleX->incremennt_flag = 0;
				}

			}
			else{
				moduleX->incremennt_flag = 1;
			}
		}
		/*add appropriate offset - create new duty cycle.*/

		if(moduleX->incremennt_flag){
			duty_cycle = duty_cycle+step_size;
		}
		else{
			duty_cycle = duty_cycle-step_size;
		}
		/*Check for Overflow and Underflow*/
		if (duty_cycle>(160+MPPT_STEP_SIZE)){//first check for underflow
			duty_cycle= MPPT_STARTUP_PWM_DUTYCYCLE;//
		}
		if (duty_cycle==(160+MPPT_STEP_SIZE)){//then check for overflow
			duty_cycle=160;
		}

	}

	  moduleX->previous_power = power_now_avg;
	  moduleX->previous_voltage = moduleX->voltage;
	  moduleX->pwm_duty_cycle = duty_cycle;

}

/**
  * @brief Apply calculated MPPT step pwm duty cycle.
  * @param  moduleX: pointer to  power module instance for which to apply the calculated pwm dutycycle.
  * @retval None.
  */
void EPS_PowerModule_mppt_apply_pwm(EPS_PowerModule *moduleX){

	uint32_t pwm_output;
	if (moduleX->module_state ==POWER_MODULE_OFF){
		pwm_output = 0;
	}
	else if(moduleX->module_state ==POWER_MODULE_ON){
		pwm_output = moduleX->pwm_duty_cycle;
	}
	else{
		pwm_output = 0;
	}

	switch ( moduleX->timChannel ) {
	case TIM_CHANNEL_1:         /*  top module */
		moduleX->htim_pwm->Instance->CCR1 = pwm_output;
		break;
	case TIM_CHANNEL_2:         /*  bottom module */
		moduleX->htim_pwm->Instance->CCR2 = pwm_output;
		break;
	case TIM_CHANNEL_3:         /*  left module */
		moduleX->htim_pwm->Instance->CCR3 = pwm_output;
		break;
	case TIM_CHANNEL_4:         /*  right module */
		moduleX->htim_pwm->Instance->CCR4 = pwm_output;
		break;
	default:
		//error handling?
		break;
	}

}
/**
  * @}
  */
