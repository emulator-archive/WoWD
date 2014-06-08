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

#define BASE_RESOURCES_UPDATE_TIME 10000
#define BASE_RESOURCES_GAIN 10
#define RESOURCES_WINVAL 2000
#define RESOURCES_TO_GAIN_BH 200

// Berserk entry = 180380
// Food entry = 180362
// Speed entry = 180146
int buffentrys[3] = {180380,180362,180146};

#define BUFF_RESPAWN_TIME 90000

// TODO:
//    LEVEL                         BHU 20-29     30-39  40-49   50-59      60
// ----------------------------------------------------------------------------
//Every 330 resources, non-holiday      1      41      68     113     189     198
//Winning the match, non-holiday      1      41      68     113     189     198
// ----------------------------------------------------------------------------
//Total Win bonus, non-holiday          7      287     476     791     1323     1386
// ----------------------------------------------------------------------------
//Every 200 resources, holiday          1      41      68     113     189     198
//Winning the match, holiday          1      41      68     113     189     198
// ----------------------------------------------------------------------------
//Total Win bonus, holiday              11  451     748     1243     2079     2178
// ----------------------------------------------------------------------------

#define BASE_BH_GAIN 41

ArathiBasin::ArathiBasin()
{
    bSpawned = false;
    bStarted = false;
    m_BGTime = 0;
    AllianceRes = HordeRes = 0;
    memset(gcbanner,0,sizeof(GameObject*)*5);
    almostend[0] = false;
    almostend[1] = false;
    
}

ArathiBasin::~ArathiBasin()
{
    // Nothing
}

void ArathiBasin::Remove()
{
     // remove some objects
    /* for(std::set<GameObject*>::iterator itr = m_Gates.begin(); itr!=m_Gates.end(); ++itr)
    {
        if( (*itr)->IsInWorld() )
            (*itr)->RemoveFromWorld();
        sObjHolder.Delete<GameObject>(*itr);
    }
    
    for(int i=0;i<5;i++)
        if(gcbanner[i])
        {
            if( gcbanner[i]->IsInWorld() )
                gcbanner[i]->RemoveFromWorld();
            sObjHolder.Delete<GameObject>(gcbanner[i]);
        }*/
}

void ArathiBasin::HandleBanner(Player *p_caster,GameObject *go,uint32 spellid)
{

    if((go->GetEntry() == 180086 && p_caster->m_bgTeam) ||  (go->GetEntry() == 180085 && !p_caster->m_bgTeam))
        return;
    
    int32 bannerslot = go->bannerslot;
                   
    // spawn contest banner
    if(bannerslot > -1 && bannerslot < 5)
    {
        sEventMgr.RemoveEvents(this,EVENT_BATTLEGROUND_STABLE_RCONTEST + bannerslot);
        
        // If whe contest a horde/alliance flag, he loses the base
        uint32 rbasesfield = p_caster->m_bgTeam ? AB_ALLIANCE_CAPTUREBASE : AB_HORDE_CAPTUREBASE;
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
        }

        uint32 goentry = p_caster->m_bgTeam ? 180086:180085;
        gcbanner[bannerslot] = SpawnGameObject(goentry, 529, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetOrientation(), 32, 83, 1);
        GameObject *pcbanner = gcbanner[bannerslot];
        pcbanner->SetFloatValue(GAMEOBJECT_ROTATION_02,go->GetFloatValue(GAMEOBJECT_ROTATION_02));
        pcbanner->SetFloatValue(GAMEOBJECT_ROTATION_03,go->GetFloatValue(GAMEOBJECT_ROTATION_03));
        pcbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
        pcbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
        pcbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
        pcbanner->SetUInt32Value(GAMEOBJECT_DYN_FLAGS,1);
        pcbanner->bannerslot = bannerslot;
                
        pcbanner->pcbannerAura = SpawnGameObject(pcbanner->GetInfo()->sound3, 529, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetOrientation(), 32, 114, 5);
        pcbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_02,go->GetFloatValue(GAMEOBJECT_ROTATION_02));
        pcbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_03,go->GetFloatValue(GAMEOBJECT_ROTATION_03));
        pcbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_STATE, 1);
        pcbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
        pcbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
        
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
        static char *resnames[5] = {"Stable","Farm","Blacksmith","Mine","Lumber Mill"};
                
        sprintf(message, "$N claims the %s! If left unchallenged, the %s will control it in 1 minute!", resnames[bannerslot] , p_caster->m_bgTeam ? "Horde" : "Alliance" );
        
        WorldPacket *data = BuildMessageChat(p_caster->m_bgTeam ? 0x54:0x53, 0, message, 0,p_caster->GetGUID());
        SendPacketToAll(data);
        delete data;
        
        setBaseCapturingValue(bannerslot,p_caster->m_bgTeam,1);
        setBaseCapturingValue(bannerslot,!p_caster->m_bgTeam,0);   
        setBaseCapturedValue(bannerslot,p_caster->m_bgTeam,0);
        setBaseCapturedValue(bannerslot,!p_caster->m_bgTeam,0);
                
        sEventMgr.AddEvent(this, &ArathiBasin::EventResourcesCapture, p_caster, (uint32)bannerslot, EVENT_BATTLEGROUND_STABLE_RCONTEST + bannerslot, 60000, 1);
    }
}

void ArathiBasin::HandleBattlegroundAreaTrigger(Player *plr, uint32 TriggerID)
{ 

    uint32 spellid=0;
    int32 buffslot = -1;
    switch(TriggerID)
    {
        case 3866:      // stables buff trigger
            buffslot = 0;
            break;
        case 3867:      // farm buff trigger
            buffslot = 2;
            break;
        case 3868:      // lumber mill buff trigger
            buffslot = 3;
            break;
        case 3869:      // mine buff trigger
            buffslot = 4;
            break;        
        case 3870:      // blacksmith buff trigger
            buffslot = 1;
            break;               
        case 3948:
        case 3949:
            {
                RemovePlayer(plr, true, true);
                return;
            }break;
        default:
            {
                sLog.outString("WARNING: Unhandled AreaTrigger in Battleground: %d", TriggerID);
            }break;
    }
    
    if(buffslot > -1 && gbuffs[buffslot] && plr->isAlive())
    {
        spellid = gbuffs[buffslot]->GetInfo()->sound3;
        gbuffs[buffslot]->Expire();
        gbuffs[buffslot] = NULL;
        sEventMgr.AddEvent(this, &ArathiBasin::SpawnBuff,(uint32)buffentrys[(int)rand()%3],(uint32)buffslot, EVENT_BATTLEGROUND_SPAWN_BUFF1+buffslot, BUFF_RESPAWN_TIME, 1);  
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

void ArathiBasin::HandleBattlegroundEvent(Object *src, Object *dst, uint16 EventID, uint32 params1, uint32 params2)
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

void ArathiBasin::EventUpdateResourcesAlliance()
{
    if(m_GameStatus)
        return;
    
    uint32 rfield = AB_ALLIANCE_RESOURCES;
    uint32 rbasesfield = AB_ALLIANCE_CAPTUREBASE;
    
    if(WorldStateVars[rbasesfield])
        SetWorldStateValue(rfield,WorldStateVars[rfield]+BASE_RESOURCES_GAIN);
        
    if(WorldStateVars[rfield] && !(WorldStateVars[rfield]%RESOURCES_TO_GAIN_BH))
    {
        std::map<uint64, BattlegroundScore>::iterator itr;
        for(itr = m_PlayerScores.begin(); itr != m_PlayerScores.end(); ++itr)
        {
            if(!itr->second.teamid)
                itr->second.BonusHonor+=BASE_BH_GAIN;
        }
    }
    
    if(WorldStateVars[rfield] >= RESOURCES_WINVAL)
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
        
    }    
    else if(WorldStateVars[rfield] >= uint32(RESOURCES_WINVAL*0.9) && !almostend[0])
    {
        almostend[0] = true;
        // Play sound
        WorldPacket pkt;
        pkt.Initialize(SMSG_PLAY_SOUND);
        pkt << uint32(SOUND_ALLIANCE_BGALMOSTEND);
        SendPacketToAll(&pkt);
        
        char message[300];        
        sprintf(message, "The Alliance has gathered %d resources, and is near victory!", WorldStateVars[rfield] );
        WorldPacket *data = BuildMessageChat(0x52, 0x29, message, 0);
        SendPacketToAll(data);
        delete data;
    }        
}

void ArathiBasin::EventUpdateResourcesHorde()
{
    if(m_GameStatus)
        return;
    
    uint32 rfield = AB_HORDE_RESOURCES;
    uint32 rbasesfield = AB_HORDE_CAPTUREBASE;
    
    if(WorldStateVars[rbasesfield])
        SetWorldStateValue(rfield,WorldStateVars[rfield]+BASE_RESOURCES_GAIN);
    
    if(WorldStateVars[rfield] && !(WorldStateVars[rfield]%RESOURCES_TO_GAIN_BH))
    {
        std::map<uint64, BattlegroundScore>::iterator itr;
        for(itr = m_PlayerScores.begin(); itr != m_PlayerScores.end(); ++itr)
        {
            if(itr->second.teamid)
                itr->second.BonusHonor+=BASE_BH_GAIN;
        }
    }
    
    if(WorldStateVars[rfield] >= RESOURCES_WINVAL)
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
    }    
    else if(WorldStateVars[rfield] >= uint32(RESOURCES_WINVAL*0.9) && !almostend[1])
    {
        almostend[1] = true;
        // Play sound
        WorldPacket pkt;
        pkt.Initialize(SMSG_PLAY_SOUND);
        pkt << uint32(SOUND_HORDE_BGALMOSTEND);
        SendPacketToAll(&pkt);
        
        char message[300];        
        sprintf(message, "The Horde has gathered %d resources, and is near victory!", WorldStateVars[rfield] );
        WorldPacket *data = BuildMessageChat(0x52, 0x29, message, 0);
        SendPacketToAll(data);
        delete data;
    }    
}

void ArathiBasin::EventResourcesCapture(Player *src,uint32 bannerslot)
{   
    if(!gcbanner[bannerslot])
        return;
        
       uint64 guid = src->GetGUID();
    std::map<uint64, BattlegroundScore>::iterator itr = m_PlayerScores.find(guid);
    if(itr == m_PlayerScores.end())
    {
        SendMessageToPlayer(src, "INTERNAL ERROR: Could not find in internal player score map!", true);
        return;
    }
    itr->second.FlagCaptures++;
    UpdatePVPData();
            
    uint32 rbasesfield = src->m_bgTeam ? AB_HORDE_CAPTUREBASE : AB_ALLIANCE_CAPTUREBASE;
       
    uint32 goentry = src->m_bgTeam ? 180078:180076;

    GameObject *pcbanner = src->GetCurrentBattleground()->SpawnGameObject(goentry, 529, gcbanner[bannerslot]->GetPositionX(), gcbanner[bannerslot]->GetPositionY(), gcbanner[bannerslot]->GetPositionZ(), gcbanner[bannerslot]->GetOrientation(), 32, src->m_bgTeam ?84:83, 1);
    pcbanner->SetFloatValue(GAMEOBJECT_ROTATION_02,gcbanner[bannerslot]->GetFloatValue(GAMEOBJECT_ROTATION_02));
    pcbanner->SetFloatValue(GAMEOBJECT_ROTATION_03,gcbanner[bannerslot]->GetFloatValue(GAMEOBJECT_ROTATION_03));
    pcbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pcbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 1);
    pcbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    pcbanner->pcbannerAura = src->GetCurrentBattleground()->SpawnGameObject(pcbanner->GetInfo()->sound3, 529, pcbanner->GetPositionX(), pcbanner->GetPositionY(), pcbanner->GetPositionZ(), pcbanner->GetOrientation(), 32, 114, 5);
    pcbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_02,pcbanner->GetFloatValue(GAMEOBJECT_ROTATION_02));
    pcbanner->pcbannerAura->SetFloatValue(GAMEOBJECT_ROTATION_03,pcbanner->GetFloatValue(GAMEOBJECT_ROTATION_03));
    pcbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pcbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
    pcbanner->pcbannerAura->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
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
    static char *Resnames[5] = {"Stable","Farm","Blacksmith","Mine","Lumber Mill"};               
    sprintf(message, "The %s has taken the %s!", src->m_bgTeam ? "Horde" : "Alliance" , Resnames[bannerslot] );
        
    
    WorldPacket *data = BuildMessageChat(src->m_bgTeam ? 0x54:0x53, 0x27, message, 0);
    SendPacketToAll(data);
    delete data;
     
    
    SetWorldStateValue(rbasesfield,WorldStateVars[rbasesfield]+1);
    
    // updates map resource as captured
    static uint32 capturedfields[5][2] = {AB_CAPTURED_STABLES_ALLIANCE,AB_CAPTURED_FARM_ALLIANCE,AB_CAPTURED_BLACKSMITH_ALLIANCE,AB_CAPTURED_GOLDMINE_ALLIANCE,AB_CAPTURED_LUMBERMILL_ALLIANCE,
                                          AB_CAPTURED_STABLES_HORDE,   AB_CAPTURED_FARM_HORDE,   AB_CAPTURED_BLACKSMITH_HORDE,   AB_CAPTURED_GOLDMINE_HORDE,   AB_CAPTURED_LUMBERMILL_HORDE};
   
    setBaseCapturingValue(bannerslot,src->m_bgTeam,0);
    setBaseCapturingValue(bannerslot,!src->m_bgTeam,0);   
    setBaseCapturedValue(bannerslot,src->m_bgTeam,1);
    setBaseCapturedValue(bannerslot,!src->m_bgTeam,0);
    
    if(WorldStateVars[rbasesfield] )
    {
        sEventMgr.ModifyEventTime(this, EVENT_BATTLEGROUND_UPDATE_RESOURCES_ALLIANCE + src->m_bgTeam, (uint32)(BASE_RESOURCES_UPDATE_TIME/WorldStateVars[rbasesfield]));
    }
}

void ArathiBasin::setBaseCapturedValue(uint32 bannerslot,uint32 team,uint32 value)
{
    static uint32 capturedfields[2][5] = 
    {
        {AB_CAPTURED_STABLES_ALLIANCE,AB_CAPTURED_FARM_ALLIANCE,AB_CAPTURED_BLACKSMITH_ALLIANCE,AB_CAPTURED_GOLDMINE_ALLIANCE,AB_CAPTURED_LUMBERMILL_ALLIANCE},
        {AB_CAPTURED_STABLES_HORDE,   AB_CAPTURED_FARM_HORDE,   AB_CAPTURED_BLACKSMITH_HORDE,   AB_CAPTURED_GOLDMINE_HORDE,   AB_CAPTURED_LUMBERMILL_HORDE}
    };
   
    SetWorldStateValue(capturedfields[team][bannerslot],value);    
}

void ArathiBasin::setBaseCapturingValue(uint32 bannerslot,uint32 team,uint32 value)
{
    static uint32 capturingfields[2][5] = 
    {
        {AB_CAPTURING_STABLES_ALLIANCE, AB_CAPTURING_FARM_ALLIANCE,AB_CAPTURING_BLACKSMITH_ALLIANCE,AB_CAPTURING_GOLDMINE_ALLIANCE,AB_CAPTURING_LUMBERMILL_ALLIANCE},
        {AB_CAPTURING_STABLES_HORDE,   AB_CAPTURING_FARM_HORDE,   AB_CAPTURING_BLACKSMITH_HORDE,   AB_CAPTURING_GOLDMINE_HORDE,   AB_CAPTURING_LUMBERMILL_HORDE}
    };

    SetWorldStateValue(capturingfields[team][bannerslot],value);
}

void ArathiBasin::SetupBattleground()
{
    // default world state's
    WorldStateVars[0x8D8] = uint32(0x00);
    WorldStateVars[0x8D7] = uint32(0x00);
    WorldStateVars[0x8D6] = uint32(0x00);
    WorldStateVars[0x8D5] = uint32(0x00);
    WorldStateVars[0x8D4] = uint32(0x00);
    WorldStateVars[0x8D3] = uint32(0x00);

    // AB world state's
    // unknowns, need more research
    WorldStateVars[0x7A3] = uint32(1800); // unknown
    WorldStateVars[0x745] = uint32(0x02); // unknown

    // Icon stuff for on the map
    WorldStateVars[AB_SHOW_BACKSMITH_ICON]              = uint32(0x01);
    WorldStateVars[AB_SHOW_FARM_ICON]                   = uint32(0x01);
    WorldStateVars[AB_SHOW_LUMBERMILL_ICON]             = uint32(0x01);
    WorldStateVars[AB_SHOW_GOLDMINE_ICON]               = uint32(0x01);
    WorldStateVars[AB_SHOW_STABLE_ICON]                 = uint32(0x01);
    
    // LumberMill
    WorldStateVars[AB_CAPTURING_LUMBERMILL_HORDE]       = uint32(0x00);
    WorldStateVars[AB_CAPTURING_LUMBERMILL_ALLIANCE]    = uint32(0x00);
    WorldStateVars[AB_CAPTURED_LUMBERMILL_HORDE]        = uint32(0x00);
    WorldStateVars[AB_CAPTURED_LUMBERMILL_ALLIANCE]     = uint32(0x00);

    // GoldMine
    WorldStateVars[AB_CAPTURING_GOLDMINE_HORDE]         = uint32(0x00);
    WorldStateVars[AB_CAPTURING_GOLDMINE_ALLIANCE]      = uint32(0x00);
    WorldStateVars[AB_CAPTURED_GOLDMINE_HORDE]          = uint32(0x00);
    WorldStateVars[AB_CAPTURED_GOLDMINE_ALLIANCE]       = uint32(0x00);

    // BlackSmith
    WorldStateVars[AB_CAPTURING_BLACKSMITH_HORDE]       = uint32(0x00);
    WorldStateVars[AB_CAPTURING_BLACKSMITH_ALLIANCE]    = uint32(0x00);
    WorldStateVars[AB_CAPTURED_BLACKSMITH_HORDE]        = uint32(0x00);
    WorldStateVars[AB_CAPTURED_BLACKSMITH_ALLIANCE]     = uint32(0x00);

    WorldStateVars[AB_MAX_SCORE]                        = uint32(RESOURCES_WINVAL);
    WorldStateVars[AB_ALLIANCE_CAPTUREBASE]             = uint32(0x00);
    WorldStateVars[AB_HORDE_CAPTUREBASE]                = uint32(0x00);
    WorldStateVars[AB_HORDE_RESOURCES]                  = uint32(0x00);
    WorldStateVars[AB_ALLIANCE_RESOURCES]               = uint32(0x00);

    // Farm
    WorldStateVars[AB_CAPTURING_FARM_ALLIANCE]         = uint32(0x00);
    WorldStateVars[AB_CAPTURING_FARM_HORDE]             = uint32(0x00);
    WorldStateVars[AB_CAPTURED_FARM_HORDE]              = uint32(0x00);
    WorldStateVars[AB_CAPTURED_FARM_ALLIANCE]           = uint32(0x00);

    // Stables
    WorldStateVars[AB_CAPTURING_STABLES_HORDE]          = uint32(0x00);
    WorldStateVars[AB_CAPTURING_STABLES_ALLIANCE]       = uint32(0x00);
    WorldStateVars[AB_CAPTURED_STABLES_HORDE]           = uint32(0x00);
    WorldStateVars[AB_CAPTURED_STABLES_ALLIANCE]        = uint32(0x00);
}

void ArathiBasin::SpawnBuff(uint32 typeentry,uint32 bannerslot)
{
    switch(bannerslot)
    {
        case 0: // stables buff
            gbuffs[bannerslot] = SpawnGameObject(typeentry, 529, 1185.56616210938, 1184.62854003906, -56.3632850646973, 2.30383467674255, 0, 114, 1);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.913545489311218);
               gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_03,0.406736612319946);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_STATE, 1);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
               break;
           case 1: // Blacksmith Buff
            gbuffs[bannerslot] = SpawnGameObject(typeentry, 529, 990.113098144531, 1008.73028564453, -42.6032752990723, 0.820304811000824, 0, 114, 1);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.398749083280563);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_03,0.917060077190399);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_STATE, 1);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            break;
        case 2: // Farm Buff
            gbuffs[bannerslot] = SpawnGameObject(typeentry, 529, 816.906799, 842.339844, -56.538746, 3.272740, 0, 114, 1);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.913545489311218);
               gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_03,0.406736612319946);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_STATE, 1);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            break;
        case 3: // Lumber mill Buff
            gbuffs[bannerslot] = SpawnGameObject(typeentry, 529, 808.846252441406, 1185.41748046875, 11.9216051101685, -0.663225054740906, 0, 114, 1);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.325568109750748);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.945518612861633);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_STATE, 1);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            break;
        case 4: // Mine Buff
            gbuffs[bannerslot] = SpawnGameObject(typeentry, 529, 1147.09057617188, 816.836242675781, -98.3989562988281, -0.226892784237862, 0, 114, 1);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_02,0.113203197717667);
            gbuffs[bannerslot]->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.993571877479553);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_STATE, 1);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_TYPE_ID, 6);
            gbuffs[bannerslot]->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
            break;
    }
}

void ArathiBasin::SpawnBattleground()
{
    
    for(int i=0;i<5;i++)
        SpawnBuff(buffentrys[(int)rand()%3],i);
        
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
    
    // -- Resources Banner --
    
    //Stable Banner
    GameObject *pbanner = SpawnGameObject(180087, 529, 1166.779541, 1200.147583, -56.701763, -2.251474, 0, 35, 1);
    pbanner->SetFloatValue(GAMEOBJECT_ROTATION_02,0.9025853);
    pbanner->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.4305111);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 10);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    pbanner->bannerslot = 0;
    
    // Farm Banner
    pbanner = SpawnGameObject(180089, 529, 806.2484741, 874.2167358, -55.9936981, 0.8377580, 0, 35, 1);
    pbanner->SetFloatValue(GAMEOBJECT_ROTATION_02,0.4067366);
    pbanner->SetFloatValue(GAMEOBJECT_ROTATION_03,0.9135454);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 10);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    pbanner->bannerslot = 1;
    
    // Blacksmith Banner
    pbanner = SpawnGameObject(180088, 529, 977.0503540, 1046.5208740, -44.8276138, 0.5410520, 0, 35, 1);
    pbanner->SetFloatValue(GAMEOBJECT_ROTATION_02,0.2672384);
    pbanner->SetFloatValue(GAMEOBJECT_ROTATION_03,0.9636304);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 10);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    pbanner->bannerslot = 2;

    // Mine Banner
    pbanner = SpawnGameObject(180091, 529, 1146.9224854, 848.1899414, -110.9200210, 2.4260077, 0, 35, 1);
    pbanner->SetFloatValue(GAMEOBJECT_ROTATION_02,0.9366722);
    pbanner->SetFloatValue(GAMEOBJECT_ROTATION_03,0.3502073);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 10);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    pbanner->bannerslot = 3;
    
    // Lumber Mill Banner
    pbanner = SpawnGameObject(180090, 529, 856.141907, 1148.902100, 11.184692, -2.303835, 0, 35, 1);
    pbanner->SetFloatValue(GAMEOBJECT_ROTATION_02,0.9135455);
    pbanner->SetFloatValue(GAMEOBJECT_ROTATION_03,-0.4067366);
    pbanner->SetUInt32Value(GAMEOBJECT_STATE, 1);
    pbanner->SetUInt32Value(GAMEOBJECT_TYPE_ID, 10);
    pbanner->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 100);
    
    pbanner->bannerslot = 4;  
    
    bSpawned = true;
}

void ArathiBasin::Start()
{
    // Open all the doors

    for(std::set<GameObject*>::iterator itr = m_Gates.begin(); itr!=m_Gates.end(); ++itr)
    {
        (*itr)->SetUInt32Value(GAMEOBJECT_FLAGS, 33);
        (*itr)->SetUInt32Value(GAMEOBJECT_STATE, 0);
    }

    // Send message packet
    WorldPacket *data = BuildMessageChat(0x52, 0x27, "The Battle for Arathi Basin has begun!", 0);
    SendPacketToAll(data);
    delete data;
    
    WorldPacket pkt;
    pkt.Initialize(SMSG_PLAY_SOUND);
    pkt << uint32(SOUND_BATTLEGROUND_BEGIN); // Battleground Sound Begin
    SendPacketToAll(&pkt);
    
    sEventMgr.AddEvent(this, &ArathiBasin::EventUpdateResourcesAlliance, EVENT_BATTLEGROUND_UPDATE_RESOURCES_ALLIANCE, BASE_RESOURCES_UPDATE_TIME, 0);
    sEventMgr.AddEvent(this, &ArathiBasin::EventUpdateResourcesHorde, EVENT_BATTLEGROUND_UPDATE_RESOURCES_HORDE, BASE_RESOURCES_UPDATE_TIME, 0);
}

