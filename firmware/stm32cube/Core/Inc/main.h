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
#include "stm32g4xx_hal.h"

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
#define NRST_Pin GPIO_PIN_10
#define NRST_GPIO_Port GPIOG
#define LED_485_Pin GPIO_PIN_1
#define LED_485_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_2
#define LED_GPIO_Port GPIOB
#define BOOT_BTN_Pin GPIO_PIN_12
#define BOOT_BTN_GPIO_Port GPIOB
#define LED_RX_Pin GPIO_PIN_10
#define LED_RX_GPIO_Port GPIOC
#define LED_TX_Pin GPIO_PIN_11
#define LED_TX_GPIO_Port GPIOC
#define BTN_S1_Pin GPIO_PIN_3
#define BTN_S1_GPIO_Port GPIOB
#define BTN_S2_Pin GPIO_PIN_4
#define BTN_S2_GPIO_Port GPIOB
#define LED_CAN_Pin GPIO_PIN_5
#define LED_CAN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
