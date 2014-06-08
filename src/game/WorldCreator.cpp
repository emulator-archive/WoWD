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
// WorldCreator.cpp
//

#include "StdAfx.h"

initialiseSingleton( WorldCreator );
initialiseSingleton( InstanceSavingManagement );

WorldCreator::WorldCreator()
{
    // Create all non-instance type maps.
    m_InstanceHigh = 0;

    for(World::MapInfoMap::iterator itr = sWorld.GetMapInfoBegin(); itr != sWorld.GetMapInfoEnd(); ++itr)
    {
        MapInfo * pInfo = itr->second;
        if(pInfo->type == INSTANCE_NULL)
        {
            // Create it (we're a non-instance).
            _CreateMap(pInfo->mapid);
        }
    }
}

WorldCreator::~WorldCreator()
{
    std::map<uint32, Map*>::iterator it;

    for(it = _maps.begin(); it != _maps.end(); it++)
    {
        delete it->second;
    }

    mInstanceMap::iterator iter, end;
    for( iter = mInstanceList.begin(), end = mInstanceList.end(); iter != end; iter++ )
    {
        delete iter->second;
    }
    mInstanceList.clear();
}

Map* WorldCreator::_CreateMap(uint32 mapid)
{
    std::map<uint32, Map*>::iterator it;
    it = _maps.find(mapid);
    ASSERT(it == _maps.end());

    Map *newmap = new Map(mapid);
    ASSERT(newmap);

    _maps.insert( std::pair<uint32, Map*>(mapid, newmap) );

    return newmap;
}

Map* WorldCreator::GetMap(uint32 mapid)
{
    std::map<uint32, Map*>::iterator it;
    it = _maps.find(mapid);

    if (it != _maps.end())
        return it->second;
    else
        return _CreateMap(mapid);
}

MapMgr* WorldCreator::GetInstance(uint32 mapid, Object* obj)
{
    Map* mp = GetMap(mapid);
    if(!mp) return NULL;
    return mp->GetInstance(obj);
}

MapMgr* WorldCreator::GetInstance(uint32 mapid, uint32 instanceId)
{
    Map* mp = GetMap(mapid);
    if(!mp) return NULL;
    return mp->GetInstance(instanceId);
}

MapMgr* WorldCreator::GetInstance(uint32 instanceId)
{
    std::map<uint32, Map*>::iterator it;
    MapMgr *instance;
    for(it = _maps.begin(); it != _maps.end(); it++)
    {
        instance = it->second->InstanceExists(instanceId);
        if(instance != NULL)
            return instance;
    }
    return NULL;
}

void WorldCreator::CreateBattlegroundInstance(Battleground* m_Battleground)
{
    Map* instance_map = GetMap(m_Battleground->GetMapId());
    // Create a new instance of this battleground.
    ASSERT(instance_map);
    
    uint32 instanceId = GenerateInstanceID();
    m_Battleground->SetInstanceID(instanceId);

    MapMgr * mapMgr = instance_map->CreateMapMgrInstance(instanceId);
    ASSERT(mapMgr);

    m_Battleground->SetMapMgr(mapMgr);
    m_Battleground->SetMap(instance_map);
}

void WorldCreator::DestroyBattlegroundInstance(Battleground* m_Battleground)
{
    Map* instance_map = GetMap(m_Battleground->GetMapId());
    ASSERT(instance_map != NULL);

    // this kills mapmgr
    instance_map->DestoryMapMgrInstance(m_Battleground->GetInstanceID());

    // null out remaining ptr's
    m_Battleground->SetMapMgr(NULL);
    m_Battleground->SetInstanceID(0);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Instance Manager
// instance creation and basic management
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void WorldCreator::CheckInstanceForObject(Object *obj, MapInfo *pMapinfo)
{
    WorldPacket data;
    Player *_player = NULL;

    if(obj->GetTypeId() == TYPEID_PLAYER)
    {
        _player = static_cast<Player*>(obj);
    }

    if(pMapinfo && _player)
    {
        switch(pMapinfo->type)
        {
            case INSTANCE_NONRAID:
                {
                    //instance creation detection types
                    //case 1, player is inside a group aka not soloing
                    //onoes: leader can be inside a instance already and make a group or its a fresh group, noone inside
                     
                     if(_player->InGroup()) //group created, join leader instance.
                     {
                         Group *pGroup = _player->GetGroup();
                         if(pGroup)
                         {
                             //check if instance already exists(player solo created instance and made group after)
                             WoWInstance *pInstance = sWorldCreator.GetInstanceByGroup(pGroup, pMapinfo);
                             if(pInstance)
                             {
                                 //INSTANCE_OK
                                 _player->SetInstanceID(pInstance->GetInstanceID());
                             }
                             else
                             {
                                //group leader didnt made any instance yet, create instance for this group.
                                uint32 id = sWorldCreator.CreateInstance(pGroup, pGroup->GetLeader(), pMapinfo->mapid);
                                if(id != NULL)
                                {
                                    //INSTANCE CREATED
                                    _player->SetInstanceID(id);
                                }
                                else
                                {
                                    data.Initialize(SMSG_TRANSFER_ABORTED);
                                    data << uint32(INSTANCE_ABORT_ERROR);
                                    _player->GetSession()->SendPacket(&data);
                                }
                             }
                         }
                         else
                         {
                             data.Initialize(SMSG_TRANSFER_ABORTED);
                             data << uint32(INSTANCE_ABORT_ERROR);
                             _player->GetSession()->SendPacket(&data);
                         }
                     }
                     else
                     {
                         WoWInstance *pInstance = sWorldCreator.GetInstanceByCreator(_player, pMapinfo);
                         if(pInstance)
                         {
                              //INSTANCE_OK
                             _player->SetInstanceID(pInstance->GetInstanceID());
                         }
                         else
                         {
                             uint32 id2 = sWorldCreator.CreateInstance(NULL, _player, pMapinfo->mapid);
                             if(id2 != NULL)
                             {
                                _player->SetInstanceID(id2);
                             }
                             else
                             {
                                data.Initialize(SMSG_TRANSFER_ABORTED);
                                data << uint32(INSTANCE_ABORT_ERROR);
                                _player->GetSession()->SendPacket(&data);
                             }
                         }
                     }
                }break;
            case INSTANCE_RAID:
                {

                }break;
            case INSTANCE_MULTIMODE:
                {

                }break;
        }
    }
}

uint32 WorldCreator::CreateInstance(Group *pGroup, Player *pPlayer, uint32 mapid)
{
    if(pGroup == NULL && pPlayer == NULL)
    {
        return INSTANCE_ABORT_ERROR;
    }

    WoWInstance *pInstance;
    pInstance = new WoWInstance;

    if(pGroup)
    {
        pInstance->SetGroupSignature(pGroup->GetID());
    }
    if(pPlayer)
    {
        pInstance->SetCreator(pPlayer);
    }

    Map* instance_map = GetMap(mapid);
    ASSERT(instance_map != NULL);

    MapMgr * mapMgr = instance_map->CreateMapMgrInstance();
    ASSERT(mapMgr);

    pInstance->SetInstanceID(mapMgr->GetInstanceID());
    pInstance->SetInstanceMapMgr(mapMgr);
    pInstance->SetMap(instance_map);

    m_AddInstance(pInstance);

    Instance_Map_Info_Holder *pMapHolder = sInstanceSavingManager.SaveInstance(pInstance); //first time instance saving holder
    return mapMgr->GetInstanceID();
}

WoWInstance *WorldCreator::GetInstanceByGroup(Group *pGroup, MapInfo *pMapInfo)
{
    mInstanceMap::iterator iter, end;
    for( iter = mInstanceList.begin(), end = mInstanceList.end(); iter != end; iter++ )
    {
        WoWInstance *pInstance = iter->second;
        //check if this group rly exists in that instance
        if(pInstance->GetGroupSignature() == pGroup->GetID() && pInstance->GetInstanceMapMgr()->GetMapId() == pMapInfo->mapid) // > 1
        {
            return pInstance;
        }
        else //instance was not made with a group, prolly leader made a group, attach group to this instance and return it
        {
            //check if the instance creator leader matchs the new group created.
            if(pGroup->GetLeader()->GetGUID() == pInstance->GetCreator()  && pInstance->GetInstanceMapMgr()->GetMapId() == pMapInfo->mapid)
            {
                pInstance->SetGroupSignature(pGroup->GetID());
                return pInstance;
            }
        }
    }
    return NULL;
}

WoWInstance *WorldCreator::GetInstanceByCreator(Player *pCreator, MapInfo *pMapInfo)
{
    mInstanceMap::iterator iter, end;
    for( iter = mInstanceList.begin(), end = mInstanceList.end(); iter != end; iter++ )
    {
        WoWInstance *pInstance = iter->second;
        //check if creator exists, to avoid problems we dont compare pointer address to detect if they are true,
        //better use guid cause player can logout etc and login back to the instance with a diference address.
        if(pInstance->GetCreator() && pCreator && !pInstance->GetGroupSignature() && pInstance->GetInstanceMapMgr()->GetMapId() == pMapInfo->mapid)
        {
            if(pInstance->GetCreator() == pCreator->GetGUID())
            {
                return pInstance;
            }
        }
    }
    return NULL;
}

uint32 WorldCreator::GenerateInstanceID()
{
    m_InstanceHigh++;
    return m_InstanceHigh;
}

void WorldCreator::m_AddInstance(WoWInstance *pInstance)
{
    ASSERT( pInstance->GetInstanceID() );
    ASSERT( mInstanceList.find(pInstance->GetInstanceID()) == mInstanceList.end() );

    mInstanceList[pInstance->GetInstanceID()] = pInstance;
}

void WorldCreator::DeleteInstance(uint32 instanceid)
{
    Guard guard(_busy);

    mInstanceMap::iterator itr = mInstanceList.find( instanceid );
    if(itr != mInstanceList.end())
    {
         WoWInstance *pInstance = itr->second;
         pInstance->GetMap()->DestoryMapMgrInstance(pInstance->GetInstanceID());
         delete pInstance;
         mInstanceList.erase(itr);
         sLog.outDebug("Instance %i deleted with success\n", instanceid);
    }
    else
    {
        sLog.outDebug("Instance %i failed to be deleted, reason: Doesnt Exist\n", instanceid);
    }
}

void WorldCreator::InstanceSoftReset(MapMgr *mMapMgr)
{
    uint32 mapid = mMapMgr->GetMapId();
    uint32 instanceid = mMapMgr->GetInstanceID();
    //delete public instance WoWInstance to avoid "rats".
     DeleteInstance(mMapMgr->GetInstanceID());
    //delete instance save data
    sInstanceSavingManager.RemoveSavedInstance(mapid, instanceid);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Instance Saving Management
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
InstanceSavingManagement::InstanceSavingManagement()
{

}

InstanceSavingManagement::~InstanceSavingManagement()
{
    InstanceInfo::const_iterator itr;
    for (itr = mInstanceInfoList.begin();itr != mInstanceInfoList.end(); itr++)
    {
        Instance_Map_Info_Holder *p = itr->second;
        delete p;
    }
    mInstanceInfoList.clear();
}

void InstanceSavingManagement::BuildSavedInstancesForPlayer(Player *pPlayer)
{
    bool result = false;
    bool hasBeenSaved = false;
    WorldPacket data;

    InstanceInfo::const_iterator itr;
    for (itr = mInstanceInfoList.begin();itr != mInstanceInfoList.end(); itr++)
    {
        Instance_Map_Info_Holder *p = itr->second;
        result = p->FindPlayer(pPlayer->GetGUID(), NULL); //find only non grouped results
        if(result)
        {
            data.Initialize(SMSG_INSTANCE_SAVE);
            data << uint32(itr->first);
            pPlayer->GetSession()->SendPacket(&data);

            data.Initialize(SMSG_INSTANCE_RESET_ACTIVATE);
            data << uint32(0x01);
            pPlayer->GetSession()->SendPacket(&data);

            hasBeenSaved = true;
        }
    }

    if(!hasBeenSaved)
    {
        data.Initialize(SMSG_INSTANCE_RESET_ACTIVATE);
        data << uint32(0x00);
        pPlayer->GetSession()->SendPacket(&data);
    }
}

Instance_Map_Info_Holder *InstanceSavingManagement::SaveInstance(WoWInstance *pInstance)
{
    ASSERT( pInstance->GetInstanceID() );


    InstanceInfo::iterator itr = mInstanceInfoList.find( pInstance->GetInstanceMapMgr()->GetMapId() );
    if(itr == mInstanceInfoList.end())
    {
        Instance_Map_Info_Holder *mapholder;

        mapholder = new Instance_Map_Info_Holder;

        mapholder->AddInstanceId(pInstance);

        mInstanceInfoList[pInstance->GetInstanceMapMgr()->GetMapId()] = mapholder;

        return mapholder;
    }
    else
    {
        Instance_Map_Info_Holder *mapholder = itr->second;
        mapholder->AddInstanceId(pInstance);
        return mapholder;
    }
    return NULL;
}

void InstanceSavingManagement::SavePlayerToInstance(Player *pPlayer, uint32 mapid)
{
    WorldPacket data;

    InstanceInfo::iterator itr = mInstanceInfoList.find( mapid );
    if(itr != mInstanceInfoList.end())
    {
        bool result = itr->second->FindPlayer(pPlayer->GetGUID(), (pPlayer->InGroup() ? pPlayer->GetGroup()->GetID() : 0));
        if(result)
        {
            return;
        }
        else
        {
            if(pPlayer->GetInstanceID()) //check if player is really on a instanceid
            {
                itr->second->AddPlayer(pPlayer->GetGUID(), pPlayer->GetInstanceID());

                if(!pPlayer->InGroup())
                {
                    data.Initialize(SMSG_INSTANCE_SAVE);
                    data << mapid;
                    pPlayer->GetSession()->SendPacket(&data);

                    data.Initialize(SMSG_INSTANCE_RESET_ACTIVATE);
                    data << uint32(0x01);
                    pPlayer->GetSession()->SendPacket(&data);
                }
            }
        }
    }
}

void InstanceSavingManagement::ResetSavedInstancesForPlayer(Player *pPlayer)
{
    bool result = false;
    WorldPacket data;

    InstanceInfo::const_iterator itr;
    for (itr = mInstanceInfoList.begin();itr != mInstanceInfoList.end(); itr++)
    {
        Instance_Map_Info_Holder *p = itr->second;
        result = p->RemovePlayer(pPlayer->GetGUID());
        if(result)
        {
            data.Initialize(SMSG_RESET_INSTANCE);
            data << itr->first;
            pPlayer->GetSession()->SendPacket(&data);
        }
    }
}

void InstanceSavingManagement::RemoveSavedInstance(uint32 mapid, uint32 instanceid)
{
    //my concept: while the real instance data is deleted from worldcreator,  the player saving remain until the real
    //reset comes from MapMgr wich should be a "ghost" map by then without players going in or out.
    //why is this? simple, due to players be abling to reset their instance saving. this method is more safe to avoid conflits
    //or worse :P bugs.
    Guard guard(_busy);

    ASSERT( mapid );
    ASSERT( instanceid );

    InstanceInfo::iterator itr = mInstanceInfoList.find( mapid );
    if(itr != mInstanceInfoList.end())
    {
        Instance_Map_Info_Holder *p1 = itr->second;
        p1->RemoveInstanceId(instanceid);
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Instance Management - MapID InstanceId list holder
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Instance_Map_Info_Holder::Instance_Map_Info_Holder()
{

}
Instance_Map_Info_Holder::~Instance_Map_Info_Holder()
{
    //clear all instanceids inside this mapid
    InstanceIdList::const_iterator itr;
    for (itr = mInstanceIdList.begin();itr != mInstanceIdList.end(); itr++)
    {
         Instance_Map_Id_List_Holder *p = itr->second;
         delete p;
    }
    mInstanceIdList.clear();
}

void Instance_Map_Info_Holder::RemoveInstanceId(uint32 instanceid)
{
    InstanceIdList::iterator itr = mInstanceIdList.find( instanceid );
    if(itr != mInstanceIdList.end())
    {
        delete itr->second;
        mInstanceIdList.erase(itr);
    }
}

Instance_Map_Id_List_Holder *Instance_Map_Info_Holder::GetInstanceId(uint32 instanceid)
{
    InstanceIdList::iterator itr = mInstanceIdList.find( instanceid );
    if(itr != mInstanceIdList.end())
    {
        return itr->second;
    }
    return NULL;
}

void Instance_Map_Info_Holder::AddInstanceId(WoWInstance *pInstance)
{
    ASSERT( pInstance->GetInstanceID() );

    InstanceIdList::iterator itr = mInstanceIdList.find( pInstance->GetInstanceID() );
    if(itr == mInstanceIdList.end())
    {
        Instance_Map_Id_List_Holder *pIdList;
        pIdList = new Instance_Map_Id_List_Holder;

        pIdList->SetGroupSignature(pInstance->GetGroupSignature()); //allows multiple instanceids per mapid

        mInstanceIdList[pInstance->GetInstanceID()] = pIdList;
    }
}

void Instance_Map_Info_Holder::AddPlayer(uint64 guid, uint32 InstanceID)
{
    ASSERT( InstanceID );

    InstanceIdList::iterator itr = mInstanceIdList.find( InstanceID );
    if(itr != mInstanceIdList.end())
    {
         Instance_Map_Id_List_Holder *p = itr->second;
         p->AddPlayer(guid);
    }
}

bool Instance_Map_Info_Holder::RemovePlayer(uint64 guid, uint32 InstanceID)
{
    ASSERT( InstanceID );
    bool result = false;

    InstanceIdList::iterator itr = mInstanceIdList.find( InstanceID );
    if(itr != mInstanceIdList.end())
    {
         Instance_Map_Id_List_Holder *p = itr->second;
         result = p->RemovePlayer(guid);
         return result;
    }
    return false;
}

bool Instance_Map_Info_Holder::RemovePlayer(uint64 guid)
{
    bool result = false;
    bool result2 = false;

    InstanceIdList::iterator itr, itr2;
    for (itr = mInstanceIdList.begin();itr != mInstanceIdList.end();)
    {
         itr2 = itr;
         ++itr;
         Instance_Map_Id_List_Holder *p = itr2->second;
         if(!p->GetGroupSignature()) //only resets solo instances
         {
             result = p->RemovePlayer(guid);
             if(result)
             {
                 sWorldCreator.DeleteInstance(itr2->first);
                 delete p;
                 sLog.outDebug("Removing instance from the itr\n");
                 mInstanceIdList.erase(itr2);
                 result2 = true;
             }
         }
    }
    return result2;
}

bool Instance_Map_Info_Holder::FindPlayer(uint64 guid, uint32 iGroupSignature)
{
    InstanceIdList::const_iterator itr;
    for (itr = mInstanceIdList.begin();itr != mInstanceIdList.end(); itr++)
    {
        Instance_Map_Id_List_Holder *p = itr->second;
         // group matches, returns false and breaks loop if it fails
         // cant exist 2 groups with same id  so no point in continue loop
        if(iGroupSignature)
        {
            if(p->GetGroupSignature() == iGroupSignature)
            {
                bool result = p->FindPlayer(guid);
                if(result) { return true; }

                return false;
            }
        }
        else
        {
            bool result = p->FindPlayer(guid);
            if(result) { return true; }
        }
    }
    return false;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Instance Management - InstanceId player list holder
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Instance_Map_Id_List_Holder::Instance_Map_Id_List_Holder()
{

}

Instance_Map_Id_List_Holder::~Instance_Map_Id_List_Holder()
{
    mPlayerList.clear();
}

bool Instance_Map_Id_List_Holder::FindPlayer(uint64 guid)
{
    PlayerList::iterator itr = mPlayerList.find( guid );
    if(itr == mPlayerList.end())
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Instance_Map_Id_List_Holder::AddPlayer(uint64 guid)
{
    mPlayerList[guid] = guid;
}

bool Instance_Map_Id_List_Holder::RemovePlayer(uint64 guid)
{
    PlayerList::iterator itr = mPlayerList.find( guid );
    if(itr == mPlayerList.end())
    {
        //error
        return false;
    }
    else
    {
        mPlayerList.erase(itr);
        return true;
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Instance
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
WoWInstance::WoWInstance()
{
     m_pMap = NULL;
    m_pInstanceHolder = NULL;
    m_pCreator = 0;

    m_InstanceID = 0;
    m_InstanceType = 0;
    m_IntanceTimer = 0;
    m_pInstanceHolder = 0;
    m_GroupSignatureId = 0;

}

WoWInstance::~WoWInstance()
 {


 }


