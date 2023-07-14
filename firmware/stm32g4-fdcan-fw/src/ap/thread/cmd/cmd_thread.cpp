#include "cmd_thread.h"
#include "driver/cmd_uart.h"
#include "process/cmd_boot.h"




#define CMD_DRIVER_MAX_CH     1


static cmd_t        cmd[CMD_DRIVER_MAX_CH];
static cmd_driver_t cmd_drvier[CMD_DRIVER_MAX_CH];




bool cmdThreadInit(void)
{
  cmdUartInitDriver(&cmd_drvier[0], HW_UART_CH_USB, 1000000);  
  cmdInit(&cmd[0], &cmd_drvier[0]);
  cmdOpen(&cmd[0]);

  cmdBootInit();
  return true;
}

bool cmdThreadUpdate(void)
{
  for (int i=0; i<CMD_DRIVER_MAX_CH; i++)
  {
    if (cmd[i].is_init == true)
    {
      if (cmdReceivePacket(&cmd[i]) == true)
      {
        bool ret = false;
        ret |= cmdBootProcess(&cmd[i]);

        if (ret != true)
        {
          cmdSendResp(&cmd[i], cmd[i].packet.cmd, ERR_CMD_NO_CMD, NULL, 0);
        }
      }
      cmdBootUpdate(&cmd[i]);
    }
  }

  

  return true;
}
