/*
 * usb.h
 *
 *  Created on: 2018. 3. 16.
 *      Author: HanCheol Cho
 */

#ifndef SRC_HW_USB_CDC_USB_H_
#define SRC_HW_USB_CDC_USB_H_




#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_USB



#include "usbd_core.h"
#include "usbd_desc.h"

#if HW_USE_CDC == 1
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#endif

#if HW_USE_MSC == 1
#include "usbd_msc.h"
#endif

typedef enum UsbMode
{
  USB_NON_MODE,
  USB_CDC_MODE,
  USB_MSC_MODE
} UsbMode_t;

typedef enum UsbType
{
  USB_CON_CDC = 0,
  USB_CON_CLI = 1,
  USB_CON_CAN = 2,
  USB_CON_ESP = 3,
} UsbType_t;

bool usbInit(void);
bool usbBegin(UsbMode_t usb_mode);
void usbDeInit(void);
bool usbIsOpen(void);
bool usbIsConnect(void);

UsbMode_t usbGetMode(void);
UsbType_t usbGetType(void);


#endif


#ifdef __cplusplus
}
#endif




#endif /* SRC_HW_USB_CDC_USB_H_ */
