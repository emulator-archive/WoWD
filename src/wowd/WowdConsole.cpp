
#include "WowdConsole.h"
#include "Log.h"
#include "Master.h"
/*#include "../shared/NetworkBase/BaseSocket.h"
#include "../shared/Auth/WowCrypt.h"
#include "../shared/CallBack.h"
#include "../game/EventMgr.h"
#include "../game/EventableObject.h"
#include "../game/AreaTrigger.h"
#include "../game/Opcodes.h"
#include "../game/World.h"
#include "../game/Opcodes.h"
#include "../shared/WorldPacket.h"
#include "../game/WorldSocket.h"
#include "../game/ThreadMgr.h"*/
#include "../game/StdAfx.h"


createFileSingleton(WowdConsole);

void WowdConsoleThread::run()
{
    SetThreadName("Console Interpreter");
    WowdThread::run();
    sWowdConsole._thread = this;
    char i = 0;
	char cmd[96];
	

	while (ThreadState != WOWD_THREADSTATE_TERMINATE)
    {
        if(ThreadState == WOWD_THREADSTATE_PAUSED)
        {
            while(ThreadState == WOWD_THREADSTATE_PAUSED)
            {
                Sleep(200);
            }
        }
		
		// Make sure our buffer is clean to avoid Array bounds overflow
		memset(cmd,NULL,sizeof(cmd)); 
		// Read in single line from "stdin"
		fgets(cmd, 80, stdin);

        if(ThreadState == WOWD_THREADSTATE_TERMINATE)
            return;

		for( i = 0 ; i < 80 || cmd[i] != '\0' ; i++ )
		{
			if( cmd[i] =='\n' )
			{
				cmd[i]='\0';
				sWowdConsole.ProcessCmd(cmd);
				fflush(stdin);
				break;
			}
		}
	}
}

//------------------------------------------------------------------------------
// Protected methods:
//------------------------------------------------------------------------------
// Process one command
void WowdConsole::ProcessCmd(char *cmd)
{
    typedef void (WowdConsole::*PTranslater)(char *str);
    struct SCmd
    {
        char *name;
        PTranslater tr;
    };

    SCmd cmds[] =
    {

        {"?", &WowdConsole::TranslateHelp}, {"help", &WowdConsole::TranslateHelp},
        {"updaterealms", &WowdConsole::TranslateUpdateRealms},
        {"ver", &WowdConsole::TranslateVersion}, {"version", &WowdConsole::TranslateVersion},
        { "uptime", &WowdConsole::GetUptime},
        { "threads", &WowdConsole::TranslateThreads},
        { "cancelshutdown", &WowdConsole::CancelShutdown },
        { "status", &WowdConsole::ObjectStats },
        { "announce", &WowdConsole::Announce },
        { "wannounce", &WowdConsole::WideAnnounce },
		{ "saveall", &WowdConsole::SaveallPlayers },
        {"quit", &WowdConsole::TranslateQuit}, {"exit", &WowdConsole::TranslateQuit}, 
    };

    if(_thread->GetThreadState() == WOWD_THREADSTATE_TERMINATE)
    {
        printf("Exit confirmed. \n");
        return;
    }
    char cmd2[80];
    strcpy(cmd2, cmd);
    for(int i = 0; i < strlen(cmd); ++i)
        cmd2[i] = tolower(cmd[i]);

    for (int i = 0; i < sizeof(cmds)/sizeof(SCmd); i++)
        if (strncmp(cmd2, cmds[i].name, strlen(cmds[i].name)) == 0)
        {
            (this->*(cmds[i].tr)) (cmd + strlen(cmds[i].name));
            return;
        }

        printf("Console:Unknown console command (use \"help\" for help).\n");
}

void WowdConsole::CancelShutdown(char *str)
{
    printf("Shutdown aborted.\n");
    WorldPacket data(20);
    data.SetOpcode(SMSG_SERVER_MESSAGE);
    data << uint32(SERVER_MSG_SHUTDOWN_CANCELLED);
    sWorld.SendGlobalMessage(&data);
    sMaster.m_ShutdownEvent = false;
    sMaster.m_ShutdownTimer = 0;
}
void WowdConsole::GetUptime(char *str)
{
    HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;
    uint32 count = 0;
    for (itr = sObjHolder.Begin<Player>(); itr != sObjHolder.End<Player>(); itr++)
    {
        if(itr->second->GetSession())
            count++;
    }
    
    sLog.outString("Console: Server has been running for %s currently is %d online players", sWorld.GetUptimeString().c_str(),count);
}
//------------------------------------------------------------------------------
// ver[sion]
void WowdConsole::TranslateVersion(char *str)
{
    ProcessVersion();
}
void WowdConsole::ProcessVersion()
{
    sLog.outString("Console:WoWd Server %s", _FULLVERSION);
}
//------------------------------------------------------------------------------
// quit | exit
void WowdConsole::TranslateQuit(char *str)
{
    int delay = str != NULL ? atoi(str) : 5000;
    if(!delay)
        delay = 5000;
    else
        delay *= 1000;

    ProcessQuit(delay);
}
void WowdConsole::ProcessQuit(int delay)
{
    sMaster.m_ShutdownTimer = delay;
    sMaster.m_ShutdownEvent = true;
}
//------------------------------------------------------------------------------
// help | ?
void WowdConsole::TranslateHelp(char *str)
{
    ProcessHelp(NULL);
}
void WowdConsole::ProcessHelp(char *command)
{
    if (command == NULL)
    {
        sLog.outString("Console:--------help--------");
        sLog.outString("   help, ?: print this text");
		sLog.outString("   uptime: print uptime of the server");
        sLog.outString("   updaterealms: update realms from db");
        sLog.outString("   version, ver: print version");
		sLog.outString("   cancelshutdown: cancels the shutdown of the server");
		sLog.outString("   announce: announces a msg to the server.");
		sLog.outString("   wannounce: announces a wide msg to the server");
        sLog.outString("   saveall: saves all players");
        sLog.outString("   quit, exit: close program");
    }
}
//------------------------------------------------------------------------------
// updaterealms
void WowdConsole::TranslateUpdateRealms(char *str)
{
    //UpdateRealms();
}
void WowdConsole::UpdateRealms()
{
    //    sRealmList.UpdateRealms();
    //    sLog.outString("Console:Realms Updated!");
}

WowdConsoleThread::WowdConsoleThread() : WowdThread()
{
    ThreadType = WOWD_THREADTYPE_CONSOLEINTERFACE;
}

void WowdConsole::TranslateThreads(char* str)
{
    std::string threads = sThreadMgr.ShowStatus();
    printf(threads.c_str());
}

WowdConsoleThread::~WowdConsoleThread()
{

}

void WowdConsole::ObjectStats(char *str)
{
    printf("\n");
    printf("Loaded object information:\n");
    printf("======================================\n");
    printf("Loaded Creatures:      %u/%u\n", sWorld.mLoadedCreatures[1], sWorld.mLoadedCreatures[0]);
    printf("Loaded GameObjects:    %u/%u\n", sWorld.mLoadedGameObjects[1], sWorld.mLoadedGameObjects[0]);
    printf("Loaded Cells:          %u\n", sWorld.mTotalCells);
    printf("Active Cells:          %u\n", sWorld.mActiveCellCount);
    printf("Half-Active Cells:     %u\n", sWorld.mHalfActiveCellCount);
    printf("Inactive Cells:        %u\n", sWorld.mInactiveCellCount);
    printf("======================================\n");
    printf("\n");
}

void WowdConsole::Announce(char* str)
{
    if(!str)
        return;

    char msg[500];
    sprintf(msg, "%sConsole:%s%s", "|cff00ccff", "|r", str);
    sWorld.SendWorldText(msg, 0);
}

void WowdConsole::WideAnnounce(char *str)
{
    if(!str)
        return;

    char msg[500];
    sprintf(msg, "%sConsole:%s%s", "|cff00ccff", "|r", str);
	sWorld.SendWorldText(msg, 0);
    sWorld.SendWorldWideScreenText(msg, 0);
}

void WowdConsole::SaveallPlayers(char *str)
{
	HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;
    uint32 stime = now();
    uint32 count = 0;
    for (itr = sObjHolder.Begin<Player>(); itr != sObjHolder.End<Player>(); itr++)
    {
        if(itr->second->GetSession())
        {
            itr->second->SaveToDB();
            count++;
        }
    }
    char msg[100];
    sprintf(msg, "Saved all %d online players in %d msec.", count, (uint32)now() - stime);
    sWorld.SendWorldText(msg);
    sWorld.SendWorldWideScreenText(msg);
}