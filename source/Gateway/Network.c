/*
 *  Network.c
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#include "Local.h"

//////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////

#define MAX_NUM_SOCKET  FD_SETSIZE
static NetSocket s_sockets[MAX_NUM_SOCKET];
static I32 s_max_socket = 0;
static U16 s_mark_counter = 0;

//////////////////////////////////////////////////////////////////////////
/*
====================
NetworkOpen
====================
*/
VOID NetworkInit()
{
  I32 i = 0;
  for(i = 0; i < MAX_NUM_SOCKET; i++)
  {
    NetSocket* net_socket = &s_sockets[i];
    net_socket->state = NS_UNUSED;
    net_socket->socket = INVALID_SOCKET;
    net_socket->mark = 0;
    net_socket->error_func = NULL;
    net_socket->write_func = NULL;
    net_socket->read_func = NULL;
    net_socket->write_size = 0;
    net_socket->read_size = 0;
  }
}

/*
====================
NetworkExit
====================
*/
VOID NetworkExit()
{
  I32 i = 0;
  for(i = 0; i < MAX_NUM_SOCKET; i++)
  {
    NetSocket* net_socket = &s_sockets[i];
    net_socket->state = NS_UNUSED;
    if(net_socket->socket != INVALID_SOCKET)
    {
      NTCPClose(net_socket->socket);
      net_socket->socket = INVALID_SOCKET;
    }
    net_socket->mark = 0;
    net_socket->error_func = NULL;
    net_socket->write_func = NULL;
    net_socket->read_func = NULL;
    net_socket->write_size = 0;
    net_socket->read_size = 0;
  }
}

/*
====================
NetworkOpen
====================
*/
I32 NetworkOpen(U32 socket)
{  
  I32 id = -1;
  if(s_sockets[socket%MAX_NUM_SOCKET].socket == INVALID_SOCKET)
  {
    id = socket%MAX_NUM_SOCKET;
  }
  else
  {
    U32 i = 0;
    for(i = 0; i < MAX_NUM_SOCKET; i++)
    {
      if(s_sockets[i].socket == INVALID_SOCKET)
      {        
        id = i;
        break;
      }
    }
  }

  if(id >= 0)
  {
    NetSocket* net_socket = NULL;
    net_socket = &s_sockets[id];
    net_socket->state = NS_UNUSED;
    net_socket->socket = socket;
    net_socket->mark++;
    net_socket->error_func = NULL;
    net_socket->write_func = NULL;
    net_socket->read_func = NULL;
    net_socket->write_size = 0;
    net_socket->read_size = 0;
    s_max_socket = ((s_max_socket <= id) ? (id + 1) : s_max_socket);
  }

  return id;
}

/*
====================
NetworkClose
====================
*/
VOID NetworkClose(I32 id)
{
  NetSocket* net_socket = NULL;
  CHECK(id >= 0);
  net_socket = &s_sockets[id];
  net_socket->state = NS_UNUSED;
  CHECK(net_socket->socket!=INVALID_SOCKET);
  NTCPClose(net_socket->socket);
  net_socket->socket = INVALID_SOCKET;
  net_socket->mark++;
  net_socket->error_func = NULL;
  net_socket->write_func = NULL;
  net_socket->read_func = NULL;
  net_socket->write_size = 0;
  net_socket->read_size = 0;
}

/*
====================
NetworkFind
====================
*/
NetSocket* NetworkFind(I32 id)
{
  if(id >= 0 && id < MAX_NUM_SOCKET)
  {
    return &s_sockets[id];
  }
  return NULL;
}

/*
====================
NetworkPoll
====================
*/
BOOL NetworkPoll()
{
  fd_set read_set;
  fd_set write_set;
  fd_set exception_set;
  I32 i = 0;
  NetSocket* net_socket = NULL;
  U32 max_fd = 0;
  struct timeval tv;
  I32 rv;

  FD_ZERO(&read_set);
  FD_ZERO(&write_set);
  FD_ZERO(&exception_set);

  // set the fds
  for(i = 0; i < s_max_socket; i++ )
  {
    net_socket = &s_sockets[i];
    if(net_socket->socket == INVALID_SOCKET) continue;
    max_fd = (net_socket->socket >= max_fd) ? (net_socket->socket + 1) : max_fd;
    FD_SET(net_socket->socket, &read_set);
    if(net_socket->write_size > 0)
    {
      FD_SET(net_socket->socket, &write_set);
    }
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
    for(i = 0; i < s_max_socket; i++)
    {
      net_socket = &s_sockets[i];
      if(net_socket->socket == INVALID_SOCKET) continue;
      if(FD_ISSET(net_socket->socket, &exception_set))
      {
        if(!net_socket->error_func(i))
        {
          continue;
        }
      }
      else
      {
        if(FD_ISSET(net_socket->socket, &read_set))
        {
          if(!net_socket->read_func(i))
          {
            continue;
          }
        }

        if(FD_ISSET(net_socket->socket, &write_set))
        {
          if(!net_socket->write_func(i))
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