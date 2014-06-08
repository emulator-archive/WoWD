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

enum LFGTypes 
{
    LFG_NONE = 0,
    LFG_INSTANCE = 1,
    LFG_RAID = 2,
    LFG_QUEST = 3,
    LFG_ZONE = 4,
};

#define MAX_DUNGEONS 166 // check max entrys +1 on lfgdungeons.dbc

class LfgMgr : public Singleton < LfgMgr >
{
public:    
    
    LfgMgr();
    ~LfgMgr();
    
    void SetPlayerInLFGqueue(Player *pl,uint32 LfgDungeonId);
    void RemoveFromLfgQueue(Player *pl,uint32 LfgDungeonId);
    uint32 GetLfgQueueSize(uint32 LfgDungeonId);
    std::set<Player*>::iterator GetLfgQueueBegin(uint32 LfgDungeonId);
    std::set<Player*>::iterator GetLfgQueueEnd(uint32 LfgDungeonId);
    
protected:
    
    std::set<Player*> m_LFGqueueMembers[MAX_DUNGEONS];
    
    
};

#define sLfgMgr LfgMgr::getSingleton()
