#ifndef BOOT_H_
#define BOOT_H_

#include "ap_def.h"


#define BOOT_SIZE_TAG   1024
#define BOOT_SIZE_VER   1024




typedef struct
{
  uint32_t mode;
} boot_info_t;


typedef struct
{
  firm_ver_t boot;
  firm_ver_t firm;
  firm_ver_t update;
} boot_version_t;



typedef struct
{
  char     fw_name[64];
  uint32_t fw_size;
} boot_begin_t;


bool bootInit(uint8_t ch, char *port_name, uint32_t baud);
bool bootInitUdp(char *ip_addr, uint32_t port);
bool bootDeInit(void);

cmd_driver_t *bootGetDriver(void);


uint16_t bootCmdReadInfo(boot_info_t *info, uint32_t timeout);
uint16_t bootCmdReadVersion(boot_version_t *version, uint32_t timeout);

uint16_t bootCmdFirmBegin(boot_begin_t *begin, uint32_t timeout);
uint16_t bootCmdFirmEnd(uint32_t timeout);
uint16_t bootCmdFirmVersion(firm_ver_t *version, uint32_t timeout);
uint16_t bootCmdFirmErase(uint32_t addr, uint32_t length, uint32_t timeout);
uint16_t bootCmdFirmWrite(uint32_t addr, uint8_t *p_data, uint32_t length, uint32_t timeout);
uint16_t bootCmdFirmRead(uint32_t addr, uint8_t *p_data, uint32_t length, uint32_t timeout);
uint16_t bootCmdFirmVerify(uint32_t timeout);
uint16_t bootCmdFirmUpdate(uint32_t timeout);
uint16_t bootCmdFirmJump(uint32_t timeout);

#endif