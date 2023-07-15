#ifndef RESET_H_
#define RESET_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"


#ifdef _USE_HW_RESET


#define RESET_BIT_POWER       0
#define RESET_BIT_PIN         1
#define RESET_BIT_WDG         2
#define RESET_BIT_SOFT        3
#define RESET_BIT_ETC         4
#define RESET_BIT_MAX         5


#define MODE_BIT_BOOT         0
#define MODE_BIT_UPDATE       1
#define MODE_BIT_MAX          2


bool resetInit(void);
void resetLog(void);
void resetToBoot(void);
void resetToReset(void);

uint32_t resetGetBits(void);
void     resetSetBits(uint32_t data);
void     resetSetBootMode(uint32_t data);
uint32_t resetGetBootMode(void);

#endif


#ifdef __cplusplus
}
#endif

#endif 
