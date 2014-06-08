/************************************************************************/
/* Copyright (C) 2006 Burlex                                            */
/************************************************************************/

/* *
   * Class SocketHolder
   * Temporary storage solution for sockets to update them, updated by a
   * seperate thread. Sockets are dynamically allocated to the Holders via
   * SocketLoadBalancer evenly.

   * Class SocketHolderThread
   * Simply the worker thread for socket update. Calls Socket->Update,
   * pretty easy stuff :P Thread delays are customizable.

   * Class SocketLoadBalancer
   * Storage container for all socketholders for that part of the server.
   * It will dynamically allocate new sockets to a holder based on the load
   * of the stored holders. The amount of threads/holders is customizable.

*/

#include "SocketHolder.h"
#include "BaseSocket.h"
#include "../CrashHandler.h"

void SocketHolder::AddSocket(BaseSocket* Socket)
{
    Sockets.insert(Socket);
}

void SocketHolder::RemoveSocket(BaseSocket* Socket)
{
    Sockets.erase(Socket);
}

SocketHolderThread::SocketHolderThread(SocketHolder& Holder, u32 Thread_Delay) : WowdThread(), _Holder(Holder)
{
    ThreadDelay = Thread_Delay;
    ThreadType = WOWD_THREADTYPE_NETWORK;
    _Holder.SetThread(this);
}

SocketHolderThread::~SocketHolderThread()
{
    _Holder.SetThread(NULL);
}

void SocketHolderThread::run()
{
    SetThreadName("Network Thread");
    WOWD_THREAD_TRY_EXECUTION2

    Run();

    WOWD_THREAD_HANDLE_CRASH2
}

void SocketHolderThread::Run()
{
    /*fd_set slist;*/

    std::set<BaseSocket*>::iterator iter;
    std::set<BaseSocket*>::iterator iter_end;
    BaseSocket *Socket;
    uint32 start_time;
    int32 etime;

    while(ThreadState != WOWD_THREADSTATE_TERMINATE)
    {
        if(ThreadState == WOWD_THREADSTATE_TERMINATE)
            return;

        if(_Holder.Count() == 0)
        {
            Sleep(50); // sleep when we got no sockets.
            continue;
        }
        start_time = getMSTime();

        iter = _Holder.Begin();
        iter_end = _Holder.End();

        // check the fd set for sockets which have had activity
        iter = _Holder.Begin();
        iter_end = _Holder.End();
        
        while(iter != iter_end)
        {
            Socket = (*iter);
            ++iter;
            if(!Socket->UpdateWrite())
                continue;

            Socket->UpdateRead();
        }
        etime = getMSTime() - start_time;
        if(etime < ThreadDelay)
            Sleep(ThreadDelay-etime);


        if(ThreadState == WOWD_THREADSTATE_TERMINATE)
            return;

    }
}

void SocketLoadBalancer::SpawnThreads()
{
    for(int i = 0; i < ThreadCount; ++i)
    {
        SocketHolder * Holder = new SocketHolder;
        launch_thread(new SocketHolderThread(*Holder, ThreadDelay));
        Holders.insert(Holder);
    }
    printf("\nSpawned %u socket handler threads with a delay of %u ms.\n", ThreadCount, ThreadDelay);
}

void SocketHolder::CloseAll()
{
    if(_Thread)
        _Thread->SetThreadState(WOWD_THREADSTATE_TERMINATE);

    std::set<BaseSocket*>::iterator itr;
	BaseSocket * s;
    for(itr = Sockets.begin(); itr != Sockets.end();)
    {
		s = *itr;
		++itr;
		s->Delete();
    }
    Sockets.clear();
}
