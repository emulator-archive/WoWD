//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////

// WoWBot, as some people say, is back again. :)
// This time in the form as a plugin :P

#include "StdAfx.h"

/* Script Export Declarations */

extern "C" WOWD_SCRIPT_DECL void ScriptInitialize(ScriptModule *mod);
extern "C" WOWD_SCRIPT_DECL PluginModule* CreatePluginModule();

// Build version info function
BUILD_VERSIONINFO_DATA(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);

#ifdef WIN32

/* This is needed because windows is a piece of shit. ;) */
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    return TRUE;
}

#endif

/****
This is a very cut down version of BIRCLib (burlex's irc lib)
While it lacks the auth parts and rejoin/kick handlers, it
should be sufficient for WoWBot's purposes.
****/
class IRCBot;
class WoWBot;

class IRCSocket : public BaseSocket
{
public:
    IRCSocket(SocketHolder& socketHolder, IRCBot& ircBot);
    ~IRCSocket();

    void OnConnect();
    void OnReceive(const u16 Size);

    void SendLine(const char* Text, ...);
    void Delete();
protected:
    IRCBot& _Bot;
};

class IRCBot
{
public:
    IRCBot()
    {
        ircSocket = 0;
        ChannelName = "";
        NickName = "";
        joined_channels = false;
    }

    void PushLine(const char* Data);
    void SetSocket(IRCSocket* Socket) { ircSocket = Socket; }
    void JoinChannel(const char* name, const char* key);
    void SendMessageToAll(const char* message, ...);
    string ChannelName;
    string NickName;
    string Server;
    void Connect();
    bool joined_channels;
    WoWBot * wbot;
    IRCSocket * ircSocket;
protected:
    void _HandlePing(string& Stream, string& Source);
    void _HandleMode(string& Stream, string& Source);
    void _HandlePrivMsg(string& Stream, string& Source);

    SocketHolder * holder;
};

IRCSocket::IRCSocket(SocketHolder& socketHolder, IRCBot& ircBot) : BaseSocket(socketHolder, 32000, 32000, 32000), _Bot(ircBot)
{
    //Log.Success("IRCSocket created.");
}

IRCSocket::~IRCSocket()
{
    //Log.Error("IRCSocket deleted.");
}

void IRCSocket::OnConnect()
{
    //TODO: Send authentication info from main IRCBOT class
    //Log.Success("Connected to: %s.", RetreiveClientIP());
    SendLine("NICK %s", _Bot.NickName.c_str());
    SendLine("USER %s * 0 : %s", _Bot.NickName.c_str(), _Bot.NickName.c_str());
}

void IRCSocket::SendLine(const char* Text, ...)
{
    va_list l;
    va_start(l, Text);

    char * line = new char[strlen(Text) + 300];
    vsprintf(line, Text, l);
    
    // Append a newline character
    strcat(line, "\n");

    // remove spaces @ start
    char * nl = line;
    while(nl[0] == ' ')
        ++nl;

    // Send over the wire.
    SendArray((const u8*)nl, (u16)strlen(nl), FALSE);

    // cleanup.
    //Log.Out("OUT: \"%s\"", line);
    delete [] line;
    va_end(l);
}



void IRCSocket::OnReceive(const u16 Size)
{
    // Loop through our received data until we find a newline character. If we
    // don't have one, it means we don't have a full irc command.
    u16 Bytes = Size;
    while(1)
    {
        u16 i;
        for(i = 0; i < RecvSize; i++)
        {
            if(ReceiveBuffer[i] == '\n')
            {
                break;
            }
        }

        if(i != RecvSize)
        {
            // We found a newline, weeee. :)
            char* Line = new char[i+2];
            ReadBytes((u8*)Line, i+1);
            Line[i+1] = 0;
            // Push to IRC Bot for later handling.
            _Bot.PushLine(Line);
            //Log.Out("IN: \"%s\"", Line);
            delete [] Line;
            Bytes -= i;
        } else {
            break;
        }
    }
}

class WoWBot : public PluginModule
{
public:
    WoWBot();

    void HandleMessage(PluginMessage * msg);
    void HandleIRCMessage(string &Channel, string &Source, string& Message);

    IRCBot * connection;
    bool enabled;
    //PostMessageToServer poster;
    //SpawnThread spawner;
};

void IRCSocket::Delete()
{
    _Bot.wbot->enabled = false;
}

class WoWBotThread : public WowdThread
{
public:
    WoWBotThread(WoWBot * b);

    void run();
    WoWBot * bot;
};

void IRCBot::PushLine(const char* Data)
{
    // Dereference.. easier
    std::string CommandLine = Data;

    // Find the first space
    std::string::size_type pos = CommandLine.find(" ");
    std::string::size_type pos_start = 0;
    std::string Source = "";
    if(CommandLine[0] == ':' || CommandLine[0] == '!')
    {
        // Skip first part and get real command.
        Source = CommandLine.substr(1, pos-1);     // skip :
        pos_start = pos+1;
        pos = CommandLine.find(" ", pos+1);
    }
    ASSERT(pos != -1);

    // Grab the command
    std::string Cmd = CommandLine.substr(pos_start, pos-pos_start);

    // Get parameters
    std::string Params = CommandLine.substr(pos+1, -1);
    // Remove carriage return or line feed
    for(size_t j = Params.size() - 5; j < Params.size(); ++j)
    {
        if(Params[j] == 0xa || Params[j] == 0xd)
            Params[j] = 0;
    }

    // Transform to uppercase
    std::transform(Cmd.begin(), Cmd.end(), Cmd.begin(), toupper);
    //script_debuglog("irc: %s %s %s", Source.c_str(), Cmd.c_str(), Params.c_str());

    // we can just use an if here..
    if(Cmd == "PRIVMSG")
        _HandlePrivMsg(Params, Source);
    else if(Cmd == "PING")
        _HandlePing(Params, Source);
    else if(Cmd == "MODE")
        _HandleMode(Params, Source);
}

void IRCBot::_HandlePing(string& Stream, string& Source)
{
    // Cut off the :
    std::string Response = Stream.substr(1);
    ircSocket->SendLine("PONG %s", Response.c_str());
}

void IRCBot::_HandleMode(string& Stream, string& Source)
{
    ircSocket->SendLine("JOIN %s", this->ChannelName.c_str());
    ircSocket->SendLine("OPER wowbot hax3db0t");
}

void IRCBot::SendMessageToAll(const char* message, ...)
{
    va_list l;
    va_start(l, message);
    char line[300];
    vsprintf(line, message, l);
    ircSocket->SendLine("PRIVMSG %s :%s", ChannelName.c_str(), line);
    va_end(l);
}

void IRCBot::_HandlePrivMsg(string& Stream, string& Source)
{
    string::size_type colon = Stream.find(":");
    string channel = Stream.substr(0, colon-1);
    string message = Stream.substr(colon+1);
    if(channel == NickName) // Ooh, we got a pm.
        channel = Source.substr(0, Source.find("!"));
    
    wbot->HandleIRCMessage(channel, Source, message);
}

void IRCBot::Connect()
{
    holder = new SocketHolder;
    ircSocket = new IRCSocket(*holder, *this);
    ircSocket->Connect(Server.c_str(), 6667);
}

/* Module info */
void ScriptInitialize(ScriptModule *mod)
{
    // Register us as a plugin in the server, this
    // will automatically call the factory function.

    sScriptMgr.ScriptAddServerModule(mod);
}


WoWBotThread::WoWBotThread(WoWBot * b)
{
    ThreadType = WOWD_THREADTYPE_UNASSIGNED;
    ThreadState = WOWD_THREADSTATE_BUSY;
    bot = b;
}

void WoWBotThread::run()
{
    IRCSocket * s = bot->connection->ircSocket;
    //SetThreadName("Plugin-IRC BOT");
    while(ThreadState != WOWD_THREADSTATE_TERMINATE)
    {
        if(!s->UpdateRead()) return;
        if(!s->UpdateWrite()) return;
        Sleep(100);
    }
}

WoWBot::WoWBot()
{
    enabled = true;
    // get process addresses
    //spawner = GetProcAddress()
    // create the irc bot
    connection = new IRCBot;
    connection->NickName = "MMOWoWBot";
    connection->ChannelName = "";
    connection->Server = "";
    connection->Connect();
    connection->wbot = this;

    // spawn the thread
    WoWBotThread * t = new WoWBotThread(this);
    t->manual_start();
}

void WoWBot::HandleMessage(PluginMessage * msg)
{
    if(!enabled) return;

    if(msg->Opcode == PSMSG_MESSAGE)
    {
        connection->SendMessageToAll((const char*)msg->Data);
        return;
    }

    switch(msg->Opcode)
    {
    case PSMSG_CHAT:
        {
            /*ByteBuffer buf(msg->DataLen);
            buf.resize(msg->DataLen);
            buf.put(0, msg->Data, msg->DataLen);
            uint32 type;
            buf >> type;
            stringstream out;
            string temp, temp2, temp3;*/
            uint32 type = *(uint32*)&msg->Data[0];

            const char * arg1 = (const char*)msg->Data + 4;
            uint32 p = 4;
            const char * arg2 = arg1;
            while(*arg2 != 0 && p < msg->DataLen)
            {
                ++arg2;
                ++p;
            }
            ++arg2;

            const char * arg3 = arg2;
            while(*arg3 != 0 && p < msg->DataLen)
            {
                ++arg3;
                ++p;
            }
            ++arg3;

            switch(type)
            {
            case CHAT_MSG_EMOTE:
                {
                    connection->SendMessageToAll("\x02\x03\x37[Emote] %s: %s", arg1, arg2);
                }break;

            case CHAT_MSG_SAY:
                {
                    connection->SendMessageToAll("%s: %s", arg1, arg2);
                }break;

            case CHAT_MSG_GUILD:
            case CHAT_MSG_OFFICER:
                {
                    connection->SendMessageToAll("\x03\x33[Guild %s] %s: %s", arg1, arg2, arg3);
                }break;

            case CHAT_MSG_YELL:
                {
                    connection->SendMessageToAll("\x03\x34[Yell] %s: %s", arg1, arg2);
                }break;

            case CHAT_MSG_WHISPER:
                {
                    connection->SendMessageToAll("\x03\x36[Whisper] %s to %s: %s", arg1, arg2, arg3);
                }break;

            case CHAT_MSG_CHANNEL:
                {
                    connection->SendMessageToAll("\x03\x31\x34[%s] %s: %s", arg1, arg2, arg3);
                }break;
            }
        }break;
    }
}

void WoWBot::HandleIRCMessage(string &Channel, string &Source, string& Message)
{
    if(Message.length() < 2)
        return;

    if(Message[0] == '.')
    {
        string src = Source.substr(0, Source.find("!"));
        if(!stricmp(Message.c_str(), ".info"))
        {
            connection->SendMessageToAll("\x02\x03\x37 Current active sessions: \x0F%u", sWorld.GetSessionCount());
            connection->SendMessageToAll("\x02\x03\x37 Current inworld players: \x0F%u",  sWorld.mInWorldPlayerCount);
            connection->SendMessageToAll("\x02\x03\x37 Current CPU Load: \x0F%u%%", 0);
            connection->SendMessageToAll("\x02\x03\x37 Connections accepted: \x0F%u", sWorld.mAcceptedConnections);
        }
        else if(!strnicmp(Message.c_str(), ".announce", 9) || !strnicmp(Message.c_str(), ".wannounce", 10))
        {
            if(Message.size() < 12)
                connection->SendMessageToAll("Arguments required for .announce");
            else
            {
                string text = Message.substr(Message.find(" ") + 1);
                connection->SendMessageToAll("Sending ingame announcement: <%s> %s", src.c_str(), text.c_str());

                // build the announcement
                char ann[1024];
                sprintf(ann, "%s announces from IRC: %s", src.c_str(), text.c_str());
                if(Message[1] == 'W' || Message[1] == 'w')
                    sWorld.SendWorldWideScreenText(ann);
                else
                    sWorld.SendWorldText(ann);
            }
        }
        else if(!strnicmp(Message.c_str(), ".kick", 5))
        {
            if(Message.size() < 7)
                connection->SendMessageToAll("\x02\x03\x34 a players name must be specified.");
            else
            {
                string::size_type off1 = Message.find(" ") + 1;
                string::size_type off2 = Message.find(" ", off1);
                if(off2 == -1)
                {
                    connection->SendMessageToAll("\x02\x03\x34 a reason must be specified");
                    return;
                }
                string plr = Message.substr(off1, off2-off1);
                string reason = Message.substr(off2+1);
                string src = Source.substr(0, Source.find("!"));
                Player * wplr = objmgr.GetPlayer(plr.c_str(), false);
                if(!wplr)
                {
                    connection->SendMessageToAll("\x02\x03\x34 player %s not found inworld!", plr.c_str());
                }
                else
                {
                    char msg[300];
                    sprintf(msg, "%s (irc) kicked %s from the server. Reason: %s", src.c_str(), plr.c_str(), reason.c_str());
                    sWorld.SendWorldText(msg);
                    connection->SendMessageToAll("\x02\x03\x33 kicking %s for %s.", plr.c_str(), reason.c_str());
                    wplr->Kick(5000);
                }
            }
        }
        else if(!strnicmp(Message.c_str(), ".kill", 5))
        {
            string::size_type off1 = Message.find(" ") + 1;
            string::size_type off2 = Message.find(" ", off1);
            if(off2 == -1)
            {
                connection->SendMessageToAll("\x02\x03\x34 a reason must be specified");
                return;
            }
            string plr = Message.substr(off1, off2-off1);
            string reason = Message.substr(off2+1);
            string src = Source.substr(0, Source.find("!"));
            Player * wplr = objmgr.GetPlayer(plr.c_str(), false);
            if(!wplr)
            {
                connection->SendMessageToAll("\x02\x03\x34 player %s not found inworld!", plr.c_str());
            }
            else
            {
                char msg[300];
                sprintf(msg, "%s (irc) disconnected %s from the server. Reason: %s", src.c_str(), plr.c_str(), reason.c_str());
                sWorld.SendWorldText(msg);
                connection->SendMessageToAll("\x02\x03\x33 killing %s for %s.", plr.c_str(), reason.c_str());
                wplr->GetSession()->Disconnect();
            }
        }
        else if(!strnicmp(Message.c_str(), ".whisper", 8))
        {
            string::size_type off1 = Message.find(" ") + 1;
            string::size_type off2 = Message.find(" ", off1);
            if(off2 == -1)
            {
                connection->SendMessageToAll("\x02\x03\x34 a message must be specified");
                return;
            }
            string plr = Message.substr(off1, off2-off1);
            string reason = Message.substr(off2+1);
            string src = Source.substr(0, Source.find("!"));
            Player * wplr = objmgr.GetPlayer(plr.c_str(), false);
            if(!wplr)
            {
                connection->SendMessageToAll("\x02\x03\x34 player %s not found inworld!", plr.c_str());
            }
            else
            {
                wplr->BroadcastMessage("%s (from irc) whispers: %s", src.c_str(), reason.c_str());
            }
        }
    }
}

PluginModule* CreatePluginModule()
{
    return new WoWBot();
}