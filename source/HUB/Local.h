/*
 *  Local.h
 *  Nova
 *
 *  Created by sunjun on 11-6-14.
 *  Copyright 2011 sunstdio. All rights reserved.
 *
 */
//////////////////////////////////////////////////////////////////////////

#ifndef __Local__
#define __Local__

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

#define MAX_NUM_PIPE  64

//////////////////////////////////////////////////////////////////////////

#include <Nova.h>
#include <NMemory.h>
#include <NPipe.h>

//////////////////////////////////////////////////////////////////////////

#include <Protocol.h>

//////////////////////////////////////////////////////////////////////////

#include "App.h"

//////////////////////////////////////////////////////////////////////////

extern VOID Exit();

//////////////////////////////////////////////////////////////////////////

#endif // __Local__

//////////////////////////////////////////////////////////////////////////
