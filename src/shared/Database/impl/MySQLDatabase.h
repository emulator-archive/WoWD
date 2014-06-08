/************************************************************************/
/* Copyright (C) 2007 Burlex                                            */
/************************************************************************/

#ifndef _MYSQLDATABASE_H
#define _MYSQLDATABASE_H

#include <string>
#include "../Database.h"

using namespace std;
class MySQLQueryResult;
class MySQLDatabaseThread;

class MySQLDatabase : public Database
{
    friend class MySQLDatabaseThread;
public:
    MySQLDatabase();
    ~MySQLDatabase();

    bool Initialize(const char* Hostname, unsigned int port,
        const char* Username, const char* Password, const char* DatabaseName,
        uint32 ConnectionCount, uint32 BufferSize);

    void Shutdown();

    QueryResult* Query(const char* QueryString, ...);
    bool WaitExecute(const char* QueryString, ...);
    bool Execute(const char* QueryString, ...);

    void CheckConnections();

protected:

    bool Connect();

    bool Connect(uint32 ConnectionIndex);
    bool Disconnect(uint32 ConnectionIndex);

    bool SelectDatabase();
    bool SelectDatabase(uint32 ConnectionIndex);
    bool HandleError(uint32 ConnectionIndex, uint32 ErrorNumber);

    bool SendQuery(uint32 ConnectionIndex, const char* Sql, bool Self = false);

    uint32 GetConnection();

    MYSQL ** Connections;
    char ** QueryBuffer;

    Mutex DelayedQueryBufferMutex;
    char * DelayedQueryBuffer;

    Mutex * InUseMarkers;

    int32 mConnectionCount;
    int32 mNormalCount;
    int32 mDelayedID;

    // For reconnecting a broken connection
    string mHostname;
    string mUsername;
    string mPassword;
    string mDatabaseName;
    uint32 mPort;

    uint32 mNextPing;

    Mutex mSearchMutex;
    MySQLDatabaseThread* mQueryThread;
};

class MySQLQueryResult : public QueryResult
{
public:
    MySQLQueryResult(MYSQL_RES* res, uint32 FieldCount, uint32 RowCount);
    ~MySQLQueryResult();

    bool NextRow();
    void Destroy();

protected:
    MYSQL_RES* mResult;
};

class MySQLDatabaseThread : public WowdThread
{
public:
    MySQLDatabaseThread(MySQLDatabase *db);
    ~MySQLDatabaseThread();

    void run();
    void Do();
    void AddQuery(std::string query);

protected:
    LockedQueue<string> _queue;
    MySQLDatabase *_db;
};

#endif
