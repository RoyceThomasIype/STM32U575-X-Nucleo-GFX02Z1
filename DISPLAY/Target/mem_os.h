/**
  ******************************************************************************
  * File Name          : Target/mem_os.h
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
#ifndef __MEM_OS_H__
#define __MEM_OS_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/** @addtogroup DISPLAY DISPLAY
  * @brief      DISPLAY Software Expansion Pack.
  * @{
  */
/** @defgroup External_Memory_Driver External Memory Driver
  * @brief    External Memory Driver API.
  * @{
  */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  MEM_OS_ERROR_NONE = 0
, MEM_OS_ERROR_WRONG_PARAM
, MEM_OS_ERROR_INIT
, MEM_OS_ERROR_BUSY
, MEM_OS_ERROR_WAIT
, MEM_OS_ERROR_LOCK
, MEM_OS_ERROR_UNLOCK
} MEM_OS_Error_t;

/* USER CODE BEGIN ETD */

/* USER CODE END ETD */

/* Exported constants --------------------------------------------------------*/
/** @defgroup External_Memory_Exported_Constants External Memory Exported Constants
  * @brief    External Memory Drivers Constants.
  * @{
  */
#define MEM_OS_NO_WAIT                      ((uint32_t)  0)
#define MEM_OS_WAIT_FOREVER                 ((uint32_t)  0xFFFFFFFFUL)

/**
  * @addtogroup External_Memory_OS_Busy_Timeout External Memory OS Busy Timeout value in Milliseconds
  * @brief      External Memory OS Busy Timeout value in Milliseconds, default is 1ms.
  * @{
  */
/* USER CODE BEGIN EC */
#define MEM_OS_TIMEOUT_BUSY                 ((uint32_t)  1) /*!< \hideinitializer External Memory OS Busy Timeout value in Milliseconds, default is 1ms */
/* USER CODE END EC */
/**
  * @}
  */
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Initialize the MEM OS ressources.
  * @param  Instance MEM Instance
  * @retval MEM_OS_Error_t
  */
uint8_t MEM_OS_Initialize(uint32_t Instance);

/**
  * @brief  Lock the MEM ressources. Blocks until lock is available.
  * @param  Instance MEM Instance
  * @retval MEM_OS_Error_t
  */
uint8_t MEM_OS_Lock(uint32_t Instance);

/**
  * @brief  Unlock the MEM ressources.
  * @param  Instance MEM Instance
  * @retval MEM_OS_Error_t
  */
uint8_t MEM_OS_Unlock(uint32_t Instance);

/**
  * @brief  Safe unlock the MEM ressources in interrupt context.
  * @param  Instance MEM Instance
  * @retval MEM_OS_Error_t
  */
uint8_t MEM_OS_UnlockFromISR(uint32_t Instance);

/**
  * @brief  Attempt to lock the MEM. If the lock is not available, do
  *         nothing.
  * @param  Instance MEM Instance
  * @param  Timeout timeout in ms
  * @retval MEM_OS_Error_t
  */
uint8_t MEM_OS_TryLock(uint32_t Instance, uint32_t Timeout);

/**
  * @brief  This function checks if the MEM ressources are locked or not.
  * @param  Instance MEM Instance
  * @retval '1' if locked, '0' otherwise
  */
uint8_t MEM_OS_IsLocked(uint32_t Instance);

/**
  * @brief  This function will block until MEM transfer is Done.
  * @param  Instance MEM Instance
  * @retval MEM_OS_Error_t
  */
uint8_t MEM_OS_WaitForTransferToBeDone(uint32_t Instance);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/**
  * @}
  */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif
#endif /* __MEM_OS_H__ */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
