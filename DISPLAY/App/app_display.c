/**
  ******************************************************************************
  * File Name          : app_display.c
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

/* Includes ------------------------------------------------------------------*/
#include "app_display.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
#include "key_io.h"
#include "mem_io.h"
#include "lcd_io.h"
/* USER CODE BEGIN Includes */
//#define LCD_STRESS_TESTS
#include "string.h"
#include "stdbool.h"
#if !defined(LCD_STRESS_TESTS)
#include "stm32_lcd.h"
#endif /* RENDER_TIME_Pin */
#include "Image1.h"
#include "Image2.h"
#include "Image3.h"
#include "Image4.h"
#include "Image5.h"
#include "Image6.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct image_s
{
  uint32_t  Width;
  uint32_t  Height;
  uint8_t   bpp;
  uint8_t*  Data;
} image_t;

typedef struct orientation_s
{
  uint32_t  lcd;
  uint32_t  key;
} orientation_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STEP_LINES                  1
#define USE_LCD_DMA                 1
#define USE_LCD_DMA_LINKED_LIST
#define BUTTON_USER_PRESSED_STATE   GPIO_PIN_SET
#if defined(UTIL_LCD_DEFAULT_FONT)
#define MAIN_BOARD_NAME             (uint8_t *)"NUCLEO-U575ZI-Q"
#define EXPANSION_BOARD_NAME        (uint8_t *)"X-NUCLEO-GFX02Z1"
#endif /* UTIL_LCD_DEFAULT_FONT */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#if defined(VSYNC_FREQ_Pin)
#define LCD_VSYNC_FREQ_LOW()                       WRITE_REG(VSYNC_FREQ_GPIO_Port->BRR, VSYNC_FREQ_Pin)
#define LCD_VSYNC_FREQ_HIGH()                      WRITE_REG(VSYNC_FREQ_GPIO_Port->BSRR, VSYNC_FREQ_Pin)
#endif /* VSYNC_FREQ_Pin */

#if defined(RENDER_TIME_Pin)
#define LCD_RENDER_TIME_LOW()                       WRITE_REG(RENDER_TIME_GPIO_Port->BRR, RENDER_TIME_Pin)
#define LCD_RENDER_TIME_HIGH()                      WRITE_REG(RENDER_TIME_GPIO_Port->BSRR, RENDER_TIME_Pin)
#endif /* RENDER_TIME_Pin */

#if defined(FRAME_RATE_Pin)
#define LCD_FRAME_RATE_LOW()                       WRITE_REG(FRAME_RATE_GPIO_Port->BRR, FRAME_RATE_Pin)
#define LCD_FRAME_RATE_HIGH()                      WRITE_REG(FRAME_RATE_GPIO_Port->BSRR, FRAME_RATE_Pin)
#endif /* FRAME_RATE_Pin */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t __IO TransferAllowed = 0;
#ifdef ALIGN_32BYTES
ALIGN_32BYTES(static uint8_t CacheBuffer[(320*2*BUFFER_CACHE_LINES)]);
#else
static uint8_t CacheBuffer[(320*2*BUFFER_CACHE_LINES)];
#endif
static uint16_t posy0 = 0;
static uint16_t posx = 0;
static uint16_t posy = 0;
static uint8_t key = 1;
static uint8_t image_id = 0;
static uint32_t LCD_Width = 0;
static uint32_t LCD_Height = 0;
static uint32_t LCD_Orientation = 0;
static uint8_t orientation_id = 0;

static image_t Images[] = { { 240, 240, 2, (uint8_t *)Image1 }
                          , { 240, 320, 2, (uint8_t *)Image2 }
                          , { 320, 240, 2, (uint8_t *)Image3 }
                          , { 240, 240, 2, (uint8_t *)Image4 }
                          , { 240, 320, 2, (uint8_t *)Image5 }
                          , { 240, 240, 2, (uint8_t *)Image6 }
                          , { 0, 0, 0, 0} };

static const orientation_t orientations[] = { { LCD_ORIENTATION_PORTRAIT, KEY_ORIENTATION_PORTRAIT }
                                            , { LCD_ORIENTATION_LANDSCAPE, KEY_ORIENTATION_LANDSCAPE }
                                            , { LCD_ORIENTATION_PORTRAIT_ROT180, KEY_ORIENTATION_PORTRAIT_ROT180 }
                                            , { LCD_ORIENTATION_LANDSCAPE_ROT180, KEY_ORIENTATION_LANDSCAPE_ROT180 }} ;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void BSP_LCD_Clear(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height);
static void Display_Image(image_t *image, uint16_t posx, uint16_t posy);
#if defined(UTIL_LCD_DEFAULT_FONT)
static int32_t BSP_LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color);
static int32_t BSP_LCD_FillRGBRect2(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height);
static int32_t BSP_LCD_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat);
static void Display_DemoDescription(void);
static void Display_DemoHints(void);
#endif /* UTIL_LCD_DEFAULT_FONT */

#if defined(UTIL_LCD_DEFAULT_FONT)
static const LCD_UTILS_Drv_t LCD_Driver =
{
  NULL,                   /* DrawBitmap   */
  BSP_LCD_FillRGBRect2,   /* FillRGBRect  */
  NULL,                   /* DrawHLine    */
  NULL,                   /* DrawVLine    */
  BSP_LCD_FillRect,       /* FillRect     */
  NULL,                   /* GetPixel     */
  NULL,                   /* SetPixel     */
  BSP_LCD_GetXSize,       /* GetXSize     */
  BSP_LCD_GetYSize,       /* GetYSize     */
  NULL,                   /* SetLayer     */
  BSP_LCD_GetPixelFormat  /* GetFormat    */
};
#endif /* UTIL_LCD_DEFAULT_FONT */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
  * @brief  Draws a full rectangle in currently active layer.
  * @param  Instance   LCD Instance
  * @param  Xpos X position
  * @param  Ypos Y position
  * @param  Width Rectangle width
  * @param  Height Rectangle height
  */
static int32_t BSP_LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height, uint32_t Color)
{
  uint32_t size;
  uint32_t CacheLinesCnt, CacheLinesSz;
  uint32_t offset = 0, line_cnt = Ypos;

  size = (2*Width*Height);
  CacheLinesCnt = (Height > BUFFER_CACHE_LINES ? BUFFER_CACHE_LINES : Height);
  CacheLinesSz = (2*Width*CacheLinesCnt);
  memset(CacheBuffer, Color, CacheLinesSz);
#if (__CORTEX_M == 0x07) && defined(SCB_CCR_DC_Msk)
  if((SCB->CCR) & (uint32_t)SCB_CCR_DC_Msk)
  {
    SCB_CleanDCache_by_Addr((uint32_t*)CacheBuffer, CacheLinesSz);
  }
#endif

  while(1)
  {
    BSP_LCD_WaitForTransferToBeDone(0);
    if(BSP_LCD_FillRGBRect(Instance, USE_LCD_DMA, CacheBuffer, Xpos, line_cnt, Width, CacheLinesCnt) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    line_cnt += CacheLinesCnt;
    offset += CacheLinesSz;
    /* Check remaining data size */
    if(offset == size)
    {
      /* last block transfer was done, so exit */
      break;
    }
    else if((offset + CacheLinesSz) > size)
    {
      /* Transfer last block and exit */
      CacheLinesCnt = ((size - offset)/ (2*Width));
      BSP_LCD_WaitForTransferToBeDone(0);
      if(BSP_LCD_FillRGBRect(Instance, USE_LCD_DMA, CacheBuffer, Xpos, line_cnt, Width, CacheLinesCnt) != BSP_ERROR_NONE)
      {
        Error_Handler();
      }
      break;
    }
  }
  BSP_LCD_WaitForTransferToBeDone(0);
  return BSP_ERROR_NONE;
}

static void BSP_LCD_Clear(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint32_t Width, uint32_t Height)
{
  BSP_LCD_FillRect(Instance, Xpos, Ypos, Width, Height, 0);
}

#if defined(UTIL_LCD_DEFAULT_FONT)
/**
  * @brief  Gets the LCD Active LCD Pixel Format.
  * @param  Instance    LCD Instance
  * @param  PixelFormat Active LCD Pixel Format
  * @retval BSP status
  */
static int32_t BSP_LCD_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat)
{
  int32_t ret = BSP_ERROR_NONE;

  if(Instance >= LCD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Only RGB565 format is supported */
    *PixelFormat = LCD_PIXEL_FORMAT_RGB565;
  }

  return ret;
}

static int32_t BSP_LCD_FillRGBRect2(uint32_t Instance, uint32_t Xpos, uint32_t Ypos, uint8_t *pData, uint32_t Width, uint32_t Height)
{
  BSP_LCD_WaitForTransferToBeDone(0);
  return BSP_LCD_FillRGBRect(Instance, 0, pData, Xpos, Ypos, Width, Height);
}

/**
  * @brief  Display main demo messages
  * @retval None
  */
static void Display_DemoDescription(void)
{
  uint32_t x_size;
  uint32_t y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_TRANSPARENT);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_YELLOW);

  UTIL_LCD_SetFont(&Font20);
  UTIL_LCD_DisplayStringAt(0, 0, MAIN_BOARD_NAME, CENTER_MODE);
  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_DisplayStringAt(0, 20, EXPANSION_BOARD_NAME, CENTER_MODE);

  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_MAGENTA);
  if((LCD_Orientation == LCD_ORIENTATION_LANDSCAPE) || (LCD_Orientation == LCD_ORIENTATION_LANDSCAPE_ROT180))
  {
    UTIL_LCD_SetFont(&Font12);
    UTIL_LCD_DisplayStringAt(0, y_size - 12, (uint8_t *)"Copyright (c) STMicroelectronics 2023", CENTER_MODE);
  }
  else
  {
    UTIL_LCD_SetFont(&Font8);
    UTIL_LCD_DisplayStringAt(0, y_size - 8, (uint8_t *)"Copyright (c) STMicroelectronics 2023", CENTER_MODE);
  }
}

static void Display_DemoHints(void)
{
  uint32_t t = 10;
  uint32_t x_size;
  uint32_t y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  /* Background Image */
  Display_Image(&Images[0], ((LCD_Width - Images[0].Width)/2) , ((LCD_Height - Images[0].Height)/2));

  /* Hint 1 */
  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_FillRect(4, ((y_size/2) - 47), (x_size - 8), 94, UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(8, ((y_size/2) - 44), (uint8_t *)"Use Joystick :", LEFT_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(8, ((y_size/2) - 21), (uint8_t *)" * Left/Right to slide images", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(8, ((y_size/2) -  7), (uint8_t *)" * Up/Down to move image up/down", LEFT_MODE);
  UTIL_LCD_DisplayStringAt(8, ((y_size/2) +  7), (uint8_t *)" * Center to center the image", LEFT_MODE);
  while (HAL_GPIO_ReadPin(BUTTON_USER_GPIO_Port, BUTTON_USER_Pin) != BUTTON_USER_PRESSED_STATE)
  {
    if(t == 5)
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    }
    else if (t == 10)
    {
      t = 0;
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
    }
    t++;
    UTIL_LCD_DisplayStringAt(4, ((y_size/2) + 31), (uint8_t *)"Press USER Button to continue", CENTER_MODE);
    HAL_Delay(100);
  }
  HAL_Delay(400);

  /* Background Image */
  Display_Image(&Images[0], ((LCD_Width - Images[0].Width)/2) , ((LCD_Height - Images[0].Height)/2));

  /* Hint 2 */
  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_FillRect(4, ((y_size/2) - 33), (x_size - 8), 66, UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DisplayStringAt(8, ((y_size/2) - 30), (uint8_t *)"Use USER Button :", LEFT_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(8, ((y_size/2) -  7), (uint8_t *)" * Rotate image clockwise", LEFT_MODE);
  while (HAL_GPIO_ReadPin(BUTTON_USER_GPIO_Port, BUTTON_USER_Pin) != BUTTON_USER_PRESSED_STATE)
  {
    if(t == 5)
    {
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
    }
    else if (t == 10)
    {
      t = 0;
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_BLUE);
    }
    t++;
    UTIL_LCD_DisplayStringAt(4, ((y_size/2) + 17), (uint8_t *)"Press USER Button to start", CENTER_MODE);
    HAL_Delay(100);
  }
  HAL_Delay(400);
}
#endif /* UTIL_LCD_DEFAULT_FONT */

/**
  * @brief  Refresh the display.
  * @param  None.
  * @retval None.
  */
static void Display_Image(image_t *image, uint16_t posx, uint16_t posy)
{
  uint8_t *pData = (uint8_t *)image->Data;
  uint32_t offset = 0;
  uint32_t Height = image->Height;
#if !defined(USE_LCD_DMA_LINKED_LIST)
  uint32_t size = (image->bpp * image->Width * image->Height);
  uint32_t CacheLinesCnt, CacheLinesSz;
  uint32_t line_cnt = posy;
#endif /* ! USE_LCD_DMA_LINKED_LIST */

  if(image->Height > LCD_Height)
  {
    offset = (image->bpp * image->Width * ((image->Height - LCD_Height) / 2));
    Height = LCD_Height;
#if !defined(USE_LCD_DMA_LINKED_LIST)
    size -= offset;
#endif /* ! USE_LCD_DMA_LINKED_LIST */
  }

#if defined(RENDER_TIME_Pin)
  LCD_RENDER_TIME_HIGH();
#endif /* RENDER_TIME_Pin */

#if defined(USE_LCD_DMA_LINKED_LIST)
  BSP_LCD_WaitForTransferToBeDone(0);
  if(BSP_LCD_FillRGBRect(0, USE_LCD_DMA, pData+offset, posx, posy, image->Width, Height) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#else
  CacheLinesCnt = (Height > BUFFER_CACHE_LINES ? BUFFER_CACHE_LINES : Height);
  CacheLinesSz = (image->bpp * image->Width * CacheLinesCnt);

  while(1)
  {
    BSP_LCD_WaitForTransferToBeDone(0);
    if(BSP_LCD_FillRGBRect(0, USE_LCD_DMA, pData+offset, posx, line_cnt, image->Width, CacheLinesCnt) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    line_cnt += CacheLinesCnt;
    offset += CacheLinesSz;
    /* Check remaining data size */
    if(offset == size)
    {
      /* last block transfer was done, so exit */
      break;
    }
    else if((offset + CacheLinesSz) > size)
    {
      /* Transfer last block and exit */
      CacheLinesCnt = ((size - offset)/ (2*image->Width));
      BSP_LCD_WaitForTransferToBeDone(0);
      if(BSP_LCD_FillRGBRect(0, USE_LCD_DMA, pData+offset, posx, line_cnt, image->Width, CacheLinesCnt) != BSP_ERROR_NONE)
      {
        Error_Handler();
      }
      break;
    }
  }
#endif /* USE_LCD_DMA_LINKED_LIST */

  /* We need block until this image transfer is done */
  BSP_LCD_WaitForTransferToBeDone(0);

#if defined(RENDER_TIME_Pin)
  LCD_RENDER_TIME_LOW();
#endif /* RENDER_TIME_Pin */
}
/* USER CODE END 0 */

/**
 * Initialize DISPLAY application
 */
void MX_DISPLAY_Init(void)
{
  /* USER CODE BEGIN MX_DISPLAY_Init 0 */

  /* USER CODE END MX_DISPLAY_Init 0 */
  if(BSP_LCD_Init(0, LCD_ORIENTATION_PORTRAIT) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  if(BSP_MEM_Init(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  if(BSP_KEY_Init(0, KEY_ORIENTATION_PORTRAIT) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MX_DISPLAY_Init 1 */
  if((BSP_LCD_GetXSize(0, &LCD_Width) != BSP_ERROR_NONE) \
  || (BSP_LCD_GetYSize(0, &LCD_Height) != BSP_ERROR_NONE) \
  || (BSP_LCD_GetOrientation(0, &LCD_Orientation) != BSP_ERROR_NONE) )
  {
    Error_Handler();
  }
  BSP_LCD_Clear(0, 0, 0, LCD_Width, LCD_Height);
  if(BSP_LCD_DisplayOn(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#if defined(UTIL_LCD_DEFAULT_FONT)
  /* Register LCD UTILS */
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  /* Show Demo Hints */
  Display_DemoDescription();
  Display_DemoHints();
#endif /* UTIL_LCD_DEFAULT_FONT */
  /* USER CODE END MX_DISPLAY_Init 1 */
}

/**
 * DISPLAY application entry function
 */
void MX_DISPLAY_Process(void)
{
  /* USER CODE BEGIN MX_DISPLAY_Process */
  int i;
  static __IO uint8_t can_move = 0;

#if !defined(LCD_STRESS_TESTS)
   if(key)
#endif /* RENDER_TIME_Pin */
  {
#if defined(FRAME_RATE_Pin)
    LCD_FRAME_RATE_HIGH();
#endif /* FRAME_RATE_Pin */

    /* Check if we can allow scrolling up/down the picture */
    if ((can_move == 0) && (Images[image_id].Height < LCD_Height) \
    && ((LCD_Orientation == LCD_ORIENTATION_PORTRAIT) || (LCD_Orientation == KEY_ORIENTATION_PORTRAIT_ROT180)))
    {
      /* Allow moving the picture on screen */
      can_move = 1;
    }
    if(posy == 0) /* reload new image */
    {
      posx = ((LCD_Width - Images[image_id].Width)/2);
      if(Images[image_id].Height < LCD_Height)
      {
        posy = ((LCD_Height - Images[image_id].Height)/2);
      }
      else
      {
        posy = 0;
      }
      posy0 = posy;
      if(key)
      {
        if (posx > 0)
        {
          BSP_LCD_Clear(0, 0, 0, posx, LCD_Height);
          BSP_LCD_Clear(0, posx+Images[image_id].Width, 0, posx, LCD_Height);
        }
        if (posy > 0)
        {
          BSP_LCD_Clear(0, 0, 0, LCD_Width, posy);
          BSP_LCD_Clear(0, 0, posy+Images[image_id].Height, LCD_Width, posy);
        }
      }
    }
    else if(posy == posy0) /* center current image */
    {
      posy = ((LCD_Height - Images[image_id].Height)/2);
      if(key)
      {
        BSP_LCD_Clear(0, 0, 0, LCD_Width, posy);
        BSP_LCD_Clear(0, 0, posy+Images[image_id].Height, LCD_Width, posy);
      }
    }
    Display_Image(&Images[image_id], posx, posy);
#if defined(UTIL_LCD_DEFAULT_FONT)
    /* Display the Demo Description Text */
    Display_DemoDescription();
#endif /* UTIL_LCD_DEFAULT_FONT */
#if defined(FRAME_RATE_Pin)
    LCD_FRAME_RATE_LOW();
#endif /* FRAME_RATE_Pin */
    /* Reset key value */
    key = 0;
  }

  if(BSP_KEY_GetState(0, &key) == BSP_ERROR_NONE)
  {
    switch(key)
    {
      case BSP_KEY_CENTER:
        if(posy != posy0)
        {
          posy = posy0;
          HAL_Delay(200);
        }
        else
        {
          /* Ignore this key */
          key = 0;
        }
        break;
      case BSP_KEY_UP:
        if (can_move && (posy > STEP_LINES))
        {
          BSP_LCD_Clear(0, 0, (posy + Images[image_id].Height), LCD_Width, STEP_LINES);
          posy -= STEP_LINES;
        }
        else
        {
          /* Ignore this key */
          key = 0;
        }
        break;
      case BSP_KEY_DOWN:
        if (can_move && (posy <(LCD_Height-Images[image_id].Height-STEP_LINES)))
        {
          BSP_LCD_Clear(0, 0, posy, LCD_Width, STEP_LINES);
          posy += STEP_LINES;
        }
        else
        {
          /* Ignore this key */
          key = 0;
        }
        break;
      case BSP_KEY_LEFT:
        i = image_id-1;
        while ((i >= 0) && (Images[i].Width > LCD_Width)) { i--; }
        if (image_id > 0)
        {
          can_move = 0;
          image_id = i;
          posy = 0;
          HAL_Delay(200);
        }
        else
        {
          /* Ignore this key */
          key = 0;
        }
        break;
      case BSP_KEY_RIGHT:
        i = image_id+1;
        while ((Images[i].Height > 0) && (Images[i].Width > LCD_Width)) { i++; }
        if (Images[image_id+1].Height > 0)
        {
          can_move = 0;
          image_id = i;
          posy = 0;
          HAL_Delay(200);
        }
        else
        {
          /* Ignore this key */
          key = 0;
        }
        break;
      default:
        break;
    }
  }
  else if (HAL_GPIO_ReadPin(BUTTON_USER_GPIO_Port, BUTTON_USER_Pin) == BUTTON_USER_PRESSED_STATE)
  {
    /* Read User Button */
    orientation_id++;
    if(orientation_id == 4)
    {
      orientation_id = 0;
    }
    BSP_LCD_Clear(0, 0, 0, LCD_Width, LCD_Height);
    if(BSP_LCD_SetOrientation(0, orientations[orientation_id].lcd) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    if(BSP_KEY_SetOrientation(0, orientations[orientation_id].key) != BSP_ERROR_NONE)
    {
      Error_Handler();
    }
    if((BSP_LCD_GetXSize(0, &LCD_Width) != BSP_ERROR_NONE) \
    || (BSP_LCD_GetYSize(0, &LCD_Height) != BSP_ERROR_NONE) \
    || (BSP_LCD_GetOrientation(0, &LCD_Orientation) != BSP_ERROR_NONE) )
    {
      Error_Handler();
    }
#if defined(UTIL_LCD_DEFAULT_FONT)
    /* Update the UTILS Orientation */
    UTIL_LCD_SetFuncDriver(&LCD_Driver);
#endif /* UTIL_LCD_DEFAULT_FONT */
    i = image_id;
    while ((i >= 0) && (Images[i].Width > LCD_Width)) { i--; }
    while ((Images[i].Height > 0) && (Images[i].Width > LCD_Width)) { i++; }
    image_id = i;
    key = 255;
    posy = 0;
    can_move = 0;
    HAL_Delay(200);
  }
  /* USER CODE END MX_DISPLAY_Process */
}

void BSP_LCD_SignalTearingEffectEvent(uint32_t Instance, uint8_t state, uint16_t Line)
{
  if(Instance == 0)
  {
    /* USER CODE BEGIN BSP_LCD_SignalTearingEffectEvent */
    if(state)
    {
      /* Line '0' is the Vsync event */
      if(Line == 0)
      {
        /* TE event is received : de-allow display refresh */
        TransferAllowed = 0;
#if defined(VSYNC_FREQ_Pin)
        LCD_VSYNC_FREQ_HIGH();
#endif /* VSYNC_FREQ_Pin */
      }
    }
    else
    {
      /* TE event is done : allow display refresh */
      TransferAllowed = 1;
#if defined(VSYNC_FREQ_Pin)
      LCD_VSYNC_FREQ_LOW();
#endif /* VSYNC_FREQ_Pin */
    }
    /* USER CODE END BSP_LCD_SignalTearingEffectEvent */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
