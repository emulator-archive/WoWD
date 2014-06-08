// Copyright (C) 2004 WoW Daemon
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

//
// MapMgr.h
//

#ifndef __MAPMGR_H
#define __MAPMGR_H

class MapCell;
class Map;
class Object;
class InstanceScript;
class MapScriptInterface;

enum MapMgrTimers
{
    MMUPDATE_OBJECTS = 0,
    MMUPDATE_SESSIONS = 1,
    MMUPDATE_FIELDS = 2,
    MMUPDATE_IDLE_OBJECTS = 3,
    MMUPDATE_ACTIVE_OBJECTS = 4,
    MMUPDATE_COUNT = 5
};

typedef std::set<Object*> AObjectSet;

enum ObjectActiveState
{
    OBJECT_STATE_NONE     = 0,
    OBJECT_STATE_INACTIVE = 1,
    OBJECT_STATE_ACTIVE   = 2,
};

typedef std::set<Object*> ObjectSet;
typedef std::set<Object*> UpdateQueue;
typedef std::set<Player*> PUpdateQueue;
typedef std::set<Player*> PlayerSet;

#define CALL_MAP_SCRIPT_EVENT(obj, func) if(obj->GetScript() != NULL) obj->GetScript()->func

class WOWD_SERVER_DECL MapMgr : public CellHandler <MapCell>, public EventableObject
{
    friend class UpdateObjectThread;
    friend class ObjectUpdaterThread;
    friend class MapCell;
    friend class MapScriptInterface;
public:
    MapMgr(Map *map, uint32 mapid, uint32 instanceid);
    ~MapMgr();

    typedef HM_NAMESPACE::hash_map<uint64, Object*> ObjectMap;

    void AddObject(Object *obj);
    void RemoveObject(Object *obj);
    void ChangeObjectLocation(Object *obj); // update inrange lists
    void ChangeFarsightLocation(Player *plr, Creature *farsight);

    ObjectMap::iterator GetObjectsBegin() { return _objects.begin(); }
    ObjectMap::iterator GetObjectsEnd() { return _objects.end(); }

    //! Mark object as updated
    void ObjectUpdated(Object *obj);
    void UpdateCellActivity(uint32 x, uint32 y, int radius, bool lock = true);

    // Terrain Functions
    inline float  GetLandHeight(float x, float y) { return GetBaseMap()->GetTerrainMgr()->GetLandHeight(x, y); }
    inline float  GetWaterHeight(float x, float y) { return GetBaseMap()->GetTerrainMgr()->GetWaterHeight(x, y); }
    inline uint8  GetWaterType(float x, float y) { return GetBaseMap()->GetTerrainMgr()->GetWaterType(x, y); }
    inline uint8  GetWalkableState(float x, float y) { return GetBaseMap()->GetTerrainMgr()->GetWalkableState(x, y); }
    inline uint16 GetAreaID(float x, float y) { return GetBaseMap()->GetTerrainMgr()->GetAreaID(x, y); }

    inline uint32 GetMapId() { return _mapId; }
    inline uint32 GetObjectCount() { return _objects.size(); }

    void PushToProcessed(Player* plr);

    void WaitForUnlockNearbyCells(uint32 x, uint32 y, uint32 radius);
    void LockNearbyCells(uint32 x, uint32 y, uint32 radius);
    void UnlockNearbyCells(uint32 x, uint32 y, uint32 radius);

    void AddSession(WorldSession *session);
    void RemoveSession(WorldSession *session);
    inline bool HasPlayers() { return (_players.size() > 0); }
    void SoftReset();
    
    void CallScriptUpdate();
    void RegisterScriptUpdater(uint32 Frequency);

    void Respawn(bool RemoveOld, bool SpawnNew);
    void SpawnEvents();
    inline void SetResetPending(bool val) { reset_pending = val; }

    uint32 GetInstanceID() { return m_instanceID; }
    MapInfo *GetMapInfo() { return pMapInfo; }

    bool _shutdown;

    inline ObjectUpdaterThread* GetUpdaterThread() { return _objectUpdaterThread; }
    inline InstanceScript* GetScript() { return myScript; }

    inline MapScriptInterface * GetInterface() { return ScriptInterface; }
    int32 event_GetInstanceID() { return m_instanceID; }

    void LoadAllCells();

protected:

    //! Collect and send updates to clients
    void _UpdateObjects();

    InstanceScript* myScript;
private:
    //! Objects that exist on map
    ObjectMap _objects;
    PlayerSet _players;

    uint32 _mapId;

    bool _CellActive(uint32 x, uint32 y);
    void UpdateInRangeSet(Object *obj, Player *plObj, MapCell* cell, ByteBuffer *buf);

    // Distance a Player can "see" other objects and receive updates from them (!! ALREADY dist*dist !!)
    float m_UpdateDistance;

    /* Threading */
    UpdateObjectThread *_updateObjectThread;
    ObjectUpdaterThread *_objectUpdaterThread;

    /* Update System */
    UpdateQueue _updates;
    PUpdateQueue _processQueue;

    /* Sessions */
    typedef std::set<WorldSession*> SessionSet;
    SessionSet Sessions;

    /* Map Information */
    MapInfo *pMapInfo;

    Mutex _updateS;
    Mutex _processL;
    Mutex _busy;
    Mutex _activityS;

    uint32 m_instanceID;

    MapScriptInterface * ScriptInterface;
    bool reset_pending;
};

#endif
