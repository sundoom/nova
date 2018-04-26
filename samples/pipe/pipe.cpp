//////////////////////////////////////////////////////////////////////////
// $Id: launcher.cpp,v 1.1 2009/04/03 12:46:11 sunjun Exp $
//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#	pragma warning(disable : 4786)	// identifier was truncated to 256 characters in the debug information
#	pragma warning(disable : 4244)  // conversion to float, possible loss of data
#	pragma warning(disable : 4699)	// creating precompiled header
#	pragma warning(disable : 4200)	// Zero-length array item at end of structure, a VC-specific extension
#	pragma warning(disable : 4100)	// unreferenced formal parameter
#	pragma warning(disable : 4514)	// unreferenced inline function has been removed
#	pragma warning(disable : 4201)	// nonstandard extension used : nameless struct/union
#	pragma warning(disable : 4710)	// inline function not expanded
#	pragma warning(disable : 4714)	// __forceinline function not expanded
#	pragma warning(disable : 4702)	// unreachable code in inline expanded function
#	pragma warning(disable : 4711)	// function selected for autmatic inlining
#	pragma warning(disable : 4725)	// Pentium fdiv bug
#	pragma warning(disable : 4127)	// Conditional expression is constant
#	pragma warning(disable : 4512)	// assignment operator could not be generated
#	pragma warning(disable : 4530)	// C++ exception handler used, but unwind semantics are not enabled
#	pragma warning(disable : 4245)	// conversion from 'enum ' to 'unsigned long', signed/unsigned mismatch
#	pragma warning(disable : 4389)	// signed/unsigned mismatch
#	pragma warning(disable : 4238)	// nonstandard extension used : class rvalue used as lvalue
#	pragma warning(disable : 4251)	// needs to have dll-interface to be used by clients of class 'ULinker'
#	pragma warning(disable : 4275)	// non dll-interface class used as base for dll-interface class
#	pragma warning(disable : 4511)	// copy constructor could not be generated
#	pragma warning(disable : 4284)	// return type is not a UDT or reference to a UDT
#	pragma warning(disable : 4355)	// this used in base initializer list
#	pragma warning(disable : 4097)	// typedef-name '' used as synonym for class-name ''
#	pragma warning(disable : 4291)	// typedef-name '' used as synonym for class-name ''
#	pragma warning(disable : 4731)	// frame pointer register 'ebp' modified by inline assembly code
#endif

#if _MSC_VER == 1400
#	pragma warning(disable : 4819)	// The file contains a character that cannot be represented in the current code page (number). Save the file in Unicode format to prevent data loss.
#endif

//////////////////////////////////////////////////////////////////////////

#ifndef __GCN__
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////////

#include <Nova.h>

//////////////////////////////////////////////////////////////////////////

#include <vector>
#include <map>
#include <string>
typedef std::string	Str;
typedef std::wstring WStr;

//////////////////////////////////////////////////////////////////////////

#include <Nova.h>

//////////////////////////////////////////////////////////////////////////
/*
// this class provides an object-oriented output thread interface.
class OutputThread : public NThread
{
public:
	// constructor
	OutputThread()
	{
		// alloc the output pipe
		mOutputPipePtr = NNEW(NPipe("test pipe", 20));
		CHECK(mOutputPipePtr);		
	}

	// destructor
	virtual ~OutputThread()
	{
		if( mOutputPipePtr != NULL ) mOutputPipePtr.release();
	}

	// begin
	VOID begin()
	{
		// start the thread
		NThread::start();
	}

	// end
	VOID end()
	{
		// end the thread
		if( alive() ) NThread::end();
	}


private:
	// the thread's run method.
	VOID run()
	{
		while( alive() )
		{
			CHAR buffer[20];
			U32 len = rand()%7;
			memset(buffer,0,sizeof(buffer));
			mOutputPipePtr->write(buffer,len);
			::Sleep(1);
		}
	}

private:
	NPtr<NPipe>mOutputPipePtr;
};

//////////////////////////////////////////////////////////////////////////

// this class provides an object-oriented input thread interface.
class InputThread : public NThread
{
public:
	// constructor
	InputThread()
	{
		// alloc the output pipe
		mInputPipePtr = NNEW(NPipe("test pipe", 20));
		CHECK(mInputPipePtr);		
	}

	// destructor
	virtual ~InputThread()
	{
		if( mInputPipePtr != NULL ) mInputPipePtr.release();
	}

	// begin
	VOID begin()
	{
		// start the thread
		NThread::start();
	}

	// end
	VOID end()
	{
		// end the thread
		if( alive() ) NThread::end();
	}


private:
	// the thread's run method.
	VOID run()
	{
		while( alive() )
		{
			CHAR buffer[20];U32 size;
			memset(buffer,0,sizeof(buffer));
			mInputPipePtr->read(buffer,size);
			::Sleep(1);
		}
	}

private:
	NPtr<NPipe> mInputPipePtr;
};

class A
{
public:
	// constructor
	A()
	{
		int a = 0;	
	}

	// destructor
	virtual ~A()
	{
		
	}

public:
	U32 mValue;
};
*/
//////////////////////////////////////////////////////////////////////////

I32 main(I32 argc, CHAR** argv)
{
	srand( (unsigned)time( NULL ) );	
/*
	// initialize the instance of the nova os
	NMemory::NOptions options;
	options.name = "Nova";
	options.size = 0x04000000;
	NMemory::instance(&options);
	BOOL restore = FALSE;
	NOS::instance(&restore);
	
	{
		A c;
		c.mValue = 100;
		NArray<A> a(10,c);
		for ( U32 i = 0; i < a.size(); i++ )
		{
			a[i].mValue = i;
		}
		NArray<A> b;
		b = a;
		b.resize(20);
	}
	

	A *buf = NNEW(A[10]); CHECK(buf);

	NStr s1;
	NStr s2;
	s1 = "aa";
	s1 += "sunkiller";
	s2 = "ki";
	U32 pos = s1.strstr(s2);
	
	const CHAR *s = s1.c_str();

	NPtr<OutputThread>output_ptr = NNEW(OutputThread); CHECK(output_ptr!=NULL);
	NPtr<InputThread>input_ptr = NNEW(InputThread); CHECK(input_ptr!=NULL);

	output_ptr->begin();
	input_ptr->begin();

	// the message loop
	MSG msg; memset(&msg,0,sizeof(msg));
	while(msg.message != WM_QUIT) 
	{
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} 
		else 
		{
			// run the nova os
			NOS::instance().run();		
		}
	}

	output_ptr->end();
	input_ptr->end();
*/
	return 0;
}

//////////////////////////////////////////////////////////////////////////

