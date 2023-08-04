import sys
import time
import os
import socket
import serial
import serial.tools.list_ports as sp
import struct

from os import path
from PySide6.QtWidgets import QApplication, QMainWindow, QFileDialog, QHBoxLayout
from PySide6.QtCore import QFile
from PySide6.QtGui import *
from ui.ui_can import *
from ui.ui_main import *
from configparser import ConfigParser



class TabCAN(QWidget, Ui_CAN):
  def __init__(self, ui_main :Ui_MainWindow, cmd, config_item, syslog):
    super().__init__()
    self.setupUi(self)

    self.ui_main = ui_main
    self.bit_rate = {"100K":0, "125K":1, "250K":2, "500K":3, "1M":4, "2M":5, "4M":6, "5M":7}
    self.dlc_str = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "12", "16", "20", "24", "32", "48", "64"]
    self.can_tx_head = {"Sel":0, "ID":1, "Ext.ID":2, "FD":3, "BRS":4, "DLC":5, "Data":6}
    self.can_rx_head = {"Time":0, "ID":1, "Tx/Rx":2, "Type":3, "DLC":4, "Data":5}

    self.is_open = False
    self.cmd = cmd
    self.config_item = config_item
    self.log = syslog
    
    self.combo_open_rate_normal.clear()
    self.combo_open_rate_data.clear()
    for item in self.bit_rate:
      if self.bit_rate[item] <= self.bit_rate["1M"]:
        self.combo_open_rate_normal.addItem(item)
      self.combo_open_rate_data.addItem(item)

    self.loadConfig()

    self.setClickedEvent(self.btn_open, self.btnOpen)
    self.setClickedEvent(self.btn_close, self.btnClose)
    self.setClickedEvent(self.btn_send, self.btnSend)
    self.setClickedEvent(self.btn_add, self.btnAdd)

    self.check_open_canfd.stateChanged.connect(self.btnUpdate)  
    self.check_open_brs.stateChanged.connect(self.btnUpdate)  

    self.table_can_rx.resizeColumnsToContents()
    self.table_can_tx.setSelectionMode(QAbstractItemView.SelectionMode.SingleSelection)
    self.table_can_tx.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectRows)
    self.table_can_tx.itemChanged.connect(self.updateCanTxTable)

    self.btnUpdate()

  def btnUpdate(self):
    
    if self.is_open == True:
      self.btn_open.setEnabled(False)
      self.btn_close.setEnabled(True)

      self.check_open_canfd.setEnabled(False)
      self.check_open_brs.setEnabled(False)
      self.combo_open_rate_normal.setEnabled(False)
      self.combo_open_rate_data.setEnabled(False)
    else:
      self.btn_open.setEnabled(True)
      self.btn_close.setEnabled(False)

      self.check_open_canfd.setEnabled(True)
      self.combo_open_rate_normal.setEnabled(True)
      if self.check_open_canfd.isChecked():
        self.check_open_brs.setEnabled(True)
      else:
        self.check_open_brs.setChecked(False)
        self.check_open_brs.setEnabled(False)

      if self.check_open_brs.isChecked():
        self.combo_open_rate_data.setEnabled(True)
      else:
        self.combo_open_rate_data.setEnabled(False)

    self.btn_send.setEnabled(self.is_open)

  def setClickedEvent(self, event_dst, event_func):
    event_dst.clicked.connect(lambda: self.btnClicked(event_dst, event_func))   

  def btnClicked(self, button, event_func):
    event_func()   
    self.btnUpdate()    

  def getTableCenterWidget(self, item):
    cell_widget = QWidget()
    layout = QHBoxLayout(cell_widget)
    layout.addWidget(item)
    layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
    layout.setContentsMargins(0, 0, 0, 0)
    cell_widget.setLayout(layout)    
    return cell_widget

  def btnSend(self):
    pass
    
  def btnAdd(self):
    row_pos = self.table_can_tx.rowCount()
    self.table_can_tx.insertRow(row_pos)

    ckbox = QCheckBox()
    self.table_can_tx.setCellWidget(row_pos, self.can_tx_head["Sel"], self.getTableCenterWidget(ckbox))

    ckbox = QCheckBox()
    ckbox.setChecked(self.check_tx_extid.isChecked())
    self.table_can_tx.setCellWidget(row_pos, self.can_tx_head["Ext.ID"], self.getTableCenterWidget(ckbox))

    ckbox = QCheckBox()
    ckbox.setChecked(self.check_tx_fd.isChecked())
    self.table_can_tx.setCellWidget(row_pos, self.can_tx_head["FD"], self.getTableCenterWidget(ckbox))

    ckbox = QCheckBox()
    ckbox.setChecked(self.check_tx_brs.isChecked())
    self.table_can_tx.setCellWidget(row_pos, self.can_tx_head["BRS"], self.getTableCenterWidget(ckbox))

    cbbox = QComboBox()
    cbbox.addItems(self.dlc_str)
    cbbox.setCurrentIndex(self.combo_tx_dlc.currentIndex())
    self.table_can_tx.setCellWidget(row_pos, self.can_tx_head["DLC"], self.getTableCenterWidget(cbbox))

    item = QTableWidgetItem(self.text_tx_id.text())
    self.table_can_tx.setItem(row_pos, self.can_tx_head["ID"], item)

    self.table_can_tx.cellWidget(row_pos, self.can_tx_head["Sel"]).layout().itemAt(0).widget().setChecked(True)

    self.updateCanTxTable()


  def updateCanTxTable(self):
    self.table_can_tx.resizeColumnToContents(self.can_tx_head["DLC"])
    self.table_can_tx.resizeColumnToContents(self.can_tx_head["ID"])
    self.table_can_tx.viewport().update()

  def btnOpen(self):  
    if self.is_open == True:
      self.is_open = False
      self.btnUpdate()
      return

    if self.cmd.is_open:
      # err_code, resp = self.cmd_rs485.open(int(self.combo_baud.currentText()))
      # if err_code == OK:
      #   self.is_open = True
      #   self.saveConfig()
      self.is_open = True
    else:
      self.log.println("Not Connected")
    self.btnUpdate()

  def btnClose(self):  
    self.is_open = False

  def loadConfig(self):        
    try:
      if self.config_item['can_open_fd'] == "True":
        self.check_open_canfd.setChecked(True)
      if self.config_item['can_open_brs'] == "True":
        self.check_open_brs.setChecked(True)
      if self.config_item['can_tx_extid'] == "True":
        self.check_tx_extid.setChecked(True)
      if self.config_item['can_tx_brs'] == "True":
        self.check_tx_brs.setChecked(True)

      self.combo_open_rate_normal.setCurrentIndex(int(self.config_item['can_open_rate_normal']))
      self.combo_open_rate_data.setCurrentIndex(int(self.config_item['can_open_rate_data']))
      self.combo_tx_dlc.setCurrentIndex(int(self.config_item['can_tx_dlc']))
      self.text_tx_id.setText(self.config_item['can_tx_id'])
    except Exception as e:
      print(e)

  def saveConfig(self):  
    self.config_item['can_open_fd'] = str(self.check_open_canfd.isChecked())
    self.config_item['can_open_brs'] = str(self.check_open_brs.isChecked())
    self.config_item['can_open_rate_normal'] = str(self.combo_open_rate_normal.currentIndex())
    self.config_item['can_open_rate_data'] = str(self.combo_open_rate_data.currentIndex())
    self.config_item['can_tx_extid'] = str(self.check_tx_extid.isChecked())
    self.config_item['can_tx_brs'] = str(self.check_tx_brs.isChecked())
    self.config_item['can_tx_dlc'] = str(self.combo_tx_dlc.currentIndex())
    self.config_item['can_tx_id'] = self.text_tx_id.text()