#include "cmd_uart.h"


typedef struct
{
  uint8_t  ch;
  uint32_t baud;
} cmd_uart_args_t;

static bool open(void *args);
static bool close(void *args);  
static uint32_t available(void *args);
static bool flush(void *args);
static uint8_t read(void *args);
static uint32_t write(void *args, uint8_t *p_data, uint32_t length);  



bool cmdUartInitDriver(cmd_driver_t *p_driver, uint8_t ch, uint32_t baud)
{
  cmd_uart_args_t *p_args = (cmd_uart_args_t *)p_driver->args;

  p_args->ch = ch;
  p_args->baud = baud;

  p_driver->open = open;
  p_driver->close = close;
  p_driver->available = available;
  p_driver->flush = flush;
  p_driver->read = read;
  p_driver->write= write;

  return true;
}

bool open(void *args)
{
  cmd_uart_args_t *p_args = (cmd_uart_args_t *)args;

  logPrintf("[OK] cmdUartOpen()\n");

  return uartOpen(p_args->ch, p_args->baud);
}

bool close(void *args)
{
  cmd_uart_args_t *p_args = (cmd_uart_args_t *)args;

  return uartClose(p_args->ch);
}

uint32_t available(void *args)
{
  cmd_uart_args_t *p_args = (cmd_uart_args_t *)args;

  if (p_args->ch != cliGetPort())
    return uartAvailable(p_args->ch);
  else
    return 0;
}

bool flush(void *args)
{
  cmd_uart_args_t *p_args = (cmd_uart_args_t *)args;

  return uartFlush(p_args->ch);
}

uint8_t read(void *args)
{
  cmd_uart_args_t *p_args = (cmd_uart_args_t *)args;

  return uartRead(p_args->ch);
}

uint32_t write(void *args, uint8_t *p_data, uint32_t length)
{
  cmd_uart_args_t *p_args = (cmd_uart_args_t *)args;

  return uartWrite(p_args->ch, p_data, length);
}
