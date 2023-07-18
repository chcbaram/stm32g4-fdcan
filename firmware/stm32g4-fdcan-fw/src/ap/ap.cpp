#include "ap.h"

#include "cmd/cmd_thread.h"
#include "mode/mode_thread.h"
#include "common/cli_thread.h"
#include "common/led_thread.h"




void apInit(void)
{
  threadInit();

  threadCreate(cliThread());
  threadCreate(modeThread());
  threadCreate(cmdThread());
  threadCreate(ledThread());

  threadBegin();
}

void apMain(void)
{
  while(1)
  {
    threadUpdate();    
  }
}

