/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM16_Init(void);
void clearRx(void);
void modbusTransHandle(void);
/* USER CODE BEGIN PFP */
char checkLampStatus(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define MODBUSADD 0x01
#define ADCthreshold  2000
#define BlinkTimerthreshold  500
#define SensorType	0x01

uint16_t debug = 0;
char isBlink = 0;
char isLight = 0;
uint16_t counter = 0;
uint8_t rx_flag = 0;

uint8_t lampStatus = 0;
uint8_t whichLight = 0;
uint16_t Blink_RedAccumulateTime = 0;
uint16_t Blink_YellowAccumulateTime = 0;
uint16_t Blink_GreenAccumulateTime = 0;

#define maxRxSize 20
uint8_t rx_dataChar;
uint8_t rx_data[maxRxSize];
uint8_t rx_count = 0;

uint8_t tx_data[15];

uint8_t CRC_MODBUS(uint8_t *input, uint8_t size);
void Calc_CRC16(uint8_t *input, uint16_t size, uint8_t *output);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start(&hadc1);
  HAL_TIM_Base_Start(&htim16);


  HAL_UART_Receive_IT(&huart1, &rx_dataChar, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	lampStatus = checkLampStatus();
	uint16_t currentCounter = __HAL_TIM_GetCounter(&htim16);
	if(lampStatus)
	{
		if(isLight == 0)
		{
			isLight = lampStatus;
			counter = __HAL_TIM_GetCounter(&htim16);
		}
		if(currentCounter - counter > BlinkTimerthreshold)
		{
			isBlink = 0;
		}
		whichLight = lampStatus;
	}
	else if(lampStatus == 0)
	{
		if(currentCounter - counter < BlinkTimerthreshold)
		{
			isBlink = 1;
			if(isLight == 1) Blink_RedAccumulateTime++;
			else if(isLight == 2) Blink_YellowAccumulateTime++;
			else if(isLight == 3) Blink_GreenAccumulateTime++;
		}
		isLight = 0;
	}
	if(rx_flag)
	{
		if(CRC_MODBUS(rx_data, 6) && rx_data[0] == MODBUSADD) modbusTransHandle();
		clearRx();
		rx_flag = 0;
	}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 64000;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
char checkLampStatus(void)
{
  ADC_ChannelConfTypeDef sConfigPrivate = {0};
  uint16_t DCValue[3];

  sConfigPrivate.Channel = ADC_CHANNEL_3;
  HAL_ADC_ConfigChannel(&hadc1, &sConfigPrivate);
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1,1000);
  DCValue[0] = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);

  sConfigPrivate.Channel = ADC_CHANNEL_4;
  HAL_ADC_ConfigChannel(&hadc1, &sConfigPrivate);
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1,1000);
  DCValue[1] = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);

  sConfigPrivate.Channel = ADC_CHANNEL_5;
  HAL_ADC_ConfigChannel(&hadc1, &sConfigPrivate);
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1,1000);
  DCValue[2] = HAL_ADC_GetValue(&hadc1);
  HAL_ADC_Stop(&hadc1);

  //check which is highest value
  uint16_t max = 0;
  char maxPos = 0;
  for(char i = 0; i < 3; i++)
  {
    if(DCValue[i] > max)
    {
      max = DCValue[i];
      maxPos = i + 1;
    }
  }
  //check if that is no light (all off)
  if(max < ADCthreshold) // fix here
  {
    maxPos = 0;
  }
  return maxPos;

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if(huart->Instance == USART1){
        HAL_UART_Receive_IT(&huart1, &rx_dataChar, 1);
        rx_data[rx_count] = rx_dataChar;
        rx_count++;
        if(rx_count >= 6) rx_flag = 1;
    }
}
void clearRx (void)
{
	for(uint8_t i = 0; i < maxRxSize; i++)
	{
		rx_data[i] = '\0';
	}
	rx_count = 0;
}
uint8_t CRC_MODBUS(uint8_t *input, uint8_t size)
{
  uint16_t check_sum[2];
  uint16_t reg_crc = 0xFFFF;

	for(uint8_t i = 0; i < size; i++) {
		reg_crc ^= input[i];

		for(uint8_t j = 0; j < 8; j++) {
			if((reg_crc & 0x01) == 1) reg_crc = (reg_crc >> 1) ^ 0xA001;
			else reg_crc = reg_crc >> 1;
		}

		check_sum[1] = (reg_crc >> 8) &0xFF;
		check_sum[0] = reg_crc & 0xFF;

		if(input[i + 1] == check_sum[0] && input[i + 2] == check_sum[1]) return i + 3;
	}
	return 0;
}
void Calc_CRC16(uint8_t *input, uint16_t size, uint8_t *output)
{
  uint16_t reg_crc = 0xFFFF;

  for(uint16_t i = 0; i < size; i++)
  {
    reg_crc ^= input[i];
    for(uint8_t j = 0; j < 8; j++)
    {
      if((reg_crc & 0x01) == 1) reg_crc = (reg_crc >> 1) ^ 0xA001;
      else reg_crc = reg_crc >> 1;
    }
  }

  output[1] = (reg_crc >> 8) &0xFF;
  output[0] = reg_crc & 0xFF;
}

void modbusTransHandle(void){
	uint8_t CRCbuff[2];

	tx_data[0] = MODBUSADD;
	tx_data[1] = 0x04;
	tx_data[2] = 0x0A;
	tx_data[3] = SensorType;
	tx_data[4] = isBlink;
	tx_data[5] = lampStatus;
	tx_data[6] = whichLight;
	tx_data[7] = Blink_RedAccumulateTime >> 8;
	tx_data[8] = Blink_RedAccumulateTime;
	tx_data[9] = Blink_YellowAccumulateTime >> 8;
	tx_data[10] = Blink_YellowAccumulateTime;
	tx_data[11] = Blink_GreenAccumulateTime >> 8;
	tx_data[12] = Blink_GreenAccumulateTime;

	Calc_CRC16(tx_data, 13, CRCbuff);
	tx_data[13] = CRCbuff[0];
	tx_data[14] = CRCbuff[1];

	HAL_UART_Transmit(&huart1,&tx_data,sizeof(tx_data), 100);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
