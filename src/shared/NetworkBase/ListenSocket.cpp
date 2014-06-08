/************************************************************************/
/* Copyright (C) 2006 Burlex                                            */
/************************************************************************/

/* *
   * Class ListenSocket
   * This class handles the listening/accepting of connections to the server.
   * It is only needed in a server socket, not in a client. It will request
   * Holders from the load balancer and add the sockets to the holders as
   * connections are accepted.
*/


#include "ListenSocket.h"

void InitializeWinsock()
{
#ifdef WIN32
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,0), &wsaData))
    {
        printf("[Network] Could not initialize Winsock!\n");
        exit(-1);
    } else {
        //printf("[Network] Winsock %u.%u (%s) initialized.\n",
//            wsaData.wVersion, wsaData.wHighVersion, wsaData.szDescription);
    }
#endif
}

void CleanupWinsock()
{
#ifdef WIN32
    WSACleanup();
#endif
}

