#ifndef THREAD_H_
#define THREAD_H_


#include "ap_def.h"
#include "common/event.h"


typedef struct Event_t_  Event_t;
typedef struct thread_t_ thread_t;

typedef struct thread_t_
{
  thread_t *p_thread;

  bool (*init)(void);
  bool (*update)(void);
  bool (*event)(Event_t *p_event);
} thread_t;


bool threadInit(void);
bool threadCreate(void *p_thread);
bool threadBegin(void);
bool threadUpdate(void);

#endif