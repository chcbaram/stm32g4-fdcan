#include "event.h"
#include "qbuffer.h"


#define EVENT_Q_MAX       AP_EVENT_Q_MAX
#define EVENT_NODE_MAX    AP_EVENT_NODE_MAX


typedef struct
{
  int32_t count;
  bool (*event_func[EVENT_NODE_MAX])(event_t *p_event);
} event_node_t;


static bool eventGet(event_t *p_event);
static bool eventAvailble(void);

const char *event_str[] = 
  {
    "EVENT_MODE_CHANGE",
    "EVENT_USB_OPEN",
  };

static bool is_init = false;
static bool is_log = true;
static qbuffer_t event_q;
static event_t event_buf[EVENT_Q_MAX];
static event_node_t event_node;





bool eventInit(void)
{
  event_node.count = 0;
  for (int i=0; i<EVENT_NODE_MAX; i++)
  {
    event_node.event_func[i] = NULL;
  }
  is_init = qbufferCreateBySize(&event_q, (uint8_t *)event_buf, sizeof(event_t), EVENT_Q_MAX);
  return is_init;
}

bool eventNodeAdd(bool (*func)(event_t *p_event))
{
  bool ret;

  if (event_node.count < EVENT_NODE_MAX)
  {
    event_node.event_func[event_node.count] = func;
    event_node.count++;
    ret = true;
  }
  else
  {
    ret = false;
  }

  return ret;  
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

bool eventPut(event_t *p_event)
{
  bool ret;

  if (is_init != true)
    return false;

  ret = qbufferWrite(&event_q, (uint8_t *)p_event, 1);

  return ret;
}

bool eventGet(event_t *p_event)
{
  bool ret;

  if (is_init != true)
    return false;

  ret = qbufferRead(&event_q, (uint8_t *)p_event, 1);

  return ret;
}

bool eventUpdate(void)
{

  if (is_init != true)
    return false;


  while(eventAvailble())
  {
    event_t evt;

    if (eventGet(&evt) == true)
    {
      if (is_log == true)
      {
        logPrintf("[  ] Event %s:%d\n", event_str[evt.code], evt.data);
      }

      for (int i=0; i<event_node.count; i++)
      {
        if (event_node.event_func[i] != NULL)
        {
          event_node.event_func[i](&evt);
        }
      }
    }
  }

  return true;
}  