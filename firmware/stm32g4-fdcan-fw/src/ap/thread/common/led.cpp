#include "led.h"

#include "thread.h"
#include "event.h"
#include "manage/can.h"
#include "manage/rs485.h"


static bool ledThreadinit(void);
static bool ledThreadupdate(void);
static void ledThreadISR(void *arg);

__attribute__((section(".thread"))) 
static volatile thread_t thread_obj = 
  {
    .name = "led",
    .is_enable = true,
    .init = ledThreadinit,
    .update = ledThreadupdate
  };





bool ledThreadinit(void)
{
  swtimer_handle_t timer_ch;
  timer_ch = swtimerGetHandle();
  if (timer_ch >= 0)
  {
    swtimerSet(timer_ch, 10, LOOP_TIME, ledThreadISR, NULL);
    swtimerStart(timer_ch);
  }
  else
  {
    logPrintf("[NG] ledThreadinit()\n     swtimerGetHandle()\n");
  }    
  return true;
}

bool ledThreadupdate(void)
{
  static uint32_t pre_time;


  if (millis()-pre_time >= 500)
  {
    pre_time = millis();
    ledToggle(_DEF_LED1);
  }    
  return true;
}

void ledThreadISR(void *arg)
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
  bool is_tx_update;
  bool is_rx_update;


  switch(state_tx)
  {
    case LED_IDLE:
      is_tx_update = canObj()->getTxUpdate() | rs485Obj()->getTxUpdate();
      if (is_tx_update)
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
      is_rx_update = canObj()->getRxUpdate() | rs485Obj()->getRxUpdate();
      if (is_rx_update)
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
