#ifndef FMC_H_
#define FMC_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"


bool fmcInit(void);
void *fmcGetAddr(void);

#ifdef __cplusplus
}
#endif

#endif