#include "StdAfx.h"
initialiseSingleton(LogonCommHandler);

LogonCommHandler::LogonCommHandler(SocketLoadBalancer * l) : lb(l)
{
    idhigh = 1;
    next_request = 1;
    pings = !Config.MainConfig.GetBoolDefault("DisablePings", false);
}

LogonCommClientSocket * LogonCommHandler::ConnectToLogon(string Address, uint32 Port)
{
    // Create the socket.
#ifndef ASYNC_NET
    LogonCommClientSocket * conn = new LogonCommClientSocket(lb->DynamicLoadBalance());
#else
    LogonCommClientSocket * conn = new LogonCommClientSocket(reinterpret_cast<SocketHolder&>(lb));
    conn->SetCP(AsyncNet<BaseSocket, Mutex>::getSingleton().GetCompletionPort());
#endif
    try
    {
        conn->Connect(Address.c_str(), Port);
    }
    catch (...)
    {
        delete conn;
        return 0;
    }

    // Send the initial ping
    conn->SendPing();

    return conn;
}

void LogonCommHandler::RequestAddition(LogonCommClientSocket * Socket)
{
    set<Realm*>::iterator itr = realms.begin();
    WorldPacket data(RCMSG_REGISTER_REALM, 100);
    for(; itr != realms.end(); ++itr)
    {
        data.clear();

        // Add realm to the packet
        Realm * realm = *itr;
        data << realm->Name;
        data << realm->Address;
        data << realm->Colour;
        data << realm->Icon;
        data << realm->TimeZone;
        data << realm->Population;
        Socket->SendPacket(&data);
    }
}

void LogonCommHandler::Startup()
{
    // Try to connect to all logons.
    sLog.outColor(TNORMAL, "\n >> loading realms and logon server definitions... ");
    LoadRealmConfiguration();

    sLog.outColor(TNORMAL, " >> attempting to connect to all logon servers... \n");
    for(set<LogonServer*>::iterator itr = servers.begin(); itr != servers.end(); ++itr)
        Connect(*itr);

    sLog.outColor(TNORMAL, "\n");
}

void LogonCommHandler::Connect(LogonServer * server)
{
    sLog.outColor(TNORMAL, "    >> connecting to `%s` on `%s:%u`...", server->Name.c_str(), server->Address.c_str(), server->Port);
    server->RetryTime = time(NULL) + 10;
    server->Registered = false;
    LogonCommClientSocket * conn = ConnectToLogon(server->Address, server->Port);
    logons[server] = conn;
    if(conn == 0)
    {
        sLog.outColor(TRED, " fail!\n       server connection failed. I will try again later.");
        sLog.outColor(TNORMAL, "\n");
        return;
    }
    sLog.outColor(TGREEN, " ok!\n");
    sLog.outColor(TNORMAL, "       >> registering realms... ");
    conn->_id = server->ID;

    RequestAddition(conn);

    uint32 st = time(NULL) + 10;

    // Wait for register ACK
    while(server->Registered == false)
    {
        // Don't wait more than.. like 10 seconds for a registration
        if(time(NULL) >= st)
        {
            sLog.outColor(TYELLOW, "timeout.");
            break;
        }
        Sleep(50);
    }

    if(!server->Registered)
        return;

    // Wait for all realms to register
    Sleep(200);

    //sLog.outColor(TNORMAL, "registered as realm id ");
    //sLog.outColor(TGREEN, "%u, ", server->ServerID);
    sLog.outColor(TNORMAL, "       >> ping test: ");
    sLog.outColor(TYELLOW, "%ums", conn->latency);
    sLog.outColor(TNORMAL, "\n");
}

void LogonCommHandler::AdditionAck(uint32 ID, uint32 ServID)
{
    map<LogonServer*, LogonCommClientSocket*>::iterator itr = logons.begin();
    for(; itr != logons.end(); ++itr)
    {
        if(itr->first->ID == ID)
        {
            itr->first->ServerID = ServID;
            itr->first->Registered = true;
            return;
        }
    }
}

void LogonCommHandler::UpdateSockets()
{
    Guard guard(mapLock);
    map<LogonServer*, LogonCommClientSocket*>::iterator itr = logons.begin();
    LogonCommClientSocket * cs;
    uint32 t = time(NULL);
    for(; itr != logons.end(); ++itr)
    {
        cs = itr->second;
        if(cs != 0)
        {
            if(pings && (t - cs->last_ping) > 10)
            {
                if(!cs->gotpong)        // burlex: if we haven't got a pong in 10 seconds, i'd be pretty worried
                {
                    // flag for dc
                    cs->Disconnect();
#ifdef ASYNC_NET
                    cs->PostCompletion(IOShutdownRequest);
#endif
                    itr->second = 0;
                    continue;
                }
                else
                {
                    cs->SendPing();
                }
            }
        }
        else
        {
            // check retry time
            if(t >= itr->first->RetryTime)
            {
                Connect(itr->first);
            }
        }
    }
}

void LogonCommHandler::ConnectionDropped(uint32 ID)
{
    Guard guard(mapLock);
    map<LogonServer*, LogonCommClientSocket*>::iterator itr = logons.begin();
    for(; itr != logons.end(); ++itr)
    {
        if(itr->first->ID == ID && itr->second != 0)
        {
            sLog.outColor(TNORMAL, " >> realm id %u connection was dropped unexpectedly. reconnecting next loop.", ID);
            sLog.outColor(TNORMAL, "\n");
            itr->second = 0;
            return;
        }
    }
}

uint32 LogonCommHandler::ClientConnected(string AccountName, WorldSocket * Socket)
{
    uint32 request_id = next_request++;
    sLog.outColor(TNORMAL, " >> sending request for account information: `%s` (request %u).", AccountName.c_str(), request_id);
    sLog.outColor(TNORMAL, "\n");
    
    // Send request packet to server.
    map<LogonServer*, LogonCommClientSocket*>::iterator itr = logons.begin();
    if(logons.size() == 0 || itr->second == 0)
    {
        // No valid logonserver is connected.
        return (uint32)-1;
    }
    Guard guard(pendingLock);

    WorldPacket data(RCMSG_REQUEST_SESSION, 100);
    data << request_id;
    data << AccountName;
    itr->second->SendPacket(&data);

    pending_logons[request_id] = Socket;

    return request_id;
}

void LogonCommHandler::UnauthedSocketClose(uint32 id)
{
    Guard guard(pendingLock);
    pending_logons.erase(id);
}

void LogonCommHandler::LogonDatabaseSQLExecute(const char* str, ...)
{
    va_list ap;
    va_start(ap, str);
    char query[1024];
    vsprintf(query, str, ap);
    va_end(ap);
    WorldPacket data(RCMSG_SQL_EXECUTE, strlen(query)+1);
    data << query;
    
    // Send request packet to server.
    map<LogonServer*, LogonCommClientSocket*>::iterator itr = logons.begin();
    if(logons.size() == 0 || itr->second == 0)
    {
        // No valid logonserver is connected.
        return;
    }
    itr->second->SendPacket(&data);
}

void LogonCommHandler::LoadRealmConfiguration()
{
    uint32 logoncount = Config.RealmConfig.GetIntDefault("LogonServerCount", 0);
    if(logoncount == 0)
    {
        sLog.outColor(TRED, "\n   >> no logon servers found. this server will not be online anywhere!\n");
    }
    else
    {
        for(uint32 i = 1; i < logoncount+1; ++i)
        {
            LogonServer * ls = new LogonServer;
            ls->ID = idhigh++;
            ls->Name = Config.RealmConfig.GetStringVA("UnkLogon", "LogonServer%u.Name", i);
            ls->Address = Config.RealmConfig.GetStringVA("127.0.0.1", "LogonServer%u.Address", i);
            ls->Port = Config.RealmConfig.GetIntVA(8093, "LogonServer%u.Port", i);
            servers.insert(ls);
        }
        sLog.outColor(TYELLOW, "%u servers, ", logoncount);
    }

    uint32 realmcount = Config.RealmConfig.GetIntDefault("RealmCount", 1);
    if(realmcount == 0)
    {
        sLog.outColor(TRED, "\n   >> no realms found. this server will not be online anywhere!\n");
    }
    else
    {
        for(uint32 i = 1; i < realmcount+1; ++i)
        {
            Realm * realm = new Realm;
            realm->Name = Config.RealmConfig.GetStringVA("SomeRealm", "Realm%u.Name", i);
            realm->Address = Config.RealmConfig.GetStringVA("127.0.0.1:8129", "Realm%u.Address", i);
            realm->Colour = Config.RealmConfig.GetIntVA(1, "Realm%u.Colour", i);
            realm->TimeZone = Config.RealmConfig.GetIntVA(1, "Realm%u.TimeZone", i);
            realm->Population = Config.RealmConfig.GetFloatVA(0, "Realm%u.Population", i);
            string rt = Config.RealmConfig.GetStringVA("Normal", "Realm%u.Icon", i);
            uint32 type;

            // process realm type
            if(!stricmp(rt.c_str(), "pvp"))
                type = 4;
            else if(!stricmp(rt.c_str(), "rp"))
                type = 6;
            else if(!stricmp(rt.c_str(), "rppvp"))
                type = 8;
            else
                type = 0;

            _realmType = type;

            realm->Icon = type;
            realms.insert(realm);
        }
        sLog.outColor(TBLUE, "%u realms.\n", realmcount);
    }

}