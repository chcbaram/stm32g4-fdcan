#include "flash.h"
#include "spi_flash.h"
#include "cli.h"


#define FLASH_MAX_BANK            1
#define FLASH_MAX_SECTOR          64
#define FLASH_WRITE_SIZE          8
#define FLASH_SECTOR_SIZE         2048



#ifdef _USE_HW_CLI
static void cliFlash(cli_args_t *args);
#endif






bool flashInit(void)
{

  logPrintf("[OK] flashInit()\n");

#ifdef _USE_HW_CLI
  cliAdd("flash", cliFlash);
#endif
  return true;
}

bool flashInSector(uint16_t sector_num, uint32_t addr, uint32_t length)
{
  bool ret = false;

  uint32_t sector_start;
  uint32_t sector_end;
  uint32_t flash_start;
  uint32_t flash_end;


  sector_start = FLASH_BASE + (sector_num * FLASH_SECTOR_SIZE);
  sector_end   = sector_start + FLASH_SECTOR_SIZE - 1;
  flash_start  = addr;
  flash_end    = addr + length - 1;


  if (sector_start >= flash_start && sector_start <= flash_end)
  {
    ret = true;
  }

  if (sector_end >= flash_start && sector_end <= flash_end)
  {
    ret = true;
  }

  if (flash_start >= sector_start && flash_start <= sector_end)
  {
    ret = true;
  }

  if (flash_end >= sector_start && flash_end <= sector_end)
  {
    ret = true;
  }

  return ret;
}

bool flashErase(uint32_t addr, uint32_t length)
{
  bool ret = false;

  int32_t start_sector = -1;
  int32_t end_sector = -1;


#ifdef _USE_HW_SPI_FLASH
  if (addr >= spiFlashGetAddr() && addr < (spiFlashGetAddr() + spiFlashGetLength()))
  {
    ret = spiFlashErase(addr - spiFlashGetAddr(), length);
    return ret;
  }
#endif


  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

  start_sector = -1;
  end_sector = -1;

  for (int i=0; i<FLASH_MAX_SECTOR; i++)
  {
    if (flashInSector(i, addr, length) == true)
    {
      if (start_sector < 0)
      {
        start_sector = i;
      }
      end_sector = i;
    }
  }

  if (start_sector >= 0)
  {
    FLASH_EraseInitTypeDef EraseInit;
    uint32_t SectorError;
    HAL_StatusTypeDef status;


    EraseInit.Page         = start_sector;
    EraseInit.NbPages      = (end_sector - start_sector) + 1;
    EraseInit.TypeErase    = FLASH_TYPEERASE_PAGES;
    EraseInit.Banks        = FLASH_BANK_1;

    status = HAL_FLASHEx_Erase(&EraseInit, &SectorError);
    if (status == HAL_OK)
    {
      ret = true;
    }
  }

  HAL_FLASH_Lock();

  return ret;
}

bool flashWrite(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret = true;
  HAL_StatusTypeDef status;


#ifdef _USE_HW_SPI_FLASH
  if (addr >= spiFlashGetAddr() && addr < (spiFlashGetAddr() + spiFlashGetLength()))
  {
    ret = spiFlashWrite(addr - spiFlashGetAddr(), p_data, length);
    return ret;
  }
#endif

  if (addr%FLASH_WRITE_SIZE != 0)
  {
    return false;
  }

  HAL_FLASH_Unlock();

  for (int i=0; i<length; i+=FLASH_WRITE_SIZE)
  {
    uint64_t data;

    memcpy(&data, &p_data[i], FLASH_WRITE_SIZE);
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr + i, data);
    if (status != HAL_OK)
    {
      ret = false;
      break;
    }
  }

  HAL_FLASH_Lock();

  return ret;
}

bool flashRead(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret = true;
  uint8_t *p_byte = (uint8_t *)addr;


#ifdef _USE_HW_SPI_FLASH
  if (addr >= spiFlashGetAddr() && addr < (spiFlashGetAddr() + spiFlashGetLength()))
  {
    ret = spiFlashRead(addr - spiFlashGetAddr(), p_data, length);
    return ret;
  }
#endif

  for (int i=0; i<length; i++)
  {
    p_data[i] = p_byte[i];
  }

  return ret;
}





#ifdef _USE_HW_CLI
void cliFlash(cli_args_t *args)
{
  bool ret = false;
  uint32_t i;
  uint32_t addr;
  uint32_t length;
  uint32_t pre_time;
  bool flash_ret;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliPrintf("flash addr  : 0x%X\n", 0x8000000);
    cliPrintf("spi   addr  : 0x%X\n", spiFlashGetAddr());
    
    ret = true;
  }

  if(args->argc == 3 && args->isStr(0, "read"))
  {
    uint8_t data;

    addr   = (uint32_t)args->getData(1);
    length = (uint32_t)args->getData(2);

    for (i=0; i<length; i++)
    {
      flash_ret = flashRead(addr+i, &data, 1);

      if (flash_ret == true)
      {
        cliPrintf( "addr : 0x%X\t 0x%02X\n", addr+i, data);
      }
      else
      {
        cliPrintf( "addr : 0x%X\t Fail\n", addr+i);
      }
    }

    ret = true;
  }
    
  if(args->argc == 3 && args->isStr(0, "erase"))
  {
    addr   = (uint32_t)args->getData(1);
    length = (uint32_t)args->getData(2);

    pre_time = millis();
    flash_ret = flashErase(addr, length);

    cliPrintf( "addr : 0x%X\t len : %d %d ms\n", addr, length, (millis()-pre_time));
    if (flash_ret)
    {
      cliPrintf("OK\n");
    }
    else
    {
      cliPrintf("FAIL\n");
    }

    ret = true;
  }
    
  if(args->argc == 3 && args->isStr(0, "write"))
  {
    uint32_t data;

    addr = (uint32_t)args->getData(1);
    data = (uint32_t)args->getData(2);

    pre_time = millis();
    flash_ret = flashWrite(addr, (uint8_t *)&data, 4);

    cliPrintf( "addr : 0x%X\t 0x%X %dms\n", addr, data, millis()-pre_time);
    if (flash_ret)
    {
      cliPrintf("OK\n");
    }
    else
    {
      cliPrintf("FAIL\n");
    }

    ret = true;
  }

  if(args->argc == 3 && args->isStr(0, "check"))
  {
    uint64_t data = 0;
    uint32_t block = 8;
    bool flash_ret;

    addr   = (uint32_t)args->getData(1);
    length = (uint32_t)args->getData(2);
    length -= (length % block);

    do
    {
      flash_ret = true;
      cliPrintf("flashErase()..");
      if (flashErase(addr, length) == false)
      {
        flash_ret = false;
        break;
      }
      cliPrintf("%s\n", flash_ret ? "OK" : "Fail");


      flash_ret = true;
      cliPrintf("flashWrite()..");
      for (uint32_t i=0; i<length; i+=block)
      {
        data = ((uint64_t)i<<32) | ((uint64_t)i<<0);
        if (flashWrite(addr + i, (uint8_t *)&data, block) == false)
        {
          cliPrintf("0x%X ", i);
          flash_ret = false;
          break;
        }
      }
      cliPrintf("%s\n", flash_ret ? "OK" : "Fail");

      flash_ret = true;
      cliPrintf("flashRead() ..");
      for (uint32_t i=0; i<length; i+=block)
      {
        data = 0;
        if (flashRead(addr + i, (uint8_t *)&data, block) == false)
        {
          cliPrintf("0x%X ", i);
          flash_ret = false;
          break;
        }
        if (data != (((uint64_t)i<<32)|((uint64_t)i<<0)))
        {
          cliPrintf("Check 0x%X ", i);
          flash_ret = false;
          break;
        }
      }  
      cliPrintf("%s\n", flash_ret ? "OK" : "Fail");


      flash_ret = true;
      cliPrintf("flashErase()..");
      if (flashErase(addr, length*4) == false)
      {
        flash_ret = false;
        break;
      }
      cliPrintf("%s\n", flash_ret ? "OK" : "Fail");
    } while (0);
    
    ret = true;
  }


  if (ret == false)
  {
    cliPrintf( "flash info\n");
    cliPrintf( "flash read  [addr] [length]\n");
    cliPrintf( "flash erase [addr] [length]\n");
    cliPrintf( "flash write [addr] [data]\n");
    cliPrintf( "flash check [addr] [length]\n");
  }
}
#endif