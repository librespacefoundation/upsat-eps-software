/*
 * eps_safety.h
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */

#ifndef INC_EPS_SAFETY_H_
#define INC_EPS_SAFETY_H_

#include "eps_state.h"
#include "eps_soft_error_handling.h"


#define LIMIT_BATTERY_LOW_DEFAULT ((uint32_t) 0x12345678)
#define LIMIT_BATTERY_HIGH_DEFAULT ((uint32_t) 0x12345678)
#define LIMIT_BATTERY_CRITICAL_DEFAULT ((uint32_t) 0x12345678)
#define LIMIT_BATTERY_TEMPERATURE_LOW_DEFAULT ((uint32_t) 0x12345678)

typedef struct {
	/**/
	uint32_t limit_battery_low;
	uint32_t limit_battery_high;
	uint32_t limit_battery_critical;
	uint32_t limit_battery_temperature_low;

}EPS_safety_limits;


//load limits from memory - must be properly iitialized!
//limits are loaded from flash so in case of eroneous operation
//a reset will not demand to communicate again with the eps and
//cocmmand a new limit - its safety and got to be safe.
void EPS_load_safety_limits_from_memory(EPS_safety_limits *limits);

//check for limits and take action.
EPS_soft_error_status EPS_perform_safety_checks(EPS_State *state, EPS_safety_limits *limits);


#endif /* INC_EPS_SAFETY_H_ */
