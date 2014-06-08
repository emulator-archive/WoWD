/************************************************************************/
/* Copyright (C) 2006 Burlex                                            */
/************************************************************************/

/* * SocketDefines.h
   * Holds all required defines/includes for both win32 and *nix-based
   * systems. If you got a problem, change it here, don't make a mess
   * of other socket classes. ;)
   */

#ifndef __SOCKETDEFINES_H
#define __SOCKETDEFINES_H

/* Non-platform Specific Defines */
typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef int BOOL;

inline u16 SwapBytes(u16 src)
{
    return ( ( ( (src >> 8) & 0xFF ) ) |
        ( ( (src) & 0xFF ) << 8 ) );
}

#define TRUE 1
#define FALSE 0

#define INVALID_PTR 0

#include <Common.h>

/* Win32 Defines */
#ifdef WIN32

#define GetLastSocketError WSAGetLastError()

/* Other Platform Defines */

#else

#define TCP_NODELAY 0x1

#define SOCKET int

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#define SD_BOTH SHUT_RDWR
#define closesocket close
#define ioctlsocket ioctl
#define GetLastSocketError errno
#define WSAGetLastError() errno
#define SOCKADDR sockaddr
#define SOCKADDR_IN sockaddr
#define LPHOSTENT hostent*
#define LPIN_ADDR in_addr*
#define TRUE 1
#define FALSE 0
#define WSAEISCONN EISCONN
#define WSAENOTCONN ENOTCONN
#define WSAECONNRESET ECONNRESET
#define WSAEWOULDBLOCK EWOULDBLOCK
#define WSAENETRESET ENETRESET
#define WSAENOTSOCK ENOTSOCK
#define WSAESHUTDOWN ESHUTDOWN
#define WSAEINVAL EINVAL
#define WSAECONNABORTED ECONNABORTED

#endif

#endif
