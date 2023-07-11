#include "eeprom.h"
#include "cli.h"


#ifdef _USE_HW_EEPROM
#include "eeprom_emul.h"

#if PAGES_NUMBER > HW_EEPROM_MAX_PAGES
#error "EEPROM PAGES_NUMBER OVER"
#endif


#ifdef _USE_HW_CLI
void cliEeprom(cli_args_t *args);
#endif


#define EEPROM_MAX_SIZE   NB_OF_VARIABLES


static bool is_init = false;
static __IO bool is_erasing = false;


bool eepromInit()
{
  EE_Status ee_ret = EE_OK;


  HAL_NVIC_SetPriority(FLASH_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(FLASH_IRQn);

  HAL_FLASH_Unlock();
  ee_ret = EE_Init(EE_FORCED_ERASE);
  HAL_FLASH_Lock();
  logPrintf("[%s] eepromInit()\n", ee_ret == EE_OK ? "OK":"NG");

  is_init = ee_ret == EE_OK ? true:false;

#ifdef _USE_HW_CLI
  cliAdd("eeprom", cliEeprom);
#endif

  return is_init;
}

bool eepromIsInit(void)
{
  return is_init;
}

bool eepromValid(uint32_t addr)
{
  if (addr >= EEPROM_MAX_SIZE)
  {
    return false;
  }

  return is_init;
}

bool eepromReadByte(uint32_t addr, uint8_t *p_data)
{
  bool ret = true;
  EE_Status ee_ret = EE_OK;
  uint16_t ee_addr;

  if (addr >= EEPROM_MAX_SIZE)
    return false;
  if (is_init != true)
    return false;
  ee_addr = addr + 1;

  HAL_FLASH_Unlock();
  ee_ret = EE_ReadVariable8bits(ee_addr, p_data);
  HAL_FLASH_Lock();
  if (ee_ret != EE_OK)
  {
    if (ee_ret == EE_NO_DATA)
      *p_data = 0;
    else
      ret = false;
  }
  return ret;
}

bool eepromWriteByte(uint32_t addr, uint8_t data_in)
{
  bool ret = true;
  EE_Status ee_ret = EE_OK;
  uint16_t ee_addr;
  uint32_t pre_time = millis();

  if (addr >= EEPROM_MAX_SIZE)
    return false;
  if (is_init != true)
    return false;

  ee_addr = addr + 1;

  HAL_FLASH_Unlock();
  ee_ret = EE_WriteVariable8bits(ee_addr, data_in);
  if (ee_ret != EE_OK)
  {
    if ((ee_ret & EE_STATUSMASK_CLEANUP) == EE_STATUSMASK_CLEANUP) 
    {
      is_erasing = true;
      ee_ret = EE_CleanUp_IT();
      while (is_erasing == true) 
      { 
        if (millis()-pre_time >= 500)
        {
          ret = false;
          break;
        }
      }      
    }    
    else
    {
      ret = false;
    }
  }  
  HAL_FLASH_Lock();

  return ret;
}

bool eepromRead(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret = true;
  uint32_t i;


  for (i=0; i<length; i++)
  {
    if (eepromReadByte(addr, &p_data[i]) != true)
    {
      ret = false;
      break;
    }
  }

  return ret;
}

bool eepromWrite(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret;
  uint32_t i;


  for (i=0; i<length; i++)
  {
    ret = eepromWriteByte(addr, p_data[i]);
    if (ret == false)
    {
      break;
    }
  }

  return ret;
}

uint32_t eepromGetLength(void)
{
  return EEPROM_MAX_SIZE;
}

bool eepromFormat(void)
{
  bool ret = true;
  EE_Status ee_ret = EE_OK;

  HAL_FLASH_Unlock();
  ee_ret = EE_Format(EE_FORCED_ERASE);
  HAL_FLASH_Lock();
  if (ee_ret != EE_OK)
  {
    ret = false;
  }  
  return ret;
}

void FLASH_IRQHandler(void)
{
  HAL_FLASH_IRQHandler();
}

void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
  /* Call CleanUp callback when all requested pages have been erased */
  if (ReturnValue == 0xFFFFFFFF)
  {
    EE_EndOfCleanup_UserCallback();
  }
}

void EE_EndOfCleanup_UserCallback(void)
{
  is_erasing = false;
}

#ifdef _USE_HW_CLI
void cliEeprom(cli_args_t *args)
{
  bool ret = true;
  uint32_t i;
  uint32_t addr;
  uint32_t length;
  uint8_t  data;
  uint32_t pre_time;
  bool eep_ret;


  if (args->argc == 1)
  {
    if(args->isStr(0, "info") == true)
    {
      cliPrintf("eeprom init   : %d\n", eepromIsInit());
      cliPrintf("eeprom length : %d bytes\n", eepromGetLength());
    }
    else if(args->isStr(0, "format") == true)
    {
      if (eepromFormat() == true)
      {
        cliPrintf("format OK\n");
      }
      else
      {
        cliPrintf("format Fail\n");
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (args->argc == 3)
  {
    if(args->isStr(0, "read") == true)
    {
      bool ee_ret;
      addr   = (uint32_t)args->getData(1);
      length = (uint32_t)args->getData(2);

      if (length > eepromGetLength())
      {
        cliPrintf( "length error\n");
      }
      for (i=0; i<length; i++)
      {
        ee_ret = eepromReadByte(addr+i, &data);
        cliPrintf( "addr : %d\t 0x%02X, ret %d\n", addr+i, data, ee_ret);
      }
    }
    else if(args->isStr(0, "write") == true)
    {
      addr = (uint32_t)args->getData(1);
      data = (uint8_t )args->getData(2);

      pre_time = millis();
      eep_ret = eepromWriteByte(addr, data);

      cliPrintf( "addr : %d\t 0x%02X %dms\n", addr, data, millis()-pre_time);
      if (eep_ret)
      {
        cliPrintf("OK\n");
      }
      else
      {
        cliPrintf("FAIL\n");
      }
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    cliPrintf( "eeprom info\n");
    cliPrintf( "eeprom format\n");
    cliPrintf( "eeprom read  [addr] [length]\n");
    cliPrintf( "eeprom write [addr] [data]\n");
  }

}
#endif /* _USE_HW_CMDIF_EEPROM */


#endif /* _USE_HW_EEPROM */