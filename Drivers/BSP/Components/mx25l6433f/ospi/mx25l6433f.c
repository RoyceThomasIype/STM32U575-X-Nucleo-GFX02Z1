/**
 ******************************************************************************
 * @file    mx25l6433f.c
 * @modify  MCD Application Team
 * @brief   This file provides the MX25L6433F QSPI driver.
 ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "mx25l6433f.h"
#include "mem_conf.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @defgroup MX25L6433F MX25L6433F
  * @{
  */

/** @defgroup MX25L6433F_Exported_Variables MX25L6433F Exported Variables
  * @{
  */
const MX25L6433F_MEM_Drv_t MX25L6433F_MEM_Driver =
{
  MX25L6433F_Init,
  MX25L6433F_DeInit,
  MX25L6433F_ReadID,
  MX25L6433F_GetFlashInfo,
  MX25L6433F_Read,
  MX25L6433F_ReadDMA,
  MX25L6433F_PageProgram,
  MX25L6433F_PageProgramDMA,
  MX25L6433F_BlockErase,
  MX25L6433F_ChipErase,
  MX25L6433F_EnableMemoryMappedMode,
  MX25L6433F_ResetEnable,
  MX25L6433F_ResetMemory
};

/** @defgroup MX25L6433F_Private_Functions Private Functions
  * @{
  */


/** @defgroup MX25L6433F_Exported_Functions MX25L6433F Exported Functions
  * @{
  */
/**
  * @brief  Register component IO bus
  * @param  pObj Component object pointer
  * @param  pIO  Component IO structure pointer
  * @retval Component status
  */
int32_t MX25L6433F_RegisterBusIO(MX25L6433F_Object_t *pObj, MX25L6433F_IO_t *pIO)
{
  int32_t ret = MX25L6433F_OK;

  if(pObj == NULL)
  {
    ret = MX25L6433F_ERROR;
  }
  else
  {
    pObj->IO.Init         = pIO->Init;
    pObj->IO.DeInit       = pIO->DeInit;
    pObj->IO.Address      = pIO->Address;
    pObj->IO.SendData     = pIO->SendData;
    pObj->IO.SendDataDMA  = pIO->SendDataDMA;
    pObj->IO.RecvData     = pIO->RecvData;
    pObj->IO.RecvDataDMA  = pIO->RecvDataDMA;
    pObj->IO.GetTick      = pIO->GetTick;

    if(pObj->IO.Init != NULL)
    {
      ret = pObj->IO.Init();
    }
    else
    {
      ret = MX25L6433F_ERROR;
    }
  }

  return ret;
}

/**
  * @brief  Initialize the MX25L6433F device
  * @param  pObj Component object pointer
  * @retval error status
  */
int32_t MX25L6433F_Init(MX25L6433F_Object_t *pObj)
{
  int32_t ret = MX25L6433F_OK;

  if(pObj == NULL)
  {
    ret = MX25L6433F_ERROR;
  }
  else if (pObj->IsInitialized == 0)
  {
    /* Reset Memory */
    ret = MX25L6433F_ResetEnable(pObj);
    if(ret == MX25L6433F_OK)
    {
      ret = MX25L6433F_ResetMemory(pObj);
      if(ret == MX25L6433F_OK)
      {
        /* Setup Quad SPI mode */
        ret = MX25L6433F_EnableQuadMode(pObj);
        if(ret == MX25L6433F_OK)
        {
          pObj->IsInitialized = 1;
        }
      }
    }
  }

  return ret;
}

/**
  * @brief  De-Initialize the ili9341 LCD Component.
  * @param  pObj Component object
  * @retval Component status
  */
int32_t MX25L6433F_DeInit(MX25L6433F_Object_t *pObj)
{
  int32_t ret = MX25L6433F_OK;

  if(pObj == NULL)
  {
    ret = MX25L6433F_ERROR;
  }
  else if (pObj->IsInitialized == 1)
  {
    if(pObj->IO.DeInit() == 0)
    {
      pObj->IsInitialized = 0;
    }
    else
    {
      ret = MX25L6433F_ERROR;
    }
  }

  return ret;
}

/**
  * @brief  Get Flash information
  * @param  pInfo pointer to information structure
  * @retval error status
  */
int32_t MX25L6433F_GetFlashInfo(MX25L6433F_Info_t *pInfo)
{
  /* Configure the structure with the memory configuration */
  pInfo->FlashSize              = MX25L6433F_FLASH_SIZE;
  pInfo->EraseBlockSize         = MX25L6433F_BLOCK_64K;
  pInfo->EraseBlocksNumber      = (MX25L6433F_FLASH_SIZE/MX25L6433F_BLOCK_64K);
  pInfo->EraseSubBlockSize      = MX25L6433F_BLOCK_32K;
  pInfo->EraseSubBlocksNumber   = (MX25L6433F_FLASH_SIZE/MX25L6433F_BLOCK_32K);
  pInfo->EraseSectorSize        = MX25L6433F_SECTOR_4K;
  pInfo->EraseSectorsNumber     = (MX25L6433F_FLASH_SIZE/MX25L6433F_SECTOR_4K);
  pInfo->ProgPageSize           = MX25L6433F_PAGE_SIZE;
  pInfo->ProgPagesNumber        = (MX25L6433F_FLASH_SIZE/MX25L6433F_PAGE_SIZE);

  return MX25L6433F_OK;
};

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  hospi: OSPI handle
  * @retval BSP status
  */
int32_t MX25L6433F_AutoPollingMemReady(MX25L6433F_Object_t *pObj)
{
  HAL_StatusTypeDef status;
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_AutoPollingTypeDef sConfig;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Configure automatic polling mode to wait for memory ready ------ */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = MX25L6433F_READ_STATUS_REG_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = 1;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  status = HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
  if (status != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  sConfig.Match         = MX25L6433F_MEMORY_READY_MATCH_VALUE;
  sConfig.Mask          = MX25L6433F_MEMORY_READY_MASK_VALUE;
  sConfig.MatchMode     = HAL_OSPI_MATCH_MODE_AND;
  sConfig.Interval      = MX25L6433F_AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

  status = HAL_OSPI_AutoPolling(hospi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
  if (status != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  return MX25L6433F_OK;
}

/* Read/Write Array Commands ****************************************************/
/**
  * @brief  Reads an amount of data from the QSPI memory.
  *         SPI/DUAL_OUT/DUAL_INOUT/QUAD_OUT/QUAD_INOUT/; 1-1-1/1-1-2/1-2-2/1-1-4/1-4-4
  * @param  pObj Component object pointer
  * @param  Mode Interface mode
  * @param  pData Pointer to data to be read
  * @param  ReadAddr Read start address
  * @param  Size Size of data to read
  * @retval QSPI memory status
  */
int32_t MX25L6433F_Read(MX25L6433F_Object_t *pObj, MX25L6433F_Interface_t Mode, uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Wait till the memory is ready */
  if(MX25L6433F_AutoPollingMemReady(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Address            = ReadAddr;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = Size;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  switch(Mode)
  {
    case MX25L6433F_SPI_MODE :
      sCommand.Instruction      = MX25L6433F_FAST_READ_CMD;
      sCommand.AddressMode      = HAL_OSPI_ADDRESS_1_LINE;
      sCommand.DataMode         = HAL_OSPI_DATA_1_LINE;
      sCommand.DummyCycles      = MEM_DUMMY_CYCLES_READ;
      break;

    case MX25L6433F_DUAL_OUT_MODE :
      sCommand.Instruction      = MX25L6433F_DUAL_OUT_READ_CMD;
      sCommand.AddressMode      = HAL_OSPI_ADDRESS_1_LINE;
      sCommand.DataMode         = HAL_OSPI_DATA_2_LINES;
      sCommand.DummyCycles      = MEM_DUMMY_CYCLES_READ;
      break;

    case MX25L6433F_DUAL_IO_MODE :
      sCommand.Instruction      = MX25L6433F_DUAL_INOUT_READ_CMD;
      sCommand.AddressMode      = HAL_OSPI_ADDRESS_2_LINES;
      sCommand.DataMode         = HAL_OSPI_DATA_2_LINES;
      sCommand.DummyCycles      = MEM_DUMMY_CYCLES_READ_DUAL;
      break;

    case MX25L6433F_QUAD_OUT_MODE :
      sCommand.Instruction      = MX25L6433F_QUAD_OUT_READ_CMD;
      sCommand.AddressMode      = HAL_OSPI_ADDRESS_1_LINE;
      sCommand.DataMode         = HAL_OSPI_DATA_4_LINES;
      sCommand.DummyCycles      = MEM_DUMMY_CYCLES_READ;
      break;

    case MX25L6433F_QUAD_IO_MODE :
      sCommand.Instruction      = MX25L6433F_QUAD_INOUT_READ_CMD;
      sCommand.AddressMode      = HAL_OSPI_ADDRESS_4_LINES;
      sCommand.DataMode         = HAL_OSPI_DATA_4_LINES;
      sCommand.DummyCycles      = MEM_DUMMY_CYCLES_READ_QUAD;
      break;

    default :
      return MX25L6433F_ERROR;
  }

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  if (HAL_OSPI_Receive(hospi, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  return MX25L6433F_OK;
}

/**
  * @brief  Reads an amount of data using DMA from the QSPI memory.
  *         SPI/DUAL_OUT/DUAL_INOUT/QUAD_OUT/QUAD_INOUT/; 1-1-1/1-1-2/1-2-2/1-1-4/1-4-4
  * @param  pObj Component object pointer
  * @param  Mode Interface mode
  * @param  pData Pointer to data to be read
  * @param  ReadAddr Read start address
  * @param  Size Size of data to read
  * @retval QSPI memory status
  */
int32_t MX25L6433F_ReadDMA(MX25L6433F_Object_t *pObj, MX25L6433F_Interface_t Mode, uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Wait till the memory is ready */
  if(MX25L6433F_AutoPollingMemReady(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Address            = ReadAddr;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = Size;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  switch(Mode)
  {
    case MX25L6433F_SPI_MODE :
      sCommand.Instruction      = MX25L6433F_FAST_READ_CMD;
      sCommand.AddressMode      = HAL_OSPI_ADDRESS_1_LINE;
      sCommand.DataMode         = HAL_OSPI_DATA_1_LINE;
      sCommand.DummyCycles      = MEM_DUMMY_CYCLES_READ;
      break;

    case MX25L6433F_DUAL_OUT_MODE :
      sCommand.Instruction      = MX25L6433F_DUAL_OUT_READ_CMD;
      sCommand.AddressMode      = HAL_OSPI_ADDRESS_1_LINE;
      sCommand.DataMode         = HAL_OSPI_DATA_2_LINES;
      sCommand.DummyCycles      = MEM_DUMMY_CYCLES_READ;
      break;

    case MX25L6433F_DUAL_IO_MODE :
      sCommand.Instruction      = MX25L6433F_DUAL_INOUT_READ_CMD;
      sCommand.AddressMode      = HAL_OSPI_ADDRESS_2_LINES;
      sCommand.DataMode         = HAL_OSPI_DATA_2_LINES;
      sCommand.DummyCycles      = MEM_DUMMY_CYCLES_READ_DUAL;
      break;

    case MX25L6433F_QUAD_OUT_MODE :
      sCommand.Instruction      = MX25L6433F_QUAD_OUT_READ_CMD;
      sCommand.AddressMode      = HAL_OSPI_ADDRESS_1_LINE;
      sCommand.DataMode         = HAL_OSPI_DATA_4_LINES;
      sCommand.DummyCycles      = MEM_DUMMY_CYCLES_READ;
      break;

    case MX25L6433F_QUAD_IO_MODE :
      sCommand.Instruction      = MX25L6433F_QUAD_INOUT_READ_CMD;
      sCommand.AddressMode      = HAL_OSPI_ADDRESS_4_LINES;
      sCommand.DataMode         = HAL_OSPI_DATA_4_LINES;
      sCommand.DummyCycles      = MEM_DUMMY_CYCLES_READ_QUAD;
      break;

    default :
      return MX25L6433F_ERROR;
  }

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  if (HAL_OSPI_Receive_DMA(hospi, pData) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  return MX25L6433F_OK;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  *         SPI/QUAD_INOUT/; 1-1-1/1-4-4
  * @param  pObj Component object pointer
  * @param  Mode Interface mode
  * @param  pData Pointer to data to be written
  * @param  WriteAddr Write start address
  * @param  Size Size of data to write. Range 1 ~ MX25L6433F_PAGE_SIZE
  * @note   Address size is forced to 3 Bytes when the 4 Bytes address size
  *         command is not available for the specified interface mode
  * @retval QSPI memory status
  */
int32_t MX25L6433F_PageProgram(MX25L6433F_Object_t *pObj, MX25L6433F_Interface_t Mode, uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  OSPI_RegularCmdTypeDef sCommand;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Wait till the memory is ready */
  if(MX25L6433F_AutoPollingMemReady(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  // Enable Write operation
  if(MX25L6433F_WriteEnable(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Page Program: 8-bit cmd, 24-bit WriteAddr, data... */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Address            = WriteAddr;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = Size;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  switch(Mode)
  {
    case MX25L6433F_SPI_MODE :
      sCommand.Instruction = MX25L6433F_PAGE_PROG_CMD;
      sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
      sCommand.DataMode    = HAL_OSPI_DATA_1_LINE;
      break;

    case MX25L6433F_QUAD_IO_MODE :
      sCommand.Instruction = MX25L6433F_QUAD_PAGE_PROG_CMD;
      sCommand.AddressMode = HAL_OSPI_ADDRESS_4_LINES;
      sCommand.DataMode    = HAL_OSPI_DATA_4_LINES;
      break;

    default :
      return MX25L6433F_ERROR;
  }

  /* Configure the command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Transmission of the data */
  if (HAL_OSPI_Transmit(hospi, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Wait till the memory is ready */
  return MX25L6433F_AutoPollingMemReady(pObj);
}

/**
  * @brief  Writes an amount of data using DMA to the QSPI memory.
  *         SPI/QUAD_INOUT/; 1-1-1/1-4-4
  * @param  pObj Component object pointer
  * @param  Mode Interface mode
  * @param  pData Pointer to data to be written
  * @param  WriteAddr Write start address
  * @param  Size Size of data to write. Range 1 ~ MX25L6433F_PAGE_SIZE
  * @note   Address size is forced to 3 Bytes when the 4 Bytes address size
  *         command is not available for the specified interface mode
  * @retval QSPI memory status
  */
int32_t MX25L6433F_PageProgramDMA(MX25L6433F_Object_t *pObj, MX25L6433F_Interface_t Mode, uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  OSPI_RegularCmdTypeDef sCommand;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Wait till the memory is ready */
  if(MX25L6433F_AutoPollingMemReady(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  // Enable Write operation
  if(MX25L6433F_WriteEnable(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Page Program: 8-bit cmd, 24-bit WriteAddr, data... */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Address            = WriteAddr;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = Size;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  switch(Mode)
  {
    case MX25L6433F_SPI_MODE :
      sCommand.Instruction = MX25L6433F_PAGE_PROG_CMD;
      sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
      sCommand.DataMode    = HAL_OSPI_DATA_1_LINE;
      break;

    case MX25L6433F_QUAD_IO_MODE :
      sCommand.Instruction = MX25L6433F_QUAD_PAGE_PROG_CMD;
      sCommand.AddressMode = HAL_OSPI_ADDRESS_4_LINES;
      sCommand.DataMode    = HAL_OSPI_DATA_4_LINES;
      break;

    default :
      return MX25L6433F_ERROR;
  }

  /* Configure the command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Transmission of the data using DMA */
  if (HAL_OSPI_Transmit_DMA(hospi, pData) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  return MX25L6433F_OK;
}

/**
  * @brief  Erases the specified block of the QSPI memory.
  *         MX25L6433F support 4K, 32K and 64K size block erase commands.
  * @param  pObj Component object pointer
  * @param  BlockAddress Block address to erase
  * @param  BlockSize Block size to erase
  * @retval QSPI memory status
  */
int32_t MX25L6433F_BlockErase(MX25L6433F_Object_t *pObj, uint32_t BlockAddress, MX25L6433F_Erase_t BlockSize)
{
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_AutoPollingTypeDef sConfig;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Wait till the memory is ready */
  if(MX25L6433F_AutoPollingMemReady(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  // Enable Write operation
  if(MX25L6433F_WriteEnable(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Sector Erase: 8-bit cmd, 24-bit BlockAddress */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Address            = BlockAddress;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = 0;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  switch(BlockSize)
  {
    case MX25L6433F_ERASE_4K :
      sCommand.Instruction = MX25L6433F_SECTOR_ERASE_CMD;
      break;

    case MX25L6433F_ERASE_32K :
      sCommand.Instruction = MX25L6433F_SUBBLOCK_ERASE_CMD;
      break;

    case MX25L6433F_ERASE_64K :
      sCommand.Instruction = MX25L6433F_BLOCK_ERASE_CMD;
      break;

    default :
      return MX25L6433F_ERROR;
  }

  /* Send the command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Configure automatic polling mode to wait for memory ready ------ */
  sCommand.Instruction        = MX25L6433F_READ_STATUS_REG_CMD;
  sCommand.Address            = 0x0;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.NbData             = 1;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  sConfig.Match         = MX25L6433F_MEMORY_READY_MATCH_VALUE;
  sConfig.Mask          = MX25L6433F_MEMORY_READY_MASK_VALUE;
  sConfig.MatchMode     = HAL_OSPI_MATCH_MODE_AND;
  sConfig.Interval      = MX25L6433F_AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_OSPI_AutoPolling(hospi, &sConfig, MX25L6433F_BLOCK_64K_ERASE_MAX_TIME) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  return MX25L6433F_OK;
}

/**
  * @brief  Whole chip erase.
  * @param  pObj Component object pointer
  * @param  Mode Interface mode
  * @retval error status
  */
int32_t MX25L6433F_ChipErase(MX25L6433F_Object_t *pObj)
{
  OSPI_RegularCmdTypeDef sCommand;
  OSPI_AutoPollingTypeDef sConfig;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Wait till the memory is ready */
  if(MX25L6433F_AutoPollingMemReady(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  // Enable Write operation
  if(MX25L6433F_WriteEnable(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Initialize the reset command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Instruction        = MX25L6433F_CHIP_ERASE_CMD_2;
  sCommand.Address            = 0;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = 0;
  sCommand.DummyCycles        = 0U;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Configure automatic polling mode to wait for memory ready ------ */
  sCommand.Instruction        = MX25L6433F_READ_STATUS_REG_CMD;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.NbData             = 1;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  sConfig.Match         = MX25L6433F_MEMORY_READY_MATCH_VALUE;
  sConfig.Mask          = MX25L6433F_MEMORY_READY_MASK_VALUE;
  sConfig.MatchMode     = HAL_OSPI_MATCH_MODE_AND;
  sConfig.Interval      = MX25L6433F_AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_OSPI_AutoPolling(hospi, &sConfig, MX25L6433F_CHIP_ERASE_MAX_TIME) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  return MX25L6433F_OK;
}

/* ID Commands ****************************************************************/
/**
  * @brief  Read Flash 3 Byte IDs.
  *         Manufacturer ID, Memory type, Memory density
  * @param  pObj Component object pointer
  * @param  ID 3 bytes IDs pointer
  * @retval error status
  */
int32_t MX25L6433F_ReadID(MX25L6433F_Object_t *pObj, uint32_t *ID)
{
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;
  uint8_t __IO id[4] = {0x00, 0x00, 0x00, 0x00};

  /* Wait till the memory is ready */
  if(MX25L6433F_AutoPollingMemReady(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Read Identification (RDID) */
  // 1-byte command OUT, 3-byte(Manufacturer ID: 1, Device ID: 2) IN
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = MX25L6433F_READ_ID_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = 3;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  if (HAL_OSPI_Receive(hospi, (uint8_t *)id, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  *ID = *(uint32_t *)id;

  return MX25L6433F_OK;
}

/**
  * @brief  This function configure the memory in Quad mode.
  * @param  pObj Component object pointer
  * @retval error status
  */
int32_t MX25L6433F_EnableQuadMode(MX25L6433F_Object_t *pObj)
{
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_AutoPollingTypeDef sConfig;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;
  uint8_t __IO reg = 0;

  /* Wait till the memory is ready */
  if(MX25L6433F_AutoPollingMemReady(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Read status register */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = MX25L6433F_READ_STATUS_REG_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = 1;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  if (HAL_OSPI_Receive(hospi, (uint8_t *)&reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  if((reg & MX25L6433F_SR_QE) != MX25L6433F_SR_QE)
  {
    /* Enable write operations */
    if(MX25L6433F_WriteEnable(pObj) != MX25L6433F_OK)
    {
      return MX25L6433F_ERROR;
    }

    SET_BIT(reg, MX25L6433F_SR_QE);

    sCommand.Instruction = MX25L6433F_WRITE_STATUS_CFG_REG_CMD;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return MX25L6433F_ERROR;
    }

    if (HAL_OSPI_Transmit(hospi, (uint8_t *)&reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return MX25L6433F_ERROR;
    }

    /* Configure automatic polling mode to wait for write enabling */
    sCommand.Instruction        = MX25L6433F_READ_STATUS_REG_CMD;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return MX25L6433F_ERROR;
    }

    sConfig.Match           = MX25L6433F_QUAD_ENABLE_MATCH_VALUE;
    sConfig.Mask            = MX25L6433F_QUAD_ENABLE_MASK_VALUE;
    sConfig.MatchMode       = HAL_OSPI_MATCH_MODE_AND;
    sConfig.Interval        = MX25L6433F_AUTO_POLLING_INTERVAL;
    sConfig.AutomaticStop   = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_OSPI_AutoPolling(hospi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return MX25L6433F_ERROR;
    }
  }

  return MX25L6433F_OK;
}

/**
  * @brief  This function disable the Quad mode.
  * @param  pObj Component object pointer
  * @retval error status
  */
int32_t MX25L6433F_DisableQuadMode(MX25L6433F_Object_t *pObj)
{
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_AutoPollingTypeDef sConfig;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;
  uint8_t __IO reg = 0xFF;

  /* Wait till the memory is ready */
  if(MX25L6433F_AutoPollingMemReady(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Read status register */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = MX25L6433F_READ_STATUS_REG_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = 1;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  if (HAL_OSPI_Receive(hospi, (uint8_t *)&reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  if((reg & MX25L6433F_SR_QE) == MX25L6433F_SR_QE)
  {
    CLEAR_BIT(reg, MX25L6433F_SR_QE);

    /* Enable write operations */
    if(MX25L6433F_WriteEnable(pObj) != MX25L6433F_OK)
    {
      return MX25L6433F_ERROR;
    }

    sCommand.Instruction = MX25L6433F_WRITE_STATUS_CFG_REG_CMD;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return MX25L6433F_ERROR;
    }

    if (HAL_OSPI_Transmit(hospi, (uint8_t *)&reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return MX25L6433F_ERROR;
    }

    /* Configure automatic polling mode to wait for write enabling */
    sCommand.Instruction        = MX25L6433F_READ_STATUS_REG_CMD;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return MX25L6433F_ERROR;
    }

    sConfig.Match           = 0;
    sConfig.Mask            = MX25L6433F_QUAD_ENABLE_MASK_VALUE;
    sConfig.MatchMode       = HAL_OSPI_MATCH_MODE_AND;
    sConfig.Interval        = MX25L6433F_AUTO_POLLING_INTERVAL;
    sConfig.AutomaticStop   = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_OSPI_AutoPolling(hospi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return MX25L6433F_ERROR;
    }
  }

  return MX25L6433F_OK;
}

int32_t MX25L6433F_EnableMemoryMappedMode(MX25L6433F_Object_t *pObj, MX25L6433F_Interface_t Mode)
{
  OSPI_RegularCmdTypeDef    sCommand;
  OSPI_MemoryMappedTypeDef  sMemMappedCfg;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Wait till the memory is ready */
  if(MX25L6433F_AutoPollingMemReady(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Initialize the read command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_READ_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Address            = 0;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = 0U;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  switch(Mode)
  {
    case MX25L6433F_SPI_MODE :
      sCommand.Instruction      = MX25L6433F_FAST_READ_CMD;
      sCommand.AddressMode      = HAL_OSPI_ADDRESS_1_LINE;
      sCommand.DataMode         = HAL_OSPI_DATA_1_LINE;
      sCommand.DummyCycles      = MEM_DUMMY_CYCLES_READ;
      break;

    case MX25L6433F_QUAD_IO_MODE :
      sCommand.Instruction      = MX25L6433F_QUAD_INOUT_READ_CMD;
      sCommand.AddressMode      = HAL_OSPI_ADDRESS_4_LINES;
      sCommand.DataMode         = HAL_OSPI_DATA_4_LINES;
      sCommand.DummyCycles      = MEM_DUMMY_CYCLES_READ_QUAD;
      break;

    default :
      return MX25L6433F_ERROR;
  }

  /* Send the Command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Initialize the program command */
  sCommand.OperationType        = HAL_OSPI_OPTYPE_WRITE_CFG;
  sCommand.DummyCycles          = 0U;
  sCommand.DQSMode              = HAL_OSPI_DQS_DISABLE;

  switch(Mode)
  {
    case MX25L6433F_SPI_MODE :
      sCommand.Instruction = MX25L6433F_PAGE_PROG_CMD;
      break;

    case MX25L6433F_QUAD_IO_MODE :
      sCommand.Instruction = MX25L6433F_QUAD_PAGE_PROG_CMD;
      break;

    default :
      return MX25L6433F_ERROR;
  }

  /* Send the command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  sMemMappedCfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_DISABLE;
  sMemMappedCfg.TimeOutPeriod     = 0;
  if (HAL_OSPI_MemoryMapped(hospi, &sMemMappedCfg) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  return MX25L6433F_OK;
}

/* Register/Setting Commands **************************************************/
/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  pObj Component object pointer
  * @retval error status
  */
int32_t MX25L6433F_WriteEnable(MX25L6433F_Object_t *pObj)
{
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_AutoPollingTypeDef sConfig;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Initialize the write enable command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = MX25L6433F_WRITE_ENABLE_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Configure automatic polling mode to wait for write enabling */
  sCommand.Instruction        = MX25L6433F_READ_STATUS_REG_CMD;
  sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.Address            = 0x0;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.NbData             = 1;

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  sConfig.Match           = MX25L6433F_WRITE_ENABLE_MATCH_VALUE;
  sConfig.Mask            = MX25L6433F_WRITE_ENABLE_MASK_VALUE;
  sConfig.MatchMode       = HAL_OSPI_MATCH_MODE_AND;
  sConfig.Interval        = MX25L6433F_AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop   = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_OSPI_AutoPolling(hospi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  return MX25L6433F_OK;
}

/**
  * @brief  This function reset the (WEN) Write Enable Latch bit.
  * @param  pObj Component object pointer
  * @retval error status
  */
int32_t MX25L6433F_WriteDisable(MX25L6433F_Object_t *pObj)
{
  OSPI_RegularCmdTypeDef sCommand;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Initialize the write disable command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Instruction        = MX25L6433F_WRITE_DISABLE_CMD;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  return MX25L6433F_OK;
}

/**
  * @brief  Read Flash Status register value
  * @param  pObj Component object pointer
  * @param  Value Status register value pointer
  * @retval error status
  */
int32_t MX25L6433F_ReadStatusRegister(MX25L6433F_Object_t *pObj, uint8_t *Value)
{
  OSPI_RegularCmdTypeDef sCommand;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;
  uint8_t __IO value;

  /* Initialize the reading of status register */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Instruction        = MX25L6433F_READ_STATUS_REG_CMD;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.DummyCycles        = 0U;
  sCommand.NbData             = 1U;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Reception of the data */
  if (HAL_OSPI_Receive(hospi, (uint8_t *)&value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  *Value = value;

  return MX25L6433F_OK;
}

int32_t MX25L6433F_ReadCfgRegister(MX25L6433F_Object_t *pObj, MX25L6433F_Interface_t Mode, uint8_t *Value)
{
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;
  uint8_t __IO value;

  /* Read status register */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = MX25L6433F_READ_CFG_REG_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = 1;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  switch(Mode)
  {
    case MX25L6433F_SPI_MODE :
      sCommand.DataMode    = HAL_OSPI_DATA_1_LINE;
      break;

    case MX25L6433F_DUAL_OUT_MODE :
    case MX25L6433F_DUAL_IO_MODE :
      sCommand.DataMode    = HAL_OSPI_DATA_2_LINES;
      break;

    case MX25L6433F_QUAD_OUT_MODE :
    case MX25L6433F_QUAD_IO_MODE :
      sCommand.DataMode    = HAL_OSPI_DATA_4_LINES;
      break;

    default :
      return MX25L6433F_ERROR;
  }

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  if (HAL_OSPI_Receive(hospi, (uint8_t *)&value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  *Value = value;

  return MX25L6433F_OK;
}

int32_t MX25L6433F_WriteCfgRegister(MX25L6433F_Object_t *pObj, MX25L6433F_Interface_t Mode, uint8_t Value)
{
  OSPI_RegularCmdTypeDef  sCommand;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;
  uint8_t __IO value = Value;

  /* Wait till the memory is ready */
  if(MX25L6433F_AutoPollingMemReady(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Enable write operations */
  if(MX25L6433F_WriteEnable(pObj) != MX25L6433F_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Read status register */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = MX25L6433F_WRITE_STATUS_CFG_REG_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData             = 1;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  switch(Mode)
  {
    case MX25L6433F_SPI_MODE :
      sCommand.DataMode    = HAL_OSPI_DATA_1_LINE;
      break;

    case MX25L6433F_DUAL_OUT_MODE :
    case MX25L6433F_DUAL_IO_MODE :
      sCommand.DataMode    = HAL_OSPI_DATA_2_LINES;
      break;

    case MX25L6433F_QUAD_OUT_MODE :
    case MX25L6433F_QUAD_IO_MODE :
      sCommand.DataMode    = HAL_OSPI_DATA_4_LINES;
      break;

    default :
      return MX25L6433F_ERROR;
  }

  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  if (HAL_OSPI_Transmit(hospi, (uint8_t *)&value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  /* Wait till the memory is ready */
  return MX25L6433F_AutoPollingMemReady(pObj);
}

/* Reset Commands *************************************************************/
/**
  * @brief  Flash reset enable command
  * @param  pObj Component object pointer
  * @retval error status
  */
int32_t MX25L6433F_ResetEnable(MX25L6433F_Object_t *pObj)
{
  OSPI_RegularCmdTypeDef sCommand;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Initialize the reset enable command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = MX25L6433F_RESET_ENABLE_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  return MX25L6433F_OK;
}

/**
  * @brief  Flash reset memory command
  * @param  pObj Component object pointer
  * @retval error status
  */
int32_t MX25L6433F_ResetMemory(MX25L6433F_Object_t *pObj)
{
  OSPI_RegularCmdTypeDef sCommand;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Initialize the reset command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = MX25L6433F_RESET_MEMORY_CMD;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  return MX25L6433F_Delay(MX25L6433F_RESET_MAX_TIME);
}

/**
  * @brief  Flash no operation command
  * @param  pObj Component object pointer
  * @retval error status
  */
int32_t MX25L6433F_NoOperation(MX25L6433F_Object_t *pObj)
{
  OSPI_RegularCmdTypeDef sCommand;
  OSPI_HandleTypeDef *hospi = (OSPI_HandleTypeDef *)pObj->handle;

  /* Initialize the no operation command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Instruction        = MX25L6433F_NO_OPERATION_CMD;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DummyCycles        = 0U;
  sCommand.DataDtrMode         = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MX25L6433F_ERROR;
  }

  return MX25L6433F_OK;
}

int32_t MX25L6433F_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);

  return MX25L6433F_OK;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
