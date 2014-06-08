//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////

// Class WowdThread - Base class for all threads in the
// server, and allows for easy management by ThreadMgr.

#ifndef _MAPUPDATER_H
#define _MAPUPDATER_H

class MapMgr;
class Object;
class Player;
class WorldSession;
class Creature;
class GameObject;

#define MAPMGR_UPDATEOBJECT_LOOP_DELAY 100
#define MAPMGR_SESSION_UPDATE_DELAY    50

#define MAPMGR_UPDATE_DELAY            100

enum WowdThreadState
{
    WOWD_THREADSTATE_TERMINATE = 0,
    WOWD_THREADSTATE_PAUSED    = 1,
    WOWD_THREADSTATE_SLEEPING  = 2,
    WOWD_THREADSTATE_BUSY      = 3,
    WOWD_THREADSTATE_AWAITING  = 4,
};

enum WowdThreadType
{
    WOWD_THREADTYPE_UNASSIGNED         = 0,
    WOWD_THREADTYPE_OBJECTUPDATER      = 1,
    WOWD_THREADTYPE_UDPATEOBJECT       = 2,
    WOWD_THREADTYPE_WORLDRUNNABLE      = 3,
    WOWD_THREADTYPE_MAILDELIVERY       = 4,
    WOWD_THREADTYPE_CONSOLEINTERFACE   = 5,
    WOWD_THREADTYPE_IRCBOT             = 6,
    WOWD_THREADTYPE_AUCTIONHOUSE       = 7, // future ;)
    WOWD_THREADTYPE_NETWORK            = 8,
    WOWD_THREADTYPE_SESSIONUPDATER     = 9,
    WOWD_THREADTYPE_SOCKETUPDATER      = 10,
    WOWD_THREADTYPE_DATABASE           = 11,
};

struct NameTableEntry;
extern NameTableEntry g_threadStates[];
extern NameTableEntry g_threadTypes[];

class WOWD_SERVER_DECL WowdThread : public ThreadBase
{
    friend class ThreadMgr;
public:
    WowdThread();
    ~WowdThread();

    inline void SetThreadState(WowdThreadState thread_state) { ThreadState = thread_state; }
    inline WowdThreadState GetThreadState() { return ThreadState; }
    inline WowdThreadType GetThreadType() { return ThreadType; }
    int GetThreadId() { return ThreadId; }
    time_t GetStartTime() { return start_time; }
    virtual void run();
    void manual_start();

protected:
    WowdThreadState ThreadState;
    WowdThreadType  ThreadType;
    time_t start_time;
    int ThreadId;
};

class Creature;
class GameObject;
class EventableObjectHolder;

class ObjectUpdaterThread : public WowdThread
{
    friend class MapMgr;
public:
    ObjectUpdaterThread(MapMgr *mapMgr, int32 instance_id);
    ~ObjectUpdaterThread();

    void run();

    void Do();

    inline MapMgr* GetMapMgr() { return m_MapMgr; }

    template<class T> void AddObject(T* Obj);
    template<class T> void RemoveObject(T* Obj);

protected:

    MapMgr *m_MapMgr;

    typedef std::set<Creature*> UpdateableCreaturesSet;
    typedef std::set<GameObject*> UpdateableGameobjectsSet;

    UpdateableCreaturesSet creatures;
    Mutex creatureLock;

    UpdateableGameobjectsSet gameobjects;
    Mutex gameobjectLock;

    EventableObjectHolder * eventholder;
    int32 mInstance;
};

class UpdateObjectThread : public WowdThread
{
    friend class MapMgr;
    friend class ThreadMgr;
public:
    UpdateObjectThread(MapMgr *mapMgr);
    ~UpdateObjectThread();

    void run();

    inline MapMgr* GetMapMgr() { return m_MapMgr; }

protected:
    MapMgr* m_MapMgr;
};

#endif
