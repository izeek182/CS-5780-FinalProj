/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f0xx_hal.h"

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
#define TriggerA_Pin GPIO_PIN_1
#define TriggerA_GPIO_Port GPIOC
#define TriggerB_Pin GPIO_PIN_2
#define TriggerB_GPIO_Port GPIOC
#define Motor1En_Pin GPIO_PIN_0
#define Motor1En_GPIO_Port GPIOA
#define Motor1A_Pin GPIO_PIN_1
#define Motor1A_GPIO_Port GPIOA
#define Motor1B_Pin GPIO_PIN_2
#define Motor1B_GPIO_Port GPIOA
#define Motor2En_Pin GPIO_PIN_6
#define Motor2En_GPIO_Port GPIOA
#define Motor2A_Pin GPIO_PIN_7
#define Motor2A_GPIO_Port GPIOA
#define Motor2B_Pin GPIO_PIN_4
#define Motor2B_GPIO_Port GPIOC
#define EchoB_Pin GPIO_PIN_14
#define EchoB_GPIO_Port GPIOB
#define EchoA_Pin GPIO_PIN_15
#define EchoA_GPIO_Port GPIOB
#define rLED_Pin GPIO_PIN_6
#define rLED_GPIO_Port GPIOC
#define bLED_Pin GPIO_PIN_7
#define bLED_GPIO_Port GPIOC
#define oLED_Pin GPIO_PIN_8
#define oLED_GPIO_Port GPIOC
#define gLED_Pin GPIO_PIN_9
#define gLED_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
