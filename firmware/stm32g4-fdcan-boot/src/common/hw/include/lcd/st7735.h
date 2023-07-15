/*
 * st7735.h
 *
 *  Created on: 2020. 12. 27.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_LCD_ST7735_H_
#define SRC_COMMON_HW_INCLUDE_LCD_ST7735_H_

#include "hw_def.h"


#ifdef _USE_HW_ST7735

#include "lcd.h"
#include "st7735_regs.h"



bool st7735Init(void);
bool st7735InitDriver(lcd_driver_t *p_driver);
bool st7735DrawAvailable(void);
bool st7735RequestDraw(void);
void st7735SetWindow(int32_t x, int32_t y, int32_t w, int32_t h);

uint32_t st7735GetFps(void);
uint32_t st7735GetFpsTime(void);

uint16_t st7735GetWidth(void);
uint16_t st7735GetHeight(void);

bool st7735SetCallBack(void (*p_func)(void));
bool st7735SendBuffer(uint8_t *p_data, uint32_t length, uint32_t timeout_ms);
bool st7735DrawBuffer(int16_t x, int16_t y, uint16_t *buffer, uint16_t w, uint16_t h);
bool st7735DrawBufferedLine(int16_t x, int16_t y, uint16_t *buffer, uint16_t w);


#endif

#endif /* SRC_COMMON_HW_INCLUDE_LCD_ST7735_H_ */
