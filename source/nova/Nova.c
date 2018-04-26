/*
 *  Nova.c
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#ifndef __GCN__
	#include <sys/types.h>
	#include <sys/stat.h>
#endif

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <mmsystem.h>
	#pragma comment(lib,"winmm.lib")
	#include <io.h>
#endif

//////////////////////////////////////////////////////////////////////////

#include <Nova.h>

//////////////////////////////////////////////////////////////////////////

/*
====================
VA
====================
*/
const CHAR* VA(const CHAR *format, ...)
{
	va_list	argptr;
	static CHAR	string[32000];	// in case va is called by nested functions

	va_start (argptr, format);
	vsnprintf(string,sizeof(string), format,argptr);
	va_end (argptr);

	return(string);
}

/*
====================
NAssert
====================
*/
VOID NAssert(const CHAR* msg)
{
#ifdef _WIN32
	MessageBox(NULL, msg, "Assert", MB_OK|MB_ICONERROR|MB_TASKMODAL);
#else
	NPrint(msg);
#endif
}

/*
====================
NPrint
====================
*/
VOID NPrint(const CHAR* msg)
{
#ifdef _WIN32
	printf(msg);
#endif
}

//////////////////////////////////////////////////////////////////////////

/*
====================
GSeconds

  Returns time in seconds. Origin is arbitrary.
====================
*/
#ifdef _WIN32
  static F64 s_seconds_per_cycle = 0.0;
  static BOOL s_seconds_initialize = FALSE;
  static F64 InitTiming()
  {
    LARGE_INTEGER Frequency;
    CHECK(QueryPerformanceFrequency(&Frequency));  
    return 1.0 / Frequency.QuadPart;
  }
  F64 NSeconds()
  {
	  LARGE_INTEGER cycles;
    if (!s_seconds_initialize)
    {
      s_seconds_per_cycle = InitTiming();
      s_seconds_initialize = TRUE;
    }
	  QueryPerformanceCounter(&cycles);
	  return cycles.QuadPart * s_seconds_per_cycle + 16777216.0;
  }
#else
  F64 NSeconds()
  {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((F64)tv.tv_sec) + (((F64)tv.tv_usec)/1000000.0);
  }
#endif

/*
====================
NSleep
====================
*/
VOID NSleep(F32 seconds)
{
#ifdef _WIN32
  Sleep((U32)(seconds*1000.0f));
#else
  usleep((I32)(seconds*1000000.0f));
#endif
}

//////////////////////////////////////////////////////////////////////////