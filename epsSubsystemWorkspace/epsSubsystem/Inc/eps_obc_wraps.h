/*
 * eps_obc_wraps.h
 *
 *  Created on: May 21, 2016
 *      Author: ariolis
 */

#ifndef INC_EPS_OBC_WRAPS_H_
#define INC_EPS_OBC_WRAPS_H_

#include "eps.h"
#include "service_utilities.h"

#include "eps_soft_error_handling.h"


#undef __FILE_ID__
#define __FILE_ID__ 666

EPS_soft_error_status EPS_obc_communication_init(void);

EPS_soft_error_status EPS_obc_communication_service(void);






#endif /* INC_EPS_OBC_WRAPS_H_ */
