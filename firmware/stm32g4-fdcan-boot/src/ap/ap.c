#include "ap.h"
#include "thread/boot/boot.h"
#include "thread/cmd/cmd_thread.h"


bool is_run_fw = true;
bool is_update_fw = false;




void apInit(void)
{
  uint32_t boot_param;
  uint16_t err_code;

  boot_param = resetGetBootMode();

  if (boot_param & (1<<MODE_BIT_BOOT))
  {
    boot_param &= ~(1<<MODE_BIT_BOOT);
    resetSetBootMode(boot_param);
    
    is_run_fw = false;
  }

  if (buttonGetPressed(HW_BUTTON_CH_BOOT) == true)
  {
    is_run_fw = false;
  }

  if (boot_param & (1<<MODE_BIT_UPDATE))
  {
    boot_param &= ~(1<<MODE_BIT_UPDATE);
    resetSetBootMode(boot_param);
    
    is_run_fw = true;
    is_update_fw = true;
  }
  logPrintf("\n");


  if (is_update_fw)
  {
    logPrintf("[  ] bootUpdateFirm()\r");
    err_code = bootUpdateFirm();
    logPrintf("[%s]\n", err_code==CMD_OK ? "OK":"NG");
    if (err_code != CMD_OK)
      logPrintf("     err : 0x%04X\n", err_code);
  }

  if (faultIsReady())
  {
    logPrintf("[  ] fault ready\n");
    ledOn(HW_LED_CH_FAULT);
    is_run_fw = false;
  }

  if (is_run_fw)
  {
    logPrintf("[  ] bootJumpFirm()\r");
    err_code = bootJumpFirm();
    logPrintf("[%s]\n", err_code==CMD_OK ? "OK":"NG");
    if (err_code != CMD_OK)
      logPrintf("     err : 0x%04X\n", err_code);
  }

  logPrintf("\n");
  logPrintf("Boot Mode..\n");  
}

void apMain(void)
{
  uint32_t pre_time;
  

  cmdThreadInit();

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 100)
    {
      pre_time = millis();
      ledToggle(HW_LED_CH_DEBUG);
    }    

    if (cmdThreadUpdate() == true)
    {
      ledToggle(HW_LED_CH_DOWN);
    }    
  }
}

