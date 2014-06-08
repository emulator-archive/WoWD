//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////

// Class Map
// Holder for all instances of each mapmgr, handles transferring
// players between, and template holding.

#include "StdAfx.h"

Map::Map(uint32 mapid)
{
    _template = sTemplateMgrHolder.GetTemplateMgr(mapid);
    _mapInfo = sWorld.GetMapInformation(mapid);
    _mapId = mapid;

    bool instance = true;
    if(_mapInfo)
    {
        // for non-instances, create our one instance.
        if(_mapInfo->type == INSTANCE_NULL)
        {
            instance = false;
        }
    } else {
        if(_mapId < 2)
        {
            instance = false;
        }
    }

    // Setup terrain
    _terrain = new TerrainMgr(sWorld.MapPath, _mapId, instance);

    if(!instance)
        CreateMapMgrInstance();

    // get our name
    me = sMapStore.LookupEntry(_mapId);
    if(me)
    {
        name = sMapStore.LookupString(me->real_name);
    }
    else
    {
        name = "Unknown";
    }
    
    if(mapid < 2 || mapid == 530)
        sLog.outDetail("  Map handler for '%s' (mapId %u) created, %u instances spawned.\n", GetName(), mapid, _instances.size());
}

Map::~Map()
{
    sLog.outString("  Deleting all instances of map %u", _mapId);
    for(std::map<uint32, MapMgr*>::iterator it = _instances.begin();
        it != _instances.end(); ++it)
    {
        delete it->second;
    }
    _instances.clear();
    delete _terrain;
}

MapMgr * Map::GetInstance(uint32 instanceId)
{
    if(_mapInfo && _mapInfo->type == INSTANCE_NULL)
        return GetFirstInstance();
    else if(_mapId == 0 || _mapId == 1 || _mapId == 530)
        return GetFirstInstance();

    // we called the wrong number? :P
    InstanceMap::iterator itr = _instances.find(instanceId);
    if(itr == _instances.end())
        return NULL;

    return itr->second;
}

MapMgr * Map::InstanceExists(uint32 instanceId)
{
    // we called the wrong number? :P
    InstanceMap::iterator itr = _instances.find(instanceId);
    if(itr == _instances.end())
        return NULL;

    return itr->second;
}

MapMgr * Map::GetInstance(Object* obj)
{
    // lazy
    MapMgr * mapMgr = GetInstance(obj->GetInstanceID());
    if(!mapMgr)
    {
        // Oops, we're trying to join an invalid instance
        if(obj->GetTypeId() == TYPEID_PLAYER)
        {
            Player *plr = static_cast<Player*>(obj);
            sChatHandler.RedSystemMessage(plr->GetSession(), "You tried to join an invalid instance (%u on map %u). Repopping at %s.", obj->GetInstanceID(), _mapId, (plr->m_bgEntryPointX != 0.0f ? "entry point" : "exit battleground"));
            if(plr->m_bgEntryPointX != 0.0f)
            {
                plr->SetPosition(plr->m_bgEntryPointX, plr->m_bgEntryPointY, plr->m_bgEntryPointZ,
                    plr->m_bgEntryPointO, true);
                plr->SetMapId(plr->m_bgEntryPointMap);
                plr->SetInstanceID(plr->m_bgEntryPointInstance);
            } else if(_mapInfo != NULL) {
                plr->SetMapId(_mapInfo->repopmapid);
                plr->SetPosition(_mapInfo->repopx, _mapInfo->repopy, _mapInfo->repopz, 3.14f);
                plr->SetInstanceID(0);
            } else {
                PlayerCreateInfo *Info = objmgr.GetPlayerCreateInfo(plr->getRace(), plr->getClass());
                plr->SetMapId(Info->mapId);
                plr->SetInstanceID(Info->mapId+1);
                plr->SetPosition(Info->positionX, Info->positionY, Info->positionZ, 0, true);
            }
            plr->_Relocate(plr->GetMapId(), plr->GetPosition(), true, true);
            return NULL;
        } else {
            // this will destroy the creature :p
            return 0;
        }
    }
    return mapMgr;
}

MapMgr * Map::CreateMapMgrInstance(uint32 instanceid)
{
    uint32 instanceId;
    if(instanceid)
        instanceId = instanceid;
    else
        instanceId = sWorldCreator.GenerateInstanceID();

    //sLog.outError("    Creating new instance of map %u (Instance ID: %u)", _mapId, instanceId);
    MapMgr *mapMgr = new MapMgr(this, _mapId, instanceId);

    ASSERT(_instances.find(instanceId) == _instances.end());
    _instances.insert( InstanceMap::value_type( instanceId, mapMgr ) );

    return mapMgr;
}

void Map::DestoryMapMgrInstance(uint32 instanceId)
{
    InstanceMap::iterator it = _instances.find(instanceId);
    ASSERT(it != _instances.end());

    sLog.outError("Deleting instance %u of map %u",
        instanceId, _mapId);

    delete it->second;
    _instances.erase(it);
}

MapMgr * Map::GetFirstInstance()
{
    InstanceMap::iterator it = _instances.begin();
    ASSERT(it != _instances.end());

    return it->second;
}

