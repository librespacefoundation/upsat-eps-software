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

/** @addtogroup eps_safety
  * @{
  */

#define LIMIT_BATTERY_VOLTAGE_LOW_DEFAULT ((uint32_t) 1694)/**< 7.6Volt  X 222,93651*/
#define LIMIT_BATTERY_VOLTAGE_HIGH_DEFAULT ((uint32_t) 2854)/**< 12.8Volt X 222,93651*/
#define LIMIT_BATTERY_VOLTAGE_CRITICAL_DEFAULT ((uint32_t) 1895)/**< 8.5Volt*/
#define LIMIT_BATTERY_TEMPERATURE_LOW_DEFAULT ((uint32_t) 5)//**< 5 Celcius degree */
#define LIMIT_BATTERY_TEMPERATURE_HIGH_DEFAULT ((uint32_t) 55)/**< 55 Celcius degree */

/*Specified range for limit values - any value out of this range will not be considered*/
#define LIMIT_BATTERY_LOW_MAX ((uint32_t) 1784)/**< 8Volt  X 222,93651*/
#define LIMIT_BATTERY_LOW_MIN ((uint32_t) 666)/**<  3Volt  X 222,93651*/
#define LIMIT_BATTERY_HIGH_MAX ((uint32_t) 3344)/**< 15Volt  X 222,93651*/
#define LIMIT_BATTERY_HIGH_MIN ((uint32_t) 2007)/**< 9.1Volt  X 222,93651*/
#define LIMIT_BATTERY_CRITICAL_MAX ((uint32_t) 2006)/**< 9Volt  X 222,93651*/
#define LIMIT_BATTERY_CRITICAL_MIN ((uint32_t) 1785)/**< 8.1Volt  X 222,93651*/
#define LIMIT_BATTERY_TEMPERATURE_LOW__MAX ((uint32_t) 10)/**<  Celcius degree*/
#define LIMIT_BATTERY_TEMPERATURE_LOW__MIN ((uint32_t) 0)/**<  Celcius degree*/
#define LIMIT_BATTERY_TEMPERATURE_HIGH__MAX ((uint32_t) 100)/**<  Celcius degree*/
#define LIMIT_BATTERY_TEMPERATURE_HIGH__MIN ((uint32_t) 7)/**<  Celcius degree*/

/**
 * @brief safety system limit values.
 *
 * This structure is used to pass around to safety system related functions related safety limit values.
 * Limits are initially loaded from memory - must be properly initialized!
 */
typedef struct {
	uint32_t limit_battery_low;/**< Voltage level threshold of battery low - Undervoltage. */
	uint32_t limit_battery_high;/**< Voltage level threshold of battery high - Overvoltage. */
	uint32_t limit_battery_critical;/**<  Voltage level threshold of battery critical. */
	uint32_t limit_battery_temperature_low;/**<  Temperature level threshold of temperature high - OverTemperature. */
	uint32_t limit_battery_temperature_high;/**< Temperature level threshold of temperature low - UnderTemperature.  */
}EPS_safety_limits;


EPS_soft_error_status EPS_load_safety_limits_from_memory(EPS_safety_limits *limits);

//check for limits and take action.
EPS_soft_error_status EPS_perform_safety_checks(EPS_State *state, EPS_safety_limits *limits);


#endif /* INC_EPS_SAFETY_H_ */
/**
  * @}
  */
