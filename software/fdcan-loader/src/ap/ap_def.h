#ifndef AP_DEF_H_
#define AP_DEF_H_

#include "hw.h"


enum
{
  ARG_OPTION_PORT = (1<<0),
  ARG_OPTION_BAUD = (1<<1),
  ARG_OPTION_FILE = (1<<2),
  ARG_OPTION_RUN  = (1<<3),
  ARG_OPTION_ADDR = (1<<4),
};

typedef struct
{
  uint8_t mode;
  bool    is_udp;
  bool    is_audio;
  uint32_t arg_bits;
  char     port_str[128];
  uint32_t port_baud;
  char     file_str[128];
  bool     run_fw;
  uint8_t  type;

  uint32_t tx_block_len;
} arg_option_t;


void apExit(void);

#endif