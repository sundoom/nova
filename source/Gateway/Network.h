/*
 *  Network.h
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#ifndef __Network__
#define __Network__

//////////////////////////////////////////////////////////////////////////

#define SOCKET_WRITE_BUFFER_SIZE   0X20000   // 128*1024
#define SOCKET_READ_BUFFER_SIZE    0X400     // 1024

typedef BOOL (*NET_FUNC)(I32);

typedef enum _NetState
{
  NS_UNUSED,
  NS_OPENED,
  NS_CLOSED  
}NetState;

typedef struct _NetSocket
{
  U8  state;
  U32 socket;
  U8  mark;
  NET_FUNC error_func;
  NET_FUNC write_func;
  NET_FUNC read_func;
  U32 write_size;
  U8 write_buf[SOCKET_WRITE_BUFFER_SIZE];
  U32 read_size;
  U8 read_buf[SOCKET_READ_BUFFER_SIZE];
}NetSocket;

//////////////////////////////////////////////////////////////////////////

#define SLEEPSEC 1

//////////////////////////////////////////////////////////////////////////

extern VOID NetworkInit();
extern VOID NetworkExit();
extern I32 NetworkOpen(U32 socket);
extern VOID NetworkClose(I32 id);
extern NetSocket* NetworkFind(I32 id);
extern BOOL NetworkPoll();

//////////////////////////////////////////////////////////////////////////

#endif // __Network__

//////////////////////////////////////////////////////////////////////////