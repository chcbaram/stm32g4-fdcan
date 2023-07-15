#ifndef W5300_REGS_H_
#define W5300_REGS_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_W5300



typedef union
{
  uint8_t  d8[2];
  uint16_t d16;
} reg16_t;

typedef union
{
  uint8_t  d8[4];
  uint16_t d16[2];
  uint32_t d32;
} reg32_t;

typedef union
{
  uint8_t  d8[6];
  uint16_t d16[3];
} reg48_t;

typedef union
{
  uint8_t  d8[8];
  uint16_t d16[4];
} reg64_t;


typedef struct
{
  __IO reg16_t MR;              // 0x200 SOCKET0 Mode Register
  __IO reg16_t CR;              // 0x202 SOCKET0 Command Register
  __IO reg16_t IMR;             // 0x204 SOCKET0 Interrupt Mask Register
  __IO reg16_t IR;              // 0x206 SOCKET0 Interrupt Register
  __IO reg16_t SSR;             // 0x208 SOCKET0 Socket Status Register
  __IO reg16_t PORTR;           // 0x20A SOCKET0 Source Port Register
  __IO reg48_t DHAR;            // 0x20C SOCKET0 Destination Hardware Address Register
  __IO reg16_t DPORTR;          // 0x212 SOCKET0 Destination Port Register
  __IO reg32_t DIPR;            // 0x214 SOCKET0 Destination IP Address
  __IO reg16_t MSSR;            // 0x218 SOCKET0 Maximum Segment Size Register
  __IO reg16_t PORTOR;          // 0x21A SOCKET0 Keep Alive Time Register
                                // 0x21B SOCKET0 Protocol Number Register
  __IO reg16_t TOSR;            // 0x21C SOCKET0 TOS Register
  __IO reg16_t TTLR;            // 0x21E SOCKET0 TTL Register
  __IO reg32_t TX_WRSR;         // 0x220 SOCKET0 TX Write Size Register
  __IO reg32_t TX_FSR;          // 0x224 SOCKET0 TX Free Size Register
  __IO reg32_t RX_RSR;          // 0x228 SOCKET0 RX Receive Size Register
  __IO reg16_t FRAGR;           // 0x22C SOCKET0 FLAG Register
  __IO reg16_t TX_FIFOR;        // 0x22E SOCKET0 TX FIFO Register
  __IO reg16_t RX_FIFOR;        // 0x230 SOCKET0 RX FIFO Register
  __IO uint8_t r0[0x0E];
} W5300_REG_SOCKET_t;


typedef struct
{
  //-- MODE
  //
  __IO reg16_t  MR;             // 0x000 Mode Register

  //-- COMMON
  //
  __IO reg16_t  IR;             // 0x002 Interrupt Register
  __IO reg16_t  IMR;            // 0x004 Interrupt Mask Register
  __IO uint8_t  r0[2];          // 0x006
  __IO reg48_t  SHAR;           // 0x008 Source Hardware Address Register
  __IO uint8_t  r1[2];          // 0x00E
  __IO reg32_t  GAR;            // 0x010 Gateway Address Register
  __IO reg32_t  SUBR;           // 0x014 Subnet Mask Register
  __IO reg32_t  SIPR;           // 0x018 Source IP Address Register   
  __IO reg16_t  RTR;            // 0x01C Retransmission Timeout-value Register
  __IO reg16_t  RCR;            // 0x01E Retransmission Retry-count Register
  __IO reg64_t  TMSR;           // 0x020 Transmit Memory Size Register of SOCKET[0~7]
  __IO reg64_t  RMSR;           // 0x028 Receive Memory Size Register of SOCKET[0~7]
  __IO reg16_t  MTYPER;         // 0x030 Memory Block Type Register
  __IO reg16_t  PATR;           // 0x032 PPPoE Authentication Register
  __IO uint8_t  r2[2];          // 0x034
  __IO reg16_t  PTIMER;         // 0x036 PPP LCP Request Time Register
  __IO reg16_t  PMAGICR;        // 0x038 PPP LCP Magic Number Register
  __IO uint8_t  r3[2];          // 0x03A
  __IO reg16_t  PSIDR;          // 0x03C PPP Session ID Register
  __IO uint8_t  r4[2];          // 0x03E
  __IO reg48_t  PDHAR;          // 0x040 PPP Destination Hardware Address Register
  __IO uint8_t  r5[2];          // 0x046
  __IO reg32_t  UIPR;           // 0x048 Unreachable IP Address Register
  __IO reg16_t  UPORTR;         // 0x04C Unreachable Port Number Register
  __IO reg16_t  FMTUR;          // 0x04E Fragment MTU Register
  __IO uint8_t  r6[0x10];       // 0x050
  __IO reg16_t  P0_BRDYR;       // 0x060 PIN "BRDY0" Configure Register
  __IO reg16_t  P0_BDPTHR;      // 0x062 PIN "BRDY0" Buffer Depth Register
  __IO reg16_t  P1_BRDYR;       // 0x064 PIN "BRDY1" Configure Register
  __IO reg16_t  P1_BDPTHR;      // 0x066 PIN "BRDY1" Buffer Depth Register
  __IO reg16_t  P2_BRDYR;       // 0x068 PIN "BRDY2" Configure Register
  __IO reg16_t  P2_BDPTHR;      // 0x06A PIN "BRDY2" Buffer Depth Register
  __IO reg16_t  P3_BRDYR;       // 0x06C PIN "BRDY3" Configure Register
  __IO reg16_t  P3_BDPTHR;      // 0x06E PIN "BRDY3" Buffer Depth Register
  __IO uint8_t  r7[0x8E];       // 0x070
  __IO reg16_t  IDR;            // 0x0FE W5300 ID Register


  __IO uint8_t  r8[0x100];      // 0x100

  //-- SOCKET
  //
  W5300_REG_SOCKET_t SOCKET[8];

} W5300_REGS_t;


#endif

#ifdef __cplusplus
}
#endif

#endif