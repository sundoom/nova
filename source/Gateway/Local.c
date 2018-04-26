/*
 *  Local.c
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#include "Local.h"

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif

//////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////

U8 gGatewayID = INVALID_VALUE;

//////////////////////////////////////////////////////////////////////////

static BOOL s_exit = FALSE;

//////////////////////////////////////////////////////////////////////////

I32 main(I32 argc, CHAR** argv)
{
  WSADATA	wsa_data;

  if(argc < 2)
  {
    NAssert(VA("%s : requires gateway argument.",argv[0]));
    return -1;
  }

  // init window socket	
  CHECK(WSAStartup(MAKEWORD(1, 1), &wsa_data) == 0);	

  // init the application
  AppInit(argc-1, &argv[1]);

  // run the application
  while(s_exit == FALSE)
  {
    AppRun();
  }
  
  // exit the application
  AppExit();
	
	// free window socket
	WSACleanup();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

VOID Exit()
{
  s_exit = TRUE;
}

//////////////////////////////////////////////////////////////////////////
