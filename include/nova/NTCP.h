/*
 *  NTCP.h
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#ifndef __NTCP__
#define __NTCP__

//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
	#include <winsock2.h>
	#pragma comment(lib,"ws2_32.lib")
#endif

//////////////////////////////////////////////////////////////////////////

#include <Nova.h>

//////////////////////////////////////////////////////////////////////////

#ifndef INVALID_SOCKET
#define INVALID_SOCKET  (U32)(~0)
#endif

//////////////////////////////////////////////////////////////////////////

#pragma pack (push, 1)
typedef struct _netaddr 
{ 
  U8 ip[4]; 
  U16	port; 
}netaddr;
#pragma pack (pop)

//////////////////////////////////////////////////////////////////////////

// open a new socket.
extern U32 NTCPOpen(U16 port);

// connect the socket.
extern U32 NTCPConnect(const netaddr* addr);

// accept a connection on a socket.
extern U32 NTCPAccept(U32 s, netaddr* addr);

// close a socket.
extern VOID NTCPClose(U32 s);

// select the max socket.
extern BOOL NTCPSelect(I32 nfds, fd_set *rfds, fd_set *wfds, fd_set *efds, I32 sec);

// send data on a socket.
extern I32 NTCPSend(U32 s, const CHAR* buf, I32 len);

// recv data from a socket
extern I32 NTCPRecv(U32 s, CHAR* buf, I32 len);

//////////////////////////////////////////////////////////////////////////

#endif // __NTCP__

//////////////////////////////////////////////////////////////////////////