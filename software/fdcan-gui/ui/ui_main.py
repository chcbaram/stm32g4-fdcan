# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'ui_main.ui'
##
## Created by: Qt User Interface Compiler version 6.5.0
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QAction, QBrush, QColor, QConicalGradient,
    QCursor, QFont, QFontDatabase, QGradient,
    QIcon, QImage, QKeySequence, QLinearGradient,
    QPainter, QPalette, QPixmap, QRadialGradient,
    QTransform)
from PySide6.QtWidgets import (QApplication, QComboBox, QFrame, QHBoxLayout,
    QMainWindow, QMenu, QMenuBar, QPlainTextEdit,
    QProgressBar, QPushButton, QRadioButton, QSizePolicy,
    QSpacerItem, QStatusBar, QTabWidget, QVBoxLayout,
    QWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(900, 562)
        font = QFont()
        font.setPointSize(14)
        MainWindow.setFont(font)
        self.actionForce_Exit = QAction(MainWindow)
        self.actionForce_Exit.setObjectName(u"actionForce_Exit")
        self.action10 = QAction(MainWindow)
        self.action10.setObjectName(u"action10")
        self.action10.setCheckable(True)
        self.action11 = QAction(MainWindow)
        self.action11.setObjectName(u"action11")
        self.action11.setCheckable(True)
        self.action12 = QAction(MainWindow)
        self.action12.setObjectName(u"action12")
        self.action12.setCheckable(True)
        self.action13 = QAction(MainWindow)
        self.action13.setObjectName(u"action13")
        self.action13.setCheckable(True)
        self.action14 = QAction(MainWindow)
        self.action14.setObjectName(u"action14")
        self.action14.setCheckable(True)
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.horizontalLayout = QHBoxLayout(self.centralwidget)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.frame = QFrame(self.centralwidget)
        self.frame.setObjectName(u"frame")
        sizePolicy = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.frame.sizePolicy().hasHeightForWidth())
        self.frame.setSizePolicy(sizePolicy)
        self.frame.setMinimumSize(QSize(200, 0))
        self.frame.setMaximumSize(QSize(100, 16777215))
        self.frame.setFrameShape(QFrame.StyledPanel)
        self.frame.setFrameShadow(QFrame.Raised)
        self.verticalLayout_2 = QVBoxLayout(self.frame)
#ifndef Q_OS_MAC
        self.verticalLayout_2.setSpacing(-1)
#endif
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.btn_scan = QPushButton(self.frame)
        self.btn_scan.setObjectName(u"btn_scan")

        self.verticalLayout_2.addWidget(self.btn_scan)

        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.radio_scan_fdcan = QRadioButton(self.frame)
        self.radio_scan_fdcan.setObjectName(u"radio_scan_fdcan")

        self.horizontalLayout_3.addWidget(self.radio_scan_fdcan)

        self.radio_scan_uart = QRadioButton(self.frame)
        self.radio_scan_uart.setObjectName(u"radio_scan_uart")

        self.horizontalLayout_3.addWidget(self.radio_scan_uart)


        self.verticalLayout_2.addLayout(self.horizontalLayout_3)

        self.combo_device = QComboBox(self.frame)
        self.combo_device.setObjectName(u"combo_device")

        self.verticalLayout_2.addWidget(self.combo_device)

        self.btn_connect = QPushButton(self.frame)
        self.btn_connect.setObjectName(u"btn_connect")

        self.verticalLayout_2.addWidget(self.btn_connect)

        self.text_device_info = QPlainTextEdit(self.frame)
        self.text_device_info.setObjectName(u"text_device_info")
        sizePolicy1 = QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Minimum)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.text_device_info.sizePolicy().hasHeightForWidth())
        self.text_device_info.setSizePolicy(sizePolicy1)
        self.text_device_info.setMinimumSize(QSize(140, 0))
        self.text_device_info.setLineWrapMode(QPlainTextEdit.NoWrap)
        self.text_device_info.setReadOnly(True)
        self.text_device_info.setCursorWidth(0)

        self.verticalLayout_2.addWidget(self.text_device_info)

        self.system_log = QPlainTextEdit(self.frame)
        self.system_log.setObjectName(u"system_log")

        self.verticalLayout_2.addWidget(self.system_log)

        self.btn_syslog_clear = QPushButton(self.frame)
        self.btn_syslog_clear.setObjectName(u"btn_syslog_clear")

        self.verticalLayout_2.addWidget(self.btn_syslog_clear)


        self.horizontalLayout.addWidget(self.frame)

        self.tabWidget = QTabWidget(self.centralwidget)
        self.tabWidget.setObjectName(u"tabWidget")
        self.tab_update = QWidget()
        self.tab_update.setObjectName(u"tab_update")
        self.verticalLayout_3 = QVBoxLayout(self.tab_update)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.frame_2 = QFrame(self.tab_update)
        self.frame_2.setObjectName(u"frame_2")
        self.frame_2.setMinimumSize(QSize(0, 300))
        self.frame_2.setFrameShape(QFrame.StyledPanel)
        self.frame_2.setFrameShadow(QFrame.Raised)
        self.verticalLayout_4 = QVBoxLayout(self.frame_2)
#ifndef Q_OS_MAC
        self.verticalLayout_4.setSpacing(-1)
#endif
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.verticalLayout_4.setContentsMargins(-1, 12, -1, -1)
        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.combo_file = QComboBox(self.frame_2)
        self.combo_file.setObjectName(u"combo_file")

        self.horizontalLayout_2.addWidget(self.combo_file)

        self.btn_open = QPushButton(self.frame_2)
        self.btn_open.setObjectName(u"btn_open")
        sizePolicy2 = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.btn_open.sizePolicy().hasHeightForWidth())
        self.btn_open.setSizePolicy(sizePolicy2)
        self.btn_open.setMinimumSize(QSize(0, 0))

        self.horizontalLayout_2.addWidget(self.btn_open)

        self.btn_led = QPushButton(self.frame_2)
        self.btn_led.setObjectName(u"btn_led")
        sizePolicy2.setHeightForWidth(self.btn_led.sizePolicy().hasHeightForWidth())
        self.btn_led.setSizePolicy(sizePolicy2)

        self.horizontalLayout_2.addWidget(self.btn_led)


        self.verticalLayout_4.addLayout(self.horizontalLayout_2)

        self.horizontalLayout_4 = QHBoxLayout()
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.horizontalLayout_4.setContentsMargins(0, 0, -1, -1)
        self.btn_down = QPushButton(self.frame_2)
        self.btn_down.setObjectName(u"btn_down")
        sizePolicy3 = QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Fixed)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.btn_down.sizePolicy().hasHeightForWidth())
        self.btn_down.setSizePolicy(sizePolicy3)

        self.horizontalLayout_4.addWidget(self.btn_down)

        self.btn_down_stop = QPushButton(self.frame_2)
        self.btn_down_stop.setObjectName(u"btn_down_stop")

        self.horizontalLayout_4.addWidget(self.btn_down_stop)

        self.btn_log_clear = QPushButton(self.frame_2)
        self.btn_log_clear.setObjectName(u"btn_log_clear")

        self.horizontalLayout_4.addWidget(self.btn_log_clear)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout_4.addItem(self.horizontalSpacer)


        self.verticalLayout_4.addLayout(self.horizontalLayout_4)

        self.prog_bar_down = QProgressBar(self.frame_2)
        self.prog_bar_down.setObjectName(u"prog_bar_down")
        sizePolicy4 = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        sizePolicy4.setHorizontalStretch(0)
        sizePolicy4.setVerticalStretch(0)
        sizePolicy4.setHeightForWidth(self.prog_bar_down.sizePolicy().hasHeightForWidth())
        self.prog_bar_down.setSizePolicy(sizePolicy4)
        self.prog_bar_down.setMinimumSize(QSize(0, 0))
        self.prog_bar_down.setValue(0)

        self.verticalLayout_4.addWidget(self.prog_bar_down)

        self.log_text = QPlainTextEdit(self.frame_2)
        self.log_text.setObjectName(u"log_text")

        self.verticalLayout_4.addWidget(self.log_text)


        self.verticalLayout_3.addWidget(self.frame_2)

        self.tabWidget.addTab(self.tab_update, "")

        self.horizontalLayout.addWidget(self.tabWidget)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 900, 24))
        self.menuFile = QMenu(self.menubar)
        self.menuFile.setObjectName(u"menuFile")
        self.menuLog = QMenu(self.menubar)
        self.menuLog.setObjectName(u"menuLog")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.menubar.addAction(self.menuFile.menuAction())
        self.menubar.addAction(self.menuLog.menuAction())
        self.menuFile.addAction(self.actionForce_Exit)
        self.menuLog.addAction(self.action10)
        self.menuLog.addAction(self.action11)
        self.menuLog.addAction(self.action12)
        self.menuLog.addAction(self.action13)
        self.menuLog.addAction(self.action14)

        self.retranslateUi(MainWindow)

        self.tabWidget.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"FDCAN-GUI", None))
        self.actionForce_Exit.setText(QCoreApplication.translate("MainWindow", u"Force Exit", None))
        self.action10.setText(QCoreApplication.translate("MainWindow", u"10", None))
        self.action11.setText(QCoreApplication.translate("MainWindow", u"11", None))
        self.action12.setText(QCoreApplication.translate("MainWindow", u"12", None))
        self.action13.setText(QCoreApplication.translate("MainWindow", u"13", None))
        self.action14.setText(QCoreApplication.translate("MainWindow", u"14", None))
        self.btn_scan.setText(QCoreApplication.translate("MainWindow", u"Scan", None))
        self.radio_scan_fdcan.setText(QCoreApplication.translate("MainWindow", u"FDCAN", None))
        self.radio_scan_uart.setText(QCoreApplication.translate("MainWindow", u"UART", None))
        self.btn_connect.setText(QCoreApplication.translate("MainWindow", u"Connect", None))
        self.btn_syslog_clear.setText(QCoreApplication.translate("MainWindow", u"Clear", None))
        self.btn_open.setText(QCoreApplication.translate("MainWindow", u"Open", None))
        self.btn_led.setText(QCoreApplication.translate("MainWindow", u"LED", None))
        self.btn_down.setText(QCoreApplication.translate("MainWindow", u"Download", None))
        self.btn_down_stop.setText(QCoreApplication.translate("MainWindow", u"Stop", None))
        self.btn_log_clear.setText(QCoreApplication.translate("MainWindow", u"Clear", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_update), QCoreApplication.translate("MainWindow", u"UPDATE", None))
        self.menuFile.setTitle(QCoreApplication.translate("MainWindow", u"File", None))
        self.menuLog.setTitle(QCoreApplication.translate("MainWindow", u"Font", None))
    # retranslateUi

