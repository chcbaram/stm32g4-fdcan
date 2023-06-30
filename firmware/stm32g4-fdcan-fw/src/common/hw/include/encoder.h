#ifndef ENCODER_H_
#define ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_ENCODER

#define ENCODER_MAX_CH  HW_ENCODER_MAX_CH


bool encoderInit(void);
bool encoderReset(void);

bool encoderClearCount(uint8_t ch);
int32_t encoderGetCount(uint8_t ch);


#endif

#ifdef __cplusplus
}
#endif

#endif