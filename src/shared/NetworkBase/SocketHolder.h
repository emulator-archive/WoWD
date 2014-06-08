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

#ifndef __SOCKETHOLDER_H
#define __SOCKETHOLDER_H

#include "SocketDefines.h"
#include "Common.h"
#include "Timer.h"
#include "Singleton.h"
#include "../../game/WowdThreads.h"
#include "../../game/ThreadMgr.h"

class BaseSocket;
class SocketHolderThread;

class WOWD_SERVER_DECL SocketHolder
{
public:
    void CloseAll();
    void AddSocket(BaseSocket* Socket);
    void RemoveSocket(BaseSocket* Socket);
    inline u32 Count() { return Sockets.size(); }
    
    inline void SetThread(SocketHolderThread* thread)
    {
        _Thread = thread;
    }

    inline std::set<BaseSocket*>::iterator Begin() { return Sockets.begin(); }
    inline std::set<BaseSocket*>::iterator End() { return Sockets.end(); }

    Mutex SetMutex;
    void Set(bool Read, bool Write, SOCKET Socket);

protected:
    std::set<BaseSocket*> Sockets;
    SocketHolderThread *_Thread;
};

class WOWD_SERVER_DECL SocketHolderThread : public WowdThread
{
    friend class SocketHolder;
public:
    SocketHolderThread(SocketHolder& Holder, u32 Thread_Delay);
    ~SocketHolderThread();

    void run();
    void Run();
protected:
    SocketHolder& _Holder;
    /*fd_set ReadSet;
    fd_set WriteSet;
    fd_set AllSet;*/
    u32 ThreadDelay;
    /*u32 HighSocket;*/
};

class SocketLoadBalancer
{
public:
    SocketLoadBalancer(u32 Thread_Count, u32 Thread_Delay) : ThreadCount(Thread_Count), ThreadDelay(Thread_Delay)
    {

    }
    ~SocketLoadBalancer()
    {
        for(std::set<SocketHolder*>::iterator itr = Holders.begin();
            itr != Holders.end(); ++itr)
        {
            (*itr)->CloseAll();
            delete (*itr);
        }
        Holders.clear();
    }

    void SpawnThreads();
    void CloseAll()
    {
        for(std::set<SocketHolder*>::iterator itr = Holders.begin();
            itr != Holders.end(); ++itr)
        {
            (*itr)->CloseAll();
        }
    }

    SocketHolder& DynamicLoadBalance(void)
    {
        std::pair<unsigned int, SocketHolder*> Chosen;
        Chosen.first = 999999;
        Chosen.second = NULL;
        
        for(std::set<SocketHolder*>::iterator itr = Holders.begin();
            itr != Holders.end(); ++itr)
        {
            if((*itr)->Count() < Chosen.first)
            {
                Chosen.second = *itr;
                Chosen.first = (*itr)->Count();
            }
        }

        ASSERT(Chosen.second != NULL);
        return *(Chosen.second);
    }

protected:
    std::set<SocketHolder*> Holders;
    u32 ThreadCount, ThreadDelay;
};
#endif

