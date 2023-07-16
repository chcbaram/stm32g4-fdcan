import sys
import time
import os
import socket
import serial
import serial.tools.list_ports as sp
from os import path
from PySide6.QtWidgets import QApplication, QMainWindow, QFileDialog
from PySide6.QtCore import QFile
from PySide6.QtGui import *
from ui.ui_main import *
from lib.log import LogWidget
from lib.cmd import *
from lib.cmd_audio import *
from struct import *



def millis():
  return round(time.time() * 1000)





class MainWindow(QMainWindow):
    
  def __init__(self):
    super(MainWindow, self).__init__()
    self.ui = Ui_MainWindow()
    self.ui.setupUi(self)

    self.log = LogWidget(self.ui.log_text)
    self.log.printLog('FDCAN-GUI 23-07-16\n')

    self.file_list = []

    self.cmd = Cmd()

    self.setClickedEvent(self.ui.btn_scan, self.btnScan)  
    self.setClickedEvent(self.ui.btn_open, self.btnOpen)  
    self.ui.combo_device.currentTextChanged.connect(self.onComboDeviceChanged)
 
    self.ui.radio_usb.setChecked(True)
    self.ui.radio_scan_fdcan.setChecked(True)
    self.btnScan()
  
  def __del__(self):
    self.cmd.stop()
    print("del()")

  def setClickedEvent(self, event_dst, event_func):
    event_dst.clicked.connect(lambda: self.btnClicked(event_dst, event_func))   

  def btnClicked(self, button, event_func):
    event_func()

  def btnScan(self):
    self.ui.combo_device.clear()
    ports = sorted(sp.comports())

    for i in ports :
      print(str(i) + " \t" + str(i.usb_info()))
      found_device = False
      if self.ui.radio_scan_fdcan.isChecked():
        if i.vid == 0x0483:
          found_device = True
      else:
        found_device = True  
      
      if found_device == True:
        self.ui.combo_device.addItem(i.device)      
        item_tip_str = i.manufacturer
        if i.description is None:
          item_tip_str = item_tip_str + ' ' +  i.description
        self.ui.combo_device.setItemData(self.ui.combo_device.count()-1, item_tip_str, Qt.ToolTipRole)

  def onComboDeviceChanged(self, value):
    if self.ui.combo_device.count() == 0:
      return  
    
    self.ui.text_device_info.clear()
    device_name = self.ui.combo_device.currentText()
    ports = sorted(sp.comports())
    for i in ports:
      if i.device == device_name:
        self.ui.text_device_info.appendPlainText(i.manufacturer)
        self.ui.text_device_info.appendPlainText(i.description)
        self.ui.text_device_info.appendPlainText(i.serial_number)     
        
        
      
    
  def rxdSignal(self, packet: CmdPacket):
    pass
      
  def btnOpen(self):
    fname = QFileDialog.getOpenFileNames(self, "Open File", "", "WAV File(*.wav)")
    if len(fname[0]) > 0:
      self.updateFileList(fname)
      self.showFileInfo()

  def updateFileList(self, fname):
    self.ui.combo_file.clear()
    self.file_list.clear()
    for i in fname[0]:
      self.file_list.append(i)
      self.ui.combo_file.addItem(os.path.basename(i))

  def showFileInfo(self):
    if self.ui.combo_file.count() == 0:
      return
    
  def closeEvent(self, QCloseEvent):
    QCloseEvent.accept()




def main():
  app = QApplication(sys.argv)
  
  fontpath = os.path.abspath("data/font/D2Coding.ttf")
  QFontDatabase.addApplicationFont(fontpath)
  app.setFont(QFont("D2Coding", 14))


  window = MainWindow()
  center = QScreen.availableGeometry(QApplication.primaryScreen()).center()
  geo = window.frameGeometry()
  geo.moveCenter(center)
  window.move(geo.topLeft())
  window.show()

  sys.exit(app.exec())


if __name__ == "__main__":  
  main()