/*
 *  NMemory.c
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
	#include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////////

#include <NMemory.h>

//////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////

#pragma pack (push,1)

typedef struct _MEMPAGE
{
	HANDLE	handle;
	VOID*	ptr;
}MEMPAGE;

#pragma pack (pop)

//////////////////////////////////////////////////////////////////////////

#define MAX_NUM_PAGE	32
static MEMPAGE s_pages[MAX_NUM_PAGE];
static BOOL s_initialized_pages = FALSE;

//////////////////////////////////////////////////////////////////////////

/*
====================
NAllocMemory
====================
*/ 
VOID* NAllocMemory(const CHAR* name, U32 size, U32* rounded_size)
{
	U32 os_page_size = 0, page_size = 0;
	HANDLE handle = NULL;
	U32*ptr = NULL;
	U32 i = 0;
	
	// alloc the new page
#ifdef _WIN32
	SYSTEM_INFO sys_info;
	GetSystemInfo (&sys_info);
	os_page_size = sys_info.dwPageSize;
	page_size = (sizeof(U32) + size + (os_page_size - 1)) & ~(os_page_size - 1);	
	(*rounded_size) = page_size - sizeof(U32);
	CHECK(name);
	handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE|SEC_COMMIT, 0, page_size, name);
	if(handle == NULL) return 0;
	ptr = (U32*)MapViewOfFile(handle, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
	if(ptr == NULL) return 0;
	*ptr = page_size;
#else
	os_page_size = getpagesize();
#endif

	// add the page to the table
	if(s_initialized_pages==FALSE){memset(s_pages,0,sizeof(s_pages));s_initialized_pages=TRUE;}
	for(i = 0; i < MAX_NUM_PAGE; i++)
	{
		MEMPAGE* page = &s_pages[i];		
		if(page->handle==NULL)
		{
			page->handle = handle;
			page->ptr = ptr;
			return ptr+1;
		}
	}
	return 0;
}

/*
====================
NFindMemory
====================
*/
VOID* NFindMemory(const CHAR* name)
{
	HANDLE handle = NULL;
	U32*ptr = NULL;
	U32 i = 0;

	// check the name
	CHECK(name);

	// open the page
#ifdef _WIN32
	handle = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE, TRUE, name);
	if(handle == NULL) return 0;
	ptr = (U32*)MapViewOfFile(handle, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
	if(ptr == NULL) return 0;
#endif

	// add the page to the table
	if(s_initialized_pages==FALSE){memset(s_pages,0,sizeof(s_pages));s_initialized_pages=TRUE;}
	for(i = 0; i < MAX_NUM_PAGE; i++)
	{
		MEMPAGE* page = &s_pages[i];		
		if(page->handle==NULL)
		{
			page->handle = handle;
			page->ptr = ptr;
			return ptr+1;
		}
	}
	return 0;
}

/*
====================
NFreeMemory
====================
*/
VOID NFreeMemory(VOID* p)
{
	U32 i = 0;
	U32*ptr = (U32*)p;

	// check the name
	CHECK(ptr);

	// free the page from the table
	if(s_initialized_pages==FALSE){memset(s_pages,0,sizeof(s_pages));s_initialized_pages=TRUE;}
	for(i = 0; i < MAX_NUM_PAGE; i++)
	{
		MEMPAGE* page = &s_pages[i];
		if(page->ptr==(ptr-1))
		{
			if(page->ptr){CHECK(UnmapViewOfFile(page->ptr)); page->ptr=NULL;}
			if(page->handle){CHECK(CloseHandle(page->handle)); page->handle=NULL;}
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////