#ifndef AP_DEF_H_
#define AP_DEF_H_


#include "hw.h"



typedef struct thread_t_ thread_t;


typedef enum
{
  EVENT_MODE_CHANGE,
  EVENT_USB_OPEN,
  EVENT_MAX,
} EventCode_t;



#define AP_EVENT_NODE_MAX           16        
#define AP_EVENT_Q_MAX              8



#endif