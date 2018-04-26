/*
 *  NPipe.c
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#include <NPipe.h>
#include <NMemory.h>
#include <NQueue.h>

//////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////

#ifndef MAX_NUM_PIPE
  #define MAX_NUM_PIPE 32
#endif
static QUEUE* s_pipes[MAX_NUM_PIPE];
static BOOL s_initialized_pipes = FALSE;

//////////////////////////////////////////////////////////////////////////

/*
====================
NPipeCreate

	create a new pipe.
====================
*/ 
I32 NPipeCreate(const CHAR* name, U32 size)
{
	QUEUE* pipe = NULL;
	U32 rounded_size = 0;
	I32 i = 0;

	// find the pipe memory
	pipe = (QUEUE*)NFindMemory(name);
	if(pipe == NULL)
	{
		// alloc the new memory for the pipe		
		pipe = (QUEUE*)NAllocMemory(name, sizeof(QUEUE)+size, &rounded_size);
		CHECK(pipe);
		pipe->size = rounded_size-sizeof(QUEUE);
		pipe->head = 0;
		pipe->tail = 1;
	}

	// add the pipe to the table
	if(s_initialized_pipes==FALSE){memset(s_pipes,0,sizeof(s_pipes));s_initialized_pipes=TRUE;}
	for(i = 0; i < MAX_NUM_PIPE; i++)
	{
		if(s_pipes[i]==NULL)
		{
			s_pipes[i] = pipe;
			return i;
		}
	}
	return -1;
}

/*
====================
NPipeDestory

	destory a pipe.
====================
*/
VOID NPipeDestory(I32 id)
{
	// free the pipe from the table
	if(s_initialized_pipes==FALSE){memset(s_pipes,0,sizeof(s_pipes));s_initialized_pipes=TRUE;}
	CHECK(id>=0&&id<MAX_NUM_PIPE&&s_pipes[id]);
	NFreeMemory(s_pipes[id]);
	s_pipes[id] = NULL;
}

/*
====================
NPipeWrite

	write the data to the pipe
====================
*/
BOOL NPipeWrite(I32 id, const VOID* buf, U32 len)
{
	CHECK(id>=0&&id<MAX_NUM_PIPE&&s_pipes[id]);
	return NQueueWrite(s_pipes[id], buf, len);	
}

/*
====================
NPipeRead

	read the data from the pipe
====================
*/ 
BOOL NPipeRead(I32 id, VOID* buf, U32* len)
{
	CHECK(id>=0&&id<MAX_NUM_PIPE&&s_pipes[id]);
	return NQueueRead(s_pipes[id], buf, len);
}

//////////////////////////////////////////////////////////////////////////