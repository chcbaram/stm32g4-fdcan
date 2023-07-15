#include "log.h"
#include "uart.h"
#ifdef _USE_HW_CLI
#include "cli.h"
#endif


#ifdef _USE_HW_LOG

#ifdef _USE_HW_RTOS
#define lock()      xSemaphoreTake(mutex_lock, portMAX_DELAY);
#define unLock()    xSemaphoreGive(mutex_lock);
#endif


static bool is_init = false;
static bool is_enable = true;
static bool is_open = false;

static uint8_t  log_ch = LOG_CH;
static uint32_t log_baud = 57600;

#ifdef _USE_HW_RTOS
static SemaphoreHandle_t mutex_lock;
#endif




bool logInit(void)
{
  is_init = true;
  return true;
}



bool logOpen(uint8_t ch, uint32_t baud)
{
  log_ch   = ch;
  log_baud = baud;
  is_open  = true;

  is_open = uartOpen(ch, baud);

  return is_open;
}

void logPrintf(const char *fmt, ...)
{
#ifdef _USE_HW_RTOS
  lock();
#endif

  va_list arg;

  if (is_init != true) return;

  if (is_open == true && is_enable == true)
  {
    va_start(arg, fmt);
    uartVPrintf(log_ch, fmt, arg);
    va_end(arg);
  }

#ifdef _USE_HW_RTOS
  unLock();
#endif
}


#endif