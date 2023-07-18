#include "cli_thread.h"





bool cliThreadInit(void)
{
  #ifdef _USE_HW_CLI
  cliOpen(_DEF_UART1, 115200);
  #endif  
  return true;
}

bool cliThreadEvent(Event_t *p_event)
{
  logPrintf("Event %X:%d\n", p_event->code, p_event->data);
  return true;
}

bool cliThreadUpdate(void)
{

  #ifdef _USE_HW_CLI
  uint8_t cli_ch;


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
  #endif

  return true;
}

cli_thread_t *cliThread(void)
{
  static thread_t thread_obj = 
  {
    .p_thread = &thread_obj,
    .init = cliThreadInit,
    .update = cliThreadUpdate,
    .event = cliThreadEvent,
  };

  static cli_thread_t cli_thread = 
  {
    .p_thread = &thread_obj
  };

  return &cli_thread;
}