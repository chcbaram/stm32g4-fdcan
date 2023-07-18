#include "event.h"
#include "qbuffer.h"


#define EVENT_Q_MAX       8


static bool is_init = false;
static qbuffer_t event_q;
static Event_t event_buf[EVENT_Q_MAX];




bool eventInit(void)
{
  is_init = qbufferCreateBySize(&event_q, (uint8_t *)event_buf, sizeof(Event_t), EVENT_Q_MAX);
  return is_init;
}

bool eventAvailble(void)
{
  if (is_init != true)
    return false;

  if (qbufferAvailable(&event_q) > 0)
    return true;
  else
    return false;
}

bool eventPut(Event_t *p_event)
{
  bool ret;

  if (is_init != true)
    return false;

  ret = qbufferWrite(&event_q, (uint8_t *)p_event, 1);

  return ret;
}

bool eventGet(Event_t *p_event)
{
  bool ret;

  if (is_init != true)
    return false;

  ret = qbufferRead(&event_q, (uint8_t *)p_event, 1);

  return ret;
}

  