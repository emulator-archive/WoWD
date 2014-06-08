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
// MapMgr.cpp
//

#include "StdAfx.h"

MapMgr::MapMgr(Map *map, uint32 mapId, uint32 instanceid) : _mapId(mapId), CellHandler<MapCell>(map)
{
    _shutdown = false;
    m_instanceID = instanceid;
    m_UpdateDistance = sWorld.GetUpdateDistance();
    pMapInfo = sWorld.GetMapInformation(mapId);

    _updateObjectThread = 0;
    _objectUpdaterThread = 0;

    ThreadMgr::getSingleton( ).OnMapMgrCreated(this);

    ScriptInterface = 0;
    reset_pending = false;

    // Create script interface
    ScriptInterface = new MapScriptInterface(*this);
    myScript = sScriptMgr.CreateInstanceScriptForMap(mapId, this);

    sEventMgr.AddEvent(this, &MapMgr::SpawnEvents, EVENT_MAP_SPAWN_EVENTS, 1000, 1);
}


MapMgr::~MapMgr()
{
    sEventMgr.RemoveEvents(this);
    delete ScriptInterface;

    // Remove objects
    sLog.outString("    Emptying all cells of objects...");
    if(_cells)
    {
        for (uint32 i = 0; i < _sizeX; i++)
        {
            for (uint32 j = 0; j < _sizeY; j++)
            {
                if(_cells[i][j] != 0)
                    _cells[i][j]->RemoveObjects();
            }
        }
    }

    sLog.outString("    Instance %d deleted (MapId: %u)" , m_instanceID, _mapId);
    sLog.outString("    Closing Map Threads...");
    
    m_eventHolder = 0;
    m_lastHolder = 0;
    event_SetActive(false);

    if(_updateObjectThread)
        _updateObjectThread->SetThreadState(WOWD_THREADSTATE_TERMINATE);
    if(_objectUpdaterThread)
        _objectUpdaterThread->SetThreadState(WOWD_THREADSTATE_TERMINATE);

    sLog.outString("    Waiting for threads to exit...");
    while(_updateObjectThread != NULL || _objectUpdaterThread != NULL)
        Sleep(1);

    sLog.outString("    Instance closed successfully.");
}


void MapMgr::AddObject(Object *obj)
{
    /////////////
    // Assertions
    /////////////

    ASSERT(obj);
    ASSERT(obj->GetInRangeSetBegin() == obj->GetInRangeSetEnd()); // make sure object is a virgin
    ASSERT(obj->GetMapId() == _mapId);
    ASSERT(obj->GetPositionX() < _maxX && obj->GetPositionX() > _minX);
    ASSERT(obj->GetPositionY() < _maxY && obj->GetPositionY() > _minY);
    ASSERT(_cells);

    // That object types are not map objects. TODO: add AI groups here?
    if(obj->GetTypeId() == TYPEID_ITEM || obj->GetTypeId() == TYPEID_CONTAINER)
    {
        // mark object as updatable and exit
        return;
    }

    _busy.Acquire();

    ASSERT(!obj->GetMapCell());

    ///////////////////////
    // Get cell coordinates
    ///////////////////////

    uint32 x = GetPosX(obj->GetPositionX());
    uint32 y = GetPosY(obj->GetPositionY());

    MapCell *objCell = GetCell(x,y);
    if (!objCell)
    {
        objCell = Create(x,y);
        objCell->Init(x, y, _mapId, this);
    }

    uint32 endX = x < _sizeX ? x + 1 : _sizeX;
    uint32 endY = y < _sizeY ? y + 1 : _sizeY;
    uint32 startX = x > 0 ? x - 1 : 0;
    uint32 startY = y > 0 ? y - 1 : 0;
    uint32 posX, posY;
    MapCell *cell;
    MapCell::ObjectSet::iterator iter;

    ByteBuffer buf(2500);
    uint32 count;
    Player *plObj;

    if(obj->GetTypeId() == TYPEID_PLAYER)
        plObj = ((Player*)obj);
    else
        plObj = NULL;

    //////////////////////
    // Build in-range data
    //////////////////////

    for (posX = startX; posX <= endX; posX++ )
    {
        for (posY = startY; posY <= endY; posY++ )
        {
            cell = GetCell(posX, posY);
            if (cell)
            {
                UpdateInRangeSet(obj, plObj, cell, &buf);
            }
        }
    }

    if(plObj)
    {
        sLog.outDetail("Creating player "I64FMT" for himself.", obj->GetGUID());
        ByteBuffer pbuf(10000);
        count = plObj->BuildCreateUpdateBlockForPlayer(&pbuf, plObj);
        plObj->PushUpdateData(&pbuf, count);
    }

    //Add to the cell's object list
    //objCell->GetLock()->acquire();
    objCell->AddObject(obj);
    //objCell->GetLock()->release();

    //Add to the mapmanager's object list
    _objects.insert( ObjectMap::value_type( obj->GetGUID(), obj) );
    
    if(plObj)
        _players.insert( plObj );

    //If it's a player - update his nearby cells

    _busy.Release();
    if(obj->GetTypeId() == TYPEID_PLAYER)
    {
        sLog.outDebug("Updatecellactivity in AddObject");
        UpdateCellActivity(x, y, 2);
    }

    obj->SetMapCell(objCell);
}


void MapMgr::RemoveObject(Object *obj)
{
    /////////////
    // Assertions
    /////////////

    ASSERT(obj);
    ASSERT(obj->GetMapId() == _mapId);
    ASSERT(obj->GetPositionX() > _minX && obj->GetPositionX() < _maxX);
    ASSERT(obj->GetPositionY() > _minY && obj->GetPositionY() < _maxY);
    ASSERT(_cells);
    if(!_shutdown)
        _busy.Acquire();

    // remove from active object set if we are in there. (thread checks are auto here)
    switch(UINT32_LOPART(obj->GetGUIDHigh()))
    {

    case HIGHGUID_GAMEOBJECT:
        if(_objectUpdaterThread)
            _objectUpdaterThread->RemoveObject<GameObject>(static_cast<GameObject*>(obj));
        break;

    case HIGHGUID_UNIT:
        if(_objectUpdaterThread)
            _objectUpdaterThread->RemoveObject<Creature>(static_cast<Creature*>(obj));
        break;

    }

    if(!_shutdown)
        _updateS.Acquire();
    _updates.erase(obj);
    obj->ClearUpdateMask();
    if(!_shutdown)
        _updateS.Release();

    // That object types are not map objects. TODO: add AI groups here?
    if(obj->GetTypeId() == TYPEID_ITEM || obj->GetTypeId() == TYPEID_CONTAINER)
    {
        _busy.Release();
        return;
    }

    ///////////////////////////////////////
    // Remove object from all needed places
    ///////////////////////////////////////

    // Remove object from map
    ObjectMap::iterator itr = _objects.find(obj->GetGUID());
    ASSERT(itr != _objects.end());
    _objects.erase(itr);

    ASSERT(obj->GetMapCell());

    ///////////////////////
    // Get cell coordinates
    ///////////////////////

    uint32 x = obj->GetMapCell()->GetX();
    uint32 y = obj->GetMapCell()->GetY();

    // Remove object from cell
    obj->GetMapCell()->GetLock()->Acquire();
    obj->GetMapCell()->RemoveObject(obj);
    obj->GetMapCell()->GetLock()->Release();

    // Unset object's cell
    obj->SetMapCell(NULL);

    // Clear any updates pending
    if(obj->GetTypeId() == TYPEID_PLAYER)
    {
        if(!_shutdown)
            _processL.Acquire();
        _processQueue.erase(((Player*)obj));
        ((Player*)obj)->ClearAllPendingUpdates();
        if(!_shutdown)
            _processL.Release();
    }
    
    // Remove object from all objects 'seeing' him

    for (Object::InRangeSet::iterator iter = obj->GetInRangeSetBegin();
        iter != obj->GetInRangeSetEnd(); ++iter)
    {
        if((*iter)->GetTypeId() == TYPEID_PLAYER)
            if (((Player*)(*iter))->IsVisible(obj) && static_cast<Player*>(*iter)->m_TransporterGUID != obj->GetGUID())
                ((Player*)*iter)->PushOutOfRange(obj->GetNewGUID());

        (*iter)->RemoveInRangeObject(obj);
    }
    
    // Clear object's in-range set
    obj->ClearInRangeSet();

    if(!_shutdown)
        _busy.Release();

    // If it's a player - update his nearby cells
    if(!_shutdown && obj->GetTypeId() == TYPEID_PLAYER)
    {
        UpdateCellActivity(x, y, 2);
        _players.erase(((Player*)obj));
    }
}

void MapMgr::ChangeObjectLocation(Object *obj)
{
    // Items and containers are of no interest for us
    if(obj->GetTypeId() == TYPEID_ITEM || obj->GetTypeId() == TYPEID_CONTAINER)
    {
        return;
    }

    // cling!
    _busy.Acquire();

    ASSERT(obj);
    ASSERT(obj->GetMapId() == _mapId);
    ASSERT(_cells);
    ASSERT(obj->GetMapCell());

    Player *plObj, *plObj2;
    ByteBuffer buf;

    if(obj->GetTypeId() == TYPEID_PLAYER) {
        plObj = ((Player*)obj);
    } else {
        plObj = NULL;
    }

    Object* curObj;
    float fRange;

    ///////////////////////////////////////
    // Update in-range data for old objects
    ///////////////////////////////////////

    for (Object::InRangeSet::iterator iter = obj->GetInRangeSetBegin();
        iter != obj->GetInRangeSetEnd();)
    {
        curObj = *iter;
        iter++;
        if(curObj->IsPlayer() && obj->IsPlayer() && plObj->m_TransporterGUID && plObj->m_TransporterGUID == ((Player*)curObj)->m_TransporterGUID)
            fRange = 0.0f;             // unlimited distance for people on same boat
        else if((UINT32_LOPART(curObj->GetGUIDHigh()) == HIGHGUID_TRANSPORTER || UINT32_LOPART(obj->GetGUIDHigh()) == HIGHGUID_TRANSPORTER))
            fRange = 0.0f;              // unlimited distance for transporters (only up to 2 cells +/- anyway.)
        else
            fRange = m_UpdateDistance;      // normal distance

        if (curObj->GetDistance2dSq(obj) > fRange && fRange > 0)
        {
            if( plObj )
            {
                if (plObj->IsVisible(curObj))
                {
                    plObj->PushOutOfRange(curObj->GetNewGUID());
                    plObj->RemoveVisibleObject(curObj);
                }
            }

            if( curObj->IsPlayer() )
            {
                plObj2 = ((Player*)curObj);
                if (plObj2->IsVisible(obj))
                {
                    plObj2->PushOutOfRange(obj->GetNewGUID());
                    plObj2->RemoveVisibleObject(obj);
                }
            }

            obj->RemoveInRangeObject(curObj);
            curObj->RemoveInRangeObject(obj);
        }
    }
    
    ///////////////////////////
    // Get new cell coordinates
    ///////////////////////////

    uint32 cellX = GetPosX(obj->GetPositionX());
    uint32 cellY = GetPosY(obj->GetPositionY());

    MapCell *objCell = GetCell(cellX, cellY);
    if (!objCell)
    {
        objCell = Create(cellX,cellY);
        objCell->Init(cellX, cellY, _mapId, this);
    }

    // If object moved cell
    if (objCell != obj->GetMapCell())
    {
        // THIS IS A HACK!
        // Current code, if a creature on a long waypoint path moves from an active
        // cell into an inactive one, it will disable itself and will never return.
        // This is to prevent cpu leaks. I will think of a better solution very soon :P

        if(!objCell->IsActive() && !plObj && _objectUpdaterThread
            && UINT32_LOPART(obj->GetGUIDHigh()) == HIGHGUID_UNIT)
        {
            _objectUpdaterThread->RemoveObject<Creature>(static_cast<Creature*>(obj));
        }


        // remove from current cell
        //obj->GetMapCell()->GetLock()->acquire();
        obj->GetMapCell()->RemoveObject(obj);
        //obj->GetMapCell()->GetLock()->release();

        //objCell->GetLock()->acquire();
        objCell->AddObject(obj);
        //objCell->GetLock()->release();

        obj->SetMapCell(objCell);

        // if player we need to update cell activity
        // radius = 2 is used in order to update both
        // old and new cells
        if(obj->GetTypeId() == TYPEID_PLAYER)
        {
            // have to unlock/lock here to avoid a deadlock situation.
            _busy.Release();
            UpdateCellActivity(cellX, cellY, 2);
            _busy.Acquire();
        }
    }


    //////////////////////////////////////
    // Update in-range set for new objects
    //////////////////////////////////////

    uint32 endX = cellX < _sizeX ? cellX + 1 : _sizeX;
    uint32 endY = cellY < _sizeY ? cellY + 1 : _sizeY;
    uint32 startX = cellX > 0 ? cellX - 1 : 0;
    uint32 startY = cellY > 0 ? cellY - 1 : 0;
    uint32 posX, posY;
    MapCell *cell;
    MapCell::ObjectSet::iterator iter;

    for (posX = startX; posX <= endX; ++posX )
    {
        for (posY = startY; posY <= endY; ++posY )
        {
            cell = GetCell(posX, posY);
            if (cell)
               UpdateInRangeSet(obj, plObj, cell, &buf);
        }
    }
    _busy.Release();

    // update stealth ranges etc
    if(plObj) plObj->UpdateVisibility(false);
}

void MapMgr::UpdateInRangeSet(Object *obj, Player *plObj, MapCell* cell, ByteBuffer *buf)
{
    Object *curObj;
    Player *plObj2;
    int count;
    ObjectSet::iterator iter = cell->Begin();
    float fRange;

    while(iter != cell->End())
    {
        curObj = *iter;
        ++iter;
        if(curObj->IsPlayer() && obj->IsPlayer() && plObj->m_TransporterGUID && plObj->m_TransporterGUID == ((Player*)curObj)->m_TransporterGUID)
            fRange = 0.0f;             // unlimited distance for people on same boat
        else if((UINT32_LOPART(curObj->GetGUIDHigh()) == HIGHGUID_TRANSPORTER || UINT32_LOPART(obj->GetGUIDHigh()) == HIGHGUID_TRANSPORTER))
            fRange = 0.0f;              // unlimited distance for transporters (only up to 2 cells +/- anyway.)
        else
            fRange = m_UpdateDistance;      // normal distance

        if (curObj != obj &&
            ((curObj)->GetDistance2dSq(obj) <= fRange || fRange == 0.0f) &&
            !obj->IsInRangeSet(curObj))
        {
            // Object in range, add to set
            curObj->AddInRangeObject(obj);
            obj->AddInRangeObject(curObj);

            if(curObj->IsPlayer())
            {
                plObj2 = ((Player*)curObj);

                if (plObj2->CanSee(obj) && !plObj2->IsVisible(obj))
                {
                    count = obj->BuildCreateUpdateBlockForPlayer(buf, plObj2);
                    plObj2->PushUpdateData(buf, count);
                    plObj2->AddVisibleObject(obj);
                    buf->clear();
                }
            }

            if(plObj)
            {
                if (plObj->CanSee(curObj) && !plObj->IsVisible(curObj))
                {
                    count = curObj->BuildCreateUpdateBlockForPlayer(buf, plObj);
                    plObj->PushUpdateData(buf, count);
                    plObj->AddVisibleObject(curObj);
                    buf->clear();
                }
            }
        }
    }
}

void MapMgr::SoftReset()
{
    if(HasPlayers())
    {
        reset_pending = false;
        return;
    }

    sEventMgr.AddEvent(World::getSingletonPtr(), &World::EventDeleteInstance, GetMapId(),
        m_instanceID, EVENT_INSTANCE_SOFT_RESET, 5000, 1);
}

void MapMgr::_UpdateObjects()
{
    if(this->pMapInfo && pMapInfo->type == INSTANCE_NONRAID)
    {
        if(HasPlayers() && reset_pending)
        {
            sEventMgr.RemoveEvents(this, EVENT_INSTANCE_SOFT_RESET);
            reset_pending = false;
        }
        else if(!HasPlayers() && !reset_pending)
        {
            sEventMgr.AddEvent(this, &MapMgr::SoftReset, EVENT_INSTANCE_SOFT_RESET, 1800000, 1);
            reset_pending = true;
        }
    }

    if(!_updates.size() && !_processQueue.size())
        return;

    Object *pObj;
    Player *pOwner;
    //std::set<Object*>::iterator it_start, it_end, itr;
    std::set<Player*>::iterator it_start, it_end, itr;
    Player * lplr;
    ByteBuffer update(2500);
    uint32 count = 0;
    
    _updateS.Acquire();
    UpdateQueue::iterator iter = _updates.begin();
    PUpdateQueue::iterator it, eit;

    for(; iter != _updates.end(); ++iter)
    {
        pObj = *iter;
        if(pObj->GetTypeId() == TYPEID_ITEM || pObj->GetTypeId() == TYPEID_CONTAINER)
        {
            // our update is only sent to the owner here.
            pOwner = static_cast<Item*>(pObj)->GetOwner();
            if(pOwner != NULL)
            {
                count = static_cast<Item*>(pObj)->BuildValuesUpdateBlockForPlayer(&update, pOwner);
                // send update to owner
                pOwner->PushUpdateData(&update, count);
                update.clear();
            }
        } else {
            if(pObj->IsInWorld())
            {
                // players have to receive their own updates ;)
                if(pObj->GetTypeId() == TYPEID_PLAYER)
                {
                    // need to be different! ;)
                    count = pObj->BuildValuesUpdateBlockForPlayer(&update, static_cast<Player*>(pObj));
                    ((Player*)pObj)->PushUpdateData(&update, count);
                    update.clear();
                }

                // build the update
                count = pObj->BuildValuesUpdateBlockForPlayer(&update, NULL);

                /*// lock the _busy mutex, so the inrange sets arent modified while we're looping it
                _busy.Acquire();*/

                it_start = pObj->GetInRangePlayerSetBegin();
                it_end = pObj->GetInRangePlayerSetEnd();
                for(itr = it_start; itr != it_end;)
                {
                    lplr = *itr;
                    ++itr;
                    // Make sure that the target player can see us.
                    if(lplr->GetTypeId() == TYPEID_PLAYER && lplr->IsVisible(pObj))
                        lplr->PushUpdateData(&update, count);
                }

                /*_busy.Release();*/

                update.clear();
            }
        }
        pObj->ClearUpdateMask();
    }
    _updates.clear();
    _updateS.Release();
    
    _processL.Acquire();
    // generate pending a9packets and send to clients.
    Player *plyr;
    for(it = _processQueue.begin(); it != _processQueue.end();)
    {
        plyr = *it;
        eit = it;
        ++it;
        _processQueue.erase(eit);
        if(plyr->GetMapMgr() == this)
            plyr->ProcessPendingUpdates();
    }
    _processL.Release();
}

void MapMgr::UpdateCellActivity(uint32 x, uint32 y, int radius, bool lock /* = true */)
{
    if(lock)
        _activityS.Acquire();

    uint32 endX = x + radius < _sizeX ? x + radius : _sizeX;
    uint32 endY = y + radius < _sizeY ? y + radius : _sizeY;
    uint32 startX = x - radius > 0 ? x - radius : 0;
    uint32 startY = y - radius > 0 ? y - radius : 0;
    uint32 posX, posY;

    MapCell *objCell;

    for (posX = startX; posX <= endX; posX++ )
    {
        for (posY = startY; posY <= endY; posY++ )
        {
            objCell = GetCell(posX, posY);

            if (!objCell)
            {
                if (_CellActive(posX, posY))
                {
                    objCell = Create(posX, posY);
                    objCell->Init(posX, posY, _mapId, this);

                    sLog.outDetail("Cell [%d,%d] on map %d (instance %d) is now active.", 
                        posX, posY, this->_mapId, m_instanceID);
                    objCell->SetActivity(true);
                    ASSERT(!objCell->IsLoaded());

                    sLog.outDetail("Loading objects for Cell [%d][%d] on map %d (instance %d)...", 
                        posX, posY, this->_mapId, m_instanceID);
                    objCell->LoadObjects();
                }
            }
            else
            {
                //Cell is now active
                if (_CellActive(posX, posY) && !objCell->IsActive())
                {
                    sLog.outDetail("Cell [%d,%d] on map %d (instance %d) is now active.", 
                        posX, posY, this->_mapId, m_instanceID);
                    objCell->SetActivity(true);

                    if (!objCell->IsLoaded())
                    {
                        sLog.outDetail("Loading objects for Cell [%d][%d] on map %d (instance %d)...", 
                            posX, posY, this->_mapId, m_instanceID);
                        objCell->LoadObjects();
                    }
                }
                //Cell is no longer active
                else if (!_CellActive(posX, posY) && objCell->IsActive())
                {
                    sLog.outDetail("Cell [%d,%d] on map %d (instance %d) is now idle.", 
                        posX, posY, this->_mapId, m_instanceID);
                    objCell->SetActivity(false);
                }
            }
        }
    }

    if(lock)
        _activityS.Release();
}

bool MapMgr::_CellActive(uint32 x, uint32 y)
{
    uint32 endX = x < _sizeX ? x + 1 : _sizeX;
    uint32 endY = y < _sizeY ? y + 1 : _sizeY;
    uint32 startX = x > 0 ? x - 1 : 0;
    uint32 startY = y > 0 ? y - 1 : 0;
    uint32 posX, posY;

    MapCell *objCell;

    for (posX = startX; posX <= endX; posX++ )
    {
        for (posY = startY; posY <= endY; posY++ )
        {
            objCell = GetCell(posX, posY);

            if (objCell)
            {
                if (objCell->HasPlayers())
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void MapMgr::ObjectUpdated(Object *obj)
{
    // set our fields to dirty
    _updateS.Acquire();
    _updates.insert(obj);
    _updateS.Release();
}

void MapMgr::PushToProcessed(Player* plr)
{
    _processL.Acquire();
    _processQueue.insert(plr);
    _processL.Release();
}

void MapMgr::AddSession(WorldSession *session)
{
    Sessions.insert(session);
}

void MapMgr::RemoveSession(WorldSession *session)
{
    Sessions.erase(session);
}

void MapMgr::Respawn(bool RemoveOld, bool SpawnNew)
{
    if(RemoveOld)
    {
        // Loop all existing spawns, remove all objects.
        ObjectMap::iterator iter = _objects.begin();
        Object * obj;
        for(; iter != _objects.end();)
        {
            obj = iter->second;
            ++iter;
            if(obj->IsSpawnedObject())
            {
                obj->RemoveFromWorld();
                switch(obj->GetGUIDHigh())
                {
                case HIGHGUID_UNIT:
                    sObjHolder.Delete<Creature>(static_cast<Creature*>(obj));
                    break;
                case HIGHGUID_GAMEOBJECT:
                    sObjHolder.Delete<GameObject>(static_cast<GameObject*>(obj));
                    break;
                case HIGHGUID_CORPSE:
                    break;
                default:
                    ASSERT(false);
                    break;
                }
            }
        }

        // Loop cells, set them as unloaded, so that objects will be loaded again upon movement.
        MapCell *pCell;
        for (uint32 posX = 0; posX < _sizeX; posX++ )
        {
            for (uint32 posY = 0; posY < _sizeY; posY++ )
            {
                pCell = _cells[posX][posY];
                if(pCell != 0)
                {
                    pCell->_loaded = false;
                    pCell->_loadedObjects = false;

                    // respawn active cells
                    if(pCell->IsActive() && !pCell->IsLoaded())
                        pCell->LoadObjects();
                }
            }
        }
    }

    /*if(SpawnNew)
    {
        // Loop players, update cell activity for each one. This will load any creatures again.
        _activityS.Acquire();
        MapCell *pCell;
        for (uint32 posX = 0; posX < _sizeX; posX++ )
        {
            for (uint32 posY = 0; posY < _sizeY; posY++ )
            {
                pCell = _cells[posX][posY];
                if(pCell != 0 && pCell->HasPlayers() && !pCell->IsLoaded())
                {
                    UpdateCellActivity(pCell->GetX(), pCell->GetY(), 2, false);
                }
            }
        }
        _activityS.Release();
    }*/    
}

void MapMgr::RegisterScriptUpdater(uint32 Frequency)
{
    sEventMgr.AddEvent(this, &MapMgr::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, Frequency, 0);
}

void MapMgr::CallScriptUpdate()
{
    myScript->UpdateEvent();
}

void MapMgr::SpawnEvents()
{
    // executed once server is fully up.
    sScriptMgr.ExecuteEventScriptsOnMap(GetMapId(), this);
}

void MapMgr::ChangeFarsightLocation(Player *plr, Creature *farsight)
{
    if(farsight == 0)
    {
        // We're clearing.
        for(ObjectSet::iterator itr = plr->m_visibleFarsightObjects.begin(); itr != plr->m_visibleFarsightObjects.end();
            ++itr)
        {
            if(!plr->IsVisible((*itr)))
            {
                // Send destroy
                plr->PushOutOfRange((*itr)->GetNewGUID());
            }
        }
        plr->m_visibleFarsightObjects.clear();
    }
    else
    {
        _busy.Acquire();
        uint32 cellX = GetPosX(farsight->GetPositionX());
        uint32 cellY = GetPosY(farsight->GetPositionY());
        uint32 endX = cellX < _sizeX ? cellX + 1 : _sizeX;
        uint32 endY = cellY < _sizeY ? cellY + 1 : _sizeY;
        uint32 startX = cellX > 0 ? cellX - 1 : 0;
        uint32 startY = cellY > 0 ? cellY - 1 : 0;
        uint32 posX, posY;
        MapCell *cell;
        Object *obj;
        MapCell::ObjectSet::iterator iter, iend;
        uint32 count;
        for (posX = startX; posX <= endX; ++posX )
        {
            for (posY = startY; posY <= endY; ++posY )
            {
                cell = GetCell(posX, posY);
                if (cell)
                {
                    iter = cell->Begin();
                    iend = cell->End();
                    for(; iter != iend; ++iter)
                    {
                        obj = (*iter);
                        if(!plr->IsVisible(obj) && farsight->GetDistance2dSq(obj) <= m_UpdateDistance)
                        {
                            ByteBuffer buf;
                            count = obj->BuildCreateUpdateBlockForPlayer(&buf, plr);
                            plr->PushUpdateData(&buf, count);
                            plr->m_visibleFarsightObjects.insert(obj);
                        }
                    }
                    
                }
            }
        }
        _busy.Release();
    }
}

void MapMgr::LoadAllCells()
{
    MapCell * pCell;
    char msg[50];
    sprintf(msg, "Preload: Map%u", _mapId);
    uint32 count = _sizeX * _sizeY;
    uint32 c = 0;

    for(uint32 x = 0; x < _sizeX; ++x)
    {
        for(uint32 y = 0; y < _sizeY; ++y)
        {
            pCell = _cells[x][y];
            if(pCell == 0)
            {
                pCell = Create(x, y);
                pCell->Init(x, y, _mapId, this);
                pCell->LoadObjects();
            }
            else if(pCell->IsLoaded() == false)
            {
                pCell->LoadObjects();
            }
            ++c;
            if(!(c % 50))
                SetProgressBar(c, count, msg);
        }
    }
    ClearProgressBar();
}
