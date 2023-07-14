/*
 * def.h
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */

#ifndef SRC_COMMON_DEF_H_
#define SRC_COMMON_DEF_H_


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#if defined (__WIN32__) || (__WIN64__)

// Windows
#ifndef _WINSOCKAPI_
#include <winsock2.h>
#endif

#include <conio.h>
#include <Windows.h>
#endif



#include "err_code.h"



#define _DEF_UART1            0
#define _DEF_UART2            1
#define _DEF_UART3            2
#define _DEF_UART4            3




#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#ifndef cmax
#define cmax(a,b) (((a) > (b)) ? (a) : (b))
#define cmin(a,b) (((a) < (b)) ? (a) : (b))
#endif


#ifndef cmap
#define cmap(value, in_min, in_max, out_min, out_max) ((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#endif



#define VERSION_MAGIC_NUMBER      0x56455220    // "VER "
#define TAG_MAGIC_NUMBER          0x54414720    // "TAG "


typedef struct 
{
  uint32_t magic_number;
  char     version_str[32];
  char     name_str[32];
  uint32_t firm_addr;
} firm_ver_t;

typedef struct 
{
  uint32_t magic_number;
  uint32_t fw_addr;
  uint32_t fw_size;
  uint32_t fw_crc;

  uint32_t tag_crc;
} firm_tag_t;

#endif /* SRC_COMMON_DEF_H_ */
