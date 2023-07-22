#include "rs485.h"

#include "thread.h"
#include "mode.h"
#include "common/event.h"


static bool rs485Threadinit(void);
static bool rs485Threadupdate(void);
static bool rs485ThreadEvent(event_t *p_event);
static void rs485ThreadISR(void *arg);
#ifdef _USE_HW_CLI
static void cliCmd(cli_args_t *args);
#endif

__attribute__((section(".thread"))) 
static volatile thread_t thread_obj = 
  {
    .name = "rs485",
    .is_enable = true,
    .init = rs485Threadinit,
    .update = rs485Threadupdate
  };

static bool is_enable = false;
static bool is_rx_update = false;
static bool is_tx_update = false;



bool rs485Threadinit(void)
{
  eventNodeAdd(rs485ThreadEvent);

  swtimer_handle_t timer_ch;
  timer_ch = swtimerGetHandle();
  if (timer_ch >= 0)
  {
    swtimerSet(timer_ch, 10, LOOP_TIME, rs485ThreadISR, NULL);
    swtimerStart(timer_ch);
  }
  else
  {
    logPrintf("[NG] rs485Threadinit()\n     swtimerGetHandle()\n");
  }  

#ifdef _USE_HW_CLI
  cliAdd("rs485", cliCmd);
#endif  
  return true;
}

bool rs485Threadupdate(void)
{
  if (modeObj()->getMode() == MODE_USB_TO_RS485 && modeObj()->getType() == TYPE_USB_UART)
  {
    is_enable = true;
  }
  else
  {
    is_enable = false;
  }

  if (is_enable == true)
  {
    uint32_t length;
    uint8_t buf[256];

    if (uartGetBaud(HW_UART_CH_USB) != uartGetBaud(HW_UART_CH_RS485))
    {
      uartOpen(HW_UART_CH_RS485, uartGetBaud(HW_UART_CH_USB));
      logPrintf("[  ] rs485 baud %d\n", uartGetBaud(HW_UART_CH_RS485));
    }

    // USB -> RS485
    //
    length = cmin(uartAvailable(HW_UART_CH_USB), 256);
    if (length > 0)
    {
      is_tx_update = true;
      for (uint32_t i=0; i<length; i++)
      {
        buf[i] = uartRead(HW_UART_CH_USB);
      }
      uartWrite(HW_UART_CH_RS485, buf, length);
    }    

    // RS485 -> USB
    //
    length = cmin(uartAvailable(HW_UART_CH_RS485), 256);
    if (length > 0)
    {
      is_rx_update = true;
      for (uint32_t i=0; i<length; i++)
      {
        buf[i] = uartRead(HW_UART_CH_RS485);
      }
      uartWrite(HW_UART_CH_USB, buf, length);
    }      
  }
  else
  {
    while(uartAvailable(HW_UART_CH_RS485))
    {
      uartRead(HW_UART_CH_RS485);
    }
  }
  return true;
}

bool rs485ThreadEvent(event_t *p_event)
{
  return true;
}

void rs485ThreadISR(void *arg)
{
  enum 
  {
    LED_IDLE, 
    LED_ON,
    LED_OFF,
  };
  static uint8_t  state_rx = LED_IDLE;
  static uint32_t pre_time_rx;
  static uint8_t  state_tx = LED_IDLE;
  static uint32_t pre_time_tx;

  switch(state_tx)
  {
    case LED_IDLE:
      if (is_enable && is_tx_update)
      {
        is_tx_update = false;
        state_tx = LED_ON;
        ledOn(HW_LED_CH_TX);
        pre_time_tx = millis();
      }
      break;
    case LED_ON:
      if (millis()-pre_time_tx >= 50)
      {
        state_tx = LED_OFF;
        ledOff(HW_LED_CH_TX);
        pre_time_tx = millis();
      }
      break;
    case LED_OFF:
      if (millis()-pre_time_tx >= 50)
      {
        state_tx = LED_IDLE;
        pre_time_tx = millis();
      }
      break;
  }
  switch(state_rx)
  {
    case LED_IDLE:
      if (is_enable && is_rx_update)
      {
        is_rx_update = false;
        state_rx = LED_ON;
        ledOn(HW_LED_CH_RX);
        pre_time_rx = millis();
      }
      break;
    case LED_ON:
      if (millis()-pre_time_rx >= 50)
      {
        state_rx = LED_OFF;
        ledOff(HW_LED_CH_RX);
        pre_time_rx = millis();
      }
      break;
    case LED_OFF:
      if (millis()-pre_time_rx >= 50)
      {
        state_rx = LED_IDLE;
        pre_time_rx = millis();
      }
      break;
  }
}

#ifdef _USE_HW_CLI
void cliCmd(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliPrintf("rs485 enable : %s\n", is_enable ? "True":"False");
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("rs485 info\n");
  }
}
#endif