import sys
import time
import os
import socket
import serial
import serial.tools.list_ports as sp
import struct
import csv
import string
import pandas as pd

from os import path
from PySide6 import QtCore
from PySide6.QtWidgets import QApplication, QMainWindow, QFileDialog, QHBoxLayout
from PySide6.QtCore import QFile, QAbstractTableModel
from PySide6.QtGui import *
from ui.ui_can import *
from ui.ui_main import *
from configparser import ConfigParser

from lib.cmd import *
from lib.cmd_can import *





class TableModel(QtCore.QAbstractTableModel):
  def __init__(self, data, parent=None):
    super(TableModel, self).__init__(parent)
    self._data = data
    self.add_data_cnt = 0

  def rowCount(self, parent=None):
    return len(self._data.index)

  def columnCount(self, parent=None):
    return self._data.columns.size

  def data(self, index, role=QtCore.Qt.DisplayRole):
    if index.isValid():
      if role == QtCore.Qt.DisplayRole:
        return str(self._data.iloc[index.row(), index.column()])
      elif role == QtCore.Qt.TextAlignmentRole:
        if index.column() <= 4:
          return QtCore.Qt.AlignCenter
    return None

  def headerData(self, rowcol, orientation, role):
    if orientation == QtCore.Qt.Horizontal and role == QtCore.Qt.DisplayRole:
      return self._data.columns[rowcol]
    if orientation == QtCore.Qt.Vertical and role == QtCore.Qt.DisplayRole:
      return rowcol
    return None

  def flags(self, index):
    flags = super(self.__class__, self).flags(index)
    flags |= QtCore.Qt.ItemIsEditable
    flags |= QtCore.Qt.ItemIsSelectable
    flags |= QtCore.Qt.ItemIsEnabled
    flags |= QtCore.Qt.ItemIsDragEnabled
    flags |= QtCore.Qt.ItemIsDropEnabled

    return flags

  def sort(self, Ncol, order):
    """Sort table by given column number.
    """
    try:
      self.layoutAboutToBeChanged.emit()
      self._data = self._data.sort_values(self._data.columns[Ncol], ascending=not order)
      self.layoutChanged.emit()
    except Exception as e:
      print(e)

  def addData(self, data):
    self.beginInsertRows(QtCore.QModelIndex(), self.rowCount(), self.rowCount())
    self._data.loc[len(self._data)] = data
    self.endInsertRows()

  def addDataOnly(self, data):
    self._data.loc[len(self._data)] = data
    self.add_data_cnt += 1

  def updateRows(self):
    if self.add_data_cnt > 0:
      self.beginInsertRows(QtCore.QModelIndex(), self.rowCount()-self.add_data_cnt+1, self.rowCount())
      self.endInsertRows()
      self.add_data_cnt = 0

  def clear(self):
    self._data = self._data[0:0].copy(deep=True)
    self.layoutChanged.emit()



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
    self.cmd_can = CmdCAN(cmd)    
    self.config_item = config_item
    self.log = syslog
    self.can_filter = CmdCANFilter()

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
    self.setClickedEvent(self.btn_del, self.btnDel)
    self.setClickedEvent(self.btn_clear_rx_msg, self.btnClearRxMsg)
    self.setClickedEvent(self.btn_filter_set, self.btnFilterSet)


    self.check_open_canfd.stateChanged.connect(self.btnUpdate)  
    self.check_open_brs.stateChanged.connect(self.btnUpdate)  

    self.table_can_tx.setSelectionMode(QAbstractItemView.SelectionMode.SingleSelection)
    self.table_can_tx.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectRows)
    self.table_can_tx.itemChanged.connect(self.updateCanTxTable)
    self.btnUpdate()
    self.loadSendMsg()

    self.table_can_tx.resizeColumnsToContents()


    self.dataframe = pd.DataFrame(columns=['   Time   ','   ID   ', "Tx/Rx", "    Type    ", "DLC", "Data"])
    self.tm = TableModel(self.dataframe)
    # self.tm.rowsInserted.connect(lambda: QtCore.QTimer.singleShot(0, self.view_can_rx.scrollToBottom))
    self.tm.rowsInserted.connect(self.tableAutoScroll)
    self.view_can_rx.setModel(self.tm)
    self.view_can_rx.setSelectionBehavior(QAbstractItemView.SelectRows)
    self.view_can_rx.resizeColumnsToContents()
    self.view_can_rx.horizontalHeader().setStretchLastSection(True)
    # self.view_can_rx.resizeRowsToContents()

    self.table_timer = QtCore.QTimer()
    self.table_timer.setInterval(500)
    self.table_timer.timeout.connect(self.tableUpdate)
    self.table_timer.start()

  def __del__(self):
    return

  def tableAutoScroll(self):  
    if self.check_autoscroll.isChecked():
      QtCore.QTimer.singleShot(0, self.view_can_rx.scrollToBottom)

  def tableUpdate(self):  
    if self.tm.rowCount() < 5000:
      if self.table_timer.interval() > 100:
        self.table_timer.setInterval(100)
    else:
      if self.table_timer.interval() < 500:
        self.table_timer.setInterval(500)
    self.tm.updateRows()

  def btnUpdate(self):
    
    if self.cmd.is_open == False:
      self.is_open = False

    if self.is_open == True:
      self.btn_open.setEnabled(False)
      self.btn_close.setEnabled(True)

      self.check_open_canfd.setEnabled(False)
      self.check_open_brs.setEnabled(False)
      self.combo_open_rate_normal.setEnabled(False)
      self.combo_open_rate_data.setEnabled(False)
      self.combo_open_mode.setEnabled(False)
    else:
      self.btn_open.setEnabled(True)
      self.btn_close.setEnabled(False)

      self.check_open_canfd.setEnabled(True)
      self.combo_open_rate_normal.setEnabled(True)
      self.combo_open_mode.setEnabled(True)

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
    row_pos = self.table_can_tx.rowCount()

    for i in range(row_pos):
      sel = self.table_can_tx.cellWidget(i, self.can_tx_head["Sel"]).layout().itemAt(0).widget().isChecked()
      if sel == False:
        continue

      fd = self.table_can_tx.cellWidget(i, self.can_tx_head["FD"]).layout().itemAt(0).widget().isChecked()
      fd_brs = self.table_can_tx.cellWidget(i, self.can_tx_head["BRS"]).layout().itemAt(0).widget().isChecked()
      ext_id = self.table_can_tx.cellWidget(i, self.can_tx_head["Ext.ID"]).layout().itemAt(0).widget().isChecked()
      dlc = self.table_can_tx.cellWidget(i, self.can_tx_head["DLC"]).layout().itemAt(0).widget().currentIndex()
      length = int(self.dlc_str[dlc])

      can_msg = CmdCANPacket()
      can_msg.id = int(self.table_can_tx.item(i, self.can_tx_head["ID"]).text(), 16)
      
      if fd == True and fd_brs == True:
        can_msg.frame = CAN_FD_BRS  
      elif fd == True and fd_brs == False:
        can_msg.frame = CAN_FD_NO_BRS
      else:
        can_msg.frame = CAN_CLASSIC

      can_msg.dlc = dlc
      can_msg.length = length
      if ext_id == True:
        can_msg.id_type = CAN_EXT
      else:
        can_msg.id_type = CAN_STD

      data_str = self.table_can_tx.item(i, self.can_tx_head["Data"]).text()
      data_bytes = bytes.fromhex(data_str)
      index = len(data_bytes)
      can_msg.data[:index] = data_bytes[:index]
      for i in range(index, length):
        can_msg.data[i] = 0x00

      self.cmd_can.send(can_msg)


  def btnDel(self):
    items = self.table_can_tx.selectedIndexes()
    if len(items) > 0:
      self.table_can_tx.removeRow(items[0].row())
    return

  def insertCanTxLine(self):
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

    item = QTableWidgetItem("")
    self.table_can_tx.setItem(row_pos, self.can_tx_head["Data"], item)
    return True


  def btnAdd(self):
    id_text = self.text_tx_id.text().replace("0x", "")
    if len(id_text) == 0:
      self.log.println("ID Empty")
      return

    if self.is_hex(id_text) == False:
      self.log.println("ID Not Hex")
      return

    self.insertCanTxLine()
    self.table_can_tx.scrollToBottom()
    return True


  def updateCanTxTable(self, item):
    if item.column() == self.can_tx_head["ID"]:
      item_text = item.text().replace("0x","")
      if self.is_hex(item_text):        
        item.setText(item_text.upper())
      else:
        self.log.println("Not Hex")
        item.setText("")
    if item.column() == self.can_tx_head["Data"]:
      item.setText(self.getCanDataLine(item.text()))

    self.table_can_tx.resizeColumnToContents(self.can_tx_head["DLC"])
    self.table_can_tx.resizeColumnToContents(self.can_tx_head["ID"])
    self.table_can_tx.viewport().update()

  def is_hex(self, s):
     hex_digits = set(string.hexdigits)
     return all(c in hex_digits for c in s)

  def getCanDataLine(self, can_msg_line):
    line_list = can_msg_line.split(" ")
    out_line = ""
    for data in line_list:
      data = data.upper()
      if len(data) == 2 and self.is_hex(data):        
        out_line += data + " "

    return out_line

  def btnOpen(self):  
    if self.is_open == True:
      self.is_open = False
      self.btnUpdate()
      return

    if self.cmd.is_open:
      option = CmdCANOpen()
      option.mode = self.combo_open_mode.currentIndex()
      option.frame = CAN_CLASSIC
      if self.check_open_canfd.isChecked():
        if self.check_open_brs.isChecked():
          option.frame = CAN_FD_BRS
        else:
          option.frame = CAN_FD_NO_BRS
      option.baud = self.combo_open_rate_normal.currentIndex()
      option.baud_data = self.combo_open_rate_data.currentIndex()

      err_code, resp = self.cmd_can.open(option)
      if err_code == OK:
        self.is_open = True
        self.saveConfig()
      self.is_open = True
    else:
      self.log.println("Not Connected")
    self.btnUpdate()

    if self.is_open:
      err_code, resp = self.cmd_can.getFilter(self.can_filter)
      if err_code == OK:
        self.log.printLog("getFilter()")
        self.updateFromFilter()
      else:
        self.log.printLog("getFilter() Fail")

  def btnClose(self):  
    self.is_open = False
    if self.cmd.is_open:
      err_code, resp = self.cmd_can.close()
    else:
      self.log.println("Not Connected")
    self.btnUpdate()

  def btnClearRxMsg(self):
    self.view_can_rx.reset()
    self.tm.clear()

  def btnFilterSet(self):
    self.updateToFilter()
    if self.is_open:
      err_code, resp = self.cmd_can.setFilter(self.can_filter)
      if err_code == OK:
        self.log.printLog("setFilter()")
      else:
        self.log.printLog("setFilter() Fail")    

  def updateFromFilter(self):
    self.combo_filter_type.setCurrentIndex(self.can_filter.type)
    if self.can_filter.id_type == CAN_STD:
      self.radio_filter_std.setChecked(True)
      self.radio_filter_ext.setChecked(False)
    else:
      self.radio_filter_std.setChecked(False)
      self.radio_filter_ext.setChecked(True)

    self.line_filter_id1.setText(hex(self.can_filter.id1))
    self.line_filter_id2.setText(hex(self.can_filter.id2))

  def updateToFilter(self):
    self.can_filter.type = self.combo_filter_type.currentIndex()
    if self.radio_filter_std.isChecked():
      self.can_filter.id_type = CAN_STD
    else:
      self.can_filter.id_type = CAN_EXT

    self.can_filter.id1 = int(self.line_filter_id1.text().replace("0x", ""), 16)
    self.can_filter.id2 = int(self.line_filter_id2.text().replace("0x", ""), 16)

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
      self.combo_open_mode.setCurrentIndex(int(self.config_item['can_open_mode']))
      self.combo_tx_dlc.setCurrentIndex(int(self.config_item['can_tx_dlc']))
      self.text_tx_id.setText(self.config_item['can_tx_id'])
    except Exception as e:
      print(e)

  def saveConfig(self):  
    self.config_item['can_open_fd'] = str(self.check_open_canfd.isChecked())
    self.config_item['can_open_brs'] = str(self.check_open_brs.isChecked())
    self.config_item['can_open_rate_normal'] = str(self.combo_open_rate_normal.currentIndex())
    self.config_item['can_open_rate_data'] = str(self.combo_open_rate_data.currentIndex())
    self.config_item['can_open_mode'] = str(self.combo_open_mode.currentIndex())
    self.config_item['can_tx_extid'] = str(self.check_tx_extid.isChecked())
    self.config_item['can_tx_brs'] = str(self.check_tx_brs.isChecked())
    self.config_item['can_tx_dlc'] = str(self.combo_tx_dlc.currentIndex())
    self.config_item['can_tx_id'] = self.text_tx_id.text()
    self.saveSendMsg()    

  def loadSendMsg(self):
    if os.path.exists("can_send.csv") == False:
      return

    with open('can_send.csv', 'r', newline='') as csvfile:
      check = {"True":True, "False":False}
      
      reader = csv.reader(csvfile)
      first_line = True
      for line in reader:
        if first_line:
          first_line = False
          continue

        self.insertCanTxLine()
        row_pos = self.table_can_tx.rowCount() - 1
        self.table_can_tx.cellWidget(row_pos, 0).layout().itemAt(0).widget().setChecked(check[line[0]])
        self.table_can_tx.item(row_pos, 1).setText(line[1])
        self.table_can_tx.cellWidget(row_pos, 2).layout().itemAt(0).widget().setChecked(check[line[2]])
        self.table_can_tx.cellWidget(row_pos, 3).layout().itemAt(0).widget().setChecked(check[line[3]])
        self.table_can_tx.cellWidget(row_pos, 4).layout().itemAt(0).widget().setChecked(check[line[4]])
        self.table_can_tx.cellWidget(row_pos, 5).layout().itemAt(0).widget().setCurrentIndex(int(line[5]))
        self.table_can_tx.item(row_pos, 6).setText(line[6])

  def saveSendMsg(self):
    columns = range(self.table_can_tx.columnCount())
    header = [self.table_can_tx.horizontalHeaderItem(column).text() 
              for column in columns]

    with open('can_send.csv', 'w', encoding='utf-8', newline='') as csvfile:
      writer = csv.writer(csvfile)
      writer.writerow(header)
      for row in range(self.table_can_tx.rowCount()):
        out_line = []
        out_line.append(str(self.table_can_tx.cellWidget(row, 0).layout().itemAt(0).widget().isChecked()))
        out_line.append(self.table_can_tx.item(row, 1).text())
        out_line.append(str(self.table_can_tx.cellWidget(row, 2).layout().itemAt(0).widget().isChecked()))
        out_line.append(str(self.table_can_tx.cellWidget(row, 3).layout().itemAt(0).widget().isChecked()))
        out_line.append(str(self.table_can_tx.cellWidget(row, 4).layout().itemAt(0).widget().isChecked()))
        out_line.append(str(self.table_can_tx.cellWidget(row, 5).layout().itemAt(0).widget().currentIndex()))
        if self.table_can_tx.item(row, 6) is not None:
          out_line.append(self.table_can_tx.item(row, 6).text())
        else:
          out_line.append("")

        writer.writerow(out_line)      

  def receiveCanMsg(self, packet: CmdPacket):
    can_msg = CmdCANPacket()
    can_msg.setCmdPacket(packet)
    
    msg_item = []
    msg_item.append(str(can_msg.timestamp))
    msg_item.append(str(hex(can_msg.id)))
    msg_item.append("rx")

    if can_msg.id_type == 0:
      frame_str = "STD "
    else:
      frame_str = "EXT "

    if can_msg.frame == CAN_CLASSIC:
      frame_str += ""
    elif can_msg.frame == CAN_FD_NO_BRS:
      frame_str += "FD"
    else:
      frame_str += "FD BRS"
    msg_item.append(frame_str)

    msg_item.append(self.dlc_str[can_msg.dlc])
    msg_item.append(can_msg.data[:can_msg.length].hex(" "))

    self.tm.addDataOnly(msg_item)
    return
    
    
  def rxdEvent(self, packet: CmdPacket):
    if packet.cmd == self.cmd_can.CMD_CAN_DATA:
      if self.is_open == True:
        self.receiveCanMsg(packet)