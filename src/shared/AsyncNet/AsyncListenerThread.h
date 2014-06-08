/************************************************************************/
/* Copyright (C) 2006 WoWD                                              */
/************************************************************************/

#ifndef __ASYNCLISTENERTHREAD_H
#define __ASYNCLISTENERTHREAD_H

#include "Common.h"
#include "Timer.h"
#include "AsyncListenSocket.h"
#include "../CrashHandler.h"
#include "../game/WowdThreads.h"
#include "../game/ThreadMgr.h"

template <class TypeSocket, class ThreadingPolicy>
class AsyncListenerThread : public WowdThread
{
public:
    AsyncListenerThread(AsyncListenSocket<TypeSocket>* s) : m_ls(s) { }
    ~AsyncListenerThread() { }
    
    virtual void run();
    void SafeRunner();
private:
    
    AsyncListenSocket<TypeSocket>* m_ls;
};

template <class TypeSocket, class ThreadingPolicy>
void AsyncListenerThread<TypeSocket, ThreadingPolicy>::SafeRunner()
{
    while(ThreadState != WOWD_THREADSTATE_TERMINATE)
    {
        if(ThreadState == WOWD_THREADSTATE_TERMINATE)
            return;

        ThreadState = WOWD_THREADSTATE_BUSY;
        
        m_ls->Listen();
        
        Sleep(50);

        if(ThreadState == WOWD_THREADSTATE_TERMINATE)
            return;
    }
}


template <class TypeSocket, class ThreadingPolicy>
void AsyncListenerThread<TypeSocket, ThreadingPolicy>::run()
{
    SetThreadName("Network Listener Thread");

    WOWD_THREAD_TRY_EXECUTION2

    SafeRunner();

    WOWD_THREAD_HANDLE_CRASH2
}

#endif
