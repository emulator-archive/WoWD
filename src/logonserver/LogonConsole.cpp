
#include "LogonConsole.h"
#include "Log.h"
#include "Main.h"

createFileSingleton(LogonConsole);

void LogonConsoleThread::run()
{
    SetThreadName("Console Interpreter");
//    WowdThread::run();
    sLogonConsole._thread = this;
    char i = 0;
	char cmd[96];
	

	while (true)
    {
        /*if(ThreadState == WOWD_THREADSTATE_PAUSED)
        {
            while(ThreadState == WOWD_THREADSTATE_PAUSED)
            {
                Sleep(200);
            }
        }*/
		
		// Make sure our buffer is clean to avoid Array bounds overflow
		memset(cmd,NULL,sizeof(cmd)); 
		// Read in single line from "stdin"
		fgets(cmd, 80, stdin);

        //if(ThreadState == WOWD_THREADSTATE_TERMINATE)
          //  return;

		for( i = 0 ; i < 80 || cmd[i] != '\0' ; i++ )
		{
			if( cmd[i] =='\n' )
			{
				cmd[i]='\0';
				sLogonConsole.ProcessCmd(cmd);
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
void LogonConsole::ProcessCmd(char *cmd)
{
    typedef void (LogonConsole::*PTranslater)(char *str);
    struct SCmd
    {
        char *name;
        PTranslater tr;
    };

    SCmd cmds[] =
    {

        {"?", &LogonConsole::TranslateHelp}, {"help", &LogonConsole::TranslateHelp},
        { "uptime", &LogonConsole::GetUptime},
        { "cancelshutdown", &LogonConsole::CancelShutdown },
        {"quit", &LogonConsole::TranslateQuit}, {"exit", &LogonConsole::TranslateQuit}, 
    };

    /*if(_thread->GetThreadState() == WOWD_THREADSTATE_TERMINATE)
    {
        printf("Exit confirmed. \n");
        return;
    }*/
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

void LogonConsole::CancelShutdown(char *str)
{
    /*printf("Shutdown aborted.\n");
    WorldPacket data(20);
    data.SetOpcode(SMSG_SERVER_MESSAGE);
    data << uint32(SERVER_MSG_SHUTDOWN_CANCELLED);
    sWorld.SendGlobalMessage(&data);
    sMaster.m_ShutdownEvent = false;
    sMaster.m_ShutdownTimer = 0;*/
}
void LogonConsole::GetUptime(char *str)
{
    //sLog.outString("Console: Server has been running for %s", sWorld.GetUptimeString().c_str());
}

// quit | exit
void LogonConsole::TranslateQuit(char *str)
{
    int delay = str != NULL ? atoi(str) : 5000;
    if(!delay)
        delay = 5000;
    else
        delay *= 1000;

    ProcessQuit(delay);
}
void LogonConsole::ProcessQuit(int delay)
{
    //sMaster.m_ShutdownTimer = delay;
    //sMaster.m_ShutdownEvent = true;
    mrunning = false;
}
//------------------------------------------------------------------------------
// help | ?
void LogonConsole::TranslateHelp(char *str)
{
    ProcessHelp(NULL);
}
void LogonConsole::ProcessHelp(char *command)
{
    if (command == NULL)
    {
        sLog.outString("Console:--------help--------");
        sLog.outString("   help, ?: print this text");
		sLog.outString("   uptime: print uptime of the server");
        sLog.outString("   version, ver: print version");
		sLog.outString("   cancelshutdown: cancels the shutdown of the server");
        sLog.outString("   quit, exit: close program");
    }
}
//------------------------------------------------------------------------------

LogonConsoleThread::LogonConsoleThread()
{
//    ThreadType = WOWD_THREADTYPE_CONSOLEINTERFACE;
}

LogonConsoleThread::~LogonConsoleThread()
{

}
