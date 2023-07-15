#ifndef W5300_H_
#define W5300_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_W5300
#include "w5300_regs.h"


#define W5300_BASE_ADDR   HW_W5300_BASE_ADDR
#define W5300_REGS       ((W5300_REGS_t *)(HW_W5300_BASE_ADDR + 0x000))


#define SWAP16(x)   (uint16_t)(((uint16_t)(x)<<8) | ((uint16_t)(x)>>8))



bool w5300Init(void);
bool w5300IsInit(void);
bool w5300Reset(void);

bool w5300SetNetInfo(const uint8_t ip[4], 
                     const uint8_t subnet[4], 
                     const uint8_t gateway[4], 
                     const uint8_t mac[6]);
bool w5300GetNetInfo(uint8_t ip[4], uint8_t subnet[4], uint8_t gateway[4], uint8_t mac[6]);


#endif

#ifdef __cplusplus
}
#endif

#endif