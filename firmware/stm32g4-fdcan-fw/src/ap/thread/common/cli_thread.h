#ifndef CLI_THREAD_H_
#define CLI_THREAD_H_


#include "ap_def.h"


typedef struct
{
  thread_t *p_thread;
} cli_thread_t;


cli_thread_t *cliThread(void);

#endif