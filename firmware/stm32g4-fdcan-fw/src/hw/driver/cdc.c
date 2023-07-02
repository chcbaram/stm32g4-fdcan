/*
 * cdc.c
 *
 *  Created on: 2021. 11. 14.
 *      Author: baram
 */




#include "cdc.h"


#ifdef _USE_HW_CDC
#include "usb/usb_cdc/usbd_cdc_if.h"




static bool is_init = false;

bool cdcInit(void)
{
  bool ret = true;


  ret = cdcIfInit();

  is_init = ret;

  return ret;
}

bool cdcIsInit(void)
{
  return is_init;
}

bool cdcIsConnect(void)
{
  return cdcIfIsConnected();
}

uint32_t cdcAvailable(void)
{
  return cdcIfAvailable();
}

uint8_t cdcRead(void)
{
  return cdcIfRead();
}

uint32_t cdcWrite(uint8_t *p_data, uint32_t length)
{
  return cdcIfWrite(p_data, length);
}

uint32_t cdcGetBaud(void)
{
  return cdcIfGetBaud();
}

uint8_t cdcGetType(void)
{
  return cdcIfGetType();
}

#endif
