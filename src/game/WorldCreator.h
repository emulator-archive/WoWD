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
// WorldCreator.h
//

#ifndef __WORLDCREATOR_H
#define __WORLDCREATOR_H

enum INSTANCE_TYPE
{
    INSTANCE_NULL,
    INSTANCE_RAID,
    INSTANCE_NONRAID,
    INSTANCE_PVP,
    INSTANCE_MULTIMODE,
};

enum INSTANCE_ABORT_ERROR
{
    INSTANCE_ABORT_ERROR     = 0x00,
    INSTANCE_ABORT_FULL      = 0x01,
    INSTANCE_ABORT_NOT_FOUND = 0x02,
    INSTANCE_ABORT_TOO_MANY  = 0x03,
};

#define INSTANCE_OK  INSTANCE_ABORT_TOO_MANY + 0x01

class Map;
class MapMgr;

class Object;
class Group;
class Player;
class MapUpdaterThread;
class Battleground;

// first = mapid
// first = instance id
typedef std::map<uint32, std::map<uint32, std::pair<Map*, Battleground*> > > BattlegroundInstanceMap;

class WoWInstance
{
public:
    WoWInstance();
    ~WoWInstance();

    void SetInstanceMapMgr(MapMgr *mapmgr) { m_pInstanceHolder = mapmgr; }
    MapMgr *GetInstanceMapMgr() { return m_pInstanceHolder; }

    void SetMap(Map *pMap) { m_pMap = pMap; }
    Map *GetMap() { return m_pMap; }
    
    void   SetInstanceID(uint32 instance) { m_InstanceID = instance; }
    uint32 GetInstanceID() { return m_InstanceID; }

    void   SetCreator(Player *pPlayer) { m_pCreator = pPlayer->GetGUID(); }
    uint64 GetCreator() { return m_pCreator; }

    uint32 GetGroupSignature() { return m_GroupSignatureId; }
    void SetGroupSignature(uint32 iGroupSignature) { m_GroupSignatureId = iGroupSignature; }

    private:
    uint32 m_InstanceID;
    uint8 m_InstanceType;
    uint32 m_IntanceTimer;
    uint32 m_GroupSignatureId;
    uint64 m_pCreator;
    Map *m_pMap;
    MapMgr *m_pInstanceHolder;
};

struct Instance_Map_Id_List_Holder
{
public:
    typedef HM_NAMESPACE::hash_map<uint64, uint64> PlayerList;
    Instance_Map_Id_List_Holder();
    ~Instance_Map_Id_List_Holder();

    bool FindPlayer(uint64 guid);
    bool RemovePlayer(uint64 guid);
    void AddPlayer(uint64 guid);
    uint32 GetGroupSignature() { return mGroupSignature; }
    void SetGroupSignature(uint32 iGroupSignature) { mGroupSignature = iGroupSignature; }

private:
    PlayerList mPlayerList;
    uint32 mGroupSignature;
};

struct Instance_Map_Info_Holder
{
public:
    typedef HM_NAMESPACE::hash_map<uint32, Instance_Map_Id_List_Holder*> InstanceIdList;

    Instance_Map_Info_Holder();
    ~Instance_Map_Info_Holder();

    bool FindPlayer(uint64 guid, uint32 InstanceID, uint32 iGroupSignature);
    bool FindPlayer(uint64 guid, uint32 iGroupSignature);

    bool RemovePlayer(uint64 guid, uint32 InstanceID);
    bool RemovePlayer(uint64 guid);
    void RemoveInstanceId(uint32 instanceid);

    void AddPlayer(uint64 guid, uint32 InstanceID);
    void AddInstanceId(WoWInstance *pInstance);

    Instance_Map_Id_List_Holder *GetInstanceId(uint32 instanceid);

private:
    InstanceIdList mInstanceIdList;
};

class InstanceSavingManagement :  public Singleton < InstanceSavingManagement >
{
public:
    InstanceSavingManagement();
    ~InstanceSavingManagement();

    typedef HM_NAMESPACE::hash_map<uint32, Instance_Map_Info_Holder*> InstanceInfo;

    Instance_Map_Info_Holder *SaveInstance(WoWInstance *pInstance);
    void SavePlayerToInstance(Player *pPlayer, uint32 mapid); 
    void ResetSavedInstancesForPlayer(Player *pPlayer); 
    void RemoveSavedInstance(uint32 mapid, uint32 instanceid);
    void BuildSavedInstancesForPlayer(Player *pPlayer);

private:
    InstanceInfo mInstanceInfoList;
    Mutex _busy;
};



class WOWD_SERVER_DECL WorldCreator :  public Singleton < WorldCreator >
{
public:
    WorldCreator();    
    ~WorldCreator();

    typedef HM_NAMESPACE::hash_map<uint64, WoWInstance*> mInstanceMap;

    Map* GetMap(uint32 mapid);
    MapMgr * GetInstance(uint32 mapid, Object* obj);
    MapMgr * GetInstance(uint32 mapid, uint32 instanceId);
    MapMgr * GetInstance(uint32 instanceId);

    //Normal instance management
    uint32 CreateInstance(Group *pGroup, Player *pPlayer, uint32 mapid);
    void DeleteInstance(uint32 instanceid);

    WoWInstance *GetInstanceByGroup(Group *pGroup, MapInfo *pMapInfo);
    WoWInstance *GetInstanceByCreator(Player *pCreator, MapInfo *pMapInfo);
    void CheckInstanceForObject(Object *obj, MapInfo *pMapinfo);

    uint32 GenerateInstanceID();
    void InstanceSoftReset(MapMgr *mMapMgr);

    void CreateBattlegroundInstance(Battleground* m_Battleground);
    void DestroyBattlegroundInstance(Battleground* m_Battleground);

private:
    Map* _CreateMap(uint32 mapid);
    std::map<uint32, Map*> _maps;

    mInstanceMap     mInstanceList;
    void m_AddInstance(WoWInstance *pInstance);
    void m_RemoveInstance(WoWInstance *pInstance);
    uint32 m_InstanceHigh;

    //thread locks
    Mutex _busy;
};

#define sWorldCreator WorldCreator::getSingleton()
#define sInstanceSavingManager InstanceSavingManagement::getSingleton()

#endif
