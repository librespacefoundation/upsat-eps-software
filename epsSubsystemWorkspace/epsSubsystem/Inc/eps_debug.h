/*
 * eps_debug.h
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */

#ifndef SRC_EPS_DEBUG_H_
#define SRC_EPS_DEBUG_H_

#include "eps_soft_error_handling.h"

/** @addtogroup eps_debug
  * @{
  */
/*
 * if this definition is not commended then debug mode of eps board will be on.
 * comment out to ignore. In this mode the debug uart peripheral is initialized and is provided with
 * debug data every timed event period microseconds. The error handling also reports to console the occured
 * error status and the line that it has been collected.
*/
//#define EPS_DEBUG_MODE

EPS_soft_error_status EPS_flush_debug_data_to_uart(void);

EPS_soft_error_status EPS_debug_uart_init(void);


#endif /* SRC_EPS_DEBUG_H_ */
/**
  * @}
  */
