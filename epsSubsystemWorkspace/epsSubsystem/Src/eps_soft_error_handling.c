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
		/*kick watchdog*/
		break;

	case EPS_SOFT_ERROR_UMBILICAL_UNPREDICTED:
		//report and soft reset
		break;

	case EPS_SOFT_ERROR_BOOTSEQ_POWEROFF_ALL_RAILS:
		//report and soft reset
		break;

	case EPS_SOFT_ERROR_BOOTSEQ_UMBILICAL_CHECK:
		//report and soft reset
		break;

	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_STAGE:
		//report and soft reset
		break;

	case EPS_SOFT_ERROR_OBC_COMM_INIT:
		//report and soft reset
		break;

	case EPS_SOFT_ERROR_DEBUG_UART_INIT:
		//report and soft reset
		break;

	case EPS_SOFT_ERROR_DEBUG_UART_FLUSH:
		//report and soft reset
		break;

	case EPS_SOFT_ERROR_LOAD_SAFETY_LIMITS:
		//
		break;

	case EPS_SOFT_ERROR_BOOTSEQ_POWERON_ALL_RAILS:
		//
		break;

	case EPS_SOFT_ERROR_STATE_INIT:
		//
		break;

	case EPS_SOFT_ERROR_STATE_UPDATE:
		//
		break;

	case EPS_SOFT_ERROR_KICK_TIMER:
		//
		break;

	case EPS_SOFT_ERROR_POWER_MODULE_INIT_ALL:
		//
		break;

	case EPS_SOFT_ERROR_POWER_MODULE_INIT_TOP:
		//
		break;

	case EPS_SOFT_ERROR_POWER_MODULE_INIT_BOTTOM:
		//
		break;

	case EPS_SOFT_ERROR_POWER_MODULE_INIT_LEFT:
		//
		break;

	case EPS_SOFT_ERROR_POWER_MODULE_INIT_RIGHT:
		//
		break;

	case EPS_SOFT_ERROR_MPPT_UPDATE_TOP:
		//
		break;

	case EPS_SOFT_ERROR_MPPT_UPDATE_BOTTOM:
		//
		break;

	case EPS_SOFT_ERROR_MPPT_UPDATE_LEFT:
		//
		break;

	case EPS_SOFT_ERROR_MPPT_UPDATE_RIGHT:
		//
		break;

	case EPS_SOFT_ERROR_OBC_COMM_SERVICE:
		//
		break;

	case EPS_SOFT_ERROR_SAFETY_CHECK:
		//
		break;

	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_LEFT:
		//
		break;

	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_RIGHT:
		//
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_TOP:
		//
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_BOTTOM:
		//
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_A:
		//
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_B:
		//
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_C:
		//
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_D:
		//
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_E:
		//
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_F:
		//
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_G:
		//
		break;






	case EPS_SOFT_ERROR_UNRESOLVED:
		//report and soft reset
		break;

	case EPS_SOFT_ERROR_LAST_VALUE:
		//report and soft reset
		break;

// 	   default :
// 		   //YOU SHOULDNT BE HERE!
// 		   //ERROR HANDLING
	}

}
