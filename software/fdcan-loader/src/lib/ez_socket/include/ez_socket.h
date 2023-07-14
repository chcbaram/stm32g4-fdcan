#ifndef EZ_SOCKET_H_
#define EZ_SOCKET_H_


#include "ez_def.h"


namespace ez
{


#ifndef _WIN32

// Unix
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct sockaddr SOCKADDR;
typedef int SOCKET;
#else

// Windows
#ifndef _WINSOCKAPI_
#include <winsock2.h>
#endif

#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#endif



typedef enum
{
  EZ_SOCKET_SERVER,
  EZ_SOCKET_CLIENT,
} ez_role_t;

typedef enum
{
  EZ_SOCKET_TCP,
  EZ_SOCKET_UDP,
} ez_protocol_t;

typedef struct
{
  char     ip_addr[32];
  uint32_t port;

#ifdef _WIN32
  SOCKADDR_IN socket_addr;
#else
  struct sockaddr_in socket_addr;
#endif
} ez_ip_addr_t;

typedef struct
{
  bool is_init;
  bool is_created;
  bool is_connected;
  ez_err_t  err_code;

#ifdef _WIN32
  SOCKET h_socket;
  SOCKADDR_IN socket_addr;

  SOCKET h_client;
  SOCKADDR_IN client_addr;
#else
  int h_socket;
  struct sockaddr_in socket_addr;

  int h_client;
  struct sockaddr_in client_addr;
#endif

  bool is_local_ip;  
  bool is_remote_ip;
  bool is_broadcast;

  ez_ip_addr_t local_ip;
  ez_ip_addr_t remote_ip;

  ez_role_t role;
  ez_protocol_t protocol;

  uint32_t packet_id;

} ez_socket_t;



ez_err_t socketInit(ez_socket_t *p_socket, ez_role_t socket_role, ez_protocol_t protocol=EZ_SOCKET_TCP);
ez_err_t socketDeInit(ez_socket_t *p_socket);
ez_err_t socketCreate(ez_socket_t *p_socket);
ez_err_t socketSetBroadCast(ez_socket_t *p_socket, bool broad_cast);
ez_err_t socketSetReceiveTimeout(ez_socket_t *p_socket, uint32_t milliseconds);
ez_err_t socketBind(ez_socket_t *p_socket, const char *ip_addr, uint32_t port);
ez_err_t socketConnect(ez_socket_t *p_socket, const char *ip_addr, uint32_t port);
ez_err_t socketListen(ez_socket_t *p_socket, int baklog);
ez_err_t socketAccept(ez_socket_t *p_socket);
int      socketWrite(ez_socket_t *p_socket, const uint8_t *p_data, uint32_t length);
int      socketRead(ez_socket_t *p_socket, uint8_t *p_data, uint32_t length);
int      socketReadForLength(ez_socket_t *p_socket, uint8_t *p_data, uint32_t length);
ez_err_t socketClose(ez_socket_t *p_socket);
ez_err_t socketDestroy(ez_socket_t *p_socket);
bool     socketIsConnected(ez_socket_t *p_socket);
bool     socketIsRemtoeIP(ez_socket_t *p_socket);
ez_err_t socketGetRemoteIP(ez_socket_t *p_socket, ez_ip_addr_t *p_ip_addr);
ez_err_t socketSetRemoteIP(ez_socket_t *p_socket, const char *ip_addr, uint32_t port);

}

#endif