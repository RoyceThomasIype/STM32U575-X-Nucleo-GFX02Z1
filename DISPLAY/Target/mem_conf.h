/**
  ******************************************************************************
  * File Name          : Target/mem_conf.h
  * Description        : This file provides code for the configuration
  *                      of the MEM instances.
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
#ifndef __MEM_CONF_H__
#define __MEM_CONF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* BUS IO Instance handler */
extern  OSPI_HandleTypeDef                  hospi1;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* Number of MEM instances */
#define MEM_INSTANCES_NBR                   1U

/* Memory Interface Mode */
#define MEM_INTERFACE_MODE                  MEM_INTERFACE_QUAD_IO_MODE

/* BUS IO Instance handler */
#define hMEMOSPI                            hospi1

/* Memory's dummy cycle read timings */
#define MEM_DUMMY_CYCLES_READ               8U
#define MEM_DUMMY_CYCLES_READ_DUAL          4U
#define MEM_DUMMY_CYCLES_READ_QUAD          6U

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif
#endif /* __MEM_CONF_H__ */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
