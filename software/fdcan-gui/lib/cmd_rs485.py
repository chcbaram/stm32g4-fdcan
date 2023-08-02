import time
import cmd
from struct import *
from lib.err_code import *
from lib.cmd import *







class CmdRS485:

  CMD_RS485_OPEN        = 0x0100
  CMD_RS485_CLOSE       = 0x0101
  CMD_RS485_DATA        = 0x0102


  def __init__(self, cmd):
    self.cmd = cmd

  def __del__(self):
    pass



  def open(self, baud, timeout=500):
    err_code = ERR_CMD_RX_TIMEOUT
    send_buf = pack("I", baud)
    ret, packet = self.cmd.sendCmdRxResp(self.CMD_RS485_OPEN, send_buf, len(send_buf), timeout)
    if ret == True:
      err_code = packet.err_code
    return err_code, None

  def close(self, timeout=500):
    err_code = ERR_CMD_RX_TIMEOUT
    ret, packet = self.cmd.sendCmdRxResp(self.CMD_RS485_CLOSE, None, 0, timeout)
    if ret == True:
      err_code = packet.err_code
    return err_code, None

  def send(self, send_buf, timeout=500):
    err_code = OK
    if len(send_buf) == 0:
      return
      
    self.cmd.send(PKT_TYPE_UART, self.CMD_RS485_DATA, OK, send_buf, len(send_buf))
    return err_code, None    