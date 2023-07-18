#ifndef MODE_THREAD_H_
#define MODE_THREAD_H_


#include "ap_def.h"


typedef enum
{
  MODE_USB_TO_CAN,
  MODE_USB_TO_RS485,
  MODE_USB_TO_CLI
} ModeUsb_t;


typedef struct
{
  thread_t *p_thread;
} mode_thread_t;


mode_thread_t *modeThread(void);



#endif