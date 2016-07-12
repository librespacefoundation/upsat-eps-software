/*
 * eps_state.c
 *
 *  Created on: May 2, 2016
 *      Author: Aris Stathakis
 */

#include "eps_state.h"
#include "tc74_temp_sensor.h"
#include "cpu_adc_utilities.h"

extern volatile uint8_t adc_reading_complete;//flag to check when dma transfer is complete.

/*update eps state analog measurements*/
static EPS_soft_error_status EPS_update_state_adc_measurements(volatile EPS_State *state, ADC_HandleTypeDef *hadc_eps);
/*get battery pack temperature*/
static int16_t get_batterypack_temperature(I2C_HandleTypeDef *h_i2c, uint8_t sensor_A_i2c_address, uint8_t sensor_B_i2c_address, EPS_State *state);



EPS_soft_error_status EPS_state_init(volatile EPS_State *state){

	EPS_soft_error_status eps_status = EPS_SOFT_ERROR_STATE_INIT;

	EPS_mode_status eps_functional_mode;
	EPS_safety_battery_status EPS_safety_battery_mode;
	EPS_safety_temperature_status EPS_safety_temperature_mode;

	state->eps_functional_mode = EPS_NOMINAL_MODE;
	state->EPS_safety_battery_mode = EPS_SAFETY_MODE_BATTERY_NOT_SET;
	state->EPS_safety_temperature_mode = EPS_SAFETY_MODE_TEMPERATURE_NOT_SET;
	state->v5_current_avg = 0x00;
	state->v3_3_current_avg = 0x00;
	state->battery_voltage = 0x00;
	state->battery_current_plus = 0x00;
	state->battery_current_minus = 0x00;
	state->battery_temp = 10;//do not initialize to zero so as not to false trigger safety limits during initialization.
	state->batterypack_health_status = EPS_BATTERY_SENSOR_SYSTEM_OK;


	eps_status = EPS_SOFT_ERROR_OK;
	return eps_status;

}

EPS_soft_error_status EPS_update_state(volatile EPS_State *state, ADC_HandleTypeDef *hadc_eps, I2C_HandleTypeDef *h_i2c) {

	EPS_soft_error_status eps_status = EPS_SOFT_ERROR_STATE_UPDATE;

	/*get eps adc measurements*/
	eps_status = EPS_update_state_adc_measurements(state, hadc_eps);

	/*get eps switch states*/
	//rail
	state->su_p_switch = EPS_get_rail_switch_status(SU);
	state->obc_p_switch = EPS_get_rail_switch_status(OBC);
 	state->adcs_p_switch = EPS_get_rail_switch_status(ADCS);
 	state->comm_p_switch = EPS_get_rail_switch_status(COMM);
 	state->i2c_tc74_p_switch= EPS_get_rail_switch_status(TEMP_SENSOR);

 	state->deploy_left_switch = EPS_get_control_switch_status(DEPLOY_LEFT);
 	state->deploy_right_switch = EPS_get_control_switch_status(DEPLOY_RIGHT);
 	state->deploy_bottom_switch = EPS_get_control_switch_status(DEPLOY_BOTTOM);
 	state->deploy_ant1_switch = EPS_get_control_switch_status(DEPLOY_ANT1);
    state->deploy_top_switch  = EPS_get_control_switch_status(DEPLOY_TOP);
    state->heaters_switch = EPS_get_control_switch_status(BATTERY_HEATERS);

    eps_status = EPS_SOFT_ERROR_STATE_UPDATE_BATTERYPACK_TEMP;
    state->battery_temp = get_batterypack_temperature( h_i2c, TC74_ADDRESS_A, TC74_ADDRESS_B, state);


	eps_status = EPS_SOFT_ERROR_OK;
	return eps_status;

}


static EPS_soft_error_status EPS_update_state_adc_measurements(volatile EPS_State *state, ADC_HandleTypeDef *hadc_eps){

	EPS_soft_error_status adc_update_state = EPS_SOFT_ERROR_UPDATE_STATE_ADC;

	/*initialize adc handle*/
	hadc_eps->Init.NbrOfConversion = 6;
	HAL_ADC_Init(hadc_eps);

	/*setup conversion sequence for */
	ADC_ChannelConfTypeDef sConfig;
	sConfig.SamplingTime = ADC_SAMPLETIME_384CYCLES;

	/*Vbat*/
	sConfig.Channel = ADC_VBAT;
	sConfig.Rank = 1;
	HAL_ADC_ConfigChannel(hadc_eps, &sConfig);

	/*Ibat+*/
	sConfig.Channel = ADC_IBAT_PLUS;
	sConfig.Rank = 2;
	HAL_ADC_ConfigChannel(hadc_eps, &sConfig);

	/*Ibat-*/
	sConfig.Channel = ADC_IBAT_MINUS;
	sConfig.Rank = 3;
	HAL_ADC_ConfigChannel(hadc_eps, &sConfig);

	/*I3v3*/
	sConfig.Channel = ADC_I3V3;
	sConfig.Rank = 4;
	HAL_ADC_ConfigChannel(hadc_eps, &sConfig);

	/*I5v*/
	sConfig.Channel = ADC_I5V;
	sConfig.Rank = 5;
	HAL_ADC_ConfigChannel(hadc_eps, &sConfig);

	/*cpu internal temp sensor*/
	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	sConfig.Rank = 6;
	HAL_ADC_ConfigChannel(hadc_eps, &sConfig);

	adc_update_state = EPS_SOFT_ERROR_UPDATE_STATE_ADC_DMA;

	/*Start conversion and dma transfer*/
	uint32_t adc_measurement_dma_eps_state[55]= { 0 };//2*6 +1

	adc_reading_complete = 0;
	HAL_ADC_Start_DMA(hadc_eps, adc_measurement_dma_eps_state, 54);

	/*Process Measurements*/
	uint32_t battery_voltage_avg =0;
	uint32_t battery_current_plus_avg =0;
	uint32_t battery_current_minus_avg =0;
	uint32_t v3_3_current_avg =0;
	uint32_t v5_current_avg =0;
	uint32_t cpu_temp_avg =0;

	/*Wait till DMA ADC sequence transfer is ready*/
	while(adc_reading_complete==0){
 	}
	HAL_ADC_Stop_DMA(hadc_eps);


	adc_update_state = EPS_SOFT_ERROR_UPDATE_STATE_ADC_FILTER;

	//de-interleave and sum adc state measurements.overflow strategy??? : 2^12(max adc value) * 32 = 2^17 < 2^32 so you do not need one!
	for (int sum_index = 6; sum_index < 54; sum_index+=6) {
		/*top*/
		battery_voltage_avg = battery_voltage_avg + adc_measurement_dma_eps_state[sum_index];
		battery_current_plus_avg = battery_current_plus_avg + adc_measurement_dma_eps_state[sum_index+1];
		battery_current_minus_avg = battery_current_minus_avg + adc_measurement_dma_eps_state[sum_index+2];
		v3_3_current_avg = v3_3_current_avg + adc_measurement_dma_eps_state[sum_index+3];
		v5_current_avg = v5_current_avg + adc_measurement_dma_eps_state[sum_index+4];
		cpu_temp_avg = cpu_temp_avg + adc_measurement_dma_eps_state[sum_index+5];
	}

	/*filter ting*/
	//average of 8 concecutive adc measurements.skip the first to avoid adc power up distortion.
	state->battery_voltage = battery_voltage_avg>>3;
	state->battery_current_plus = battery_current_plus_avg>>3;
	state->battery_current_minus = battery_current_minus_avg>>3;
	state->v3_3_current_avg = v3_3_current_avg>>3;
	state->v5_current_avg = v5_current_avg>>3;

	adc_update_state = EPS_SOFT_ERROR_UPDATE_STATE_ADC_CPU_TEMP;

	state->cpu_temperature = COMPUTATION_TEMPERATURE_TEMP30_TEMP110((cpu_temp_avg>>3));

	adc_update_state = EPS_SOFT_ERROR_OK;
	return adc_update_state;
}



int16_t get_batterypack_temperature(I2C_HandleTypeDef *h_i2c, uint8_t sensor_A_i2c_address, uint8_t sensor_B_i2c_address, EPS_State *state){

	static  uint8_t battery_temperature_valid_counter=0;//counter to control the battery sensor scheme
	/*calculate battery temperature depending on sensor status*/
	int16_t battery_temp_avg;


	if(battery_temperature_valid_counter>19){


		/* if i2c rail is off turn it on - this is how to reset the i2c power line.*/
		if(EPS_get_rail_switch_status(TEMP_SENSOR)==EPS_SWITCH_RAIL_OFF){
			EPS_set_rail_switch(TEMP_SENSOR, EPS_SWITCH_RAIL_ON, state);
		}

		TC_74_STATUS statusA, statusB;
		/*wake up sensor1*/
		statusA = device_wake_up( h_i2c, sensor_A_i2c_address);

		/*wake up sensor2*/
		statusB = device_wake_up( h_i2c, sensor_B_i2c_address);

		/*check sensor1 status if still in standby or error*/
		if(statusA == DEVICE_NORMAL){
			statusA = read_device_status(h_i2c, sensor_A_i2c_address);
		}

		/*check sensor2 status if still in standby or error*/
		if(statusB == DEVICE_NORMAL){
			statusB = read_device_status(h_i2c, sensor_B_i2c_address);
		}


		/*get temperatue1*/
		int8_t temperature_measurementA;
		if(statusA == DEVICE_NORMAL){
			statusA= read_device_temperature(h_i2c, sensor_A_i2c_address, &temperature_measurementA);
		}
		/*get temperatue2*/
		int8_t temperature_measurementB;
		if(statusB == DEVICE_NORMAL){
			statusB= read_device_temperature(h_i2c, sensor_B_i2c_address, &temperature_measurementB);
		}

		/*if you put temp sesors to sleep you will never have a new conversion */
		/* beloved tc74 has 4 samples per second rate - thank you ti*/




		if((statusA!=DEVICE_ERROR)&&(statusB!=DEVICE_ERROR)){
			//nominal state: both temp sensors not in erroneous state.
			battery_temp_avg =  (temperature_measurementA + temperature_measurementB)>>1;
			state->batterypack_health_status = EPS_BATTERY_SENSOR_SYSTEM_OK;
		}
		else if((statusA!=DEVICE_ERROR)&&(statusB==DEVICE_ERROR)){
			//temp sensor A is OK but B is in eroneous state.
			battery_temp_avg =  temperature_measurementA;
			state->batterypack_health_status = EPS_BATTERY_SENSOR_B_DEAD;
		}
		else if((statusA==DEVICE_ERROR)&&(statusA!=DEVICE_ERROR)){
			//temp sensor A is OK but B is in eroneous state.
			battery_temp_avg =  temperature_measurementB;
			state->batterypack_health_status = EPS_BATTERY_SENSOR_A_DEAD;
		}
		else if((statusA==DEVICE_ERROR)&&(statusA==DEVICE_ERROR)){
			//both temp sensors are dead... so we extraploate the only temp sensor availiable, the cpu temp sensor.
			battery_temp_avg = state->cpu_temperature - CPU_TO_BATTERY_TEMPERATURE_OFFSET;
			state->batterypack_health_status = EPS_BATTERY_SENSOR_CPU_TEMP_ONLY;
			/* turn of power temperature power supply so as to reset to next temp requet */
			EPS_set_rail_switch(TEMP_SENSOR, EPS_SWITCH_RAIL_OFF, state);

			//TODO: find the relation of cpu temperature to battery pack temperature.
		}
		else{
			//TODO: handle this ...
			state->batterypack_health_status = EPS_BATTERY_SENSOR_LAST_VALUE;
		}

		battery_temperature_valid_counter = 0;//reset valid battery temperature counter

	}
	else{
		//if o update has been made - return the previous state.
		battery_temp_avg = state->battery_temp;
		battery_temperature_valid_counter++;
	}

	 return battery_temp_avg;

}




void EPS_set_rail_switch(EPS_switch_rail eps_switch, EPS_switch_rail_status switch_status, EPS_State *state){

 	GPIO_PinState gpio_write_value;

	if(switch_status == EPS_SWITCH_RAIL_ON){
		gpio_write_value = GPIO_PIN_RESET;
 	}
	else{
		gpio_write_value = GPIO_PIN_SET;
 	}


	switch (eps_switch) {

	case SU:
		HAL_GPIO_WritePin(GPIO_SU_SWITCH_GPIO_Port, GPIO_SU_SWITCH_Pin, gpio_write_value);
		state->su_p_switch = switch_status;
		break;

	case OBC:
		HAL_GPIO_WritePin(GPIO_OBC_SWITCH_GPIO_Port, GPIO_OBC_SWITCH_Pin, gpio_write_value);
		state->obc_p_switch = switch_status;
		break;

	case ADCS:
		HAL_GPIO_WritePin(GPIO_ADCS_SWITCH_GPIO_Port, GPIO_ADCS_SWITCH_Pin, gpio_write_value);
		state->adcs_p_switch = switch_status;
		break;

	case COMM:
		HAL_GPIO_WritePin(GPIO_COMM_SWITCH_GPIO_Port, GPIO_COMM_SWITCH_Pin, gpio_write_value);
		state->comm_p_switch = switch_status;
		break;

	case TEMP_SENSOR:
		HAL_GPIO_WritePin(GPIO_TC74_POWER_GPIO_Port, GPIO_TC74_POWER_Pin, gpio_write_value);
		state->i2c_tc74_p_switch = switch_status;
		break;

// 	   default :
// 		   //YOU SHOULDNT BE HERE!
// 		   //ERROR HANDLING
	}


}




void EPS_set_control_switch(EPS_switch_control eps_switch, EPS_switch_control_status switch_status, EPS_State *state){

 	GPIO_PinState gpio_write_value;

	if(switch_status == EPS_SWITCH_CONTROL_ON){
		gpio_write_value = GPIO_PIN_SET;
 	}
	else{
		gpio_write_value = GPIO_PIN_RESET;
 	}


	switch (eps_switch) {


	case DEPLOY_LEFT:
		HAL_GPIO_WritePin(GPIO_DEPLOY_LEFT_GPIO_Port, GPIO_DEPLOY_LEFT_Pin, gpio_write_value);
		state->deploy_left_switch = switch_status;
		break;

	case DEPLOY_RIGHT:
		HAL_GPIO_WritePin(GPIO_DEPLOY_RIGHT_GPIO_Port, GPIO_DEPLOY_RIGHT_Pin, gpio_write_value);
		state->deploy_right_switch = switch_status;
		break;

	case DEPLOY_BOTTOM:
		HAL_GPIO_WritePin(GPIO_DEPLOY_BOTTOM_GPIO_Port, GPIO_DEPLOY_BOTTOM_Pin, gpio_write_value);
		state->deploy_bottom_switch = switch_status;
		break;

	case DEPLOY_ANT1:
		HAL_GPIO_WritePin(GPIO_DEPLOY_ANT1_GPIO_Port, GPIO_DEPLOY_ANT1_Pin, gpio_write_value);
		state->deploy_ant1_switch = switch_status;
		break;

	case BATTERY_HEATERS:
		HAL_GPIO_WritePin(GPIO_HEATERS_GPIO_Port, GPIO_HEATERS_Pin, gpio_write_value);
		state->heaters_switch = switch_status;
		break;
	case DEPLOY_TOP:
		HAL_GPIO_WritePin(GPIO_DEPLOY_TOP_GPIO_Port, GPIO_DEPLOY_TOP_Pin, gpio_write_value);
		state->deploy_top_switch = switch_status;
		break;

// 	   default :
// 		   //YOU SHOULDNT BE HERE!
// 		   //ERROR HANDLING
	}


}



EPS_switch_rail_status EPS_get_rail_switch_status(EPS_switch_rail eps_switch) {

	EPS_switch_rail_status return_status;
	GPIO_PinState gpio_read_value;

	switch (eps_switch) {

	case SU:
		gpio_read_value =HAL_GPIO_ReadPin(GPIO_SU_SWITCH_GPIO_Port, GPIO_SU_SWITCH_Pin);
		break;

	case OBC:
		gpio_read_value =HAL_GPIO_ReadPin(GPIO_OBC_SWITCH_GPIO_Port, GPIO_OBC_SWITCH_Pin);
		break;

	case ADCS:
		gpio_read_value =HAL_GPIO_ReadPin(GPIO_ADCS_SWITCH_GPIO_Port, GPIO_ADCS_SWITCH_Pin);
		break;

	case COMM:
		gpio_read_value =HAL_GPIO_ReadPin(GPIO_COMM_SWITCH_GPIO_Port, GPIO_COMM_SWITCH_Pin);
		break;

	case TEMP_SENSOR:
		gpio_read_value =HAL_GPIO_ReadPin(GPIO_TC74_POWER_GPIO_Port, GPIO_TC74_POWER_Pin);
		break;

// 	   default :
// 		   //YOU SHOULDNT BE HERE!
// 		   //ERROR HANDLING
	}

	if(gpio_read_value == GPIO_PIN_RESET){
		return_status = EPS_SWITCH_RAIL_ON;
	}
	else{
		return_status =  EPS_SWITCH_RAIL_OFF;
	}

	return return_status;

}

EPS_switch_control_status EPS_get_control_switch_status(EPS_switch_control eps_switch) {

	EPS_switch_control_status return_status;
	GPIO_PinState gpio_read_value;

	switch (eps_switch) {

	case DEPLOY_LEFT:
		gpio_read_value =HAL_GPIO_ReadPin(GPIO_DEPLOY_LEFT_GPIO_Port, GPIO_DEPLOY_LEFT_Pin);
		break;

	case DEPLOY_RIGHT:
		gpio_read_value =HAL_GPIO_ReadPin(GPIO_DEPLOY_RIGHT_GPIO_Port, GPIO_DEPLOY_RIGHT_Pin);
		break;

	case DEPLOY_BOTTOM:
		gpio_read_value =HAL_GPIO_ReadPin(GPIO_DEPLOY_BOTTOM_GPIO_Port, GPIO_DEPLOY_BOTTOM_Pin);
		break;

	case DEPLOY_ANT1:
		gpio_read_value =HAL_GPIO_ReadPin(GPIO_DEPLOY_ANT1_GPIO_Port, GPIO_DEPLOY_ANT1_Pin);
		break;

	case BATTERY_HEATERS:
		gpio_read_value =HAL_GPIO_ReadPin(GPIO_HEATERS_GPIO_Port, GPIO_HEATERS_Pin);
		break;
	case DEPLOY_TOP:
		gpio_read_value =HAL_GPIO_ReadPin(GPIO_DEPLOY_TOP_GPIO_Port, GPIO_DEPLOY_TOP_Pin);
		break;

// 	   default :
// 		   //YOU SHOULDNT BE HERE!
// 		   //ERROR HANDLING
	}

	if(gpio_read_value == GPIO_PIN_RESET){
		return_status = EPS_SWITCH_CONTROL_OFF;
	}
	else{
		return_status =  EPS_SWITCH_CONTROL_ON;
	}

	return return_status;

}
