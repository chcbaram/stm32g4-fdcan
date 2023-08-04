import time
import cmd
from struct import *
from lib.err_code import *
from lib.cmd import *



CAN_NORMAL    = 0
CAN_MONITOR   = 1
CAN_LOOPBACK  = 2

CAN_CLASSIC   = 0
CAN_FD_NO_BRS = 1
CAN_FD_BRS    = 2



class CmdCANOpen:
  def __init__(self):
    super().__init__()  
    self.mode = 0
    self.frame = 0
    self.baud = 0
    self.baud_data = 0


class CmdCAN:

  CMD_CAN_OPEN        = 0x0110
  CMD_CAN_CLOSE       = 0x0111
  CMD_CAN_DATA        = 0x0112


  def __init__(self, cmd):
    self.cmd = cmd

  def __del__(self):
    pass

  def open(self, option :CmdCANOpen, timeout=500):
    err_code = ERR_CMD_RX_TIMEOUT
    send_buf = pack("BBBB", 
                    option.mode, 
                    option.frame, 
                    option.baud,
                    option.baud_data)
    ret, packet = self.cmd.sendCmdRxResp(self.CMD_CAN_OPEN, send_buf, len(send_buf), timeout)
    if ret == True:
      err_code = packet.err_code
    return err_code, None

  def close(self, timeout=500):
    err_code = ERR_CMD_RX_TIMEOUT
    ret, packet = self.cmd.sendCmdRxResp(self.CMD_CAN_CLOSE, None, 0, timeout)
    if ret == True:
      err_code = packet.err_code
    return err_code, None

  def send(self, send_buf, timeout=500):
    err_code = OK
    if len(send_buf) == 0:
      return
      
    self.cmd.send(PKT_TYPE_CAN, self.CMD_CAN_DATA, OK, send_buf, len(send_buf))
    return err_code, None    