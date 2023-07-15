#include "nvs.h"
#include "fs.h"

#ifdef _USE_HW_NVS


static bool is_init = false;
static fs_t nvs_fs;




bool nvsInit(void)
{
  bool ret = false;

  if (fsIsInit() == true)
  {
    is_init = true;
  }

  ret = is_init;

  logPrintf("[%s] nvsInit()\n", ret ? "OK" : "NG");

  return ret;
}

bool nvsIsInit(void)
{
  return is_init;
}

bool nvsIsExist(const char *p_name)
{
  bool ret = false;

  if (is_init != true) return false;

  if (fsIsExist(p_name) == true)
  {
    ret = true;
  }

  return ret;
}

bool nvsSet(const char *p_name, void *p_data, uint32_t length)
{
  bool ret = false;
  int32_t file_len;

  do
  {
    if (fsFileOpen(&nvs_fs, p_name) != true)
      break;

    file_len = fsFileWrite(&nvs_fs, p_data, length);

    if (fsFileClose(&nvs_fs) != true)
      break;

    if (file_len == length)
      ret = true;
  } while (0);
  
  return ret;
}

bool nvsGet(const char *p_name, void *p_data, uint32_t length)
{
  bool ret = false;
  int32_t file_len;

  do
  {
    if (fsIsExist(p_name) != true)
      break;

    if (fsFileOpen(&nvs_fs, p_name) != true)
      break;

    file_len = fsFileRead(&nvs_fs, p_data, length);

    if (fsFileClose(&nvs_fs) != true)
      break;

    if (file_len == length)
      ret = true;
  } while (0);
  
  return ret;
}




#endif