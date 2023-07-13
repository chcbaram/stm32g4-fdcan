#ifndef HW_DEF_H_
#define HW_DEF_H_



#include "bsp.h"


#define _DEF_FIRMWATRE_VERSION    "V230702R1"
#define _DEF_BOARD_NAME           "STM32G4-FDCAN-FW"



#define _USE_HW_SPI_FLASH
#define _USE_HW_FLASH

#define _USE_HW_LED
#define      HW_LED_MAX_CH          5
#define      HW_LED_CH_DEBUG        _DEF_LED1
#define      HW_LED_CH_RX           _DEF_LED2
#define      HW_LED_CH_TX           _DEF_LED3
#define      HW_LED_CH_CAN          _DEF_LED4
#define      HW_LED_CH_RS485        _DEF_LED5


#define _USE_HW_UART
#define      HW_UART_MAX_CH         4
#define      HW_UART_CH_DEBUG       _DEF_UART1
#define      HW_UART_CH_RS485       _DEF_UART2
#define      HW_UART_CH_EXT         _DEF_UART3
#define      HW_UART_CH_USB         _DEF_UART4

#define _USE_HW_CLI
#define      HW_CLI_CMD_LIST_MAX    32
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_LINE_HIS_MAX    8
#define      HW_CLI_LINE_BUF_MAX    64

#define _USE_HW_LOG
#define      HW_LOG_CH              _DEF_UART1
#define      HW_LOG_BOOT_BUF_MAX    1024
#define      HW_LOG_LIST_BUF_MAX    1024

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH       3
#define      HW_BUTTON_CH_BOOT      _DEF_BUTTON1
#define      HW_BUTTON_CH_S1        _DEF_BUTTON2
#define      HW_BUTTON_CH_S2        _DEF_BUTTON3

#define _USE_HW_SWTIMER
#define      HW_SWTIMER_MAX_CH      4

#define _USE_HW_CAN
#define      HW_CAN_MAX_CH          1
#define      HW_CAN_MSG_RX_BUF_MAX  32

#define _USE_HW_GPIO
#define      HW_GPIO_MAX_CH         1

#define _USE_HW_SPI
#define      HW_SPI_MAX_CH          1

#define _USE_HW_USB
#define _USE_HW_CDC
#define      HW_USE_CDC             1
#define      HW_USE_MSC             0

#define _USE_HW_EEPROM
#define      HW_EEPROM_MAX_PAGES    4

#define _USE_HW_RESET
#define      HW_RESET_BOOT          1

#define _USE_HW_RTC
#define      HW_RTC_BOOT_MODE       RTC_BKP_DR3
#define      HW_RTC_RESET_BITS      RTC_BKP_DR4


#endif