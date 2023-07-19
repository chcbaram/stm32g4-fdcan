#include "can.h"

#include "thread.h"
#include "common/event.h"


static bool canThreadinit(void);
static bool canThreadupdate(void);


__attribute__((section(".thread"))) 
static volatile thread_t thread_obj = 
  {
    .name = "can",
    .is_enable = true,
    .init = canThreadinit,
    .update = canThreadupdate
  };



bool canThreadinit(void)
{
  (void)thread_obj;

  return true;
}

bool canThreadupdate(void)
{
  return true;
}

