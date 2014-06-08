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

#include "StdAfx.h"

initialiseSingleton(TemplateMgrHolder);

template<> std::map<uint32, ObjectTemplate*>& TemplateCell::_GetMap<Creature>()
    {return _creatures;}
template<> std::map<uint32, ObjectTemplate*>& TemplateCell::_GetMap<GameObject>()
    {return _gameobjects;}

TemplateMgr::TemplateMgr(Map *map, uint32 mapId) : _mapId(mapId), CellHandler<TemplateCell>(map)
{

}

TemplateMgrHolder::TemplateMgrHolder()
{
    new CreatureDataCache;
    _cached.reserve(150000);

    DBCFile d;
    d.open("DBC/Map.dbc");
    for(uint32 x = 0; x < d.getRecordCount(); ++x)
    {
        GetTemplateMgr(d.getRecord(x).getUInt(0));
    }

    CacheCreatures();
    CacheGameObjects();
}

TemplateMgrHolder::~TemplateMgrHolder()
{
    for(std::map<uint32, TemplateMgr*>::iterator itr = _templates.begin();
        itr != _templates.end(); ++itr)
    {
        delete itr->second;
    }
    _templates.clear();

    for(vector<ObjectTemplate*>::iterator itr = _cached.begin(); itr != _cached.end(); ++itr)
        delete (*itr);
    _cached.clear();
}

TemplateMgr* TemplateMgrHolder::GetTemplateMgr(uint32 mapid)
{
    std::map<uint32, TemplateMgr*>::iterator it = _templates.find(mapid);
    if(it == _templates.end())
    {
        TemplateMgr *tempMgr = new TemplateMgr(NULL, mapid);
        ASSERT(tempMgr);
        _templates.insert( std::pair<uint32, TemplateMgr*>(mapid, tempMgr) );
        return tempMgr;
    } else {
        return it->second;
    }
}

void TemplateMgrHolder::CacheCreatures()
{
    uint32 start_time = getMSTime();
#ifdef PRECACHE_LOAD
    QueryResult * result = sDatabase.Query("SELECT * FROM creatures ORDER BY mapId");
#else
    QueryResult * result = sDatabase.Query("SELECT id, positionX, positionY FROM creatures ORDER BY mapId");
#endif

    uint32 rowcount = result ? result->GetRowCount() : 0;
    uint32 counter  = 0;
    uint32 interval = rowcount / 100;
    if(interval == 0) interval = 1;

    if(result)
    {
        CreatureTemplate * t = NULL;
        Field * fields = result->Fetch();
        TemplateMgr * mgr = 0;
        int32 mapid = 0, lastmap = 0;
        uint32 map_count = 0;
        do 
        {
#ifdef PRECACHE_LOAD
            t = new CreatureTemplate();
            t->Load(fields);
#endif

            // Add to template manager
            mapid = fields[6].GetUInt32();
            if(mapid != lastmap || !mgr)
            {
                mgr = GetTemplateMgr(mapid);
                lastmap = mapid;
                map_count++;
            }
#ifdef PRECACHE_LOAD
            mgr->AddIndex<Creature>(fields[1].GetFloat(),
                fields[2].GetFloat(), fields[0].GetUInt32(), t);
#endif
            ++counter;
            if(!(counter % interval))
                SetProgressBar(counter, rowcount, "creatures");
            _cached.push_back(t);

        } while(result->NextRow());
        ClearProgressBar();
        sLog.outString("  Cached data for %u creatures on %u maps in %ums", counter, map_count, 
            getMSTime() - start_time);

        delete result;
    }
}

void TemplateMgrHolder::CacheGameObjects()
{
    uint32 start_time = getMSTime();
#ifdef PRECACHE_LOAD
    QueryResult * result = sDatabase.Query("SELECT * FROM gameobjects ORDER BY mapId");
#else
    QueryResult * result = sDatabase.Query("SELECT id, positionX, positionY FROM gameobjects ORDER BY mapId");
#endif

    uint32 rowcount = result ? result->GetRowCount() : 0;
    uint32 counter  = 0;
    uint32 interval = rowcount / 100;
    if(interval == 0) interval = 1;

    if(result)
    {
        GameObjectTemplate * t = NULL;
        Field * fields = result->Fetch();
        TemplateMgr * mgr = 0;
        int32 mapid = 0, lastmap = 0;
        uint32 map_count = 0;
        do 
        {
#ifdef PRECACHE_LOAD
            t = new GameObjectTemplate();
            t->Load(fields);
#endif

            // Add to template manager
            mapid = fields[6].GetUInt32();
            if(mapid != lastmap || !mgr)
            {
                mgr = GetTemplateMgr(mapid);
                lastmap = mapid;
                map_count++;
            }

#ifdef PRECACHE_LOAD
            mgr->AddIndex<GameObject>(fields[1].GetFloat(),
                fields[2].GetFloat(), fields[0].GetUInt32(), t);
#endif
            ++counter;
            if(!(counter % interval))
                SetProgressBar(counter, rowcount, "gameobjects");
            _cached.push_back(t);

        } while(result->NextRow());
        ClearProgressBar();
        sLog.outString("  Cached data for %u gameobjects on %u maps in %ums", counter, map_count, 
            getMSTime() - start_time);

        delete result;
    }
}
