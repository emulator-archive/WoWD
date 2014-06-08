#ifdef WIN32

/************************************************************************/
/* Copyright (C) 2006 Burlex                                            */
/************************************************************************/

/* *
   @file CrashHandler.h
   Handles crashes/exceptions on a win32 based platform, writes a dump file,
   for later bug fixing.
*/
#  pragma warning( disable : 4311 )

#include "CrashHandler.h"
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <tchar.h>

bool ON_CRASH_BREAK_DEBUGGER;

void StartCrashHandler()
{
    // Firstly, check if there is a debugger present. There isn't any point in
    // handling crashes internally if we have a debugger attached, that would 
    // just piss us off. :P

    DWORD code;

    // Check for a debugger.
    __asm
    {
        MOV EAX, FS:[0x18]
        MOV EAX, DWORD PTR [EAX + 0x30]
        MOV ECX, DWORD PTR [EAX]
        MOV [DWORD PTR code], ECX
    }

    if(code & 0x00010000)
    {
        // We got a debugger. We'll tell wowd to not exit on a crash but instead break into debugger.
        ON_CRASH_BREAK_DEBUGGER = true;
    }
    else
    {
        // No debugger. On crash, we'll call OnCrash to save etc.
        ON_CRASH_BREAK_DEBUGGER = false;
    }
    //ON_CRASH_BREAK_DEBUGGER = IsDebuggerPresent();
}


///////////////////////////////////////////////////////////////////////////////
// GetExceptionDescription
// Translate the exception code into something human readable
static const TCHAR *GetExceptionDescription(DWORD ExceptionCode)
{
    struct ExceptionNames
    {
        DWORD	ExceptionCode;
        TCHAR *	ExceptionName;
    };

#if 0  // from winnt.h
#define STATUS_WAIT_0                    ((DWORD   )0x00000000L)    
#define STATUS_ABANDONED_WAIT_0          ((DWORD   )0x00000080L)    
#define STATUS_USER_APC                  ((DWORD   )0x000000C0L)    
#define STATUS_TIMEOUT                   ((DWORD   )0x00000102L)    
#define STATUS_PENDING                   ((DWORD   )0x00000103L)    
#define STATUS_SEGMENT_NOTIFICATION      ((DWORD   )0x40000005L)    
#define STATUS_GUARD_PAGE_VIOLATION      ((DWORD   )0x80000001L)    
#define STATUS_DATATYPE_MISALIGNMENT     ((DWORD   )0x80000002L)    
#define STATUS_BREAKPOINT                ((DWORD   )0x80000003L)    
#define STATUS_SINGLE_STEP               ((DWORD   )0x80000004L)    
#define STATUS_ACCESS_VIOLATION          ((DWORD   )0xC0000005L)    
#define STATUS_IN_PAGE_ERROR             ((DWORD   )0xC0000006L)    
#define STATUS_INVALID_HANDLE            ((DWORD   )0xC0000008L)    
#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)    
#define STATUS_ILLEGAL_INSTRUCTION       ((DWORD   )0xC000001DL)    
#define STATUS_NONCONTINUABLE_EXCEPTION  ((DWORD   )0xC0000025L)    
#define STATUS_INVALID_DISPOSITION       ((DWORD   )0xC0000026L)    
#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((DWORD   )0xC000008CL)    
#define STATUS_FLOAT_DENORMAL_OPERAND    ((DWORD   )0xC000008DL)    
#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((DWORD   )0xC000008EL)    
#define STATUS_FLOAT_INEXACT_RESULT      ((DWORD   )0xC000008FL)    
#define STATUS_FLOAT_INVALID_OPERATION   ((DWORD   )0xC0000090L)    
#define STATUS_FLOAT_OVERFLOW            ((DWORD   )0xC0000091L)    
#define STATUS_FLOAT_STACK_CHECK         ((DWORD   )0xC0000092L)    
#define STATUS_FLOAT_UNDERFLOW           ((DWORD   )0xC0000093L)    
#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((DWORD   )0xC0000094L)    
#define STATUS_INTEGER_OVERFLOW          ((DWORD   )0xC0000095L)    
#define STATUS_PRIVILEGED_INSTRUCTION    ((DWORD   )0xC0000096L)    
#define STATUS_STACK_OVERFLOW            ((DWORD   )0xC00000FDL)    
#define STATUS_CONTROL_C_EXIT            ((DWORD   )0xC000013AL)    
#define STATUS_FLOAT_MULTIPLE_FAULTS     ((DWORD   )0xC00002B4L)    
#define STATUS_FLOAT_MULTIPLE_TRAPS      ((DWORD   )0xC00002B5L)    
#define STATUS_ILLEGAL_VLM_REFERENCE     ((DWORD   )0xC00002C0L)     
#endif

    ExceptionNames ExceptionMap[] =
    {
        {0x40010005, _T("a Control-C")},
        {0x40010008, _T("a Control-Break")},
        {0x80000002, _T("a Datatype Misalignment")},
        {0x80000003, _T("a Breakpoint")},
        {0xc0000005, _T("an Access Violation")},
        {0xc0000006, _T("an In Page Error")},
        {0xc0000017, _T("a No Memory")},
        {0xc000001d, _T("an Illegal Instruction")},
        {0xc0000025, _T("a Noncontinuable Exception")},
        {0xc0000026, _T("an Invalid Disposition")},
        {0xc000008c, _T("a Array Bounds Exceeded")},
        {0xc000008d, _T("a Float Denormal Operand")},
        {0xc000008e, _T("a Float Divide by Zero")},
        {0xc000008f, _T("a Float Inexact Result")},
        {0xc0000090, _T("a Float Invalid Operation")},
        {0xc0000091, _T("a Float Overflow")},
        {0xc0000092, _T("a Float Stack Check")},
        {0xc0000093, _T("a Float Underflow")},
        {0xc0000094, _T("an Integer Divide by Zero")},
        {0xc0000095, _T("an Integer Overflow")},
        {0xc0000096, _T("a Privileged Instruction")},
        {0xc00000fD, _T("a Stack Overflow")},
        {0xc0000142, _T("a DLL Initialization Failed")},
        {0xe06d7363, _T("a Microsoft C++ Exception")},
    };

    for (int i = 0; i < sizeof(ExceptionMap) / sizeof(ExceptionMap[0]); i++)
        if (ExceptionCode == ExceptionMap[i].ExceptionCode)
            return ExceptionMap[i].ExceptionName;

    return _T("an Unknown exception type");
}

void PrintCrashInformation(PEXCEPTION_POINTERS except)
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    TCHAR username[200];
    DWORD usize = 198;
    if(!GetUserName(username, &usize))
        strcpy(username, "Unknown");
    TCHAR winver[200];
    OSVERSIONINFO ver;
    ver.dwOSVersionInfoSize = sizeof(ver);
    if(GetVersionEx(&ver) == 0)
    {
        ver.dwBuildNumber = 0;
        ver.dwMajorVersion = 5;
        ver.dwMinorVersion = 1;
    }
    MEMORYSTATUS mi;
    mi.dwLength = sizeof(mi);
    GlobalMemoryStatus(&mi);

    if(ver.dwMajorVersion == 5 && ver.dwMinorVersion == 0)
        strcpy(winver, "Windows 2000");
    else if(ver.dwMajorVersion == 5 && ver.dwMinorVersion == 1)
        strcpy(winver, "Windows XP");
    else if(ver.dwMajorVersion == 5 && ver.dwMajorVersion >= 2)
        strcpy(winver, "Windows 2003");
    else if(ver.dwMajorVersion >= 5 && ver.dwMinorVersion >= 0)
        strcpy(winver, "Windows Vista");
    else
        strcpy(winver, "Unknown Windows");

    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    printf("WoWd has crashed. Reason was:\n");
    printf("   %s at 0x%08X\n", GetExceptionDescription(except->ExceptionRecord->ExceptionCode),
        (unsigned long)except->ExceptionRecord->ExceptionAddress);
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    printf("System Information:\n");
    printf("   Running as: %s on %s Build %u\n", username, winver, ver.dwBuildNumber);
    printf("   Running on %u processors (type %u)\n", si.dwNumberOfProcessors, si.dwProcessorType);
    printf("   Physical Memory Total: %u\n", mi.dwTotalPhys);
    printf("   Physical Memory Free: %u\n", mi.dwAvailPhys);
    printf("   Page File Total: %u\n", mi.dwTotalPageFile);
    printf("   Page File Free: %u\n", mi.dwAvailPageFile);
    printf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
}


int __cdecl HandleCrash(PEXCEPTION_POINTERS pExceptPtrs)
{
    if(pExceptPtrs == 0)
    {
        // Raise an exception :P
        __try
        {
            RaiseException(EXCEPTION_BREAKPOINT, 0, 0, 0);
        }
        __except(HandleCrash(GetExceptionInformation()), EXCEPTION_CONTINUE_EXECUTION)
        {

        }        
    }

    // Create the date/time string
    time_t curtime = time(NULL);
    tm * pTime = localtime(&curtime);
    char filename[MAX_PATH];
    TCHAR modname[MAX_PATH*2];
    ZeroMemory(modname, sizeof(modname));
    if(GetModuleFileName(0, modname, MAX_PATH*2-2) <= 0)
        strcpy(modname, "UNKNOWN");

    char * mname = strrchr(modname, '\\');
    (void*)mname++;     // Remove the last \

    sprintf(filename, "CrashDumps\\dump-%s-%u-%u-%u-%u-%u-%u.dmp",
        mname, pTime->tm_year+1900, pTime->tm_mon, pTime->tm_mday,
        pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
    

    HANDLE hDump = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);

    if(hDump == INVALID_HANDLE_VALUE)
    {
        // Create the directory first
        CreateDirectory("CrashDumps", 0);
        hDump = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);
    }

    PrintCrashInformation(pExceptPtrs);
    printf("\x7\nCreating crash dump file %s\n", filename);
    
    if(hDump == INVALID_HANDLE_VALUE)
    {
        MessageBox(0, "Could not open crash dump file.", "Crash dump error.", MB_OK);
    }
    else
    {
        MINIDUMP_EXCEPTION_INFORMATION info;
        info.ClientPointers = FALSE;
        info.ExceptionPointers = pExceptPtrs;
        info.ThreadId = GetCurrentThreadId();


        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
            hDump, MiniDumpNormal, &info, 0, 0);

        CloseHandle(hDump);
    }

    OnCrash(!ON_CRASH_BREAK_DEBUGGER);      

    return EXCEPTION_CONTINUE_SEARCH;
}
#endif