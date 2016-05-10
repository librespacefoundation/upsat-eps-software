/*
 * eps_configuration.h
 *
 *  Created on: May 2, 2016
 *      Author: Aris Stathakis
 */

#ifndef INC_EPS_CONFIGURATION_H_
#define INC_EPS_CONFIGURATION_H_

#include "stm32l1xx_hal.h"

//#define TIMED_EVENT_PERIOD ((uint32_t)50000)//time period in uicroseconds.cpu wakes from interrupt: 1.update mppt algorithm 2.update eps state 3.poll for obc communication request 4.increment soft counter for battery temperatur control
#define TIMED_EVENT_PERIOD ((uint32_t)50000)

#define MPPT_STEP_SIZE ((uint32_t)1)
#define MPPT_STARTUP_PWM_DUTYCYCLE ((uint32_t) 16)//1 - 160 for 0 -100%duty cycle - must not start from 0

#define TC74_ADDRESS_A TC74_A2 // TC74	A2
#define TC74_ADDRESS_B TC74_A5 // TC74	A5

/*Set up power modules pwm timer channels*/
#define PWM_TIM_CHANNEL_TOP TIM_CHANNEL_3
#define PWM_TIM_CHANNEL_BOTTOM TIM_CHANNEL_1
#define PWM_TIM_CHANNEL_LEFT TIM_CHANNEL_2
#define PWM_TIM_CHANNEL_RIGHT TIM_CHANNEL_4
/*Set up power modules adc current channels*/
#define ADC_CURRENT_CHANNEL_TOP ADC_CHANNEL_5
#define ADC_CURRENT_CHANNEL_BOTTOM ADC_CHANNEL_20
#define ADC_CURRENT_CHANNEL_LEFT ADC_CHANNEL_11
#define ADC_CURRENT_CHANNEL_RIGHT ADC_CHANNEL_18
/*Set up power modules adc voltage channels*/
#define ADC_VOLTAGE_CHANNEL_TOP ADC_CHANNEL_6
#define ADC_VOLTAGE_CHANNEL_BOTTOM ADC_CHANNEL_21
#define ADC_VOLTAGE_CHANNEL_LEFT ADC_CHANNEL_10
#define ADC_VOLTAGE_CHANNEL_RIGHT ADC_CHANNEL_19

/*Setup eps state adc channels*/
#define ADC_VBAT ADC_CHANNEL_1;
#define ADC_IBAT_PLUS ADC_CHANNEL_2;
#define ADC_IBAT_MINUS ADC_CHANNEL_3;
#define ADC_I3V3 ADC_CHANNEL_4;
#define ADC_I5V ADC_CHANNEL_12;


typedef enum {
	TIMED_EVENT_SERVICED =0,
	TIMED_EVENT_NOT_SERVICED
}EPS_timed_event_status;



void kick_TIM6_timed_interrupt(uint32_t period_in_uicroseconds);


#endif /* INC_EPS_CONFIGURATION_H_ */
