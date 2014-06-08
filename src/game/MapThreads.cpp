//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////

// Class WowdThread - Base class for all threads in the
// server, and allows for easy management by ThreadMgr.

#include "StdAfx.h"

void ThreadMgr::OnMapMgrCreated(MapMgr *mapMgr)
{
    launch_thread(new ObjectUpdaterThread(mapMgr, mapMgr->GetInstanceID()));
    
    if(mapMgr->GetMapId() < 2 || mapMgr->GetMapId() == 530)      // Main continents
        launch_thread(new UpdateObjectThread(mapMgr));
}

ObjectUpdaterThread::ObjectUpdaterThread(MapMgr *mapMgr, int32 instance_id) : WowdThread()
{
    ThreadType = WOWD_THREADTYPE_OBJECTUPDATER;
    m_MapMgr = mapMgr;
    m_MapMgr->_objectUpdaterThread = this;
    mInstance = instance_id;
    eventholder = new EventableObjectHolder(instance_id);
}

ObjectUpdaterThread::~ObjectUpdaterThread()
{
    m_MapMgr->_objectUpdaterThread = NULL;
    delete eventholder;
}

template<>
void ObjectUpdaterThread::AddObject<Creature>(Creature* obj)
{
    ASSERT(obj->GetTypeId() == TYPEID_UNIT);
    ASSERT(obj->IsInWorld() && obj->GetMapMgr() && obj->GetMapCell() && obj->GetNewGUID().GetNewGuidLen());
    Guard guard(creatureLock);
    creatures.insert(obj);
    sWorld.mLoadedCreatures[0]--;
    sWorld.mLoadedCreatures[1]++;
}

template<>
void ObjectUpdaterThread::RemoveObject<Creature>(Creature* obj)
{
    ASSERT(obj->GetTypeId() == TYPEID_UNIT);
    Guard guard(creatureLock);
    creatures.erase(obj);
    sWorld.mLoadedCreatures[1]--;
    sWorld.mLoadedCreatures[0]++;
}

template<>
void ObjectUpdaterThread::AddObject<GameObject>(GameObject* obj)
{
    ASSERT(obj->GetTypeId() == TYPEID_GAMEOBJECT);
    ASSERT(obj->IsInWorld() && obj->GetMapMgr() && obj->GetMapCell() && obj->GetNewGUID().GetNewGuidLen());
    Guard guard(gameobjectLock);
    gameobjects.insert(obj);
    sWorld.mLoadedGameObjects[0]--;
    sWorld.mLoadedGameObjects[1]++;
}

template<>
void ObjectUpdaterThread::RemoveObject<GameObject>(GameObject* obj)
{
    ASSERT(obj->GetTypeId() == TYPEID_GAMEOBJECT);
    Guard guard(gameobjectLock);
    gameobjects.erase(obj);
    sWorld.mLoadedGameObjects[1]--;
    sWorld.mLoadedGameObjects[0]++;
}

void ObjectUpdaterThread::Do()
{
    uint32 last_time = getMSTime();
    uint32 last_cTime = getMSTime();
    uint32 last_gTime = getMSTime();
    uint32 diff = 0;
    uint32 mstime = getMSTime();
    uint32 cFlipper = 1;
    uint32 gFlipper = 3;
    uint32 mapid = m_MapMgr->GetMapId();
    Creature * pCreature;
    GameObject * pGameObject;

    int result;

    int objectUpdate = 1;
    UpdateableCreaturesSet::iterator citr, citr_end, citr_last;
    UpdateableGameobjectsSet::iterator gitr, gitr_end, gitr_last;
    PlayerSet::iterator pitr, pitr_end;
    Player * pPlayer;

    SessionSet::iterator itr, it2;
    WorldSession *session;

    while(ThreadState != WOWD_THREADSTATE_TERMINATE)
    {
        // Provision for pausing this thread.
        if(ThreadState == WOWD_THREADSTATE_PAUSED)
        {
            while(ThreadState == WOWD_THREADSTATE_PAUSED)
            {
                Sleep(200);
            }
        }

        mstime = getMSTime();
        last_time = mstime;

        if(cFlipper == 1)
        {
            citr_end = creatures.end();

            // possible uint32 overflow ~50days
            if(last_cTime > mstime)
                diff = 100;
            else
                diff = mstime - last_cTime;

            citr_end = creatures.end();
            for(citr = creatures.begin(); citr != citr_end;)
            {
                pCreature = (*citr);
                citr_last = citr;
                ++citr;
                if(!pCreature->IsInWorld() || !pCreature->GetMapCell()->IsActive())
                {
                    creatureLock.Acquire();
                    creatures.erase(citr_last);
                    creatureLock.Release();
                }
                else
                    pCreature->Update(diff);
            }
            cFlipper = 0;   // 2 loops away now. :)
            
            // update event holder
            eventholder->Update(diff);

            // update players
            pitr_end = m_MapMgr->_players.end();
            pitr = m_MapMgr->_players.begin();
            for(; pitr != pitr_end;)
            {
                pPlayer = (*pitr);
                ++pitr;
                pPlayer->Update(diff);                
            }

            last_cTime = mstime;
        } else {
            cFlipper = 1;   // Next loop we can have our cake. :)
        }

        if(gFlipper == 3)
        {
            if(last_gTime > mstime)
                diff = 300;
            else
                diff = mstime - last_gTime;

            gitr_end = gameobjects.end();
            for(gitr = gameobjects.begin(); gitr != gitr_end;)
            {
                pGameObject = (*gitr);
                gitr_last = gitr;
                ++gitr;
                if(!pGameObject->IsInWorld() || !pGameObject->GetMapCell()->IsActive())
                {
                    gameobjectLock.Acquire();
                    gameobjects.erase(gitr_last);
                    gameobjectLock.Release();
                }
                else
                    pGameObject->Update(diff);
            }
            gFlipper = 0;
            last_gTime = mstime;
        } else {
            ++gFlipper;
        }

        for(itr = m_MapMgr->Sessions.begin(); itr != m_MapMgr->Sessions.end();)
        {
            session = (*itr);
            it2 = itr;
            ++itr;

            if(result = session->Update(50, m_MapMgr->GetMapId()))
            {
                if(result == 1)
                {
                    // complete deletion
                    sWorld.DeleteSession(session);
                }
                m_MapMgr->Sessions.erase(it2);
            }
        }

        // instance thread object updates are done here
        if(mapid > 1 && mapid != 530)
            m_MapMgr->_UpdateObjects();        

        // Execution time compensation. :)
        mstime = getMSTime();
        if(last_time > mstime)
        {
            Sleep(MAPMGR_SESSION_UPDATE_DELAY);    // uint32 overflow
        }
        else
        {
            mstime = mstime - last_time;
            if(mstime < MAPMGR_SESSION_UPDATE_DELAY)
            {
                mstime = MAPMGR_SESSION_UPDATE_DELAY - mstime;
                if(mstime > 20)
                    Sleep(mstime);
            }
        }
    }
}

void ObjectUpdaterThread::run()
{
    SetThreadName("Object Updater - M%u|I%u", m_MapMgr->GetMapId(), m_MapMgr->GetInstanceID());
    WOWD_THREAD_TRY_EXECUTION2
    Do();
    WOWD_THREAD_HANDLE_CRASH2
}

/* Update Object Thread */
// Purpose: Collect updates from MapMgr and flush them to clients
// every 100ms

UpdateObjectThread::UpdateObjectThread(MapMgr *mapMgr) : WowdThread()
{
    ThreadType = WOWD_THREADTYPE_UDPATEOBJECT;
    m_MapMgr = mapMgr;
    m_MapMgr->_updateObjectThread = this;
}

UpdateObjectThread::~UpdateObjectThread()
{
    m_MapMgr->_updateObjectThread = NULL;
}

void UpdateObjectThread::run()
{
    SetThreadName("Update Object - Map %u Instance %u", m_MapMgr->GetMapId(), m_MapMgr->GetInstanceID());
    WOWD_THREAD_TRY_EXECUTION2

    WowdThread::run();

    uint32 last_time = getMSTime();
    uint32 diff, start_execute;
    uint32 execution_time;

    while(ThreadState != WOWD_THREADSTATE_TERMINATE)
    {
        // Provision for pausing this thread.
        if(ThreadState == WOWD_THREADSTATE_PAUSED)
        {
            while(ThreadState == WOWD_THREADSTATE_PAUSED)
            {
                Sleep(200);
            }
        }

        diff = getMSTime() - last_time;
        start_execute = getMSTime();
        last_time = start_execute;
        if(ThreadState == WOWD_THREADSTATE_TERMINATE)
            break;

        ThreadState = WOWD_THREADSTATE_BUSY;

        // now i'm just lazy
        m_MapMgr->_UpdateObjects();
        // end of body
        if(ThreadState == WOWD_THREADSTATE_TERMINATE)
            break;

        ThreadState = WOWD_THREADSTATE_SLEEPING;

        execution_time = getMSTime() - start_execute;
        if(execution_time >= MAPMGR_UPDATE_DELAY)
        {
            // this _shouldn't_ really happen.. :D
            sLog.outString("*** ThreadMgr: PUpdater lagging due to stress. Map=%u Cnt=%u Etm=%u", m_MapMgr->GetMapId(), m_MapMgr->GetObjectCount(), execution_time );
            // no need to sleep here. 
        } else {
            Sleep(MAPMGR_UPDATE_DELAY - execution_time);
        }
    }

    WOWD_THREAD_HANDLE_CRASH2
}

