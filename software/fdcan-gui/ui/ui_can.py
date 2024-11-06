# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'ui_can.ui'
##
## Created by: Qt User Interface Compiler version 6.5.0
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QAbstractItemView, QApplication, QCheckBox, QComboBox,
    QFrame, QHBoxLayout, QHeaderView, QLabel,
    QLineEdit, QPushButton, QRadioButton, QSizePolicy,
    QSpacerItem, QSpinBox, QSplitter, QTableView,
    QTableWidget, QTableWidgetItem, QVBoxLayout, QWidget)

class Ui_CAN(object):
    def setupUi(self, CAN):
        if not CAN.objectName():
            CAN.setObjectName(u"CAN")
        CAN.resize(864, 632)
        self.verticalLayout_3 = QVBoxLayout(CAN)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.splitter_2 = QSplitter(CAN)
        self.splitter_2.setObjectName(u"splitter_2")
        self.splitter_2.setAutoFillBackground(False)
        self.splitter_2.setFrameShape(QFrame.NoFrame)
        self.splitter_2.setLineWidth(1)
        self.splitter_2.setMidLineWidth(0)
        self.splitter_2.setOrientation(Qt.Vertical)
        self.frame = QFrame(self.splitter_2)
        self.frame.setObjectName(u"frame")
        sizePolicy = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.frame.sizePolicy().hasHeightForWidth())
        self.frame.setSizePolicy(sizePolicy)
        self.frame.setMinimumSize(QSize(0, 0))
        self.frame.setFrameShape(QFrame.NoFrame)
        self.frame.setFrameShadow(QFrame.Raised)
        self.frame.setLineWidth(0)
        self.verticalLayout = QVBoxLayout(self.frame)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(-1, 0, -1, -1)
        self.combo_open_mode = QComboBox(self.frame)
        self.combo_open_mode.addItem("")
        self.combo_open_mode.addItem("")
        self.combo_open_mode.addItem("")
        self.combo_open_mode.setObjectName(u"combo_open_mode")
        sizePolicy1 = QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.combo_open_mode.sizePolicy().hasHeightForWidth())
        self.combo_open_mode.setSizePolicy(sizePolicy1)

        self.horizontalLayout.addWidget(self.combo_open_mode)

        self.check_open_canfd = QCheckBox(self.frame)
        self.check_open_canfd.setObjectName(u"check_open_canfd")

        self.horizontalLayout.addWidget(self.check_open_canfd)

        self.check_open_brs = QCheckBox(self.frame)
        self.check_open_brs.setObjectName(u"check_open_brs")

        self.horizontalLayout.addWidget(self.check_open_brs)

        self.line = QFrame(self.frame)
        self.line.setObjectName(u"line")
        self.line.setFrameShape(QFrame.VLine)
        self.line.setFrameShadow(QFrame.Sunken)

        self.horizontalLayout.addWidget(self.line)

        self.label = QLabel(self.frame)
        self.label.setObjectName(u"label")

        self.horizontalLayout.addWidget(self.label)

        self.combo_open_rate_normal = QComboBox(self.frame)
        self.combo_open_rate_normal.setObjectName(u"combo_open_rate_normal")

        self.horizontalLayout.addWidget(self.combo_open_rate_normal)

        self.label_2 = QLabel(self.frame)
        self.label_2.setObjectName(u"label_2")

        self.horizontalLayout.addWidget(self.label_2)

        self.combo_open_rate_data = QComboBox(self.frame)
        self.combo_open_rate_data.setObjectName(u"combo_open_rate_data")

        self.horizontalLayout.addWidget(self.combo_open_rate_data)

        self.line_5 = QFrame(self.frame)
        self.line_5.setObjectName(u"line_5")
        self.line_5.setFrameShape(QFrame.VLine)
        self.line_5.setFrameShadow(QFrame.Sunken)

        self.horizontalLayout.addWidget(self.line_5)

        self.btn_open = QPushButton(self.frame)
        self.btn_open.setObjectName(u"btn_open")

        self.horizontalLayout.addWidget(self.btn_open)

        self.btn_close = QPushButton(self.frame)
        self.btn_close.setObjectName(u"btn_close")

        self.horizontalLayout.addWidget(self.btn_close)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)


        self.verticalLayout.addLayout(self.horizontalLayout)

        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.horizontalLayout_3.setContentsMargins(-1, 0, -1, -1)
        self.combo_filter_type = QComboBox(self.frame)
        self.combo_filter_type.addItem("")
        self.combo_filter_type.addItem("")
        self.combo_filter_type.setObjectName(u"combo_filter_type")
        sizePolicy1.setHeightForWidth(self.combo_filter_type.sizePolicy().hasHeightForWidth())
        self.combo_filter_type.setSizePolicy(sizePolicy1)

        self.horizontalLayout_3.addWidget(self.combo_filter_type)

        self.radio_filter_std = QRadioButton(self.frame)
        self.radio_filter_std.setObjectName(u"radio_filter_std")
        self.radio_filter_std.setChecked(True)

        self.horizontalLayout_3.addWidget(self.radio_filter_std)

        self.radio_filter_ext = QRadioButton(self.frame)
        self.radio_filter_ext.setObjectName(u"radio_filter_ext")

        self.horizontalLayout_3.addWidget(self.radio_filter_ext)

        self.line_4 = QFrame(self.frame)
        self.line_4.setObjectName(u"line_4")
        self.line_4.setFrameShape(QFrame.VLine)
        self.line_4.setFrameShadow(QFrame.Sunken)

        self.horizontalLayout_3.addWidget(self.line_4)

        self.label_6 = QLabel(self.frame)
        self.label_6.setObjectName(u"label_6")

        self.horizontalLayout_3.addWidget(self.label_6)

        self.line_filter_id1 = QLineEdit(self.frame)
        self.line_filter_id1.setObjectName(u"line_filter_id1")
        sizePolicy1.setHeightForWidth(self.line_filter_id1.sizePolicy().hasHeightForWidth())
        self.line_filter_id1.setSizePolicy(sizePolicy1)

        self.horizontalLayout_3.addWidget(self.line_filter_id1)

        self.label_7 = QLabel(self.frame)
        self.label_7.setObjectName(u"label_7")

        self.horizontalLayout_3.addWidget(self.label_7)

        self.line_filter_id2 = QLineEdit(self.frame)
        self.line_filter_id2.setObjectName(u"line_filter_id2")
        sizePolicy1.setHeightForWidth(self.line_filter_id2.sizePolicy().hasHeightForWidth())
        self.line_filter_id2.setSizePolicy(sizePolicy1)

        self.horizontalLayout_3.addWidget(self.line_filter_id2)

        self.btn_filter_set = QPushButton(self.frame)
        self.btn_filter_set.setObjectName(u"btn_filter_set")

        self.horizontalLayout_3.addWidget(self.btn_filter_set)

        self.horizontalSpacer_3 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout_3.addItem(self.horizontalSpacer_3)


        self.verticalLayout.addLayout(self.horizontalLayout_3)

        self.view_can_rx = QTableView(self.frame)
        self.view_can_rx.setObjectName(u"view_can_rx")
        sizePolicy2 = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.view_can_rx.sizePolicy().hasHeightForWidth())
        self.view_can_rx.setSizePolicy(sizePolicy2)

        self.verticalLayout.addWidget(self.view_can_rx)

        self.horizontalLayout_4 = QHBoxLayout()
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.horizontalLayout_4.setContentsMargins(-1, 0, -1, -1)
        self.check_autoscroll = QCheckBox(self.frame)
        self.check_autoscroll.setObjectName(u"check_autoscroll")
        self.check_autoscroll.setChecked(True)

        self.horizontalLayout_4.addWidget(self.check_autoscroll)

        self.btn_clear_rx_msg = QPushButton(self.frame)
        self.btn_clear_rx_msg.setObjectName(u"btn_clear_rx_msg")

        self.horizontalLayout_4.addWidget(self.btn_clear_rx_msg)

        self.line_2 = QFrame(self.frame)
        self.line_2.setObjectName(u"line_2")
        self.line_2.setFrameShape(QFrame.VLine)
        self.line_2.setFrameShadow(QFrame.Sunken)

        self.horizontalLayout_4.addWidget(self.line_2)

        self.btn_save_rx = QPushButton(self.frame)
        self.btn_save_rx.setObjectName(u"btn_save_rx")

        self.horizontalLayout_4.addWidget(self.btn_save_rx)

        self.horizontalSpacer_4 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout_4.addItem(self.horizontalSpacer_4)


        self.verticalLayout.addLayout(self.horizontalLayout_4)

        self.splitter_2.addWidget(self.frame)
        self.frame_2 = QFrame(self.splitter_2)
        self.frame_2.setObjectName(u"frame_2")
        sizePolicy.setHeightForWidth(self.frame_2.sizePolicy().hasHeightForWidth())
        self.frame_2.setSizePolicy(sizePolicy)
        self.frame_2.setMinimumSize(QSize(0, 0))
        self.frame_2.setBaseSize(QSize(0, 0))
        self.frame_2.setFrameShape(QFrame.NoFrame)
        self.frame_2.setFrameShadow(QFrame.Raised)
        self.frame_2.setLineWidth(0)
        self.verticalLayout_2 = QVBoxLayout(self.frame_2)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.verticalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalLayout_2.setContentsMargins(-1, 0, -1, -1)
        self.check_tx_extid = QCheckBox(self.frame_2)
        self.check_tx_extid.setObjectName(u"check_tx_extid")

        self.horizontalLayout_2.addWidget(self.check_tx_extid)

        self.check_tx_fd = QCheckBox(self.frame_2)
        self.check_tx_fd.setObjectName(u"check_tx_fd")

        self.horizontalLayout_2.addWidget(self.check_tx_fd)

        self.check_tx_brs = QCheckBox(self.frame_2)
        self.check_tx_brs.setObjectName(u"check_tx_brs")

        self.horizontalLayout_2.addWidget(self.check_tx_brs)

        self.line_3 = QFrame(self.frame_2)
        self.line_3.setObjectName(u"line_3")
        self.line_3.setFrameShape(QFrame.VLine)
        self.line_3.setFrameShadow(QFrame.Sunken)

        self.horizontalLayout_2.addWidget(self.line_3)

        self.label_3 = QLabel(self.frame_2)
        self.label_3.setObjectName(u"label_3")

        self.horizontalLayout_2.addWidget(self.label_3)

        self.text_tx_id = QLineEdit(self.frame_2)
        self.text_tx_id.setObjectName(u"text_tx_id")
        sizePolicy1.setHeightForWidth(self.text_tx_id.sizePolicy().hasHeightForWidth())
        self.text_tx_id.setSizePolicy(sizePolicy1)
        self.text_tx_id.setMinimumSize(QSize(0, 0))

        self.horizontalLayout_2.addWidget(self.text_tx_id)

        self.label_4 = QLabel(self.frame_2)
        self.label_4.setObjectName(u"label_4")

        self.horizontalLayout_2.addWidget(self.label_4)

        self.combo_tx_dlc = QComboBox(self.frame_2)
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.addItem("")
        self.combo_tx_dlc.setObjectName(u"combo_tx_dlc")
        sizePolicy1.setHeightForWidth(self.combo_tx_dlc.sizePolicy().hasHeightForWidth())
        self.combo_tx_dlc.setSizePolicy(sizePolicy1)

        self.horizontalLayout_2.addWidget(self.combo_tx_dlc)

        self.btn_add = QPushButton(self.frame_2)
        self.btn_add.setObjectName(u"btn_add")

        self.horizontalLayout_2.addWidget(self.btn_add)

        self.btn_del = QPushButton(self.frame_2)
        self.btn_del.setObjectName(u"btn_del")

        self.horizontalLayout_2.addWidget(self.btn_del)

        self.btn_send = QPushButton(self.frame_2)
        self.btn_send.setObjectName(u"btn_send")
        self.btn_send.setStyleSheet(u"")

        self.horizontalLayout_2.addWidget(self.btn_send)

        self.btn_repeat = QPushButton(self.frame_2)
        self.btn_repeat.setObjectName(u"btn_repeat")
        self.btn_repeat.setStyleSheet(u"")

        self.horizontalLayout_2.addWidget(self.btn_repeat)

        self.repeat_ms = QSpinBox(self.frame_2)
        self.repeat_ms.setObjectName(u"repeat_ms")
        self.repeat_ms.setEnabled(True)
        self.repeat_ms.setMinimum(100)
        self.repeat_ms.setMaximum(1000000)
        self.repeat_ms.setSingleStep(100)
        self.repeat_ms.setValue(1000)

        self.horizontalLayout_2.addWidget(self.repeat_ms)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout_2.addItem(self.horizontalSpacer_2)


        self.verticalLayout_2.addLayout(self.horizontalLayout_2)

        self.table_can_tx = QTableWidget(self.frame_2)
        if (self.table_can_tx.columnCount() < 7):
            self.table_can_tx.setColumnCount(7)
        __qtablewidgetitem = QTableWidgetItem()
        self.table_can_tx.setHorizontalHeaderItem(0, __qtablewidgetitem)
        __qtablewidgetitem1 = QTableWidgetItem()
        self.table_can_tx.setHorizontalHeaderItem(1, __qtablewidgetitem1)
        __qtablewidgetitem2 = QTableWidgetItem()
        self.table_can_tx.setHorizontalHeaderItem(2, __qtablewidgetitem2)
        __qtablewidgetitem3 = QTableWidgetItem()
        self.table_can_tx.setHorizontalHeaderItem(3, __qtablewidgetitem3)
        __qtablewidgetitem4 = QTableWidgetItem()
        self.table_can_tx.setHorizontalHeaderItem(4, __qtablewidgetitem4)
        __qtablewidgetitem5 = QTableWidgetItem()
        self.table_can_tx.setHorizontalHeaderItem(5, __qtablewidgetitem5)
        __qtablewidgetitem6 = QTableWidgetItem()
        self.table_can_tx.setHorizontalHeaderItem(6, __qtablewidgetitem6)
        self.table_can_tx.setObjectName(u"table_can_tx")
        sizePolicy3 = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Expanding)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.table_can_tx.sizePolicy().hasHeightForWidth())
        self.table_can_tx.setSizePolicy(sizePolicy3)
        self.table_can_tx.setMinimumSize(QSize(0, 0))
        self.table_can_tx.setMaximumSize(QSize(16777215, 16777215))
        self.table_can_tx.setAlternatingRowColors(True)
        self.table_can_tx.setSelectionMode(QAbstractItemView.NoSelection)
        self.table_can_tx.setSelectionBehavior(QAbstractItemView.SelectItems)
        self.table_can_tx.setRowCount(0)
        self.table_can_tx.horizontalHeader().setCascadingSectionResizes(False)
        self.table_can_tx.horizontalHeader().setMinimumSectionSize(50)
        self.table_can_tx.horizontalHeader().setDefaultSectionSize(50)
        self.table_can_tx.horizontalHeader().setStretchLastSection(True)

        self.verticalLayout_2.addWidget(self.table_can_tx)

        self.splitter_2.addWidget(self.frame_2)

        self.verticalLayout_3.addWidget(self.splitter_2)


        self.retranslateUi(CAN)

        QMetaObject.connectSlotsByName(CAN)
    # setupUi

    def retranslateUi(self, CAN):
        CAN.setWindowTitle(QCoreApplication.translate("CAN", u"Form", None))
        self.combo_open_mode.setItemText(0, QCoreApplication.translate("CAN", u"NORMAL", None))
        self.combo_open_mode.setItemText(1, QCoreApplication.translate("CAN", u"MONITOR", None))
        self.combo_open_mode.setItemText(2, QCoreApplication.translate("CAN", u"LOOPBACK", None))

        self.check_open_canfd.setText(QCoreApplication.translate("CAN", u"FD ", None))
        self.check_open_brs.setText(QCoreApplication.translate("CAN", u"BRS", None))
        self.label.setText(QCoreApplication.translate("CAN", u"Normal", None))
        self.label_2.setText(QCoreApplication.translate("CAN", u"Data", None))
        self.btn_open.setText(QCoreApplication.translate("CAN", u"Open", None))
        self.btn_close.setText(QCoreApplication.translate("CAN", u"Close", None))
        self.combo_filter_type.setItemText(0, QCoreApplication.translate("CAN", u"MASK     ", None))
        self.combo_filter_type.setItemText(1, QCoreApplication.translate("CAN", u"RANGE", None))

        self.radio_filter_std.setText(QCoreApplication.translate("CAN", u"STD", None))
        self.radio_filter_ext.setText(QCoreApplication.translate("CAN", u"EXT", None))
        self.label_6.setText(QCoreApplication.translate("CAN", u"ID", None))
        self.label_7.setText(QCoreApplication.translate("CAN", u"MASK", None))
        self.btn_filter_set.setText(QCoreApplication.translate("CAN", u"Set", None))
        self.check_autoscroll.setText(QCoreApplication.translate("CAN", u"Auto Scroll", None))
        self.btn_clear_rx_msg.setText(QCoreApplication.translate("CAN", u"Clear", None))
        self.btn_save_rx.setText(QCoreApplication.translate("CAN", u"Save", None))
        self.check_tx_extid.setText(QCoreApplication.translate("CAN", u"Ext.ID", None))
        self.check_tx_fd.setText(QCoreApplication.translate("CAN", u"FD", None))
        self.check_tx_brs.setText(QCoreApplication.translate("CAN", u"BRS", None))
        self.label_3.setText(QCoreApplication.translate("CAN", u"ID", None))
        self.label_4.setText(QCoreApplication.translate("CAN", u"DLC", None))
        self.combo_tx_dlc.setItemText(0, QCoreApplication.translate("CAN", u"0", None))
        self.combo_tx_dlc.setItemText(1, QCoreApplication.translate("CAN", u"1", None))
        self.combo_tx_dlc.setItemText(2, QCoreApplication.translate("CAN", u"2", None))
        self.combo_tx_dlc.setItemText(3, QCoreApplication.translate("CAN", u"3", None))
        self.combo_tx_dlc.setItemText(4, QCoreApplication.translate("CAN", u"4", None))
        self.combo_tx_dlc.setItemText(5, QCoreApplication.translate("CAN", u"5", None))
        self.combo_tx_dlc.setItemText(6, QCoreApplication.translate("CAN", u"6", None))
        self.combo_tx_dlc.setItemText(7, QCoreApplication.translate("CAN", u"7", None))
        self.combo_tx_dlc.setItemText(8, QCoreApplication.translate("CAN", u"8", None))
        self.combo_tx_dlc.setItemText(9, QCoreApplication.translate("CAN", u"12", None))
        self.combo_tx_dlc.setItemText(10, QCoreApplication.translate("CAN", u"16", None))
        self.combo_tx_dlc.setItemText(11, QCoreApplication.translate("CAN", u"20", None))
        self.combo_tx_dlc.setItemText(12, QCoreApplication.translate("CAN", u"24", None))
        self.combo_tx_dlc.setItemText(13, QCoreApplication.translate("CAN", u"32", None))
        self.combo_tx_dlc.setItemText(14, QCoreApplication.translate("CAN", u"48", None))
        self.combo_tx_dlc.setItemText(15, QCoreApplication.translate("CAN", u"64", None))

        self.btn_add.setText(QCoreApplication.translate("CAN", u"Add", None))
        self.btn_del.setText(QCoreApplication.translate("CAN", u"Del", None))
        self.btn_send.setText(QCoreApplication.translate("CAN", u"Send", None))
        self.btn_repeat.setText(QCoreApplication.translate("CAN", u"Repeat", None))
        self.repeat_ms.setSuffix(QCoreApplication.translate("CAN", u" ms", None))
        ___qtablewidgetitem = self.table_can_tx.horizontalHeaderItem(0)
        ___qtablewidgetitem.setText(QCoreApplication.translate("CAN", u"Sel", None));
        ___qtablewidgetitem1 = self.table_can_tx.horizontalHeaderItem(1)
        ___qtablewidgetitem1.setText(QCoreApplication.translate("CAN", u"ID", None));
        ___qtablewidgetitem2 = self.table_can_tx.horizontalHeaderItem(2)
        ___qtablewidgetitem2.setText(QCoreApplication.translate("CAN", u"Ext.ID", None));
        ___qtablewidgetitem3 = self.table_can_tx.horizontalHeaderItem(3)
        ___qtablewidgetitem3.setText(QCoreApplication.translate("CAN", u"FD", None));
        ___qtablewidgetitem4 = self.table_can_tx.horizontalHeaderItem(4)
        ___qtablewidgetitem4.setText(QCoreApplication.translate("CAN", u"BRS", None));
        ___qtablewidgetitem5 = self.table_can_tx.horizontalHeaderItem(5)
        ___qtablewidgetitem5.setText(QCoreApplication.translate("CAN", u"DLC", None));
        ___qtablewidgetitem6 = self.table_can_tx.horizontalHeaderItem(6)
        ___qtablewidgetitem6.setText(QCoreApplication.translate("CAN", u"Data", None));
    # retranslateUi

