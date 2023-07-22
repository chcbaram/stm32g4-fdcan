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
from ui.ui_can import *
from ui.ui_main import *


class TabCAN(QWidget, Ui_CAN):
  def __init__(self, ui_main :Ui_MainWindow):
    super().__init__()
    self.setupUi(self)

    self.ui_main = ui_main