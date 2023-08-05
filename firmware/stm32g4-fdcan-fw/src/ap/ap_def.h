#ifndef AP_DEF_H_
#define AP_DEF_H_


#include "hw.h"



typedef struct thread_t_      thread_t;
typedef struct cmd_process_t_ cmd_process_t;


typedef enum
{
  EVENT_MODE_CHANGE,
  EVENT_USB_OPEN,
  EVENT_MAX,
} EventCode_t;



#define AP_EVENT_NODE_MAX           16        
#define AP_EVENT_Q_MAX              8


#define CMD_RS485_OPEN              0x0100
#define CMD_RS485_CLOSE             0x0101
#define CMD_RS485_DATA              0x0102

#define CMD_CAN_OPEN                0x0110
#define CMD_CAN_CLOSE               0x0111
#define CMD_CAN_DATA                0x0112
#define CMD_CAN_ERR_INFO            0x0113
#define CMD_CAN_SET_FILTER          0x0114
#define CMD_CAN_GET_FILTER          0x0115

#endif