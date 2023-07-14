#ifndef EZ_LOG_H_
#define EZ_LOG_H_


#include "ez_def.h"


namespace ez
{

enum EZ_LOG_TYPE
{
  EZ_LOG_DEBUG   = 1<<0,
  EZ_LOG_INFO    = 1<<1,
  EZ_LOG_WARN    = 1<<2,
  EZ_LOG_ERROR   = 1<<3,

  EZ_LOG_NONE    = 0,
  EZ_LOG_DEFAULT = EZ_LOG_INFO|EZ_LOG_WARN|EZ_LOG_ERROR,
  EZ_LOG_ALL     = EZ_LOG_INFO|EZ_LOG_WARN|EZ_LOG_ERROR|EZ_LOG_DEBUG,
};


#define EZ_LOG_TRACE_ERROR(x)      logError("%s,%s(),%d,%d\n", __FILE__, __FUNCTION__, __LINE__, (int)(x));



void logLevel(uint32_t level);

ez_err_t log(const char *fmt, ...);
ez_err_t logInfo(const char *fmt, ...);
ez_err_t logError(const char *fmt, ...);
ez_err_t logDebug(const char *fmt, ...);
ez_err_t logErrCode(int err_code);

}

#endif