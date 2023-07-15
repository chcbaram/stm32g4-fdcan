/*
 * dxl.h
 *
 *  Created on: 2021. 3. 6.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_DXL_H_
#define SRC_COMMON_HW_INCLUDE_DXL_H_

#ifdef __cplusplus
 extern "C" {
#endif



#include "hw_def.h"

#ifdef _USE_HW_DXL

#define DXL_PACKET_BUF_MAX        HW_DXL_PACKET_BUF_MAX
#define DXL_DEVICE_CNT_MAX        HW_DXL_DEVICE_CNT_MAX

#define DXL_SYNC_DATA_MAX         ((DXL_PACKET_BUF_MAX-14-DXL_DEVICE_CNT_MAX*1)/DXL_DEVICE_CNT_MAX)
#define DXL_BULK_DATA_MAX         ((DXL_PACKET_BUF_MAX-14-DXL_DEVICE_CNT_MAX*5)/DXL_DEVICE_CNT_MAX)


#define DXL_BROADCAST_ID          254



typedef struct dxl_driver_t_ dxl_driver_t;

typedef struct dxl_driver_t_
{
  bool     is_init;
  bool     is_open;
  
  bool     (*open)(uint8_t dxl_ch, uint32_t baud);
  bool     (*close)(uint8_t dxl_ch);
  uint32_t (*available)(uint8_t dxl_ch);
  uint32_t (*write)(uint8_t dxl_ch, uint8_t *p_data, uint32_t length);
  uint8_t  (*read)(uint8_t dxl_ch);
  bool     (*flush)(uint8_t dxl_ch);
} dxl_driver_t;


enum
{
  DXL_INST_PING           = 0x01,
  DXL_INST_READ           = 0x02,
  DXL_INST_WRITE          = 0x03,
  DXL_INST_REG_WRITE      = 0x04,
  DXL_INST_ACTION         = 0x05,
  DXL_INST_FACTORY_RESET  = 0x06,
  DXL_INST_REBOOT         = 0x08,
  DXL_INST_CLEAR          = 0x10,
  DXL_INST_STATUS         = 0x55,
  DXL_INST_SYNC_READ      = 0x82,
  DXL_INST_SYNC_WRITE     = 0x83,
  DXL_INST_BULK_READ      = 0x92,
  DXL_INST_BULK_WRITE     = 0x93,
};


typedef struct
{
  uint8_t  header[3];
  uint8_t  reserved;
  uint8_t  id;
  uint16_t length;
  uint8_t  inst;
  uint8_t  err;
  uint16_t param_len;
  uint16_t param_index;
  uint8_t  *param;
  uint16_t crc;
} dxl_packet_t;


typedef struct
{
  bool     is_open;
  uint8_t  ch;
  uint32_t baud;
  uint8_t  state;
  uint32_t pre_time;
  uint16_t index;
  bool     is_status_packet;

  dxl_packet_t packet;

  uint8_t  packet_buf[DXL_PACKET_BUF_MAX];
  uint8_t  inst_buf[DXL_PACKET_BUF_MAX];
  dxl_driver_t driver;
} dxl_t;


//-- Ping
//
typedef struct
{
  uint8_t  id;
  uint8_t  firm_version;
  uint16_t model_number;
} dxl_ping_node_t;

typedef struct
{
  uint8_t  id_cnt;
  dxl_ping_node_t node[DXL_DEVICE_CNT_MAX];
} dxl_ping_resp_t;

typedef struct
{
  dxl_ping_resp_t resp;
} dxl_ping_t;


//-- Sync Read
//
typedef struct
{
  uint8_t  id_cnt;
  uint16_t addr;
  uint16_t length;

  uint8_t id[DXL_DEVICE_CNT_MAX];
} dxl_sync_read_param_t;

typedef struct
{
  uint8_t  id;
  uint16_t addr;
  uint16_t length;
  uint8_t  data[DXL_SYNC_DATA_MAX];
} dxl_sync_read_node_t;

typedef struct
{
  uint8_t id_cnt;
  dxl_sync_read_node_t  node[DXL_DEVICE_CNT_MAX];
} dxl_sync_read_resp_t;

typedef union
{
  dxl_sync_read_param_t  param;
  dxl_sync_read_resp_t   resp;
} dxl_sync_read_t;


//-- Sync Write
//
typedef struct
{
  uint8_t id;
  uint8_t data[DXL_SYNC_DATA_MAX];
} dxl_sync_write_node_t;

typedef struct
{
  uint8_t  id_cnt;
  uint16_t addr;
  uint16_t length;
  dxl_sync_write_node_t node[DXL_DEVICE_CNT_MAX];
} dxl_sync_write_param_t;

typedef union
{
  dxl_sync_write_param_t  param;
} dxl_sync_write_t;


//-- Bulk Read
//
typedef struct
{
  uint8_t  id;
  uint16_t addr;
  uint16_t length;
  uint8_t  data[DXL_BULK_DATA_MAX];
} dxl_bulk_read_param_node_t;

typedef struct
{
  uint8_t  id_cnt;
  uint8_t  id    [DXL_DEVICE_CNT_MAX];
  uint16_t addr  [DXL_DEVICE_CNT_MAX];
  uint16_t length[DXL_DEVICE_CNT_MAX];
} dxl_bulk_read_param_t;

typedef struct
{
  uint8_t  id;
  uint16_t addr;
  uint16_t length;
  uint8_t  data[DXL_BULK_DATA_MAX];
} dxl_bulk_read_node_t;

typedef struct
{
  uint8_t id_cnt;
  dxl_bulk_read_node_t  node[DXL_DEVICE_CNT_MAX];
} dxl_bulk_read_resp_t;

typedef union
{
  dxl_bulk_read_param_t  param;
  dxl_bulk_read_resp_t   resp;
} dxl_bulk_read_t;


//-- Bulk Write
//
typedef struct
{
  uint8_t  id;
  uint16_t addr;
  uint16_t length;
  uint8_t data[DXL_BULK_DATA_MAX];
} dxl_bulk_write_node_t;

typedef struct
{
  uint8_t  id_cnt;
  dxl_bulk_write_node_t node[DXL_DEVICE_CNT_MAX];
} dxl_bulk_write_param_t;

typedef union
{
  dxl_bulk_write_param_t  param;
} dxl_bulk_write_t;



typedef union
{
  dxl_ping_t       ping;
  dxl_sync_read_t  sync_read;
  dxl_sync_write_t sync_write;
  dxl_bulk_read_t  bulk_read;
  dxl_bulk_write_t bulk_write;
} dxl_inst_t;



bool dxlInit(void);
bool dxlLoadDriver(dxl_t *p_dxl, bool (*load_func)(dxl_driver_t *));
bool dxlOpen(dxl_t *p_dxl, uint8_t dxl_ch, uint32_t baud);
bool dxlIsOpen(dxl_t *p_dxl);
bool dxlClose(dxl_t *p_dxl);

bool dxlInstPing(dxl_t *p_dxl, uint8_t id, dxl_ping_t *p_inst, uint32_t timeout);
bool dxlInstRead(dxl_t *p_dxl, uint8_t id, uint16_t addr, uint8_t *p_data, uint16_t length, uint32_t timeout);
bool dxlInstWrite(dxl_t *p_dxl, uint8_t id, uint16_t addr, uint8_t *p_data, uint16_t length, uint32_t timeout);

bool dxlInstSyncRead(dxl_t *p_dxl, dxl_sync_read_t *p_inst, uint32_t timeout);
bool dxlInstSyncWrite(dxl_t *p_dxl, dxl_sync_write_t *p_inst, uint32_t timeout);

bool dxlInstBulkRead(dxl_t *p_dxl, dxl_bulk_read_t *p_inst, uint32_t timeout);
bool dxlInstBulkWrite(dxl_t *p_dxl, dxl_bulk_write_t *p_inst, uint32_t timeout);

#endif


#ifdef __cplusplus
}
#endif


#endif /* SRC_COMMON_HW_INCLUDE_DXL_H_ */
