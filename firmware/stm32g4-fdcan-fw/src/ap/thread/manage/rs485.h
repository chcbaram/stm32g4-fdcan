#ifndef RS485_THREAD_H_
#define RS485_THREAD_H_


#include "ap_def.h"


typedef struct
{
  bool (*getTxUpdate)(void);
  bool (*getRxUpdate)(void);
} rs485_obj_t;


rs485_obj_t *rs485Obj(void);

#endif