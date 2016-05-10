/*
 * eps_soft_error_handling.c
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */
#include "eps_soft_error_handling.h"


void EPS_soft_error_handling(EPS_soft_error_status eps_error_status){


	switch (eps_error_status) {

	case EPS_SOFT_ERROR_OK:
		//update watchdog
		break;

	case EPS_SOFT_ERROR_UNRESOLVED:
		//report and soft reset
		break;



// 	   default :
// 		   //YOU SHOULDNT BE HERE!
// 		   //ERROR HANDLING
	}

}
