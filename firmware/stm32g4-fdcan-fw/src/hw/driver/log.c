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


typedef struct
{
  uint16_t line_index;
  uint16_t buf_length;
  uint16_t buf_length_max;
  uint16_t buf_index;
  uint8_t *buf;
} log_buf_t;


log_buf_t log_buf_boot;
log_buf_t log_buf_list;

static uint8_t buf_boot[LOG_BOOT_BUF_MAX];
static uint8_t buf_list[LOG_LIST_BUF_MAX];

static bool is_init = false;
static bool is_boot_log = true;
static bool is_enable = true;
static bool is_open = false;

static uint8_t  log_ch = LOG_CH;
static uint32_t log_baud = 57600;

static char print_buf[256];

#ifdef _USE_HW_RTOS
static SemaphoreHandle_t mutex_lock;
#endif



#ifdef _USE_HW_CLI
static void cliCmd(cli_args_t *args);
#endif





bool logInit(void)
{
#ifdef _USE_HW_RTOS
  mutex_lock = xSemaphoreCreateMutex();
#endif

  log_buf_boot.line_index     = 0;
  log_buf_boot.buf_length     = 0;
  log_buf_boot.buf_length_max = LOG_BOOT_BUF_MAX;
  log_buf_boot.buf_index      = 0;
  log_buf_boot.buf            = buf_boot;


  log_buf_list.line_index     = 0;
  log_buf_list.buf_length     = 0;
  log_buf_list.buf_length_max = LOG_LIST_BUF_MAX;
  log_buf_list.buf_index      = 0;
  log_buf_list.buf            = buf_list;


  is_init = true;

#ifdef _USE_HW_CLI
  cliAdd("log", cliCmd);
#endif

  return true;
}

void logEnable(void)
{
  is_enable = true;
}

void logDisable(void)
{
  is_enable = false;
}

void logBoot(uint8_t enable)
{
  is_boot_log = enable;
}

bool logOpen(uint8_t ch, uint32_t baud)
{
  log_ch   = ch;
  log_baud = baud;
  is_open  = true;

  is_open = uartOpen(ch, baud);

  return is_open;
}

bool logBufPrintf(log_buf_t *p_log, char *p_data, uint32_t length)
{
  uint32_t buf_last;
  uint8_t *p_buf;
  int buf_len;


  buf_last = p_log->buf_index + length + 8;
  if (buf_last > p_log->buf_length_max)
  {
    p_log->buf_index = 0;
    buf_last = p_log->buf_index + length + 8;

    if (buf_last > p_log->buf_length_max)
    {
      return false;
    }
  }

  p_buf = &p_log->buf[p_log->buf_index];

  buf_len = snprintf((char *)p_buf, length + 8, "%04X\t%s", p_log->line_index, p_data);
  p_log->line_index++;
  p_log->buf_index += buf_len;


  if (buf_len + p_log->buf_length <= p_log->buf_length_max)
  {
    p_log->buf_length += buf_len;
  }

  return true;
}

void logPrintf(const char *fmt, ...)
{
#ifdef _USE_HW_RTOS
  lock();
#endif

  va_list args;
  int len;

  if (is_init != true) return;


  va_start(args, fmt);
  len = vsnprintf(print_buf, 256, fmt, args);

  if (is_open == true && is_enable == true)
  {
    uartWrite(log_ch, (uint8_t *)print_buf, len);
  }

  if (is_boot_log)
  {
    logBufPrintf(&log_buf_boot, print_buf, len);
  }
  logBufPrintf(&log_buf_list, print_buf, len);

  va_end(args);

#ifdef _USE_HW_RTOS
  unLock();
#endif
}


#ifdef _USE_HW_CLI
void cliCmd(cli_args_t *args)
{
  bool ret = false;



  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliPrintf("boot.line_index %d\n", log_buf_boot.line_index);
    cliPrintf("boot.buf_length %d\n", log_buf_boot.buf_length);
    cliPrintf("\n");
    cliPrintf("list.line_index %d\n", log_buf_list.line_index);
    cliPrintf("list.buf_length %d\n", log_buf_list.buf_length);

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "boot"))
  {
    uint32_t index = 0;

    while(cliKeepLoop())
    {
      uint32_t buf_len;

      buf_len = log_buf_boot.buf_length - index;
      if (buf_len == 0)
      {
        break;
      }
      if (buf_len > 64)
      {
        buf_len = 64;
      }

      #ifdef _USE_HW_RTOS
      lock();
      #endif

      cliWrite((uint8_t *)&log_buf_boot.buf[index], buf_len);
      index += buf_len;

      #ifdef _USE_HW_RTOS
      unLock();
      #endif
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "list"))
  {
    uint32_t index = 0;

    while(cliKeepLoop())
    {
      uint32_t buf_len;

      buf_len = log_buf_list.buf_length - index;
      if (buf_len == 0)
      {
        break;
      }
      if (buf_len > 64)
      {
        buf_len = 64;
      }

      #ifdef _USE_HW_RTOS
      lock();
      #endif

      cliWrite((uint8_t *)&log_buf_list.buf[index], buf_len);
      index += buf_len;

      #ifdef _USE_HW_RTOS
      unLock();
      #endif

    }
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("log info\n");
    cliPrintf("log boot\n");
    cliPrintf("log list\n");
  }
}
#endif


#endif