#include "usbd_cdc_if.h"
#include "qbuffer.h"
#include "usb.h"



const char *JUMP_BOOT_STR = "BOOT 5555AAAA";


USBD_CDC_LineCodingTypeDef LineCoding =
    {
        115200,
        0x00,
        0x00,
        0x08
    };


uint8_t CDC_Reset_Status = 0;
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];



static qbuffer_t q_rx;
static qbuffer_t q_tx;

static uint8_t q_rx_buf[1024];
static uint8_t q_tx_buf[1024];

static bool is_opened = false;
static bool is_rx_full = false;
static uint8_t cdc_type = 0;

extern USBD_HandleTypeDef USBD_Device;


static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);




USBD_CDC_ItfTypeDef USBD_CDC_fops =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS,
  CDC_TransmitCplt_FS
};




bool cdcIfInit(void)
{
  is_opened = false;
  qbufferCreate(&q_rx, q_rx_buf, 1024);
  qbufferCreate(&q_tx, q_tx_buf, 1024);

  return true;
}

uint32_t cdcIfAvailable(void)
{
  return qbufferAvailable(&q_rx);
}

uint8_t cdcIfRead(void)
{
  uint8_t ret = 0;

  qbufferRead(&q_rx, &ret, 1);

  return ret;
}

uint32_t cdcIfWrite(uint8_t *p_data, uint32_t length)
{
  uint32_t pre_time;
  uint32_t tx_len;
  uint32_t buf_len;
  uint32_t sent_len;


  if (cdcIfIsConnected() != true) return 0;


  sent_len = 0;

  pre_time = millis();
  while(sent_len < length)
  {
    buf_len = (q_tx.len - qbufferAvailable(&q_tx)) - 1;
    tx_len = length - sent_len;

    if (tx_len > buf_len)
    {
      tx_len = buf_len;
    }

    if (tx_len > 0)
    {
      qbufferWrite(&q_tx, p_data, tx_len);
      p_data += tx_len;
      sent_len += tx_len;
    }
    else
    {
      delay(1);
    }
    
    if (cdcIfIsConnected() != true)
    {
      break;
    }

    if (millis()-pre_time >= 100)
    {
      break;
    }
  }

  return sent_len;
}

uint32_t cdcIfGetBaud(void)
{
  return LineCoding.bitrate;
}

bool cdcIfIsConnected(void)
{
  bool ret = true;

  if (USBD_Device.pClassData == NULL)
  {
    ret = false;
  }
  if (is_opened == false)
  {
    ret = false;
  }
  if (USBD_Device.dev_state != USBD_STATE_CONFIGURED)
  {
    ret = false;
  }
  if (USBD_Device.dev_config == 0)
  {
    ret = false;
  }

  is_opened = ret;

  return ret;
}

uint8_t cdcIfGetType(void)
{
  return cdc_type;
}

uint8_t CDC_SoF_ISR(struct _USBD_HandleTypeDef *pdev)
{

  //-- RX
  //
  if (is_rx_full)
  {
    uint32_t buf_len;

    buf_len = (q_rx.len - qbufferAvailable(&q_rx)) - 1;

    if (buf_len >= CDC_DATA_FS_MAX_PACKET_SIZE)
    {
      USBD_CDC_SetRxBuffer(&USBD_Device, &UserRxBufferFS[0]);
      USBD_CDC_ReceivePacket(&USBD_Device);
      is_rx_full = false;
    }
  }


  //-- TX
  //
  uint32_t tx_len;
  tx_len = qbufferAvailable(&q_tx);

  if (tx_len%CDC_DATA_FS_MAX_PACKET_SIZE == 0)
  {
    if (tx_len > 0)
    {
      tx_len = tx_len - 1;
    }
  }

  if (tx_len > 0)
  {
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)USBD_Device.pClassData;
    if (hcdc->TxState == 0)
    {
      qbufferRead(&q_tx, UserTxBufferFS, tx_len);

      USBD_CDC_SetTxBuffer(&USBD_Device, UserTxBufferFS, tx_len);
      USBD_CDC_TransmitPacket(&USBD_Device);
    }
  }

  return 0;
}




/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&USBD_Device, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&USBD_Device, UserRxBufferFS);

  is_opened = false;

  return (USBD_OK);
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{

  is_opened = false;

  return (USBD_OK);
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  USBD_SetupReqTypedef *req = (USBD_SetupReqTypedef *)pbuf;
  uint32_t bitrate;


  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:
      bitrate   = (uint32_t)(pbuf[0]);
      bitrate  |= (uint32_t)(pbuf[1]<<8);
      bitrate  |= (uint32_t)(pbuf[2]<<16);
      bitrate  |= (uint32_t)(pbuf[3]<<24);
      LineCoding.format    = pbuf[4];
      LineCoding.paritytype= pbuf[5];
      LineCoding.datatype  = pbuf[6];
      LineCoding.bitrate   = bitrate - (bitrate%10);

      if( LineCoding.bitrate == 1200 )
      {
        CDC_Reset_Status = 1;
      }
      if (LineCoding.bitrate == 115200)
        cdc_type = USB_CON_CLI;
      else
        cdc_type = 0;
    break;

    case CDC_GET_LINE_CODING:
      bitrate = LineCoding.bitrate | cdc_type;

      pbuf[0] = (uint8_t)(bitrate);
      pbuf[1] = (uint8_t)(bitrate>>8);
      pbuf[2] = (uint8_t)(bitrate>>16);
      pbuf[3] = (uint8_t)(bitrate>>24);
      pbuf[4] = LineCoding.format;
      pbuf[5] = LineCoding.paritytype;
      pbuf[6] = LineCoding.datatype;
    break;

    case CDC_SET_CONTROL_LINE_STATE:
      // TODO : 나중에 다른 터미널에서 문제 없는지 확인 필요
      //is_opened = req->wValue&0x01;  // 0 bit:DTR, 1 bit:RTS
      if (req->wValue & 0x01)
        is_opened = true;
      else
        is_opened = false;
        
    break;

    case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  uint32_t i;


  qbufferWrite(&q_rx, Buf, *Len);

  if( CDC_Reset_Status == 1 )
  {
    CDC_Reset_Status = 0;

    if( *Len >= 13 )
    {
      for(i=0; i<13; i++ )
      {
        if( JUMP_BOOT_STR[i] != Buf[i] ) break;
      }

      // if( i == 13 )
      // {
      //   resetToBoot(0);
      // }
    }
  }

  uint32_t buf_len;

  buf_len = (q_rx.len - qbufferAvailable(&q_rx)) - 1;

  if (buf_len >= CDC_DATA_FS_MAX_PACKET_SIZE)
  {
    USBD_CDC_SetRxBuffer(&USBD_Device, &Buf[0]);
    USBD_CDC_ReceivePacket(&USBD_Device);
  }
  else
  {
    is_rx_full = true;
  }

  return (USBD_OK);
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;

  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)USBD_Device.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&USBD_Device, Buf, Len);
  result = USBD_CDC_TransmitPacket(&USBD_Device);

  return result;
}

/**
  * @brief  CDC_TransmitCplt_FS
  *         Data transmitted callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 13 */
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);
  /* USER CODE END 13 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
