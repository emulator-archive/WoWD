#include "LogonStdAfx.h"

#include "../shared/Common.h"
#include "../shared/Log.h"
#include "../shared/Timer.h"
#include "../game/NameTables.h"
#include "../game/WowdThreads.h"
#include "../game/ThreadMgr.h"

using namespace std;

initialiseSingleton(ThreadMgr);

ThreadMgr::ThreadMgr()
{
    ThreadIdCounter = 0;
    m_SetBusy = false;
}

ThreadMgr::~ThreadMgr()
{
    sLog.outString("*** ThreadMgr Destroyed.\n");
}

void ThreadMgr::Initialize()
{

}

void ThreadMgr::AddThread(WowdThread *thread)
{
    mThreads.insert(thread);
}

void ThreadMgr::RemoveThread(WowdThread *thread)
{
    // dont add/remove while searching a set. ;)
    while(m_SetBusy);

    mThreads.erase(thread);
}

#define SETTHREADCOUNT threadcount = mThreads.size()

void ThreadMgr::Shutdown()
{
    uint32 threadcount;
    uint32 timer = getMSTime() + 500;
    uint32 start = getMSTime();
    SETTHREADCOUNT;

    sLog.outString("*** ThreadMgr: Shutdown requested. Requesting termination from %u threads.", threadcount);

    for(ThreadSet::iterator itr = mThreads.begin(); itr != mThreads.end(); ++itr)
        (*itr)->SetThreadState(WOWD_THREADSTATE_TERMINATE);

    SETTHREADCOUNT;
    sLog.outString("*** ThreadMgr: %u threads remaining.", threadcount);

    while(mThreads.size())
    {
        SETTHREADCOUNT;
        if(getMSTime() > timer)
        {
            sLog.outString("*** ThreadMgr: %u threads remaining.", threadcount);
            timer = getMSTime() + 1000;
        }
        Sleep(20);
    }

    sLog.outString("*** ThreadMgr: Shutdown completed in %u msec.", getMSTime() - start);


}

std::string ThreadMgr::ShowStatus()
{
    std::stringstream s;
    s << endl;
    s << "*** ThreadMgr: " << mThreads.size() << " threads active." << endl;
    s <<    "=============================================================================" << endl;
    s <<    "|  ID  |      Type        |     State      |            Start Time          |" << endl;
    s <<    "=============================================================================" << endl;
    for(ThreadSet::iterator itr = mThreads.begin(); itr != mThreads.end(); ++itr)
    {
        AppendThreadStatus((*itr), s);
        s << endl;
    }
    s <<    "=============================================================================" << endl;
    s << endl;

    return s.str();
}

void ThreadMgr::AppendThreadStatus(WowdThread* thread, std::stringstream &stream)
{
    char line[400];
    time_t stm = thread->GetStartTime();
    char tmp[40];
    strcpy(tmp, ctime(&stm));
    //strncpy(tmp, tmp, strlen(tmp)-2);
    tmp[strlen(tmp)-1] = 0;
    sprintf(line, "| %4u | %16s | %14s | %30s |", thread->GetThreadId(),
        LookupName(thread->GetThreadType(), g_threadTypes),
        LookupName(thread->GetThreadState(), g_threadStates),
        tmp);

    stream.write(line, strlen(line));    
}

int ThreadMgr::GenerateThreadId()
{
    ThreadIdCounter++;
    return ThreadIdCounter;
}

#undef SETTHREADCOUNT

void ThreadMgr::PauseAllThreads()
{
    sLog.outString("*** ThreadMgr: Pausing %u threads.", mThreads.size());
    for(ThreadSet::iterator itr = mThreads.begin(); itr != mThreads.end(); ++itr)
        (*itr)->SetThreadState(WOWD_THREADSTATE_PAUSED);

    // give them time to kick in
    Sleep(1000);
}

void ThreadMgr::ResumeAllThreads()
{
    sLog.outString("*** ThreadMgr: Resuming %u threads.", mThreads.size());
    for(ThreadSet::iterator itr = mThreads.begin(); itr != mThreads.end(); ++itr)
        if((*itr)->GetThreadState() == WOWD_THREADSTATE_PAUSED)
            (*itr)->SetThreadState(WOWD_THREADSTATE_PAUSED);

    // give them time to kick in
    Sleep(1000);
}
