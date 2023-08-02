#include "cmd_thread.h"

#include "thread.h"
#include "manage/mode.h"
#include "driver/cmd_uart.h"
#include "process/cmd_boot.h"




#define CMD_DRIVER_MAX_CH     1


typedef struct
{
  int32_t count;
  cmd_process_t *p_process;
} cmd_process_info_t;

static bool cmdThreadInit(void);
static bool cmdThreadUpdate(void);

static cmd_t        cmd[CMD_DRIVER_MAX_CH];
static cmd_driver_t cmd_drvier[CMD_DRIVER_MAX_CH];

static cmd_process_info_t cmd_process_info;
extern uint32_t   _scmd_process;
extern uint32_t   _ecmd_process;

__attribute__((section(".thread"))) 
static volatile thread_t thread_obj = 
  {
    .name = "cmd",
    .is_enable = true,
    .init = cmdThreadInit,
    .update = cmdThreadUpdate
  };




bool cmdThreadInit(void)
{
  (void)thread_obj;

  cmd_process_info.count = ((int)&_ecmd_process - (int)&_scmd_process)/sizeof(cmd_process_t);
  cmd_process_info.p_process = (cmd_process_t *)&_scmd_process;

  logPrintf("[ ] cmdThreadInit()\n");
  logPrintf("    count : %d\n", cmd_process_info.count);
  for (int i=0; i<cmd_process_info.count; i++)
  {
    logPrintf("    %d %s\n", i, cmd_process_info.p_process[i].name);
  }

  cmdUartInitDriver(&cmd_drvier[0], HW_UART_CH_USB, 1000000);  
  cmdInit(&cmd[0], &cmd_drvier[0]);
  cmdOpen(&cmd[0]);

  cmdBootInit();  
  return true;
}

bool cmdThreadUpdate(void)
{

  if (modeObj()->getType() != TYPE_USB_PACKET)
  {
    return false;
  }

  for (int i=0; i<CMD_DRIVER_MAX_CH; i++)
  {
    if (cmd[i].is_init == true)
    {
      if (cmdReceivePacket(&cmd[i]) == true)
      {
        bool ret = false;
        ret |= cmdBootProcess(&cmd[i]);

        for (int cnt=0; cnt<cmd_process_info.count; cnt++)
        {
          cmd_process_t *p_process = &cmd_process_info.p_process[cnt];

          if (p_process->cmd_code == cmd[i].packet.cmd)
          {
            if (p_process->cmd_type == cmd[i].packet.type)
            {
              ret |= p_process->process(&cmd[i]);
            }
          }
        }

        if (ret != true && cmd[i].packet.type == PKT_TYPE_CMD)
        {
          cmdSendResp(&cmd[i], cmd[i].packet.cmd, ERR_CMD_NO_CMD, NULL, 0);
        }
      }
      cmdBootUpdate(&cmd[i]);
    }
  }

  return true;
}

static bool sendPacket(CmdType_t type, uint16_t cmd_code, uint16_t err_code, uint8_t *p_data, uint32_t length)
{
  return cmdSend(&cmd[0], type, cmd_code, err_code, p_data, length);
}

static bool sendResp(cmd_t *p_cmd, uint16_t err_code, uint8_t *p_data, uint32_t length)
{
  return cmdSendResp(p_cmd, p_cmd->packet.cmd, err_code, p_data, length);
}

cmd_obj_t *cmdObj(void)
{
  static cmd_obj_t cmd_obj = 
  {
    .sendPacket = sendPacket,
    .sendResp = sendResp
  };

  return &cmd_obj;
}