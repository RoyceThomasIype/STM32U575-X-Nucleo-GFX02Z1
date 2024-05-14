/**
  ******************************************************************************
  * File Name          : Target/lcd_conf.h
  * Description        : This file provides code for the configuration
  *                      of the LCD instances.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_CONF_H__
#define __LCD_CONF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* Tearing Effect EXTI handler */
extern EXTI_HandleTypeDef                   hexti_lcd_te;

/* DMA Instance handlers */
extern  DMA_HandleTypeDef                   handle_GPDMA1_Channel13;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* Number of LCD instances */
#define LCD_INSTANCES_NBR                   1U

/**
  * @brief LCD constroller REG and RAM addresses
  * @note STM32 FMC Interface
  *         - Base Address           : 0x60000000
  *         - Bank Size              : 0x04000000
  *         - Bank ID                : Bank 1
  *         - Bank Memory Offset     : 0x00100000
  *         - Bank Register Address  : 0x60000000
  *         - Bank Memory Address    : (0x60000000 | 0x0100000)
  *         - Data Size              : 8bits
  *         - DMA                    : GPDMA1_Channel13
  */
#define FMC_BANK1_REG                       ((uint16_t *) 0x60000000)
#define FMC_BANK1_MEM                       ((uint16_t *) (0x60000000 | 0x0100000))

/* DMA Instance handlers */
#define hLCDDMA                             handle_GPDMA1_Channel13

/* RESET Pin mapping */
#define LCD_RESET_GPIO_PORT                 GPIOE
#define LCD_RESET_GPIO_PIN                  GPIO_PIN_11

/* TE Pin mapping */
#define LCD_TE_GPIO_PORT                    GPIOE
#define LCD_TE_GPIO_PIN                     GPIO_PIN_13
#define LCD_TE_GPIO_LINE                    EXTI_LINE_13
#define LCD_TE_GPIO_IRQn                    EXTI13_IRQn

/* Tearing Effect EXTI handler */
#define H_EXTI_13                           hexti_lcd_te

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* Chip Reset macro definition */
#define LCD_RST_LOW()                       WRITE_REG(GPIOE->BRR, GPIO_PIN_11)
#define LCD_RST_HIGH()                      WRITE_REG(GPIOE->BSRR, GPIO_PIN_11)

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif
#endif /* __LCD_CONF_H__ */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
