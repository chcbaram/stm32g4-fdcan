#include "led.h"

#include "thread.h"
#include "event.h"


static bool ledThreadinit(void);
static bool ledThreadupdate(void);


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

