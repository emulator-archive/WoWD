/************************************************************************/
/* Copyright (C) 2006 WoWD                                              */
/************************************************************************/

#ifndef __ASYNCNET_H
#define __ASYNCNET_H

#include "Common.h"
#include "Timer.h"
#include "Singleton.h"
#include "AsyncListenSocket.h"
#include "AsyncListenerThread.h"
#include "AsyncWorkerThread.h"
#include "../game/WowdThreads.h"
#include "../game/ThreadMgr.h"

#include <windows.h>

#define WORKTHREADS_PER_CORE 2

template <class TypeSocket, class ThreadingPolicy> 
class AsyncNet : public Singleton < AsyncNet < TypeSocket, ThreadingPolicy > >
{
public:
    
    AsyncNet()
    {
        m_CompletionPort = NULL;
        printf("[AsyncNet]: Created Overlapped I/O Interface.\n");
        _InitWinsock();
        _InitCompletionPort();
    }
    ~AsyncNet()
    {
        printf("[AsyncNet]: Shutting Down Overlapped I/O Interface.\n");
        m_socketHolder->CloseAllSockets();
        delete m_socketHolder;
        printf("[AsyncNet]: Overlapped I/O Interface Shutdown.\n");
    }
    
    inline void * GetCompletionPort() { return m_CompletionPort; }
    
    void RunNetwork();
    
    template <class T> 
    bool CreateListener(std::string host, uint16 port);
private:

    bool _InitWinsock();
    bool _InitCompletionPort();
    bool _SpawnWorkerThreads(uint32 c);

    void * m_CompletionPort;

    AsyncSocketHolder<TypeSocket, ThreadingPolicy> *m_socketHolder;
};


template <class TypeSocket, class ThreadingPolicy> 
bool AsyncNet<TypeSocket, ThreadingPolicy>::_InitWinsock()
{
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,0), &wsaData))
    {
        printf("[AsyncNet] Could not initialize Winsock!\n");
        return false;
    }

    return true;
}

template <class TypeSocket, class ThreadingPolicy> 
bool AsyncNet<TypeSocket, ThreadingPolicy>::_InitCompletionPort()
{
    m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL,
                                                (ULONG_PTR)0, 0);

    if (m_CompletionPort == NULL)
    {
        printf("[AsyncNet] Could not create a Completion Port!\n");
        return false;
    }
    m_socketHolder = new AsyncSocketHolder<TypeSocket, ThreadingPolicy>(m_CompletionPort);

    return true;
}

template <class TypeSocket, class ThreadingPolicy> 
void AsyncNet<TypeSocket, ThreadingPolicy>::RunNetwork()
{
    printf("[AsyncNet]: Starting Overlapped I/O Net Core...\n");

    SYSTEM_INFO systemInfo;
    GetSystemInfo( &systemInfo );
	uint32 c = systemInfo.dwNumberOfProcessors;

    printf("[AsyncNet]: Spawning %d Network Worker Threads (%d per Core)...\n", 
            c * WORKTHREADS_PER_CORE, 
            WORKTHREADS_PER_CORE);
    _SpawnWorkerThreads(c * WORKTHREADS_PER_CORE);
}

template <class TypeSocket, class ThreadingPolicy> 
template <class T> 
bool AsyncNet<TypeSocket, ThreadingPolicy>::CreateListener(std::string host, uint16 port)
{
    AsyncListenSocket<T> *l = new AsyncListenSocket<T>(m_CompletionPort, host, port);
    if (!l->CreateListener())
        return false;

    launch_thread( new AsyncListenerThread<T, ThreadingPolicy>(l) );
        
    printf("[AsyncNet]: Spawned Listener Thread (%s:%d).\n", host.c_str(), port);

    return true;
}

template <class TypeSocket, class ThreadingPolicy> 
bool AsyncNet<TypeSocket, ThreadingPolicy>::_SpawnWorkerThreads(uint32 c)
{
    for(int i = 0; i < c; i++)
    {
        launch_thread( new AsyncWorkerThread<TypeSocket, ThreadingPolicy>(
            m_CompletionPort, m_socketHolder) );
    }
    
    printf("[AsyncNet]: Spawned %d Network Worker Threads.\n", c);

    return true;
}


#endif
