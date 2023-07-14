/*
 * util.h
 *
 *  Created on: 2021. 2. 13.
 *      Author: baram
 */

#ifndef SRC_COMMON_CORE_UTIL_H_
#define SRC_COMMON_CORE_UTIL_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "def.h"


void utilUpdateCrc(uint16_t *p_crc_cur, uint8_t data_in);


#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_CORE_UTIL_H_ */
