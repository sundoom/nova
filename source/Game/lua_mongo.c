/*
 *  lua_mongo.c
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#include <Nova.h>

//////////////////////////////////////////////////////////////////////////

#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

//////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////

static I32	__socket_open(lua_State* ls);
static I32	__socket_connect(lua_State* ls);
static I32	__socket_accept(lua_State* ls);
static I32	__socket_close(lua_State* ls);
static I32	__socket_send(lua_State* ls);
static I32	__socket_recv(lua_State* ls);

// socket object methods
static luaL_reg socket_methods[] = {
  {"open",        __socket_open},
  {"connect",     __socket_connect},
  {"accept",      __socket_accept},
  {"close",       __socket_close},  
  {"send",        __socket_send},
  {"recv",        __socket_recv},
  {NULL,          NULL}
};

//////////////////////////////////////////////////////////////////////////

VOID __register_table();

//////////////////////////////////////////////////////////////////////////

/*
====================
lua_mongo_init
====================
*/
VOID lua_mongo_init(lua_State* ls)
{
  
}

/*
====================
lua_mongo_exit
====================
*/
VOID lua_mongo_exit(lua_State* ls)
{

}

//////////////////////////////////////////////////////////////////////////

I32	__socket_open(lua_State* ls)
{
  return 0;
}

I32	__socket_connect(lua_State* ls)
{
  return 0;
}

I32	__socket_accept(lua_State* ls)
{
  return 0;
}

I32	__socket_close(lua_State* ls)
{
  return 0;
}

I32	__socket_send(lua_State* ls)
{
  return 0;
}

I32	__socket_recv(lua_State* ls)
{
  return 0;
}

//////////////////////////////////////////////////////////////////////////
