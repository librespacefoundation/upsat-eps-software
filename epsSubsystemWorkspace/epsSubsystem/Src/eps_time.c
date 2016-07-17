/*
 * eps_time.c
 *
 *  Created on: Jul 17, 2016
 *      Author: ariolis
 */

#include "eps_time.h"


volatile uint32_t EPS_time_counter =0;/*a counter that increments at every interrupt every TIMED_EVENT_PERIOD microseconds */

/*returns the eps_timer value*/
uint32_t EPS_time_counter_get(void){

	return EPS_time_counter;

}

/*incrementss the eps_timer */
void EPS_time_counter_increment(void){

	++EPS_time_counter;
}
