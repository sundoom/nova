/*
 *  luascript.h
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#ifndef __lua_script__
#define __lua_script__

//////////////////////////////////////////////////////////////////////////

#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

//////////////////////////////////////////////////////////////////////////

#define PT0()
#define PT1(pt0)                            pt0 a;
#define PT2(pt0, pt1)                       pt0 a; pt1 b;
#define PT3(pt0, pt1, pt2)                  pt0 a; pt1 b; pt3 c;
#define PT4(pt0, pt1, pt2, pt3)             pt0 a; pt1 b; pt3 c; pt4 d;
#define PT5(pt0, pt1, pt2, pt3, pt4)        pt0 a; pt1 b; pt3 c; pt4 d; pt5 e;

#define RPT0(rt)                            rt r;
#define RPT1(rt, pt0)                       rt r; pt0 a;
#define RPT2(rt, pt0, pt1)                  rt r; pt0 a; pt1 b;
#define RPT3(rt, pt0, pt1, pt2)             rt r; pt0 a; pt1 b; pt3 c;
#define RPT4(rt, pt0, pt1, pt2, pt3)        rt r; pt0 a; pt1 b; pt3 c; pt4 d;
#define RPT5(rt, pt0, pt1, pt2, pt3, pt4)   rt r; pt0 a; pt1 b; pt3 c; pt4 d; pt5 e;

#define P_BOOL(var, idx)    var = (BOOL)lua_toboolean(ls, idx);
#define P_INT(var, idx)     var = (I32)lua_tointeger(ls, idx);
#define P_FLOAT(var, idx)   var = (F64)lua_tonumber(ls, idx);
#define P_STR(var, idx)     var = (CHAR*)lua_tostring(ls, idx);

#define R_VOID(func)        func; return 0;
#define R_BOOL(func)        r = func; lua_pushboolean(ls,(BOOL)r); return 1;
#define R_INT(func)         r = func; lua_pushinteger(ls, (I32)r); return 1;
#define R_FLOAT(func)       r = func; lua_pushnumber(ls, (F64)r); return 1;
#define R_STR(func)         r = func; lua_pushstring(ls, (CHAR*)r); return 1;

#define C_FUNCTION_0(FUNC_NAME, PARAM_TYPE, RETURN)  \
  I32 C_##FUNC_NAME(lua_State* ls) \
  { \
  PARAM_TYPE; \
  RETURN(FUNC_NAME());  \
  } \

#define C_FUNCTION_1(FUNC_NAME, PARAM_TYPE, RETURN, PARAM_0)  \
  I32 C_##FUNC_NAME(lua_State* ls) \
  { \
  PARAM_TYPE; \
  PARAM_0(a, 1); \
  RETURN(FUNC_NAME(a)); \
  } \

#define C_FUNCTION_2(FUNC_NAME, PARAM_TYPE, RETURN, PARAM_0, PARAM_1)  \
  I32 C_##FUNC_NAME(lua_State* ls) \
  { \
  PARAM_TYPE; \
  PARAM_0(a, 1); \
  PARAM_1(b, 2); \
  RETURN(FUNC_NAME(a, b)); \
  } \

#define C_FUNCTION_3(FUNC_NAME, PARAM_TYPE, RETURN, PARAM_0, PARAM_1, PARAM_2)  \
  I32 C_##FUNC_NAME(lua_State* ls) \
  { \
  PARAM_TYPE; \
  PARAM_0(a, 1); \
  PARAM_1(b, 2); \
  PARAM_2(c, 3); \
  RETURN(FUNC_NAME(a, b, c)); \
  } \

#define C_FUNCTION_4(FUNC_NAME, PARAM_TYPE, RETURN, PARAM_0, PARAM_1, PARAM_2, PARAM_3)  \
  I32 C_##FUNC_NAME(lua_State* ls) \
  { \
  PARAM_TYPE; \
  PARAM_0(a, 1); \
  PARAM_1(b, 2); \
  PARAM_2(c, 3); \
  PARAM_3(d, 4); \
  RETURN(FUNC_NAME(a, b, c, d)); \
  } \

#define C_FUNCTION_5(FUNC_NAME, PARAM_TYPE, RETURN, PARAM_0, PARAM_1, PARAM_2, PARAM_3, PARAM_4)  \
  I32 C_##FUNC_NAME(lua_State* ls) \
  { \
  PARAM_TYPE; \
  PARAM_0(a, 1); \
  PARAM_1(b, 2); \
  PARAM_2(c, 3); \
  PARAM_3(d, 4); \
  PARAM_4(e, 5); \
  RETURN(FUNC_NAME(a, b, c, d, e)); \
  } \

#define DECLARE_C_FUNCTION(FUNC_NAME) \
  lua_script_func(#FUNC_NAME, (lua_CFunction)C_##FUNC_NAME);

//////////////////////////////////////////////////////////////////////////
/*
// Test the script call the c function
VOID player_reset() {}
VOID player_set_hp(CHAR* name, I32 hp) {}
I32 player_get_hp(CHAR* name){return 0;}
C_FUNCTION_0( player_reset, PT0(), R_VOID )
C_FUNCTION_2( player_set_hp, PT2(CHAR*, I32), R_VOID, P_STR, P_INT )
C_FUNCTION_1( player_get_hp, RPT1(I32, CHAR*), R_INT, P_STR )

DECLARE_C_FUNCTION(player_reset);
DECLARE_C_FUNCTION(player_set_hp);
DECLARE_C_FUNCTION(player_get_hp);

ScriptCall("test_func1", "[10,20,30,40]");
*/

//////////////////////////////////////////////////////////////////////////

#define LUA_DEBUG
#ifdef LUA_DEBUG
  #define LUA_STACK	const CHAR* __lua_stack_info;
  #define LUA_WATCH(ls)	__lua_stack_info = __lua_stack(ls);
  extern const CHAR* __lua_stack(lua_State* ls);
  extern I32 __lua_print(lua_State* ls);
  extern I32 __lua_error(lua_State* ls);
#else
  #define LUA_STACK
  #define LUA_WATCH(ls)
#endif

//////////////////////////////////////////////////////////////////////////

extern VOID lua_script_init();
extern VOID lua_script_exit();
extern VOID lua_script_tick();

extern BOOL lua_script_load(const CHAR* file_name);
extern BOOL lua_script_exec(const CHAR* name, const CHAR* string);
extern VOID lua_script_func(const CHAR* name, lua_CFunction func);
extern const CHAR* lua_script_call(const CHAR* name, const CHAR* params);

//////////////////////////////////////////////////////////////////////////

#endif // __lua_script__

//////////////////////////////////////////////////////////////////////////