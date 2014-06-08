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

#include "StdAfx.h"

#define BUFF_RESPAWN_TIME 90000

WarsongGulch::WarsongGulch()
{
    FlagHolders[0] = 0;
    FlagHolders[1] = 0;
    FlagGuids[0] = 0;
    FlagGuids[1] = 0;
    pFlags[0] = pFlags[1] = NULL;
    bSpawned = false;
    bStarted = false;
    m_BGTime = 0;
    m_HordeGroup = m_AllianceGroup = NULL;
    m_MaxScore = 3;
}

WarsongGulch::~WarsongGulch()
{
    // Nothing
}

void WarsongGulch::Remove()
{
     // remove some objects

    // There's no need to remove the objects. mapmgr will take care of that when its deleted. :)

    /*for(std::set<GameObject*>::iterator itr = m_Gates.begin(); itr!=m_Gates.end(); ++itr)
    {
        if( (*itr)->IsInWorld() )
            (*itr)->RemoveFromWorld();
        sObjHolder.Delete<GameObject>(*itr);
    }
    for(int i=0;i<2;i++)
        if(pFlags[i])
        {
            if( pFlags[i]->IsInWorld() )
                pFlags[i]->RemoveFromWorld();
            sObjHolder.Delete<GameObject>(pFlags[i]);
        }*/
}

void WarsongGulch::HandleBattlegroundAreaTrigger(Player *plr, uint32 TriggerID)
{
    uint32 spellid=0;
    int32 buffslot = -1;
    switch(TriggerID)
    {
        case 3686:      // Speed
            buffslot = 0;
            break;
        case 3687:      // Speed (Horde)
            buffslot = 1;
            break;
        case 3706:      // Restoration
            buffslot = 2;
            break;
        case 3708:      // Restoration (Horde)
            buffslot = 3;
            break;
        case 3707:      // Berserking
            buffslot = 4;
            break;
        case 3709:      // Berserking (Horde)
            buffslot = 5;
            break;
        case 3669:
        case 3671:
            {
                RemovePlayer(plr, true, true);
                return;
            }break;
        case 3646:
        case 3647:
            {
                // Flag capture points
                if(plr->GetGUID() == FlagHolders[plr->m_bgTeam ? 0 : 1])
                    if(!((plr->m_bgTeam == 0 && TriggerID == 3647) || (plr->m_bgTeam == 1 && TriggerID == 3646)))
                        if(FlagInbase[plr->m_bgTeam])
                            HandleBattlegroundEvent(((Object*)plr), NULL, BGEVENT_WSG_SCORE_FLAG);
                return;
            }break;
        default:
            {
                sLog.outString("WARNING: Unhandled AreaTrigger in Battleground: %d", TriggerID);
            }break;
    }
    
    if(buffslot > -1 && gbuffs[buffslot])
    {
        spellid = gbuffs[buffslot]->GetInfo()->sound3;
        gbuffs[buffslot]->Expire();
        gbuffs[buffslot] = NULL;
        sEventMgr.AddEvent(this, &WarsongGulch::SpawnBuff,(uint32)0,(uint32)buffslot, EVENT_BATTLEGROUND_SPAWN_BUFF1+buffslot, BUFF_RESPAWN_TIME, 1);  
    }    

    if(spellid)
    {
        SpellEntry *entry = sSpellStore.LookupEntry(spellid);
        if(!entry)
            sLog.outError("WARNING: Tried to add unknown spell id %d to plr.", spellid);
        Spell *spell = new Spell(plr, entry, true,NULL);
        SpellCastTargets targets;
        targets.m_unitTarget = plr->GetGUID();
        targets.m_targetMask = 0x2;
        spell->prepare(&targets);
    }
}

void WarsongGulch::HandleBattlegroundEvent(Object *src, Object *dst, uint16 EventID, uint32 params1, uint32 params2)
{
    switch(EventID)
    {
    case BGEVENT_WSG_PLAYER_DEATH:
        {
            Player *Source = ((Player*)src);
            if(!Source || src->GetTypeId() != TYPEID_PLAYER) return;
            if(Source->m_bgHasFlag)
                HandleBattlegroundEvent(src, dst, BGEVENT_WSG_PLAYER_DIED_WITH_FLAG);

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
    case BGEVENT_WSG_PICKUP_FLAG:
        {
            Player *Source = ((Player*)src);
            if(!Source || src->GetTypeId() != TYPEID_PLAYER || Source->m_MountSpellId) return;
            GameObject *flag = ((GameObject*)dst);
            if(!flag || dst->GetTypeId() != TYPEID_GAMEOBJECT) return;

            uint32 entry = flag->GetUInt32Value(OBJECT_FIELD_ENTRY);
            if((Source->m_bgTeam == 1 && entry == 179831) || (Source->m_bgTeam == 0 && entry == 179830))
            {
                // nope.
                return;
            }
                        
            // Despawn flag
            WorldPacket data(SMSG_GAMEOBJECT_DESPAWN_ANIM, 8);
            data << flag->GetGUID();
            Source->SendMessageToSet(&data, true);

            bool returning = false;

            // Remove from obj and map if it's a dropped flag.. we dont want to remove src flag
            if(flag->GetUInt32Value(GAMEOBJECT_TYPE_ID) == 26)
            {
                if(flag->GetGUID() == FlagGuids[Source->m_bgTeam])
                {
                    returning = true;
                    
                }
                if(!returning)
                {
                    uint32 flagteam = Source->m_bgTeam ? 0 : 1;
                    FlagGuids[flagteam] = 0;
                }
            }

            uint64 flagguid = flag->GetGUID();

            // Check if we're returning or capturing
            if (!returning) HandleBattlegroundEvent(src, dst, BGEVENT_WSG_CAPTURE_FLAG);
            else HandleBattlegroundEvent(src, dst, BGEVENT_WSG_RETURN_FLAG);
        }
        break;
    case BGEVENT_WSG_DROP_FLAG:
        {
            char message[100];
            Player *Source = (Player*)src;
            sprintf(message, "The %s flag was dropped by $N!", Source->m_bgTeam ? "Horde" : "Alliance" );

            WorldPacket * data = sChatHandler.FillMessageData(0x53, 0, message, Source->GetGUID());
            SendPacketToAll(data);
            delete data;
        }
        break;
    case BGEVENT_WSG_CAPTURE_FLAG:
        {
            Player *Source = ((Player*)src);
            if(!Source || src->GetTypeId() != TYPEID_PLAYER || Source->m_MountSpellId) return;
            // Check if it's been dropped somewhere or if someone else has it
            GameObject *flag = ((GameObject*)dst);
            if(!flag || dst->GetTypeId() != TYPEID_GAMEOBJECT) return;
            uint32 entry = flag->GetUInt32Value(OBJECT_FIELD_ENTRY);
            if((Source->m_bgTeam == 1 && entry == 179831) || (Source->m_bgTeam == 0 && entry == 179830))
            {
                // nope.
                return;
            }
            if(flag->GetUInt32Value(GAMEOBJECT_TYPE_ID) != 26)
            {
                uint64 guid = Source->GetGUID();
                std::map<uint64, BattlegroundScore>::iterator itr = m_PlayerScores.find(guid);
                if(itr == m_PlayerScores.end())
                {
                    SendMessageToPlayer(Source, "INTERNAL ERROR: Could not find in internal player score map!", true);
                    return;
                }
                itr->second.FlagCaptures++;
                UpdatePVPData();
            }
            
            if(flag->IsInWorld())
            {
                flag->RemoveFromWorld();
            }
            sObjHolder.Delete<GameObject>(flag);
                                 
           uint32 flagteam = Source->m_bgTeam ? 0 : 1;
           if(FlagGuids[flagteam] != 0 || FlagHolders[flagteam] != 0)
            {
                // nope.
                WorldPacket data(SMSG_AREA_TRIGGER_MESSAGE, 100);
                data << uint32(0);
                data << "Someone else on your team has the opponent's flag or it's been dropped.";
                data << uint8(0);
                Source->GetSession()->SendPacket(&data);
                return;
            }

            SetWorldStateValue(Source->m_bgTeam ? WSG_ALLIANCE_FLAG_CAPTURED:WSG_HORDE_FLAG_CAPTURED,2);

            char message[200];
            sprintf(message, "The %s Flag was picked up by $n!", Source->m_bgTeam ? "Alliance" : "Horde");
            WorldPacket *data2 = BuildMessageChat(0x54, 0x27, message,0, Source->GetGUID());
            SendPacketToAll(data2);
            delete data2;
            
            // Update the worldstate
            if(Source->m_bgTeam == 0)
                SetWorldStateValue(0x60A,-1);
            else
                SetWorldStateValue(0x609,-1);

            // Play sound
            WorldPacket data(SMSG_PLAY_SOUND, 4);
            data << uint32(Source->m_bgTeam ? SOUND_HORDE_CAPTURE : SOUND_ALLIANCE_CAPTURE);
            SendPacketToAll(&data);    
            Source->m_bgHasFlag = true;

            SpellEntry *proto = sSpellStore.LookupEntry(23333+(Source->m_bgTeam*2));
            Spell *sp = new Spell(Source, proto, true, NULL);
            SpellCastTargets targets;
            targets.m_unitTarget = Source->GetGUID();
            targets.m_targetMask = 0x2;
            sp->prepare(&targets);
            FlagInbase[flagteam] = false;
            FlagHolders[flagteam] = Source->GetGUID();
            pFlags[flagteam] = NULL;
        }
        break;
    case BGEVENT_WSG_PLAYER_DIED_WITH_FLAG:
        {
            Player *Source = ((Player*)src);
            if(!Source || src->GetTypeId() != TYPEID_PLAYER) return;
            char message[200];
            sprintf(message, "The %s Flag was dropped by $n!", Source->m_bgTeam ? "Alliance" : "Horde");
            WorldPacket *data2 = BuildMessageChat(0x53, 0x25, message, 0, Source->GetGUID());
            SendPacketToAll(data2);
            delete data2;
            
            SetWorldStateValue(Source->m_bgTeam ? WSG_ALLIANCE_FLAG_CAPTURED:WSG_HORDE_FLAG_CAPTURED,1);

            // Create dropped flag
            GameObject *newflag = sObjHolder.Create<GameObject>();
            newflag->SetInstanceID(GetInstanceID());
            newflag->Create(this->GetMapId(),Source->GetPositionX(),Source->GetPositionY(),Source->GetPositionZ(),Source->GetOrientation());
            newflag->SetUInt32Value(OBJECT_FIELD_TYPE,33);
            newflag->SetUInt32Value(OBJECT_FIELD_ENTRY, (Source->m_bgTeam ? 179785 : 179786));
            newflag->SetFloatValue(OBJECT_FIELD_SCALE_X,2.5f);
            newflag->SetFloatValue(GAMEOBJECT_POS_X, Source->GetPositionX());
            newflag->SetFloatValue(GAMEOBJECT_POS_Y, Source->GetPositionY());
            newflag->SetFloatValue(GAMEOBJECT_POS_Z, Source->GetPositionZ());
            newflag->SetFloatValue(GAMEOBJECT_FACING, Source->GetOrientation());
            newflag->SetUInt32Value(GAMEOBJECT_DISPLAYID,(Source->m_bgTeam ? 5912 : 5913));
            newflag->SetUInt32Value(GAMEOBJECT_STATE, 1);
            newflag->SetUInt32Value(GAMEOBJECT_TYPE_ID,26);
            newflag->SetUInt32Value(GAMEOBJECT_DYN_FLAGS,1);
            newflag->AddToWorld();

            uint32 flagteam = Source->m_bgTeam ? 0 : 1;
            FlagGuids[flagteam] = newflag->GetGUID();
            FlagHolders[flagteam] = 0;  // no holder anymore..

            Source->m_bgHasFlag = false;
            
            // Remove aura from plr
            Source->RemoveAura(23333+(Source->m_bgTeam*2));

            HandleBattlegroundEvent(src, dst, BGEVENT_WSG_DROP_FLAG);
        }
        break;
    case BGEVENT_WSG_SCORE_FLAG:
        {
            Player *Source = ((Player*)src);
            if(!Source || src->GetTypeId() != TYPEID_PLAYER) return;
            // Remove aura from plr
            Source->RemoveAura(23333+(Source->m_bgTeam*2));

            // Remove flag
            uint32 flagteam = Source->m_bgTeam ? 0 : 1;
            FlagGuids[flagteam] = 0;
            FlagHolders[flagteam] = 0;

            // Spawn Flags again
            if(flagteam)
            {
                // warsong flag - horde base
                pFlags[flagteam] = SpawnGameObject(179831, 489, 915.367, 1433.78, 346.089, 3.17301, 0, 210, 2.5f);
                pFlags[flagteam]->SetUInt32Value(GAMEOBJECT_STATE, 1);
                pFlags[flagteam]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 24);
                pFlags[flagteam]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            }
            else
            {
                // silverwing flag - alliance base
                pFlags[flagteam] = SpawnGameObject(179830, 489, 1540.29, 1481.34, 352.64, 3.17301, 0,1314, 2.5f);
                pFlags[flagteam]->SetUInt32Value(GAMEOBJECT_STATE, 1);
                pFlags[flagteam]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 24);
                pFlags[flagteam]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            }
            
            SetWorldStateValue(Source->m_bgTeam ? WSG_ALLIANCE_FLAG_CAPTURED:WSG_HORDE_FLAG_CAPTURED,1);
            std::map<uint64, BattlegroundScore>::iterator itr;
            for(itr = m_PlayerScores.begin(); itr != m_PlayerScores.end(); ++itr)
            {
                if(itr->second.teamid == Source->m_bgTeam)
                    itr->second.BonusHonor+=82;
            }
            
            SetWorldStateValue(Source->m_bgTeam ? WSG_ALLIANCE_FLAG_CAPTURED:WSG_HORDE_FLAG_CAPTURED,1);

            WorldPacket data(SMSG_PLAY_SOUND, 4);
            char message[100];
            sprintf(message, "%s captured the %s flag!", Source->GetName(), Source->m_bgTeam ? "Horde" : "Alliance" );

            WorldPacket * data4 = sChatHandler.FillMessageData(0x53, 0, message, Source->GetGUID());
            SendPacketToAll(data4);
            delete data4;

            data << uint32(Source->m_bgTeam ? SOUND_HORDE_SCORES : SOUND_ALLIANCE_SCORES);
            SendPacketToAll(&data);

            Source->m_bgHasFlag = false;        // oops.. forgot this lawl
            
            // Update teh worldstate
            if(Source->m_bgTeam == 0) {
                SetWorldStateValue(0x60A,0x00);
                SetAllianceScore(GetAllianceScore()+1);
            } 
            else
            {
                SetWorldStateValue(0x609,0x00);
                SetHordeScore(GetHordeScore()+1);                
            }
        }break;
    case BGEVENT_WSG_RETURN_FLAG:
        {
            Player *Source = ((Player*)src);
            if(!Source || src->GetTypeId() != TYPEID_PLAYER) return;
            GameObject *flag = ((GameObject*)dst);
            
            if(flag->IsInWorld())
            {
                flag->RemoveFromWorld();
            }
            sObjHolder.Delete<GameObject>(flag);
            
            char message[200];
            sprintf(message, "The %s Flag was returned to its base by $n!", Source->m_bgTeam ? "Horde" : "Alliance");
            WorldPacket *data2 = BuildMessageChat(0x53, 0x32, message, 0, Source->GetGUID());
            SendPacketToAll(data2);
            delete data2;

            WorldPacket *data3 = sChatHandler.FillMessageData(0x53, 0, message, Source->GetGUID());
            SendPacketToAll(data3);
            delete data3;

            WorldPacket data(SMSG_PLAY_SOUND, 4);
            data << uint32(0x00002000);
            SendPacketToAll(&data);
            
            GameObject *pFlag = NULL;
            uint32 flagteam = Source->m_bgTeam;
            
            SetWorldStateValue(Source->m_bgTeam ? WSG_HORDE_FLAG_CAPTURED:WSG_ALLIANCE_FLAG_CAPTURED,1);
            
            // Spawn Flags again
            if(flagteam)
            {
                // warsong flag - horde base
                pFlags[flagteam] = SpawnGameObject(179831, 489, 915.367, 1433.78, 346.089, 3.17301, 0, 210, 2.5f);
                pFlags[flagteam]->SetUInt32Value(GAMEOBJECT_STATE, 1);
                pFlags[flagteam]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 24);
                pFlags[flagteam]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            }
            else
            {
                // silverwing flag - alliance base
                pFlags[flagteam] = SpawnGameObject(179830, 489, 1540.29, 1481.34, 352.64, 3.17301, 0,1314, 2.5f);
                pFlags[flagteam]->SetUInt32Value(GAMEOBJECT_STATE, 1);
                pFlags[flagteam]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 24);
                pFlags[flagteam]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            }
            
            uint64 guid = Source->GetGUID();
            std::map<uint64, BattlegroundScore>::iterator itr = m_PlayerScores.find(guid);
            if(itr == m_PlayerScores.end())
            {
                SendMessageToPlayer(Source, "INTERNAL ERROR: Could not find in internal player score map!", true);
                return;
            }
            itr->second.FlagReturns++;
            UpdatePVPData();
            
            Source->m_bgHasFlag = false;
            
            
            flagteam = Source->m_bgTeam;
            FlagInbase[flagteam] = true;
            FlagGuids[flagteam] = 0;
            FlagHolders[flagteam] = 0;   
        }break;
    case BGEVENT_WSG_PLAYER_KILL:
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
    default:
        {
            sLog.outString("WARNING: Unhandled battleground event: %d", EventID);
            return;
        }break;
    }
}

void WarsongGulch::SetupBattleground()
{
    // global world state's
    WorldStateVars[0x8D8] = uint32(0x00);
    WorldStateVars[0x8D7] = uint32(0x00);
    WorldStateVars[0x8D6] = uint32(0x00);
    WorldStateVars[0x8D5] = uint32(0x00);
    WorldStateVars[0x8D4] = uint32(0x00);
    WorldStateVars[0x8D3] = uint32(0x00);

    // battleground world state's
    WorldStateVars[WSG_ALLIANCE_FLAG_CAPTURED]  = uint32(0x01);
    WorldStateVars[WSG_HORDE_FLAG_CAPTURED]     = uint32(0x01);
    WorldStateVars[0x60B]                       = uint32(0x00); // unk
    WorldStateVars[WSG_MAX_SCORE]               = uint32(m_MaxScore);
    WorldStateVars[WSG_CURRENT_HORDE_SCORE]     = uint32(0x00);
    WorldStateVars[WSG_CURRENT_ALLIANCE_SCORE]  = uint32(0x00);
    
    WorldStateVars[0x60A] = uint32(0x00);
    WorldStateVars[0x609] = uint32(0x00);

    //sLog.outString("BATTLEGROUND: Warsong Gulch Battleground Setup! %d WorldState Variables Set, 2 Spirit Guides Spawned.", WorldStateVars.size());
}

void WarsongGulch::SpawnBuff(uint32 typeentry,uint32 bannerslot)
{
    switch(bannerslot)
    {    
        case 0:
            gbuffs[bannerslot] = SpawnGameObject(179871, 489, 1449.9296875, 1470.70971679688, 342.634552001953, -1.64060950279236, 0, 114, 1);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.73135370016098);
               gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.681998312473297);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_STATE, 1);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            break;
        case 1:
            gbuffs[bannerslot] = SpawnGameObject(179899, 489, 1005.17071533203, 1447.94567871094, 335.903228759766, 1.64060950279236, 0, 114, 1);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.73135370016098);
               gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_03,0.681998372077942);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_STATE, 1);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            break;
        case 2:
            gbuffs[bannerslot] = SpawnGameObject(179904, 489, 1317.50573730469, 1550.85070800781, 313.234375, -0.26179963350296, 0, 114, 1);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.130526319146156);
               gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.991444826126099);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_STATE, 1);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            break;
        case 3:
            gbuffs[bannerslot] = SpawnGameObject(179906, 489, 1110.45129394531, 1353.65563964844, 316.518096923828, -0.68067866563797, 0, 114, 1);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.333806991577148);
               gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.94264143705368);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_STATE, 1);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            break;
        case 4:
            gbuffs[bannerslot] = SpawnGameObject(179905, 489, 1320.09375, 1378.78967285156, 314.753234863281, 1.18682384490967, 0, 114, 1);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.559192895889282);
               gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_03,0.829037606716156);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_STATE, 1);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            break;
        case 5:
            gbuffs[bannerslot] = SpawnGameObject(179907, 489, 1139.68774414063, 1560.28771972656, 306.843170166016, -2.4434609413147, 0, 114, 1);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.939692616462708);
               gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.342020124197006);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_STATE, 1);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            break;
    }
}

void WarsongGulch::SpawnBattleground()
{
    for(int i=0;i<6;i++)
        SpawnBuff(0,i);
    
	// Alliance Gates
    /*GameObject *gate = SpawnGameObject(179921, 489, 1471.554688, 1458.778076, 362.633240, 0, 33, 114, 2.33271);
    m_Gates.insert(gate);
    
    gate = SpawnGameObject(179919, 489, 1492.477783, 1457.912354, 342.968933, 0, 33, 114, 2.68149);
    m_Gates.insert(gate);
    
    gate = SpawnGameObject(179918, 489, 1503.335327, 1493.465820, 352.188843, 0, 33, 114, 2.26);
    m_Gates.insert(gate);

    // Horde Gates
    gate = SpawnGameObject(179916, 489, 949.1663208, 1423.7717285, 345.6241455, -0.5756807, 32, 114, 0.900901);
    gate->SetFloatValue(GAMEOBJECT_ROTATION,-0.0167336);
    gate->SetFloatValue(GAMEOBJECT_ROTATION_01,-0.004956);
    gate->SetFloatValue(GAMEOBJECT_ROTATION_02,-0.283972);
    gate->SetFloatValue(GAMEOBJECT_ROTATION_03,0.9586736);
    m_Gates.insert(gate);
    
    gate = SpawnGameObject(179917, 489, 953.0507202, 1459.8424072, 340.6525573, -1.9966197, 32, 114, 0.854700);   
    gate->SetFloatValue(GAMEOBJECT_ROTATION,-0.1971825);
    gate->SetFloatValue(GAMEOBJECT_ROTATION_01,0.1575096);
    gate->SetFloatValue(GAMEOBJECT_ROTATION_02,-0.8239487);
    gate->SetFloatValue(GAMEOBJECT_ROTATION_03,0.5073640);
    m_Gates.insert(gate);*/

    // Flags
    // warsong flag - horde base
    pFlags[1] = SpawnGameObject(179831, 489, 915.367, 1433.78, 346.089, 3.17301, 0, 210, 2.5f);
    pFlags[1]->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pFlags[1]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 24);
    pFlags[1]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);

    // silverwing flag - alliance base
    pFlags[0] = SpawnGameObject(179830, 489, 1540.29, 1481.34, 352.64, 3.17301, 0,1314, 2.5f);
    pFlags[0]->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pFlags[0]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 24);
    pFlags[0]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    SpawnSpiritGuides();

    FlagInbase[0] = true;
    FlagInbase[1] = true;

    bSpawned = true;
}

void WarsongGulch::Start()
{
    // Open all the doors

    for(std::set<GameObject*>::iterator itr = m_Gates.begin(); itr!=m_Gates.end(); ++itr)
    {
        (*itr)->SetUInt32Value(GAMEOBJECT_FLAGS, 64);
        (*itr)->SetUInt32Value(GAMEOBJECT_STATE, 0);
    }

    // Send message packet
    WorldPacket *data = BuildMessageChat(0x52, 0x29, "Let the battle for Warsong Gulch begin!", 0);
    SendPacketToAll(data);
    delete data;
    
    data = BuildMessageChat(0x52, 0x29, "The flags are now placed at their bases.", 0);
    SendPacketToAll(data);
    delete data;
    
    WorldPacket pkt;
    pkt.Initialize(SMSG_PLAY_SOUND);
    pkt << uint32(SOUND_BATTLEGROUND_BEGIN); // Battleground Sound Begin
    SendPacketToAll(&pkt);
}

void WarsongGulch::SetAllianceScore(uint32 score)
{
    SetWorldStateValue(WSG_CURRENT_ALLIANCE_SCORE,score);
    if(WorldStateVars[WSG_CURRENT_ALLIANCE_SCORE] == m_MaxScore)
    {
        // set battleground End
        SetBattleGroundStatus(true);
        // alliance wins
        SetBattleGameWin(true);

        // Team wins.
        WorldPacket *data2 = BuildMessageChat(0x53, 0x13, "The Alliance wins!", 0);
        SendPacketToAll(data2);
        delete data2;

        UpdatePVPData();

        WorldPacket data(SMSG_PLAY_SOUND, 4);
        data << uint32(SOUND_ALLIANCEWINS);
        SendPacketToAll(&data);
    }
}

void WarsongGulch::SetHordeScore(uint32 score)
{
    SetWorldStateValue(WSG_CURRENT_HORDE_SCORE,score);
    if(WorldStateVars[WSG_CURRENT_HORDE_SCORE] == m_MaxScore)
    {
        // set battleground win
        SetBattleGroundStatus(true);
        // horde wins
        SetBattleGameWin(false);
        // Team wins.
        WorldPacket *data2 = BuildMessageChat(0x53, 0x13, "The Horde wins!", 0);
        SendPacketToAll(data2);
        delete data2;

        UpdatePVPData();

        WorldPacket data(SMSG_PLAY_SOUND, 4);
        data << uint32(SOUND_HORDEWINS);
        SendPacketToAll(&data);
    }
}

uint32 WarsongGulch::GetAllianceScore()
{
    return WorldStateVars[WSG_CURRENT_ALLIANCE_SCORE];
}

uint32 WarsongGulch::GetHordeScore()
{
    return WorldStateVars[WSG_CURRENT_HORDE_SCORE];
}

void WarsongGulch::SpawnSpiritGuides()
{
    // Alliance Spirit Guide
    SpawnSpiritGuide(1423.218872f, 1554.663574f, 342.833801f, 3.124139f, 0);

    // Horde Spirit Guide
    SpawnSpiritGuide(1032.644775f, 1388.316040f, 340.559937f, 0.043200f, 1);
}