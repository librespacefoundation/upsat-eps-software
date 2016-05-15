/*
 * eps_debug.h
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */

#ifndef SRC_EPS_DEBUG_H_
#define SRC_EPS_DEBUG_H_


//if this definition is not comended then debug mode of eps board will be on.
//comment out to ignore. In this mode the debug uart perihera is initialized and is provided with
//debug data every timed event period microseconds. The error handling also reports to console the occured
//error status and the line that it has been collected.


#define EPS_DEBUG_MODE

void flush_debug_data_to_uart(void);



#endif /* SRC_EPS_DEBUG_H_ */
