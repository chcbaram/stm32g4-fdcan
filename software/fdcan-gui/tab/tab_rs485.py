import sys
import time
import os
import socket
import serial
import serial.tools.list_ports as sp
import struct

from os import path
from PySide6.QtWidgets import QApplication, QMainWindow, QFileDialog
from PySide6.QtCore import QFile
from PySide6.QtGui import *
from ui.ui_rs485 import *
from ui.ui_main import *


class TabRS485(QWidget, Ui_RS485):
  def __init__(self, ui_main :Ui_MainWindow):
    super().__init__()
    self.setupUi(self)

    self.ui_main = ui_main

    self.setClickedEvent(self.btn_open, self.btnTest)  

  def setClickedEvent(self, event_dst, event_func):
    event_dst.clicked.connect(lambda: self.btnClicked(event_dst, event_func))   

  def btnClicked(self, button, event_func):
    event_func() 

  def btnTest(self):
    self.textEdit.append(str(self.ui_main.radio_scan_fdcan.isChecked()))
    self.textEdit.append(str(self.ui_main.radio_scan_uart.isChecked()))