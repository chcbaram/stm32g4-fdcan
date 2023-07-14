/*
 * hw.h
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */

#ifndef SRC_HW_HW_H_
#define SRC_HW_HW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"



#include "uart.h"
#include "cli.h"
#include "util.h"
#include "cmd.h"


void hwInit(void);


#ifdef __cplusplus
}
#endif

#endif /* SRC_HW_HW_H_ */
