#ifndef CAN_THREAD_H_
#define CAN_THREAD_H_


#include "ap_def.h"

typedef struct
{
  bool (*getTxUpdate)(void);
  bool (*getRxUpdate)(void);
} can_obj_t;


can_obj_t *canObj(void);

#endif