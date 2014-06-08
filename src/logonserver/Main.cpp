/************************************************************************/
/* Copyright (C) 2007 Burlex for WoWd Project                           */
/************************************************************************/
#include "LogonStdAfx.h"
#include <signal.h>
#include "../shared/svn_revision.h"

#ifdef WIN32
#define PLATFORM_TEXT "Win32"
#else
#define PLATFORM_TEXT "Unix"
#endif

// Database impl
Database * sLogonSQL;
uint8 loglevel = 3;
initialiseSingleton(LogonServer);
#ifdef ASYNC_NET
template <> AsyncNet<BaseSocket, Mutex> * Singleton< AsyncNet<BaseSocket, Mutex> >::mSingleton = 0;
template <> DeadSocketCollector<BaseSocket, Mutex>
* Singleton < DeadSocketCollector<BaseSocket, Mutex> > :: mSingleton = 0;
#endif

bool mrunning = true;

/*** Signal Handler ***/
void _OnSignal(int s)
{
    switch (s)
    {
#ifndef WIN32
    case SIGHUP:
       {
           sLog.outString("Received SIGHUP signal, reloading accounts.");
           AccountMgr::getSingleton().ReloadAccounts(true);
       }break;
#endif
    case SIGINT:
    case SIGTERM:
    case SIGABRT:
#ifdef _WIN32
    case SIGBREAK:
#endif
        mrunning = false;
        break;
    }

    signal(s, _OnSignal);
}

int main(int argc, char** argv)
{
    new LogonServer;

    // Run!
    LogonServer::getSingleton( ).Run();
}

bool startdb()
{
    string lhostname, lusername, lpassword, ldatabase;
    int lport = 0;
    int ltype = 1;
    // Configure Main Database

    bool result;

    // Configure Logon Database...
    result = Config.MainConfig.GetString("LogonDatabase.Username", &lusername);
    result = !result ? result : Config.MainConfig.GetString("LogonDatabase.Password", &lpassword);
    result = !result ? result : Config.MainConfig.GetString("LogonDatabase.Hostname", &lhostname);
    result = !result ? result : Config.MainConfig.GetString("LogonDatabase.Name", &ldatabase);
    result = !result ? result : Config.MainConfig.GetInt("LogonDatabase.Port", &lport);
    result = !result ? result : Config.MainConfig.GetInt("LogonDatabase.Type", &ltype);

    if(result == false)
    {
        sLog.outString("sql: Logon database parameters not found.");
        return false;
    }

    loglevel = (uint8)Config.MainConfig.GetIntDefault("LogLevel", 0);
    sLogonSQL = CreateDatabaseInterface((DatabaseType)ltype);

    // Initialize it
    if(!sLogonSQL->Initialize(lhostname.c_str(), (unsigned int)lport, lusername.c_str(),
        lpassword.c_str(), ldatabase.c_str(), Config.MainConfig.GetIntDefault("LogonDatabase.ConnectionCount", 1),
        16384))
    {
        sLog.outError("sql: Logon database initialization failed. Exiting.");
        return false;
    }   

    return true;
}

void LogonServer::Run()
{
    sLog.Init();
    sLog.outString("===============================================================================");
    sLog.outString("WoWD :: Logon Server (running under %s)", PLATFORM_TEXT);
    sLog.outString("Compatibility is tested with 2.0.8. Revision %u.", WOWD_SVN_REVISION);
    sLog.outString("===============================================================================");
    sLog.outString("The key combination <Ctrl-C> will safely shut down the server at any time.");
    sLog.outString("");

    sLog.outColor(TNORMAL, "Loading Config Files...\n");
    sLog.outColor(TYELLOW, "  >> logonserver.conf :: ");
    if(Config.MainConfig.SetSource("logonserver.conf"))
    {
        sLog.outColor(TGREEN, "ok!");
        sLog.outColor(TNORMAL, "\n\n");
    }
    else
    {
        sLog.outColor(TRED, "fail.");
        sLog.outColor(TNORMAL, "\n\n");
        return;
    }

    if(!startdb())
        return;

    sLog.outColor(TNORMAL, "\n >> establishing database connection...");
    sLog.outColor(TGREEN, " ok!\n");

    sLog.outColor(TNORMAL, " >> starting: thread manager...");
        new ThreadMgr;
        ThreadMgr::getSingleton( ).Initialize();
    sLog.outColor(TGREEN, " ok!\n");

    sLog.outColor(TNORMAL, " >> starting: account manager...");
        new AccountMgr;
        new IPBanner;
    sLog.outColor(TGREEN, " ok!\n");

    sLog.outColor(TNORMAL, " >> starting: information core...");
        new InformationCore;
    sLog.outColor(TGREEN, " ok!\n");

    sLog.outColor(TNORMAL, " >> precaching accounts...");
        sAccountMgr.ReloadAccounts(true);
    sLog.outColor(TGREEN, " %u accounts.\n", sAccountMgr.GetCount());
    sLog.outColor(TNORMAL, "\n");

    // Spawn periodic function caller thread for account reload every 10mins
    int time = Config.MainConfig.GetIntDefault("AccountRefresh",600);
    time *= 1000;
    SpawnPeriodicCallThread(AccountMgr, AccountMgr::getSingletonPtr(), &AccountMgr::ReloadAccountsCallback, time);
    /*PeriodicFunctionCaller<AccountMgr> * pfc = new PeriodicFunctionCaller<AccountMgr>(AccountMgr::getSingletonPtr(),
        &AccountMgr::ReloadAccountsCallback, time);*/

    // Load conf settings..
    uint32 cport = Config.MainConfig.GetIntDefault("RealmListPort", 3724);
    uint32 sport = Config.MainConfig.GetIntDefault("ServerPort", 8093);
    uint32 threadcount = Config.MainConfig.GetIntDefault("Network.ThreadCount", 5);
    uint32 threaddelay = Config.MainConfig.GetIntDefault("Network.ThreadDelay", 20);
    string host = Config.MainConfig.GetStringDefault("Host", "0.0.0.0");
    string shost = Config.MainConfig.GetStringDefault("ISHost", host.c_str());
    min_build = Config.MainConfig.GetIntDefault("MinClientBuild", 6180);
    max_build = Config.MainConfig.GetIntDefault("MaxClientBuild", 6999);

	launch_thread(new LogonConsoleThread);

#ifndef ASYNC_NET

    InitializeWinsock();
    SocketLoadBalancer LoadBalancer(threadcount, threaddelay);
    LoadBalancer.SpawnThreads();

    // Spawn auth listener
    ListenSocket<AuthSocket> clientListener(LoadBalancer);
    clientListener.CreateListenSocket(host, cport);
    
    // Spawn interserver listener
    ListenSocket<LogonCommServerSocket> interListener(LoadBalancer);
    interListener.CreateListenSocket(shost, sport);

#else

    new AsyncNet<BaseSocket, Mutex>;
    new DeadSocketCollector<BaseSocket, Mutex>;
    AsyncNet<BaseSocket, Mutex>::getSingleton().CreateListener<AuthSocket>(host, cport);
    AsyncNet<BaseSocket, Mutex>::getSingleton().CreateListener<LogonCommServerSocket>(shost, sport);
    AsyncNet<BaseSocket, Mutex>::getSingleton().RunNetwork();

#endif

    // hook signals
    sLog.outString("Hooking signals...");
    signal(SIGINT, _OnSignal);
    signal(SIGTERM, _OnSignal);
    signal(SIGABRT, _OnSignal);
#ifdef _WIN32
    signal(SIGBREAK, _OnSignal);
#else
    signal(SIGHUP, _OnSignal);
#endif

    while(mrunning)
    {
#ifndef ASYNC_NET
        clientListener.Update();
        interListener.Update();
        sInfoCore.TimeoutSockets();
        Sleep(50);
#else
        sInfoCore.TimeoutSockets();
        sDeadSocketCollector.Update();
        Sleep(200);
#endif        
    }

    sLog.outString("Shutting down...");

#ifdef ASYNC_NET
    delete AsyncNet<BaseSocket, Mutex>::getSingletonPtr();
#else
    interListener.CloseListener();
    clientListener.CloseListener();
    LoadBalancer.CloseAll();
#endif

	/*LogonConsoleThread *thread = (LogonConsoleThread*)sThreadMgr.GetThreadByType(WOWD_THREADTYPE_CONSOLEINTERFACE);
    ASSERT(thread);
    thread->SetThreadState(WOWD_THREADSTATE_TERMINATE);
    // have to cleanup manually.
    sThreadMgr.RemoveThread(thread);*/

    sThreadMgr.Shutdown();

    delete AccountMgr::getSingletonPtr();
    delete InformationCore::getSingletonPtr();
    delete ThreadMgr::getSingletonPtr();
    delete IPBanner::getSingletonPtr();
}

void OnCrash(bool Terminate)
{

}
