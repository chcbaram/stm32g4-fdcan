#include "rs485.h"

#include "thread.h"
#include "mode.h"
#include "common/event.h"
#include "cmd/cmd_thread.h"


static bool rs485Threadinit(void);
static bool rs485Threadupdate(void);
static bool rs485ThreadEvent(event_t *p_event);
static void rs485ThreadUpdateUart(void);
static void rs485ThreadUpdatePacket(void);
static bool rs485CmdOpen(cmd_t *p_cmd);
static bool rs485CmdClose(cmd_t *p_cmd);
static bool rs485CmdData(cmd_t *p_cmd);

#ifdef _USE_HW_CLI
static void cliCmd(cli_args_t *args);
#endif

__attribute__((section(".thread"))) 
static volatile thread_t thread_obj = 
  {
    .name = "rs485",
    .is_enable = true,
    .init = rs485Threadinit,
    .update = rs485Threadupdate
  };

__attribute__((section(".cmd_process"))) 
static volatile cmd_process_t cmd_obj[] = 
  {
    {PKT_TYPE_CMD,  "CMD_RS485_OPEN",  CMD_RS485_OPEN,  rs485CmdOpen},
    {PKT_TYPE_CMD,  "CMD_RS485_CLOSE", CMD_RS485_CLOSE, rs485CmdClose},
    {PKT_TYPE_UART, "CMD_RS485_DATA",  CMD_RS485_DATA,  rs485CmdData}
  };


static bool is_enable = false;
static bool is_rx_update = false;
static bool is_tx_update = false;
static bool is_packet_open = false;
static uint32_t packet_baud = 19200;



bool rs485Threadinit(void)
{
  eventNodeAdd(rs485ThreadEvent);

#ifdef _USE_HW_CLI
  cliAdd("rs485", cliCmd);
#endif  
  return true;
}

bool rs485Threadupdate(void)
{
  if (modeObj()->getMode() == MODE_USB_TO_CLI && modeObj()->getType() == TYPE_USB_UART)
    is_enable = false;
  else
    is_enable = true;

  if (is_enable == true)
  {
    if (modeObj()->getType() == TYPE_USB_UART)
      rs485ThreadUpdateUart();
    else
      rs485ThreadUpdatePacket();
  }
  else
  {
    while(uartAvailable(HW_UART_CH_RS485))
    {
      uartRead(HW_UART_CH_RS485);
    }
  }
  return true;
}

void rs485ThreadUpdateUart(void)
{
  uint32_t length;
  uint8_t buf[256];

  if (uartGetBaud(HW_UART_CH_USB) != uartGetBaud(HW_UART_CH_RS485))
  {
    uartOpen(HW_UART_CH_RS485, uartGetBaud(HW_UART_CH_USB));
    logPrintf("[  ] rs485 baud %d\n", uartGetBaud(HW_UART_CH_RS485));
  }

  // USB -> RS485
  //
  length = cmin(uartAvailable(HW_UART_CH_USB), 256);
  if (length > 0)
  {
    is_tx_update = true;
    for (uint32_t i=0; i<length; i++)
    {
      buf[i] = uartRead(HW_UART_CH_USB);
    }
    uartWrite(HW_UART_CH_RS485, buf, length);
  }    

  // RS485 -> USB
  //
  length = cmin(uartAvailable(HW_UART_CH_RS485), 256);
  if (length > 0)
  {
    is_rx_update = true;
    for (uint32_t i=0; i<length; i++)
    {
      buf[i] = uartRead(HW_UART_CH_RS485);
    }
    uartWrite(HW_UART_CH_USB, buf, length);
  }      
}

void rs485ThreadUpdatePacket(void)
{
  uint32_t length;
  uint8_t buf[256];

  if (is_packet_open != true)
    return;


  if (packet_baud != uartGetBaud(HW_UART_CH_RS485))
  {
    uartOpen(HW_UART_CH_RS485, packet_baud);
    logPrintf("[  ] rs485 baud %d\n", uartGetBaud(HW_UART_CH_RS485));
  }  

  // RS485 -> USB
  //
  length = cmin(uartAvailable(HW_UART_CH_RS485), 256);
  if (length > 0)
  {
    is_rx_update = true;
    for (uint32_t i=0; i<length; i++)
    {
      buf[i] = uartRead(HW_UART_CH_RS485);
    }
    cmdObj()->sendPacket(PKT_TYPE_UART, CMD_RS485_DATA, OK, buf, length);  
  }      
}

bool rs485ThreadEvent(event_t *p_event)
{
  return true;
}


bool rs485CmdOpen(cmd_t *p_cmd)
{
  data_t baud;

  memcpy(baud.u8Data, &p_cmd->packet.data[0], 4);

  logPrintf("[  ] rs485CmdOpen()\n");
  logPrintf("     %d bps\n", baud.u32D);

  is_packet_open = true;
  packet_baud = baud.u32D;
 
  cmdObj()->sendResp(p_cmd, OK, NULL, 0);
  return true;
}

bool rs485CmdClose(cmd_t *p_cmd)
{
  logPrintf("[  ] rs485CmdClose()\n");
  is_packet_open = false;

  cmdObj()->sendResp(p_cmd, OK, NULL, 0);
  return true;
}

bool rs485CmdData(cmd_t *p_cmd)
{
  // USB -> RS485
  //
  is_tx_update = true;
  uartWrite(HW_UART_CH_RS485, p_cmd->packet.data, p_cmd->packet.length);
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

rs485_obj_t *rs485Obj(void)
{
  static rs485_obj_t obj = 
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
    cliPrintf("rs485 enable : %s\n", is_enable ? "True":"False");
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("rs485 info\n");
  }
}
#endif