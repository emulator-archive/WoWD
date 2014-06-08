// Copyright (C) 2004 WoW Daemon
// Copyright (C) 2006 Burlex
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

class WarsongGulch : public Battleground
{
public:
    WarsongGulch();
    ~WarsongGulch();

    /* Scorekeeping */
    uint64 FlagGuids[2];
    uint64 FlagHolders[2];
    bool FlagInbase[2];
    GameObject *pFlags[2];
    uint32 m_BGTime;
    uint32 m_MaxScore;

    GameObject *gbuffs[5];

    std::set<GameObject*> m_Gates;

    void HandleBattlegroundAreaTrigger(Player *plr, uint32 TriggerID);
    void HandleBattlegroundEvent(Object *src, Object *dst, uint16 EventID, uint32 params1 = 0, uint32 params2 = 0);
    void SetupBattleground();
    void SpawnBattleground();
    void SpawnBuff(uint32 typeentry,uint32 bannerslot);

    void Remove();

    void Start();
    void EventUpdate(uint32 diff);

    inline void SetBattlegroundMaxScore(uint32 value){m_MaxScore = value;}
    inline uint32 GetBattlegroundMaxScore(){return m_MaxScore;}

    void SetAllianceScore(uint32 score);
    void SetHordeScore(uint32 score);
    uint32 GetAllianceScore();
    uint32 GetHordeScore();

    void SpawnSpiritGuides();
};
