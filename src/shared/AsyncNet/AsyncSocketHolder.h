/************************************************************************/
/* Copyright (C) 2006 WoWD                                              */
/************************************************************************/

#ifndef __ASYNCSOCKETHOLDER_H
#define __ASYNCSOCKETHOLDER_H

#include "Common.h"
#include "AsyncDefines.h"

class AsyncSocketHolderBase
{
public:
    AsyncSocketHolderBase(HANDLE cp) : m_completionPort(cp) { }
    virtual ~AsyncSocketHolderBase() { }

private:
    HANDLE m_completionPort;
};

template <class TypeSocket, class ThreadingPolicy>
class AsyncSocketHolder : public AsyncSocketHolderBase
{
public:
    AsyncSocketHolder(HANDLE cp) : AsyncSocketHolderBase(cp) { }
    ~AsyncSocketHolder() { }
    
    void AddSocket(TypeSocket *s);
    void DeleteSocket(TypeSocket *s);

    void CloseAllSockets();
    uint32 GetSocketCount();
private:
    std::set<TypeSocket *> m_sockets;
    ThreadingPolicy _mutex;
};

template <class TypeSocket, class ThreadingPolicy>
void AsyncSocketHolder<TypeSocket, ThreadingPolicy>::AddSocket(TypeSocket *s)
{
    _mutex.Acquire();
    m_sockets.insert(s);
    _mutex.Release();
}

template <class TypeSocket, class ThreadingPolicy>
void AsyncSocketHolder<TypeSocket, ThreadingPolicy>::DeleteSocket(TypeSocket *s)
{
    _mutex.Acquire();
    m_sockets.erase(s);
    _mutex.Release();
}

template <class TypeSocket, class ThreadingPolicy>
void AsyncSocketHolder<TypeSocket, ThreadingPolicy>::CloseAllSockets()
{
    printf("[AsyncNet]: Shutting Down All Sockets...\n");

    _mutex.Acquire();

    printf("[AsyncNet]: Signalling %d Socket Shutdowns...\n", m_sockets.size());
    std::set<TypeSocket *>::iterator itr;

    for(itr = m_sockets.begin(); itr != m_sockets.end(); itr++)
    {
        (*itr)->PostCompletion(IOShutdownRequest);
    }
    
    uint32 size = m_sockets.size();

    _mutex.Release();

    printf("[AsyncNet]: Waiting For Socket Shutdowns...\n");

    while (size)
    {
        _mutex.Acquire();
        size = m_sockets.size();
        _mutex.Release();

        Sleep(100);
    }

    printf("[AsyncNet]: Sockets Shutdown Successfuly\n");
}

template <class TypeSocket, class ThreadingPolicy>
uint32 AsyncSocketHolder<TypeSocket, ThreadingPolicy>::GetSocketCount()
{
    _mutex.Acquire();
    uint32 n = m_sockets.size();
    _mutex.Release();

    return n;
}

#endif