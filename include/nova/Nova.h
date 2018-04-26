/*
 *  Nova.h
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#ifndef __Nova__
#define __Nova__

//////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <malloc.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////

#undef CHAR
#undef VOID
#undef HANDLE

#ifdef _WIN32
	typedef __int64				I64;
	typedef signed int			I32;
	typedef signed short		I16;
	typedef signed char			I8;

	typedef unsigned __int64	U64;
	typedef unsigned int		U32;
	typedef unsigned short		U16;
	typedef unsigned char		U8;

	typedef float				F32;
	typedef double				F64;

	typedef char				CHAR;
	typedef wchar_t				WCHAR;

	typedef void				VOID;
	typedef void*				HANDLE;
	typedef signed int			BOOL;		
#elif LINUX
	typedef long long			I64;
	typedef signed int			I32;
	typedef signed short		I16;
	typedef signed char			I8;

	typedef unsigned long long	U64;
	typedef unsigned int		U32;
	typedef unsigned short		U16;
	typedef unsigned char		U8;

	typedef float				F32;
	typedef double				F64;

	typedef char				CHAR;
	typedef wchar_t				WCHAR;

	typedef void				VOID;
	typedef void*				HANDLE;
	typedef signed int			BOOL;
#elif __APPLE__
	typedef long long			I64;
	typedef signed int			I32;
	typedef signed short		I16;
	typedef signed char			I8;

	typedef unsigned long long	U64;
	typedef unsigned int		U32;
	typedef unsigned short		U16;
	typedef unsigned char		U8;

	typedef float				F32;
	typedef double				F64;

	typedef char				CHAR;
	typedef wchar_t				WCHAR;

	typedef void				VOID;
	typedef void*				HANDLE;
	typedef signed char			BOOL;
#elif _XBOX
	typedef __int64				I64;
	typedef signed int			I32;
	typedef signed short		I16;
	typedef signed char			I8;

	typedef unsigned __int64	U64;
	typedef unsigned int		U32;
	typedef unsigned short		U16;
	typedef unsigned char		U8;

	typedef float				F32;
	typedef double				F64;

	typedef char				CHAR;
	typedef wchar_t				WCHAR;

	typedef void				VOID;
	typedef void*				HANDLE;
	typedef signed int			BOOL;
#else
	#error Unknown platform!
#endif

#ifndef NULL
	#ifdef __cplusplus
		#define NULL    0
	#else
		#define NULL    ((void *)0)
	#endif
#endif

#ifndef FALSE
	#define FALSE       0
#endif

#ifndef TRUE
	#define TRUE		1
#endif

//////////////////////////////////////////////////////////////////////////

#define MAX_NAME		32
#define MAX_PATH		260
#define MAX_DATA	  8192

//////////////////////////////////////////////////////////////////////////

#define	MAX_I8			0x7f
#define	MIN_I8			0x80
#define	MAX_U8			0xff
#define	MIN_U8			0x00
#define	MAX_I16			0x7fff
#define	MIN_I16			0x8000
#define	MAX_U16			0xffff
#define	MIN_U16			0x0000
#define	MAX_I32			0x7fffffff
#define	MIN_I32			0x80000000
#define	MAX_U32			0xffffffff
#define	MIN_U32			0x00000000
#define	MAX_I64			0x7fffffffffffffff
#define	MIN_I64			0x8000000000000000
#define	MAX_U64			0xffffffffffffffff
#define	MIN_U64			0x0000000000000000
#define	MAX_F32			3.402823466e+38F
#define	MIN_F32			(-3.402823466e+38F)
#define	MAX_F64			1.7976931348623158e+308
#define	MIN_F64			(-1.7976931348623158e+308)
#define EPS_F32			1.192092896e-07F			// smallest number not zero
#define EPS_F64			2.2204460492503131e-016		// smallest number not zero
#define IEEE_1_0		0x3f800000					// integer representation of 1.0
#define IEEE_255_0		0x437f0000					// integer representation of 255.0
#define IEEE_MAX_F32	0x7f7fffff					// integer representation of max float
#define IEEE_MIN_F32	0xff7fffff					// integer representation of min float
#define	N_PI			3.14159265358979323846
#define	MIN(a, b)		((a) < (b) ? (a) : (b))		// returns the min value between a and b
#define	MAX(a, b)		((a) > (b) ? (a) : (b))		// returns the max value between a and b
#define INVALID_VALUE  (~0)

//////////////////////////////////////////////////////////////////////////	

extern const CHAR* VA(const CHAR* format, ...);
extern VOID NAssert(const CHAR* msg);
extern VOID NPrint(const CHAR* msg);

//////////////////////////////////////////////////////////////////////////

#ifndef DO_GUARD
	#define DO_GUARD 1
#endif

#ifndef DO_CHECK
	#define DO_CHECK 1
#endif

#if _DEBUG || !DO_GUARD || __UNIX__
	#if __GCN__ || __LINUX__
		#define GUARD(func)		{
	#else
		#define GUARD(func)		{static const CHAR *__FUNC_NAME__=#func;
	#endif

	#define UNGUARD				}	
#else
	#define GUARD(func)			{ static const CHAR *__FUNC_NAME__=#func; try{
	#define UNGUARD				} catch(CHAR*Err){throw Err;}catch(...){ NAssert(__FUNC_NAME__); throw;}}
#endif

#if DO_CHECK
	#define CHECK(expr)	{if(!(expr))NAssert(VA("Assertion failed: %s [File:%s] [Line: %i]", #expr, __FILE__, __LINE__));}
#else
	#define CHECK(expr)	if(expr){}
#endif

//////////////////////////////////////////////////////////////////////////

extern F64 NSeconds();
extern VOID NSleep(F32 seconds);

//////////////////////////////////////////////////////////////////////////

typedef struct _RPC_CALL
{
  U8	type;
  U32 sid;
  U16	size;
  U8	data[MAX_DATA];
}RPC_CALL;

//////////////////////////////////////////////////////////////////////////

#endif // __Nova__

//////////////////////////////////////////////////////////////////////////
