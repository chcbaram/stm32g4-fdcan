import time
import socket
from enum import Enum
from PySide6.QtCore import QThread, QObject, Signal


CMD_STX0 = 0x02
CMD_STX1 = 0xFD

PKT_TYPE_CMD   = 0x00
PKT_TYPE_RESP  = 0x01
PKT_TYPE_EVENT = 0x02
PKT_TYPE_LOG   = 0x03
PKT_TYPE_PING  = 0x04

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



def millis():
  return round(time.time() * 1000)

class CmdPacket:
  def __init__(self):
    super().__init__()
    self.type = 0
    self.cmd = 0
    self.err_code = 0
    self.check_sum = 0
    self.check_sum_recv = 0
    self.index = 0
    self.data = bytearray(4096)    
    self.str_ip = ""
    self.str_port = ""


class CmdThread(QThread):
  rxd_sig = Signal(CmdPacket)

  def __init__(self, sock):
    super().__init__()
    self.working = True
    self.request_exit = False    
    self.sock = sock
    self.packet_state = 0
    self.packet = CmdPacket()
    self.rxd_packet = CmdPacket()
    self.is_rxd_packet = False

  def __del__(self):
    pass
    
  def run(self):
    while self.working:
      try:
        data, addr = self.sock.recvfrom(1024)
        self.parsingPacket(data, addr)
      except Exception as e:
        if self.request_exit == True:
          self.working = False

  def setRxdSignal(self, receive_func):
    self.rxd_sig.connect(receive_func)

  def stop(self):
    self.request_exit = True
    self.quit()
    while True:
      self.sleep(0.1)
      if self.working == False:
        break

  def getPacket(self):
    self.is_rxd_packet = False
    return self.rxd_packet

  def parsingPacket(self, rx_bytes, addr):
    CMD_STATE_WAIT_STX0      = 0
    CMD_STATE_WAIT_STX1      = 1
    CMD_STATE_WAIT_TYPE      = 2
    CMD_STATE_WAIT_CMD_L     = 3
    CMD_STATE_WAIT_CMD_H     = 4
    CMD_STATE_WAIT_ERR_L     = 5
    CMD_STATE_WAIT_ERR_H     = 6
    CMD_STATE_WAIT_LENGTH_L  = 7
    CMD_STATE_WAIT_LENGTH_H  = 8
    CMD_STATE_WAIT_DATA      = 9
    CMD_STATE_WAIT_CHECKSUM  = 10

    try:
      for rx_data in rx_bytes:
        if self.packet_state == CMD_STATE_WAIT_STX0:
          if rx_data == CMD_STX0:
            self.packet.check_sum = 0
            self.packet.check_sum += rx_data
            self.packet_state = CMD_STATE_WAIT_STX1

        elif self.packet_state == CMD_STATE_WAIT_STX1:
          if rx_data == CMD_STX1:
            self.packet_state = CMD_STATE_WAIT_TYPE
            self.packet.check_sum += rx_data
          else:
            self.packet_state = CMD_STATE_WAIT_STX0

        elif self.packet_state == CMD_STATE_WAIT_TYPE:
          self.packet.type = rx_data          
          self.packet.check_sum += rx_data
          self.packet_state = CMD_STATE_WAIT_CMD_L

        elif self.packet_state == CMD_STATE_WAIT_CMD_L:
          self.packet.cmd = rx_data
          self.packet.check_sum += rx_data
          self.packet_state = CMD_STATE_WAIT_CMD_H
          
        elif self.packet_state == CMD_STATE_WAIT_CMD_H:
          self.packet.cmd = (rx_data << 8) | self.packet.cmd
          self.packet.check_sum += rx_data
          self.packet_state = CMD_STATE_WAIT_ERR_L     

        elif self.packet_state == CMD_STATE_WAIT_ERR_L:
          self.packet.err_code = rx_data
          self.packet.check_sum += rx_data
          self.packet_state = CMD_STATE_WAIT_ERR_H              

        elif self.packet_state == CMD_STATE_WAIT_ERR_H:
          self.packet.err_code = (rx_data << 8) | self.packet.err_code
          self.packet.check_sum += rx_data
          self.packet_state = CMD_STATE_WAIT_LENGTH_L     
          
        elif self.packet_state == CMD_STATE_WAIT_LENGTH_L:
          self.packet.length = rx_data
          self.packet.check_sum += rx_data
          self.packet_state = CMD_STATE_WAIT_LENGTH_H              

        elif self.packet_state == CMD_STATE_WAIT_LENGTH_H:
          self.packet.length = (rx_data << 8) | self.packet.length
          self.packet.check_sum += rx_data

          if self.packet.length > 0:
            self.packet.index = 0
            self.packet_state = CMD_STATE_WAIT_DATA
          else:
            self.packet_state = CMD_STATE_WAIT_CHECKSUM
                   
        elif self.packet_state == CMD_STATE_WAIT_DATA:
          self.packet.data[self.packet.index] = rx_data
          self.packet.check_sum += rx_data
          self.packet.index += 1
          if self.packet.index == self.packet.length:
            self.packet_state = CMD_STATE_WAIT_CHECKSUM

        elif self.packet_state == CMD_STATE_WAIT_CHECKSUM:
          self.packet.check_sum_recv = rx_data
          self.packet.check_sum = ((~self.packet.check_sum) + 1) & 0xFF
   
          if self.packet.check_sum == self.packet.check_sum_recv:
            self.packet.err_code = 0
            self.packet.str_ip = str(addr[0])
            self.packet.str_port = str(addr[1])
            # self.rxd_sig.emit(self.packet)
            self.rxd_packet = self.packet
            self.is_rxd_packet = True
          else:
            self.packet.err_code = 0x0021
          self.packet_state = CMD_STATE_WAIT_STX0  

    except Exception as e:
      print(e)
    
    

class Cmd:
  def __init__(self):
    self.is_init = False
    self.send_ip = ""
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    self.rxd_thread = CmdThread(self.sock)    
    self.rxd_thread.start()    
    self.rxd_thread.setRxdSignal(self.rxdPacketSignal)

    self.is_rxd_packet = False
    self.rxd_packet = CmdPacket()

  def __del__(self):
    self.sock.close()
    self.rxd_thread.stop()
    print("cmd->del()")

  def init(self):
    return
  
  def open(self):
    return

  def stop(self):
    self.sock.close()
    self.rxd_thread.stop()
    print("cmd->stop()")
    return

  def send(self, type, cmd, err_code, data, length):
    index = 0
    buffer = bytearray(10 + length)
    buffer[0] = CMD_STX0
    buffer[1] = CMD_STX1
    buffer[2] = type & 0xFF
    buffer[3] = (cmd >> 0) & 0xFF
    buffer[4] = (cmd >> 8) & 0xFF
    buffer[5] = (err_code >> 0) & 0xFF
    buffer[6] = (err_code >> 8) & 0xFF
    buffer[7] = (length >> 0) & 0xFF
    buffer[8] = (length >> 8) & 0xFF

    index = 9
    for i in range(length):
      buffer[index+i] = data[i]

    index += length

    check_sum = 0
    for i in range(index):
      check_sum += buffer[i]

    check_sum = (~check_sum) + 1
    buffer[index] = check_sum & 0xFF
    index += 1

    if len(self.send_ip) == 0:
      tx_len = self.sock.sendto(buffer, ("255.255.255.255", 5000))
    else:
      tx_len = self.sock.sendto(buffer, (self.send_ip, 5000))

  def sendCmd(self, cmd, data, length):
    self.send(PKT_TYPE_CMD, cmd, 0, data, length)

  def sendCmdRxResp(self, cmd, data, length, timeout_ms):
    self.is_rxd_packet = False
    self.sendCmd(cmd, data, length)

    pre_time = millis()
    while True:
      if self.rxd_thread.is_rxd_packet == True:
        self.is_rxd_packet = True
        self.rxd_packet = self.rxd_thread.getPacket()
        break
      if millis()-pre_time > timeout_ms:
        break
      # time.sleep(0.001)

    return self.is_rxd_packet, self.rxd_packet


  def rxdPacketSignal(self, packet: CmdPacket):
    self.rxd_packet = packet
    self.is_rxd_packet = True
    print("RXD")

  def getPacket(self):
    return self.rxd_packet

  # def setRxdSignal(self, rxd_func):
  #   self.rxd_thread.setRxdSignal(rxd_func)

  def print(self):
    pre_time = millis()
    time.sleep(0.1)
    exe_time = millis()-pre_time
    print("cmd test " + str(exe_time))