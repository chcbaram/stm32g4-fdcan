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
from ui.ui_main import *
from lib.err_code import *
from lib.log import LogWidget
from lib.cmd import *
from lib.cmd_boot import *
from lib.down_firm import *







class MainWindow(QMainWindow):
    
  def __init__(self):
    super(MainWindow, self).__init__()
    self.ui = Ui_MainWindow()
    self.ui.setupUi(self)

    self.log = LogWidget(self.ui.log_text)
    self.log.setTimeLog(True)
    self.log.printLog('FDCAN-GUI 23-07-16\n')

    self.file_list = []
    self.cmd = Cmd()
    self.cmd_boot = CmdBoot(self.cmd)
    self.device_info = []
    self.is_fdcan = []
    self.down_thread = None

    self.setClickedEvent(self.ui.btn_scan, self.btnScan)  
    self.setClickedEvent(self.ui.btn_open, self.btnOpen)  
    self.setClickedEvent(self.ui.btn_connect, self.btnConnect)  
    self.setClickedEvent(self.ui.btn_down, self.btnDown)  
    self.setClickedEvent(self.ui.btn_down_stop, self.btnDownStop)  
    self.setClickedEvent(self.ui.btn_log_clear, self.btnLogClear)  

    self.ui.combo_device.currentTextChanged.connect(self.onComboDeviceChanged)
 
    self.ui.radio_scan_fdcan.setChecked(True)
    self.ui.btn_connect.setEnabled(False)
    self.ui.btn_down_stop.setEnabled(False)
    self.btnScan()
    self.updateBtn()
  
  def __del__(self):
    self.cmd.stop()
    print("del()")

  def setClickedEvent(self, event_dst, event_func):
    event_dst.clicked.connect(lambda: self.btnClicked(event_dst, event_func))   

  def btnClicked(self, button, event_func):
    event_func()
    self.updateBtn()

  def btnScan(self):
    self.ui.btn_connect.setEnabled(False)

    self.ui.combo_device.clear()
    self.device_info.clear()
    self.is_fdcan.clear()

    ports = sorted(sp.comports())

    for i in ports :
      # print(str(i) + " \t" + str(i.usb_info()))
      found_device = False
      is_fdcan = False
      if self.ui.radio_scan_fdcan.isChecked():
        if i.vid == 0x0483 and i.pid == 0x5740:
          found_device = True
      else:
        if i.vid is not None:
          found_device = True  

      if i.vid == 0x0483 and i.pid == 0x5740:
        is_fdcan = True

      if found_device == True:
        self.is_fdcan.append(is_fdcan)
        self.device_info.append("S/N : " + str(i.serial_number))
        self.ui.combo_device.addItem(i.device)      
        item_tip_str = i.manufacturer
        if i.description is None:
          item_tip_str = item_tip_str + ' ' +  i.description
        self.ui.combo_device.setItemData(self.ui.combo_device.count()-1, item_tip_str, Qt.ToolTipRole)

  def onComboDeviceChanged(self, value):
    if self.ui.combo_device.count() == 0:
      return  
    self.ui.text_device_info.clear()
    device_name = self.device_info[self.ui.combo_device.currentIndex()]
    self.ui.text_device_info.appendPlainText(device_name)    
    self.ui.btn_connect.setEnabled(True)        
        
      
    
  def rxdSignal(self, packet: CmdPacket):
    pass
      
  def btnOpen(self):
    fname = QFileDialog.getOpenFileNames(self, "Open File", "", "Bin File(*.bin)")
    if len(fname[0]) > 0:
      self.updateFileList(fname)
      self.showFileInfo()

  def btnLogClear(self):
    self.log.clear()

  def btnConnect(self):
    if self.ui.combo_device.count() == 0:
      return    

    if self.cmd.is_open:
      self.cmd.close()
      return   

    index = self.ui.combo_device.currentIndex() 
    port = self.ui.combo_device.currentText()
    if self.is_fdcan[index] == True:
      baud = 19210
    else:
      baud = 115200

    self.log.printLog("BAUD : " + str(baud))

    self.cmd.open(port, baud)

    if self.is_fdcan[index] == True:
      err_code, resp = self.cmd_boot.readVersion()
      if err_code == OK:
        self.ui.text_device_info.clear()
        self.ui.text_device_info.appendPlainText(self.device_info[index])    
        self.ui.text_device_info.appendPlainText("    ")

        self.ui.text_device_info.appendPlainText(resp.boot.name_str)
        self.ui.text_device_info.appendPlainText("    " + resp.boot.version_str)

        self.ui.text_device_info.appendPlainText(resp.firm.name_str)
        self.ui.text_device_info.appendPlainText("    " + resp.firm.version_str)     

  def isCanDown(self):
    if self.ui.btn_down.isEnabled() == False:
      return False    
    if self.down_thread is not None:
      if self.down_thread.isRunning() == True:
        return False
    return True

  def btnDown(self):
    # err_code, resp = self.cmd_boot.firmBegin(BootBegin("test.bin", 100))
    # if err_code == OK:
    #   self.log.printLog("firmBegin() OK")

    # err_code, resp = self.cmd_boot.firmErase(0, 1024)
    # if err_code == OK:
    #   self.log.printLog("firmErase() OK")

    # err_code, resp = self.cmd_boot.firmEnd()
    # if err_code == OK:
    #   self.log.printLog("firmEnd() OK")
    if self.isCanDown() == True:
      down_file = self.file_list[self.ui.combo_file.currentIndex()]
      self.down_thread = DownFirm(self, self.cmd_boot)
      self.down_thread.start_sig.connect(self.onStartDown)
      self.down_thread.finish_sig.connect(self.onFinishDown)
      self.down_thread.update_sig.connect(self.onUpdateDown)
      self.down_thread.log_sig.connect(self.onDownLog)
      self.down_thread.setDown(down_file)
      self.down_thread.start()

  def btnDownStop(self):
    if self.down_thread is not None:
      if self.down_thread.isRunning() == True:
        self.down_thread.stop()
        self.log.printLog("Down Stop")
    pass

  def updateFileList(self, fname):
    self.ui.combo_file.clear()
    self.file_list.clear()
    for i in fname[0]:
      self.file_list.append(i)
      self.ui.combo_file.addItem(os.path.basename(i))

  def showFileInfo(self):
    if self.ui.combo_file.count() == 0:
      return

  def updateBtn(self):
    if self.ui.combo_device.count() == 0:
      self.ui.btn_connect.setText("Connect")
      self.ui.btn_connect.setEnabled(False)   
    else:
      if self.cmd.is_open:
        self.ui.btn_connect.setText("Disconnect")
        self.ui.btn_connect.setEnabled(True)   
      else:
        self.ui.btn_connect.setText("Connect")
        self.ui.btn_connect.setEnabled(True)   
        self.onComboDeviceChanged("")

    if self.cmd.is_open:
      self.ui.btn_scan.setEnabled(False)
    else:
      self.ui.btn_scan.setEnabled(True)   

    if self.cmd.is_open and self.ui.combo_file.count() > 0:
      self.ui.btn_down.setEnabled(True)
    else:
      self.ui.btn_down.setEnabled(False)

  def onStartDown(self):
    self.log.printLog("Down Start..")
    self.ui.btn_down.setEnabled(False)
    self.ui.btn_down_stop.setEnabled(True)

  def onFinishDown(self, err_code):
    self.log.printLog("Down Finish.." + str(err_code))
    self.ui.btn_down.setEnabled(True)
    self.ui.btn_down_stop.setEnabled(False)
    self.btnConnect()
    self.updateBtn()

  def onUpdateDown(self, update_value):
    self.ui.prog_bar_down.setValue(update_value)

  def onDownLog(self, data):
    self.log.printLog(data)
    

  def closeEvent(self, QCloseEvent):
    if self.down_thread is not None:
      self.down_thread.stop()     
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