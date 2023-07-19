#include "mode.h"

#include "thread.h"
#include "common/event.h"



static bool modeThreadInit(void);
static void modeThreadISR(void *arg);
static bool modeThreadUpdate(void);


static button_event_t btn_evt;
static ModeUsb_t mode;

static const char *mode_str[] = 
  {
    "MODE_USB_TO_CAN",
    "MODE_USB_TO_RS485",
    "MODE_USB_TO_CLI"
  };

__attribute__((section(".thread"))) 
static volatile thread_t thread_obj = 
  {
    .name = "mode",
    .is_enable = true,
    .init = modeThreadInit,
    .update = modeThreadUpdate
  };




bool modeThreadInit(void)
{
  uint8_t ee_data = 0;

  (void)thread_obj;
  eepromReadByte(HW_EEPROM_MODE, &ee_data);
  mode = (ModeUsb_t)constrain(ee_data, MODE_USB_TO_CAN, MODE_USB_TO_CLI);



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
  evt.data = (uint32_t)mode;
  eventPut(&evt);  

  logPrintf("[  ] %s\n", mode_str[mode]);

  return true;
}

bool modeThreadUpdate(void)
{
  bool ret;

  ret = buttonEventGetPressed(&btn_evt, HW_BUTTON_CH_S2);
  if (ret == true)
  {
    mode = (ModeUsb_t)(((int)mode + 1) % 3);

    eepromWriteByte(HW_EEPROM_MODE, (uint8_t)mode);

    event_t evt;

    evt.code = EVENT_MODE_CHANGE;
    evt.data = (uint32_t)mode;
    eventPut(&evt);    
  }

  return true;
}

void modeThreadISR(void *arg)
{
  switch(mode)
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
