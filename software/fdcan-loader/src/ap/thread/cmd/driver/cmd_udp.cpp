#include "cmd_udp.h"
#include "ez_hal.h"
#include <iostream>
#include <thread>


typedef struct
{
  char     ip_addr[32];
  uint32_t port;
} cmd_udp_args_t;


static bool open(void *args);
static bool close(void *args);  
static uint32_t available(void *args);
static bool flush(void *args);
static uint8_t read(void *args);
static uint32_t write(void *args, uint8_t *p_data, uint32_t length);  
static bool ioctl(uint32_t ctl, void *p_data, uint32_t length);

using namespace ez;
static ez_socket_t ez_udp;
static std::thread rxd_thread;
static std::thread con_thread;
static bool is_init = false;
static bool is_open = false;
static uint8_t uart_rx_buf[8*1024];
static qbuffer_t uart_rx_q;





bool cmdUdpInitDriver(cmd_driver_t *p_driver, const char *ip_addr, uint32_t port)
{
  cmd_udp_args_t *p_args = (cmd_udp_args_t *)p_driver->args;


  qbufferCreate(&uart_rx_q, uart_rx_buf, 8*1024);

  p_args->port  = port;
  strncpy(p_args->ip_addr, ip_addr, 32);

  p_driver->open = open;
  p_driver->close = close;
  p_driver->available = available;
  p_driver->flush = flush;
  p_driver->read = read;
  p_driver->write = write;
  p_driver->ioctl = ioctl;

  is_init = true;

  return true;
}

bool open(void *args)
{
  bool ret = false;
  cmd_udp_args_t *p_args = (cmd_udp_args_t *)args;


  close(args);

  socketInit(&ez_udp, EZ_SOCKET_CLIENT, EZ_SOCKET_UDP);
  socketCreate(&ez_udp);
  socketSetBroadCast(&ez_udp, true);

  con_thread = std::thread([=] 
  {
    ez::delay(1000);
    if (is_open == false)
    {
      socketDeInit(&ez_udp);          
    }
  });


  is_open = true;
  ret = true;

  rxd_thread = std::thread([=] 
  {
    uint8_t rx_buf[512];

    while(is_open)
    {
      int rx_len;
      rx_len = socketRead(&ez_udp, (uint8_t *)rx_buf, 512);
      if (rx_len > 0)
      {
        qbufferWrite(&uart_rx_q, rx_buf, rx_len);
        // for (int i=0; i<rx_len; i++)
        // {
        //   printf("rx : 0x%02X\n", rx_buf[i]);
        // }
      }    
    }  
  });


  socketSetRemoteIP(&ez_udp, (const char *)p_args->ip_addr, p_args->port);

  logDebug("%s : %d\n", p_args->ip_addr, p_args->port);

  return ret;
}

bool close(void *args)
{
  if (is_open == false) return true;

  is_open = false;

  socketClose(&ez_udp);
  socketDestroy(&ez_udp);

  rxd_thread.join();
  con_thread.join();
  
  socketDeInit(&ez_udp);
  
  return true;  
}

uint32_t available(void *args)
{
  return qbufferAvailable(&uart_rx_q);
}

bool flush(void *args)
{  
  qbufferFlush(&uart_rx_q);
  return true;
}

uint8_t read(void *args)
{
  uint8_t ret;

  qbufferRead(&uart_rx_q, &ret, 1);
  return ret;
}

uint32_t write(void *args, uint8_t *p_data, uint32_t length)
{
  int ret = 0;

  if (is_init == false) return 0;

  if (is_open == true)
  {
    ret = socketWrite(&ez_udp, (const uint8_t *)p_data, length);  
  }
  if (ret < 0) ret = 0;

  return (uint32_t)ret;
}

bool ioctl(uint32_t ctl, void *p_data, uint32_t length)
{
  if (ctl == 0)
  {
    ez_ip_addr_t ip_addr;

    socketGetRemoteIP(&ez_udp, &ip_addr);
    logPrintf("ip : %s : %d\n", ip_addr.ip_addr, ip_addr.port);
  }
  return true;
}