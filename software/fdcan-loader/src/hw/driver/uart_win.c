/*
 * uart.c
 *
 *  Created on: 2020. 12. 8.
 *      Author: baram
 */


#include "uart.h"
#include "qbuffer.h"

#if defined (__WIN32__) || (__WIN64__)

#ifdef _USE_HW_UART


#define _USE_UART1
#define _USE_UART2


bool log_err = true;


typedef struct
{
  bool     is_open;
  bool     is_consol;
  bool     is_port_name;
  uint8_t  ch;
  uint32_t baud;

  HANDLE   serial_handle;
  char     port_name[256];
  uint8_t  received_data;
} uart_t;

static uart_t uart_tbl[UART_MAX_CH];



static uint32_t uartOpenPC(uint8_t channel, char *port_name, uint32_t baud);



bool uartInit(void)
{
  for (int i=0; i<UART_MAX_CH; i++)
  {
    uart_tbl[i].is_open   = false;
    uart_tbl[i].is_consol = false;
    uart_tbl[i].is_port_name = false;
  }


  return true;
}

bool uartOpen(uint8_t ch, uint32_t baud)
{
  bool ret = false;


  switch(ch)
  {
    case _DEF_UART1:
      uart_tbl[ch].is_open = true;
      uart_tbl[ch].ch = ch;
      ret = true;
      break;

    case _DEF_UART2:
      if (uart_tbl[ch].is_port_name == true)
      {
        if (uart_tbl[ch].is_open == true)
        {
          uartClose(uart_tbl[ch].ch);
        }
        if (uartOpenPC(ch, uart_tbl[ch].port_name, baud) == 0)
        {
          uart_tbl[ch].is_open = true;
          uart_tbl[ch].ch = ch;
          ret = true;
        }
      }
      break;
  }

  return ret;
}

bool uartIsOpen(uint8_t ch)
{
  return uart_tbl[ch].is_open;
}

bool uartOpenPort(uint8_t ch, char *port_name, uint32_t baud)
{
  bool ret = false;

  uartSetPortName(ch, port_name);

  ret = uartOpen(ch, baud);

  return ret;
}

void uartSetPortName(uint8_t ch, char *port_name)
{
  if (ch >= UART_MAX_CH)
  {
    return;
  }

  sprintf(uart_tbl[ch].port_name, "\\\\.\\%s", port_name);
  uart_tbl[ch].is_port_name  = true;
}

const char *uartGetPortName(uint8_t ch)
{
  return (const char *)uart_tbl[ch].port_name;
}

uint32_t uartOpenPC(uint8_t ch, char *port_name, uint32_t baud)
{
  uint32_t err_code  = 0;
  uart_t *p_uart = &uart_tbl[ch];

  DCB dcb;
  COMMTIMEOUTS timeouts;
  DWORD dwError;


  if (ch >= UART_MAX_CH)
  {
    return 1;
  }


  p_uart->baud = baud;


  p_uart->serial_handle = CreateFileA(p_uart->port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (p_uart->serial_handle == INVALID_HANDLE_VALUE)
  {
    if (log_err)
    {
      //printf("Error opening serial port!\n");
    }
    return 2;
  }

  while(1)
  {
    dcb.DCBlength = sizeof(DCB);
    if (GetCommState(p_uart->serial_handle, &dcb) == FALSE)
    {
      if (log_err)
        printf("Error GetCommState\n");
      err_code = 1;
      break;
    }

    // Set baudrate
    dcb.BaudRate = (DWORD)baud;
    dcb.ByteSize = 8;                    // Data bit = 8bit
    dcb.Parity   = NOPARITY;             // No parity
    dcb.StopBits = ONESTOPBIT;           // Stop bit = 1
    dcb.fParity  = NOPARITY;             // No Parity check
    dcb.fBinary  = 1;                    // Binary mode
    dcb.fNull    = 0;                    // Get Null byte
    dcb.fAbortOnError = 0;
    dcb.fErrorChar    = 0;
    // Not using XOn/XOff
    dcb.fOutX = 0;
    dcb.fInX  = 0;
    // Not using H/W flow control
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fDsrSensitivity = 0;
    dcb.fOutxDsrFlow = 0;
    dcb.fOutxCtsFlow = 0;

    if (SetCommState(p_uart->serial_handle, &dcb) == FALSE)
    {
      DWORD dwError = GetLastError();
      err_code = 2;

      if (log_err)
        printf("SetCommState err: %d\n", (int)dwError);
      break;
    }

    if (SetCommMask(p_uart->serial_handle, 0) == FALSE) // Not using Comm event
    {
      err_code = 3;
      break;
    }
    if (SetupComm(p_uart->serial_handle, 4096, 4096) == FALSE) // Buffer size (Rx,Tx)
    {
      err_code = 4;
      break;
    }
    if (PurgeComm(p_uart->serial_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR) == FALSE) // Clear buffer
    {
      err_code = 5;
      break;
    }
    if (ClearCommError(p_uart->serial_handle, &dwError, NULL) == FALSE)
    {
      err_code = 6;
      break;
    }

    if (GetCommTimeouts(p_uart->serial_handle, &timeouts) == FALSE)
    {
      err_code = 7;
      break;
    }
    // Timeout (Not using timeout)
    // Immediatly return
    timeouts.ReadIntervalTimeout         = 0;
    timeouts.ReadTotalTimeoutMultiplier  = 0;
    timeouts.ReadTotalTimeoutConstant    = 1; // must not be zero.
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant   = 0;
    if (SetCommTimeouts(p_uart->serial_handle, &timeouts) == FALSE)
    {
      err_code = 8;
      break;
    }
    EscapeCommFunction(p_uart->serial_handle, SETRTS);
    EscapeCommFunction(p_uart->serial_handle, SETDTR);
    break;
  }

  if (err_code != 0)
  {
    CloseHandle(p_uart->serial_handle);
  }
  else
  {
    p_uart->is_open = true;
  }

  return err_code;
}

bool uartClose(uint8_t ch)
{
  bool ret = false;

  switch(ch)
  {
    case _DEF_UART1:
      uart_tbl[ch].is_open = false;
      break;

    case _DEF_UART2:
      if (uart_tbl[ch].is_open == true)
      {
        CloseHandle(uart_tbl[ch].serial_handle);
        uart_tbl[ch].is_open = false;
        ret = true;
      }
      break;
  }

  return ret;
}

bool uartFlush(uint8_t ch)
{
  uint32_t pre_time;

  pre_time = millis();
  while(uartAvailable(ch))
  {
    if (millis()-pre_time >= 10)
    {
      break;
    }
    uartRead(ch);
  }

  return true;
}

uint32_t uartAvailable(uint8_t ch)
{
  uint32_t ret = 0;
  int32_t length = 0;
  DWORD dwRead = 0;
  uint8_t data;


  if (uart_tbl[ch].is_open != true)
  {
    return 0;
  }

  switch(ch)
  {
    case _DEF_UART1:
      if (kbhit())
      {
        ret = 1;
      }
      break;

    case _DEF_UART2:

      if (ReadFile(uart_tbl[ch].serial_handle, &data, (DWORD)1, &dwRead, NULL) == TRUE)
      {
        if (dwRead != 1)
        {
          length = 0;
        }
        else
        {
          length = 1;
          uart_tbl[ch].received_data = data;
        }
      }
      ret = length;
      break;
  }

  return ret;
}

uint8_t uartRead(uint8_t ch)
{
  uint8_t ret = 0;


  if (uart_tbl[ch].is_open != true)
  {
    return 0;
  }

  switch(ch)
  {
    case _DEF_UART1:
      ret = getch();
      break;

    case _DEF_UART2:
      ret = uart_tbl[ch].received_data;
      break;
  }

  return ret;
}

uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;
  DWORD dwWrite = 0;


  if (uart_tbl[ch].is_open != true)
  {
    return 0;
  }

  switch(ch)
  {
    case _DEF_UART1:
      for (int i=0; i<length; i++)
      {
        putc(p_data[i], stdout);
      }
      ret = length;
      break;

    case _DEF_UART2:
      if (WriteFile(uart_tbl[ch].serial_handle, p_data, (DWORD)length, &dwWrite, NULL) == FALSE)
      {
        ret = 0;
      }
      else
      {
        ret = dwWrite;
      }
      break;
  }

  return ret;
}

uint32_t uartPrintf(uint8_t ch, char *fmt, ...)
{
  char buf[256];
  va_list args;
  int len;
  uint32_t ret;

  va_start(args, fmt);
  len = vsnprintf(buf, 256, fmt, args);

  ret = uartWrite(ch, (uint8_t *)buf, len);

  va_end(args);


  return ret;
}

uint32_t uartGetBaud(uint8_t ch)
{
  uint32_t ret = 0;


  ret = uart_tbl[ch].baud;

  return ret;
}




#endif

#endif