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
// TemplateCell.h
//

#ifndef __TEMPLATECELL_H
#define __TEMPLATECELL_H

class Creature;
class GameObject;
class ObjectTemplate;

class TemplateCell
{
public:
    TemplateCell() { };
    ~TemplateCell() { };

    //Init
    void Init(uint32 x, uint32 y, uint32 mapid)
    {
        _x = x;
        _y = y;
        _mapid = mapid;
    }

    //Position Related
    uint32 GetX() { return _x; }
    uint32 GetY() { return _y; }
    uint32 GetMapId() { return _mapid; }

    //Object Loading Managing
    template <class T> void AddIndex(uint32 guid, ObjectTemplate* t) 
    { 
        _GetMap<T>().insert(std::map<uint32, ObjectTemplate*>::value_type(guid, t)); 
    }
    template <class T> void RemoveIndex(uint32 guid) 
    { 
        std::map<uint32, ObjectTemplate*>::iterator itr;
        itr = _GetMap<T>().find(guid);

        if (itr == _GetMap<T>().end())
            return;

        delete itr->second;
        _GetMap<T>().erase(itr); 
    }
    bool HasIndexes() { return ( (_creatures.size()) || (_gameobjects.size()) ); }

    //Iterators
    template <class T> typename std::map<uint32, ObjectTemplate*>::iterator GetMapBegin()
    { 
        return _GetMap<T>().begin();
    }
    template <class T> typename std::map<uint32, ObjectTemplate*>::iterator GetMapEnd()
    {
        return _GetMap<T>().end();
    }

private:
    template <class T> std::map<uint32, ObjectTemplate*>& _GetMap();

    uint32 _x, _y, _mapid;

    std::map<uint32, ObjectTemplate*> _creatures;
    std::map<uint32, ObjectTemplate*> _gameobjects;
};

#endif
