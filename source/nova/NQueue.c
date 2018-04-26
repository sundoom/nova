/*
 *  NQueue.c
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#include <NQueue.h>

//////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////

/*
====================
NQueueWrite

	write the data to the queue tail
====================
*/
BOOL NQueueWrite(QUEUE* q, const VOID* buf, U32 len)
{
	U32 free_size = 0;
	U32 real_size = 0;
	U8 *data_ptr = NULL;
	U32 data_pos = 0;

	CHECK(q&&buf&&len);

	// check the space if it is enough?
	free_size = (q->head - q->tail + q->size) % q->size;
	if(len + sizeof(U32) > free_size) return FALSE; // the free space is not enough

	// get the data
	data_ptr = (U8*)(q + 1);
	data_pos = (q->tail - 1 + q->size) % q->size;

	// write the size to the queue
	real_size = q->size - data_pos;
	if(sizeof(U32) <= real_size)
	{
		memcpy(&data_ptr[data_pos], &len, sizeof(U32));
	}
	else
	{
		// split two segment data
		memcpy(&data_ptr[data_pos], &len, real_size);
		memcpy(&data_ptr[0], (((U8*)&len)+real_size), (sizeof(U32)-real_size));
	}
	data_pos = (data_pos + sizeof(U32)) % q->size;

	// write the data to the queue
	real_size = q->size - data_pos;
	if(len <= real_size)
	{
		memcpy(&data_ptr[data_pos], buf, len);
	}
	else
	{
		// split two segment data
		memcpy(&data_ptr[data_pos], buf, real_size);
		memcpy(&data_ptr[0], (((U8*)buf)+real_size), (len-real_size));
	}

	// set the tail
	q->tail = (q->tail + sizeof(U32) + len) % q->size;

	return TRUE;
}

/*
====================
NQueueRead

	read the data from the queue head
====================
*/ 
BOOL NQueueRead(QUEUE* q, VOID* buf, U32* len)
{
	U32 data_size = 0;
	U8 *data_ptr = NULL;
	U32 data_pos = 0;
	U32 real_size = 0;
	U32 read_size = 0;

	CHECK(q&&buf&&len);

	// check if the data in the queue
	data_size = (q->tail - 1 - q->head + q->size) % q->size;
	if(data_size == 0) return FALSE;

	// get the data
	data_ptr = (U8*)(q + 1);
	data_pos = q->head;

	// read the size from the queue
	real_size = q->size - data_pos;
	if(sizeof(U32) <= real_size)
	{
		read_size = *((U32*)(data_ptr+data_pos));
	}
	else
	{
		// merge two segment data
		memcpy(&read_size, &data_ptr[data_pos], real_size);
		memcpy((((U8*)&read_size)+real_size), &data_ptr[0], (sizeof(U32)-real_size));
	}
	(*len) = read_size;
	data_pos = (data_pos + sizeof(U32)) % q->size;

	// read the data from the queue
	real_size = q->size - data_pos;
	if(read_size <= real_size)
	{
		memcpy(buf, &data_ptr[data_pos], read_size);
	}
	else
	{
		// merge two segment data
		memcpy(buf, &data_ptr[data_pos], real_size);
		memcpy((((U8*)buf)+real_size), &data_ptr[0], (read_size-real_size));
	}

	// set the head
	q->head = (q->head + sizeof(U32) + read_size) % q->size;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////