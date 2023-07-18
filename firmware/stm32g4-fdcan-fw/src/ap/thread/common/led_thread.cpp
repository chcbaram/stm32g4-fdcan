#include "led_thread.h"





static bool ledThreadinit(void)
{
  return true;
}

static bool ledThreadupdate(void)
{
  static uint32_t pre_time;


  if (millis()-pre_time >= 500)
  {
    pre_time = millis();
    ledToggle(_DEF_LED1);
  }    
  return true;
}

led_thread_t *ledThread(void)
{
  static thread_t thread_obj = 
  {
    .p_thread = &thread_obj,
    .init = ledThreadinit,
    .update = ledThreadupdate
  };

  static led_thread_t led_thread = 
  {
    .p_thread = &thread_obj
  };

  return &led_thread;
}