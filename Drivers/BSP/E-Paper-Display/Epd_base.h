#ifndef _DISPLAY_EPD_BASE_H_
#define _DISPLAY_EPD_BASE_H_

#include "stm32l0xx_hal.h"

#define EPD_MOSI_0    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET)
#define EPD_MOSI_1    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET)

#define EPD_CLK_0    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET)
#define EPD_CLK_1    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET)

#define EPD_CS_0    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET)
#define EPD_CS_1    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET)

#define EPD_DC_0    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)
#define EPD_DC_1    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)

#define EPD_RST_0    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET)
#define EPD_RST_1    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET)


#define EPD_BUSY_LEVEL 0
#define isEPD_BUSY HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8)


extern void SpiDelay(unsigned char xrate);
extern void DriverDelay(unsigned long xms);
extern void SpiWrite(unsigned char value);

#endif
