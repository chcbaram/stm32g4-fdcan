#ifndef CMD_THREAD_H_
#define CMD_THREAD_H_


#include "ap_def.h"


typedef struct cmd_process_t_
{
  CmdType_t cmd_type;
  const char *name;
  uint16_t cmd_code;
  bool (*process)(cmd_t *p_cmd);
} cmd_process_t;

typedef struct
{
  bool (*sendPacket)(CmdType_t type, uint16_t cmd_code, uint16_t err_code, uint8_t *p_data, uint32_t length);
  bool (*sendResp)(cmd_t *p_cmd, uint16_t err_code, uint8_t *p_data, uint32_t length);
} cmd_obj_t;

cmd_obj_t *cmdObj(void);

#endif