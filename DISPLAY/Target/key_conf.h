/**
  ******************************************************************************
  * File Name          : Target/key_conf.h
  * Description        : This file provides code for the configuration
  *                      of the KEY instances.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEY_CONF_H__
#define __KEY_CONF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx.h"

/* KEY_CENTER Pin mapping */
#define KEY_CENTER_GPIO_PORT     GPIOA
#define KEY_CENTER_GPIO_PIN      GPIO_PIN_0
#define KEY_CENTER_GPIO_LINE     0
#define KEY_CENTER_GPIO_IRQn     EXTI0_IRQn

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* KEY_DOWN Pin mapping */
#define KEY_DOWN_GPIO_PORT       GPIOF
#define KEY_DOWN_GPIO_PIN        GPIO_PIN_8
#define KEY_DOWN_GPIO_LINE       8
#define KEY_DOWN_GPIO_IRQn       EXTI8_IRQn

/* KEY_LEFT Pin mapping */
#define KEY_LEFT_GPIO_PORT       GPIOE
#define KEY_LEFT_GPIO_PIN        GPIO_PIN_3
#define KEY_LEFT_GPIO_LINE       3
#define KEY_LEFT_GPIO_IRQn       EXTI3_IRQn

/* KEY_RIGHT Pin mapping */
#define KEY_RIGHT_GPIO_PORT      GPIOF
#define KEY_RIGHT_GPIO_PIN       GPIO_PIN_7
#define KEY_RIGHT_GPIO_LINE      7
#define KEY_RIGHT_GPIO_IRQn      EXTI7_IRQn

/* KEY_UP Pin mapping */
#define KEY_UP_GPIO_PORT         GPIOF
#define KEY_UP_GPIO_PIN          GPIO_PIN_9
#define KEY_UP_GPIO_LINE         9
#define KEY_UP_GPIO_IRQn         EXTI9_IRQn

/* Exported functions prototypes ---------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif /* __KEY_CONF_H__ */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
