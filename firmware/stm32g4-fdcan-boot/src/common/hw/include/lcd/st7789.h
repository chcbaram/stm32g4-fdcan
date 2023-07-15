/*
 * st7789.h
 *
 *  Created on: 2022. 9. 9.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_LCD_ST7789_H_
#define SRC_COMMON_HW_INCLUDE_LCD_ST7789_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_ST7789

#include "lcd.h"
#include "st7789_regs.h"



bool st7789Init(void);
bool st7789InitDriver(lcd_driver_t *p_driver);
void st7789SetWindow(int32_t x, int32_t y, int32_t w, int32_t h);
bool st7789SetCallBack(void (*p_func)(void));

uint16_t st7789GetWidth(void);
uint16_t st7789GetHeight(void);

void st7789FillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);

#endif

#ifdef __cplusplus
}
#endif

#endif 