/*
 *  luasocket.c
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#include <Nova.h>
#include "lua_script.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif

//////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////

#define SLEEPSEC 1

//////////////////////////////////////////////////////////////////////////

static VOID string_to_sockaddr(const CHAR* ip, struct sockaddr_in* sadr);
static const CHAR* sockaddr_to_string(const struct sockaddr_in* sadr);
static const CHAR* get_socket_error_string();

//////////////////////////////////////////////////////////////////////////

static I32	__tcp_open(lua_State* ls);
static I32	__tcp_connect(lua_State* ls);
static I32	__tcp_accept(lua_State* ls);
static I32	__tcp_close(lua_State* ls);
static I32	__tcp_send(lua_State* ls);
static I32	__tcp_recv(lua_State* ls);

// socket object methods
static luaL_reg __tcp_funcs[] = {
  {"open",    __tcp_open},
  {"connect", __tcp_connect},
  {"accept",  __tcp_accept},
  {"close",   __tcp_close},  
  {"send",    __tcp_send},
  {"recv",    __tcp_recv},
  {NULL,      NULL}
};

//////////////////////////////////////////////////////////////////////////

typedef enum _SocketType
{
  SOCKET_TYPE_TCP,
  SOCKET_TYPE_UDP,
}SocketType;

typedef struct _SocketNode
{
  struct _SocketNode* prev;
  struct _SocketNode* next;
  U32 type;
  U32 socket;
  I32	table_ref;
}SocketNode;

static SocketNode* s_socket_list_head = NULL;
static SocketNode* s_socket_list_tail = NULL;

VOID __add_socket_node(SocketNode* node)
{
  if (s_socket_list_head == NULL)
  {
    s_socket_list_head = node;
  }
  else
  {
    s_socket_list_tail->next = node;
    node->prev = s_socket_list_tail;
  }
  s_socket_list_tail = node;
}

VOID __del_socket_node(SocketNode* node)
{
  if (node == NULL) return;
  if (s_socket_list_tail == node)
  {
    s_socket_list_tail = node->prev;
  }

  if (s_socket_list_head == node)
  {
    s_socket_list_head = node->next;

    if (node->next != NULL)
    {
      node->next->prev = NULL;
    }
  }
  else
  {
    if (node->prev != NULL)
    {
      node->prev->next = node->next;
    } 

    if (node->next != NULL)
    {
      node->next->prev = node->prev;
    }
  }
}

//////////////////////////////////////////////////////////////////////////

/*
====================
lua_socket_init
====================
*/
VOID lua_socket_init(lua_State* ls)
{
  WSADATA	wsa_data;

#ifdef _WIN32
  // init window socket	
  CHECK(WSAStartup(MAKEWORD(1, 1), &wsa_data) == 0);
#endif

  // register tcp function
  luaL_register(ls, "tcp", __tcp_funcs);
}

/*
====================
lua_socket_exit
====================
*/
VOID lua_socket_exit(lua_State* ls)
{
  SocketNode* node = NULL;
  LUA_STACK;
  LUA_WATCH(ls);
  
  node = s_socket_list_head;
  while (node)
  {
    SocketNode* curr_node = node;
    node = node->next;

    // unregister the table
    luaL_unref(ls, LUA_REGISTRYINDEX, curr_node->table_ref);
    LUA_WATCH(ls);

    // close the socket
    CHECK(curr_node->socket != INVALID_SOCKET);
    closesocket(curr_node->socket);
    __del_socket_node(curr_node);
  }
  s_socket_list_head = NULL;
  s_socket_list_tail = NULL;
}

/*
====================
lua_socket_event
====================
*/
BOOL lua_socket_event(lua_State* ls, const CHAR* func_name, SocketNode* node) 
{
  I32 argc = 0;
  I32 ret_value = 0;
  LUA_STACK;
  LUA_WATCH(ls);

  // check the script call function
  lua_pushstring(ls, func_name);
  LUA_WATCH(ls);
  lua_gettable(ls, LUA_GLOBALSINDEX);
  LUA_WATCH(ls);
  if(!lua_isfunction(ls, -1))
  {
    lua_pushstring(ls, VA("lua_socket_event : can`t find the %s function.\n", func_name));
    __lua_error(ls);
    return FALSE;
  }

  // get the socket node
  lua_rawgeti(ls, LUA_REGISTRYINDEX, node->table_ref);
  LUA_WATCH(ls);
  argc += 1;

  // call the function
  if(lua_pcall(ls, argc, 1, 0) != 0)
  {
    LUA_WATCH(ls);
    lua_pushstring(ls, VA("lua_socket_event : failed to call the %s function.\n", func_name));
    __lua_error(ls);
    return FALSE;
  }
  LUA_WATCH(ls);

  // get the return value
  if(lua_isboolean(ls, -1))
  {
    ret_value = lua_toboolean(ls, -1);
  }
  lua_pop(ls, 1);

  return ret_value != 0 ? TRUE : FALSE;
}

/*
====================
lua_socket_select
====================
*/
BOOL lua_socket_select(lua_State* ls)
{
  fd_set read_set;
  fd_set write_set;
  fd_set exception_set;
  I32 i = 0;
  SocketNode* node = NULL;
  U32 max_fd = 0;
  struct timeval tv;
  I32 rv;

  FD_ZERO(&read_set);
  FD_ZERO(&write_set);
  FD_ZERO(&exception_set);

  // set the fds
  node = s_socket_list_head;
  while (node)
  {
    SocketNode* curr_node = node;
    node = node->next;
    if(curr_node->socket == INVALID_SOCKET) continue;
    max_fd = (curr_node->socket >= max_fd) ? (curr_node->socket + 1) : max_fd;
    FD_SET(curr_node->socket, &read_set);
    FD_SET(curr_node->socket, &write_set);
  }

  // set the timeout
  tv.tv_sec = SLEEPSEC;
  tv.tv_usec = 0;

  // select all of the sockets
  rv = select(max_fd, &read_set, &write_set, &exception_set, &tv);
  if(rv < 0)
  {
    // some error occured.
    NPrint(VA("FATAL: select() returned %d", rv));
    return FALSE;
  }
  else if(rv == 0)
  {
    // no events
    return TRUE;
  }
  else
  {
    // loop each each socket and handle events on each of them
    node = s_socket_list_head;
    while (node)
    {
      SocketNode* curr_node = node;
      node = node->next;
      if(curr_node->socket == INVALID_SOCKET) continue;

      if(FD_ISSET(curr_node->socket, &exception_set))
      {
        if(!lua_socket_event(ls, "socket_event_error", curr_node))
        {
          continue;
        }
      }
      else
      {
        if(FD_ISSET(curr_node->socket, &read_set))
        {
          if(!lua_socket_event(ls, "socket_event_write", curr_node))
          {
            continue;
          }
        }

        if(FD_ISSET(curr_node->socket, &write_set))
        {
          if(!lua_socket_event(ls, "socket_event_read", curr_node))
          {
            continue;
          }
        }
      }
    }
  }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////////
/*
====================
__tcp_open
====================
*/
I32	__tcp_open(lua_State* ls)
{
  U16 port = 0;
  U32 fd = INVALID_SOCKET;
  I32	opt = 0;
  I32 ret = -1;
  struct sockaddr_in sock_addr;
  SocketNode* node = NULL;

  LUA_STACK;
  LUA_WATCH(ls);

  // check the port
  if(lua_gettop(ls) != 1 || lua_type(ls, 1) != LUA_TNUMBER)
  {
    NAssert("__tcp_open : Invalid port!");
    return 0;
  }
  port = (U16)lua_tonumber(ls, 1);

  // create the host socket.	
  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(fd == INVALID_SOCKET)
  {
    NAssert(VA("__tcp_open: There was an error creating the host socket: %s\n", get_socket_error_string()));
    return 0;
  }

  // make it non-blocking
  opt = 1;
  ret = ioctlsocket(fd, FIONBIO, (U32*)&opt);
  if(ret < 0)
  {
    NAssert(VA("__tcp_open: There was an error making a socket non-blocking : %s\n", get_socket_error_string()));
    return 0;
  }

  // bind the socket.
  memset(&sock_addr, 0, sizeof(sock_addr));
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = htons(port);
  sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  ret = bind(fd, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr_in));
  if(ret < 0)
  {
    NAssert(VA("__tcp_open: There was an error binding a socket : %s\n", get_socket_error_string()));
    return 0;
  }

  // start listening on the socket.
  ret = listen(fd, 5);
  if(ret < 0)
  {
    NAssert(VA("__tcp_open: There was an error listening on a socket : %s\n", get_socket_error_string()));		
    return 0;
  }

  // create the socket node
  node = (SocketNode*)(lua_newuserdata(ls, sizeof(SocketNode)));
  LUA_WATCH(ls);
  node->prev = NULL;
  node->next = NULL;
  node->type = SOCKET_TYPE_TCP;
  node->socket = fd;
  lua_pushvalue(ls, -1);
  LUA_WATCH(ls);
  node->table_ref = luaL_ref(ls, LUA_REGISTRYINDEX);
  LUA_WATCH(ls);
  __add_socket_node(node);
  return 1;
}

/*
====================
__tcp_connect
====================
*/
I32	__tcp_connect(lua_State* ls)
{
  U32 fd = INVALID_SOCKET;
  I32	opt = 0;
  I32 ret = -1;
  const CHAR* ip;
  struct sockaddr_in sock_addr;
  SocketNode* node = NULL;

  LUA_STACK;
  LUA_WATCH(ls);

  // check the port
  if(lua_gettop(ls) != 1 || lua_type(ls, 1) != LUA_TSTRING)
  {
    NAssert("__tcp_connect : Invalid ip!");
    return 0;
  }
  ip = lua_tostring(ls, 1);

  // make socket address
  string_to_sockaddr(ip, &sock_addr);

  // create the host socket.	
  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(fd == INVALID_SOCKET)
  {
    NAssert(VA("__tcp_connect: There was an error creating the host socket: %s\n", get_socket_error_string()));	
    return 0;
  }

  // make it non-blocking
  opt = 1;
  ret = ioctlsocket(fd, FIONBIO, (U32*)&opt);
  if(ret < 0)
  {
    NAssert(VA("__tcp_connect: There was an error making a socket non-blocking : %s\n", get_socket_error_string()));
    return -1;
  }

  // connect the socket.
  ret = connect(fd, (struct sockaddr *)&sock_addr, sizeof(struct sockaddr_in));
  if(ret < 0)
  {
    NAssert(VA("__tcp_connect: There was an error connecting a socket : %s\n", get_socket_error_string()));
    return -1;
  }

  // create the socket node
  node = (SocketNode*)(lua_newuserdata(ls, sizeof(SocketNode)));
  LUA_WATCH(ls);
  node->prev = NULL;
  node->next = NULL;
  node->type = SOCKET_TYPE_TCP;
  node->socket = fd;
  lua_pushvalue(ls, -1);
  LUA_WATCH(ls);
  node->table_ref = luaL_ref(ls, LUA_REGISTRYINDEX);
  LUA_WATCH(ls);
  __add_socket_node(node);
  return 1;
}

/*
====================
__tcp_close
====================
*/
I32	__tcp_close(lua_State* ls)
{
  SocketNode* node = NULL;
  LUA_STACK;
  LUA_WATCH(ls);

  // check the socket
  if(lua_gettop(ls) != 1 || lua_type(ls, 1) != LUA_TUSERDATA)
  {
    NAssert("__tcp_close : invalid socket!");
    return 0;
  }
  node = (SocketNode*)lua_touserdata(ls, 1);
  CHECK(node->socket != INVALID_SOCKET);

  // unregister the table
  luaL_unref(ls, LUA_REGISTRYINDEX, node->table_ref);
  LUA_WATCH(ls);

  // close the socket
  closesocket(node->socket);
  __del_socket_node(node);

  return 0;
}

/*
====================
__tcp_accept
====================
*/
I32	__tcp_accept(lua_State* ls)
{
  SocketNode* listen_node = NULL;  
  U32 fd = INVALID_SOCKET;
  I32	opt = 0;
  I32 ret = -1;
  struct sockaddr_in sock_addr;
  I32 addr_len = sizeof(sock_addr);
  SocketNode* node = NULL;
  const CHAR* ip;

  LUA_STACK;
  LUA_WATCH(ls);

  // check the socket
  if(lua_gettop(ls) != 1 || lua_type(ls, 1) != LUA_TUSERDATA)
  {
    NAssert("__tcp_accept : invalid socket!");
    return 0;
  }
  listen_node = (SocketNode*)lua_touserdata(ls, 1);
  CHECK(listen_node->socket != INVALID_SOCKET);

  // accept a new connection
  memset(&sock_addr, 0, sizeof(sock_addr));	
  fd = accept(listen_node->socket, (struct sockaddr*)&sock_addr, &addr_len);
  if(fd != INVALID_SOCKET)
  {
    NAssert(VA("__tcp_accept: There was an error accept a socket : %s\n", get_socket_error_string()));
    return 0;
  }

  // make it non-blocking
  opt = 1;
  ret = ioctlsocket(fd, FIONBIO, (U32*)&opt);
  if(ret < 0)
  {
    NAssert(VA("__tcp_accept: There was an error making a socket non-blocking : %s\n", get_socket_error_string()));
    return 0;
  }
  
  // create the socket node
  node = (SocketNode*)(lua_newuserdata(ls, sizeof(SocketNode)));
  LUA_WATCH(ls);
  node->prev = NULL;
  node->next = NULL;
  node->type = SOCKET_TYPE_TCP;
  node->socket = fd;
  lua_pushvalue(ls, -1);
  LUA_WATCH(ls);
  node->table_ref = luaL_ref(ls, LUA_REGISTRYINDEX);
  LUA_WATCH(ls);
  __add_socket_node(node);

  // get the netaddr
  ip = sockaddr_to_string(&sock_addr);
  lua_pushstring(ls, ip);
  LUA_WATCH(ls);
  return 2;
}

/*
====================
__tcp_send
====================
*/
I32	__tcp_send(lua_State* ls)
{
  SocketNode* node = NULL;
  I32 ret = -1;
  I32 total = 0;
  const CHAR* buf;
  I32 len;

  LUA_STACK;
  LUA_WATCH(ls);

  // check the socket
  if(lua_gettop(ls) != 2 || lua_type(ls, 1) != LUA_TUSERDATA || lua_type(ls, 2) != LUA_TSTRING)
  {
    NAssert("__tcp_send : invalid socket!");
    return 0;
  }
  node = (SocketNode*)lua_touserdata(ls, 1);
  CHECK(node->socket != INVALID_SOCKET);
  buf = luaL_checklstring(ls, 2, &len);

  do
  {
    ret = send(node->socket, (const CHAR*)&buf[total], (len-total), 0);
    if(ret == SOCKET_ERROR) 
    {
      if(WSAGetLastError() != WSAEWOULDBLOCK)
      {
        total = -1;
        NAssert("__tcp_send: There was an error sending on a socket.\n");
        break;
      }
    } 
    else if(ret == 0) 
    {
      total = 0;
      NAssert("__tcp_send: connection closed.\n");
      break;
    }
    else
    {
      total += ret;
      if(total >= len)
      {
        break;
      }
    }
  }
  while(ret != SOCKET_ERROR);

  // return the result
  lua_pushinteger(ls, total);
  LUA_WATCH(ls);
  return 1;
}

/*
====================
__tcp_recv
====================
*/
I32	__tcp_recv(lua_State* ls)
{
  SocketNode* node = NULL;
  I32 ret;
  I32 total = 0;
  CHAR buf[MAX_DATA];

  LUA_STACK;
  LUA_WATCH(ls);

  // check the socket
  if(lua_gettop(ls) != 1 || lua_type(ls, 1) != LUA_TUSERDATA)
  {
    NAssert("__tcp_recv : Invalid socket!");
    return 0;
  }
  node = (SocketNode*)lua_touserdata(ls, 1);
  CHECK(node->socket != INVALID_SOCKET);

  do
  {
    // read from the network.
    ret = recv(node->socket, (char*)&buf[total], (MAX_DATA-total), 0);
    if(ret == SOCKET_ERROR)
    {
      if(WSAGetLastError() != WSAEWOULDBLOCK)
      {
        total = -1;
        NAssert(VA("__tcp_recv: There was an error reading from a socket.\n"));
        break;
      }
    }
    else if(ret == 0)
    {
      // connection closed.
      total = 0;
      NAssert("__tcp_recv: connection closed.\n");
      break;
    }
    else
    {
      // update the buffer len.
      total += ret;
      if(total >= MAX_DATA)
      {
        break;
      }
    }
  }
  while(ret != SOCKET_ERROR);

  // return the result
  lua_pushinteger(ls, total);
  LUA_WATCH(ls);
  if (total > 0) 
  {
    lua_pushstring(ls, buf);
    LUA_WATCH(ls);
  } 
  else
  {
    lua_pushnil(ls);
    LUA_WATCH(ls);
  }
  return 2;
}

//////////////////////////////////////////////////////////////////////////

/*
====================
string_to_sockaddr
====================
*/
VOID string_to_sockaddr(const CHAR* ip, struct sockaddr_in* sadr)
{
  CHAR buffer[MAX_NAME];
  CHAR* port;
  struct hostent* host;

  strncpy(buffer, ip, sizeof(buffer));
  port = strstr(buffer, ":" );
  if (port) 
  {
    *port = 0;
    port++;
  }

  memset(sadr, 0, sizeof( *sadr));
  sadr->sin_family = AF_INET;
  sadr->sin_port = (U16)atoi(port);

  if(buffer[0] >= '0' && buffer[0] <= '9') 
  {
    *(I32*)&sadr->sin_addr = inet_addr(buffer);
  } 
  else 
  {
    host = gethostbyname(buffer);
    if(host == NULL) 
    {
      NAssert(VA("StringToSockaddr: the ip(%s) is error.\n", ip));
      return;
    }
    *(I32*)&sadr->sin_addr = *(I32*)host->h_addr_list[0];
  }
}

/*
====================
sockaddr_to_string
====================
*/
const CHAR* sockaddr_to_string(const struct sockaddr_in* sadr)
{
  static CHAR buffer[MAX_NAME];
  U8 ip[4]; 
  U16	port;
  *(U32*)ip = sadr->sin_addr.s_addr;
  port = sadr->sin_port;
  sprintf(buffer, sizeof(buffer), "%i.%i.%i.%i:%u", ip[0], ip[1], ip[2], ip[3], port);
  return buffer;
}

//////////////////////////////////////////////////////////////////////////

/*
====================
get_socket_error_string
====================
*/
const CHAR* get_socket_error_string()
{
  I32 error = WSAGetLastError();
  switch(error)
  {
  case WSAEINTR:				return "WSAEINTR";
  case WSAEBADF:				return "WSAEBADF";
  case WSAEACCES:				return "WSAEACCES";
  case WSAEDISCON:			return "WSAEDISCON";
  case WSAEFAULT:				return "WSAEFAULT";
  case WSAEINVAL:				return "WSAEINVAL";
  case WSAEMFILE:				return "WSAEMFILE";
  case WSAEWOULDBLOCK:		return "WSAEWOULDBLOCK";
  case WSAEINPROGRESS:		return "WSAEINPROGRESS";
  case WSAEALREADY:			return "WSAEALREADY";
  case WSAENOTSOCK:			return "WSAENOTSOCK";
  case WSAEDESTADDRREQ:		return "WSAEDESTADDRREQ";
  case WSAEMSGSIZE:			return "WSAEMSGSIZE";
  case WSAEPROTOTYPE:			return "WSAEPROTOTYPE";
  case WSAENOPROTOOPT:		return "WSAENOPROTOOPT";
  case WSAEPROTONOSUPPORT:	return "WSAEPROTONOSUPPORT";
  case WSAESOCKTNOSUPPORT:	return "WSAESOCKTNOSUPPORT";
  case WSAEOPNOTSUPP:			return "WSAEOPNOTSUPP";
  case WSAEPFNOSUPPORT:		return "WSAEPFNOSUPPORT";
  case WSAEAFNOSUPPORT:		return "WSAEAFNOSUPPORT";
  case WSAEADDRINUSE:			return "WSAEADDRINUSE";
  case WSAEADDRNOTAVAIL:		return "WSAEADDRNOTAVAIL";
  case WSAENETDOWN:			return "WSAENETDOWN";
  case WSAENETUNREACH:		return "WSAENETUNREACH";
  case WSAENETRESET:			return "WSAENETRESET";
  case WSAECONNABORTED:		return "WSWSAECONNABORTEDAEINTR";
  case WSAECONNRESET:			return "WSAECONNRESET";
  case WSAENOBUFS:			return "WSAENOBUFS";
  case WSAEISCONN:			return "WSAEISCONN";
  case WSAENOTCONN:			return "WSAENOTCONN";
  case WSAESHUTDOWN:			return "WSAESHUTDOWN";
  case WSAETOOMANYREFS:		return "WSAETOOMANYREFS";
  case WSAETIMEDOUT:			return "WSAETIMEDOUT";
  case WSAECONNREFUSED:		return "WSAECONNREFUSED";
  case WSAELOOP:				return "WSAELOOP";
  case WSAENAMETOOLONG:		return "WSAENAMETOOLONG";
  case WSAEHOSTDOWN:			return "WSAEHOSTDOWN";
  case WSASYSNOTREADY:		return "WSASYSNOTREADY";
  case WSAVERNOTSUPPORTED:	return "WSAVERNOTSUPPORTED";
  case WSANOTINITIALISED:		return "WSANOTINITIALISED";
  case WSAHOST_NOT_FOUND:		return "WSAHOST_NOT_FOUND";
  case WSATRY_AGAIN:			return "WSATRY_AGAIN";
  case WSANO_RECOVERY:		return "WSANO_RECOVERY";
  case WSANO_DATA:			return "WSANO_DATA";
  default:					return "NO ERROR";
  }
}


//////////////////////////////////////////////////////////////////////////