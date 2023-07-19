#include "rs485.h"

#include "thread.h"
#include "common/event.h"


static bool rs485Threadinit(void);
static bool rs485Threadupdate(void);

__attribute__((section(".thread"))) 
static volatile thread_t thread_obj = 
  {
    .name = "rs485",
    .is_enable = true,
    .init = rs485Threadinit,
    .update = rs485Threadupdate
  };





bool rs485Threadinit(void)
{
  return true;
}

bool rs485Threadupdate(void)
{
  return true;
}
