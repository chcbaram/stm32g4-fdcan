import time
import cmd
from struct import *
from lib.err_code import *




class FirmVer:
  version_str = ""
  name_str = ""
  firm_addr = 0

class BootVersion:
  boot = FirmVer()
  firm = FirmVer()
  update = FirmVer()

class BootBegin:
  def __init__(self, fw_name="", fw_size=0):
    self.fw_name = fw_name
    self.fw_size = fw_size





class CmdBoot:

  BOOT_CMD_INFO         = 0x0000
  BOOT_CMD_VERSION      = 0x0001
  BOOT_CMD_FLASH_ERASE  = 0x0003
  BOOT_CMD_FLASH_WRITE  = 0x0004
  BOOT_CMD_FLASH_READ   = 0x0005
  BOOT_CMD_FW_VER       = 0x0006
  BOOT_CMD_FW_ERASE     = 0x0007
  BOOT_CMD_FW_WRITE     = 0x0008
  BOOT_CMD_FW_READ      = 0x0009
  BOOT_CMD_FW_VERIFY    = 0x000A
  BOOT_CMD_FW_UPDATE    = 0x000B
  BOOT_CMD_FW_JUMP      = 0x000C
  BOOT_CMD_FW_BEGIN     = 0x000D
  BOOT_CMD_FW_END       = 0x000E
  BOOT_CMD_LED          = 0x0010

  def __init__(self, cmd):
    self.cmd = cmd
    self.is_begin = False

  def __del__(self):
    pass


  def readVersion(self, timeout=500):
    resp = BootVersion()
    err_code = ERR_CMD_RX_TIMEOUT
    ret, packet = self.cmd.sendCmdRxResp(self.BOOT_CMD_VERSION, None, 0, timeout)
    if ret == True:
      err_code = packet.err_code
      if packet.err_code == 0:
        str_fmt = "<I32s32sI"
        fmt_size = calcsize(str_fmt)
        data = unpack(str_fmt, packet.data[:fmt_size])
        resp.boot.version_str = data[1].decode("utf-8")
        resp.boot.name_str = data[2].decode("utf-8")

        data = unpack(str_fmt, packet.data[fmt_size*1:fmt_size*2])
        resp.firm.version_str = data[1].decode("utf-8")
        resp.firm.name_str = data[2].decode("utf-8")

        data = unpack(str_fmt, packet.data[fmt_size*2:fmt_size*3])
        resp.update.version_str = data[1].decode("utf-8")
        resp.update.name_str = data[2].decode("utf-8")     
    return err_code, resp 

  def firmBegin(self, boot_begin :BootBegin, timeout=500):
    err_code = ERR_CMD_RX_TIMEOUT
    fmt_begin = "64sI"
    send_buf = pack(fmt_begin, bytes(boot_begin.fw_name, "utf-8"), boot_begin.fw_size)
    ret, packet = self.cmd.sendCmdRxResp(self.BOOT_CMD_FW_BEGIN, send_buf, len(send_buf), timeout)
    if ret == True:
      err_code = packet.err_code
      if err_code == OK:
        self.is_begin = True
    return err_code, None

  def firmEnd(self, timeout=500):
    err_code = ERR_CMD_RX_TIMEOUT
    ret, packet = self.cmd.sendCmdRxResp(self.BOOT_CMD_FW_END, None, 0, timeout)
    self.is_begin = False
    if ret == True:
      err_code = packet.err_code
    return err_code, None

  def firmErase(self, addr, length, timeout=5000):
    err_code = ERR_CMD_RX_TIMEOUT
    send_buf = pack("II", addr, length)
    ret, packet = self.cmd.sendCmdRxResp(self.BOOT_CMD_FW_ERASE, send_buf, len(send_buf), timeout)
    if ret == True:
      err_code = packet.err_code
    return err_code, None

  def firmWrite(self, addr, data, timeout=500):
    err_code = ERR_CMD_RX_TIMEOUT
    send_buf = pack("II", addr, len(data)) + data
    ret, packet = self.cmd.sendCmdRxResp(self.BOOT_CMD_FW_WRITE, send_buf, len(send_buf), timeout)
    if ret == True:
      err_code = packet.err_code
    return err_code, None

  def firmVerify(self, timeout=500):
    err_code = ERR_CMD_RX_TIMEOUT
    ret, packet = self.cmd.sendCmdRxResp(self.BOOT_CMD_FW_VERIFY, None, 0, timeout)
    self.is_begin = False
    if ret == True:
      err_code = packet.err_code
    return err_code, None

  def firmUpdate(self, timeout=500):
    err_code = ERR_CMD_RX_TIMEOUT
    ret, packet = self.cmd.sendCmdRxResp(self.BOOT_CMD_FW_UPDATE, None, 0, timeout)
    self.is_begin = False
    if ret == True:
      err_code = packet.err_code
    return err_code, None    

  def ledToggle(self, timeout=500):
    err_code = ERR_CMD_RX_TIMEOUT
    ret, packet = self.cmd.sendCmdRxResp(self.BOOT_CMD_LED, None, 0, timeout)
    if ret == True:
      err_code = packet.err_code
    return err_code, None