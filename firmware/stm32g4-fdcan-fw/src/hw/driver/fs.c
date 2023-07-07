/*
 * fs.c
 *
 *  Created on: 2021. 11. 22.
 *      Author: baram
 */




#include "fs.h"
#include "littlefs/lfs.h"
#include "flash.h"
#include "spi_flash.h"
#include "cli.h"


#ifdef _USE_HW_FS

#define LFS_BUF_CACHE_SIZE    256

#define FS_FLASH_OFFSET       HW_FS_FLASH_OFFSET





static bool is_init = false;
#ifdef _USE_HW_RTOS
static osMutexId mutex_lock;
#endif

static lfs_t lfs;

static uint8_t  read_buffer[LFS_BUF_CACHE_SIZE];
static uint8_t  prog_buffer[LFS_BUF_CACHE_SIZE];
static uint32_t lookahead_buffer[LFS_BUF_CACHE_SIZE/4];


static int fsDeviceRead(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
static int fsDeviceProg(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
static int fsDeviceErase(const struct lfs_config *c, lfs_block_t block);
static int fsDeviceSync(const struct lfs_config *c);
#ifdef _USE_HW_RTOS
static int fsDeviceLock(const struct lfs_config *c);
static int fsDeviceUnlock(const struct lfs_config *c);
#endif


const struct lfs_config cfg =
{
    // block device operations
    .read   = fsDeviceRead,
    .prog   = fsDeviceProg,
    .erase  = fsDeviceErase,
    .sync   = fsDeviceSync,
#ifdef _USE_HW_RTOS
    .lock   = fsDeviceLock,
    .unlock = fsDeviceUnlock,
#endif


    // block device configuration
    .read_size      = LFS_BUF_CACHE_SIZE,
    .prog_size      = LFS_BUF_CACHE_SIZE,
    .block_size     = 4096,
    .block_count    = FS_MAX_SIZE/4096,
    .cache_size     = LFS_BUF_CACHE_SIZE,
    .lookahead_size = LFS_BUF_CACHE_SIZE,
    .block_cycles   = 100000,

    // Optional statically allocated read buffer. Must be cache_size.
    // By default lfs_malloc is used to allocate this buffer.
    .read_buffer = read_buffer,

    // Optional statically allocated program buffer. Must be cache_size.
    // By default lfs_malloc is used to allocate this buffer.
    .prog_buffer = prog_buffer,

    // Optional statically allocated lookahead buffer. Must be lookahead_size
    // and aligned to a 32-bit boundary. By default lfs_malloc is used to
    // allocate this buffer.
    .lookahead_buffer = lookahead_buffer
};


#ifdef _USE_HW_CLI
static void cliCmd(cli_args_t *args);
#endif





bool fsInit(void)
{
  bool ret = true;
  int err;


#ifdef _USE_HW_RTOS
  osMutexDef(mutex_lock);
  mutex_lock = osMutexCreate (osMutex(mutex_lock));
#endif

  logPrintf("[  ] fsInit()\n");
  logPrintf("     lfs %d.%d\n", LFS_VERSION_MAJOR, LFS_VERSION_MINOR);

  // mount the filesystem
  err = lfs_mount(&lfs, &cfg);

  // reformat if we can't mount the filesystem
  // this should only happen on the first boot
  if (err)
  {
    err = lfs_format(&lfs, &cfg);
    if (err == LFS_ERR_OK)
    {
      logPrintf("     lfs formated\r\n");
    }
    else
    {
      logPrintf("     lfs format - Fail\r\n");
    }
    err = lfs_mount(&lfs, &cfg);
  }

  if (err == LFS_ERR_OK)
  {
    logPrintf("     lfs mounted\r\n");
  }
  else
  {
    logPrintf("     lfs mount - Fail\r\n");
    ret = false;
  }



#ifdef _USE_HW_CLI
  cliAdd("fs", cliCmd);
#endif

  is_init = ret;

  if (is_init == true)
  {
    if (fsIsExist("bd_name") == true)
    {
      fs_t fs;

      if (fsFileOpen(&fs, "bd_name") == true)
      {
        char bd_name[128];

        fsFileRead(&fs, (uint8_t *)bd_name, 128);

        logPrintf("     bd_name - %s\r\n", bd_name);

        fsFileClose(&fs);
      }
    }
  }

  logPrintf("[%s] fsInit()\n", ret ? "OK" : "NG");

  return ret;
}

bool fsIsInit(void)
{
  return is_init;
}

bool fsIsExist(const char *name)
{
  bool ret = false;
  int err;
  struct lfs_info info;

  err = lfs_stat(&lfs, name, &info);
  if (err == LFS_ERR_OK)
  {
    ret = true;
  }

  return ret;
}

bool fsIsDir(const char *dirname)
{
  int err;
  lfs_dir_t lfs_dir;

  err = lfs_dir_open(&lfs, &lfs_dir, dirname);
  if (err < 0)
  {
    return false;
  }
  return true;
}

int32_t fsGetFree(void)
{
  int free_size = 0;
  int used_count = 0;


  used_count = lfs_fs_size(&lfs);

  if (used_count > 0)
  {
    free_size = (lfs.cfg->block_count - used_count) * lfs.cfg->block_size;
  }
  else
  {
    free_size = 0;
  }

  return (int32_t)free_size;
}

int32_t fsGetSize(void)
{
  return lfs.cfg->block_count * lfs.cfg->block_size;
}

bool fsFileOpen(fs_t *p_fs, const char *name)
{
  bool ret = false;
  int err;

  p_fs->is_open = false;

  err = lfs_file_open(&lfs, &p_fs->file, name, LFS_O_RDWR | LFS_O_CREAT);
  if (err == LFS_ERR_OK)
  {
    ret = true;
    p_fs->is_open = true;
  }

  return ret;
}

bool fsFileClose(fs_t *p_fs)
{
  bool ret = false;
  int err;

  if (p_fs->is_open != true)
  {
    return false;
  }

  err = lfs_file_close(&lfs, &p_fs->file);
  if (err == LFS_ERR_OK)
  {
    ret = true;
  }

  p_fs->is_open = false;

  return ret;
}

bool fsFileDel(const char *filename)
{
  int err = 0;

  if (is_init != true)
  {
    return false;
  }

  err = lfs_remove(&lfs, filename);
  if(err < 0)
  {
    return false;
  }

  return true;
}

int32_t fsFileRead(fs_t *p_fs, uint8_t *p_data, uint32_t length)
{
  int32_t ret;

  if (p_fs->is_open != true)
  {
    return false;
  }

  ret = lfs_file_read(&lfs, &p_fs->file, p_data, length);

  return ret;
}

int32_t fsFileWrite(fs_t *p_fs, uint8_t *p_data, uint32_t length)
{
  int32_t ret;

  if (p_fs->is_open != true)
  {
    return 0;
  }

  ret = lfs_file_write(&lfs, &p_fs->file, p_data, length);

  return ret;
}

int32_t fsFileSync(fs_t *p_fs)
{
  return lfs_file_sync(&lfs, &p_fs->file);
}

bool fsFileRewind(fs_t *p_fs)
{
  bool ret = false;

  if (p_fs->is_open != true)
  {
    return false;
  }

  if (lfs_file_rewind(&lfs, &p_fs->file) == LFS_ERR_OK)
  {
    ret = true;
  }

  return ret;
}

int32_t fsFileSeek(fs_t *p_fs, uint32_t seek_pos)
{
  int32_t ret;

  ret = lfs_file_seek(&lfs, &p_fs->file, seek_pos, LFS_SEEK_SET);

  return ret;
}

int32_t fsFileSize(fs_t *p_fs)
{
  int32_t ret;

  ret = lfs_file_size(&lfs, &p_fs->file);

  return ret;
}

// Read a region in a block. Negative error codes are propogated
// to the user.
int fsDeviceRead(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size)
{
  int err = LFS_ERR_OK;
  bool ret;
  uint32_t addr;


  addr = block * c->block_size + off;

  ret = flashRead(FS_FLASH_OFFSET + addr, (uint8_t *)buffer, size);
  if (ret != true)
  {
    err = LFS_ERR_IO;
  }

  return err;
}

// Program a region in a block. The block must have previously
// been erased. Negative error codes are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int fsDeviceProg(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size)
{
  int err = LFS_ERR_OK;
  bool ret;
  uint32_t addr;


  addr = block * c->block_size + off;

  ret = flashWrite(FS_FLASH_OFFSET + addr, (uint8_t *)buffer, size);
  if (ret != true)
  {
    err = LFS_ERR_IO;
  }

  return err;
}

// Erase a block. A block must be erased before being programmed.
// The state of an erased block is undefined. Negative error codes
// are propogated to the user.
// May return LFS_ERR_CORRUPT if the block should be considered bad.
int fsDeviceErase(const struct lfs_config *c, lfs_block_t block)
{
  int err = LFS_ERR_OK;
  bool ret;
  uint32_t addr;


  addr = block * c->block_size;
  
  ret = spiFlashEraseSector(addr);
  if (ret != true)
  {
    err = LFS_ERR_IO;
  }

  return err;
}

// Sync the state of the underlying block device. Negative error codes
// are propogated to the user.
int fsDeviceSync(const struct lfs_config *c)
{
  int err = LFS_ERR_OK;


  return err;
}

#ifdef _USE_HW_RTOS
static int fsDeviceLock(const struct lfs_config *c)
{
  int err = LFS_ERR_OK;

  if (osMutexWait(mutex_lock, 1000) != osOK)
  {
    err = LFS_ERR_IO;
  }

  return err;
}

static int fsDeviceUnlock(const struct lfs_config *c)
{
  int err = LFS_ERR_OK;

  osMutexRelease(mutex_lock);

  return err;
}
#endif






#ifdef _USE_HW_CLI

int lfs_ls(lfs_t *lfs, const char *path);

void cliCmd(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    cliPrintf("fs init   : %d\n", is_init);
    if (is_init == true)
    {
      cliPrintf("fs size   : %d KB / %d KB\n", lfs_fs_size(&lfs)*4096/1024, FS_MAX_SIZE/1024);
      cliPrintf("fs free   : %d KB\n", fsGetFree() / 1024);
      cliPrintf("fs used   : %d KB\n", (fsGetSize() - fsGetFree()) / 1024);
    }
    ret = true;
  }

  if(args->argc == 1 && args->isStr(0, "list") == true)
  {
    lfs_ls(&lfs, "/");
    ret = true;
  }

  if(args->argc == 1 && args->isStr(0, "format") == true)
  {
    cliPrintf("format...");
    if(lfs_format(&lfs, &cfg) > 0)
    {
      cliPrintf("Fail\n");
    }
    else
    {
      cliPrintf("OK\n");
    }
    ret = true;
  }

  if(args->argc == 2 && args->isStr(0, "del") == true)
  {
    char *file_name;

    file_name = args->getStr(1);

    cliPrintf("del [%s]...", file_name);
    if (fsFileDel(file_name) == true)
    {
      cliPrintf("OK\n");
    }
    else
    {
      cliPrintf("Fail\n");
    }
    ret = true;
  }


  if (args->argc == 1 && args->isStr(0, "test") == true)
  {
    // read current count
    uint32_t boot_count = 0;
    lfs_file_t file;

    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    cliPrintf("boot_count : %d\n", boot_count);

    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "set_name") == true)
  {
    char *name;
    fs_t fs;

    name = args->getStr(1);

    if (fsFileOpen(&fs, "bd_name") == true)
    {
      fsFileWrite(&fs, (uint8_t *)name, strlen(name) + 1);
      fsFileClose(&fs);

      cliPrintf("bd_name : %s\n", name);
    }

    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("fs info \n");
    cliPrintf("fs list \n");
    cliPrintf("fs format \n");
    cliPrintf("fs del [filename] \n");
    cliPrintf("fs test \n");
    cliPrintf("fs set_name name_str \n");
  }

}

int lfs_ls(lfs_t *lfs, const char *path)
{
  lfs_dir_t dir;
  int err = lfs_dir_open(lfs, &dir, path);
  if (err)
  {
    return err;
  }

  struct lfs_info info;
  while (true)
  {
    int res = lfs_dir_read(lfs, &dir, &info);
    if (res < 0)
    {
      return res;
    }

    if (res == 0)
    {
      break;
    }

    switch (info.type)
    {
      case LFS_TYPE_REG: cliPrintf("reg "); break;
      case LFS_TYPE_DIR: cliPrintf("dir "); break;
      default:           cliPrintf("?   "); break;
    }

    static const char *prefixes[] = {"", "K", "M", "G"};
    for (int i = sizeof(prefixes)/sizeof(prefixes[0])-1; i >= 0; i--)
    {
      if (info.size >= (1 << 10*i)-1)
      {
        cliPrintf("%*u%sB ", 4-(i != 0), info.size >> 10*i, prefixes[i]);
        break;
      }
    }

    cliPrintf("%s\n", info.name);
  }

  err = lfs_dir_close(lfs, &dir);
  if (err) {
      return err;
  }

  return 0;
}

#endif

#endif