#include "can.h"

#include "thread.h"
#include "mode.h"
#include "common/event.h"
#include "cmd/cmd_thread.h"


static bool canThreadinit(void);
static bool canThreadupdate(void);
static bool canCmdOpen(cmd_t *p_cmd);
static bool canCmdClose(cmd_t *p_cmd);
static bool canCmdData(cmd_t *p_cmd);

#ifdef _USE_HW_CLI
static void cliCmd(cli_args_t *args);
#endif

static bool is_enable = false;
static bool is_open = false;
static bool is_tx_update = false;
static bool is_rx_update = false;
static uint8_t can_ch = _DEF_CAN1;

__attribute__((section(".thread"))) 
static volatile thread_t thread_obj = 
  {
    .name = "can",
    .is_enable = true,
    .init = canThreadinit,
    .update = canThreadupdate
  };

__attribute__((section(".cmd_process"))) 
static volatile cmd_process_t cmd_obj[] = 
  {
    {PKT_TYPE_CMD,  "CMD_CAN_OPEN",  CMD_CAN_OPEN,  canCmdOpen},
    {PKT_TYPE_CMD,  "CMD_CAN_CLOSE", CMD_CAN_CLOSE, canCmdClose},
    {PKT_TYPE_CAN,  "CMD_CAN_DATA",  CMD_CAN_DATA,  canCmdData}
  };





bool canThreadinit(void)
{
  (void)thread_obj;

#ifdef _USE_HW_CLI
  cliAdd("canbus", cliCmd);
#endif 
  return true;
}

bool canThreadupdate(void)
{
  if (modeObj()->getMode() == MODE_USB_TO_CLI && modeObj()->getType() == TYPE_USB_UART)
    is_enable = false;
  else
    is_enable = true;

  if (is_enable == true)
  {
    canUpdate();
    
    if (canMsgAvailable(can_ch))
    {
      can_msg_t msg;
      canMsgRead(can_ch, &msg);

      cmdObj()->sendPacket(PKT_TYPE_CAN, CMD_CAN_DATA, OK, (uint8_t *)&msg, sizeof(msg)); 
    }
  }
  if (is_open == true && usbIsOpen() == false)
  {
    is_open = false;
    logPrintf("[  ] canCmdClose()\n");
  }
  return true;
}

bool canCmdOpen(cmd_t *p_cmd)
{
  CanMode_t mode;
  CanFrame_t frame;
  CanBaud_t baud;
  CanBaud_t baud_data;

  mode      = (CanMode_t) p_cmd->packet.data[0];
  frame     = (CanFrame_t)p_cmd->packet.data[1];
  baud      = (CanBaud_t) p_cmd->packet.data[2];
  baud_data = (CanBaud_t) p_cmd->packet.data[3];

  logPrintf("[  ] canCmdOpen()\n");
  logPrintf("     mode      : %d\n", mode);
  logPrintf("     frame     : %d\n", frame);
  logPrintf("     baud      : %d\n", baud);
  logPrintf("     baud_data : %d\n", baud_data);

  if (canIsOpen(can_ch) == true)
  {
    canClose(can_ch);
  }

  is_open = canOpen(can_ch, mode, frame, baud, baud_data);
  logPrintf("     is_open   : %s\n", is_open ? "True":"False");

 
  cmdObj()->sendResp(p_cmd, OK, NULL, 0);
  return true;
}

bool canCmdClose(cmd_t *p_cmd)
{
  logPrintf("[  ] canCmdClose()\n");
  is_open = false;

  cmdObj()->sendResp(p_cmd, OK, NULL, 0);
  return true;
}

bool canCmdData(cmd_t *p_cmd)
{
  // USB -> CAN
  //
  // is_tx_update = true;
  // uartWrite(HW_UART_CH_RS485, p_cmd->packet.data, p_cmd->packet.length);
  return true;
}

static bool getTxUpdate(void)
{
  bool ret;

  ret = is_tx_update;
  is_tx_update = false;

  return ret;
}

static bool getRxUpdate(void)
{
  bool ret;

  ret = is_rx_update;
  is_rx_update = false;

  return ret;
}

can_obj_t *canObj(void)
{
  static can_obj_t obj = 
  {
    .getTxUpdate = getTxUpdate,
    .getRxUpdate = getRxUpdate
  };

  return &obj;
}

#ifdef _USE_HW_CLI
void cliCmd(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliPrintf("enable  : %s\n", is_enable ? "True":"False");
    cliPrintf("is_open : %s\n", is_open ? "True":"False");
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("canbus info\n");
  }
}
#endif
