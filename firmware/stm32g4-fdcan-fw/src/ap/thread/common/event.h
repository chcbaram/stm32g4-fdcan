#ifndef EVENT_H_
#define EVENT_H_


#include "ap_def.h"


typedef enum
{
  EVENT_MODE_CHANGE,
} EventCode_t;

typedef struct Event_t_
{
  EventCode_t code;
  uint32_t    data;
} Event_t;


bool eventInit(void);
bool eventAvailble(void);
bool eventPut(Event_t *p_event);
bool eventGet(Event_t *p_event);


#endif