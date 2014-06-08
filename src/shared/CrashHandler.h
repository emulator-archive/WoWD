

/************************************************************************/
/* Copyright (C) 2006 Burlex                                            */
/************************************************************************/

/* *
   @file CrashHandler.h
   Handles crashes/exceptions on a win32 based platform, writes a dump file,
   for later bug fixing.
*/

#ifndef _CRASH_HANDLER_H
#define _CRASH_HANDLER_H

#ifdef WIN32

#include <Windows.h>
#include <DbgHelp.h>

void StartCrashHandler();
void OnCrash(bool Terminate);

typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;
int __cdecl HandleCrash(PEXCEPTION_POINTERS pExceptPtrs);

#define WOWD_THREAD_TRY_EXECUTION __try 
#define WOWD_THREAD_HANDLE_CRASH  __except(HandleCrash(GetExceptionInformation())) {}

#define WOWD_THREAD_TRY_EXECUTION2 __try {
#define WOWD_THREAD_HANDLE_CRASH2  } __except(HandleCrash(GetExceptionInformation())) {}

#else

// We dont wanna confuse nix ;p
#define WOWD_THREAD_TRY_EXECUTION 
#define WOWD_THREAD_HANDLE_CRASH 

#define WOWD_THREAD_TRY_EXECUTION2 ;
#define WOWD_THREAD_HANDLE_CRASH2 ;

#endif

#endif

