#ifndef LED_THREAD_H_
#define LED_THREAD_H_


#include "ap_def.h"


typedef struct
{
  thread_t *p_thread;
} led_thread_t;


led_thread_t *ledThread(void);

#endif