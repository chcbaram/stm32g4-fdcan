#ifndef EVENT_H_
#define EVENT_H_


#include "ap_def.h"



typedef struct 
{
  EventCode_t code;
  uint32_t    data;
} event_t;


bool eventInit(void);
bool eventNodeAdd(bool (*func)(event_t *p_event));
bool eventPut(event_t *p_event);
bool eventUpdate(void);

#endif