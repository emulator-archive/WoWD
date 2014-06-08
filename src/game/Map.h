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
// Map.h
//

#ifndef __MAP_H
#define __MAP_H

class MapMgr;
class TemplateMgr;
struct MapInfo;
typedef std::map<uint32, MapMgr*> InstanceMap;
class TerrainMgr;

class Map
{
public:
    Map(uint32 mapid);
    ~Map();

    inline TemplateMgr* GetTemplate() { return _template; }
    inline InstanceMap::iterator GetInstancesBegin() { return _instances.begin(); }
    inline InstanceMap::iterator GetInstancesEnd() { return _instances.end(); }

    MapMgr * CreateMapMgrInstance(uint32 instanceid = 0);
    void DestoryMapMgrInstance(uint32 instanceId);
    MapMgr * GetFirstInstance();    // for main continents

    MapMgr * GetInstance(Object* obj);
    MapMgr * GetInstance(uint32 instanceId);
    MapMgr * InstanceExists(uint32 instanceId);
    inline TerrainMgr * GetTerrainMgr() { return _terrain; }
    inline string GetNameString() { return name; }
    inline const char* GetName() { return name.c_str(); }
    inline MapEntry* GetDBCEntry() { return me; }

private:
    TemplateMgr*    _template;
    InstanceMap     _instances;    
    MapInfo *       _mapInfo;
    TerrainMgr*     _terrain;
    uint32 _mapId;
    string name;
    MapEntry * me;
};

#endif
