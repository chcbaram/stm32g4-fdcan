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

CAN_STD       = 0
CAN_EXT       = 1

CAN_ID_MASK   = 0
CAN_ID_RANGE  = 1


class CmdCANOpen:
  def __init__(self):
    super().__init__()  
    self.mode = 0
    self.frame = 0
    self.baud = 0
    self.baud_data = 0

class CmdCANFilter:
  def __init__(self):
    super().__init__()  
    self.type = 0
    self.id_type = 0
    self.id1 = 0
    self.id2 = 0

class CmdCANPacket:
  def __init__(self):
    super().__init__()
    self.length = 0
    self.data = bytearray(64)    

    self.timestamp  = 0
    self.frame      = 0
    self.id_type    = 0
    self.dlc        = 0
    self.id         = 0
    self.length     = 0

  def setCmdPacket(self, packet: CmdPacket):
    header_fmt = "<HBBBIB"
    fmt_size = calcsize(header_fmt)
    data = unpack(header_fmt, packet.data[:fmt_size])
    self.timestamp  = data[0]
    self.frame      = data[1]
    self.id_type    = data[2]
    self.dlc        = data[3]
    self.id         = data[4]
    self.length     = data[5]
    self.data       = packet.data[fmt_size:fmt_size + self.length]


class CmdCAN:

  CMD_CAN_OPEN        = 0x0110
  CMD_CAN_CLOSE       = 0x0111
  CMD_CAN_DATA        = 0x0112
  CMD_CAN_ERR_INFO    = 0x0113
  CMD_CAN_SET_FILTER  = 0x0114
  CMD_CAN_GET_FILTER  = 0x0115


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

  def send(self, packet: CmdCANPacket, timeout=500):
    err_code = OK

    send_buf = pack("<HBBBIB", 
                    packet.timestamp, 
                    packet.frame,
                    packet.id_type,
                    packet.dlc,
                    packet.id,
                    packet.length)

    send_buf += packet.data[0:packet.length]

    self.cmd.send(PKT_TYPE_CAN, self.CMD_CAN_DATA, OK, send_buf, len(send_buf))
    return err_code, None    

  def getFilter(self, filter: CmdCANFilter, timeout=500):
    err_code = ERR_CMD_RX_TIMEOUT
    ret, packet = self.cmd.sendCmdRxResp(self.CMD_CAN_GET_FILTER, None, 0, timeout)
    if ret == True:
      err_code = packet.err_code
      if packet.err_code == 0:
        str_fmt = "<BBII"
        fmt_size = calcsize(str_fmt)
        data = unpack(str_fmt, packet.data[:fmt_size])
        filter.type = data[0]
        filter.id_type = data[1]
        filter.id1 = data[2]
        filter.id2 = data[3]
    return err_code, None    

  def setFilter(self, filter: CmdCANFilter, timeout=500):
    err_code = ERR_CMD_RX_TIMEOUT
    send_buf = pack("<BBII", 
                    filter.type, 
                    filter.id_type,
                    filter.id1,
                    filter.id2)
    ret, packet = self.cmd.sendCmdRxResp(self.CMD_CAN_SET_FILTER, send_buf, len(send_buf), timeout)
    if ret == True:
      err_code = packet.err_code

    return err_code, None        