// Copyright (C) 2004 WoW Daemon
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef WOWSERVER_COMMON_H
#define WOWSERVER_COMMON_H

#ifdef WIN32
#pragma warning(disable:4996)
#endif

// Precaching Enable / Disable
#define PRECACHE_LOAD


enum TimeVariables
{
    TIME_SECOND = 1,
    TIME_MINUTE = TIME_SECOND * 60,
    TIME_HOUR   = TIME_MINUTE * 60,
    TIME_DAY    = TIME_HOUR * 24,
};

enum MsTimeVariables
{
    MSTIME_SECOND = 1000,
    MSTIME_MINUTE = MSTIME_SECOND * 60,
    MSTIME_HOUR   = MSTIME_MINUTE * 60,
    MSTIME_DAY    = MSTIME_HOUR * 24,
};

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <errno.h>

// current platform and compiler
#define PLATFORM_WIN32 0
#define PLATFORM_UNIX  1
#define PLATFORM_APPLE 2

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#  define PLATFORM PLATFORM_WIN32
#elif defined( __APPLE_CC__ )
#  define PLATFORM PLATFORM_APPLE
#else
#  define PLATFORM PLATFORM_UNIX
#endif

#define COMPILER_MICROSOFT 0
#define COMPILER_GNU       1
#define COMPILER_BORLAND   2

#ifdef _MSC_VER
#  define COMPILER COMPILER_MICROSOFT
#elif defined( __BORLANDC__ )
#  define COMPILER COMPILER_BORLAND
#elif defined( __GNUC__ )
#  define COMPILER COMPILER_GNU
#else
#  pragma error "FATAL ERROR: Unknown compiler."
#endif

#if COMPILER == COMPILER_MICROSOFT
#  pragma warning( disable : 4267 ) // conversion from 'size_t' to 'int', possible loss of data
#  pragma warning( disable : 4311 ) // 'type cast': pointer truncation from HMODULE to uint32
#  pragma warning( disable : 4786 ) // identifier was truncated to '255' characters in the debug information
#  pragma warning( disable : 4146 )
#  pragma warning( disable : 4800 )
#endif

#if PLATFORM == PLATFORM_WIN32
#define STRCASECMP stricmp
#define _WIN32_WINNT 0x0500
#else
#define STRCASECMP strcasecmp
#endif

#if PLATFORM == PLATFORM_WIN32
    #define ASYNC_NET
#endif

#include <set>
#include <list>
#include <string>
#include <map>
#include <queue>
#include <sstream>
#include <algorithm>
//#include <iostream>

#if defined (__GNUC__)
#  define GCC_VERSION (__GNUC__ * 10000 \
                       + __GNUC_MINOR__ * 100 \
                       + __GNUC_PATCHLEVEL__)
#endif

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
//#  include <windows.h>
#else
#  include <string.h>
#  define MAX_PATH 1024
#  if defined (__GNUC__)
#    if GCC_VERSION >= 30400
#      define __fastcall __attribute__((__fastcall__))
#    else
#      define __fastcall __attribute__((__regparm__(3)))
#    endif
#  else
#    define __fastcall __attribute__((__fastcall__))
#  endif
#endif

#if COMPILER == COMPILER_GNU && __GNUC__ >= 3
#include <ext/hash_map>
#include <ext/hash_set>
#else
#include <hash_map>
#include <hash_set>
#endif

#undef FD_SETSIZE
#define FD_SETSIZE 200  // 200 per thread should be plenty :p

#if PLATFORM == PLATFORM_WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#endif

#ifdef _STLPORT_VERSION
#define HM_NAMESPACE std
using std::hash_map;
using std::hash_set;
#elif COMPILER == COMPILER_MICROSOFT && _MSC_VER >= 1300
#define HM_NAMESPACE stdext
using stdext::hash_map;
using stdext::hash_set;
#elif COMPILER == COMPILER_INTEL
#define HM_NAMESPACE std
using std::hash_map;
using std::hash_set;
#elif COMPILER == COMPILER_GNU && __GNUC__ >= 3
#define HM_NAMESPACE __gnu_cxx
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_set;

namespace __gnu_cxx
{
    template<> struct hash<unsigned long long>
    {
        size_t operator()(const unsigned long long &__x) const { return (size_t)__x; }
    };
    template<typename T> struct hash<T *>
    {
        size_t operator()(T * const &__x) const { return (size_t)__x; }
    };

};

#else
#define HM_NAMESPACE std
using std::hash_map;
#endif

#if COMPILER == COMPILER_MICROSOFT
  typedef __int64   int64;
#else
  typedef long long int64;
#endif
typedef long        int32;
typedef short       int16;
typedef char        int8;

#if COMPILER == COMPILER_MICROSOFT
  typedef unsigned __int64   uint64;
  typedef __int64            int64;
#else
  typedef unsigned long long uint64;
  typedef long long          int64;
  typedef unsigned long      DWORD;
#endif

typedef unsigned long        uint32;
typedef unsigned short       uint16;
typedef unsigned char        uint8;

/* 
Scripting system exports/imports
*/

#ifdef WIN32
    #ifndef WOWD_SCRIPTLIB
        #define WOWD_SERVER_DECL __declspec(dllexport)
        #define WOWD_SCRIPT_DECL __declspec(dllimport)
    #else
        #define WOWD_SERVER_DECL __declspec(dllimport)
        #define WOWD_SCRIPT_DECL __declspec(dllexport)
    #endif
#else
    #define WOWD_SERVER_DECL 
    #define WOWD_SCRIPT_DECL 
#endif

// Include all threading files
#include <assert.h>
#include "Threading/Threading.h"

#include "MersenneTwister.h"

#if COMPILER == COMPILER_MICROSOFT

#define I64FMT "%016I64X"
#define I64FMTD "%I64u"
#define SI64FMTD "%I64d"
#define snprintf _snprintf
#define atoll __atoi64

#else

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define I64FMT "%016llX"
#define I64FMTD "%llu"
#define SI64FMTD "%lld"

#endif

#define GUID_HIPART(x) (*(((uint32*)&(x))+1))
#define GUID_LOPART(x) (*((uint32*)&(x)))
#define UINT32_HIPART(x) (*(((uint16*)&(x))+1))
#define UINT32_LOPART(x) (*((uint16*)&(x)))

#define atol(a) strtoul( a, NULL, 10)

#define STRINGIZE(a) #a

// fix buggy MSVC's for variable scoping to be reliable =S
#define for if(true) for

#ifdef GNL_BIG_ENDIAN
#  define GNL_LOWER_WORD_BYTE    4
#else
#  define GNL_LOWER_WORD_BYTE    0
#endif

#define GNL_LONG_AT_BYTE(x,b)    *(long *)(((char *)&x) + b)
#define FIST_MAGIC_QROUND (((65536.0 * 65536.0 * 16.0) + (65536.0 * 0.5)) * 65536.0)


/// Round a floating-point value and convert to integer
static inline long QRound (double inval)
{
    double dtemp = FIST_MAGIC_QROUND + inval;
    return GNL_LONG_AT_BYTE (dtemp, GNL_LOWER_WORD_BYTE) - 0x80000000;
}

/// Convert a float to a cross-platform 32-bit format (no endianess adjustments!)
static inline long float2long (float f)
{
  int exp;
  long mant = QRound (frexp (f, &exp) * 0x1000000);
  long sign = mant & 0x80000000;
  if (mant < 0) mant = -mant;
  if (exp > 63) exp = 63; else if (exp < -64) exp = -64;
  return sign | ((exp & 0x7f) << 24) | (mant & 0xffffff);
}

/// Convert a 32-bit cross-platform float to native format (no endianess adjustments!)
static inline float long2float (long l)
{
  int exp = (l >> 24) & 0x7f;
  if (exp & 0x40) exp = exp | ~0x7f;
  float mant = float (l & 0x00ffffff) / 0x1000000;
  if (l & 0x80000000) mant = -mant;
  return (float) ldexp (mant, exp);
}

#ifndef max
#define max(a,b) ((a)>(b))?(a):(b)
#define min(a,b) ((a)<(b))?(a):(b)
#endif

#ifndef WIN32
#include <sys/timeb.h>
#endif

inline uint32 now()
{    
#ifdef WIN32   
return timeGetTime();    
#else    
struct timeb tp;    
ftime(&tp);    
return  tp.time * 1000 + tp.millitm;    
#endif
}

#ifndef WIN32
#define FALSE   0
#define TRUE    1
#endif

#ifndef WIN32
#define Sleep(ms) usleep(1000*ms)
#endif

#ifdef WIN32
#ifndef __SHOW_STUPID_WARNINGS__
#pragma warning(disable:4018)
#pragma warning(disable:4244)
#pragma warning(disable:4305) 
#pragma warning(disable:4748)
#pragma warning(disable:4800) 
#pragma warning(disable:4996)
#pragma warning(disable:4251)
#endif      
#endif


#include "Util.h"

#endif
