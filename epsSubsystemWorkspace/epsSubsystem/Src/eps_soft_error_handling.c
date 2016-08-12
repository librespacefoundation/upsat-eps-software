/*
 * eps_soft_error_handling.c
 *
 *  Created on: May 10, 2016
 *      Author: ariolis
 */
#include "stm32l1xx_hal.h"
#include "eps_soft_error_handling.h"


extern IWDG_HandleTypeDef hiwdg;

volatile EPS_soft_error_status error_status = EPS_SOFT_ERROR_UNRESOLVED;/* initialize global software error status to OK.*/

/** @addtogroup softerror
  * @{
  */

/**
  * @brief Actions based on soft error status state machine.
  *        If an error has occured the watchdog is not updated so a reset is to come.
  * @param  eps_error_status: the eps state structure containing central info of the EPS subsystem.
  * @retval None.
  */
void EPS_soft_error_handling(EPS_soft_error_status eps_error_status){


	switch (eps_error_status) {

	case EPS_SOFT_ERROR_OK:
		/*Refresh watchdog. */
		//HAL_IWDG_Refresh(&hiwdg);
		break;
	case EPS_SOFT_ERROR_UNRESOLVED:
		//report and soft reset
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
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_POWERON_ALL_RAILS:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_STATE_INIT:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_STATE_UPDATE:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_KICK_TIMER:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_POWER_MODULE_INIT_ALL:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_POWER_MODULE_INIT_TOP:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_POWER_MODULE_INIT_BOTTOM:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_POWER_MODULE_INIT_LEFT:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_POWER_MODULE_INIT_RIGHT:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_MPPT_UPDATE_TOP:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_MPPT_UPDATE_BOTTOM:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_MPPT_UPDATE_LEFT:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_MPPT_UPDATE_RIGHT:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_OBC_COMM_SERVICE:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_OBC_COMM_SERVICE_COMPLETE:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_OBC_COMM_SERVICE_CHECK_TIMEOUTS:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_OBC_COMM_SERVICE_EXPORT_PKT:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_SAFETY_CHECK:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_SAFETY_CHECK_BATTERY_VOLTAGE_UNPREDICTED:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_SAFETY_CHECK_BATTERY_TEMPERATURE_UNPREDICTED:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_LEFT:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_RIGHT:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_TOP:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_BOTTOM:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_ANT1:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_A:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_B:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_C:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_D:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_E:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_F:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_BOOTSEQ_DEPLOYMENT_KEY_G:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_UPDATE_STATE_ADC:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_UPDATE_STATE_ADC_DMA:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_UPDATE_STATE_ADC_FILTER:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_UPDATE_STATE_ADC_CPU_TEMP:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_STATE_UPDATE_BATTERYPACK_TEMP:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_WHILE_LOOP_TOP:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_TIMED_EVENT_END:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_READY_TO_SLEEP:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_AWAKE_FROM_SLEEP:
		//report and soft reset
		break;
	case EPS_SOFT_ERROR_LAST_VALUE:
		//report and soft reset
		break;

		// 	   default :
		// 		   //YOU SHOULDNT BE HERE!
		// 		   //ERROR HANDLING
	}

//	//TODO: if not a valid status maybe you should somehow lock until the rest or else you go back in action not?
//	        //at least write to flash the status if not a valid status i.e:
//	if(eps_error_status~=EPS_SOFT_ERROR_OK) {
//
//		EPS_set_memory_word(SOFT_ERROR_STATUS_MEM_ADDRESS, (uint32_t)eps_error_status);
//		while(1){
//			/*brake here until a reset from watchdog occurs*/
//		}
//
//	}

}
/**
  * @}
  */
