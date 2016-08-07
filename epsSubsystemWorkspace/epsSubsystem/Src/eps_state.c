/*
 * eps_state.c
 *
 *  Created on: May 2, 2016
 *      Author: Aris Stathakis
 */

#include "eps_state.h"
#include "tc74_temp_sensor.h"
#include "cpu_adc_utilities.h"

EPS_adc_dma_transfer_status adc_reading_complete=ADC_TRANSFER_NOT_COMPLETED;/* flag to check when dma transfer is complete.*/

/*update eps state analog measurements*/
static EPS_soft_error_status EPS_update_state_adc_measurements(volatile EPS_State *state, ADC_HandleTypeDef *hadc_eps);
/*get battery pack temperature*/
static int16_t get_batterypack_temperature(I2C_HandleTypeDef *h_i2c, uint8_t sensor_A_i2c_address, uint8_t sensor_B_i2c_address,volatile EPS_State *state);


/** @addtogroup eps_state_Functions
  * @{
  */

/**
  * @brief Initialize the eps state structure with valid initial values.
  * @param  state: the eps state structure containing central info of the EPS subsystem.
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_state_init(volatile EPS_State *state){

	error_status = EPS_SOFT_ERROR_STATE_INIT;

	state->eps_functional_mode = EPS_MODE_LAST_VALUE;
	state->EPS_safety_battery_mode = EPS_SAFETY_MODE_BATTERY_NOT_SET;
	state->EPS_safety_temperature_mode = EPS_SAFETY_MODE_TEMPERATURE_NOT_SET;
	state->v5_current_avg = 0x00;
	state->v3_3_current_avg = 0x00;
	state->battery_voltage = 0x00;
	state->battery_current_plus = 0x00;
	state->battery_current_minus = 0x00;
	state->battery_temp = 10;//do not initialize to zero so as not to false trigger safety limits during initialization.
	state->batterypack_health_status = EPS_BATTERY_SENSOR_LAST_VALUE;

	return EPS_SOFT_ERROR_STATE_INIT_COMPLETE;

}

/**
  * @brief Update the eps state structure with state values. This is called every 50msec as the first main task of the eps.
  *        This basically collect info from the adc connected sensors, and the temperature sensors.
  * @param  state: the eps state structure containing central info of the EPS subsystem.
  * @retval Error status for handling and debugging.
  */
EPS_soft_error_status EPS_update_state(volatile EPS_State *state, ADC_HandleTypeDef *hadc_eps, I2C_HandleTypeDef *h_i2c) {

	error_status = EPS_SOFT_ERROR_STATE_UPDATE;

	/*get eps adc measurements*/
	error_status = EPS_update_state_adc_measurements(state, hadc_eps);

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

    error_status = EPS_SOFT_ERROR_STATE_UPDATE_BATTERYPACK_TEMP;
    state->battery_temp = get_batterypack_temperature( h_i2c, TC74_ADDRESS_A, TC74_ADDRESS_B, state);


    error_status = EPS_SOFT_ERROR_OK;
	return EPS_SOFT_ERROR_STATE_UPDATE_COMPLETE;

}

/**
  * @brief  set adc handle properly and get adc measurements and perform a basic low pass filtering on them.
  * @param  state: pointer to the eps state structure containing central info of the EPS subsystem.
  * @param  hadc_eps: pointer to the adc peripheral handle.
  * @retval Error status for handling and debugging.
  */
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

	adc_reading_complete = ADC_TRANSFER_NOT_COMPLETED;
	HAL_ADC_Start_DMA(hadc_eps, adc_measurement_dma_eps_state, 54);

	/*Process Measurements*/
	uint32_t battery_voltage_avg =0;
	uint32_t battery_current_plus_avg =0;
	uint32_t battery_current_minus_avg =0;
	uint32_t v3_3_current_avg =0;
	uint32_t v5_current_avg =0;
	uint32_t cpu_temp_avg =0;

	/*Wait till DMA ADC sequence transfer is ready*/
	while(adc_reading_complete==ADC_TRANSFER_NOT_COMPLETED){
 	}
	HAL_ADC_Stop_DMA(hadc_eps);


	adc_update_state = EPS_SOFT_ERROR_UPDATE_STATE_ADC_FILTER;

	/*de-interleave and sum adc state measurements.*/
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
	/*average of 8 concecutive adc measurements.skip the first to avoid adc power up distortion.*/
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

/**
  * @brief  Strategy to calculate eps batterypack temperature.
  *         The battery temperature measurement is updated every 20 calls of this function, assuming it is called once every 50msec,
  *         the battery measurement update mechanism is:
  *         calls:  -0-1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19-
  *         actions:|--------sleep---------------|----wakeup----|--measure|
  *         tc74 temperature sensors need at least 125msec to return a valid measurement from wakeup.
  *         If a temperature sensor returns a device error status, only the valid temperature is considered.
  *         If both sensors are functioning then the average values is returned.
  *         If both sensors are malfunctioning then the stm32l1 internal temp sensor is considered plus a calibration
  *          constant that has been derived from experimental calibration.
  * @param  h_i2c: pointer to the i2c bus peripheral handle that the temperature sensor ics are connected.
  * @param  sensor_A_i2c_address: i2c address of the first tc74 temperature sensor.
  * @param  sensor_B_i2c_address: i2c address of the second tc74 temperature sensor.
  * @param  state: pointer to the eps state structure containing central info of the EPS subsystem.
  * @retval The measured battery-pack temperature given the sensors device status.
  */
int16_t get_batterypack_temperature(I2C_HandleTypeDef *h_i2c, uint8_t sensor_A_i2c_address, uint8_t sensor_B_i2c_address,volatile EPS_State *state){

	static  uint8_t battery_temperature_valid_counter=0;/*counter to control the battery sensor scheme*/
	/*calculate battery temperature depending on sensor status*/
	int16_t battery_temp_avg;
	TC_74_STATUS statusA =DEVICE_STATUS_LAST_VALUE;
	TC_74_STATUS statusB =DEVICE_STATUS_LAST_VALUE;

	/*add check to turn on tc74power if they are off*/
	if(EPS_get_rail_switch_status(TEMP_SENSOR)==EPS_SWITCH_RAIL_OFF){
		EPS_set_rail_switch( TEMP_SENSOR, EPS_SWITCH_RAIL_ON, state);
		battery_temperature_valid_counter=0;//start the tc74 temperature cycle from start.
	}

	if(battery_temperature_valid_counter<15){
		/*sleep time*/
		battery_temp_avg = state->battery_temp;/*update return value*/
		battery_temperature_valid_counter++;/*update counter*/
	}
	else if((battery_temperature_valid_counter>=15)&&(battery_temperature_valid_counter<18)){
		/*waky-waky*/
		statusA = TC74_read_device_status(h_i2c, sensor_A_i2c_address);
		statusB = TC74_read_device_status(h_i2c, sensor_B_i2c_address);

		if(statusA == DEVICE_STANDBY){
			/*wake up sensor1*/
			statusA = TC74_device_wake_up( h_i2c, sensor_A_i2c_address);
		}

		if(statusB == DEVICE_STANDBY){
			/*wake up sensor2*/
			statusB = TC74_device_wake_up( h_i2c, sensor_B_i2c_address);
		}
		battery_temp_avg = state->battery_temp;/*update return value*/
	    battery_temperature_valid_counter++;/*update counter*/
	}

	else if((battery_temperature_valid_counter>=18)&&(battery_temperature_valid_counter<20)){
		//take measurement

		statusA = TC74_read_device_status(h_i2c, sensor_A_i2c_address);
		statusB = TC74_read_device_status(h_i2c, sensor_B_i2c_address);

		/*get temperatue1*/
		int8_t temperature_measurementA;
		if(statusA == DEVICE_NORMAL_DATA_READY){
			statusA= TC74_read_device_temperature(h_i2c, sensor_A_i2c_address, &temperature_measurementA);
		}
		/*get temperatue2*/
		int8_t temperature_measurementB;
		if(statusB == DEVICE_NORMAL_DATA_READY){
			statusB= TC74_read_device_temperature(h_i2c, sensor_B_i2c_address, &temperature_measurementB);
		}



		if((statusA==DEVICE_NORMAL_DATA_READY)&&(statusB==DEVICE_NORMAL_DATA_READY)){
			//nominal state: both temp sensors not in erroneous state.
			battery_temp_avg =  (temperature_measurementA + temperature_measurementB)>>1;
			state->batterypack_health_status = EPS_BATTERY_SENSOR_SYSTEM_OK;
		}
		else if((statusA==DEVICE_NORMAL_DATA_READY)&&(statusB!=DEVICE_NORMAL_DATA_READY)){
			//temp sensor A is OK but B is in eroneous state.
			battery_temp_avg =  temperature_measurementA;
			state->batterypack_health_status = EPS_BATTERY_SENSOR_B_DEAD;
		}
		else if((statusA!=DEVICE_NORMAL_DATA_READY)&&(statusB==DEVICE_NORMAL_DATA_READY)){
			//temp sensor A is OK but B is in eroneous state.
			battery_temp_avg =  temperature_measurementB;
			state->batterypack_health_status = EPS_BATTERY_SENSOR_A_DEAD;
		}
		else if((statusA!=DEVICE_NORMAL_DATA_READY)&&(statusB!=DEVICE_NORMAL_DATA_READY)){
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
			battery_temperature_valid_counter=0;//reset tc74 cycle.
		}

		battery_temperature_valid_counter++;/*update counter*/
		/*if counter overflows zero out*/
		if(battery_temperature_valid_counter>=20){
			/*reset valid battery temperature counter*/
			battery_temperature_valid_counter = 0;
			/*put sensor to sleep*/
			statusA = TC74_device_sleep( h_i2c, sensor_A_i2c_address);
			statusB = TC74_device_sleep( h_i2c, sensor_B_i2c_address);
		}



	}
	else{
		//handle this-you shouldnt be here.
	}


	return battery_temp_avg;


}

/**
  * @brief  Turn on or off the power switches of the connected subsystem. These are inverted logic mosfets that control
  *          power supply rails at the rest of the satellite subsystems.
  * @param  eps_switch: eps power control subsystem ports.
  * @param  switch_status: power switch status.
  * @param  state: pointer to the eps state structure containing central info of the EPS subsystem.
  * @retval None.
  */
void EPS_set_rail_switch(EPS_switch_rail eps_switch, EPS_switch_rail_status switch_status,volatile EPS_State *state){

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
	case RAIL_LAST_VALUE:
		//TODO:error handling
		break;

// 	   default :
// 		   //YOU SHOULDNT BE HERE!
// 		   //ERROR HANDLING
	}


}



/**
  * @brief  Turn on or off the control switches of the eps control mechanisms. These are normal logic mosfets that control
  *         deploy system and battery heaters safety mechanism.
  * @param  eps_switch: eps control port.
  * @param  switch_status: control switch status.
  * @param  state: pointer to the eps state structure containing central info of the EPS subsystem.
  * @retval None.
  */
void EPS_set_control_switch(EPS_switch_control eps_switch, EPS_switch_control_status switch_status,volatile EPS_State *state){

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
	case CONTROL_LAST_VALUE:
		//TODO:error handling
		break;

// 	   default :
// 		   //YOU SHOULDNT BE HERE!
// 		   //ERROR HANDLING
	}


}


/**
  * @brief  get the state of a power switch.
  * @param  eps_switch: eps power port.
  * @retval  switch_status: power switch status.
  */
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
	case RAIL_LAST_VALUE:
		//TODO:error handling
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

/**
  * @brief  get the state of a control switch.
  * @param  eps_switch: eps control port.
  * @retval  switch_status: control switch status.
  */
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
	case CONTROL_LAST_VALUE:
		//TODO:error handling
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
/**
  * @}
  */
