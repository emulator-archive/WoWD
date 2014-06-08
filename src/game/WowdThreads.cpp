//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////

// Class WowdThread - Base class for all threads in the
// server, and allows for easy management by ThreadMgr.

#include "Common.h"
#include "NameTables.h"
#include "WowdThreads.h"
#include "ThreadMgr.h"

/* Name tables */
NameTableEntry g_threadStates[] = {
    { WOWD_THREADSTATE_TERMINATE, "STATE_TERM" },
    { WOWD_THREADSTATE_PAUSED, "TSTATE_PAUSE" },
    { WOWD_THREADSTATE_SLEEPING, "STATE_SLEEP" },
    { WOWD_THREADSTATE_BUSY, "STATE_BUSY" },
    { WOWD_THREADSTATE_AWAITING, "STATE_AWAIT" },
    { 0, NULL },
};

NameTableEntry g_threadTypes[] = {
    { WOWD_THREADTYPE_UNASSIGNED, "UNASSIGNED" },
    { WOWD_THREADTYPE_OBJECTUPDATER, "OBJECTUPDATER" },
    { WOWD_THREADTYPE_UDPATEOBJECT, "UPDATEOBJECT" },
    { WOWD_THREADTYPE_WORLDRUNNABLE, "WORLDRUNNABLE" },
    { WOWD_THREADTYPE_MAILDELIVERY, "MAILDELIVERY" },
    { WOWD_THREADTYPE_CONSOLEINTERFACE, "CONSOLE" },
    { WOWD_THREADTYPE_IRCBOT, "IRCBOT" },
    { WOWD_THREADTYPE_AUCTIONHOUSE, "AUCTIONHOUSE" },
    { WOWD_THREADTYPE_NETWORK, "NETWORK" },
    { WOWD_THREADTYPE_SESSIONUPDATER, "SESSIONUPDATER" },
    { WOWD_THREADTYPE_SOCKETUPDATER, "SOCKETUPDATER" },
    { WOWD_THREADTYPE_DATABASE, "DATABASEQUEUE" },
    { 0, NULL },
};

WowdThread::WowdThread() : ThreadBase()
{
    ThreadState = WOWD_THREADSTATE_AWAITING;
    ThreadType  = WOWD_THREADTYPE_UNASSIGNED;
    ThreadId    = sThreadMgr.GenerateThreadId();
    start_time  = time(NULL);
    sThreadMgr.AddThread(this);
}

WowdThread::~WowdThread()
{
    if(ThreadType == WOWD_THREADTYPE_CONSOLEINTERFACE)
        return; // we're deleted manually
    sThreadMgr.RemoveThread(this);
}

void WowdThread::run()
{

}

void WowdThread::manual_start()
{
    launch_thread(this);
}
