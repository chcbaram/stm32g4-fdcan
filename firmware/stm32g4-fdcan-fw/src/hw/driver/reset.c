#include "reset.h"
#include "rtc.h"
#include "cli.h"

#ifdef _USE_HW_RESET


static void cliReset(cli_args_t *args);


static bool is_init = false;
static uint32_t reset_bits = 0;
static uint32_t boot_mode = 0;


static const char *reset_bit_str[] = 
  {
    "RESET_BIT_POWER",
    "RESET_BIT_PIN",
    "RESET_BIT_WDG",
    "RESET_BIT_SOFT",
  };

static const char *mode_bit_str[] = 
  {
    "MODE_BIT_BOOT",
    "MODE_BIT_UPDATE",
  };



bool resetInit(void)
{
  bool ret;


#if defined(HW_RESET_BOOT) && HW_RESET_BOOT > 0
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
  {
   reset_bits |= (1<<RESET_BIT_PIN);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET)
  {
   reset_bits |= (1<<RESET_BIT_POWER);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
  {
   reset_bits |= (1<<RESET_BIT_WDG);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET)
  {
   reset_bits |= (1<<RESET_BIT_WDG);
  }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
  {
   reset_bits |= (1<<RESET_BIT_SOFT);
  }

  __HAL_RCC_CLEAR_RESET_FLAGS();

  rtcSetReg(HW_RTC_RESET_BITS, reset_bits);
#else
  rtcGetReg(HW_RTC_RESET_BITS, &reset_bits);
#endif

  rtcGetReg(HW_RTC_BOOT_MODE, &boot_mode);
  rtcSetReg(HW_RTC_BOOT_MODE, 0);  


  cliPrintf("[OK] resetInit()\n");
  for (int i=0; i<RESET_BIT_MAX; i++)
  {
    if (reset_bits & (1<<i))
    {
      cliPrintf("     %s\n", reset_bit_str[i]);
    }
  }
  for (int i=0; i<RESET_BIT_MAX; i++)
  {
    if (boot_mode & (1<<i))
    {
      cliPrintf("     %s\n", mode_bit_str[i]);
    }
  }  

  is_init = true;
  cliAdd("reset", cliReset);

  ret = is_init;
  return ret;
}

void resetLog(void)
{

}

void resetToBoot(void)
{
  resetSetBootMode(1<<MODE_BIT_BOOT);
  resetToReset();
}

void resetToReset(void)
{
  HAL_NVIC_SystemReset();
}

uint32_t resetGetBits(void)
{
  return reset_bits;
}

void resetSetBits(uint32_t data)
{
  reset_bits = data;
}

void resetSetBootMode(uint32_t data)
{
  boot_mode = data;
  rtcSetReg(HW_RTC_BOOT_MODE, data);
}

uint32_t resetGetBootMode(void)
{
  return boot_mode;
}


void cliReset(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliPrintf("Reset Bits\n");
    for (int i=0; i<RESET_BIT_MAX; i++)
    {
      if (reset_bits & (1<<i))
      {
        cliPrintf("      %s\n", reset_bit_str[i]);
      }
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "boot"))
  {
    resetSetBootMode(1<<MODE_BIT_BOOT);
    resetToReset();
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "update"))
  {
    resetSetBootMode(1<<MODE_BIT_UPDATE);
    resetToReset();
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "reset"))
  {
    resetToReset();
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("reset info\n");
    cliPrintf("reset boot\n");
    cliPrintf("reset update\n");
    cliPrintf("reset reset\n");
  }
}


#endif



