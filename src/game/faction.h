// Copyright (C) 2004 WoW Daemon
// Copyright (C) 2005 Oxide
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

#ifndef __FACTION_H
#define __FACTION_H

#include "Unit.h"

enum factions
{
    FACT_NULL     = 0x0,
    FACT_PLAYER   = 0x1,
    FACT_ALLIANCE = 0x2,
    FACT_HORDE    = 0x4,
    FACT_BEAST    = 0x8
};


inline bool isHostile(Object* objA, Object* objB)// B is hostile for A?
{
    if(!objA || !objB)
        return false;
    bool hostile = false;

    if(objB->m_faction == NULL || objA->m_faction == NULL)
        return true;

    if(objA == objB)
        return false;   // can't attack self.. this causes problems with buffs if we dont have it :p

    if(objA->GetTypeId() == TYPEID_CORPSE)
        return false;

    if(objB->GetTypeId() == TYPEID_CORPSE)
        return false;

    uint32 faction = objB->m_faction->myFaction;
    uint32 host = objA->m_faction->hostile;

    if(faction & host)
    {
        hostile = true;
    }
    // check friend/enemy list
    for(uint32 i = 0; i < 4; i++)
    {
        if(objA->m_faction->hostileR[i] == objB->m_faction->FactionId)
        {
            hostile = true;
            break;
        }
        if(objA->m_faction->friendlyR[i] == objB->m_faction->FactionId)
        {
            hostile = false;
            break;
        }
    }

    // PvP Flag System Checks
    // We check this after the normal isHostile test, that way if we're
    // on the opposite team we'll already know :p

    if(hostile && 
        objA->IsPlayer() && objB->IsPlayer())
    {
        // Check PvP Flags.
        if(static_cast<Player*>(objB)->IsPvPFlagged())
            return true;
        else
            return false;
    }

    return hostile;
}

inline bool isAttackable(Object* objA, Object* objB)// A can attack B?
{
    if(!objA || !objB || objB->m_factionDBC == NULL || objA->m_factionDBC == NULL)
        return false;

    if(objB->m_faction == NULL || objA->m_faction == NULL )
        return true;

    if(objA == objB)
        return false;   // can't attack self.. this causes problems with buffs if we dont have it :p

    if(objA->GetTypeId() == TYPEID_CORPSE)
        return false;

    if(objB->GetTypeId() == TYPEID_CORPSE)
        return false;

    if(objA->IsPlayer() && objB->IsPlayer())
    {
        if(
            static_cast<Player *>(objA)->DuelingWith == static_cast<Player *>(objB) && 
            static_cast<Player *>(objA)->GetDuelState() == DUEL_STATE_STARTED
            )
        return true;                    

        if(objA->HasFlag(PLAYER_FLAGS,PLAYER_FLAG_FREE_FOR_ALL_PVP) && objB->HasFlag(PLAYER_FLAGS,PLAYER_FLAG_FREE_FOR_ALL_PVP))
            return true;        // can hurt each other in FFA pvp
    }

    if(objA->m_faction == objB->m_faction)  // same faction can't kill each other unless in ffa pvp/duel
        return false;

    bool attackable = isHostile(objA, objB); // B is attackable if its hostile for A
    if((objA->m_faction->hostile & 8) && (objB->m_factionDBC->FactionGroup == 0) && 
        (objB->GetTypeId() != TYPEID_PLAYER) && objB->m_faction->FactionId != 31) // B is attackable if its a neutral Creature
    {
        attackable = true;
    }

    return attackable;
}

inline bool isFriendly(Object* objA, Object* objB)// B is friendly to A if its not hostile
{
    return !isHostile(objA, objB);
}

inline bool isCombatSupport(Object* objA, Object* objB)// B combat supports A?
{
    if(!objA || !objB)
        return false;

    if(objA->GetTypeId() == TYPEID_CORPSE)
        return false;

    if(objB->GetTypeId() == TYPEID_CORPSE)
        return false;

    bool combatSupport = false;

    uint32 fSupport = objB->m_faction->fightSupport;

    uint32 myFaction = objA->m_faction->myFaction;

    if(myFaction & fSupport)
    {
        combatSupport = true;
    }
    // check friend/enemy list
    for(uint32 i = 0; i < 4; i++)
    {
        if(objB->m_faction->hostileR[i] == objA->m_faction->FactionId)
        {
            combatSupport = false;
            break;
        }
        if(objB->m_faction->friendlyR[i] == objA->m_faction->FactionId)
        {
            combatSupport = true;
            break;
        }
    }
    return combatSupport;
}
#endif
