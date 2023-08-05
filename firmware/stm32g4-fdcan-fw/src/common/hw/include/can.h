#ifndef CAN_H_
#define CAN_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_CAN


#define CAN_MAX_CH            HW_CAN_MAX_CH
#define CAN_MSG_RX_BUF_MAX    HW_CAN_MSG_RX_BUF_MAX


typedef enum
{
  CAN_100K,
  CAN_125K,
  CAN_250K,
  CAN_500K,
  CAN_1M,
  CAN_2M,
  CAN_4M,
  CAN_5M,
} CanBaud_t;

typedef enum
{
  CAN_NORMAL,
  CAN_MONITOR,
  CAN_LOOPBACK,
} CanMode_t;

typedef enum
{
  CAN_CLASSIC,
  CAN_FD_NO_BRS,
  CAN_FD_BRS,
} CanFrame_t;

typedef enum
{
  CAN_STD,
  CAN_EXT
} CanIdType_t;

typedef enum
{
  CAN_DLC_0,
  CAN_DLC_1,
  CAN_DLC_2,
  CAN_DLC_3,
  CAN_DLC_4,
  CAN_DLC_5,
  CAN_DLC_6,
  CAN_DLC_7,
  CAN_DLC_8,
  CAN_DLC_12,
  CAN_DLC_16,
  CAN_DLC_20,
  CAN_DLC_24,
  CAN_DLC_32,
  CAN_DLC_48,
  CAN_DLC_64
} CanDlc_t;

typedef enum
{
  CAN_ID_MASK,
  CAN_ID_RANGE
} CanFilterType_t;

typedef enum
{
  CAN_ERR_NONE      = 0x00000000,
  CAN_ERR_PASSIVE   = 0x00000001,
  CAN_ERR_WARNING   = 0x00000002,
  CAN_ERR_BUS_OFF   = 0x00000004,
  CAN_ERR_BUS_FAULT = 0x00000008,
  CAN_ERR_ETC       = 0x00000010,
} CanErr_t;

typedef enum
{
  CAN_EVT_NONE,
  CAN_EVT_MSG,
  CAN_EVT_ERR_PASSIVE,
  CAN_EVT_ERR_WARNING,
  CAN_EVT_ERR_BUS_OFF,
  CAN_EVT_ERR_BUS_FAULT,
  CAN_EVT_ERR_ETC,
} CanEvent_t;

typedef struct
{
  CanMode_t  mode;
  CanFrame_t frame;
  CanBaud_t  baud;
  CanBaud_t  baud_data;
} can_info_t;

typedef struct
{
  uint32_t id;
  uint16_t length;
  uint8_t  data[64];

  CanDlc_t      dlc;
  CanIdType_t   id_type;
  CanFrame_t    frame;
} can_msg_t;





bool     canInit(void);
bool     canOpen(uint8_t ch, CanMode_t mode, CanFrame_t frame, CanBaud_t baud, CanBaud_t baud_data);
bool     canIsOpen(uint8_t ch);
void     canClose(uint8_t ch);
bool     canGetInfo(uint8_t ch, can_info_t *p_info);
bool     canSetFilterType(CanFilterType_t filter_type);
bool     canGetFilterType(CanFilterType_t *p_filter_type);
bool     canConfigFilter(uint8_t ch, uint8_t index, CanIdType_t id_type, uint32_t id, uint32_t id_mask);
CanDlc_t canGetDlc(uint8_t length);
uint8_t  canGetLen(CanDlc_t dlc);

bool     canMsgInit(can_msg_t *p_msg, CanFrame_t frame, CanIdType_t  id_type, CanDlc_t dlc);
uint32_t canMsgAvailable(uint8_t ch);
bool     canMsgWrite(uint8_t ch, can_msg_t *p_msg, uint32_t timeout);
bool     canMsgRead(uint8_t ch, can_msg_t *p_msg);

uint16_t canGetRxErrCount(uint8_t ch);
uint16_t canGetTxErrCount(uint8_t ch);
uint32_t canGetRxCount(uint8_t ch);
uint32_t canGetTxCount(uint8_t ch);
uint32_t canGetError(uint8_t ch);
uint32_t canGetState(uint8_t ch);

void     canErrClear(uint8_t ch);
void     canErrPrint(uint8_t ch);
bool     canUpdate(void);

void     canAttachRxInterrupt(uint8_t ch, bool (*handler)(uint8_t ch, CanEvent_t evt, can_msg_t *arg));
void     canDetachRxInterrupt(uint8_t ch);

bool     canLock(void);
bool     canUnLock(void);

#endif

#ifdef __cplusplus
 }
#endif


#endif 