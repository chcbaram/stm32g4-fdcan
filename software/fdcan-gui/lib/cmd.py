import time
import serial
import copy
import serial.tools.list_ports as sp

from PySide6.QtCore import QThread, QObject, Signal, QMutex
from lib.err_code import *
from queue import Queue


CMD_STX0 = 0x02
CMD_STX1 = 0xFD

PKT_TYPE_CMD   = 0x00
PKT_TYPE_RESP  = 0x01
PKT_TYPE_EVENT = 0x02
PKT_TYPE_LOG   = 0x03
PKT_TYPE_PING  = 0x04
PKT_TYPE_CAN   = 0x05
PKT_TYPE_UART  = 0x06




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


class CmdThread(QThread):
  event_sig = Signal(CmdPacket)

  def __init__(self, port, resp_q):
    super().__init__()
    self.working = True
    self.request_exit = False    
    self.port = port
    self.packet_state = 0
    self.packet = CmdPacket()

    self.resp_q = resp_q
    self.mutex = QMutex()

  def __del__(self):
    pass
    
  def run(self):
    while self.working:
      try:
        data =  self.port.read()
        self.parsingPacket(data)
      except Exception as e:
        time.sleep(0.001)
      if self.request_exit == True:
        self.working = False

  def setRxdSignal(self, receive_func):
    self.event_sig.connect(receive_func)

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

  def clearBuffer(self):
    if self.resp_q.qsize() > 0:
      self.resp_q.get()

  def parsingPacket(self, rx_bytes):
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
            try:
              if self.packet.type == PKT_TYPE_RESP:
                self.resp_q.put(self.packet, 1)
              else:
                self.mutex.lock()
                packet = copy.deepcopy(self.packet)
                self.mutex.unlock()
                self.event_sig.emit(packet)
            except Exception as e:
              print(e)

          else:
            self.packet.err_code = ERR_CMD_CHECKSUM
          self.packet_state = CMD_STATE_WAIT_STX0  

    except Exception as e:
      print(e)
    
    

class Cmd(QObject):
  rxd_sig = Signal(CmdPacket)

  def __init__(self):
    super().__init__()
    self.is_init = False
    self.is_open = False
    self.resp_q = Queue(1)
    self.mutex = QMutex()
    self.mutex_send = QMutex()

    self.uart_port = serial.Serial(timeout=0.1)   
    self.rxd_thread = CmdThread(self.uart_port, self.resp_q)    
    self.rxd_thread.start()    
    self.rxd_thread.setRxdSignal(self.eventSignal)

  def __del__(self):
    self.uart_port.close()
    self.rxd_thread.stop()
    print("cmd->del()")

  def init(self):
    return
  
  def open(self, port, baud):
    self.port = port
    self.baud = baud
    try :
      self.uart_port.port = port
      self.uart_port.baudrate = baud
      self.uart_port.open()
      time.sleep(0.1)
      self.uart_port.flush()
      self.uart_port.flushInput()
      self.uart_port.flushOutput()
      self.is_open = True
      # self.uart_port.timeout = 0.1
      print('Uart::open() OK')
    except :
      self.is_open = False
      print('Uart::open() Fail')

    return self.is_open

  def stop(self):
    self.rxd_thread.stop()
    print("cmd->stop()")
    return

  def close(self):  
    if self.uart_port is not None:
      if self.uart_port.is_open == True:
        self.is_open = False
        try:
          self.uart_port.cancel_read()
          self.uart_port.cancel_write() 
          self.uart_port.close()        
          print('Uart::close()')
        except:
          pass

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

    self.uart_port.write_timeout = 5
    self.mutex_send.lock()
    tx_len = self.uart_port.write(buffer)
    self.mutex_send.unlock()

  def sendCmd(self, cmd, data, length):
    self.send(PKT_TYPE_CMD, cmd, 0, data, length)

  def sendCmdRxResp(self, cmd, data, length, timeout_ms):
    self.mutex.lock()
    if self.resp_q.qsize() > 0:
      self.resp_q.get()
    self.sendCmd(cmd, data, length)

    ret = False 

    try:
      ret_packet = self.resp_q.get(timeout=timeout_ms/1000)
      ret = True
    except:
      ret_packet = None
    self.mutex.unlock()
    return ret, ret_packet


  def eventSignal(self, packet: CmdPacket):
    self.rxd_sig.emit(packet)

  def setRxdEvent(self, event_func):
    self.rxd_sig.connect(event_func)

  def getPacket(self):
    return self.rxd_packet

  def print(self):
    pre_time = millis()
    time.sleep(0.1)
    exe_time = millis()-pre_time
    print("cmd test " + str(exe_time))