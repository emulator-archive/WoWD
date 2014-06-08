/************************************************************************/
/* Copyright (C) 2006 WoWD                                              */
/************************************************************************/

#ifndef __ASYNCWORKERTHREAD_H
#define __ASYNCWORKERTHREAD_H

#include "Common.h"
#include "Timer.h"
#include "../CrashHandler.h"
#include "../game/WowdThreads.h"
#include "../game/ThreadMgr.h"
#include "AsyncDefines.h"
#include "AsyncSocketHolder.h"
#include <map>

template <class TypeSocket, class ThreadingPolicy>
class AsyncWorkerThread : public WowdThread
{
public:
    AsyncWorkerThread(HANDLE cp, AsyncSocketHolderBase *sh);
    ~AsyncWorkerThread() { }

    virtual void run();
    void SafeRunner();
private:

    void HandleReadRequest(TypeSocket *s, int len);
    void HandleReadCompleted(TypeSocket *s, int len);
    void HandleWriteRequest(TypeSocket *s, int len);
    void HandleWriteCompleted(TypeSocket *s, int len);
    void HandleAccept(TypeSocket *s, int len);
    void HandleShutdownRequest(TypeSocket *s, int len);

    struct IOCall
    {
        std::string OpName;
        void (AsyncWorkerThread::*WorkerHandler)(TypeSocket *s, int len);
    };

    IOCall m_handlers[6];

    HANDLE m_completionPort;
    AsyncSocketHolder<TypeSocket, ThreadingPolicy> *m_socketHolder;
};

template<class TypeSocket, class ThreadingPolicy>
class DeadSocketCollector : public Singleton<DeadSocketCollector<TypeSocket, ThreadingPolicy> >
{
    std::map<TypeSocket*, uint32> deletionQueue;
    ThreadingPolicy queueLock;
    bool shutdown;
public:
    DeadSocketCollector() : shutdown(false) {}

    void QueueSocketDeletion(TypeSocket * Socket)
    {
        queueLock.Acquire();
        deletionQueue[Socket] = time(NULL) + 15;
        queueLock.Release();
    }

    void Update()
    {
        uint32 t = time(NULL);
        queueLock.Acquire();
        map<TypeSocket*, uint32>::iterator itr = deletionQueue.begin();
        map<TypeSocket*, uint32>::iterator it2;

        for(; itr != deletionQueue.end();)
        {
            it2 = itr;
            ++itr;
            if(it2->second <= t || shutdown)
            {
                it2->first->Delete();
                deletionQueue.erase(it2);
            }
        }

        queueLock.Release();
    }
    
    void Shutdown()
    {
        shutdown = true;
        while(deletionQueue.size())
            Update();
    }
};

#define sDeadSocketCollector DeadSocketCollector<BaseSocket, Mutex>::getSingleton()

template <class TypeSocket, class ThreadingPolicy>
void AsyncWorkerThread<TypeSocket, ThreadingPolicy>::HandleReadRequest(TypeSocket *s, int len)
{
    if(s->_deleted == 0)
        s->UpdateRead();
}

template <class TypeSocket, class ThreadingPolicy>
void AsyncWorkerThread<TypeSocket, ThreadingPolicy>::HandleWriteRequest(TypeSocket *s, int len)
{
    if(s->_deleted == 0)
        s->UpdateWrite();
}

template <class TypeSocket, class ThreadingPolicy>
void AsyncWorkerThread<TypeSocket, ThreadingPolicy>::HandleReadCompleted(TypeSocket *s, int len)
{
    if(s->_deleted == 0)
    {
        if (len)
        {
            s->UseReadBuffer(len);
            s->OnReceive(len);
            s->PostCompletion(IOReadRequest);
        }
        else
        {
            s->Disconnect();
            s->PostCompletion(IOShutdownRequest);
        }
    }
}

template <class TypeSocket, class ThreadingPolicy>
void AsyncWorkerThread<TypeSocket, ThreadingPolicy>::HandleWriteCompleted(TypeSocket *s, int len)
{
    if(s->_deleted == 0)
    {
        if (len)
            s->UseSendBuffer(len);

        if (s->HasSend() && s->IsConnected())
            s->PostCompletion(IOWriteRequest);
        else
            s->DecWriteLock();
    }
}

template <class TypeSocket, class ThreadingPolicy>
void AsyncWorkerThread<TypeSocket, ThreadingPolicy>::HandleAccept(TypeSocket *s, int len)
{
    m_socketHolder->AddSocket(s);

    s->OnConnect();
    s->UpdateRead();
}

template <class TypeSocket, class ThreadingPolicy>
void AsyncWorkerThread<TypeSocket, ThreadingPolicy>::HandleShutdownRequest(TypeSocket *s, int len)
{
    m_socketHolder->DeleteSocket(s);
    s->PostDeletionCompletion();
}

template <class TypeSocket, class ThreadingPolicy>
AsyncWorkerThread<TypeSocket, ThreadingPolicy>::AsyncWorkerThread(HANDLE cp, AsyncSocketHolderBase *sh)
{
    m_completionPort = cp;
    m_socketHolder = static_cast< AsyncSocketHolder<TypeSocket, ThreadingPolicy> *>(sh);

    m_handlers[0].WorkerHandler = &AsyncWorkerThread::HandleReadRequest;
    m_handlers[1].WorkerHandler = &AsyncWorkerThread::HandleReadCompleted;
    m_handlers[2].WorkerHandler = &AsyncWorkerThread::HandleWriteRequest;
    m_handlers[3].WorkerHandler = &AsyncWorkerThread::HandleWriteCompleted;
    m_handlers[4].WorkerHandler = &AsyncWorkerThread::HandleAccept;
    m_handlers[5].WorkerHandler = &AsyncWorkerThread::HandleShutdownRequest;
}


template <class TypeSocket, class ThreadingPolicy>
void AsyncWorkerThread<TypeSocket, ThreadingPolicy>::run()
{
    SetThreadName("Network Worker Thread");

    WOWD_THREAD_TRY_EXECUTION2

    SafeRunner();

    WOWD_THREAD_HANDLE_CRASH2
}

template <class TypeSocket, class ThreadingPolicy>
void AsyncWorkerThread<TypeSocket, ThreadingPolicy>::SafeRunner()
{
    TypeSocket *ts;
    uint32 len;
    OverLapped *olclass;
    LPOVERLAPPED ol;
    IOCall * ioc;

    ThreadState = WOWD_THREADSTATE_BUSY;

    while(ThreadState != WOWD_THREADSTATE_TERMINATE)
    {        
        if(!GetQueuedCompletionStatus(m_completionPort, &len , (LPDWORD) &ts, &ol, 5000))
            continue;

        olclass = CONTAINING_RECORD(ol, OverLapped, m_ol);
        ioc = &m_handlers[olclass->m_io];
        (this->*ioc->WorkerHandler)(ts, len);
        
        delete olclass;
    }
}

#endif


