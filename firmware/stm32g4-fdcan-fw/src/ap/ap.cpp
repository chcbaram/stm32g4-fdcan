#include "ap.h"


static void cliUpdate(void);




void apInit(void)
{
  cliOpen(_DEF_UART1, 115200);
}

void apMain(void)
{
  uint32_t pre_time;
  

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }    

    cliUpdate();
  }
}

void cliUpdate(void)
{
  uint8_t cli_ch;


  if (usbIsOpen() && usbGetType() == USB_CON_CLI)
  {
    cli_ch = HW_UART_CH_USB;
  }
  else
  {
    cli_ch = HW_UART_CH_DEBUG;
  }
  if (cli_ch != cliGetPort())
  {
    cliOpen(cli_ch, 0);
  }
  cliMain();
}