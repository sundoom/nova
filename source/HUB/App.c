/*
 *  App.cpp
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

#ifndef MAX_NUM_SERVER
  #define MAX_NUM_SERVER 64
#endif

typedef struct _Server
{
  CHAR name[MAX_NAME];
  I32 pipe_in;
  I32 pipe_out;
}Server;

static U32 s_num_server = 0;
static Server s_servers[MAX_NUM_SERVER];

//////////////////////////////////////////////////////////////////////////

/*
====================
AppInit
====================
*/
// argument : gateway 0 4096 map0 1 4096
VOID AppInit(I32 argc, CHAR** argv)
{
  I32 i;
  CHAR* server_name;
  U16 server_id;
  CHAR pipe_name[MAX_NAME];
  U32 pipe_size;
  Server* server;

  CHECK(((argc%3)==0) && "The server argument is error!");

  // init all of the servers
  for(i = 0; i < MAX_NUM_SERVER; i++)
  {
    server = &s_servers[i];
    memset(server->name, 0, sizeof(server->name));
    server->pipe_in = -1;
    server->pipe_out = -1;
  }

  // create all of the servers
  for(i = 0; i < argc; i += 3)
  {
    // get the argument
    server_name = argv[i];
    server_id = atoi(argv[i+1]);
    pipe_size = atoi(argv[i+2]);

    // set the server
    CHECK(server_id<MAX_NUM_SERVER && server_id==s_num_server);
    server = &s_servers[server_id];
    s_num_server++;
    CHECK(server->name[0]==0 && server->pipe_in==-1 && server->pipe_out==-1);

    // set the server name
    strcpy_s(server->name, sizeof(server->name), server_name);

    // create the pipe in
    memset(pipe_name,0,sizeof(pipe_name));
    sprintf_s(pipe_name, sizeof(pipe_name),"%s_in",server->name);
    server->pipe_in = NPipeCreate(pipe_name, pipe_size);
    CHECK(server->pipe_in >= 0);

    // create the pipe out
    memset(pipe_name,0,sizeof(pipe_name));
    sprintf_s(pipe_name, sizeof(pipe_name),"%s_out",server->name);
    server->pipe_out = NPipeCreate(pipe_name, pipe_size);
    CHECK(server->pipe_out >= 0);
  }
}

/*
====================
Run
====================
*/
VOID AppRun()
{
  U32 i;
  static U8 buffer[MAX_DATA];
  static U32 buffer_size = 0;
  Server *src, *dst;
  CP_BASE *cp;
  U8 sid;

  // process the server stream data  
  for(i = 0; i < s_num_server; i++)
  {
    src = &s_servers[i];
    CHECK(src->pipe_out >= 0);
    while(NPipeRead(src->pipe_out, buffer, &buffer_size))
    {
      cp = (CP_BASE*)buffer;
      sid = MAKE_SID(cp->sid);
      CHECK(sid < s_num_server);
      dst = &s_servers[sid];
      CHECK(dst->pipe_in >= 0);
      CHECK(NPipeWrite(dst->pipe_in, buffer, buffer_size));
    }
  }

  // NPrint(VA("The communications protocol type(%d) and param(%d) in the pipe is error.\r\n", cp_base->type, cp_base->param));  
}

/*
====================
Exit
====================
*/
VOID AppExit()
{
  I32 i;
  for(i = 0; i < MAX_NUM_SERVER; i++)
  {
    Server *s = &s_servers[i];
    memset(s->name, 0 ,sizeof(s->name));
    if(s->pipe_in >= 0) { NPipeDestory(s->pipe_in); s->pipe_in = -1; }
    if(s->pipe_out >= 0) { NPipeDestory(s->pipe_out); s->pipe_out = -1; }
  }	
}

//////////////////////////////////////////////////////////////////////////