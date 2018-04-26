/*
 *  Protocol.h
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#ifndef __Protocol__
#define __Protocol__

//////////////////////////////////////////////////////////////////////////

#include <Nova.h>

//////////////////////////////////////////////////////////////////////////

#pragma pack (push,1)

//////////////////////////////////////////////////////////////////////////
// Communications Protocol Type
//////////////////////////////////////////////////////////////////////////

typedef enum _CP_TYPE
{
	CPT_OPEN,
  CPT_CLOSE,
  CPT_KICK,
	CPT_CALL,
}CP_TYPE;

//////////////////////////////////////////////////////////////////////////
// Protocol
//////////////////////////////////////////////////////////////////////////

typedef struct _CP_BASE
{
  U8	type;
  U32 sid;        // the server id
}CP_BASE;

typedef struct _CP_OPEN
{
	CP_BASE base;
  U32     gid;    // the gateway id
}CP_OPEN;

typedef struct _CP_CLOSE
{
	CP_BASE base;
}CP_CLOSE;

typedef struct _CP_KICK
{
  CP_BASE base;
}CP_KICK;

typedef struct _CP_CALL
{
  CP_BASE	base;
  U16		size;
  U8		data[MAX_DATA];
}CP_CALL;

//////////////////////////////////////////////////////////////////////////

#define MAKE_GUID(sid, uid, mid) (((U32)((U8)(mid)))|((U32)((U16)(uid))<<8)|((U32)((U8)(sid))<<24))
#define MAKE_SID(guid) ((U8)(((U32)(guid)>>24)&0xFF))
#define MAKE_UID(guid) ((U16)(((U32)(guid)>>8)&0xFFFF))
#define MAKE_MID(guid) ((U8)(((U32)(guid))&0xFF))
/*
U32 guid;
U8 sid = 234, mid = 156;
U16 uid = 54321;
guid = MAKE_GUID(sid, uid, mid);
sid = MAKE_SID(guid);
mid = MAKE_MID(guid);
uid = MAKE_UID(guid);
uid++;	
*/

//////////////////////////////////////////////////////////////////////////

#pragma pack (pop)

//////////////////////////////////////////////////////////////////////////

#endif // __Protocol__

//////////////////////////////////////////////////////////////////////////