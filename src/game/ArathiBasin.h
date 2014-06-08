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

class ArathiBasin : public Battleground
{
public:
    ArathiBasin();
    ~ArathiBasin();

    bool almostend[2];

    /* Scorekeeping */
    uint32 AllianceRes,HordeRes;

    uint32 m_BGTime;
    
    GameObject *gcbanner[5];

    GameObject *gbuffs[5];

    std::set<GameObject*> m_Gates;

    void HandleBanner(Player *p_caster,GameObject *go,uint32 spellid);
    void HandleBattlegroundAreaTrigger(Player *plr, uint32 TriggerID);
    void HandleBattlegroundEvent(Object *src, Object *dst, uint16 EventID, uint32 params1 = 0, uint32 params2 = 0);
    void SetupBattleground();
    void SpawnBattleground();
    void SpawnBuff(uint32 typeentry,uint32 bannerslot);
    void EventResourcesCapture(Player *src,uint32 bannerslot);
    void setBaseCapturedValue(uint32 bannerslot,uint32 team,uint32 value);
    void setBaseCapturingValue(uint32 bannerslot,uint32 team,uint32 value);
    void EventUpdateResourcesAlliance();
    void EventUpdateResourcesHorde();
    
    void Remove();

    void Start();
    void EventUpdate(uint32 diff);
};
