#ifndef CMD_BOOT_H_
#define CMD_BOOT_H_


#include "ap_def.h"



bool cmdBootInit(void);
bool cmdBootIsBusy(void);
void cmdBootUpdate(cmd_t *p_cmd);
bool cmdBootProcess(cmd_t *p_cmd);

#endif