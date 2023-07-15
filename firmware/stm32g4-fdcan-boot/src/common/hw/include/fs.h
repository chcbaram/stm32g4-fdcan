/*
 * fs.h
 *
 *  Created on: 2021. 11. 22.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_FS_H_
#define SRC_COMMON_HW_INCLUDE_FS_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"



#ifdef _USE_HW_FS

#define FS_MAX_SIZE   HW_FS_MAX_SIZE


#include "littlefs/lfs.h"

typedef struct _fs_t fs_t;

typedef struct _fs_t
{
  bool is_open;

  lfs_file_t file;
} fs_t;

bool    fsInit(void);
bool    fsIsInit(void);
bool    fsIsExist(const char *name);
bool    fsIsDir(const char *dirname);
int32_t fsGetFree(void);
int32_t fsGetSize(void);
bool    fsMakeDir(const char *dirname);
bool    fsFileOpen(fs_t *p_fs, const char *name);
bool    fsFileClose(fs_t *p_fs);
bool    fsFileRewind(fs_t *p_fs);
bool    fsFileDel(const char *filename);
int32_t fsFileRead(fs_t *p_fs, uint8_t *p_data, uint32_t length);
int32_t fsFileWrite(fs_t *p_fs, uint8_t *p_data, uint32_t length);
int32_t fsFileSize(fs_t *p_fs);
int32_t fsFileSeek(fs_t *p_fs, uint32_t seek_pos);
int32_t fsFileSync(fs_t *p_fs);
#endif

#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_FS_H_ */
