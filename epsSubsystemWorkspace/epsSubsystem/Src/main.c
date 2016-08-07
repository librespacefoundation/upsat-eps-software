/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 *
 * @file    main.c
 * @author  Aris Stathakis
 * @version V1.0
 * @date    27-May-2016
 * @brief   Main body of EPS subsystem module. The MCU system is initialized
 *          from CUBE MX software thus the generated structure is kept as is.
 *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

/* USER CODE BEGIN Includes */
#include "eps_configuration.h"
#include "eps_state.h"
#include "eps_power_module.h"
#include "eps_non_volatile_mem_handling.h"
#include "eps_safety.h"
#include "eps_soft_error_handling.h"
#include "eps_debug.h"
#include "eps_bootsequence.h"
#include "eps_obc_wraps.h"
#include "eps_time.h"
#include "sysview.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

I2C_HandleTypeDef hi2c2;

IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
EPS_State eps_board_state;/*global variable that stores the state info of the eps subsystem at any given point in time.*/
EPS_PowerModule power_module_top, power_module_bottom, power_module_left, power_module_right;/*global variables containing the state of four mppt solar power modules.*/
uint8_t uart_temp[200];/* uart buffer for obc communication.*/
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM6_Init(void);
static void MX_IWDG_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/





/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

	/* USER CODE BEGIN 1 */


	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC_Init();
	MX_TIM3_Init();
	MX_I2C2_Init();
	MX_USART1_UART_Init();
	MX_USART3_UART_Init();
	MX_TIM6_Init();
	MX_IWDG_Init();

	/* USER CODE BEGIN 2 */
	/*this is used only once to arm the satellite - this should by no means stay in the code.*/
	//EPS_set_flash_memory_initial_values();

	/*start watchdog*/
	HAL_IWDG_Start(&hiwdg);

	/*This will be powered up in get_battery_pack_measurement - good to reset tc74 when reseting the whole system*/
	EPS_set_rail_switch(TEMP_SENSOR, EPS_SWITCH_RAIL_OFF, &eps_board_state);

	/*umbilical check */
	error_status = EPS_bootseq_umbilical_check(&eps_board_state);

	/*power up all rails */
	error_status = EPS_bootseq_poweron_all_rails(&eps_board_state);

	/*obc communication initialization*/
	error_status = EPS_obc_communication_init();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	/*load eps limits from memory*/
	EPS_safety_limits eps_limits;
	error_status = EPS_load_safety_limits_from_memory(&eps_limits);

	/* start normal operation mode */

	/*explicitly turn off the battery heater element*/
	EPS_set_control_switch(BATTERY_HEATERS, EPS_SWITCH_CONTROL_OFF, &eps_board_state);

	/* initialize eps module state. */
	error_status = EPS_state_init(&eps_board_state);

	/* initialize all power modules */
	error_status = EPS_PowerModule_init_ALL(&power_module_top, &power_module_bottom, &power_module_left, &power_module_right);

	/*start main loop watchdog*/
	IWDG_change_reset_time(&hiwdg, IWDG_PRESCALER_4, 1900);/*205.409msec*/

	/*kick timer interrupt for timed threads.*/
	error_status = kick_TIM6_timed_interrupt(TIMED_EVENT_PERIOD);

	SEGGER_SYSVIEW_Conf();
	sysview_init();
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		error_status = EPS_SOFT_ERROR_WHILE_LOOP_TOP;
		uint32_t time = HAL_sys_GetTick();
		pkt_pool_IDLE(time);
		queue_IDLE(OBC_APP_ID);

		/*service timed thread every TIMED_EVENT_PERIOD microseconds*/
		if(EPS_event_period_status==TIMED_EVENT_NOT_SERVICED){

			/*mppt update for all power modules*/

			/*top power module */
			error_status = EPS_SOFT_ERROR_MPPT_UPDATE_TOP;
			EPS_update_power_module_state(&power_module_top);
			EPS_PowerModule_mppt_update_pwm(&power_module_top);
			EPS_PowerModule_mppt_apply_pwm(&power_module_top);
			/*bottom power module */
			error_status = EPS_SOFT_ERROR_MPPT_UPDATE_BOTTOM;
			EPS_update_power_module_state(&power_module_bottom);
			EPS_PowerModule_mppt_update_pwm(&power_module_bottom);
			EPS_PowerModule_mppt_apply_pwm(&power_module_bottom);
			/*left power module */
			error_status = EPS_SOFT_ERROR_MPPT_UPDATE_LEFT;
			EPS_update_power_module_state(&power_module_left);
			EPS_PowerModule_mppt_update_pwm(&power_module_left);
			EPS_PowerModule_mppt_apply_pwm(&power_module_left);
			/*right power module */
			error_status = EPS_SOFT_ERROR_MPPT_UPDATE_RIGHT;
			EPS_update_power_module_state(&power_module_right);
			EPS_PowerModule_mppt_update_pwm(&power_module_right);
			EPS_PowerModule_mppt_apply_pwm(&power_module_right);

			error_status = EPS_SOFT_ERROR_OK;

			/*eps state update*/
			error_status = EPS_update_state( &eps_board_state, &hadc, &hi2c2);

			/*check for limit safety issues */
			error_status = EPS_perform_safety_checks(&eps_board_state, &eps_limits);

			/*reset event period status so as to be set into the timer interrupt after TIMED_EVENT_PERIOD msec.*/
			EPS_event_period_status = TIMED_EVENT_SERVICED;
			/* resfresh watchdog*/
			HAL_IWDG_Refresh(&hiwdg);

			error_status = EPS_SOFT_ERROR_TIMED_EVENT_END;

		}

		/* handle OBC packets */
		error_status = EPS_obc_communication_service();


		/*if obc uart tx is not busy, put cpu to sleep WFI */
		HAL_UART_StateTypeDef obc_communication_uart_status = HAL_UART_GetState(&huart3);
		if (!((obc_communication_uart_status == HAL_UART_STATE_BUSY)
				|| (obc_communication_uart_status == HAL_UART_STATE_BUSY_TX)
				|| (obc_communication_uart_status == HAL_UART_STATE_BUSY_TX_RX))) {

			error_status = EPS_SOFT_ERROR_READY_TO_SLEEP;
			/*kill systick and sleep with WaitForInterupt with timer + UART peripherals on*/
			HAL_SuspendTick();
			/* Enter Sleep Mode , wake up is done once Key push button is pressed */
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
			/* Resume Tick interrupt if disabled prior to sleep mode entry*/
			HAL_ResumeTick();
			error_status = EPS_SOFT_ERROR_AWAKE_FROM_SLEEP;
		}



	}
	/* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC init function */
void MX_ADC_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
  hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
  hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.NbrOfConversion = 14;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = ENABLE;
  HAL_ADC_Init(&hadc);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_18;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_19;
  sConfig.Rank = 2;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = 3;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 4;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 5;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 6;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_20;
  sConfig.Rank = 7;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_21;
  sConfig.Rank = 8;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 9;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 10;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 11;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 12;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = 13;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 14;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

}

/* I2C2 init function */
void MX_I2C2_Init(void)
{

  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  HAL_I2C_Init(&hi2c2);

}

/* IWDG init function */
void MX_IWDG_Init(void)
{

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Reload = 4095;/* 4095*(1/37Khz)/Prescaler = time to watchdog = 28.33 sec*/
  HAL_IWDG_Init(&hiwdg);

}

/* TIM3 init function */
void MX_TIM3_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0xa0;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_PWM_Init(&htim3);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0x00;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);

  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2);

  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3);

  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4);

  HAL_TIM_MspPostInit(&htim3);

}

/* TIM6 init function */
void MX_TIM6_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 32;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 50000;
  HAL_TIM_Base_Init(&htim6);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig);

}

/* USART1 init function */
void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);

}

/* USART3 init function */
void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart3);

}

/** 
  * Enable DMA controller clock
  */
void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_SU_SWITCH_Pin|GPIO_OBC_SWITCH_Pin|GPIO_ADCS_SWITCH_Pin|GPIO_DEPLOY_TOP_Pin 
                          |GPIO_DEPLOY_ANT1_Pin|GPIO_DEPLOY_RIGHT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, GPIO_COMM_SWITCH_Pin|GPIO_DEPLOY_LEFT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIO_TC74_POWER_GPIO_Port, GPIO_TC74_POWER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_HEATERS_Pin|GPIO_DEPLOY_BOTTOM_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : GPIO_SU_SWITCH_Pin GPIO_OBC_SWITCH_Pin GPIO_ADCS_SWITCH_Pin GPIO_DEPLOY_TOP_Pin 
                           GPIO_DEPLOY_ANT1_Pin GPIO_DEPLOY_RIGHT_Pin */
  GPIO_InitStruct.Pin = GPIO_SU_SWITCH_Pin|GPIO_OBC_SWITCH_Pin|GPIO_ADCS_SWITCH_Pin|GPIO_DEPLOY_TOP_Pin 
                          |GPIO_DEPLOY_ANT1_Pin|GPIO_DEPLOY_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_COMM_SWITCH_Pin GPIO_DEPLOY_LEFT_Pin */
  GPIO_InitStruct.Pin = GPIO_COMM_SWITCH_Pin|GPIO_DEPLOY_LEFT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pin : GPIO_UMBILICAL_Pin */
  GPIO_InitStruct.Pin = GPIO_UMBILICAL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIO_UMBILICAL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : GPIO_TC74_POWER_Pin */
  GPIO_InitStruct.Pin = GPIO_TC74_POWER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIO_TC74_POWER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : GPIO_HEATERS_Pin */
  GPIO_InitStruct.Pin = GPIO_HEATERS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIO_HEATERS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : GPIO_DEPLOY_BOTTOM_Pin */
  GPIO_InitStruct.Pin = GPIO_DEPLOY_BOTTOM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIO_DEPLOY_BOTTOM_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/**
   * @brief ADC DMA interrupt handler
   * marks the global flag of adc peripheral as a transfer complete.
   * @param file: pointer to adc handle peripheral
   * @retval None
   */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc) {

	adc_reading_complete = ADC_TRANSFER_COMPLETED;
}

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
