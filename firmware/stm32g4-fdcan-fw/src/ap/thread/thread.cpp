#include "thread.h"
#include "common/event.h"


#define THREAD_MAX      8


typedef struct
{
  int32_t count;

  thread_t *p_thread;
  bool is_begin;

} thread_info_t;


static bool threadBegin(void);


static thread_info_t info;
extern uint32_t _sthread;
extern uint32_t _ethread;





bool threadInit(void)
{
  info.is_begin = false;

  info.count = ((int)&_ethread - (int)&_sthread)/sizeof(thread_t);
  info.p_thread = (thread_t *)&_sthread;

  logPrintf("[ ] threadInit()\n");
  logPrintf("    count : %d\n", info.count);
  
  for (int i=0; i<info.count; i++)
  {
    logPrintf("    %d %s\n", i, info.p_thread[i].name);
  }

  eventInit();
  threadBegin();

  return true;
}

bool threadBegin(void)
{
  bool ret = true;

  for (int i=0; i<info.count; i++)
  {
    if (info.p_thread[i].is_enable)
      ret &= info.p_thread[i].init();
  }
  info.is_begin = ret;

  logPrintf("[%s] threadBegin()\n", ret ? "OK":"NG");
  return ret;
}

bool threadUpdate(void)
{
  bool ret = true;

  if (info.is_begin != true)
    return false;

  eventUpdate();

  for (int i=0; i<info.count; i++)
  {
    if (info.p_thread[i].is_enable)
      ret &= info.p_thread[i].update();
  }
  return ret;
}