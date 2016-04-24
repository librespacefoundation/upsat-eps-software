/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

/* USER CODE BEGIN Includes */
#define TIMED_EVENT_PERIOD ((uint8_t)50)//in miliseconds
#define MPPT_STEP_SIZE ((uint32_t)1)
#define STARTUP_PWM_DUTYCYCLE ((uint32_t)0X50)

typedef struct {
	uint8_t su_p_switch;/*Science unit control switch - set to turn off - reset to turn on (!inverted logic!)*/
	uint8_t obc_p_switch;
	uint8_t adcs_p_switch;
	uint8_t comm_p_switch;
	uint8_t i2c_tc74_p_switch;
	/**/
	uint8_t deploy_left_switch;
	uint8_t deploy_right_switch;
	uint8_t deploy_bottom_switch;
	uint8_t deploy_ant1_switch;
	uint8_t umbilical_switch;
	uint8_t heaters_switch;
    /**/
	uint16_t module_left_voltage_avg;
	uint16_t module_left_current_avg;
	uint32_t module_left_power_avg;
    /**/
	uint16_t module_right_voltage_avg;
	uint16_t module_right_current_avg;
	uint32_t module_right_power_avg;
	    /**/
	uint16_t module_top_voltage_avg;
	uint16_t module_top_current_avg;
	uint32_t module_top_power_avg;
	    /**/
	uint16_t module_bottom_voltage_avg;
	uint16_t module_bottom_current_avg;
	uint32_t module_bottom_power_avg;
	/**/
	uint16_t v5_current_avg;
	uint16_t v3_3_current_avg;
	uint16_t battery_voltage;
	uint16_t battery_current_plus;
	uint16_t battery_current_minus;
	int8_t 	 battery_temp;
	uint32_t cpu_temperature;
}EPS_State;


typedef struct {
	uint16_t voltage; /*average voltage at each mppt step*/
	uint16_t current; /*average curret at each mppt step*/
	uint32_t previous_power; /*average power at previous mppt step*/
	uint8_t incremennt_flag;/*flag for mppt algorithm must be initialized to 1*/
	uint32_t pwm_duty_cycle; /*duty cycle of power module pwm output*/
	//uint8_t  module_id;/*module id index staring from zero.*/
	TIM_HandleTypeDef *htim_pwm;/*assign wich timer is assigned for this pwm output*/
	uint32_t timChannel;/*assign the proper timer channel assigned to module pwm output*/

}EPS_PowerModule;


/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

I2C_HandleTypeDef hi2c2;

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

volatile uint8_t EPS_soft_error_status = 0x00;//initialize global software error status to false.
volatile uint8_t EPS_event_period_status = 0xFF;//initialize global timed event flag to true.
volatile uint8_t adc_reading_complete = 0;//flag to check when dma transfer is complete.

EPS_State eps_board_state;
EPS_PowerModule power_module_top, power_module_bottom, power_module_left, power_module_right;

uint32_t adc_measurement_dma_power_modules[137]= { 0 };//20*8 +1
uint32_t adc_measurement_dma_eps_state[13]= { 0 };//2*6 +1

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void EPS_PowerModule_init(EPS_PowerModule *module_X,uint32_t starting_pwm_dutycycle,TIM_HandleTypeDef *htim, uint32_t timer_channel);
void EPS_update_power_modules_state(EPS_PowerModule *module_top, EPS_PowerModule *module_bottom, EPS_PowerModule *module_left, EPS_PowerModule *module_right);
void EPS_PowerModule_mppt_update_pwm(EPS_PowerModule *moduleX);
void EPS_PowerModule_mppt_apply_pwm(EPS_PowerModule *moduleX);
void EPS_update_state(volatile EPS_State *state);
void EPS_state_init(volatile EPS_State *state);

void ADC_EPS_POWER_MODULES_Init(void);
void ADC_EPS_STATE_Init(void);




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

  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);//top
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
  //initial checks: chech if deploymet has happened and handle it.

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //init eps module (eps state initial value)
  EPS_state_init(&eps_board_state);

  //init power modules 1,2,3,4 and mppt
  EPS_PowerModule_init(&power_module_top, STARTUP_PWM_DUTYCYCLE, &htim3, 1);
  EPS_PowerModule_init(&power_module_bottom, STARTUP_PWM_DUTYCYCLE, &htim3, 2);
  EPS_PowerModule_init(&power_module_left, STARTUP_PWM_DUTYCYCLE, &htim3, 3);
  EPS_PowerModule_init(&power_module_right, STARTUP_PWM_DUTYCYCLE, &htim3, 4);

  //kick timer interupt for timed threads.

  HAL_ADC_Stop(&hadc);

  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

	  //set error status
	  EPS_soft_error_status = 0xff;

	  //check timed thread ( eps_state update + mppt_pwm_update for each power module)
	  EPS_event_period_status = 0xff;//for now set it here for debug - it will be normally set by timer interrupt every TIMED_EVENT_PERIOD msec
	  if(EPS_event_period_status){

		  //update eps state
		  EPS_update_state( &eps_board_state);

		  //update power modules state
		  EPS_update_power_modules_state(&power_module_top, &power_module_bottom, &power_module_left, &power_module_right);
		  //mppt_pwm_update
//		  EPS_PowerModule_mppt_update_pwm(&power_module_top);
//		  EPS_PowerModule_mppt_apply_pwm(&power_module_top);//bottom
//
//		  //htim3.Instance->CCR1 = power_module_top.pwm_duty_cycle;
//
//		  EPS_PowerModule_mppt_update_pwm(&power_module_bottom);
//		  EPS_PowerModule_mppt_apply_pwm(&power_module_bottom);//left
//
//		  EPS_PowerModule_mppt_update_pwm(&power_module_left);
//		  EPS_PowerModule_mppt_apply_pwm(&power_module_left);//top

		  EPS_PowerModule_mppt_update_pwm(&power_module_right);
		  EPS_PowerModule_mppt_apply_pwm(&power_module_right);//right

		  EPS_event_period_status = 0x00;//reset event period status so as to be set into the timer interupt.
	  }


	  //if obc communication flag (set by uart interupt?dma?whateva) service obc uart ting


	  //reset errror_status
	  EPS_soft_error_status = 0x00;


	  //kill systick and sleep with WaitForInterupt with timer + UART peripherals on
	  HAL_Delay(TIMED_EVENT_PERIOD);//pros to paron me delay...



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

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
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
  sConfigOC.Pulse = 0x50;
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
  htim6.Init.Prescaler = 640;
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
  huart3.Init.BaudRate = 115200;
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
  HAL_GPIO_WritePin(GPIOC, GPIO_SU_SWITCH_Pin|GPIO_OBC_SWITCH_Pin|GPIO_ADCS_SWITCH_Pin|GPIO_UMBILICAL_Pin 
                          |GPIO_DEPLOY_ANT1_Pin|GPIO_DEPLOY_RIGHT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, GPIO_COMM_SWITCH_Pin|GPIO_DEPLOY_LEFT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIO_TC74_POWER_GPIO_Port, GPIO_TC74_POWER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_HEATERS_Pin|GPIO_DEPLOY_BOTTOM_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : GPIO_SU_SWITCH_Pin GPIO_OBC_SWITCH_Pin GPIO_ADCS_SWITCH_Pin GPIO_UMBILICAL_Pin 
                           GPIO_DEPLOY_ANT1_Pin GPIO_DEPLOY_RIGHT_Pin */
  GPIO_InitStruct.Pin = GPIO_SU_SWITCH_Pin|GPIO_OBC_SWITCH_Pin|GPIO_ADCS_SWITCH_Pin|GPIO_UMBILICAL_Pin 
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

  /*Configure GPIO pin : GPIO_TC74_POWER_Pin */
  GPIO_InitStruct.Pin = GPIO_TC74_POWER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIO_TC74_POWER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_HEATERS_Pin GPIO_DEPLOY_BOTTOM_Pin */
  GPIO_InitStruct.Pin = GPIO_HEATERS_Pin|GPIO_DEPLOY_BOTTOM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void EPS_PowerModule_init(EPS_PowerModule *module_X, uint32_t starting_pwm_dutycycle, TIM_HandleTypeDef *htim, uint32_t timer_channel){

	module_X->current =0;
	module_X->voltage =0;
	module_X->previous_power =0;
	module_X->pwm_duty_cycle = starting_pwm_dutycycle;
	//module_X->module_id = module_id;
	module_X->htim_pwm = htim;
	module_X->timChannel = timer_channel;
	module_X->incremennt_flag = 1;
}

void EPS_PowerModule_mppt_update_pwm(EPS_PowerModule *moduleX){

	//static uint8_t increment_flag = 1;//diplotriplotsekare oti to inint to kanei mono sthn arxh vlaks


	  /*power calculation*/

	  volatile uint32_t power_now_avg = moduleX->voltage * moduleX->current;
	  uint32_t duty_cycle = moduleX->pwm_duty_cycle;

	// decide duty cycle orientation - set increment flag.
	  if (power_now_avg  <(moduleX->previous_power)){

		  if (moduleX->incremennt_flag>0){
			  moduleX->incremennt_flag = 0;
		  }
		  else{
			  moduleX->incremennt_flag = 1;
		  }
	  }
    //add appropriate offset - create new duty cycle.


	  if(moduleX->incremennt_flag){
		  duty_cycle = duty_cycle+MPPT_STEP_SIZE;
	  }
	  else{
		  duty_cycle = duty_cycle-MPPT_STEP_SIZE;
	  }
 	  //Check for Overflow and Underflow
	  if (duty_cycle>350){duty_cycle=0;}
	  if (duty_cycle>320){duty_cycle=310;}
    // Set new PWM compare register
	  //duty_cycle =0;


	  moduleX->previous_power = power_now_avg;
	  moduleX->pwm_duty_cycle = duty_cycle;

}

void EPS_PowerModule_mppt_apply_pwm(EPS_PowerModule *moduleX){

	switch ( moduleX->timChannel ) {
	case 1:         /*  top module */
		moduleX->htim_pwm->Instance->CCR1 = moduleX->pwm_duty_cycle;
		break;
	case 2:         /*  bottom module */
		moduleX->htim_pwm->Instance->CCR2 = moduleX->pwm_duty_cycle;
		break;
	case 3:         /*  left module */
		moduleX->htim_pwm->Instance->CCR3 = moduleX->pwm_duty_cycle;
		break;
	case 4:         /*  right module */
		moduleX->htim_pwm->Instance->CCR4 = moduleX->pwm_duty_cycle;
		break;
	default:
		//error handling?
		break;
	}

}

void EPS_update_state(volatile EPS_State *state){

	/*adc ting*/
	ADC_EPS_STATE_Init();

	adc_reading_complete = 0;
	HAL_ADC_Start_DMA(&hadc, adc_measurement_dma_eps_state, 12);
	/*Wait till DMA ADC sequence transfer is ready*/
	while(adc_reading_complete==0){
 	}

//	HAL_ADC_Stop_DMA(&hadc);

	state->battery_voltage = adc_measurement_dma_eps_state[6];
	state->battery_current_plus = adc_measurement_dma_eps_state[7];
	state->battery_current_minus = adc_measurement_dma_eps_state[8];
	state->v3_3_current_avg = adc_measurement_dma_eps_state[9];
	state->v5_current_avg = adc_measurement_dma_eps_state[10];
	state->cpu_temperature = adc_measurement_dma_eps_state[11];

	/*i2c temp sensors*/
	//	  state->battery_temp;


}



void EPS_update_power_modules_state(EPS_PowerModule *module_top, EPS_PowerModule *module_bottom, EPS_PowerModule *module_left, EPS_PowerModule *module_right){

	/*adc ting*/
	ADC_EPS_POWER_MODULES_Init();
	adc_reading_complete = 0;
	HAL_ADC_Start_DMA(&hadc, adc_measurement_dma_power_modules, 136);

	/*Process Measurements*/
	uint32_t voltage_avg_top =0;
	uint32_t current_avg_top =0;
	uint32_t voltage_avg_bottom =0;
	uint32_t current_avg_bottom =0;
	uint32_t voltage_avg_left =0;
	uint32_t current_avg_left =0;
	uint32_t voltage_avg_right =0;
	uint32_t current_avg_right =0;

	/*Wait till DMA ADC sequence transfer is ready*/
	while(adc_reading_complete==0){
		//wait for dma transfer complete.
	}
//	HAL_ADC_Stop_DMA(&hadc);//stop transfer and turn off adc peripheral.

	//de-interleave and sum voltage and current measurements.
	for (int sum_index = 8; sum_index < 136; sum_index+=8) {
		/*top*/
		voltage_avg_top = voltage_avg_top + adc_measurement_dma_power_modules[sum_index];
		current_avg_top = current_avg_top + adc_measurement_dma_power_modules[sum_index+1];
		/*bottom*/
		voltage_avg_bottom = voltage_avg_bottom + adc_measurement_dma_power_modules[sum_index+2];
		current_avg_bottom = current_avg_bottom + adc_measurement_dma_power_modules[sum_index+3];
		/*left*/
		voltage_avg_left = voltage_avg_left + adc_measurement_dma_power_modules[sum_index+4];
		current_avg_left = current_avg_left + adc_measurement_dma_power_modules[sum_index+5];
		/*right*/
		voltage_avg_right = voltage_avg_right + adc_measurement_dma_power_modules[sum_index+6];
		current_avg_right = current_avg_right + adc_measurement_dma_power_modules[sum_index+7];
	}

	/*filter ting*/
	//average of 16 concecutive adc measurements.skip the first to avoid adc power up distortion.
	module_top->voltage = voltage_avg_top>>4;
	module_top->current = current_avg_top>>4;
	module_bottom->voltage = voltage_avg_bottom>>4;
	module_bottom->current = current_avg_bottom>>4;
	module_left->voltage = voltage_avg_left>>4;
	module_left->current = current_avg_left>>4;
	module_right->voltage = voltage_avg_right>>4;
	module_right->current = current_avg_right>>4;

}




void EPS_state_init(volatile EPS_State *state){

	state->su_p_switch = 0xff;
	state->adcs_p_switch = 0xff;
	state->comm_p_switch = 0xff;
	state->obc_p_switch = 0xff;
	state->i2c_tc74_p_switch = 0xff;

	state->deploy_left_switch = 0x00;
	state->deploy_right_switch = 0x00;
	state->deploy_bottom_switch = 0x00;
	state->deploy_ant1_switch = 0x00;
	state->umbilical_switch = 0x00;
	state->heaters_switch = 0x00;
	state->module_left_voltage_avg = 0x00;
	state->module_left_current_avg = 0x00;
	state->module_left_power_avg = 0x00;
	state->module_right_voltage_avg = 0x00;
	state->module_right_current_avg = 0x00;
	state->module_right_power_avg = 0x00;
	state->module_top_voltage_avg = 0x00;
	state->module_top_current_avg = 0x00;
	state->module_top_power_avg = 0x00;
	state->module_bottom_voltage_avg = 0x00;
	state->module_bottom_current_avg = 0x00;
	state->module_bottom_power_avg = 0x00;
	state->v5_current_avg = 0x00;
	state->v3_3_current_avg = 0x00;
	state->battery_voltage = 0x00;
	state->battery_current_plus = 0x00;
	state->battery_current_minus = 0x00;
	state->battery_temp = 0x00;

}



/* ADC init function for eps state only conversion */
void ADC_EPS_STATE_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
//  hadc.Instance = ADC1;
//  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
//  hadc.Init.Resolution = ADC_RESOLUTION_12B;
//  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//  hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
//  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
//  hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
//  hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
//  hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
//  hadc.Init.ContinuousConvMode = ENABLE;
//  hadc.Init.DiscontinuousConvMode = DISABLE;
//  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
//  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
//  hadc.Init.DMAContinuousRequests = ENABLE;
  hadc.Init.NbrOfConversion = 6;
  HAL_ADC_Init(&hadc);

  /*Vbat*/
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 9;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

   /*Ibat+*/
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 10;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  /*Ibat-*/
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 11;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  /*I3v3*/
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 12;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  /*I5v*/
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = 13;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  /*cpu internal temp sensor*/
  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 14;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

}


/* ADC init function to read eps state */
void ADC_EPS_POWER_MODULES_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
//  hadc.Instance = ADC1;
//  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
//  hadc.Init.Resolution = ADC_RESOLUTION_12B;
//  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//  hadc.Init.ScanConvMode = ADC_SCAN_ENABLE;
//  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
//  hadc.Init.LowPowerAutoWait = ADC_AUTOWAIT_DISABLE;
//  hadc.Init.LowPowerAutoPowerOff = ADC_AUTOPOWEROFF_DISABLE;
//  hadc.Init.ChannelsBank = ADC_CHANNELS_BANK_A;
//  hadc.Init.ContinuousConvMode = ENABLE;
//  hadc.Init.DiscontinuousConvMode = DISABLE;
//  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
//  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
//  hadc.Init.DMAContinuousRequests = ENABLE;
  hadc.Init.NbrOfConversion = 8;
  HAL_ADC_Init(&hadc);


  /*module top current*/
  sConfig.Channel = ADC_CHANNEL_18;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  /*module top voltage*/
  sConfig.Channel = ADC_CHANNEL_19;
  sConfig.Rank = 2;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  /*module bottom current*/
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = 3;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  /*module bottom voltage*/
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 4;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  /*module left current*/
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 5;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  /*module left voltage*/
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 6;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  /*module right current*/
  sConfig.Channel = ADC_CHANNEL_20;
  sConfig.Rank = 7;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

  /*module right voltage*/
  sConfig.Channel = ADC_CHANNEL_21;
  sConfig.Rank = 8;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);



}

// ADC DMA interrupt handler
void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef * hadc){
    //printf("ADC conversion done.\n");
    //HAL_ADC_Stop_DMA(hadc);

	HAL_ADC_Stop_DMA(hadc);
	HAL_ADC_Stop(hadc);
    adc_reading_complete = 1;
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
