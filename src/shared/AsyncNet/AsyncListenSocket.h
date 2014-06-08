/************************************************************************/
/* Copyright (C) 2006 WoWD                                              */
/************************************************************************/

#ifndef __ASYNCLISTENSOCKET_H
#define __ASYNCLISTENSOCKET_H

#include "Common.h"
#include "AsyncDefines.h"
#include "AsyncSocketHolder.h"

template <class T>
class AsyncListenSocket
{
public:
    AsyncListenSocket(HANDLE cp, std::string host, uint16 port) : m_completionPort(cp), 
        m_host(host), m_port(port) 
    { 
        m_socket = NULL;
        m_tmpsize = sizeof(sockaddr_in);
    }

    ~AsyncListenSocket()
    {
        if (m_socket != NULL)
        {
            shutdown(m_socket, SD_BOTH);
            closesocket(m_socket);
        }
    }
    
    bool CreateListener();
    void Listen();
private:

    SOCKET m_socket;
    HANDLE m_completionPort;

    struct sockaddr_in m_address;
    
    struct sockaddr_in m_tmpaddy;
    int m_tmpsize;
    
    std::string m_host;
    uint16 m_port;
};

template <class T>
bool AsyncListenSocket<T>::CreateListener()
{
        m_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
        if(m_socket == INVALID_SOCKET)
        {
            printf("[AsyncListenSocket]: Failed to create socket.\n");
            return false;
        }

        // Create listen address
        m_address.sin_family = AF_INET;
        m_address.sin_addr.s_addr = htonl(INADDR_ANY);
        if(!(m_host == "0.0.0.0" || m_host.length() < 7))
        {
            struct hostent * hostname = gethostbyname(m_host.c_str());
            if(hostname == NULL)
            {
                printf("[AsyncListenSocket]: Could not translate host for: %s\n", m_host.c_str());                
            } 
            else 
            {
                memcpy(&m_address.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);
            }
        }

        m_address.sin_port = htons(m_port);

        int res = bind(m_socket, (const sockaddr*)&m_address, sizeof(m_address));

        if(res != 0) 
        {
            printf("[AsyncListenSocket]: Could not bind to the specified interface/port. (%s:%u)\n",
                inet_ntoa(m_address.sin_addr), m_port);
            int err = WSAGetLastError();
            return false;
        }

        res = listen(m_socket, 5);
        if(res != 0) 
        {
            printf("[AsyncListenSocket]: Could not listen on port %u\n", m_port);
            int err = WSAGetLastError();
            return false;
        }

        uint32 option = 1;
        if(setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&option, 4))
            printf("[AsyncListenSocket]: Could not set SO_REUSERADDR.\n");

        if(ioctlsocket(m_socket, FIONBIO, &option) == SOCKET_ERROR)
            printf("[AsyncListenSocket]: Could not disable blocking mode on accept socket.\n");

        return true;
}

template <class T>
void AsyncListenSocket<T>::Listen()
{
    int dummy; // used to fool base socket since it won't be used anyhow ;/ 
               // yeah...i'm lazy  - Doron

    SOCKET s = WSAAccept(m_socket, (struct sockaddr*)&m_tmpaddy, (socklen_t*)&m_tmpsize,
                            NULL, NULL);
    if(s == INVALID_SOCKET)
    {
        int err = WSAGetLastError();
        if(err != WSAEWOULDBLOCK)
        {
            printf("[AsyncListenSocket] Encountered error %u.\n", err);
            return;
        }
        else
        {
            return;
        }
    }

    T *Socket = new T(reinterpret_cast<SocketHolder&>(dummy));
    Socket->Create(s, &m_tmpaddy);
    Socket->SetCP(m_completionPort);

    if (CreateIoCompletionPort((HANDLE)s, m_completionPort, (DWORD)Socket, 0) == NULL)
    {
            printf("[AsyncListenSocket] Could not assign socket to completion port");
            closesocket(s);
            return;
    }

    Socket->PostCompletion(IOAccept);
}

#endif
