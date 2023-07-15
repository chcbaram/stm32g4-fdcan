#include "cmd_thread.h"
#include "driver/cmd_uart.h"
#include "process/cmd_boot.h"



#define CMD_DRIVER_MAX_CH     2


static cmd_t        cmd[CMD_DRIVER_MAX_CH];
static cmd_driver_t cmd_drvier[CMD_DRIVER_MAX_CH];




bool cmdThreadInit(void)
{
  cmdUartInitDriver(&cmd_drvier[0], HW_UART_CH_DEBUG, 115200);
  cmdInit(&cmd[0], &cmd_drvier[0]);
  cmdOpen(&cmd[0]);

  cmdUartInitDriver(&cmd_drvier[1], HW_UART_CH_RS485, 115200);
  cmdInit(&cmd[1], &cmd_drvier[1]);
  cmdOpen(&cmd[1]);
  return true;
}

bool cmdThreadUpdate(void)
{
  bool rx_ret = false;

  for (int i=0; i<CMD_DRIVER_MAX_CH; i++)
  {
    if (cmd[i].is_init == true)
    {
      if (cmdReceivePacket(&cmd[i]) == true)
      {
        bool ret = true;
        ret &= cmdBootProcess(&cmd[i]);

        if (ret != true)
        {
          cmdSendResp(&cmd[i], cmd[i].packet.cmd, ERR_CMD_NO_CMD, NULL, 0);
        }

        rx_ret = true;
      }
    }
  }

  return rx_ret;
}
