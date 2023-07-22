#ifndef AP_H_
#define AP_H_


#include "ap_def.h"

#include "thread.h"

#include "cmd/cmd_thread.h"
#include "manage/mode.h"
#include "common/cli.h"
#include "common/led.h"


void apInit(void);
void apMain(void);


#endif