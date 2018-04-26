/*
 *  NTCP.c
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#include <NTCP.h>

//////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////

static VOID NetaddrToSockaddr(const netaddr* a, struct sockaddr_in* s);
static VOID SockaddrToNetaddr(const struct sockaddr_in* s, netaddr* a);
static VOID StringToSockaddr(const CHAR* ip, struct sockaddr_in* sadr);
static const CHAR* SockaddrToString(const struct sockaddr_in* sadr);

//////////////////////////////////////////////////////////////////////////

static const CHAR* GetErrorString();

//////////////////////////////////////////////////////////////////////////

/*
====================
NTCPOpen
====================
*/
U32 NTCPOpen(U16 port)
{
	U32 fd = INVALID_SOCKET;
	I32	opt = 0;
	I32 ret = -1;
	struct sockaddr_in sock_addr;

	// create the host socket.	
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(fd == INVALID_SOCKET)
	{
		NAssert(VA( "WARNING: There was an error creating the host socket: %s\n", GetErrorString()));	
		return INVALID_SOCKET;
	}

	// make it non-blocking
	opt = 1;
	ret = ioctlsocket(fd, FIONBIO, (U32*)&opt);
	if(ret < 0)
	{
		NAssert(VA("WARNING: There was an error making a socket non-blocking : %s\n", GetErrorString()));
		return INVALID_SOCKET;
	}

	// bind the socket.
	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(port);
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(fd, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr_in));
	if(ret < 0)
	{
		NAssert(VA("WARNING: There was an error binding a socket : %s\n", GetErrorString()));
		return INVALID_SOCKET;
	}

	// start listening on the socket.
	ret = listen(fd, 5);
	if(ret < 0)
	{
		NAssert(VA("WARNING: There was an error listening on a socket : %s\n", GetErrorString()));		
		return INVALID_SOCKET;
	}

	return fd;
}

/*
====================
NTCPConnect
====================
*/
U32 NTCPConnect(const netaddr* addr)
{
	U32 fd = INVALID_SOCKET;
	I32	opt = 0;
	I32 ret = -1;
	struct sockaddr_in sock_addr;

	// create the host socket.	
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(fd == INVALID_SOCKET)
	{
		NAssert(VA( "WARNING: There was an error creating the host socket: %s\n", GetErrorString()));	
		return -1;
	}

	// make it non-blocking
	opt = 1;
	ret = ioctlsocket(fd, FIONBIO, (U32*)&opt);
	if(ret < 0)
	{
		NAssert(VA("WARNING: There was an error making a socket non-blocking : %s\n", GetErrorString()));
		return -1;
	}

	// make socket address
	NetaddrToSockaddr(addr, &sock_addr);

	// connect the socket.
	ret = connect(fd, (struct sockaddr *)&sock_addr, sizeof(struct sockaddr_in) );
	if(ret < 0)
	{
		NAssert(VA("WARNING: There was an error connecting a socket : %s\n", GetErrorString()));
		return -1;
	}

	return fd;
}

/*
====================
NTCPAccept
====================
*/
U32 NTCPAccept(U32 s, netaddr* addr)
{
	U32 fd = INVALID_SOCKET;
	I32	opt = 0;
	I32 ret = -1;
	struct sockaddr_in sock_addr;
	I32 addr_len = sizeof(sock_addr);
	
	// accept a new connection
	CHECK(s!=INVALID_SOCKET && addr!=NULL);
	memset(&sock_addr, 0, sizeof(sock_addr));	
	fd = accept(s, (struct sockaddr*)&sock_addr, &addr_len);
	if(fd != INVALID_SOCKET)
	{
		NAssert(VA("WARNING: There was an error accept a socket : %s\n", GetErrorString()));
		return INVALID_SOCKET;
	}

	// make it non-blocking
	opt = 1;
	ret = ioctlsocket(fd, FIONBIO, (U32*)&opt);
	if(ret < 0)
	{
		NAssert(VA("WARNING: There was an error making a socket non-blocking : %s\n", GetErrorString()));
		return INVALID_SOCKET;
	}

	// get the netaddr
	SockaddrToNetaddr(&sock_addr, addr);

	return fd;
}

/*
====================
NTCPClose
====================
*/
VOID NTCPClose(U32 s)
{
	// close the socket
	CHECK(s != INVALID_SOCKET);
	closesocket(s);
}

/*
====================
NTCPSelect
====================
*/
BOOL NTCPSelect(I32 nfds, fd_set *rfds, fd_set *wfds, fd_set *efds, I32 sec)
{
	I32 ret = -1;
	struct timeval tv;
	tv.tv_sec = sec;
	tv.tv_usec = 0;
	ret = select(nfds, rfds, wfds, efds, &tv);
	if(ret < 1)
	{
		NAssert(VA("WARNING: There was an error checking for select the socket : %s\n", GetErrorString()));
		return FALSE;
	}

	return TRUE;
}

/*
====================
NTCPSend
====================
*/
I32 NTCPSend(U32 s, const CHAR* buf, I32 len)
{
	I32 ret = -1;
	I32 total = 0;

	do
	{
		ret = send(s, (const CHAR*)&buf[total], (len-total), 0 );
		if(ret == SOCKET_ERROR)
		{
			if(WSAGetLastError() != WSAEWOULDBLOCK)
			{
				NAssert(VA("NTCPSend: There was an error sending on a socket.\n"));
				return -1;
			}
		}
		else if(ret == 0)
		{
			// connection closed.
			return -1;
		}
		else
		{
			total += ret;
			if(total >= len)
			{
				return total;
			}
		}
	}
	while(ret != SOCKET_ERROR);

	return total;
}

/*
====================
NTCPRecv
====================
*/
I32 NTCPRecv(U32 s, CHAR* buf, I32 len)
{
	I32 ret;
	I32 total = 0;

	do
	{
		// read from the network.
		ret = recv(s, (char*)&buf[total], (len-total), 0);
		if(ret == SOCKET_ERROR)
		{
			if(WSAGetLastError() != WSAEWOULDBLOCK)
			{
				NAssert(VA("NTCPRecv: There was an error reading from a socket.\n"));
				return -1;
			}
		}
		else if(ret == 0)
		{
			// connection closed.
			return -1;
		}
		else
		{
			// update the buffer len.
			total += ret;
			if(total >= len)
			{
				return total;
			}
		}
	}
	while(ret != SOCKET_ERROR);

	return total;
}

//////////////////////////////////////////////////////////////////////////

/*
====================
NetaddrToSockaddr
====================
*/
VOID NetaddrToSockaddr(const netaddr *a, struct sockaddr_in *s) 
{
	memset(s, 0, sizeof(struct sockaddr_in));
	s->sin_family = AF_INET;
	s->sin_addr.s_addr = *(U32*)&a->ip;
	s->sin_port = a->port;
}

/*
====================
SockaddrToNetaddr
====================
*/
VOID SockaddrToNetaddr(const struct sockaddr_in*s, netaddr *a) 
{
	*(U32*)&a->ip = s->sin_addr.s_addr;
	a->port = s->sin_port;
}

/*
====================
StringToSockaddr
====================
*/
VOID StringToSockaddr(const CHAR* ip, struct sockaddr_in* sadr)
{
  CHAR buffer[MAX_DATA];
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
SockaddrToString
====================
*/
const CHAR* SockaddrToString(const struct sockaddr_in* sadr)
{
  CHAR buffer[MAX_DATA];
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
GetErrorString
====================
*/
const CHAR* GetErrorString()
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