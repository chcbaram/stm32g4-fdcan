#include "ez_log.h"
#include <mutex>


namespace ez
{


static uint32_t log_level = EZ_LOG_ALL;
static std::mutex log_mutex;

static void logFormat(const char *prefix, const char *fmt, va_list valist);



void logLevel(uint32_t level)
{
  log_level = level;
}

ez_err_t logErrCode(ez_err_t err_code)
{

  switch(err_code)
  {
    case EZ_ERR_SOCKET_INIT:
      logError("EZ_ERR_SOCKET_INIT\n");
      break;

    default:
      break;
  }

  return err_code;
}

ez_err_t log(const char *fmt, ...)
{
  va_list arg;


  va_start (arg, fmt);
  logFormat("", fmt, arg);
  va_end(arg);

  return EZ_OK;
}

ez_err_t logInfo(const char *fmt, ...)
{
  va_list arg;

  if (log_level & EZ_LOG_INFO)
  {
    va_start (arg, fmt);
    logFormat("[EZ::INFO]", fmt, arg);
    va_end(arg);
  }

  return EZ_OK;
}

ez_err_t logError(const char *fmt, ...)
{
  va_list arg;

  if (log_level & EZ_LOG_ERROR)
  {
    va_start (arg, fmt);
    logFormat("[EZ::ERROR]", fmt, arg);
    va_end(arg);
  }

  return EZ_OK;
}

ez_err_t logDebug(const char *fmt, ...)
{
  va_list arg;

  if (log_level & EZ_LOG_DEBUG)
  {
    va_start (arg, fmt);
    logFormat("[EZ::DEBUG]", fmt, arg);
    va_end(arg);
  }

  return EZ_OK;
}

void logFormat(const char *prefix, const char *fmt, va_list valist)
{
  const size_t buf_len = 1024;
  char buf[buf_len];

  vsnprintf(buf, buf_len, fmt, valist);

  log_mutex.lock();
  printf("%s %s", prefix, buf);
  log_mutex.unlock();
}

}
