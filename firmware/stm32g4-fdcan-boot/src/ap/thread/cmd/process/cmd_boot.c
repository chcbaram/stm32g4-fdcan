#include "cmd_boot.h"
#include "thread/boot/boot.h"


#define BOOT_CMD_INFO                   0x0000
#define BOOT_CMD_VERSION                0x0001

#define BOOT_CMD_FLASH_ERASE            0x0003
#define BOOT_CMD_FLASH_WRITE            0x0004
#define BOOT_CMD_FLASH_READ             0x0005

#define BOOT_CMD_FW_VER                 0x0006
#define BOOT_CMD_FW_ERASE               0x0007
#define BOOT_CMD_FW_WRITE               0x0008
#define BOOT_CMD_FW_READ                0x0009
#define BOOT_CMD_FW_VERIFY              0x000A
#define BOOT_CMD_FW_UPDATE              0x000B
#define BOOT_CMD_FW_JUMP                0x000C
#define BOOT_CMD_FW_BEGIN               0x000D
#define BOOT_CMD_FW_END                 0x000E


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


static boot_info_t    boot_info;
static boot_version_t boot_version;
static boot_begin_t   boot_begin;

static bool is_begin = false;
static uint32_t fw_receive_size = 0;





static void bootInfo(cmd_t *p_cmd)
{
  boot_info.mode = 0;

  cmdSendResp(p_cmd, p_cmd->packet.cmd, CMD_OK, (uint8_t *)&boot_info, sizeof(boot_info_t));
}

static void bootVersion(cmd_t *p_cmd)
{
  firm_ver_t *p_boot = (firm_ver_t *)(FLASH_ADDR_BOOT + FLASH_SIZE_VER);
  firm_ver_t *p_firm = (firm_ver_t *)(FLASH_ADDR_FIRM + FLASH_SIZE_TAG + FLASH_SIZE_VER);
  firm_ver_t update;

  memset(&boot_version, 0, sizeof(boot_version));

  if (p_boot->magic_number == VERSION_MAGIC_NUMBER) 
    boot_version.boot = *p_boot;

  if (p_firm->magic_number == VERSION_MAGIC_NUMBER) 
    boot_version.firm = *p_firm;


  flashRead(FLASH_ADDR_UPDATE + FLASH_SIZE_TAG + FLASH_SIZE_VER, (uint8_t *)&update, sizeof(firm_ver_t));

  if (update.magic_number == VERSION_MAGIC_NUMBER)
    boot_version.update = update;

  cmdSendResp(p_cmd, p_cmd->packet.cmd, CMD_OK, (uint8_t *)&boot_version, sizeof(boot_version_t));
}

static void bootFirmVersion(cmd_t *p_cmd)
{
  cmdSendResp(p_cmd, p_cmd->packet.cmd, CMD_OK, (uint8_t *)(FLASH_ADDR_FIRM + FLASH_SIZE_VER), sizeof(firm_ver_t));
}

static void bootFirmErase(cmd_t *p_cmd)
{
  uint32_t addr = 0;
  uint32_t length = 0;
  cmd_packet_t *p_packet = &p_cmd->packet;
  uint16_t err_code = CMD_OK;


  addr  = ((uint32_t)p_packet->data[0] <<  0);
  addr |= ((uint32_t)p_packet->data[1] <<  8);
  addr |= ((uint32_t)p_packet->data[2] << 16);
  addr |= ((uint32_t)p_packet->data[3] << 24);

  length  = ((uint32_t)p_packet->data[4] <<  0);
  length |= ((uint32_t)p_packet->data[5] <<  8);
  length |= ((uint32_t)p_packet->data[6] << 16);
  length |= ((uint32_t)p_packet->data[7] << 24);

  if ((addr+length) < FLASH_SIZE_FIRM)
  {    
    if (flashErase(FLASH_ADDR_UPDATE + addr, length) != true)
    {
      err_code = ERR_BOOT_FLASH_ERASE;
    }
  }
  else
  {
    err_code = ERR_BOOT_WRONG_RANGE;
  } 

  cmdSendResp(p_cmd, p_cmd->packet.cmd, err_code, NULL, 0);
}

static void bootFirmWrite(cmd_t *p_cmd)
{
  uint32_t addr = 0;
  uint32_t length = 0;
  cmd_packet_t *p_packet = &p_cmd->packet;
  uint16_t err_code = CMD_OK;


  addr  = ((uint32_t)p_packet->data[0] <<  0);
  addr |= ((uint32_t)p_packet->data[1] <<  8);
  addr |= ((uint32_t)p_packet->data[2] << 16);
  addr |= ((uint32_t)p_packet->data[3] << 24);

  length  = ((uint32_t)p_packet->data[4] <<  0);
  length |= ((uint32_t)p_packet->data[5] <<  8);
  length |= ((uint32_t)p_packet->data[6] << 16);
  length |= ((uint32_t)p_packet->data[7] << 24);

  if ((addr+length) < FLASH_SIZE_FIRM)
  {    
    if (flashWrite(FLASH_ADDR_UPDATE + addr, &p_packet->data[8], length) == true)
    {
      uint32_t index = 0;
      uint32_t rd_len;
      uint8_t buf[32];

      while(index < length)
      {
        rd_len = constrain(length - index, 0, 32);

        flashRead(FLASH_ADDR_UPDATE + addr + index, buf, rd_len);
        for (uint32_t i=0; i<rd_len; i++)
        {
          if (p_packet->data[8 + index + i] != buf[i])
          {
            err_code = ERR_BOOT_FLASH_WRITE;
            break;
          }
        }
        index += rd_len;

        if (err_code != CMD_OK)
        {
          break;
        }
      }
    }
    else
    {
      err_code = ERR_BOOT_FLASH_WRITE;
    }
    
  }
  else
  {
    err_code = ERR_BOOT_WRONG_RANGE;
  } 

  cmdSendResp(p_cmd, p_cmd->packet.cmd, err_code, NULL, 0);
}

static void bootFirmRead(cmd_t *p_cmd)
{
  uint32_t addr = 0;
  uint32_t length = 0;
  cmd_packet_t *p_packet = &p_cmd->packet;
  uint16_t err_code = CMD_OK;


  addr  = ((uint32_t)p_packet->data[0] <<  0);
  addr |= ((uint32_t)p_packet->data[1] <<  8);
  addr |= ((uint32_t)p_packet->data[2] << 16);
  addr |= ((uint32_t)p_packet->data[3] << 24);

  length  = ((uint32_t)p_packet->data[4] <<  0);
  length |= ((uint32_t)p_packet->data[5] <<  8);
  length |= ((uint32_t)p_packet->data[6] << 16);
  length |= ((uint32_t)p_packet->data[7] << 24);

  if ((addr+length) < FLASH_SIZE_FIRM)
  {    
    if (flashRead(FLASH_ADDR_UPDATE + addr, &p_packet->data[0], length) != true)
    {
      err_code = ERR_BOOT_FLASH_WRITE;
    }
  }
  else
  {
    err_code = ERR_BOOT_WRONG_RANGE;
  } 

  cmdSendResp(p_cmd, p_cmd->packet.cmd, err_code, p_packet->data, length);
}

static void bootFirmVerify(cmd_t *p_cmd)
{
  uint32_t addr = 0;
  uint32_t length = 0;
  uint16_t crc;
  uint16_t err_code = CMD_OK;
  uint32_t rd_len;
  uint8_t  rd_buf[128];
  firm_tag_t tag;

  do 
  {
    firm_tag_t *p_tag = (firm_tag_t *)&tag;

    flashRead(FLASH_ADDR_UPDATE, (uint8_t *)p_tag, sizeof(firm_tag_t));


    if (p_tag->magic_number != TAG_MAGIC_NUMBER)
    {
      err_code = ERR_BOOT_TAG_MAGIC;
      break;
    }

    if (p_tag->fw_size >= FLASH_SIZE_FIRM)
    {
      err_code = ERR_BOOT_TAG_SIZE;
      break;
    }

    addr   = FLASH_ADDR_UPDATE + p_tag->fw_addr;
    length = p_tag->fw_size;
    crc    = 0;

    uint32_t index;

    index = 0;
    while (index < length)
    {
      rd_len = length-index;
      if  (rd_len > 128)
        rd_len = 128;

      if (flashRead(addr + index, rd_buf, rd_len) != true)
      {
        err_code = ERR_BOOT_FLASH_READ;
        break;
      }

      index += rd_len;

      for (uint32_t i=0; i<rd_len; i++)
      {
        utilUpdateCrc(&crc, rd_buf[i]);
      }
    }

    if (err_code == CMD_OK)
    {
      if (p_tag->fw_crc != crc)
      {
        err_code = ERR_BOOT_FW_CRC;
      }
    }
  } while(0);

  cmdSendResp(p_cmd, p_cmd->packet.cmd, err_code, NULL, 0);
}

static void bootFirmUpdate(cmd_t *p_cmd)
{
  uint16_t err_code;


  err_code = bootUpdateFirm();  

  cmdSendResp(p_cmd, p_cmd->packet.cmd, err_code, NULL, 0); 
  delay(100);

  if (err_code == CMD_OK)
  {
    bootJumpFirm();
  }
}

static void bootFirmJump(cmd_t *p_cmd)
{
  cmdSendResp(p_cmd, p_cmd->packet.cmd, CMD_OK, NULL, 0); 
}

static void bootFirmBegin(cmd_t *p_cmd)
{
  cmd_packet_t *p_packet = &p_cmd->packet;
  uint16_t err_code = CMD_OK;


  fw_receive_size = 0;
  
  if (p_packet->length == sizeof(boot_begin_t))
  {
    memcpy(&boot_begin, p_packet->data, sizeof(boot_begin_t));
    is_begin = true;
  } 
  else
  {
    err_code = ERR_BOOT_WRONG_RANGE;
  }

  cmdSendResp(p_cmd, p_cmd->packet.cmd, err_code, NULL, 0);
}

static void bootFirmEnd(cmd_t *p_cmd)
{
  uint16_t err_code = CMD_OK;

  is_begin = false;

  cmdSendResp(p_cmd, p_cmd->packet.cmd, err_code, NULL, 0);
}

bool cmdBootProcess(cmd_t *p_cmd)
{
  bool ret = true;


  switch(p_cmd->packet.cmd)
  {
    case BOOT_CMD_INFO:
      bootInfo(p_cmd);
      break;

    case BOOT_CMD_VERSION:
      bootVersion(p_cmd);
      break;

    case BOOT_CMD_FW_VER:
      bootFirmVersion(p_cmd);
      break;

    case BOOT_CMD_FW_ERASE:
      bootFirmErase(p_cmd);
      break;

    case BOOT_CMD_FW_WRITE:
      bootFirmWrite(p_cmd);
      break;
      
    case BOOT_CMD_FW_READ:
      bootFirmRead(p_cmd);
      break;

    case BOOT_CMD_FW_VERIFY:
      bootFirmVerify(p_cmd);
      break;

    case BOOT_CMD_FW_UPDATE:
      bootFirmUpdate(p_cmd);
      break;

    case BOOT_CMD_FW_JUMP:
      bootFirmJump(p_cmd);
      break;

    case BOOT_CMD_FW_BEGIN:
      bootFirmBegin(p_cmd);
      break;

    case BOOT_CMD_FW_END:
      bootFirmEnd(p_cmd);
      break;

    default:
      ret = false;
      break;  
  }

  return ret;
}

