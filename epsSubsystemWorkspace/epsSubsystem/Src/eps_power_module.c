/*
 * eps_power_module.c
 *
 *  Created on: May 2, 2016
 *      Author: Aris Stathakis
 */

#include "eps_power_module.h"


extern volatile uint8_t adc_reading_complete;//flag to check when dma transfer is complete.


void EPS_PowerModule_init(EPS_PowerModule *module_X, uint32_t starting_pwm_dutycycle, TIM_HandleTypeDef *htim, uint32_t timer_channel, ADC_HandleTypeDef *hadc_power_module, uint32_t ADC_channel_current, uint32_t ADC_channel_voltage){

	//initialize properly all power module entries.
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

	//Start pwm with initialized from cube mx pwm duty cycle for timerX at timer_channel.
	HAL_TIM_PWM_Start(htim, timer_channel);

}




void EPS_update_power_module_state(EPS_PowerModule *power_module){



    //setup adc handle sequence and initialize adc peripheral

	/*initialize adc handle*/
	power_module->hadc_power_module->NbrOfConversionRank = 2;
	HAL_ADC_Init(power_module->hadc_power_module);

	/*setup conversion sequence for */
	ADC_ChannelConfTypeDef sConfig;
	sConfig.SamplingTime = ADC_SAMPLETIME_48CYCLES;

	/*power module current*/
	sConfig.Channel = power_module->ADC_channel_current ;
	sConfig.Rank = 1;
	HAL_ADC_ConfigChannel(power_module->hadc_power_module, &sConfig);

	/*power module voltage*/
	sConfig.Channel = power_module->ADC_channel_voltage ;
	sConfig.Rank = 2;
	HAL_ADC_ConfigChannel(power_module->hadc_power_module, &sConfig);



	//start dma transfer from adc to memory.

	uint32_t adc_measurement_dma_power_modules[67]= { 0 };//2*64 +1

	adc_reading_complete = 0;//external global flag defined in main and shared with the adc complete transfer interrupt handler.
	HAL_ADC_Start_DMA(power_module->hadc_power_module, adc_measurement_dma_power_modules, 66);

	/*Process Measurements*/
	uint32_t voltage_avg =0;
	uint32_t current_avg =0;

	/*Wait till DMA ADC sequence transfer is ready*/
	while(adc_reading_complete==0){
		//wait for dma transfer complete.
	}
	//ADC must be stopped in the adc dma transfer complete callback.

	HAL_ADC_Stop_DMA(power_module->hadc_power_module);
	//HAL_ADC_Stop(power_module->hadc_power_module);


	//de-interleave and sum voltage and current measurements. TODO:overflow strategy??? : 2^12(max adc value) * 32 = 2^17 < 2^32 so you do not need one!
	for (int sum_index = 2; sum_index < 66; sum_index+=2) {
		/*top*/
		current_avg = current_avg + adc_measurement_dma_power_modules[sum_index];
		voltage_avg = voltage_avg + adc_measurement_dma_power_modules[sum_index+1];
	}

	/*filter ting*/
	//average of 16 concecutive adc measurements.skip the first to avoid adc power up distortion.
	power_module->voltage = current_avg>>5;
	power_module->current = voltage_avg>>5;


}


void EPS_PowerModule_mppt_update_pwm(EPS_PowerModule *moduleX){



	  /*power calculation*/

	  volatile uint32_t power_now_avg = moduleX->voltage * moduleX->current;
	  uint32_t duty_cycle = moduleX->pwm_duty_cycle;

	  uint32_t step_size = MPPT_STEP_SIZE;

//	  if (moduleX->current<1000){
//		  step_size = MPPT_STEP_SIZE + 5;
//		  //moduleX->incremennt_flag = 1;
//	  }



	// decide duty cycle orientation - set increment flag.
	  if (power_now_avg  <(moduleX->previous_power)){

		  if (moduleX->incremennt_flag>0){

			  if(moduleX->voltage  <(moduleX->previous_voltage )){
				  moduleX->incremennt_flag = 0;
			  }


		  }
		  else{
			  moduleX->incremennt_flag = 1;
		  }
	  }
    //add appropriate offset - create new duty cycle.


	  if(moduleX->incremennt_flag){
		  duty_cycle = duty_cycle+step_size;
	  }
	  else{
		  duty_cycle = duty_cycle-step_size;
	  }
 	  //Check for Overflow and Underflow
	  if (duty_cycle>350){duty_cycle=0;}
	  if (duty_cycle>320){duty_cycle=310;}
    // Set new PWM compare register
	  //duty_cycle =0;


	  moduleX->previous_power = power_now_avg;
	  moduleX->previous_voltage = moduleX->voltage;
	  moduleX->pwm_duty_cycle = duty_cycle;

}


void EPS_PowerModule_mppt_apply_pwm(EPS_PowerModule *moduleX){

	switch ( moduleX->timChannel ) {
	case TIM_CHANNEL_1:         /*  top module */
		moduleX->htim_pwm->Instance->CCR1 = moduleX->pwm_duty_cycle;
		break;
	case TIM_CHANNEL_2:         /*  bottom module */
		moduleX->htim_pwm->Instance->CCR2 = moduleX->pwm_duty_cycle;
		break;
	case TIM_CHANNEL_3:         /*  left module */
		moduleX->htim_pwm->Instance->CCR3 = moduleX->pwm_duty_cycle;
		break;
	case TIM_CHANNEL_4:         /*  right module */
		moduleX->htim_pwm->Instance->CCR4 = moduleX->pwm_duty_cycle;
		break;
	default:
		//error handling?
		break;
	}

}

