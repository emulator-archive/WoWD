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

Arena::Arena()
{
    bSpawned = false;
    bStarted = false;
    m_BGTime = 0;
    m_HordeGroup = m_AllianceGroup = NULL;
    m_MaxScore = 3;
}

Arena::~Arena()
{
    // Nothing
}

void Arena::Remove()
{
}

void Arena::HandleBattlegroundAreaTrigger(Player *plr, uint32 TriggerID)
{ 
    switch(TriggerID)
    {
             
        /*case 3948:
        case 3949:
            {
                RemovePlayer(plr, true, true);
                return;
            }break;*/
        default:
            {
                sLog.outString("WARNING: Unhandled AreaTrigger in Battleground: %d", TriggerID);
            }break;
    }
}

void Arena::HandleBattlegroundEvent(Object *src, Object *dst, uint16 EventID, uint32 params1, uint32 params2)
{
    WorldPacket data;

    switch(EventID)
    {
    case BGEVENT_AB_PLAYER_DEATH:
        {
            Player *Source = ((Player*)src);
            if(!Source || src->GetTypeId() != TYPEID_PLAYER) return;

            uint64 guid = Source->GetGUID();
            std::map<uint64, BattlegroundScore>::iterator itr = m_PlayerScores.find(guid);
            if(itr == m_PlayerScores.end())
            {
                SendMessageToPlayer(Source, "INTERNAL ERROR: Could not find in internal player score map!", true);
                return;
            }
            itr->second.Deaths++;
            UpdatePVPData();
        }break;
    case BGEVENT_AB_PLAYER_KILL:
        {
            Player *Source = ((Player*)src);
            if(!Source || src->GetTypeId() != TYPEID_PLAYER) return;

            uint64 guid = Source->GetGUID();
            std::map<uint64, BattlegroundScore>::iterator itr = m_PlayerScores.find(guid);
            if(itr == m_PlayerScores.end())
            {
                SendMessageToPlayer(Source, "INTERNAL ERROR: Could not find in internal player score map!", true);
                return;
            }
            itr->second.KillingBlows++;
            UpdatePVPData();
        }break; 
   }    
}

void Arena::SetupBattleground()
{    
    // default world state's
    WorldStateVars[0x8D8] = uint32(0x00);
    WorldStateVars[0x8D7] = uint32(0x00);
    WorldStateVars[0x8D6] = uint32(0x00);
    WorldStateVars[0x8D5] = uint32(0x00);
    WorldStateVars[0x8D4] = uint32(0x00);
    WorldStateVars[0x8D3] = uint32(0x00);

    
    WorldStateVars[0x9F3] = uint32(0x01);
    WorldStateVars[0x9F1] = uint32(0x01);
    WorldStateVars[0x9F0] = uint32(0x01);
}

void Arena::SpawnBattleground()
{
       
    // Alliance Gate
    GameObject *gate = SpawnGameObject(180255, 529, 1284.597290, 1281.166626, -15.977916, 0.706859, 32, 114, 1.5799990);
    gate->SetFloatValue(GAMEOBJECT_ROTATION,0.0129570);
    gate->SetFloatValue(GAMEOBJECT_ROTATION_01,-0.0602880);
    gate->SetFloatValue(GAMEOBJECT_ROTATION_02,0.3449600);
    gate->SetFloatValue(GAMEOBJECT_ROTATION_03,0.9365900);
    m_Gates.insert(gate);
    
    // Horde Gate
    gate = SpawnGameObject(180256, 529, 708.0902710, 708.4479370, -17.3898964, -2.3910990, 32, 114, 1.5699990);
    gate->SetFloatValue(GAMEOBJECT_ROTATION,0.0502910);
    gate->SetFloatValue(GAMEOBJECT_ROTATION_01,0.0151270);
    gate->SetFloatValue(GAMEOBJECT_ROTATION_02,0.9292169);
    gate->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.3657840);
    m_Gates.insert(gate);

    bSpawned = true;
}

void Arena::Start()
{
    // Open all the doors

    for(std::set<GameObject*>::iterator itr = m_Gates.begin(); itr!=m_Gates.end(); ++itr)
    {
        (*itr)->SetUInt32Value(GAMEOBJECT_FLAGS, 33);
        (*itr)->SetUInt32Value(GAMEOBJECT_STATE, 0);
    }

    // Send message packet
    WorldPacket *data = BuildMessageChat(0x52, 0x27, "The Arena Battle has begun!", 0);
    SendPacketToAll(data);
    delete data;
}
