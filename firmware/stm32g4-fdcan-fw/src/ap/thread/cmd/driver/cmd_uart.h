#ifndef CMD_UART_H_
#define CMD_UART_H_


#include "ap_def.h"


bool cmdUartInitDriver(cmd_driver_t *p_driver, uint8_t ch, uint32_t baud);


#endif