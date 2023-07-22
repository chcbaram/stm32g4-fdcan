#ifndef MODE_THREAD_H_
#define MODE_THREAD_H_


#include "ap_def.h"


typedef enum
{
  MODE_USB_TO_CAN,
  MODE_USB_TO_RS485,
  MODE_USB_TO_CLI,
} ModeUsbMode_t;

typedef enum
{
  TYPE_USB_PACKET,
  TYPE_USB_UART,
} ModeUsbType_t;


typedef struct
{
  ModeUsbMode_t (*getMode)(void);
  ModeUsbType_t (*getType)(void);
} mode_obj_t;


mode_obj_t *modeObj(void);

#endif