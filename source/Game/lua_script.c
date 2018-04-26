/*
 *  luascript.c
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
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////

static lua_State* s_lua_state = NULL;

//////////////////////////////////////////////////////////////////////////

/*
====================
ScriptInit
====================
*/
extern VOID lua_socket_init(lua_State* ls);
VOID lua_script_init()
{
  lua_State* ls;
  LUA_STACK;

  CHECK(s_lua_state==NULL);

  // openg lua
  ls = lua_open(); 
  CHECK(ls!=NULL);
  luaL_openlibs(ls);
  s_lua_state = ls;
  
  // the new print function
  lua_pushstring(ls, "print");
  LUA_WATCH(ls);
  lua_pushcclosure(ls, __lua_print, 0);
  LUA_WATCH(ls);
  lua_settable(ls, LUA_GLOBALSINDEX);
  LUA_WATCH(ls);

  // the new error function
  lua_pushstring(ls, "error");
  LUA_WATCH(ls);
  lua_pushcclosure(ls, __lua_error, 0);
  LUA_WATCH(ls);
  lua_settable(ls, LUA_GLOBALSINDEX);
  LUA_WATCH(ls);

  // init the socket
  lua_socket_init(ls);
}

/*
====================
lua_script_exit
====================
*/
extern VOID lua_socket_exit(lua_State* ls);
VOID lua_script_exit()
{
  // exit the socket
  lua_socket_exit(s_lua_state);

  if(s_lua_state) { lua_close(s_lua_state); s_lua_state = NULL; }
}

/*
====================
lua_script_tick
====================
*/
extern BOOL lua_socket_select(lua_State* ls);
VOID lua_script_tick()
{
  // the script tick
  lua_script_call("tick", NULL);

  // the socket select
  lua_socket_select(s_lua_state);
}

/*
====================
lua_script_load
====================
*/
BOOL lua_script_load(const CHAR* file_name)
{
  lua_State* ls;
  LUA_STACK;

  ls = (lua_State*)s_lua_state;
  CHECK(ls);
  LUA_WATCH(ls);
  if(luaL_loadfile(ls, file_name) == 0)
  {
    LUA_WATCH(ls);
    if(lua_pcall(ls, 0, LUA_MULTRET, 0) == 0)
    {
      LUA_WATCH(ls);
      return TRUE;
    }
  }

  LUA_WATCH(ls);
  __lua_error(ls);

  return FALSE;
}

/*
====================
lua_script_exec
====================
*/
BOOL lua_script_exec(const CHAR* name, const CHAR* string)
{
  lua_State* ls;
  LUA_STACK;

  ls = (lua_State*)s_lua_state;
  CHECK(ls);
  LUA_WATCH(ls);
  if(luaL_loadbuffer(ls, string, strlen(string), name) == 0)
  {
    LUA_WATCH(ls);
    if(lua_pcall(ls, 0, LUA_MULTRET, 0) == 0)
    {
      LUA_WATCH(ls);
      return TRUE;
    }
  }

  LUA_WATCH(ls);
  __lua_error(ls);

  return FALSE;
}

/*
====================
lua_script_func
====================
*/
VOID lua_script_func(const CHAR* name, lua_CFunction func)
{
  lua_State* ls;
  LUA_STACK;

  ls = (lua_State*)s_lua_state;
  CHECK(ls);
  LUA_WATCH(ls);

  // check the function if it has been registered?
  lua_pushstring(ls, name);
  LUA_WATCH(ls);
  lua_gettable(ls, LUA_GLOBALSINDEX);
  LUA_WATCH(ls);
  if(!lua_isnil(ls, -1))
  {
    lua_pushstring(ls, VA("The C function(%s) had been registered!\n",lua_tostring(ls, 1)));
    __lua_error(ls);
  }
  lua_pop(ls, 1);
  LUA_WATCH(ls);

  // register the new function
  lua_pushstring(ls, name);
  LUA_WATCH(ls);
  lua_pushcclosure(ls, (lua_CFunction)func, 0);
  LUA_WATCH(ls);
  lua_settable(ls, LUA_GLOBALSINDEX);
  LUA_WATCH(ls);
}

/*
====================
lua_script_call

  param is the json string
====================
*/
const CHAR* lua_script_call(const CHAR* func_name, const CHAR* params)
{
  lua_State* ls;
  I32 argc = 0;
  const CHAR* ret_value = "";
  LUA_STACK;

  ls = s_lua_state; 
  CHECK(ls);
  LUA_WATCH(ls);
  CHECK(func_name);

  // check the script call function
  lua_pushstring(ls, "script_call");
  LUA_WATCH(ls);
  lua_gettable(ls, LUA_GLOBALSINDEX);
  LUA_WATCH(ls);
  if(!lua_isfunction(ls, -1))
  {
    lua_pushstring(ls, VA("ScriptCall : can`t find the script_call function.\n"));
    __lua_error(ls);
    return NULL;
  }

  // push the function name
  lua_pushstring(ls, func_name);
  LUA_WATCH(ls);
  argc += 1;

  // push the param
  if (params != NULL)
  {
    argc += 1;
    lua_pushstring(ls, params);
    LUA_WATCH(ls);
  }

  // call the function
  if(lua_pcall(ls, argc, 1, 0) != 0)
  {
    LUA_WATCH(ls);
    lua_pushstring(ls, VA("ScriptCall : failed to call the script_call function.\n"));
    __lua_error(ls);
    return NULL;
  }
  LUA_WATCH(ls);

  // get the return value
  if(lua_isstring(ls, -1))
  {
    ret_value = (CHAR*)lua_tostring(ls, -1);    
  }
  lua_pop(ls, 1);
  LUA_WATCH(ls);

  return ret_value;
}

//////////////////////////////////////////////////////////////////////////
/*
====================
__lua_print
====================
*/
I32	__lua_print(lua_State* ls)
{
	I32 n = lua_gettop(ls);
	I32 i;
	lua_getglobal(ls, "tostring");
	for (i = 1; i <= n; i++) 
	{
		const CHAR *s;
		lua_pushvalue(ls, -1);
		lua_pushvalue(ls, i);
		lua_call(ls, 1, 1);
		s = lua_tostring(ls, -1);
		if (s == NULL)
			return luaL_error(ls, LUA_QL("tostring") " must return a string to " LUA_QL("print"));
		if (i > 1)
      NPrint("\t");
		NPrint(s);
		lua_pop(ls, 1);
	}
	NPrint("\n");
  return 0;
}

/*
====================
__lua_error
====================
*/
I32	__lua_error(lua_State* ls)
{
  LUA_STACK;
  LUA_WATCH(ls);
  luaL_where(ls, 1);
  LUA_WATCH(ls);
  if(lua_isstring(ls, -2)) 
  {
    lua_pushvalue(ls, -2);
    LUA_WATCH(ls);
    lua_concat(ls, 2);
    LUA_WATCH(ls);
  }
  NAssert(lua_tostring(ls, -1));
  return lua_error(ls);
}

//////////////////////////////////////////////////////////////////////////
/*
====================
__lua_stack
====================
*/
const CHAR*	__lua_stack(lua_State* ls)
{
  static CHAR buffer[2048];
  I32 i, j = 0;

  memset(buffer, 0, sizeof(buffer));
  for(i = 1; i <= lua_gettop(ls); i++)
  {
    I32 type = lua_type( ls, i );
    switch(type)
    {
    case LUA_TNONE:
      j += sprintf(buffer+j, "id=%d; type=NONE; value=none \n", i);
      break;
    case LUA_TNIL:
      j += sprintf(buffer+j, "id=%d; type=NIL; value=nil \n", i);
      break;
    case LUA_TBOOLEAN:
      if((lua_toboolean(ls, i)!=0))
        j += sprintf(buffer+j, "id=%d; type=BOOL; value=true \n", i);
      else
        j += sprintf(buffer+j, "id=%d; type=BOOL; value=false \n", i);
      break;
    case LUA_TLIGHTUSERDATA:
      j += sprintf(buffer+j, "id=%d; type=LIGHTUSERDATA; value=%X \n", i, lua_touserdata(ls, i));
      break;
    case LUA_TNUMBER:
      j += sprintf(buffer+j, "id=%d; type=NUMBER; value=%f \n", i, lua_tonumber(ls, i));
      break;
    case LUA_TSTRING:
      j += sprintf(buffer+j, "id=%d; type=STRING; value=%s \n", i, lua_tostring(ls, i));
      break;
    case LUA_TTABLE:
      j += sprintf(buffer+j, "id=%d; type=TABLE; value=%X \n", i, lua_topointer(ls, i));
      break;
    case LUA_TFUNCTION:
      j += sprintf(buffer+j, "id=%d; type=FUNCTION; value=%X \n", i, lua_topointer(ls, i));
      break;
    case LUA_TUSERDATA:
      j += sprintf(buffer+j, "id=%d; type=USERDATA; value=%X \n", i, lua_touserdata(ls, i));
      break;
    case LUA_TTHREAD:
      j += sprintf(buffer+j, "id=%d; type=THREAD; value=%X \n", i, lua_topointer(ls, i));
      break;
    default:
      break;
    }
  }

  return buffer;
}

//////////////////////////////////////////////////////////////////////////

/*
====================
rpc_event

  param is the json string
====================
*/
BOOL rpc_event(const CHAR* params)
{
  lua_State* ls;
  LUA_STACK;  

  ls = s_lua_state; 
  CHECK(ls);
  LUA_WATCH(ls);
  CHECK(params);

  // check the function
  lua_pushstring(ls, "rpc_event");
  LUA_WATCH(ls);
  lua_gettable(ls, LUA_GLOBALSINDEX);
  LUA_WATCH(ls);
  if(!lua_isfunction(ls, -1))
  {
    lua_pushstring(ls, VA("rpc_event : can`t find the rpc_event function.\n"));
    __lua_error(ls);
    return FALSE;
  }
  
  lua_pushstring(ls, params);
  LUA_WATCH(ls);

  // call the function
  if(lua_pcall(ls, 1, 0, 0) != 0)
  {
    LUA_WATCH(ls);
    return FALSE;
  }
  LUA_WATCH(ls);

  return TRUE;
}


//////////////////////////////////////////////////////////////////////////
