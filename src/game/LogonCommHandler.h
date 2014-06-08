#ifndef LOGONCOMMHANDLER_H
#define LOGONCOMMHANLDER_H

typedef struct
{
    uint32 ID;
    string Name;
    string Address;
    uint32 Port;
    uint32 ServerID;
    uint32 RetryTime;
    bool   Registered;
}LogonServer;

typedef struct
{
    string Name;
    string Address;
    uint32 Colour;
    uint32 Icon;
    uint32 TimeZone;
    float Population;
}Realm;

class LogonCommHandler : public Singleton<LogonCommHandler>
{
    map<LogonServer*, LogonCommClientSocket*> logons;
    map<uint32, WorldSocket*> pending_logons;
    set<Realm*> realms;
    set<LogonServer*> servers;
    SocketLoadBalancer * lb;
    uint32 idhigh;
    uint32 next_request;
    Mutex mapLock;
    Mutex pendingLock;
    bool pings;
    uint32 _realmType;

public:

    LogonCommHandler(SocketLoadBalancer * l);

    LogonCommClientSocket * ConnectToLogon(string Address, uint32 Port);
    void RequestAddition(LogonCommClientSocket * Socket);
    void CheckAllServers();
    void Startup();
    void ConnectionDropped(uint32 ID);
    void AdditionAck(uint32 ID, uint32 ServID);
    void UpdateSockets();
    void Connect(LogonServer * server);
    void LogonDatabaseSQLExecute(const char* str, ...);

    void LoadRealmConfiguration();
    void AddServer(string Name, string Address, uint32 Port);

    uint32 GetRealmType() { return _realmType; }
    void SetRealmType(uint32 type) { _realmType = type; }

    /////////////////////////////
    // Worldsocket stuff
    ///////

    uint32 ClientConnected(string AccountName, WorldSocket * Socket);
    void UnauthedSocketClose(uint32 id);
    inline WorldSocket* GetSocketByRequest(uint32 id)
    {
        Guard guard(pendingLock);
        map<uint32, WorldSocket*>::iterator itr = pending_logons.find(id);
        if(itr == pending_logons.end())
            return 0;
        else
            return itr->second;
    }
        
};

#define sLogonCommHandler LogonCommHandler::getSingleton()

#endif

