#ifndef FAULT_H_
#define FAULT_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_FAULT



typedef struct
{
  uint32_t magic_number;
  uint32_t type;

  bool     is_reg;
  uint32_t REG_R0;
  uint32_t REG_R1;
  uint32_t REG_R2;
  uint32_t REG_R3;
  uint32_t REG_R12;
  uint32_t REG_LR;
  uint32_t REG_PC;
  uint32_t REG_PSR;

  char     msg[32];

} fault_log_t;



bool faultInit(void);
bool faultReset(const char *p_msg, uint32_t *p_stack);


#endif

#ifdef __cplusplus
}
#endif

#endif 