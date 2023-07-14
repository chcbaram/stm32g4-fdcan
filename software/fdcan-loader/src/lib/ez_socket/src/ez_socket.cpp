#include "ez_log.h"
#include "ez_socket.h"


namespace ez
{


bool is_init = false;

#ifdef _WIN32
static WSADATA wsa_data;
#endif




static ez_err_t socketIsValid(ez_socket_t *p_socket);






ez_err_t socketInit(ez_socket_t *p_socket, ez_role_t socket_role, ez_protocol_t protocol)
{
  logDebug("socketInit() : %s\n", socket_role == EZ_SOCKET_SERVER ? "Server":"Client");

  if (is_init == false)
  {
#ifdef _WIN32
    int ret;

    // lib begin
    ret = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (ret != 0)
    {
      EZ_LOG_TRACE_ERROR(ret);
      return EZ_ERR_SOCKET_INIT;
    }
#endif
    is_init = true;
  }

  p_socket->is_init = true;
  p_socket->is_created = false;
  p_socket->is_connected = false;
  p_socket->err_code = EZ_OK;
  p_socket->h_socket = -1;
  p_socket->h_client = -1;
  p_socket->role = socket_role;
  p_socket->packet_id = 0;
  p_socket->protocol = protocol;
  p_socket->is_local_ip = false;
  p_socket->is_remote_ip = false;
  p_socket->is_broadcast = false;

  return EZ_OK;
}

ez_err_t socketDeInit(ez_socket_t *p_socket)
{
  ez_err_t err_ret = EZ_OK;


  logDebug("socketDeInit(%d)\n", (int)is_init);

  if (is_init == false)
  {
    return EZ_OK;
  }


  socketClose(p_socket);
  socketDestroy(p_socket);

#ifdef _WIN32
  int ret;
  
  // lib end
  ret = WSACleanup();
  if (ret != 0)
  {
    EZ_LOG_TRACE_ERROR(ret);
    err_ret = (p_socket->err_code = EZ_ERR_SOCKET_DEINIT);
  }
#else
  err_ret = EZ_OK;
#endif

  is_init = false;

  return err_ret;
}

ez_err_t socketCreate(ez_socket_t *p_socket)
{
  int socket_type;
  int socket_protocol;


  logDebug("socketCreate() \n");

  if (is_init == false || p_socket->is_init == false)
  {
    EZ_LOG_TRACE_ERROR(0);
    return (p_socket->err_code = EZ_ERR);
  }

#ifdef _WIN32  
  if (p_socket->protocol == EZ_SOCKET_TCP)
  {
    socket_type = SOCK_STREAM;
    socket_protocol = IPPROTO_TCP;    
  }
  else
  {
    socket_type = SOCK_DGRAM;
    socket_protocol = IPPROTO_UDP;    
  }  
#else
  if (p_socket->protocol == EZ_SOCKET_TCP)
  {
    socket_type = SOCK_STREAM;
    socket_protocol = 0;    
  }
  else
  {
    socket_type = SOCK_DGRAM;
    socket_protocol = 0;    
  }
#endif

  // Create Server Socket
  p_socket->h_socket = socket(PF_INET, socket_type, socket_protocol);
  if (p_socket->h_socket <= 0)
  {
    EZ_LOG_TRACE_ERROR(p_socket->h_socket);
    return (p_socket->err_code = EZ_ERR_SOCKET_SOCKET);
  }

  // Time-Wait Remove
  int opt = 1;
  setsockopt(p_socket->h_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(int));

  // Nagle Off
  opt = 1;
  setsockopt(p_socket->h_socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&opt, sizeof(int));

  p_socket->is_created = true;

  return EZ_OK;
}

ez_err_t socketIsValid(ez_socket_t *p_socket)
{
  if (p_socket == NULL)
  {
    EZ_LOG_TRACE_ERROR(0);
    return EZ_ERR;
  }

  if (is_init == false || p_socket->is_init == false)
  {
    EZ_LOG_TRACE_ERROR(0);
    return (p_socket->err_code = EZ_ERR);
  }
  if (p_socket->is_created == false)
  {
    EZ_LOG_TRACE_ERROR(0);
    return (p_socket->err_code = EZ_ERR_SOCKET_CREATE);
  }

  return EZ_OK;
}

ez_err_t socketSetReceiveTimeout(ez_socket_t *p_socket, uint32_t milliseconds)
{
  ez_err_t err_ret;
  SOCKET h_socket;


  logDebug("socketSetReceiveTimeout() %d\n", milliseconds);

  err_ret = socketIsValid(p_socket);
  if (err_ret != EZ_OK)
  {
    EZ_LOG_TRACE_ERROR(0);
    return err_ret;
  }

  if (p_socket->role == EZ_SOCKET_SERVER)
  {
    h_socket = p_socket->h_client;
  }
  else
  {
    h_socket = p_socket->h_socket;
  }

#if defined(_WIN32)
  const DWORD timeout = static_cast<DWORD>(milliseconds);
  setsockopt(h_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(DWORD));
#else
  struct timeval tv;
  if (milliseconds >= 1000)
  {
      const int s = milliseconds / 1000;
      const int us = (milliseconds - s * 1000) * 1000;
      tv.tv_sec = s;
      tv.tv_usec = us;
  }
  else
  {
      tv.tv_sec = 0;
      tv.tv_usec = milliseconds * 1000;
  }

  setsockopt(h_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof (struct timeval));
#endif

  return EZ_OK;
}

ez_err_t socketSetBroadCast(ez_socket_t *p_socket, bool broad_cast)
{
  ez_err_t err_ret;
  SOCKET h_socket;


  logDebug("socketSetBroadCast(%s)\n", broad_cast ? "True":"False");

  err_ret = socketIsValid(p_socket);
  if (err_ret != EZ_OK)
  {
    EZ_LOG_TRACE_ERROR(0);
    return err_ret;
  }

  if (p_socket->role == EZ_SOCKET_SERVER)
  {
    h_socket = p_socket->h_client;
  }
  else
  {
    h_socket = p_socket->h_socket;
  }

  p_socket->is_broadcast = broad_cast;

  int opt = broad_cast;
  setsockopt(h_socket, SOL_SOCKET, SO_BROADCAST, (const char *)&opt, sizeof(opt));

  return EZ_OK;
}

ez_err_t socketBind(ez_socket_t *p_socket, const char *ip_addr, uint32_t port)
{
  ez_err_t err_ret;
  int ret;


  logDebug("socketBind() : ip %s, port %d\n", ip_addr, port);

  err_ret = socketIsValid(p_socket);
  if (err_ret != EZ_OK)
  {
    EZ_LOG_TRACE_ERROR(0);
    return err_ret;
  }

  // Create an address structure to put on the server socket
#ifdef _WIN32
  memset(&p_socket->socket_addr, 0, sizeof(p_socket->socket_addr));
  p_socket->socket_addr.sin_family = AF_INET;
  if (ip_addr != NULL)
  {
    p_socket->socket_addr.sin_addr.S_un.S_addr = inet_addr(ip_addr);
	  //inet_pton(AF_INET, (PCSTR)ip_addr, (PVOID)&p_socket->socket_addr.sin_addr);
    //strcpy_s(p_socket->ip_addr, ip_addr);
  }
  else
  {
    p_socket->socket_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
  }
  p_socket->socket_addr.sin_port = htons(port);
#else
    memset(&p_socket->socket_addr, 0, sizeof(p_socket->socket_addr));
    p_socket->socket_addr.sin_family = AF_INET;
    if (ip_addr != NULL)
    {      
      p_socket->socket_addr.sin_addr.s_addr=inet_addr(ip_addr);
    }
    else
    {
     p_socket->socket_addr.sin_addr.s_addr = INADDR_ANY; 
    }
    p_socket->socket_addr.sin_port = htons(port);
#endif

  p_socket->local_ip.port = port;

  // Assign an address to the server socket
  ret = bind(p_socket->h_socket, (SOCKADDR*)&p_socket->socket_addr, sizeof(p_socket->socket_addr));
  if (ret != 0)
  {
    EZ_LOG_TRACE_ERROR(ret);
    return (p_socket->err_code = EZ_ERR_SOCKET_BIND);
  }

  return EZ_OK;
}

ez_err_t socketListen(ez_socket_t *p_socket, int baklog)
{
  ez_err_t err_ret;
  int ret;


  logDebug("socketListen() : baklog %d\n", baklog);

  err_ret = socketIsValid(p_socket);
  if (err_ret != EZ_OK)
  {
    EZ_LOG_TRACE_ERROR(err_ret);
    return err_ret;
  }

  // Waiting for incoming requests on server sockets
  ret = listen(p_socket->h_socket, baklog);
  if (ret != 0)
  {
    EZ_LOG_TRACE_ERROR(ret);
    return (p_socket->err_code = EZ_ERR_SOCKET_LISTEN);
  }

  return EZ_OK;
}

ez_err_t socketAccept(ez_socket_t *p_socket)
{
  ez_err_t err_ret;


  logDebug("socketAccept()\n");

  err_ret = socketIsValid(p_socket);
  if (err_ret != EZ_OK)
  {
    EZ_LOG_TRACE_ERROR(err_ret);
    return err_ret;
  }


  // Client address structure size variable definition
#ifdef _WIN32
  int size_clintAddr = sizeof(p_socket->client_addr);
#else
  socklen_t size_clintAddr = sizeof(p_socket->client_addr);
#endif

  // Assigning an approved client address to a client socket handle
  p_socket->h_client = accept(p_socket->h_socket, (SOCKADDR*)&p_socket->client_addr, &size_clintAddr);
  if (p_socket->h_client <= 0)
  {
    EZ_LOG_TRACE_ERROR(p_socket->h_client);
    p_socket->is_connected = false;
    return (p_socket->err_code = EZ_ERR_SOCKET_ACCEPT);
  }

  p_socket->remote_ip.port = ntohs(p_socket->client_addr.sin_port);
  strncpy(p_socket->remote_ip.ip_addr, inet_ntoa(p_socket->client_addr.sin_addr), 32);
  p_socket->is_remote_ip = true;

  p_socket->is_connected = true;

  return EZ_OK;
}

ez_err_t socketConnect(ez_socket_t *p_socket, const char *ip_addr, uint32_t port)
{
  ez_err_t err_ret;
  int ret;


  logDebug("socketConnect() : ip %s, port %d\n", ip_addr, port);

  err_ret = socketIsValid(p_socket);
  if (err_ret != EZ_OK)
  {
    EZ_LOG_TRACE_ERROR(err_ret);
    return err_ret;
  }


  // Define an address structure to put on the target server socket
#ifdef _WIN32  
  memset(&p_socket->socket_addr, 0, sizeof(p_socket->socket_addr));
  p_socket->socket_addr.sin_family = AF_INET;
  p_socket->socket_addr.sin_addr.S_un.S_addr = inet_addr(ip_addr);  
  //inet_pton(AF_INET, ip_addr, &p_socket->socket_addr.sin_addr);
  //strcpy_s(p_socket->ip_addr, ip_addr);
  p_socket->socket_addr.sin_port = htons(port);
#else
  memset(&p_socket->socket_addr, 0, sizeof(p_socket->socket_addr));
  p_socket->socket_addr.sin_family = AF_INET;
  p_socket->socket_addr.sin_addr.s_addr=inet_addr(ip_addr);
  p_socket->socket_addr.sin_port = htons(port);
#endif  

  // Assign an address to the target server socket
  ret = connect(p_socket->h_socket, (SOCKADDR*)&p_socket->socket_addr, sizeof(p_socket->socket_addr));
  if (ret != 0)
  {
    p_socket->is_connected = false;
    return (p_socket->err_code = EZ_ERR_SOCKET_CONNECT);
  }

  p_socket->remote_ip.port = ntohs(p_socket->socket_addr.sin_port);
  strncpy(p_socket->remote_ip.ip_addr, inet_ntoa(p_socket->socket_addr.sin_addr), 32);
  p_socket->is_remote_ip = true;

  p_socket->is_connected = true;

  return EZ_OK;
}

bool socketIsConnected(ez_socket_t *p_socket)
{
  return p_socket->is_connected;
}

int socketWrite(ez_socket_t *p_socket, const uint8_t *p_data, uint32_t length)
{
  ez_err_t err_ret;
  int ret;
  SOCKET h_socket;


  err_ret = socketIsValid(p_socket);
  if (err_ret != EZ_OK)
  {
    EZ_LOG_TRACE_ERROR(err_ret);
    return EZ_FATAL;
  }

  if (p_socket->role == EZ_SOCKET_SERVER && p_socket->protocol == EZ_SOCKET_TCP)
  {
    h_socket = p_socket->h_client;
  }
  else
  {
    h_socket = p_socket->h_socket;
  }

  // Send messages to assigned clients.
#ifdef _WIN32  
  if (p_socket->protocol == EZ_SOCKET_TCP || p_socket->is_connected == true)
  {
    ret = send(h_socket, (const char *)p_data, length, 0);
  }
  else
  {
    if (p_socket->is_remote_ip == true || p_socket->is_broadcast == true)
    {
      ret = sendto(h_socket, (const char *)p_data, length, 0, 
                  (SOCKADDR*)&p_socket->remote_ip.socket_addr, sizeof(p_socket->remote_ip.socket_addr));
    }
    else
    {
      ret = -1;
    }
  }
#else
  if (p_socket->protocol == EZ_SOCKET_TCP || p_socket->is_connected == true)
  {
    //ret = write(h_socket, (const char *)p_data, length);
    ret = send(h_socket, (const char *)p_data, length, MSG_NOSIGNAL);
  }
  else
  {
    if (p_socket->is_remote_ip == true)
    {
      ret = sendto(h_socket, (const char *)p_data, length, 0, 
                  (struct sockaddr *)&p_socket->remote_ip.socket_addr, sizeof(p_socket->remote_ip.socket_addr));
    }
    else
    {
      ret = -2;
    }
  }
#endif  
  if (ret <= 0)
  {
    EZ_LOG_TRACE_ERROR(ret);
    p_socket->err_code = EZ_ERR_SOCKET_WRITE;
  }
  return ret;
}

int socketRead(ez_socket_t *p_socket, uint8_t *p_data, uint32_t length)
{
  ez_err_t err_ret;
  int ret;
  SOCKET h_socket;


  err_ret = socketIsValid(p_socket);
  if (err_ret != EZ_OK)
  {
    EZ_LOG_TRACE_ERROR(err_ret);
    return EZ_FATAL;
  }

  if (p_socket->role == EZ_SOCKET_SERVER && p_socket->protocol == EZ_SOCKET_TCP)
  {
    h_socket = p_socket->h_client;
  }
  else
  {
    h_socket = p_socket->h_socket;
  }

  // Send messages to assigned clients.
#ifdef _WIN32
  if (p_socket->protocol == EZ_SOCKET_TCP || p_socket->is_connected == true)
  {  
    ret = recv(h_socket, (char *)p_data, length, 0);
  }
  else
  {
    SOCKADDR_IN ip_addr;
    int ip_addr_size;

    ip_addr_size = sizeof(ip_addr);
    ret = recvfrom(h_socket, (char *)p_data, length, 0, (SOCKADDR*)&ip_addr, &ip_addr_size);
    if (ret >= 0)
    {
      p_socket->remote_ip.port = ntohs(ip_addr.sin_port);
      strcpy_s(p_socket->remote_ip.ip_addr, inet_ntoa(ip_addr.sin_addr));
      p_socket->remote_ip.socket_addr = ip_addr;

      p_socket->is_remote_ip = true;
    }
    else
    {
      p_socket->is_remote_ip = false;
    }
  }
#else
  if (p_socket->protocol == EZ_SOCKET_TCP || p_socket->is_connected == true)
  {
    ret = read(h_socket, (char *)p_data, length);
  }
  else
  {
    struct sockaddr_in ip_addr;
    socklen_t ip_addr_size;

    ip_addr_size = sizeof(ip_addr);
    ret = recvfrom(h_socket, (void *)p_data, (size_t)length, 0, (struct sockaddr *)&ip_addr, &ip_addr_size);
    if (ret >= 0)
    {
      p_socket->remote_ip.port = ntohs(ip_addr.sin_port);
      strncpy(p_socket->remote_ip.ip_addr, inet_ntoa(ip_addr.sin_addr), 32);
      p_socket->remote_ip.socket_addr = ip_addr;

      p_socket->is_remote_ip = true;
    }
    else
    {
      p_socket->is_remote_ip = false;
    }    
  }
#endif  
  if (ret <= 0)
  {
    
    if (p_socket->is_connected == true)
    {
      EZ_LOG_TRACE_ERROR(ret);
    }
    p_socket->err_code = EZ_ERR_SOCKET_READ;
  }
  return ret;
}

ez_err_t socketClose(ez_socket_t *p_socket)
{
  ez_err_t err_ret = EZ_OK;
  int ret;


  if ((int)p_socket->h_client > 0)
  {
    logDebug("socketClose() %d\n", (int)p_socket->h_client);

#ifdef _WIN32    
    ret = closesocket(p_socket->h_client);
#else
    ret = close(p_socket->h_client);
#endif        
    if (ret != 0)
    {
      EZ_LOG_TRACE_ERROR(ret);
      err_ret = EZ_ERR_SOCKET_CLOSE;
      p_socket->err_code = err_ret;
    }
  }

  p_socket->h_client = -1;
  p_socket->is_connected = false;

  return err_ret;
}

ez_err_t socketDestroy(ez_socket_t *p_socket)
{
  ez_err_t err_ret = EZ_OK;
  int ret;


  if (p_socket->is_created != true)
  {
    return EZ_OK;
  }

  logDebug("socketDestroy() \n");

  socketClose(p_socket);

  if ((int)p_socket->h_socket > 0)
  {
#ifdef _WIN32    
    ret = closesocket(p_socket->h_socket);
#else
    ret = close(p_socket->h_socket);
#endif    
    if (ret != 0)
    {
      EZ_LOG_TRACE_ERROR(ret);
      err_ret = EZ_ERR_SOCKET_CLOSE;
      p_socket->err_code = err_ret;
    }
  }
  p_socket->h_socket = -1;
  p_socket->is_connected = false;
  p_socket->is_created = false;

  return err_ret;
}

int socketReadForLength(ez_socket_t *p_socket, uint8_t *p_data, uint32_t length)
{
  int ret = 0;
  uint32_t rx_len;

  rx_len = 0;
  while(rx_len < length)
  {
    ret = socketRead(p_socket, &p_data[rx_len], length-rx_len);
    if (ret > 0)
    {
      rx_len += ret;
    }
    else
    {
      break;
    }
  }

  return ret;
}

bool socketIsRemtoeIP(ez_socket_t *p_socket)
{
  return p_socket->is_remote_ip;
}

ez_err_t socketGetRemoteIP(ez_socket_t *p_socket, ez_ip_addr_t *p_ip_addr)
{
  ez_err_t err_ret = EZ_ERR;


  if (p_socket->is_remote_ip == true)
  {
    *p_ip_addr = p_socket->remote_ip;
    err_ret = EZ_OK;
  }

  return err_ret;
}

ez_err_t socketSetRemoteIP(ez_socket_t *p_socket, const char *ip_addr, uint32_t port)
{

#ifdef _WIN32  
  memset(&p_socket->remote_ip.socket_addr, 0, sizeof(p_socket->remote_ip.socket_addr));
  p_socket->remote_ip.socket_addr.sin_family = AF_INET;
  p_socket->remote_ip.socket_addr.sin_addr.S_un.S_addr = inet_addr(ip_addr);  
  p_socket->remote_ip.socket_addr.sin_port = htons(port);
#else
  memset(&p_socket->remote_ip.socket_addr, 0, sizeof(p_socket->remote_ip.socket_addr));
  p_socket->remote_ip.socket_addr.sin_family = AF_INET;
  p_socket->remote_ip.socket_addr.sin_addr.s_addr=inet_addr(ip_addr);
  p_socket->remote_ip.socket_addr.sin_port = htons(port);
#endif

  strncpy(p_socket->remote_ip.ip_addr, ip_addr, 32);
  p_socket->remote_ip.port = port;
  p_socket->is_remote_ip = true;

  return EZ_OK;
}

}
