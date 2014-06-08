
#ifndef __WOWDCONSOLE_H
#define __WOWDCONSOLE_H

#include "Common.h"
#include "../game/WowdThreads.h"

class WowdConsoleThread : public WowdThread
{
public:
    WowdConsoleThread();
    ~WowdConsoleThread();
    void run();
};

class WowdConsole :  public Singleton < WowdConsole >
{
    friend class WowdConsoleThread;

public:                        // Public methods:

protected:                    // Protected methods:
    WowdConsoleThread *_thread;

    // Process one command
    void ProcessCmd(char *cmd);

    // ver[sion]
    void TranslateVersion(char *str);
    void ProcessVersion();

    // quit | exit
    void TranslateQuit(char *str);
    void ProcessQuit(int delay);
    void CancelShutdown(char *str);

    // help | ?
    void TranslateHelp(char *str);
    void ProcessHelp(char *command);

    //updaterealms
    void TranslateUpdateRealms(char *str);
    void UpdateRealms();
    void TranslateThreads(char* str);

    void ObjectStats(char *str);

    // getuptime
    void GetUptime(char *str);

    void WideAnnounce(char *str);
    void Announce(char* str);
	void SaveallPlayers(char *str);
};

#define sWowdConsole WowdConsole::getSingleton()

#endif
