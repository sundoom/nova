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
#include "lua_script.h"

//////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////

static CHAR s_sever_name[MAX_NAME];
static U16 s_server_id = INVALID_VALUE;
static I32 s_pipe_in = -1;
static I32 s_pipe_out = -1;

//////////////////////////////////////////////////////////////////////////

static VOID ProcessPipeStream();
static VOID ProcessPipeOpen(const CP_OPEN* cp);
static VOID ProcessPipeClose(const CP_CLOSE* cp);
static VOID ProcessPipeCall(const CP_CALL* cp);

//////////////////////////////////////////////////////////////////////////

C_FUNCTION_0(NSeconds, RPT0(F64), R_FLOAT)

//////////////////////////////////////////////////////////////////////////

// Test the script call the c function
VOID player_reset() {}
VOID player_set_hp(CHAR* name, I32 hp) {}
I32 player_get_hp(CHAR* name){return 0;}
C_FUNCTION_0( player_reset, PT0(), R_VOID )
C_FUNCTION_2( player_set_hp, PT2(CHAR*, I32), R_VOID, P_STR, P_INT )
C_FUNCTION_1( player_get_hp, RPT1(I32, CHAR*), R_INT, P_STR )

//////////////////////////////////////////////////////////////////////////
extern BOOL rpc_event(const CHAR* params);
VOID rpc_send(I32 sid, CHAR* cmd)
{
  rpc_event(cmd);
}
C_FUNCTION_2(rpc_send, PT2(I32, CHAR*), R_VOID, P_INT, P_STR)

//////////////////////////////////////////////////////////////////////////

/*
====================
AppInit
====================
*/
// argument : game_map_city 1 4096 main.lua
VOID AppInit(I32 argc, CHAR** argv)
{
  /*
  U32 pipe_size;
  CHAR pipe_name[MAX_NAME];

  CHECK((argc == 4) && "The game server argument is error!");

  // get the server name
  strcpy_s(s_sever_name, sizeof(s_sever_name), argv[0]);

  // get the server id
  s_server_id = atoi(argv[1]);

  // get the pipe size
  pipe_size = atoi(argv[2]);

  // create the pipe in
  memset(pipe_name,0,sizeof(pipe_name));
  sprintf_s(pipe_name, sizeof(pipe_name), "%s_in", s_sever_name);
  s_pipe_in = NPipeCreate(pipe_name, pipe_size);
  CHECK(s_pipe_in >= 0);

  // create the pipe out
  memset(pipe_name,0,sizeof(pipe_name));
  sprintf_s(pipe_name, sizeof(pipe_name), "%s_out", s_sever_name);
  s_pipe_out = NPipeCreate(pipe_name, pipe_size);
  CHECK(s_pipe_out >= 0);
*/
  // init the script
  lua_script_init();

  // time function
  DECLARE_C_FUNCTION(NSeconds);

  // rpc send function
  DECLARE_C_FUNCTION(rpc_send);

  // test script call the c function
  DECLARE_C_FUNCTION(player_reset);
  DECLARE_C_FUNCTION(player_set_hp);
  DECLARE_C_FUNCTION(player_get_hp);

  // lua main
  lua_script_load("../script/main.lua");

  // lua_script_call("test_func1", "[10,20,30,40]");
}

/*
====================
Run
====================
*/
VOID AppRun()
{
  U16 tick_param = 0;

  // process the pipe stream data
  // ProcessPipeStream();

  // the script tick
  lua_script_tick();
}

/*
====================
Exit
====================
*/
VOID AppExit()
{
  // exit the script
  lua_script_exit();
/*
  memset(s_sever_name, 0 ,sizeof(s_sever_name));
  s_server_id = INVALID_VALUE;
  if(s_pipe_in >= 0) { NPipeDestory(s_pipe_in); s_pipe_in = -1; }
  if(s_pipe_out >= 0) { NPipeDestory(s_pipe_out); s_pipe_out = -1; }
*/
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
    if(cp_base->type == CPT_OPEN)
    {
      ProcessPipeOpen((CP_OPEN*)cp_base);
    }
    else if(cp_base->type == CPT_CLOSE)
    {
      ProcessPipeClose((CP_CLOSE*)cp_base);
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
ProcessPipeOpen
====================
*/
VOID ProcessPipeOpen(const CP_OPEN* cp)
{
}

/*
====================
ProcessPipeClose
====================
*/
VOID ProcessPipeClose(const CP_CLOSE* cp)
{
}

/*
====================
ProcessPipeCall
====================
*/
VOID ProcessPipeCall(const CP_CALL* cp)
{

}

//////////////////////////////////////////////////////////////////////////
/*
====================
SendCall
====================
*/
VOID SendCall(const CP_CALL* cp)
{
  U32 size = sizeof(CP_BASE) + sizeof(U16) + cp->size;
  CHECK(NPipeWrite(s_pipe_out, cp, size));
}

//////////////////////////////////////////////////////////////////////////