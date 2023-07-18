#include "thread.h"
#include "common/event.h"


#define THREAD_MAX      8


typedef struct
{
  int32_t count;

  thread_t *p_thread[THREAD_MAX];
  bool is_begin;

} thread_info_t;


static thread_info_t info;




bool threadInit(void)
{
  info.count = 0;
  info.is_begin = false;

  for (int i=0; i<THREAD_MAX; i++)
  {
    info.p_thread[i] = NULL;
  }

  eventInit();
  return true;
}

bool threadBegin(void)
{
  bool ret = true;

  for (int i=0; i<info.count; i++)
  {
    ret &= info.p_thread[i]->init();
  }
  info.is_begin = true;

  return ret;
}

bool threadCreate(void *p_thread)
{
  bool ret;

  if (info.count < THREAD_MAX)
  {
    info.p_thread[info.count] = ((thread_t *)p_thread)->p_thread;
    info.count++;
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool threadUpdate(void)
{
  bool ret = true;

  if (info.is_begin != true)
    return false;


  while(eventAvailble())
  {
    Event_t evt;

    if (eventGet(&evt) == true)
    {
      for (int i=0; i<info.count; i++)
      {
        if (info.p_thread[i]->event != NULL)
        {
          info.p_thread[i]->event(&evt);
        }
      }
    }
  }

  for (int i=0; i<info.count; i++)
  {
    ret &= info.p_thread[i]->update();
  }
  return ret;
}