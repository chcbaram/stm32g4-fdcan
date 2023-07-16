import time
import cmd
from struct import *

AUDIO_TYPE_I2S = 0
AUDIO_TYPE_SAI = 1


AUDIO_CMD_INFO           =  0x0020
AUDIO_CMD_BEGIN          =  0x0021
AUDIO_CMD_END            =  0x0022
AUDIO_CMD_READY          =  0x0023
AUDIO_CMD_WRITE          =  0x0024
AUDIO_CMD_WRITE_NO_RESP  =  0x0025

fmt_begin = "B128sIIB"
fmt_ready = "I"



def millis():
  return round(time.time() * 1000)


class CmdAudio:
  def __init__(self, cmd):
    self.cmd = cmd
    self.is_begin = False

  def __del__(self):
    pass

  def begin(self, hw_type, file_name, sample_rate, file_size):
    send_buf = pack(fmt_begin, hw_type, file_name, sample_rate, file_size, 0x00)
    ret, packet = self.cmd.sendCmdRxResp(AUDIO_CMD_BEGIN, send_buf, len(send_buf), 500)
    if ret == True:
      self.is_begin = True

  def end(self):
    ret, packet = self.cmd.sendCmdRxResp(AUDIO_CMD_END, None, 0, 500)
    self.is_begin = False

  def ready(self):
    if self.is_begin == False:
      return False, None

    pre_time = millis()
    ret, packet = self.cmd.sendCmdRxResp(AUDIO_CMD_READY, None, 0, 100)
    data = unpack(fmt_ready, packet.data[:4])
    return ret, data[0]

  def writeNoResp(self, data, timeout):
    if self.is_begin == False:
      return False, None
    ret = self.cmd.sendCmd(AUDIO_CMD_WRITE_NO_RESP, data, len(data))
    return ret
