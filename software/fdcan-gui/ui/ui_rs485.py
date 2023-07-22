# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'ui_rs485.ui'
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
from PySide6.QtWidgets import (QApplication, QComboBox, QHBoxLayout, QLabel,
    QLineEdit, QPushButton, QSizePolicy, QSpacerItem,
    QTextEdit, QVBoxLayout, QWidget)

class Ui_RS485(object):
    def setupUi(self, RS485):
        if not RS485.objectName():
            RS485.setObjectName(u"RS485")
        RS485.resize(573, 547)
        sizePolicy = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(RS485.sizePolicy().hasHeightForWidth())
        RS485.setSizePolicy(sizePolicy)
        self.verticalLayout_2 = QVBoxLayout(RS485)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.verticalLayout = QVBoxLayout()
#ifndef Q_OS_MAC
        self.verticalLayout.setSpacing(-1)
#endif
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(-1, 0, -1, -1)
        self.horizontalLayout = QHBoxLayout()
#ifndef Q_OS_MAC
        self.horizontalLayout.setSpacing(-1)
#endif
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(-1, 0, -1, -1)
        self.label = QLabel(RS485)
        self.label.setObjectName(u"label")

        self.horizontalLayout.addWidget(self.label)

        self.comboBox = QComboBox(RS485)
        self.comboBox.setObjectName(u"comboBox")
        self.comboBox.setMinimumSize(QSize(300, 0))

        self.horizontalLayout.addWidget(self.comboBox)

        self.btn_open = QPushButton(RS485)
        self.btn_open.setObjectName(u"btn_open")

        self.horizontalLayout.addWidget(self.btn_open)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)


        self.verticalLayout.addLayout(self.horizontalLayout)

        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.horizontalLayout_3.setContentsMargins(-1, 0, -1, -1)
        self.label_2 = QLabel(RS485)
        self.label_2.setObjectName(u"label_2")
        sizePolicy1 = QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Preferred)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.label_2.sizePolicy().hasHeightForWidth())
        self.label_2.setSizePolicy(sizePolicy1)

        self.horizontalLayout_3.addWidget(self.label_2)

        self.comboBox_3 = QComboBox(RS485)
        self.comboBox_3.setObjectName(u"comboBox_3")

        self.horizontalLayout_3.addWidget(self.comboBox_3)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout_3.addItem(self.horizontalSpacer_2)


        self.verticalLayout.addLayout(self.horizontalLayout_3)

        self.textEdit = QTextEdit(RS485)
        self.textEdit.setObjectName(u"textEdit")

        self.verticalLayout.addWidget(self.textEdit)

        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalLayout_2.setContentsMargins(-1, 0, -1, -1)
        self.lineEdit = QLineEdit(RS485)
        self.lineEdit.setObjectName(u"lineEdit")

        self.horizontalLayout_2.addWidget(self.lineEdit)

        self.comboBox_2 = QComboBox(RS485)
        self.comboBox_2.addItem("")
        self.comboBox_2.addItem("")
        self.comboBox_2.setObjectName(u"comboBox_2")

        self.horizontalLayout_2.addWidget(self.comboBox_2)

        self.pushButton_3 = QPushButton(RS485)
        self.pushButton_3.setObjectName(u"pushButton_3")

        self.horizontalLayout_2.addWidget(self.pushButton_3)


        self.verticalLayout.addLayout(self.horizontalLayout_2)


        self.verticalLayout_2.addLayout(self.verticalLayout)


        self.retranslateUi(RS485)

        QMetaObject.connectSlotsByName(RS485)
    # setupUi

    def retranslateUi(self, RS485):
        RS485.setWindowTitle(QCoreApplication.translate("RS485", u"Form", None))
        self.label.setText(QCoreApplication.translate("RS485", u"BAUD", None))
        self.btn_open.setText(QCoreApplication.translate("RS485", u"Open", None))
        self.label_2.setText(QCoreApplication.translate("RS485", u"FORMAT", None))
        self.comboBox_2.setItemText(0, QCoreApplication.translate("RS485", u"ASCII", None))
        self.comboBox_2.setItemText(1, QCoreApplication.translate("RS485", u"HEX", None))

        self.pushButton_3.setText(QCoreApplication.translate("RS485", u"Send", None))
    # retranslateUi

