#include "mode.h"

#include "thread.h"
#include "common/event.h"



static bool modeThreadInit(void);
static void modeThreadISR(void *arg);
static bool modeThreadUpdate(void);
#ifdef _USE_HW_CLI
static void cliMode(cli_args_t *args);
#endif

__attribute__((section(".thread"))) 
static volatile thread_t thread_obj = 
  {
    .name = "mode",
    .is_enable = true,
    .init = modeThreadInit,
    .update = modeThreadUpdate
  };

static button_event_t btn_evt;
static ModeUsbMode_t usb_mode;
static ModeUsbType_t usb_type;
static ModeUsbMode_t usb_mode_pre;
static ModeUsbType_t usb_type_pre;

static bool is_usb_open = false;
static const char *usb_mode_str[] = 
  {
    "MODE_USB_TO_CAN",
    "MODE_USB_TO_RS485",
    "MODE_USB_TO_CLI"
  };
static const char *usb_type_str[] = 
  {
    "TYPE_USB_PACKET",
    "TYPE_USB_UART",
  };




bool modeThreadInit(void)
{
  uint8_t ee_data = 0;

  (void)thread_obj;
  eepromReadByte(HW_EEPROM_MODE, &ee_data);
  usb_mode = (ModeUsbMode_t)constrain(ee_data, MODE_USB_TO_CAN, MODE_USB_TO_CLI);
  usb_type = TYPE_USB_PACKET;


  buttonEventInit(&btn_evt, 5);

  swtimer_handle_t timer_ch;
  timer_ch = swtimerGetHandle();
  if (timer_ch >= 0)
  {
    swtimerSet(timer_ch, 10, LOOP_TIME, modeThreadISR, NULL);
    swtimerStart(timer_ch);
  }
  else
  {
    logPrintf("[NG] modeThreadInit()\n     swtimerGetHandle()\n");
  }  

  event_t evt;
  evt.code = EVENT_MODE_CHANGE;
  evt.data = (uint32_t)usb_mode;
  eventPut(&evt);  

  is_usb_open = usbIsOpen();
  evt.code = EVENT_USB_OPEN;
  evt.data = (uint32_t)is_usb_open;
  eventPut(&evt); 

  logPrintf("[  ] Mode : %s\n", usb_mode_str[usb_mode]);
  logPrintf("[  ] Type : %s\n", usb_type_str[usb_type]);

  usb_mode_pre = usb_mode;
  usb_type_pre = usb_type;

#ifdef _USE_HW_CLI
  cliAdd("mode", cliMode);
#endif
  return true;
}

bool modeThreadUpdate(void)
{
  bool ret;
  event_t evt;


  // Mode Button Process
  //
  ret = buttonEventGetPressed(&btn_evt, HW_BUTTON_CH_S2);
  if (ret == true)
  {
    usb_mode = (ModeUsbMode_t)(((int)usb_mode + 1) % 3);

    eepromWriteByte(HW_EEPROM_MODE, (uint8_t)usb_mode);


    evt.code = EVENT_MODE_CHANGE;
    evt.data = (uint32_t)usb_mode;
    eventPut(&evt);    
  }

  // USB Connect Process
  //
  bool is_open;

  is_open = usbIsOpen();
  if (is_usb_open != is_open)
  {
    is_usb_open = is_open;
    evt.code = EVENT_USB_OPEN;
    evt.data = (uint32_t)is_usb_open;
    eventPut(&evt);  
  }

  // USB Type Process
  //
  switch(usb_mode)
  {
    case MODE_USB_TO_CAN:
      usb_type = TYPE_USB_PACKET;
      break;

    case MODE_USB_TO_RS485:
      if (uartGetBaud(HW_UART_CH_USB) == 600)
        usb_type = TYPE_USB_PACKET;
      else
        usb_type = TYPE_USB_UART;

    case MODE_USB_TO_CLI:
      if (usbIsOpen() == true)
      {
        if (uartGetBaud(HW_UART_CH_USB) == 115200)
          usb_type = TYPE_USB_UART;
        else
          usb_type = TYPE_USB_PACKET;
      }
      else
      {
        usb_type = TYPE_USB_PACKET;
      }
      break;

  }


  if (usb_mode != usb_mode_pre)
  {
    logPrintf("[  ] Mode : %s\n", usb_mode_str[usb_mode]);
  }
  if (usb_type != usb_type_pre)
  {
    logPrintf("[  ] Type : %s\n", usb_type_str[usb_type]);
  }
  usb_mode_pre = usb_mode;
  usb_type_pre = usb_type;  
  return true;
}

void modeThreadISR(void *arg)
{
  switch(usb_mode)
  {
    case MODE_USB_TO_CAN:
      ledOn(HW_LED_CH_CAN);
      ledOff(HW_LED_CH_RS485);
      break;

    case MODE_USB_TO_RS485:
      ledOff(HW_LED_CH_CAN);
      ledOn(HW_LED_CH_RS485);
      break;

    case MODE_USB_TO_CLI:
      ledOff(HW_LED_CH_CAN);
      ledOff(HW_LED_CH_RS485);
      break;
  }
}


static ModeUsbMode_t getMode(void)
{
  return usb_mode;
}

static ModeUsbType_t getType(void)
{
  return usb_type;
}

mode_obj_t *modeObj(void)
{
  static mode_obj_t mode_obj = 
  {
    .getMode = getMode,
    .getType = getType
  };

  return &mode_obj;
}

#ifdef _USE_HW_CLI
void cliMode(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliPrintf("Mode : %s\n", usb_mode_str[usb_mode]);
    cliPrintf("Type : %s\n", usb_type_str[usb_type]);
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("mode info\n");
  }
}
#endif

