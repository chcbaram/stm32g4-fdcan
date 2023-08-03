from sre_compile import isstring
import time
import re
from PySide6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QPlainTextEdit, QGridLayout, QSizePolicy
from PySide6.QtCore import QFile
from PySide6.QtGui import *



class LogWidget(QWidget):
  def __init__(self, parent=None) :
    super().__init__()

    self.state = 0
    self.log = None
    self.is_timelog = False
    if (parent is not None):
      self.log = parent      

    self.pattern_log = re.compile('\[+.{8,16}\]+')
    self.pattern_color = re.compile('\[+.{1,2}m+')
    self.pattern_up = re.compile('\[+.{1,3}A+')
    self.pattern_down = re.compile('\[+.{1,3}B+')
    self.pattern_cli = re.compile('cli# ')

    self.line_max = 100
    self.line_index = 0
    self.line_buf = list(range(self.line_max))
    self.setupUi()    
    
    #print(len(self.line_buf))
        

  def setupUi(self):
    vbox = QVBoxLayout()

    if (self.log is None):
      self.log = QPlainTextEdit()      
      vbox.addWidget(self.log)
      vbox.setContentsMargins(0, 0, 0, 0)
      self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
      self.setLayout(vbox)
    
    self.log.setReadOnly(True)
    self.log.setStyleSheet('background-color: black; color: white;')    


  def setTimeLog(self, enable):
    self.is_timelog = enable

  def printLog(self, str): 
    if self.is_timelog == True:
      tm = time.strftime('%m-%d %H:%M:%S', time.localtime(time.time()))
      #str = str.replace('\n', '')
      self.log.appendPlainText(f'[{tm}] {str}')
    else:
      self.log.appendPlainText(str)

  def print(self, str): 
    self.log.insertPlainText(str)
    self.log.moveCursor(QTextCursor.End)

  def println(self, str): 
    self.log.appendPlainText(str)

  def write(self, data_bytes):    
    out_str = data_bytes.decode('utf-8')    
    out_str = out_str.replace('\r', '')
    self.log.insertPlainText(out_str)
    self.log.moveCursor(QTextCursor.End)

  def writeHex(self, data_bytes):    
    for ch in data_bytes:
      out_str = f"0x{ch:X} "
      self.log.insertPlainText(out_str)

    self.log.moveCursor(QTextCursor.End)

  def writeLine(self, data_bytes):    
    out_str = data_bytes.decode('utf-8')
    out_str = out_str.replace('\r', '')
    out_str = out_str.replace('\n', '')

    if len(out_str) == 0:
      return

    match_str = self.pattern_cli.search(out_str)
    if match_str is not None:
      out_str = out_str.replace(match_str.group(), '')

    match_str = self.pattern_down.search(out_str)
    if match_str is not None:
      out_str = out_str.replace(match_str.group(), '')

    match_str = self.pattern_log.search(out_str)
    if match_str is not None:
      if match_str.start() == 0:
        return
      out_str = out_str[0:match_str.start()]

    match_str = self.pattern_up.search(out_str)
    if match_str is not None:
      #numbers = re.sub(r'[^0-9]', '', match_str.group())
      #print(numbers)
      out_str = out_str.replace(match_str.group(), '')
      self.line_buf[self.line_index] = 0
      self.line_index = 0

    self.line_buf[self.line_index] = out_str      
    self.line_index += 1
    if self.line_index >= len(self.line_buf):
      self.line_index = 0


    out_str = ''
    for i in self.line_buf:
      if type(i) == str:
        #print(len(self.line_buf))
        #print(i)
        out_str += i
        out_str += '\n'
      else:
        break

    if len(out_str) > 0:
      self.log.setPlainText(out_str)  

  def writeLog(self, data_bytes):    
    try:
      out_str = data_bytes.decode('utf-8').replace('\r', '')
    except:    
      print('decode() fail')
      return

    match_str = self.pattern_log.search(out_str)
    if match_str is not None:
      out_str = out_str[match_str.start():len(out_str)]
    else:
      return

    match_str = self.pattern_color.findall(out_str)
    for i in match_str:    
      out_str = out_str.replace(i, '')

    out_str += '\n'
    self.log.insertPlainText(out_str)
    self.log.moveCursor(QTextCursor.End)

  def clear(self):
    self.log.clear()

  def clearIndex(self):
    self.line_index = 0
    self.line_buf = [0 for i in range(self.line_max)]



  def getText(self):
    return self.log.toPlainText()