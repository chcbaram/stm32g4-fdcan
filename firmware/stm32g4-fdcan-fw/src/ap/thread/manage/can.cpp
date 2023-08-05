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
static bool canCmdSetFilter(cmd_t *p_cmd);
static bool canCmdGetFilter(cmd_t *p_cmd);

#ifdef _USE_HW_CLI
static void cliCmd(cli_args_t *args);
#endif

static bool is_enable = false;
static bool is_open = false;
static bool is_tx_update = false;
static bool is_rx_update = false;
static uint8_t can_ch = _DEF_CAN1;
static CanFilterType_t can_filter_type    = CAN_ID_MASK;
static CanIdType_t     can_filter_id_type = CAN_STD;
static uint32_t        can_filter_id1     = 0x00000000;
static uint32_t        can_filter_id2     = 0x00000000;

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
    {PKT_TYPE_CMD,  "CMD_CAN_OPEN",       CMD_CAN_OPEN,       canCmdOpen},
    {PKT_TYPE_CMD,  "CMD_CAN_CLOSE",      CMD_CAN_CLOSE,      canCmdClose},
    {PKT_TYPE_CMD,  "CMD_CAN_SET_FILTER", CMD_CAN_SET_FILTER, canCmdSetFilter},
    {PKT_TYPE_CMD,  "CMD_CAN_GET_FILTER", CMD_CAN_GET_FILTER, canCmdGetFilter},
    {PKT_TYPE_CAN,  "CMD_CAN_DATA",       CMD_CAN_DATA,       canCmdData}
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
      uint8_t buf[256];
      data_t stamp;
      uint8_t index;

      canMsgRead(can_ch, &msg);

      is_rx_update = true;
      stamp.u32D = millis();

      index = 0;
      buf[index++] = stamp.u8Data[0];
      buf[index++] = stamp.u8Data[1];
      buf[index++] = (uint8_t)msg.frame;
      buf[index++] = (uint8_t)msg.id_type;
      buf[index++] = (uint8_t)msg.dlc;
      buf[index++] = (uint8_t)(msg.id >> 0);
      buf[index++] = (uint8_t)(msg.id >> 8);
      buf[index++] = (uint8_t)(msg.id >> 16);
      buf[index++] = (uint8_t)(msg.id >> 24);
      buf[index++] = (uint8_t)(msg.length);
      for (int i=0; i<msg.length; i++)
        buf[index++] = msg.data[i];

      cmdObj()->sendPacket(PKT_TYPE_CAN, CMD_CAN_DATA, OK, buf, index); 
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

  canSetFilterType(can_filter_type);
  canConfigFilter(can_ch, 0, can_filter_id_type, can_filter_id1, can_filter_id2);

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
  can_msg_t msg;

  if (is_open != true)
    return false;


  // USB -> CAN
  //
  is_tx_update = true;

  
  CanFrame_t  frame;
  CanIdType_t id_type;
  CanDlc_t    dlc;
  data_t      id;
  data_t      timestamp;
  uint8_t     length;
    
  (void)timestamp;
  (void)length;

  timestamp.u8Data[0] = p_cmd->packet.data[0];
  timestamp.u8Data[1] = p_cmd->packet.data[1];

  frame   = (CanFrame_t )p_cmd->packet.data[2];
  id_type = (CanIdType_t)p_cmd->packet.data[3];
  dlc     = (CanDlc_t   )p_cmd->packet.data[4];
  
  id.u8Data[0] = p_cmd->packet.data[5];
  id.u8Data[1] = p_cmd->packet.data[6];
  id.u8Data[2] = p_cmd->packet.data[7];
  id.u8Data[3] = p_cmd->packet.data[8];

  length = p_cmd->packet.data[9];

  canMsgInit(&msg, frame, id_type, dlc);
  for (int i=0; i<msg.length; i++)
    msg.data[i] = p_cmd->packet.data[10+i];

  msg.id = id.u32D;

  // logPrintf("id    0x%X\n", msg.id);
  // logPrintf("frame   %d\n", msg.frame);
  // logPrintf("id_type %d\n", msg.id_type);
  // logPrintf("dlc     %d\n", msg.dlc);

  canMsgWrite(can_ch, &msg, 10);

  return true;
}

bool canCmdSetFilter(cmd_t *p_cmd)
{
  if (is_open != true)
    return false;

  can_filter_type    = (CanFilterType_t)p_cmd->packet.data[0];
  can_filter_id_type = (CanIdType_t)p_cmd->packet.data[1];

  memcpy(&can_filter_id1, &p_cmd->packet.data[2], 4);
  memcpy(&can_filter_id2, &p_cmd->packet.data[6], 4);

  // logPrintf("type    : %d\n", can_filter_type);
  // logPrintf("id_type : %d\n", can_filter_id_type);
  // logPrintf("id1     : 0x%X\n", can_filter_id1);
  // logPrintf("id2     : 0x%X\n", can_filter_id2);
  
  canSetFilterType(can_filter_type);
  canConfigFilter(can_ch, 0, can_filter_id_type, can_filter_id1, can_filter_id2);

  cmdObj()->sendResp(p_cmd, OK, NULL, 0);
  return true;    
}

bool canCmdGetFilter(cmd_t *p_cmd)
{
  uint8_t buf[64];

  if (is_open != true)
    return false;

  buf[0] = (uint8_t)can_filter_type;
  buf[1] = (uint8_t)can_filter_id_type;

  memcpy(&buf[2], &can_filter_id1, 4);
  memcpy(&buf[6], &can_filter_id2, 4);

  cmdObj()->sendResp(p_cmd, OK, buf, 10);
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
