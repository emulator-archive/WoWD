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

AlteracValley::AlteracValley()
{
    bSpawned = false;
    bStarted = false;
    m_BGTime = 0;
    memset(gcbanner,0,sizeof(GameObject*)*MAXOFBANNERS);
}

AlteracValley::~AlteracValley()
{
    // Nothing
}

void AlteracValley::Remove()
{
     // remove some objects
     for(std::set<GameObject*>::iterator itr = m_Gates.begin(); itr!=m_Gates.end(); ++itr)
    {
        if( (*itr)->IsInWorld() )
            (*itr)->RemoveFromWorld();
        sObjHolder.Delete<GameObject>(*itr);
    }
    
    for(int i=0;i<MAXOFBANNERS;i++)
        if(gcbanner[i])
        {
            if( gcbanner[i]->IsInWorld() )
                gcbanner[i]->RemoveFromWorld();
            sObjHolder.Delete<GameObject>(gcbanner[i]);
        }
}

void AlteracValley::HandleBanner(Player *p_caster,GameObject *go,uint32 spellid)
{
    int32 bannerslot = go->bannerslot;
        
    // spawn contest banner
    if(bannerslot > -1 && bannerslot < MAXOFBANNERS)
    {    
        sEventMgr.RemoveEvents(this,EVENT_BATTLEGROUND_AV_CAPTURE_BASE + bannerslot);
        
        // If whe contest a horde/alliance flag, he loses the base
        /*uint32 rbasesfield = p_caster->m_bgTeam ? AB_ALLIANCE_CAPTUREBASE : AB_HORDE_CAPTUREBASE;
        if(go->GetEntry() == 180078 || go->GetEntry() == 180076)
        {
            uint64 guid = p_caster->GetGUID();
            std::map<uint64, BattlegroundScore>::iterator itr = m_PlayerScores.find(guid);
            if(itr == m_PlayerScores.end())
            {
                SendMessageToPlayer(p_caster, "INTERNAL ERROR: Could not find in internal player score map!", true);
                return;
            }
            itr->second.FlagReturns++;
            UpdatePVPData();
            SetWorldStateValue(rbasesfield,WorldStateVars[rbasesfield]-1);
        }*/
        uint32 goentry = 0;
        if(bannerslot < 6) // outside flags are diferent
            goentry = p_caster->m_bgTeam ? 180420:180419;
        else
            goentry = p_caster->m_bgTeam ? 179435:178940;
            
        gcbanner[bannerslot] = SpawnGameObject(goentry, 30, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetOrientation(), 32, 83, 1);
        GameObject *pcbanner = gcbanner[bannerslot];
        pcbanner->SetFloatValue(GAMEOBJECT_ROTATION_02,go->GetFloatValue(GAMEOBJECT_ROTATION_02));
        pcbanner->SetFloatValue(GAMEOBJECT_ROTATION_03,go->GetFloatValue(GAMEOBJECT_ROTATION_03));
        pcbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
        pcbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
        pcbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
        pcbanner->SetUInt32Value(GAMEOBJECT_DYN_FLAGS,1);
        pcbanner->bannerslot = bannerslot;
        
        if(pcbanner->GetInfo()->sound3)
        {        
            pcbanner->pcbannerAura = SpawnGameObject(pcbanner->GetInfo()->sound3, 30, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetOrientation(), 32, 114, 5);
            pcbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_02,go->GetFloatValue(GAMEOBJECT_ROTATION_02));
            pcbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_03,go->GetFloatValue(GAMEOBJECT_ROTATION_03));
            pcbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_STATE, 1);
            pcbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
            pcbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
        }
        
        if(go->pcbannerAura)
        {
            if( go->pcbannerAura->IsInWorld() )
                go->pcbannerAura->RemoveFromWorld();
            sObjHolder.Delete<GameObject>(go->pcbannerAura);
        }
        
        go->pcbannerAura = NULL;
                
        if( go->IsInWorld() )
            go->RemoveFromWorld();
        sObjHolder.Delete<GameObject>(go);


        char message[300];   
        static char *resnames[11] = {"Frostworlf Gravyard","Iceblood Graveyard","Stormpike Gravyard",
                                    "Stoneheart gravyard","Relief Hunt","Stormpike Aid Station",
                                    "Iceblood Tower","Stoneheart Bunker","Icewing Bunker",
                                    "Stormpike North Bunker","Stormpike South Bunker"};
                
        sprintf(message, "$N claims the %s! If left unchallenged, the %s will control it in 1 minute!", resnames[bannerslot] , p_caster->m_bgTeam ? "Horde" : "Alliance" );
        
        WorldPacket *data = BuildMessageChat(p_caster->m_bgTeam ? 0x54:0x53, 0, message, 0,p_caster->GetGUID());
        SendPacketToAll(data);
        delete data;
        
        sEventMgr.AddEvent(this, &AlteracValley::EventCaptureBase, p_caster, (uint32)bannerslot, EVENT_BATTLEGROUND_AV_CAPTURE_BASE + bannerslot, 60000, 1);
    }
    
}

void AlteracValley::HandleBattlegroundAreaTrigger(Player *plr, uint32 TriggerID)
{ 

    uint32 spellid=0;
    int32 buffslot = -1;
    switch(TriggerID)
    {    
        case 2606:
        case 2608:
            {
                RemovePlayer(plr, true, true);
                return;
            }break;
        default:
            {
                sLog.outString("WARNING: Unhandled AreaTrigger in Battleground: %d", TriggerID);
            }break;
    }
}

void AlteracValley::HandleBattlegroundEvent(Object *src, Object *dst, uint16 EventID, uint32 params1, uint32 params2)
{
    WorldPacket data;

    switch(EventID)
    {
    case BGEVENT_AV_GENERAL_DEAD_HORDE:
        {    
            // Play sound
            WorldPacket pkt;
            pkt.Initialize(SMSG_PLAY_SOUND);
            pkt << uint32(SOUND_ALLIANCEWINS);
            SendPacketToAll(&pkt);
            
            // set battleground win
            SetBattleGroundStatus(true);
            // alliance wins
            SetBattleGameWin(true);
    
            UpdatePVPData();
        }break;    
    case BGEVENT_AV_GENERAL_DEAD_ALLIANCE:    
        {    
            // Play sound
            WorldPacket pkt;
            pkt.Initialize(SMSG_PLAY_SOUND);
            pkt << uint32(SOUND_HORDEWINS);
            SendPacketToAll(&pkt);
    
            // set battleground win
            SetBattleGroundStatus(true);
            // horde wins
            SetBattleGameWin(false);
    
            UpdatePVPData();
        }break;    
    case BGEVENT_AV_PLAYER_DEATH:
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
    case BGEVENT_AV_PLAYER_KILL:
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

void AlteracValley::EventCaptureBase(Player *src,uint32 bannerslot)
{
    if(!gcbanner[bannerslot])
        return;
        
       /*uint64 guid = src->GetGUID();
    std::map<uint64, BattlegroundScore>::iterator itr = m_PlayerScores.find(guid);
    if(itr == m_PlayerScores.end())
    {
        SendMessageToPlayer(src, "INTERNAL ERROR: Could not find in internal player score map!", true);
        return;
    }
    itr->second.FlagCaptures++;
    UpdatePVPData();*/
            
    uint32 rbasesfield = src->m_bgTeam ? AB_HORDE_CAPTUREBASE : AB_ALLIANCE_CAPTUREBASE;
       
    uint32 goentry = 0;
    if(bannerslot < 6) // outside flags are diferent
        goentry = src->m_bgTeam ? 178393:178389;
    else
        goentry = src->m_bgTeam ? 178946:178935;

    GameObject *pcbanner = src->GetCurrentBattleground()->SpawnGameObject(goentry, 30, gcbanner[bannerslot]->GetPositionX(), gcbanner[bannerslot]->GetPositionY(), gcbanner[bannerslot]->GetPositionZ(), gcbanner[bannerslot]->GetOrientation(), 32, src->m_bgTeam ?84:83, 1);
    pcbanner->SetFloatValue(GAMEOBJECT_ROTATION_02,gcbanner[bannerslot]->GetFloatValue(GAMEOBJECT_ROTATION_02));
    pcbanner->SetFloatValue(GAMEOBJECT_ROTATION_03,gcbanner[bannerslot]->GetFloatValue(GAMEOBJECT_ROTATION_03));
    pcbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pcbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pcbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    if(pcbanner->GetInfo()->sound3)
    {
        pcbanner->pcbannerAura = src->GetCurrentBattleground()->SpawnGameObject(pcbanner->GetInfo()->sound3, 30, pcbanner->GetPositionX(), pcbanner->GetPositionY(), pcbanner->GetPositionZ(), pcbanner->GetOrientation(), 32, 114, 5);
        pcbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_02,pcbanner->GetFloatValue(GAMEOBJECT_ROTATION_02));
        pcbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_03,pcbanner->GetFloatValue(GAMEOBJECT_ROTATION_03));
        pcbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_STATE, 1);
        pcbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
        pcbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    }
    
    if(gcbanner[bannerslot]->pcbannerAura)
    {
        if( gcbanner[bannerslot]->pcbannerAura->IsInWorld() )
            gcbanner[bannerslot]->pcbannerAura->RemoveFromWorld();
        sObjHolder.Delete<GameObject>(gcbanner[bannerslot]->pcbannerAura);
    }
    
    if( gcbanner[bannerslot]->IsInWorld() )
            gcbanner[bannerslot]->RemoveFromWorld();
    sObjHolder.Delete<GameObject>(gcbanner[bannerslot]);
    
    gcbanner[bannerslot] = NULL;
    
    // Play sound
    WorldPacket pkt;
    pkt.Initialize(SMSG_PLAY_SOUND);
    pkt << uint32(src->m_bgTeam ? SOUND_HORDE_CAPTURE : SOUND_ALLIANCE_CAPTURE);
    SendPacketToAll(&pkt);
    
    char message[300];   
    static char *resnames[11] = {"Frostworlf Gravyard","Iceblood Graveyard","Stormpike Gravyard",
                                    "Stoneheart gravyard","Relief Hunt","Stormpike Aid Station",
                                    "Iceblood Tower","Stoneheart Bunker","Icewing Bunker",
                                    "Stormpike North Bunker","Stormpike South Bunker"};
                    
    sprintf(message, "The %s has taken the %s!", src->m_bgTeam ? "Horde" : "Alliance" , resnames[bannerslot] );
        
    WorldPacket *data = BuildMessageChat(src->m_bgTeam ? 0x54:0x53, 0x27, message, 0);
    SendPacketToAll(data);
    delete data;
}

void AlteracValley::SetupBattleground()
{
    // default world state's
    WorldStateVars[0x8D8] = uint32(0x00);
    WorldStateVars[0x8D7] = uint32(0x00);
    WorldStateVars[0x8D6] = uint32(0x00);
    WorldStateVars[0x8D5] = uint32(0x00);
    //WorldStateVars[0x8D4] = uint32(0x00);
    WorldStateVars[0x8D3] = uint32(0x00);
    
    WorldStateVars[0x574] = uint32(0x00);
    WorldStateVars[0x573] = uint32(0x00);
    WorldStateVars[0x572] = uint32(0x00);
    WorldStateVars[0x56F] = uint32(0x00);
    WorldStateVars[0x56E] = uint32(0x00);
    WorldStateVars[0x56D] = uint32(0x00);
    WorldStateVars[0x56C] = uint32(0x00);
    WorldStateVars[0x56B] = uint32(0x00);
    WorldStateVars[0x56A] = uint32(0x01); // unk
    
    WorldStateVars[AV_CONTROLED_ICEBLOOD_TOWER_HORDE] = uint32(0x01);
    WorldStateVars[AV_CONTROLED_TOWER_POINT_HORDE] = uint32(0x01);
    
    WorldStateVars[0x565] = uint32(0x00);
    WorldStateVars[0x564] = uint32(0x00);
    WorldStateVars[0x563] = uint32(0x00);
    WorldStateVars[0x562] = uint32(0x00);
    WorldStateVars[0x561] = uint32(0x00);
    WorldStateVars[0x560] = uint32(0x00);
    WorldStateVars[0x55F] = uint32(0x00);
    WorldStateVars[0x55E] = uint32(0x00);
    WorldStateVars[0x55D] = uint32(0x00);
    
    WorldStateVars[AV_UNCONTROLED_SNOWFALL_GRAVE] = uint32(0x01);
    WorldStateVars[AV_CONTROLED_FROSTWOLF_RELIFHUNT_HORDE] = uint32(0x01);
    
    WorldStateVars[0x531] = uint32(0x00);
    WorldStateVars[0x52E] = uint32(0x00);
    WorldStateVars[0x518] = uint32(0x00);
    WorldStateVars[0x517] = uint32(0x00);
    WorldStateVars[0x571] = uint32(0x00);
    WorldStateVars[0x570] = uint32(0x00);
    
    WorldStateVars[AV_CONTROLED_EAST_FROSTWOLF_TOWER_HORDE] = uint32(0x01);
    WorldStateVars[AV_CONTROLED_WEST_FROSTWOLF_TOWER_HORDE] = uint32(0x01); 
    WorldStateVars[AV_CONTROLED_IRONDEEP_MINE_TROGG] = uint32(0x01);
    
    WorldStateVars[0x544] = uint32(0x00);
    WorldStateVars[0x536] = uint32(0x00);
    
    WorldStateVars[AV_CONTROLED_STORMPIKE_GRAVE_ALLIANCE] = uint32(0x01);
    
    WorldStateVars[0x3C6] = uint32(0x04);
    WorldStateVars[0x3C4] = uint32(0x06);
    WorldStateVars[0x3C2] = uint32(0x04);
    
    WorldStateVars[0x55C] = uint32(0x00);
    WorldStateVars[0x55B] = uint32(0x00);
    WorldStateVars[0x55A] = uint32(0x00);
    WorldStateVars[0x559] = uint32(0x00);
    WorldStateVars[0x558] = uint32(0x00);
    WorldStateVars[0x557] = uint32(0x00);
    WorldStateVars[0x556] = uint32(0x00);
    WorldStateVars[0x555] = uint32(0x00);
    
    WorldStateVars[AV_CONTROLED_STONEHEART_BUNKER_ALLIANCE] = uint32(0x01);
    WorldStateVars[AV_CONTROLED_ICEWING_BUNKER_ALLIANCE] = uint32(0x01);
    WorldStateVars[AV_CONTROLED_DUBALDER_NORTH_BUNKER_ALLIANCE] = uint32(0x01);
    WorldStateVars[AV_CONTROLED_DUBALDER_SOUTH_BUNKER_ALLIANCE] = uint32(0x01);
    
    WorldStateVars[0x54F] = uint32(0x00);
    WorldStateVars[0x54E] = uint32(0x00);
    
    WorldStateVars[AV_CONTROLED_COLDTHOOT_MINE_KOBOLT] = uint32(0x01);
    
    WorldStateVars[0x54C] = uint32(0x00);
    WorldStateVars[0x54B] = uint32(0x00);
    WorldStateVars[0x545] = uint32(0x00);
    
    WorldStateVars[AV_CONTROLED_ICEBLOOD_GRAVE_HORDE] = uint32(0x01);
    
    WorldStateVars[0x542] = uint32(0x00);
    WorldStateVars[0x540] = uint32(0x00);
    WorldStateVars[0x53F] = uint32(0x00);
    WorldStateVars[0x53E] = uint32(0x00);
    WorldStateVars[0x53D] = uint32(0x00);
    WorldStateVars[0x53C] = uint32(0x00);
    WorldStateVars[0x53B] = uint32(0x00);
    
    WorldStateVars[AV_CONTROLED_FROSTWOLF_GRAVE_HORDE] = uint32(0x01);
    
    WorldStateVars[0x539] = uint32(0x00);
    WorldStateVars[0x538] = uint32(0x00);
    WorldStateVars[0x537] = uint32(0x00);
    WorldStateVars[0x534] = uint32(0x00);
    WorldStateVars[0x533] = uint32(0x00);
    WorldStateVars[0x530] = uint32(0x00);
    WorldStateVars[0x52F] = uint32(0x00);
    
    WorldStateVars[AV_CONTROLED_STORMPIKE_AID_STATION_ALLIANCE] = uint32(0x01);
    WorldStateVars[AV_CONTROLED_STONEHEART_GRAVE_ALLIANCE] = uint32(0x01);
    
    WorldStateVars[0x515] = uint32(0x00);
    WorldStateVars[0x3B6] = uint32(0x06);

}

void AlteracValley::SpawnBattleground()
{
        
    // Alliance Gate
    /*GameObject *gate = SpawnGameObject(180255, 529, 1284.597290, 1281.166626, -15.977916, 0.706859, 32, 114, 1.5799990);
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
    m_Gates.insert(gate);*/
    
    // -- Places Banners --
    
    // Frostworlf gravyard
    GameObject *pbanner = SpawnGameObject(178393 ,30 ,-1083.244385 ,-345.994507 ,55.124435 ,4.596910, 32, 84, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    if(pbanner->GetInfo()->sound3)
    {
        pbanner->pcbannerAura = SpawnGameObject(pbanner->GetInfo()->sound3, 30, pbanner->GetPositionX(), pbanner->GetPositionY(), pbanner->GetPositionZ(), pbanner->GetOrientation(), 32, 114, 5);
        pbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_02,pbanner->GetFloatValue(GAMEOBJECT_ROTATION_02));
        pbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_03,pbanner->GetFloatValue(GAMEOBJECT_ROTATION_03));
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_STATE, 1);
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    }
    
    pbanner->bannerslot = 0;
    
    // Iceblood graveyard
    pbanner = SpawnGameObject(178393, 30 ,-612.329285 ,-396.660095 ,60.858463 ,2.916166, 32, 84, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    if(pbanner->GetInfo()->sound3)
    {
        pbanner->pcbannerAura = SpawnGameObject(pbanner->GetInfo()->sound3, 30, pbanner->GetPositionX(), pbanner->GetPositionY(), pbanner->GetPositionZ(), pbanner->GetOrientation(), 32, 114, 5);
        pbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_02,pbanner->GetFloatValue(GAMEOBJECT_ROTATION_02));
        pbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_03,pbanner->GetFloatValue(GAMEOBJECT_ROTATION_03));
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_STATE, 1);
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    }
    
    pbanner->bannerslot = 1;
    
    // Stormpike gravyard
    pbanner = SpawnGameObject(178389, 30 ,669.968567 ,-293.467468 ,30.283821 ,3.152932, 32, 83, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    if(pbanner->GetInfo()->sound3)
    {
        pbanner->pcbannerAura = SpawnGameObject(pbanner->GetInfo()->sound3, 30, pbanner->GetPositionX(), pbanner->GetPositionY(), pbanner->GetPositionZ(), pbanner->GetOrientation(), 32, 114, 5);
        pbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_02,pbanner->GetFloatValue(GAMEOBJECT_ROTATION_02));
        pbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_03,pbanner->GetFloatValue(GAMEOBJECT_ROTATION_03));
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_STATE, 1);
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    }
    
    pbanner->bannerslot = 2;
    
    // Stoneheart gravyard
    pbanner = SpawnGameObject(178389, 30 ,78.205989 ,-405.1715 ,47.11583 ,1.535766, 32, 83, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    if(pbanner->GetInfo()->sound3)
    {
        pbanner->pcbannerAura = SpawnGameObject(pbanner->GetInfo()->sound3, 30, pbanner->GetPositionX(), pbanner->GetPositionY(), pbanner->GetPositionZ(), pbanner->GetOrientation(), 32, 114, 5);
        pbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_02,pbanner->GetFloatValue(GAMEOBJECT_ROTATION_02));
        pbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_03,pbanner->GetFloatValue(GAMEOBJECT_ROTATION_03));
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_STATE, 1);
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    }
    
    pbanner->bannerslot = 3;
           
    // Relief hunt
    pbanner = SpawnGameObject(178393, 30 ,-1403.875000 ,-309.393738 ,89.372269 ,0.204413, 32, 84, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    if(pbanner->GetInfo()->sound3)
    {
        pbanner->pcbannerAura = SpawnGameObject(pbanner->GetInfo()->sound3, 30, pbanner->GetPositionX(), pbanner->GetPositionY(), pbanner->GetPositionZ(), pbanner->GetOrientation(), 32, 114, 5);
        pbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_02,pbanner->GetFloatValue(GAMEOBJECT_ROTATION_02));
        pbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_03,pbanner->GetFloatValue(GAMEOBJECT_ROTATION_03));
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_STATE, 1);
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    }
    
    pbanner->bannerslot = 4;
    
    // Stormpike aid station
    pbanner = SpawnGameObject(178389, 30 ,639.348206 ,-31.571268 ,46.222248 ,4.638909, 32, 83, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    if(pbanner->GetInfo()->sound3)
    {
        pbanner->pcbannerAura = SpawnGameObject(pbanner->GetInfo()->sound3, 30, pbanner->GetPositionX(), pbanner->GetPositionY(), pbanner->GetPositionZ(), pbanner->GetOrientation(), 32, 114, 5);
        pbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_02,pbanner->GetFloatValue(GAMEOBJECT_ROTATION_02));
        pbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_03,pbanner->GetFloatValue(GAMEOBJECT_ROTATION_03));
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_STATE, 1);
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
        pbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    }
    
    pbanner->bannerslot = 5;
    
    // iceblood tower
    pbanner = SpawnGameObject(178946, 30 ,-571.080994 ,-263.751007 ,75.092003 ,5.410520, 32, 84, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    pbanner->bannerslot = 6;
    
    // Stoneheart bunker
    pbanner = SpawnGameObject(178935, 30 ,-149.734329 ,-441.907593 ,40.386887 ,4.147903, 32, 83, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    pbanner->bannerslot = 7;
       
    // Icewing bunker
    pbanner = SpawnGameObject(178935, 30 ,204.959793 ,-357.945190 ,56.377651 ,5.133710, 32, 83, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    pbanner->bannerslot = 8;
       
    // stormpike north bunker
    pbanner = SpawnGameObject(178935, 30 ,670.908020 ,-142.491776 ,63.662659 ,0.876855, 32, 83, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    pbanner->bannerslot = 9;
       
    // stormpike south bunker
    pbanner = SpawnGameObject(178935, 30 ,555.507324 ,-76.193176 ,51.934444 ,4.996268, 32, 83, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    pbanner->bannerslot = 10;
       
    bSpawned = true;
}

void AlteracValley::Start()
{
    // Open all the doors

    for(std::set<GameObject*>::iterator itr = m_Gates.begin(); itr!=m_Gates.end(); ++itr)
    {
        (*itr)->SetUInt32Value(GAMEOBJECT_FLAGS, 33);
        (*itr)->SetUInt32Value(GAMEOBJECT_STATE, 0);
    }

    // Send message packet
    WorldPacket *data = BuildMessageChat(0x52, 0x27, "The Battle for Alterac Valley has begun!", 0);
    SendPacketToAll(data);
    delete data;
    
    WorldPacket pkt;
    pkt.Initialize(SMSG_PLAY_SOUND);
    pkt << uint32(SOUND_BATTLEGROUND_BEGIN); // Battleground Sound Begin
    SendPacketToAll(&pkt);
}
