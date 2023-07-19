#include "cli.h"

#include "thread.h"
#include "common/event.h"
#include "mode/mode.h"


static bool cliThreadInit(void);
static bool cliThreadUpdate(void);
static bool cliThreadEvent(event_t *p_event);


static bool is_mode_update = false;
static ModeUsb_t mode = MODE_USB_TO_CAN;

__attribute__((section(".thread"))) 
static volatile thread_t thread_obj = 
  {
    .name = "cli",
    .is_enable = true,
    .init = cliThreadInit,
    .update = cliThreadUpdate,
  };




bool cliThreadInit(void)
{
  (void)thread_obj;

  #ifdef _USE_HW_CLI
  cliOpen(_DEF_UART1, 115200);
  #endif  

  eventNodeAdd(cliThreadEvent);
  return true;
}

bool cliThreadEvent(event_t *p_event)
{
  if (p_event->code == EVENT_MODE_CHANGE)
  {
    is_mode_update = true;
    mode = (ModeUsb_t)p_event->data;
  }
  return true;
}

bool cliThreadUpdate(void)
{  
  #ifdef _USE_HW_CLI
  uint8_t cli_ch;


  switch(mode)
  {
    case MODE_USB_TO_CLI:
      if (usbIsOpen() && usbGetType() == USB_CON_CLI)
      {
        cli_ch = HW_UART_CH_USB;
      }
      else
      {
        cli_ch = HW_UART_CH_DEBUG;
      }
      if (cli_ch != cliGetPort())
      {
        cliOpen(cli_ch, 0);
      }
      cliMain();
      break;
    
    case MODE_USB_TO_RS485:
    case MODE_USB_TO_CAN:
      if (cliGetPort() != HW_UART_CH_DEBUG)
      {
        cliOpen(HW_UART_CH_DEBUG, 0);
      }
      cliMain();
      break;
  }
  #endif

  return true;
}

