/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32u5xx_hal.h"

#include "lcd_io.h"
#include "mem_io.h"
#include "key_io.h"
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
#define KEY_LEFT_Pin GPIO_PIN_3
#define KEY_LEFT_GPIO_Port GPIOE
#define BUTTON_USER_Pin GPIO_PIN_13
#define BUTTON_USER_GPIO_Port GPIOC
#define KEY_RIGHT_Pin GPIO_PIN_7
#define KEY_RIGHT_GPIO_Port GPIOF
#define KEY_DOWN_Pin GPIO_PIN_8
#define KEY_DOWN_GPIO_Port GPIOF
#define KEY_UP_Pin GPIO_PIN_9
#define KEY_UP_GPIO_Port GPIOF
#define KEY_CENTER_Pin GPIO_PIN_0
#define KEY_CENTER_GPIO_Port GPIOA
#define LCD_RESET_Pin GPIO_PIN_11
#define LCD_RESET_GPIO_Port GPIOE
#define LCD_TE_Pin GPIO_PIN_13
#define LCD_TE_GPIO_Port GPIOE
#define LCD_TE_EXTI_IRQn EXTI13_IRQn
#define LED_RED_Pin GPIO_PIN_2
#define LED_RED_GPIO_Port GPIOG
#define LED_GREEN_Pin GPIO_PIN_7
#define LED_GREEN_GPIO_Port GPIOC
#define LED_BLUE_Pin GPIO_PIN_7
#define LED_BLUE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
