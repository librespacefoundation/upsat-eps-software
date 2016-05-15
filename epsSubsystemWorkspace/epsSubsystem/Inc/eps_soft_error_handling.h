/*
 * eps_soft_error_handling.h
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */

#ifndef INC_EPS_SOFT_ERROR_HANDLING_H_
#define INC_EPS_SOFT_ERROR_HANDLING_H_




typedef enum {
	EPS_SOFT_ERROR_OK,
	EPS_SOFT_ERROR_UNRESOLVED,
	/*add more cases here that will be returned and handled*/
	EPS_SOFT_ERROR_UMBILICAL_UNPREDICTED,
	EPS_SOFT_ERROR_LAST_VALUE
}EPS_soft_error_status;

//checks the status, if it is a known issue from the enumeration, tries to solve it in software
//else a soft reset is trigerred.
//if the error status is ok, the watchdog  is updated.
void EPS_soft_error_handling(EPS_soft_error_status eps_error_status);

#endif /* INC_EPS_SOFT_ERROR_HANDLING_H_ */
