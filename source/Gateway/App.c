/*
 *  App.c
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

static I32 s_pipe_in = -1;
static I32 s_pipe_out = -1;

//////////////////////////////////////////////////////////////////////////

static BOOL EventTCPListen(I32 id);
static BOOL EventTCPWrite(I32 id);
static BOOL EventTCPRead(I32 id);
static BOOL EventTCPError(I32 id);

//////////////////////////////////////////////////////////////////////////

static VOID ProcessPipeStream();
static VOID ProcessPipeKick(const CP_KICK* cp);
static VOID ProcessPipeCall(const CP_CALL* cp);

//////////////////////////////////////////////////////////////////////////

static BOOL ProcessSocketStream(I32 id, const U8*data, U32 size);
static BOOL ProcessSocketOpen(I32 id, CP_OPEN* cp);
static BOOL ProcessSocketClose(I32 id, CP_CLOSE* cp);
static BOOL ProcessSocketCall(I32 id, CP_CALL* cp);

//////////////////////////////////////////////////////////////////////////

/*
====================
AppInit
====================
*/
// argument : gateway 0 4096
VOID AppInit(I32 argc, CHAR** argv)
{
  CHAR* server_name;
  CHAR pipe_name[MAX_NAME];
  U32 pipe_size;

  CHECK((argc==3) && "The gateway argument is error!");

  // get the argument
  server_name = argv[0];
  gGatewayID = atoi(argv[1]);
  pipe_size = atoi(argv[2]);

  // create the pipe in
  memset(pipe_name,0,sizeof(pipe_name));
  sprintf_s(pipe_name, sizeof(pipe_name),"%s_in",server_name);
  s_pipe_in = NPipeCreate(pipe_name, pipe_size);
  CHECK(s_pipe_in >= 0);

  // create the pipe out
  memset(pipe_name,0,sizeof(pipe_name));
  sprintf_s(pipe_name, sizeof(pipe_name),"%s_out",server_name);
  s_pipe_out = NPipeCreate(pipe_name, pipe_size);
  CHECK(s_pipe_out >= 0);

  // init the network
  NetworkInit();

  // open the listen socket
  {
    NetSocket* listen_socket = NULL;
    I32 listen_id = -1;
    U32 fd = NTCPOpen(6620);
    CHECK(fd!=INVALID_SOCKET);
    listen_id = NetworkOpen(fd);
    CHECK(listen_id>=0);
    listen_socket = NetworkFind(listen_id);
    CHECK(listen_socket);
    listen_socket->error_func = EventTCPError;
    listen_socket->write_func = EventTCPWrite;
    listen_socket->read_func = EventTCPListen;
  }
}

/*
====================
Run
====================
*/
VOID AppRun()
{
  // process the pipe stream data
  ProcessPipeStream();

  // the network poll
  if(!NetworkPoll()) Exit();
}

/*
====================
Exit
====================
*/
VOID AppExit()
{
  // exit the network
  NetworkExit();

  // destory the pipe.
  NPipeDestory(s_pipe_in);
  NPipeDestory(s_pipe_out);	
}

//////////////////////////////////////////////////////////////////////////
/*
====================
EventTCPListen
====================
*/
BOOL EventTCPListen(I32 id)
{
  U32 fd = INVALID_SOCKET;
  netaddr addr;
  NetSocket* listen_socket = NULL;
  NetSocket* new_socket = NULL;

  // accept other connecting
  listen_socket = NetworkFind(id);
  CHECK(listen_socket);
  fd = NTCPAccept(listen_socket->socket, &addr);
  if(fd != INVALID_SOCKET)
  {
    // open the new net socket and set it
    I32 new_id = NetworkOpen(fd);
    CHECK(new_id>=0);    
    new_socket = NetworkFind(id);
    CHECK(new_socket);
    new_socket->error_func = EventTCPError;
    new_socket->write_func = EventTCPWrite;
    new_socket->read_func = EventTCPRead;
    return TRUE;
  }

  return FALSE; 
}

/*
====================
EventTCPWrite
====================
*/
BOOL EventTCPWrite(I32 id)
{
  NetSocket* net_socket = NULL;

  // find the net socket
  net_socket = NetworkFind(id);
  CHECK(net_socket && net_socket->socket!=INVALID_SOCKET);

  // send the write data
  if(net_socket->write_size)
  {
    I32 send_size = -1;
    send_size = NTCPSend(net_socket->socket, net_socket->write_buf, net_socket->write_size);
    if(send_size < 0)
    {
      NetworkClose(id);
      return FALSE;
    }
    else if(send_size > 0)
    {
      if((net_socket->write_size - send_size) > 0)
      {
        memcpy(&net_socket->write_buf[0], &net_socket->write_buf[send_size], (net_socket->write_size-send_size));
      }
      net_socket->write_size -= send_size;
    }
  }

  return TRUE;
}

/*
====================
EventTCPRead
====================
*/
BOOL EventTCPRead(I32 id)
{
  NetSocket* net_socket = NULL;

  // find the net socket
  net_socket = NetworkFind(id);
  CHECK(net_socket && net_socket->socket!=INVALID_SOCKET);

  // read the socket data
  while(1)
  {
    I32 recv_size = NTCPRecv(net_socket->socket, (CHAR*)&net_socket->read_buf[net_socket->read_size], (SOCKET_READ_BUFFER_SIZE-net_socket->read_size));
    if(recv_size < 0)
    {
      NetworkClose(id);
      return FALSE;
    }
    else if(recv_size > 0)
    {
      net_socket->read_size += recv_size;
      while(net_socket->read_size > 2)
      {
        // get the packet size
        U16 packet_size = *((U16*)net_socket->read_buf);
        if(packet_size > MAX_DATA)
        {
          // the socket is illegal and close it
          NetworkClose(id);
          return FALSE;
        }
        if(packet_size + 2 > net_socket->read_size) break;

        // decode the packet

        // uncompress the packet

        // process the socket the stream
        if(ProcessSocketStream(id, &net_socket->read_buf[2], packet_size))
        {
          // move the read buffer memory
          memcpy(&net_socket->read_buf[0], &net_socket->read_buf[packet_size+2], (net_socket->read_size - (packet_size+2)));
          net_socket->read_size -= (packet_size+2);
        }
        else
        {
          NetworkClose(id);
          return FALSE;
        }
      }
    }
    else
    {
      break;
    }
  }

  return TRUE;
}

// 需要测试如果连接突然断开时是否是先调用这个函数还是先recv先返回失败，
// 如果是则在这里发close命令,否则则另外处理
/*
====================
EventTCPError
====================
*/
BOOL EventTCPError(I32 id)
{
  NetworkClose(id);
  return FALSE;
}

//////////////////////////////////////////////////////////////////////////
/*
====================
ProcessPipeStream
====================
*/
VOID ProcessPipeStream()
{
  // read the data from the in pipe
  static U8 buffer[MAX_DATA];
  static U32 buffer_size = 0;
  while(NPipeRead(s_pipe_in, buffer, &buffer_size))
  {
    CP_BASE* cp_base = (CP_BASE*)buffer;
    if(cp_base->type == CPT_KICK)
    {
      ProcessPipeKick((CP_KICK*)cp_base);
    }
    else if(cp_base->type == CPT_CALL)
    {
      ProcessPipeCall((CP_CALL*)cp_base);
    }
    else
    {
      NPrint(VA("The communications protocol type(%d) in the pipe is error.\r\n", cp_base->type));
    }
  }
}

/*
====================
ProcessPipeKick
====================
*/
VOID ProcessPipeKick(const CP_KICK* cp)
{
  NetSocket* net_socket = NULL;
  U16 uid; U8 mid;

  // get the id
  uid = MAKE_UID(cp->base.sid);
  mid = MAKE_MID(cp->base.sid);

  // find the net socket
  net_socket = NetworkFind(uid);
  CHECK(net_socket);
  if(net_socket->socket!=INVALID_SOCKET && mid==net_socket->mark)
  {
    // close this socket
    NetworkClose(uid);
  }
  else
  {
    NPrint("ProcessPipeKick : Drop the unnecessary packet.\r\n");
  }
}

/*
====================
ProcessPipeCall
====================
*/
VOID ProcessPipeCall(const CP_CALL* cp)
{
  NetSocket* net_socket = NULL;
  U16 uid; U8 mid;
  U16 size;

  // get the packet size
  size = sizeof(CP_BASE) + sizeof(U16) + cp->size;

  // get the id
  uid = MAKE_UID(cp->base.sid);
  mid = MAKE_MID(cp->base.sid);

  // find the net socket
  net_socket = NetworkFind(uid);
  CHECK(net_socket);
  if(net_socket->socket!=INVALID_SOCKET && mid==net_socket->mark)
  {
    // compress the packet

    // encode the packet

    // write the packet to the send buffer
    if(SOCKET_WRITE_BUFFER_SIZE - net_socket->write_size >= size)
    {
      // write the packet size
      memcpy(&net_socket->write_buf[net_socket->write_size], &size, sizeof(size));
      net_socket->write_size += sizeof(size);

      // write the packet data
      memcpy(&net_socket->write_buf[net_socket->write_size], cp, size);
      net_socket->write_size += size;
    }
    else
    {
      // the write buffer is overflow and close this socket
      NetworkClose(uid);
    }
  }
  else
  {
    NPrint("ProcessPipeCall : Drop the unnecessary packet.\r\n");
  }
}

//////////////////////////////////////////////////////////////////////////

/*
====================
ProcessSocketStream
====================
*/
BOOL ProcessSocketStream(I32 id, const U8*data, U32 size)
{
  CP_BASE* cp_base = (CP_BASE*)data;
  if(cp_base->type == CPT_OPEN)
  {
    CHECK(size == sizeof(CP_OPEN));
    return ProcessSocketOpen(id, (CP_OPEN*)cp_base);
  }
  else if(cp_base->type == CPT_CLOSE)
  {
    CHECK(size == sizeof(CP_CLOSE));
    return ProcessSocketClose(id, (CP_CLOSE*)cp_base);
  }
  else if(cp_base->type == CPT_CALL)
  {
    CHECK(size == (sizeof(CP_BASE)+sizeof(U16)+((CP_CALL*)cp_base)->size));
    return ProcessSocketCall(id, (CP_CALL*)cp_base);
  }
  
  NPrint(VA("The communications protocol type(%d) in the socket is error.\r\n", cp_base->type));
  return FALSE;
}

/*
====================
ProcessSocketOpen
====================
*/
BOOL ProcessSocketOpen(I32 id, CP_OPEN* cp)
{
  // find the net socket
  NetSocket* net_socket = NetworkFind(id);
  CHECK(net_socket);
  
  // check the socket state and set it
  if(net_socket->state != NS_UNUSED)
  {
    NPrint(VA("ProcessSocketOpen : the socket state(%d) is error.\r\n", net_socket->state));
    return FALSE;
  }
  net_socket->state = NS_OPENED;

  // set the the gateway id
  cp->gid = MAKE_GUID(gGatewayID, id, net_socket->mark);

  // send open command to the server
  CHECK(NPipeWrite(s_pipe_out, cp, sizeof(CP_OPEN)));

  return TRUE;  
}

/*
====================
ProcessSocketClose
====================
*/
BOOL ProcessSocketClose(I32 id, CP_CLOSE* cp)
{
  // find the net socket
  NetSocket* net_socket = NetworkFind(id);
  CHECK(net_socket);

  // check the socket state
  if(net_socket->state != NS_OPENED)
  {
    NPrint(VA("ProcessSocketClose : the socket state(%d) is error.\r\n", net_socket->state));
    return FALSE;
  }
  net_socket->state = NS_CLOSED;

  // send close command
  CHECK(NPipeWrite(s_pipe_out, cp, sizeof(CP_CLOSE)));

  return FALSE;
}

/*
====================
ProcessSocketCall
====================
*/
BOOL ProcessSocketCall(I32 id, CP_CALL* cp)
{
  U32 size;

  // find the net socket
  NetSocket* net_socket = NetworkFind(id);
  CHECK(net_socket);

  // check the socket state
  if(net_socket->state != NS_OPENED)
  {
    NPrint(VA("ProcessSocketCall : the socket state(%d) is error.\r\n", net_socket->state));
    return FALSE;
  }  
  
  // send call command
  size = sizeof(CP_BASE) + sizeof(U16) + cp->size;
  CHECK(NPipeWrite(s_pipe_out, cp, size));

  return TRUE;
}

//////////////////////////////////////////////////////////////////////////