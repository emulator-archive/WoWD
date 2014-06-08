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
// MapCell.cpp
//
#include "StdAfx.h"

MapCell::~MapCell()
{
    //sLog.outDebug("Unloading Idle Cell [%d][%d] on map %d...",
        //this->GetX(), this->GetY(), this->_mapid);

    this->RemoveObjects();
}

void MapCell::Init(uint32 x, uint32 y, uint32 mapid, MapMgr *mapmgr)
{
    _mapmgr = mapmgr;
    _active = false;
    _loaded = false;
    _playerCount = 0;
    _x = x;
    _y = y;
    _mapid = mapid;
    _timeWentIdle = time(NULL);
    _loadedObjects = false;
    sWorld.mInactiveCellCount++;
    sWorld.mTotalCells++;
}

void MapCell::AddObject(Object *obj)
{
    if (obj->GetTypeId() == TYPEID_PLAYER)
    {
        _playerCount++;
        if(_active && _playerCount == 1)
        {
            sWorld.mHalfActiveCellCount--;
            sWorld.mActiveCellCount++;
        }
    }

    ObjectSet::iterator itr = _objects.find(obj);

    if (itr == _objects.end())
        _objects.insert(obj);
}

void MapCell::RemoveObject(Object *obj)
{
    if(obj->GetTypeId() == TYPEID_PLAYER)
    {
        _playerCount--;
        
        if(_active && _playerCount == 0)
        {
            sWorld.mHalfActiveCellCount++;
            sWorld.mActiveCellCount--;
        }

        /*if(_playerCount == 0 && _active)   // We're the last player
        {
            _mapmgr->UpdateCellActivity(_x, _y, 2);
        }*/
    }

    ObjectSet::iterator itr = _objects.find(obj);

    if (itr != _objects.end())
        _objects.erase(itr);
}

void MapCell::SetActivity(bool state)
{
    if(!_active && state)
    {
        // Load cell information
        _mapmgr->GetBaseMap()->GetTerrainMgr()->CellGoneActive(_x, _y);

        // Move all objects to active set.
        for(ObjectSet::iterator itr = _objects.begin(); itr != _objects.end(); ++itr)
        {
            if(!(*itr)->_Activated)
                continue;

            switch(UINT32_LOPART((*itr)->GetGUIDHigh()))
            {
            case HIGHGUID_UNIT:
                _mapmgr->_objectUpdaterThread->AddObject<Creature>(static_cast<Creature*>(*itr));
                break;
            case HIGHGUID_GAMEOBJECT:
                _mapmgr->_objectUpdaterThread->AddObject<GameObject>(static_cast<GameObject*>(*itr));
                break;
            }
        }
        if(_playerCount == 0)
            sWorld.mHalfActiveCellCount++;
        else
            sWorld.mActiveCellCount++;

    } else if(_active && !state && _mapmgr->_objectUpdaterThread)
    {
        // Unload cell information
        _mapmgr->GetBaseMap()->GetTerrainMgr()->CellGoneIdle(_x, _y);

        if(_playerCount == 0)
            sWorld.mHalfActiveCellCount--;
        else
            sWorld.mActiveCellCount--;

        // Remove all objects from active set.
        for(ObjectSet::iterator itr = _objects.begin(); itr != _objects.end(); ++itr)
        {
            if(!(*itr)->_Activated)
                continue;

            switch(UINT32_LOPART((*itr)->GetGUIDHigh()))
            {
            case HIGHGUID_UNIT:
                _mapmgr->_objectUpdaterThread->RemoveObject<Creature>(static_cast<Creature*>(*itr));
                break;
            case HIGHGUID_GAMEOBJECT:
                _mapmgr->_objectUpdaterThread->RemoveObject<GameObject>(static_cast<GameObject*>(*itr));
                break;
            }
        }
    }

    _active = state; 

    if (!state)
        _timeWentIdle = time(NULL);
}

void MapCell::RemoveObjects()
{
    ObjectSet::iterator itr;
    uint32 count = 0;
    uint32 ltime = getMSTime();

    //This time it's simpler! We just remove everything :)
    for(itr = _objects.begin(); itr != _objects.end(); )
    {
        count++;

        Object *obj = (*itr);

        itr++;

        if (obj->IsInWorld())
            obj->RemoveFromWorld();
        /*else
            sWorld.RemoveGlobalObject(obj);*/

        if (obj->GetTypeId() == TYPEID_PLAYER)
            sObjHolder.Delete((Player*)(obj));
        else if (obj->GetTypeId() == TYPEID_UNIT)
            sObjHolder.Delete((Creature*)(obj));
        else if (obj->GetTypeId() == TYPEID_GAMEOBJECT)
            sObjHolder.Delete((GameObject*)(obj));
        else if (obj->GetTypeId() == TYPEID_DYNAMICOBJECT)
            sObjHolder.Delete((DynamicObject*)(obj));
        else if (obj->GetTypeId() == TYPEID_CORPSE)
            sObjHolder.Delete((Corpse*)(obj));
    }

    //sLog.outDebug(" %d Objects Unloaded.", count);
    //sLog.outDebug(" Cell Unloaded in %dms.", getMSTime() - ltime);

    _objects.clear();
    _playerCount = 0;
    _loadedObjects = false;
}

void MapCell::LoadObjects()
{
    ASSERT(!this->_loaded);
    if(_loadedObjects)
    {
        sLog.outError("Dual cell load!");
        return;
    }
    _loadedObjects = true;
    //_lock.Acquire();

    uint32 ltime = getMSTime();
    uint32 count;    
    TemplateCell *tcl;
    tcl = _mapmgr->GetBaseMap()->GetTemplate()->GetCell(this->GetX(), this->GetY());
    if (!tcl) //Cell is empty
    {
        sLog.outDebug(" 0 Creatures Loaded.");
        sLog.outDebug(" 0 GameObjects Loaded.");
        sLog.outDebug(" 0 Corpses Loaded.");
        sLog.outDebug(" Cell Loaded in %dms.", getMSTime() - ltime);
        //_lock.Release();
        _loaded = true;
        return;
    }

    Creature* pCreature;
    GameObject* pGameObject;

    //Load Creatures
    std::map<uint32, ObjectTemplate*>::iterator itr;
    count = 0;

    for(itr = tcl->GetMapBegin<Creature>(); itr != tcl->GetMapEnd<Creature>(); itr++ )
    {
#ifdef PRECACHE_LOAD
        pCreature = objmgr.LoadCreature((CreatureTemplate*)itr->second, _mapmgr->GetInstanceID());
#else
        pCreature = objmgr.LoadCreature(itr->first, _mapmgr->GetInstanceID());
#endif

        if(_active && pCreature != NULL)
            _mapmgr->_objectUpdaterThread->AddObject<Creature>(pCreature);

        count++;
    }
    sLog.outDebug(" %d Creatures Loaded.", count);

    //Load GameObjects
    count = 0;
    for(itr = tcl->GetMapBegin<GameObject>(); itr != tcl->GetMapEnd<GameObject>(); itr++ )
    {
#ifdef PRECACHE_LOAD
        pGameObject = objmgr.LoadGameObject((GameObjectTemplate*)itr->second, _mapmgr->GetInstanceID());
#else
        pGameObject = objmgr.LoadGameObject(itr->first, _mapmgr->GetInstanceID());
#endif

        if(_active && pGameObject != NULL)
            _mapmgr->_objectUpdaterThread->AddObject<GameObject>(pGameObject);

        count++;
    }
    sLog.outDebug(" %d GameObjects Loaded.", count);
    sLog.outDebug(" Cell Loaded in %dms.", getMSTime() - ltime);
    _loaded = true;
}

