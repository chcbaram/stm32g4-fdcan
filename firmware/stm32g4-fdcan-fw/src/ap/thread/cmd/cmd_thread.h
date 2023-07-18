#ifndef CMD_THREAD_H_
#define CMD_THREAD_H_


#include "ap_def.h"


typedef struct
{
  thread_t *p_thread;
} cmd_thread_t;


cmd_thread_t *cmdThread(void);

#endif