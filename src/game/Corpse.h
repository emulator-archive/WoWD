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

#ifndef WOWSERVER_CORPSE_H
#define WOWSERVER_CORPSE_H

enum CORPSE_STATE
{
    CORPSE_STATE_BODY = 0,
    CORPSE_STATE_BONES = 1,
};
struct CorpseData
{
    uint32 LowGuid;
    uint32 mapid;
    uint64 owner;
    uint32 instancemapid;
    float x;
    float y;
    float z;
    void DeleteFromDB();
};

class WOWD_SERVER_DECL Corpse : public Object
{
public:
    Corpse( uint32 high, uint32 low );
    ~Corpse();

    void Create();
    void Create (Player *owner, uint32 mapid, float x, float y, float z, float ang );

    void SaveToDB();
    void DeleteFromDB();
    inline void SetCorpseState(uint32 state) { m_state = state; }
    inline uint32 GetCorpseState() { return m_state; }
    void Despawn();
    
    inline void SetLoadedFromDB(bool value) { _loadedfromdb = value; }
    inline bool GetLoadedFromDB(void) { return _loadedfromdb; }
    Loot loot;
    void generateLoot();

private:
    uint32 m_state;
    bool _loadedfromdb;
};

#endif

