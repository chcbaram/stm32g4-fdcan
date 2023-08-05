import sys
import time
import os
import socket
import wave
import struct

from os import path
from PySide6.QtGui import *
from ui.ui_main import *
from lib.cmd import *
from lib.cmd_boot import *
from lib.util import *


VERSION_MAGIC_NUMBER = 0x56455220    # "VER "
TAG_MAGIC_NUMBER     = 0x54414720    # "TAG "


BOOT_SIZE_TAG = 1024
BOOT_SIZE_VER = 1024



class FirmTag:
  magic_number = 0
  fw_addr = 0
  fw_size = 0
  fw_crc = 0
  tag_crc = 0


def millis():
  return round(time.time() * 1000)

class DownError(Exception):
  pass


class DownFirm(QThread):
  start_sig = Signal()
  finish_sig = Signal(int)
  update_sig = Signal(int)
  log_sig = Signal(str)


  def __init__(self, parent, cmd_boot):
    super().__init__(parent)
    self.is_run = True
    self.cmd_boot = cmd_boot

  def __del__(self):
    pass

  def getFileCrc(self, file_buf):
    pass

  def run(self):
    try:
      self.start_sig.emit()

      with open(self.file_name, "rb") as f:
        self.file_buf = f.read()

      self.file_len = len(self.file_buf)

      firm_tag = FirmTag()
      firm_tag.magic_number = 0x54414720
      firm_tag.fw_addr = BOOT_SIZE_TAG
      firm_tag.fw_crc = 0
      firm_tag.fw_size = self.file_len
      firm_tag.fw_crc = utilGetCrc(self.file_buf)
      self.log_sig.emit('file_len    : %d' % self.file_len)
      self.log_sig.emit('file_crc    : %s' % hex(firm_tag.fw_crc))

      str_fmt = "<I32s32sI"
      fmt_size = calcsize(str_fmt)
      firm_ver = unpack(str_fmt, self.file_buf[BOOT_SIZE_VER:BOOT_SIZE_VER+fmt_size])

      if firm_ver[0] != VERSION_MAGIC_NUMBER:
        raise DownError("Err - firm magic")

      if "STM32G4-FDCAN-FW" not in firm_ver[2].decode("utf-8"):
        self.log_sig.emit('firm not match')
        self.log_sig.emit('     STM32G4-FDCAN-FW')
        self.log_sig.emit('     %s' % firm_ver[2].decode("utf-8"))
        raise DownError("Err - firm name\n")

      self.log_sig.emit('firm ver    : %s' % firm_ver[1].decode("utf-8"))
      self.log_sig.emit('firm name   : %s' % firm_ver[2].decode("utf-8"))
      self.log_sig.emit('firm addr   : %s' % hex(firm_ver[3]).upper())
   
      # Begin
      #
      err_code, resp = self.cmd_boot.firmBegin(BootBegin(os.path.basename(self.file_name), self.file_len))
      if err_code == OK:
        self.log_sig.emit("firmBegin()  OK")
      else:
        raise DownError("Err - firmBegin")

      # Flash Erase
      #
      pre_time = millis()
      err_code, resp = self.cmd_boot.firmErase(0, BOOT_SIZE_TAG + self.file_len, 5000)
      if err_code == OK:
        self.log_sig.emit("firmErase()  OK, %d ms" % (millis()-pre_time))
      else:
        raise DownError("Err - firmErase")

      addr = BOOT_SIZE_TAG


      # Flash Write
      #
      tx_len = 0
      wrtie_done = False
      pre_time = millis()
      while tx_len < self.file_len:
        len_to_send = min(self.file_len-tx_len, 256)
        tx_buf = self.file_buf[tx_len:tx_len+len_to_send]
        
        err_code, resp = self.cmd_boot.firmWrite(addr + tx_len, tx_buf, 500)
        if err_code == OK:
          # self.log_sig.emit("firmWrite() OK")
          pass
        else:
          raise DownError("Err - firmWrite")

        tx_len += len_to_send

        percent = ((tx_len+len_to_send)*100/self.file_len)
        self.update_sig.emit(int(percent))

        if tx_len == self.file_len:
          self.log_sig.emit("firmWrite()  OK, %d ms\n" % (millis()-pre_time))
          write_done = True
          break

      if write_done == True:
        tx_buf = pack("IIIII", 
                 firm_tag.magic_number, 
                 firm_tag.fw_addr,
                 firm_tag.fw_size,
                 firm_tag.fw_crc,
                 firm_tag.tag_crc)
        err_code, resp = self.cmd_boot.firmWrite(0, tx_buf, 500)
        if err_code == OK:
          self.log_sig.emit("tagWrite()   OK")
        else:
          raise DownError("Err - tagWrite")        

        pre_time = millis()
        err_code, resp = self.cmd_boot.firmVerify(1000)
        if err_code == OK:
          self.log_sig.emit("firmVerify() OK, %d ms" % (millis()-pre_time))
        else:
          raise DownError("Err - firmVerify")        

        err_code, resp = self.cmd_boot.firmEnd()
        if err_code == OK:
          self.log_sig.emit("firmEnd()    OK")
        else:
          raise DownError("Err - firmEnd")  

        pre_time = millis()
        err_code, resp = self.cmd_boot.firmUpdate(5000)
        if err_code == OK:
          self.log_sig.emit("firmUpdate() OK, %d ms" % (millis()-pre_time))
        else:
          raise DownError("Err - firmUpdate") 

      self.finish_sig.emit(0)

    except Exception as e:
      self.is_run = False
      self.finish_sig.emit(1)
      self.log_sig.emit(str(e))

  def setDown(self, file_name):
    self.file_name = file_name

  def stop(self):
    self.is_run = False
    self.quit()
    self.wait()
