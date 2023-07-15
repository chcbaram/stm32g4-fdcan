#ifndef BUTTON_H_
#define BUTTON_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_BUTTON

#define BUTTON_MAX_CH       HW_BUTTON_MAX_CH


typedef struct
{
  bool is_init;
  uint8_t level;
  uint8_t index;

  bool pressed_event[BUTTON_MAX_CH];
  bool released_event[BUTTON_MAX_CH];
  bool repeat_event[BUTTON_MAX_CH];
} button_event_t;


bool     buttonInit(void);
bool     buttonGetPressed(uint8_t ch);
uint32_t buttonGetData(void);

uint32_t buttonGetPressedTime(uint8_t ch);
uint8_t  buttonGetPressedCount(void);

bool     buttonGetReleased(uint8_t ch);
uint32_t buttonGetReleasedTime(uint8_t ch);

void     buttonSetRepeatTime(uint8_t ch, uint32_t detect_ms, uint32_t repeat_delay_ms, uint32_t repeat_ms);
uint32_t buttonGetRepeatCount(uint8_t ch);

const char *buttonGetName(uint8_t ch);

bool     buttonEventInit(button_event_t *p_event, uint8_t level);
bool     buttonEventClear(button_event_t *p_event);
bool     buttonEventGetPressed(button_event_t *p_event, uint8_t ch);
bool     buttonEventGetReleased(button_event_t *p_event, uint8_t ch);
uint32_t buttonEventGetRepeat(button_event_t *p_event, uint8_t ch);


#endif

#ifdef __cplusplus
}
#endif



#endif 