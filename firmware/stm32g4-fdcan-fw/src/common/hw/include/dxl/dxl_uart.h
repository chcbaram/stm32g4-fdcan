/*
 * dxl_uart.h
 *
 *  Created on: 2021. 3. 8.
 *      Author: baram
 */

#ifndef SRC_HW_DRIVER_DXL_DXL_UART_H_
#define SRC_HW_DRIVER_DXL_DXL_UART_H_

#include "hw_def.h"


#ifdef _USE_HW_DXL
#include "dxl.h"

bool dxlUartDriver(dxl_driver_t *p_driver);

#endif

#endif /* SRC_HW_DRIVER_DXL_DXL_UART_H_ */
