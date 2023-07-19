#include "ap.h"



void apInit(void)
{
  threadInit();
}

void apMain(void)
{
  while(1)
  {
    threadUpdate();    
  }
}

