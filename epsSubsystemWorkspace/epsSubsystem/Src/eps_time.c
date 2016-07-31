/*
 * eps_time.c
 *
 *  Created on: Jul 17, 2016
 *      Author: ariolis
 */

#include "eps_time.h"


volatile uint32_t EPS_time_counter =0;/*a counter that increments at every interrupt every TIMED_EVENT_PERIOD microseconds */
volatile EPS_timed_event_status EPS_event_period_status = TIMED_EVENT_NOT_SERVICED;/* initialize global timed event flag to true.*/


/** @addtogroup eps_time
  * @{
  */

/**
  * @brief  Returns the eps_timer value.
  *         EPS timer has a time resolution of 50msec. This is done because when in sleep mode the systick is paused.
  *         Instead of hal_sys_tick this function is used when in need and can be multiplied by 50 to get a 1msec resolution.
  * @retval EPS_time_counter(!global ting!) value.
  */
uint32_t EPS_time_counter_get(void){

	return EPS_time_counter;

}

/**
  * @brief  Incrementss the EPS_time_counter.
  *         EPS timer has a time resolution of 50msec. This is done because when in sleep mode the systick is paused.
  *         Instead of hal_sys_tick this function is used when in need and can be multiplied by 50 to get a 1msec resolution.
  * @retval none.
  */
void EPS_time_counter_increment(void){

	++EPS_time_counter;
	if(EPS_time_counter>85899345){EPS_time_counter=0;}
}
/**
  * @}
  */
