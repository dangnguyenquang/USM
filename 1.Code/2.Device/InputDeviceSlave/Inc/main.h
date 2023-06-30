/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Input_Sensor_03_Pin GPIO_PIN_14
#define Input_Sensor_03_GPIO_Port GPIOB
#define Input_Sensor_02_Pin GPIO_PIN_15
#define Input_Sensor_02_GPIO_Port GPIOB
#define Input_Sensor_01_Pin GPIO_PIN_8
#define Input_Sensor_01_GPIO_Port GPIOA
#define RS485_Tx_Pin GPIO_PIN_9
#define RS485_Tx_GPIO_Port GPIOA
#define RS485_Rx_Pin GPIO_PIN_10
#define RS485_Rx_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */
#define delayReduceNoise 10 
#define uartBytes 50 
#define crcBytes 2

#define address 0x02
#define function 0x04
#define amongBytes 0x04 
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
