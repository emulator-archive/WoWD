#include "../../../src/game/StdAfx.h"

#pragma comment(lib, "ws2_32")

// Web Server Plugin
extern "C" WOWD_SCRIPT_DECL void ScriptInitialize(ScriptModule *mod);
extern "C" WOWD_SCRIPT_DECL PluginModule* CreatePluginModule();

BUILD_VERSIONINFO_DATA(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);

static const char * Header = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
"<html xmlns=\"http://www.w3.org/1999/xhtml\" >\n"
"<head>\n"
"<title>{SERVER_TITLE} - Status Page</title>\n"
"<style type=\"text/css\">\n"
"body { BACKGROUND: black; margin: 0px; padding: 0px; height: 100%; width: 100%}\n"
"td{	\n"
"    background-color:black;\n"
"}\n"
"th{\n"
"    text-align: left;\n"
"    background-color:#111;\n"
"color:#eee;\n"
"}\n"
"tr.head th{\n"
"    background-color:#333;\n"
"}\n"
"\n"
"td {\n"
"color: gray;\n"
"}\n"
"</style>\n"
"</head>\n";

/* This is needed because windows is a piece of shit. ;) */
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    return TRUE;
}

class WebServerSocket;
class WebServer
{
public:
    WebServer();
    ~WebServer();

    void ProcessRequest(WebServerSocket& socket);

private:
    char * Template;
};

class WebServerSocket : public BaseSocket
{
public:
    WebServerSocket(SocketHolder& socketHolder) : BaseSocket(socketHolder, 2048, 10000, 10000) {}
    
    void OnReceive(const u16 Size);
    void Delete();
    inline void Send(const char* Data, u32 Size) { SendArray((const u8*)Data, Size, FALSE); }
};

// Global Var!
WebServer * webServ;

WebServer::WebServer()
{
    FILE * fIn = fopen("webserve.templ", "r");
    ASSERT(fIn);

    // Get size
    fseek(fIn, 0, SEEK_END);
    uint32 Size = ftell(fIn);
    fseek(fIn, 0, SEEK_SET);

    Template = new char[Size];
    ASSERT(Template);

    fread(Template, Size, 1, fIn);
    fclose(fIn);

    // Create Socket Listener
    AsyncNet<BaseSocket, Mutex>::getSingleton().CreateListener<WebServerSocket>("0.0.0.0", 81);
}

WebServer::~WebServer()
{

}

void WebServer::ProcessRequest(WebServerSocket& socket)
{
    // Build the response
    stringstream htm2;
    HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;
    uint32 count = 0, load = 0;
    Player * plr;
    for (itr = sObjHolder.Begin<Player>(); itr != sObjHolder.End<Player>(); itr++)
    {
        if(itr->second->GetSession())
        {
            plr = itr->second;
            htm2 << "<plr>\n<name>" << plr->GetName() << "</name>\n"
                << "<race>" << uint32(plr->getRace()) << "</race>\n"
                << "<class>" << uint32(plr->getClass()) << "</class>\n"
                << "<level>" << uint32(plr->getLevel()) << "</level>\n"
                << "<zone>" << uint32(plr->GetZoneId()) << "</zone>\n"
                << "</plr>\n";       
            load += plr->GetSession()->GetLatency();
            count++;
        }
    }

    float avglat = float( float(load) / float(count) );
    stringstream html;
    string uptime;
    //// FORMAT UPTIME
    {
        uint32 seconds = sWorld.GetUptime();
        uint32 mins=0;
        uint32 hours=0;
        uint32 days=0;
        if(seconds >= 60)
        {
            mins = seconds / 60;
            if(mins)
            {
                seconds -= mins*60;
                if(mins >= 60)
                {
                    hours = mins / 60;
                    if(hours)
                    {
                        mins -= hours*60;
                        if(hours >= 24)
                        {
                            days = hours/24;
                            if(days)
                                hours -= days*24;
                        }
                    }
                }
            }
        }
        char str[200];
        sprintf(str, "%d days, %d hours, %d minutes, %d seconds.", days, hours, mins, seconds);
        uptime = str;
    }

    html << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl
         << "<?xml-stylesheet type=\"text/xsl\" href=\"server_stats.xsl\"?>" << endl
         << "<serverpage>" << endl
         << " <status>" << endl
         << "  <platform>Win32 rev 1337</platform>" << endl
         << "  <uptime>" << uptime << "</uptime>" << endl
         << "  <oplayers>" << sWorld.GetSessionCount() << "</oplayers>" << endl
         << "  <cpu>0.00%</cpu>"
         << "  <qplayers>0</qplayers>" << endl
         << "  <ram>100.00MB</ram>"
         << "  <avglat>" << avglat << "</avglat>" << endl
         << "  <threads>" << sThreadMgr.GetThreadCount() << "</threads>" << endl
         << " </status>" << endl;

    html << " <sessions>" << endl;
    html << htm2.str();
    html << " </sessions>" << endl;
    html << "</serverpage>" << endl;
    
    stringstream ss;
    ss << "HTTP/1.1 200 OK" << endl
       << "Content-Length: " << html.str().size() << endl
       << "Content-Type: text/html" << endl
       << "Server: WoWD/2.0.8" << endl
       << endl
       << html.str();

    socket.Send(ss.str().c_str(), ss.str().size());        
}


/*** SOCKET HANDLERS ***/
void WebServerSocket::Delete()
{
    delete this;
}

void WebServerSocket::OnReceive(const u16 Size)
{
    webServ->ProcessRequest(*this);
    PostCompletion(IOShutdownRequest);
}

void ScriptInitialize(ScriptModule *mod)
{
    webServ = new WebServer;
}

/*** GRR ***/
int __cdecl HandleCrash(PEXCEPTION_POINTERS pExceptPtrs)
{
    return 0;
}

void SetThreadName(const char* format, ...)
{

}
