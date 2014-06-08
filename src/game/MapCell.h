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
// MapCell.h
//

#ifndef __MAP_CELL_H
#define __MAP_CELL_H

class Map;
class MapUpdaterThread;

class MapCell
{
    friend class MapUpdaterThread;
    friend class MapMgr;
public:
    MapCell() {};
    ~MapCell();

    typedef std::set<Object*> ObjectSet;

    //Init
    void Init(uint32 x, uint32 y, uint32 mapid, MapMgr *mapmgr);

    //Object Managing
    void AddObject(Object *obj); 
    void RemoveObject(Object *obj);
    bool HasObject(Object *obj) { return !(_objects.find(obj) == _objects.end()); }
    bool HasPlayers() { return ((_playerCount > 0) ? true : false); }
    uint32 GetObjectCount() { return _objects.size(); }
    void RemoveObjects();
    inline ObjectSet::iterator Begin() { return _objects.begin(); }
    inline ObjectSet::iterator End() { return _objects.end(); }

    inline bool HasRemovedEvents() { return _hasRemovedEvents; }

    //State Related
    void SetActivity(bool state);

    inline bool IsActive() { return _active; }
    inline bool IsLoaded() { return _loaded; }
    inline time_t GetWentIdleTime() { return _timeWentIdle; }

    //Position Related

    inline uint32 GetX() { return _x; }
    inline uint32 GetY() { return _y; }

    //Object Loading Managing
    void LoadObjects();
    inline Mutex *GetLock() { return &_lock; }
    inline uint32 GetPlayerCount() { return _playerCount; }

private:
    ObjectSet _objects;
    bool _active, _loaded;
    bool _loadedObjects;
    uint32 _playerCount;
    uint32 _x, _y, _mapid;
    time_t _timeWentIdle;
    MapMgr* _mapmgr;
    bool _hasRemovedEvents;
    Mutex _lock;
};

#endif
