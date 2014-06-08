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


initialiseSingleton( BattlegroundMgr );

BattlegroundMgr::BattlegroundMgr()
{
    m_Battlegrounds.clear();
}

BattlegroundMgr::~BattlegroundMgr()
{
    CleanupBattlegrounds();
}

uint32 ReverseByteOrder(uint32* uint)
{
    uint32 tmp = *uint;
    ((char*)&tmp)[3] = uint[0];
    ((char*)&tmp)[2] = uint[1];
    ((char*)&tmp)[1] = uint[2];
    ((char*)&tmp)[0] = uint[3];
    return tmp;
}

WorldPacket* BattlegroundMgr::BuildBattlegroundStatusPacket(uint32 Battleground, uint32 BattleGroundType, uint8 InstanceID, uint8 StatusID, uint32 Time, bool Faction)
{
    // Capt: this is not yet perfect, its possible to crash the client if the user is cheating. Anyways who cares if his client crashes
    if (StatusID == 0)
    {
        WorldPacket *data = new WorldPacket(SMSG_BATTLEFIELD_STATUS, 12);
        *data << uint64(0);
        *data << uint32(0);
        return data;
    }
    else if (StatusID != 0)
    {
        WorldPacket *data = new WorldPacket(SMSG_BATTLEFIELD_STATUS, 29);

        uint16 MapInfo = 0x1F90; // Unknown, real use / name is unknown
    
        *data << uint32(Battleground);       // multiple instances of battleground, crap Player instances, so if you send this like 0 1 2 you get 3 instances of battlegrounds
        if(BattleGroundType < 4 || BattleGroundType == 7)
        {
            *data << uint8(0);                   // unknown
            *data << uint8(2);                   // unknown but resulted in a change in the packets
        }
        else
        {
            *data << uint8(2);                   // unknown
            *data << uint8(0xA);                   // unknown but resulted in a change in the packets
        }
        *data << uint32(BattleGroundType);   // BATTLEGROUND TYPE
        *data << uint16(MapInfo);            // MapInfo
        *data << uint32(InstanceID);         // Instance ID
        *data << uint8(Faction);             // Race, 0 is alliance 1 is horde
        *data << uint32(StatusID);           // Status ID
        if (StatusID  == 1)
        {
            *data << uint32(Time);           // Avarage waiting time
            *data << uint32(0);              // im almost sure this is the elapsed time or some kind or other time...
        }
        else if (StatusID == 2)
        {
            *data << uint32(GetBattleGroundMapIdByType(BattleGroundType));                  // mapid
            *data << uint32(120000);             // you are to be removed from que in 2 minutes
        }
        else if (StatusID == 3)
        {
            *data << uint32(GetBattleGroundMapIdByType(BattleGroundType));         // mapid
            *data << uint32(0);               // const 0
            *data << uint32(0x0001362B);      // unk / time?
            *data << uint8(1);                // unk, maybe enable
        }
        else
        {
            sLog.outError("WTF HAXOR");
        }
        return data;
    }
    return NULL;
}

WorldPacket* BattlegroundMgr::BuildPlayerJoinedBattlegroundPacket(Player *plr)
{
    WorldPacket *data = new WorldPacket(SMSG_BATTLEGROUND_PLAYER_JOINED, 8);
    *data << plr->GetGUID();          // [player] Has joined the battle.    
    return data;
}

WorldPacket* BattlegroundMgr::BuildPlayerLeftBattlegroundPacket(Player *plr)
{
    WorldPacket *data = new WorldPacket(SMSG_BATTLEGROUND_PLAYER_LEFT, 8);
    *data << plr->GetGUID();          // [player] Has left the battle.    
    return data;
}

WorldPacket* BattlegroundMgr::BuildWorldStateUpdatePacket(uint16 field, uint16 value)
{
    WorldPacket *data = new WorldPacket(SMSG_UPDATE_WORLD_STATE, 8);
    *data << uint32(field) << uint32(value);
    return data;
}

uint32 BattlegroundMgr::CreateBattleground(uint32 MaxPlayersPerTeam, uint32 LevelMin, uint32 LevelMax, std::string BattlegroundName, uint32 MapID, float Team1StartLocX, float Team1StartLocY, float Team1StartLocZ, float Team1StartLocO, float Team2StartLocX, float Team2StartLocY, float Team2StartLocZ, float Team2StartLocO)
{
    // Create the BG
    Battleground *bg = NULL;
    
    switch(MapID)
    {
        case 489: bg = new WarsongGulch; break;
        case 529: bg = new ArathiBasin; break;
        case  30: bg = new AlteracValley; break; 
        case 562: bg = new Arena; break;
        default: bg = new Battleground; break;
    }       

    bg->SetMapId(MapID);
    
    bg->m_BattlegroundType = GetBattleGroundTypeByMapId(MapID);
    
    bg->SetMaxPlayersPerTeam(MaxPlayersPerTeam);
    bg->SetMaxPlayers(MaxPlayersPerTeam*2);
    bg->SetName(BattlegroundName);
    bg->SetTeamStartLoc(0, Team1StartLocX, Team1StartLocY, Team1StartLocZ, Team1StartLocO);
    bg->SetTeamStartLoc(1, Team2StartLocX, Team2StartLocY, Team2StartLocZ, Team2StartLocO);
    bg->SetLevelRange(LevelMin, LevelMax);
    
    uint32 BattlegroundID = m_Battlegrounds.size();     // this will be replaced with instance ID later.
    BattlegroundID++;                                   // We don't start at 0.. apparently..

    
    bg->SetID(BattlegroundID);
    
    bg->SetupBattleground();
    AddBattleground(BattlegroundID, bg);
    bg->CreateInstance();
    //sLog.outString("  Created new BG: %d (Map %d, Instance %d, %d p per team, Levels %d-%d)", BattlegroundID, bg->m_MapId, bg->GetInstanceID(), bg->m_MaxPlayersPerTeam, bg->m_LevelMin, bg->m_LevelMax);
    
    return BattlegroundID;
}

void BattlegroundMgr::RemoveBattleground(uint32 ID)
{
    std::map<uint32, Battleground*>::iterator i = m_Battlegrounds.find(ID);
    if(i != m_Battlegrounds.end())
    {
        switch(i->second->GetMapId())
        {
            case 489: delete ((WarsongGulch*)i->second); break;
            case 529: delete ((ArathiBasin*)i->second); break;
            case  30: delete ((AlteracValley*)i->second); break;
            case 562: delete ((Arena*)i->second); break;
        }
        m_Battlegrounds.erase(i);
    }
}

uint32 BattlegroundMgr::GenerateTeamByRace(uint8 Race)
{
    switch(Race)
    {
        case RACE_ORC:
        case RACE_UNDEAD:
        case RACE_TAUREN:
        case RACE_TROLL:
        case RACE_BLOODELF:
            {
                return 1;
            }break;
        case RACE_HUMAN:
        case RACE_DWARF:
        case RACE_NIGHTELF:
        case RACE_GNOME:
        case RACE_DRAENEI:
            {
                return 0;
            }break;
        default:
            {
                sLog.outError("BattlegroundMgr: Warning! Unable to determine race for %d", Race);
                return 0;
            }break;
    }
}

void BattlegroundMgr::CreateWSGBattleground()
{
    std::string bg_Name;
    uint32 MaxPlayersPerTeam;
    uint32 MapId;
    float AStartLoc[4];
    float HStartLoc[4];
    uint32 LevMin;
    uint32 LevMax;

    // Create BG, Warsong Gulch, Levels 1-60, 16 per team
    bg_Name = "Warsong Gulch";
    MapId = 489;
    MaxPlayersPerTeam = 16;
    AStartLoc[0] = 1519.530273f;
    AStartLoc[1] = 1481.868408f;
    AStartLoc[2] = 352.023743f;
    AStartLoc[3] = 3.141593f;
    
    HStartLoc[0] = 933.989685f;
    HStartLoc[1] = 1430.735840f;
    HStartLoc[2] = 345.537140f;
    HStartLoc[3] = 3.141593f;

    LevMin = 1;
    LevMax = PLAYER_LEVEL_CAP;
    //sLog.outString("Creating battleground %s, %d-%d", bg_Name.c_str(), LevMin, LevMax);
    CreateBattleground(MaxPlayersPerTeam, LevMin, LevMax, bg_Name, MapId, AStartLoc[0], AStartLoc[1], AStartLoc[2], AStartLoc[3], HStartLoc[0], HStartLoc[1], HStartLoc[2], HStartLoc[3]);
}

void BattlegroundMgr::CreateABBattleground()
{
    std::string bg_Name;
    uint32 MaxPlayersPerTeam;
    uint32 MapId;
    float AStartLoc[4];
    float HStartLoc[4];
    uint32 LevMin;
    uint32 LevMax;

    // Create BG, Arathi Basin, Levels 1-60, 16 per team
    bg_Name = "Arathi Basin";
    MapId = 529;
    MaxPlayersPerTeam = 16;
    HStartLoc[0] = 684.75629f;
    HStartLoc[1] = 681.945007f;
    HStartLoc[2] = -12.915456f;
    HStartLoc[3] = 0.881211f;
    
    AStartLoc[0] = 1314.932495f;
    AStartLoc[1] = 1311.246948f;
    AStartLoc[2] = -9.00952f;
    AStartLoc[3] = 3.802896f;

    LevMin = 1;
    LevMax = PLAYER_LEVEL_CAP;
    //sLog.outString("Creating battleground %s, %d-%d", bg_Name.c_str(), LevMin, LevMax);
    CreateBattleground(MaxPlayersPerTeam, LevMin, LevMax, bg_Name, MapId, AStartLoc[0], AStartLoc[1], AStartLoc[2], AStartLoc[3], HStartLoc[0], HStartLoc[1], HStartLoc[2], HStartLoc[3]);
    
}

void BattlegroundMgr::CreateAVBattleground()
{
    std::string bg_Name;
    uint32 MaxPlayersPerTeam;
    uint32 MapId;
    float AStartLoc[4];
    float HStartLoc[4];
    uint32 LevMin;
    uint32 LevMax;

    // Create BG, Alterac Valley, Levels 1-60, 16 per team
    bg_Name = "Alterac Valley";
    MapId = 30;
    MaxPlayersPerTeam = 16;
    HStartLoc[0] = -823.837280f;
    HStartLoc[1] = -619.942566f;
    HStartLoc[2] = 54.116886f;
    HStartLoc[3] = 2.210880f;
    
    AStartLoc[0] = 870.134399f;
    AStartLoc[1] = -497.399933f;
    AStartLoc[2] = 96.655867f;
    AStartLoc[3] = 3.089727f;

    LevMin = 50;
    LevMax = PLAYER_LEVEL_CAP;
    //sLog.outString("Creating battleground %s, %d-%d", bg_Name.c_str(), LevMin, LevMax);
    CreateBattleground(MaxPlayersPerTeam, LevMin, LevMax, bg_Name, MapId, AStartLoc[0], AStartLoc[1], AStartLoc[2], AStartLoc[3], HStartLoc[0], HStartLoc[1], HStartLoc[2], HStartLoc[3]);

}

void BattlegroundMgr::CreateArena()
{
    std::string bg_Name;
    uint32 MaxPlayersPerTeam;
    uint32 MapId;
    float AStartLoc[4];
    float HStartLoc[4];
    uint32 LevMin;
    uint32 LevMax;

    // Create BG, Alterac Valley, Levels 1-60, 10 per team
    bg_Name = "Arena";
    MapId = 562;
    MaxPlayersPerTeam = 10;
    HStartLoc[0] = 6292.6586f;
    HStartLoc[1] = 288.5794f;
    HStartLoc[2] = 4.95929f;
    HStartLoc[3] = 0;
    
    AStartLoc[0] = 6184.982;
    AStartLoc[1] = 236.0114;
    AStartLoc[2] = 4.97687;
    AStartLoc[3] = 0;

    LevMin = 1;
    LevMax = PLAYER_LEVEL_CAP;
    CreateBattleground(MaxPlayersPerTeam, LevMin, LevMax, bg_Name, MapId, AStartLoc[0], AStartLoc[1], AStartLoc[2], AStartLoc[3], HStartLoc[0], HStartLoc[1], HStartLoc[2], HStartLoc[3]);

}

void BattlegroundMgr::CreateInitialBattlegrounds()
{
    CreateWSGBattleground();
    //CreateWSGBattleground();
    CreateABBattleground();
	CreateArena();
    //CreateABBattleground();
    //CreateAVBattleground();
    //CreateAVBattleground();
}

WorldPacket* BattlegroundMgr::BuildBattlegroundListPacket(uint64 guid, Player* plr,uint32 BattlegroundType)
{
    uint32 PlayerLevel = 10;
    if(plr)
        PlayerLevel = plr->GetUInt32Value(UNIT_FIELD_LEVEL);

    // TODO: Lookup npc entry code and find mapid
    /*
    15006 = Deze Snowbane           = Arathi Basin      Battlemaster
    Kartra Bloodsnarl       = Alterac Valley    Battlemaster
    Brakgul Deathbringer    = Warsong Gulch     Battlemaster
    Glordrum Steelbeard     = Alterac Valley    Battlemaster
    Donal Osgood            = Arathi Basin      Battlemaster
    Kurden Bloodclaw        = Warsong Gulch     Battlemaster
    Sir Malory Wheeler      = Arathi Basin      Battlemaster
    Grizzle Halfmane        = Alterac Valley    Battlemaster
    Martin Lindsey          = Arathi Basin      Battlemaster
    Taim Ragetotem          = Alterac Valley    Battlemaster
    Kergul Bloodaxe         = Warsong Gulch     Battlemaster
    Aethalas                = Warsong Gulch     Battlemaster
    Keras Wolfheart         = Arathi Basin      Battlemaster
    Brogun Stoneshield      = Alterac Valley    Battlemaster
    Lylandris               = Warsong Gulch     Battlemaster
    7410 = Thelman Slatefist       = Alterac Valley    Battlemaster
    Lady Hoteshem           = Arathi Basin      Battlemaster
    Elfarran                = Warsong Gulch     Battlemaster
    Sir Maximus Adams       = Arathi Basin      Battlemaster
    Grumbol Grimhammer      = Alterac Valley    Battlemaster
    Usha Eyegouge           = Alterac Valley    Battlemaster
    Gargok                  = Warsong Gulch     Battlemaster
    The Black Bride         = Arathi Basin      Battlemaster
    Su'ura Swiftarrow       = Warsong Gulch     Battlemaster
    Zeggon Botsnap          = Arena             Battlemaster
    Beka Zipwhistle         = Arena             Battlemaster
    Fizim Blastwrench       = Arena             Battlemaster
    Max Xim                 = Arena             Battlemaster
    Max Luna                = Arena             Battlemaster
    Fima Five-Fingers       = Arena             Battlemaster
    Rex Pixem               = Arena             Battlemaster
    */

    uint32 size = 0;
    WorldPacket *data = new WorldPacket(SMSG_BATTLEFIELD_LIST, 15 + m_Battlegrounds.size()/2);
    
    *data << guid;               // player guid
    *data << BattlegroundType;   // Battleground type
    *data << (uint8)2;           // unknown
    *data << uint32(0);          // size

    for(std::map<uint32, Battleground*>::iterator itr=m_Battlegrounds.begin();itr!=m_Battlegrounds.end();++itr)
    {
        /*if(itr->second->GetBattlegroundType() == BattlegroundType && (PlayerLevel >= itr->second->GetMinLevel()) && (PlayerLevel <= itr->second->GetMaxLevel()))
        {*/
        if(itr->second->GetBattlegroundType() == BattlegroundType)
        {
            *data << (uint32)itr->second->GetInstanceID();
            size++;
        }
        /*}*/
    }
    *(uint32*)&data->contents()[13] = size;

    return data;
}

uint32 BattlegroundMgr::GeneratePlayerKillMoney(Player *pKiller, Player *pVictim)
{
    // bah lets make this up for now
    uint32 money = pVictim->getLevel();
    if(money <= 0) money=1;
    return money;
}

Battleground::Battleground()
{
    m_ID = 0;
    m_Name = "";
    m_LevelMin = 0;
    m_LevelMax = 0;
   
    m_TeamScores[0] = 0;
    m_TeamScores[1] = 0;
    m_PlayerScores.clear();

    m_Players.clear();
    m_QueuedPlayers.clear();
    m_MaxPlayersPerTeam = 0;
    m_MaxPlayers = 0;

    m_MapId = 0;
    m_TeamStartLocX[0] = 0;
    m_TeamStartLocX[1] = 0;
    
    m_TeamStartLocY[0] = 0;
    m_TeamStartLocY[1] = 0;
    
    m_TeamStartLocZ[0] = 0;
    m_TeamStartLocZ[1] = 0;
    
    m_TeamStartLocO[0] = 0;
    m_TeamStartLocO[1] = 0;

    m_BattlegroundType = 0;

    m_LastResurrect = 0;
    m_Active = false;
    m_start = false;
    m_HordeGroup = m_AllianceGroup = NULL;
    m_GameStatus = false;
    m_GameWin = false;
}

Battleground::~Battleground()
{
    WorldStateVars.clear();
    sEventMgr.RemoveEvents(this);
}

WorldPacket* Battleground::BuildclearWorldStates(uint32 mapid)
{    
    WorldStateVars.clear();
    
    // global world state's
    WorldStateVars[0x8D8] = uint32(0x00);
    WorldStateVars[0x8D7] = uint32(0x00);
    WorldStateVars[0x8D6] = uint32(0x00);
    WorldStateVars[0x8D5] = uint32(0x00);
    WorldStateVars[0x8D4] = uint32(0x00);
    WorldStateVars[0x8D3] = uint32(0x00);
    
    WorldPacket *data = new WorldPacket(SMSG_INIT_WORLD_STATES, 10 + WorldStateVars.size());
    *data << uint32(mapid);
    // note: very hacky way of handling unknown
    if (mapid == 0x0211)
        *data << uint32(0xD1E);
    else    
        *data << uint32(0x5EF);

    *data << uint16(WorldStateVars.size());
    for(std::map<uint32, uint32>::iterator itr=WorldStateVars.begin();itr!=WorldStateVars.end();++itr)
    {
        *data << uint32(itr->first) << uint32(itr->second);
    }
    return data;
}    

WorldPacket* Battleground::BuildPlayerPositionsPacket(Player *Source)
{
    /*WorldPacket *data = new WorldPacket(MSG_BATTLEGROUND_PLAYER_POSITIONS, 4 + (m_Players.size() * 12));
    *data << uint32(0);
    uint32 count = 0;
    for(std::list<Player*>::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if((*itr) == Source)
            continue;

        *data << (*itr)->GetGUID();
        *data << (*itr)->GetPositionX();
        *data << (*itr)->GetPositionY();
        ++count;
    }
    *(uint32*)&data->contents()[0] = count;
    return data;*/
    WorldPacket *data = new WorldPacket(MSG_BATTLEGROUND_PLAYER_POSITIONS,8);
    *data << uint64(0);

    /*
    {SERVER} Packet: (0x02E9) CMSG_WARDEN_DATA Size = 24
    |------------------------------------------------|----------------|
    |00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
    |------------------------------------------------|----------------|
    |00 00 00 00 01 00 00 00 81 C6 50 00 00 00 00 00 |..........P.....|
    |13 2A BF 44 3E 44 B9 44                         |.*.D>D.D        |
    -------------------------------------------------------------------

    00 00 00 00 
    01 00 00 00 = unknown
    81 C6 50 = guid
    00 00 00 00 00 = unknown
    13 2A BF 44 = 1529.31484624224 x
    3E 44 B9 44 = 1482.13256891138 y
    
    */


    return data;
}

WorldPacket* Battleground::BuildPVPLogDataPacket(Player *Source, uint8 BGGameStatus, bool BGGameWin)
{
    // I guess this time we include source?
    WorldPacket *data = new WorldPacket(MSG_PVP_LOG_DATA, 5 + m_PlayerScores.size() * 10);

    *data << uint8(0);
    *data << uint8(BGGameStatus);   

    if (BGGameStatus == 1)          // 1 is equal to end game
    {
        *data << uint8(BGGameWin);   // Winner of game 0 is horde 1 is alliance
        WorldPacket *pkt = sBattlegroundMgr.BuildBattlegroundStatusPacket(0, GetBattlegroundType(), m_ID, 0, 120, 0);
        Source->GetSession()->SendPacket(pkt);
        delete pkt;
    }

    *data << uint32(m_PlayerScores.size());

    for(std::map<uint64, BattlegroundScore>::iterator itr=m_PlayerScores.begin();itr!=m_PlayerScores.end();++itr)
    {
        *data << itr->first;
        //*data << uint32(PVPRANK_PRIVATE); // ranks, needs more decoding here
        *data << itr->second.KillingBlows;
        *data << itr->second.HonorableKills;
        *data << itr->second.Deaths;
        *data << itr->second.BonusHonor;
        *data << itr->second.DamageDone;
        *data << itr->second.HealingDone;
        *data << uint32(0x02); // unk
        *data << itr->second.FlagCaptures;
        *data << itr->second.FlagReturns;
        *data << uint32(0);
    }
    //data->hexlike();
    return data;
}

void Battleground::SetTeamStartLoc(uint32 TeamID, float X, float Y, float Z, float O)
{
    m_TeamStartLocX[TeamID] = X;
    m_TeamStartLocY[TeamID] = Y;
    m_TeamStartLocZ[TeamID] = Z;
    m_TeamStartLocO[TeamID] = O;
}

void Battleground::SendPacketToAll(WorldPacket *packet)
{
    for(std::list<Player*>::iterator itr=m_Players.begin();itr!=m_Players.end();++itr)
    {
        if((*itr)->GetSession())
            (*itr)->GetSession()->SendPacket(packet);
    }
}

void Battleground::SendPacketToTeam(uint32 TeamID, WorldPacket *packet)
{
    for(std::list<Player*>::iterator itr=m_Players.begin();itr!=m_Players.end();++itr)
    {
        if((*itr)->GetSession() /* && (*itr)->m_BattlegroundTeam == TeamID */)
            (*itr)->GetSession()->SendPacket(packet);
    }
}

void Battleground::RemovePlayer(Player *plr, bool Transport, bool SendPacket, bool BeforeEnd)
{
    std::map<uint64, BattlegroundScore>::iterator itr = m_PlayerScores.find(plr->GetGUID());
    // Remove from lists/maps
    if(itr != m_PlayerScores.end())
        m_PlayerScores.erase(itr);
    plr->m_bgScore = 0;

    bool Removed = false;

    for(std::list<Player*>::iterator itr2=m_Players.begin();itr2!=m_Players.end();++itr2)
    {
        if((*itr2) == plr)
        {
            m_Players.erase(itr2);
            Removed = true;
            break;
        }
    }

    if(!Removed)
    {
        for(std::list<Player*>::iterator itr3 = m_QueuedPlayers.begin();itr3!=m_QueuedPlayers.end();++itr3)
        {
            if((*itr3) == plr)
            {
                m_QueuedPlayers.erase(itr3);
                Removed = true;
                break;
            }
        }
    }

    if(!Removed) sLog.outError("BATTLEGROUND: Player could not be removed from battleground completely!");

    if(m_MapId == 489 && plr->m_bgHasFlag)
        ((WarsongGulch*)this)->HandleBattlegroundEvent(plr, NULL, BGEVENT_WSG_PLAYER_DIED_WITH_FLAG);

    // Let others know
    WorldPacket *pkt = sBattlegroundMgr.BuildPlayerLeftBattlegroundPacket(plr);
    SendPacketToAll(pkt);
    delete pkt;

    // Log
    sLog.outDetail("BATTLEGROUND: Player %s left the battle.", plr->GetName());

    // We're not in BG.
    plr->m_bgBattlegroundID = 0;
    plr->m_bgLastBattlegroundID = 0;
    plr->m_bgInBattleground = false;
    plr->m_bgTeam = 0;

    // Packets/Movement
    
    if(Transport)
    {
        // Needs vars added to player class and I'm too lazy to rebuild..
        plr->SetInstanceID(plr->m_bgEntryPointInstance);
        plr->_Relocate(plr->m_bgEntryPointMap, LocationVector(plr->m_bgEntryPointX,
            plr->m_bgEntryPointY, plr->m_bgEntryPointZ, plr->m_bgEntryPointO), true, true);

        WorldPacket *pkt = BuildclearWorldStates(plr->GetMapId());
        plr->GetSession()->SendPacket(pkt);
        delete pkt;
    }

    if(SendPacket)
    {
        WorldPacket *pkt = sBattlegroundMgr.BuildBattlegroundStatusPacket(0, GetBattlegroundType(), m_ID, 0, 0, 0);
        plr->GetSession()->SendPacket(pkt);
        delete pkt;
    }

    // Log
    sLog.outDetail("BATTLEGROUND: Removed %s from Battleground.", plr->GetName());

    // Add deserter aura
    if(BeforeEnd)
    {
        Aura *aur = NULL;
        SpellEntry *info = sSpellStore.LookupEntry(26013);
        if(info)
        {
            aur = new Aura(info, 1200000, plr,plr);
            plr->AddAura(aur);
        }
    }
    
    uint32 TeamCounts[2];
    TeamCounts[0] = 0;
    TeamCounts[1] = 0;
    for(std::list<Player*>::iterator i=m_Players.begin();i!=m_Players.end();++i)
    {
        TeamCounts[(*i)->m_bgTeam]++;
    }
    
    if(TeamCounts[0] == 0 || TeamCounts[1] == 0)
    {
        m_start = false;
        sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_ALMOST_START);
        sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_START);
    }

    OnPlayerLeft(plr);

    /*if(m_Players.size() == 0)
    {
        sEventMgr.RemoveEvents(this,EVENT_BATTLEGROUND_REVIVE_PLAYERS);
    }*/
}

void Battleground::RemoveAllPlayers(bool Transport, bool SendPacket)
{
      
    m_PlayerScores.clear();
    m_QueuedPlayers.clear();

    Player* plr = NULL;

    for(std::list<Player*>::iterator itr=m_Players.begin();itr!=m_Players.end();++itr)
    {        
        plr = *itr;
        
        if(m_MapId == 489 && plr->m_bgHasFlag)
            ((WarsongGulch*)this)->HandleBattlegroundEvent(plr, NULL, BGEVENT_WSG_PLAYER_DIED_WITH_FLAG);
                   
        // Let others know
        WorldPacket *pkt = sBattlegroundMgr.BuildPlayerLeftBattlegroundPacket(plr);
        SendPacketToAll(pkt);    
        delete pkt;
            
        // Log
        sLog.outDetail("BATTLEGROUND: Player %s left the battle.", plr->GetName());
    
        // We're not in BG.
        plr->m_bgBattlegroundID = 0;
        plr->m_bgLastBattlegroundID = 0;
        plr->m_bgInBattleground = false;
        plr->m_bgTeam = 0;
        if(plr->m_bgEntryPointMap == 0 ||
            plr->m_bgEntryPointMap == 1)
        {
            plr->SetInstanceID(-1); // restoring back to original map
        }
    
        // Packets/Movement

        if(Transport)
        {
            plr->_Relocate(plr->m_bgEntryPointMap, LocationVector(plr->m_bgEntryPointX,
            plr->m_bgEntryPointY, plr->m_bgEntryPointZ, plr->m_bgEntryPointO), true, true);
            WorldPacket *pkt = BuildclearWorldStates(plr->GetMapId());
            plr->GetSession()->SendPacket(pkt);
            delete pkt;
        }
    
        if(SendPacket)
        {
            WorldPacket *pkt = sBattlegroundMgr.BuildBattlegroundStatusPacket(0, GetBattlegroundType(), m_ID, 0, 0, 0);
            plr->GetSession()->SendPacket(pkt);
            delete pkt;
        }
    
        // Log
        sLog.outDetail("BATTLEGROUND: Removed %s from Battleground.", plr->GetName());
    
        OnPlayerLeft(plr);
    }
    
    m_Players.clear();

    m_Active = false;
    m_start = false;
}

void Battleground::AddPlayer(Player *plr, bool Transport, bool SendPacket)
{
    // We're in BG.
    plr->m_bgLastBattlegroundID = plr->m_bgBattlegroundID = m_ID;
    plr->m_bgInBattleground = true;
    
    // Calculate team
    plr->m_bgTeam = sBattlegroundMgr.GenerateTeamByRace(plr->getRace());

    // Packets/Movement
    if(SendPacket)
    {
        if(plr->GetSession())
        {
            WorldPacket *pkt = sBattlegroundMgr.BuildBattlegroundStatusPacket(0, GetBattlegroundType(), GetInstanceID(), 3, 0x00FFFF00, 0);
            plr->GetSession()->SendPacket(pkt);
            delete pkt;
        }
    }

    if(Transport)
    {
        plr->m_bgEntryPointMap = plr->GetMapId();
        plr->m_bgEntryPointO = plr->GetOrientation();
        plr->m_bgEntryPointX = plr->GetPositionX();
        plr->m_bgEntryPointY = plr->GetPositionY();
        plr->m_bgEntryPointZ = plr->GetPositionZ();
        plr->m_bgEntryPointInstance = plr->GetInstanceID();
        if(plr->GetSession())
        {
            // TODO: Find team based on faction
            uint32 TeamID = sBattlegroundMgr.GenerateTeamByRace(plr->getRace());

            plr->SetInstanceID(GetInstanceID());
            plr->_Relocate(m_MapId, LocationVector(m_TeamStartLocX[TeamID], m_TeamStartLocY[TeamID],
                m_TeamStartLocZ[TeamID],  m_TeamStartLocO[TeamID]), true, true);
        }
    }

    WorldPacket * data = sBattlegroundMgr.BuildBattlegroundStatusPacket(0, GetBattlegroundType(),GetInstanceID(),3,0x28134C00,0);
    plr->GetSession()->SendPacket(data);
    delete data;

    // Log
    sLog.outDetail("BATTLEGROUND: Added %s to Battleground.", plr->GetName());

    // Create score struct
    BattlegroundScore sc;
    
    sc.BonusHonor = 0;
    sc.Deaths = 0;
    sc.HonorableKills = 0;
    sc.KillingBlows = 0;
    sc.FlagCaptures = 0;
    sc.FlagReturns = 0;
    sc.DamageDone = 0;
    sc.HealingDone = 0;
    sc.teamid = plr->m_bgTeam;
    // Add to list/maps
    m_Players.push_back(plr);
    uint64 guid = plr->GetGUID();

    m_PlayerScores[guid] = sc;
    plr->m_bgScore = &(m_PlayerScores[guid]);

    // Let the others know
    WorldPacket *pkt = sBattlegroundMgr.BuildPlayerJoinedBattlegroundPacket(plr);
    SendPacketToAll(pkt);
    delete pkt;

    // Log
    sLog.outDetail("BATTLEGROUND: Player %s joined the battle.", plr->GetName());

    // Bur: Not sure if we would have to send a position/score update.. maybe the client requests this automatically we'll have to see
    // WS Update: Show up scoreboard
    pkt = BuildWorldStateInit();
    plr->GetSession()->SendPacket(pkt);
    delete pkt;

    if(m_Players.size() > 0 && !m_Active)
    {
        m_Active = true;
        sEventMgr.AddEvent(this, &Battleground::EventResurrectPlayers, EVENT_BATTLEGROUND_REVIVE_PLAYERS, 30000, 0);
        EventResurrectPlayers();
    }
    if(!bSpawned && GetMapId() == 489)
        ((WarsongGulch*)this)->SpawnBattleground();
    else if(!bSpawned && GetMapId() == 529)
        ((ArathiBasin*)this)->SpawnBattleground();
    else if(!bSpawned && GetMapId() == 30)
        ((AlteracValley*)this)->SpawnBattleground();
    
    if(!m_start)
    {    
        uint32 TeamCounts[2];
        TeamCounts[0] = 0;
        TeamCounts[1] = 0;
        for(std::list<Player*>::iterator i=m_Players.begin();i!=m_Players.end();++i)
        {
            TeamCounts[(*i)->m_bgTeam]++;
            if(TeamCounts[0] > 0 && TeamCounts[1] > 0)
            {
                m_start = true;
                WorldPacket *data = NULL;
                if(m_MapId == 489)
                    data = BuildMessageChat(0x52, 0x29, "The Battle for Warsong Gulch will begin in 1 minute.", 0);
                else if(m_MapId == 529)
                    data = BuildMessageChat(0x52, 0x29, "The Battle for Arathi Basin will begin in 1 minute.", 0);    
                else if(m_MapId == 30)
                    data = BuildMessageChat(0x52, 0x29, "The Battle for Alterac Valley will begin in 1 minute.", 0);
                    
                SendPacketToAll(data);
                delete data;
                
                sEventMgr.AddEvent(this, &Battleground::EventAlmostStart, EVENT_BATTLEGROUND_ALMOST_START, 30000, 1);
                sEventMgr.AddEvent(this, &Battleground::EventStart, EVENT_BATTLEGROUND_START, 60000, 1);
                break;
            }        
        }
    }

    OnPlayerJoined(plr);
}

bool Battleground::HasFreeSlots(uint32 Team)
{
    uint32 TeamCounts[2];
    TeamCounts[0] = 0;
    TeamCounts[1] = 0;
    for(std::list<Player*>::iterator i=m_Players.begin();i!=m_Players.end();++i)
        TeamCounts[(*i)->m_bgTeam]++;

    if(TeamCounts[Team] < m_MaxPlayersPerTeam)
        return true;
    else
        return false;
}

void Battleground::SetWorldStateValue(uint32 field, uint32 value)
{
    WorldStateVars[field] = value;
    WorldPacket data(SMSG_UPDATE_WORLD_STATE, 8);
    data << uint32(field) << uint32(value);
    SendPacketToAll(&data);
}

WorldPacket* Battleground::BuildWorldStateInit()
{
    uint32 bflag = 0;
    
    switch(m_MapId)
    {
       case  489: bflag = 0x0CCD; break;
       case  529: bflag = 0x0D1E; break;
       case   30: bflag = 0x0A25; break;
       case  562: bflag = 0x0E76; break;
    }
  
    WorldPacket *data = new WorldPacket(SMSG_INIT_WORLD_STATES, 10 + (WorldStateVars.size() * 8));
    *data << uint32(m_MapId) << uint32(bflag) <<uint16(WorldStateVars.size());
    for(std::map<uint32, uint32>::iterator itr=WorldStateVars.begin();itr!=WorldStateVars.end();++itr)
    {
        *data << uint32(itr->first) << uint32(itr->second);
    }
    return data;
}

WorldPacket* Battleground::BuildMessageChat(uint32 type, uint32 p2, const char* msg, uint8 p3, uint64 guid)
{
    uint32 messageLength = strlen((char*)msg) + 1;
    
    WorldPacket *data = new WorldPacket;
    data->Initialize(SMSG_MESSAGECHAT);
    *data << uint8(type) << uint32(0) << uint64(guid) << messageLength;
    *data << msg;
    *data << uint8(p3);
    return data;
}

void Battleground::EventResurrectPlayers()
{
#define RESURRECT_SPELL 21074   // Spirit Healer Res
    for(std::map<uint64, Player*>::iterator itr = m_ReviveQueue.begin();itr!=m_ReviveQueue.end();++itr)
    {
        Player *SelectedPlayer = itr->second;
        if(SelectedPlayer)
        {
            if(SelectedPlayer->isDead())
            {
                Unit *sh = sObjHolder.GetObject<Creature>(itr->first);
                if(sh)
                {
                    WorldPacket data(SMSG_SPELL_START, 44);
                    uint32 spellId = RESURRECT_SPELL;

                    data << sh->GetNewGUID();
                    data << sh->GetNewGUID();
                    data << spellId;
                    data << uint16(0);
                    data << uint32(0);
                    data << uint16(2);
                    data << SelectedPlayer->GetGUID();
                    //        WPAssert(data.size() == 36);
                    SelectedPlayer->SendMessageToSet( &data ,true);

                    data.Initialize( SMSG_SPELL_GO );
                    data << sh->GetNewGUID();
                    data << sh->GetNewGUID();
                    data << spellId;
                    data << uint8(0) << uint8(1);       // flags
                    data << uint8(1);                   // amount of targets
                    data << SelectedPlayer->GetGUID();  // target
                    data << uint8(0);
                    data << uint16(2);
                    data << SelectedPlayer->GetGUID();
                    //        WPAssert(data.size() == 42);
                    SelectedPlayer->SendMessageToSet( &data ,true);
                }
                SelectedPlayer->ResurrectPlayer();
                SelectedPlayer->SetUInt32Value(UNIT_FIELD_HEALTH, SelectedPlayer->GetUInt32Value(UNIT_FIELD_MAXHEALTH));                
            }
        }
    }

    // Empty queue
    m_ReviveQueue.clear();
    m_LastResurrect = uint32(time(NULL));
}

void Battleground::SetupBattleground()
{
    sLog.outString("WARNING: Virtual SetupBattleground()!!");
}

void Battleground::HandleBattlegroundEvent(Object *src, Object *dst, uint16 EventID, uint32 param1, uint32 param2)
{
    sLog.outString("WARNING: Virtual HandleBattlegroundEvent!!!");
}

void Battleground::HandleBanner(Player *p_caster,GameObject *go,uint32 spellid)
{
    sLog.outString("WARNING: Virtual HandleBanner!!!");
}

void Battleground::HandleBattlegroundAreaTrigger(Player *plr, uint32 TriggerID)
{
    sLog.outString("WARNING: Virtual HandleBattlegroundAreaTrigger!!!");
}

void Battleground::SendMessageToAll(std::string msg, bool widetoo)
{
    if(widetoo)
    {
        WorldPacket data(SMSG_AREA_TRIGGER_MESSAGE, 6+msg.size());
        data << uint32(0);
        data << msg.c_str();
        data << uint8(0);
        SendPacketToAll(&data);
    }   

    WorldPacket * data = sChatHandler.FillSystemMessageData(msg.c_str());
    SendPacketToAll(data);
    delete data;
}

void Battleground::SendMessageToPlayer(Player *plr, std::string msg, bool widetoo)
{
    if(widetoo)
    {
        WorldPacket data(SMSG_AREA_TRIGGER_MESSAGE, 6+msg.size());
        data << uint32(0);
        data << msg.c_str();
        data << uint8(0);
        plr->GetSession()->SendPacket(&data);
    }   

    WorldPacket * data = sChatHandler.FillSystemMessageData(msg.c_str());
    plr->GetSession()->SendPacket(data);
    delete data;
}


void Battleground::SendCMessageToAll(const char* szmsg, bool widetoo)
{
    string msg = szmsg;
    if(widetoo)
    {
        WorldPacket data(SMSG_AREA_TRIGGER_MESSAGE, 6+msg.size());
        data << uint32(0);
        data << msg.c_str();
        data << uint8(0);
        SendPacketToAll(&data);
    }   

    WorldPacket * data = sChatHandler.FillSystemMessageData(msg.c_str());
    SendPacketToAll(data);
    delete data;
}

void Battleground::SendCMessageToPlayer(Player *plr, const char* szmsg, bool widetoo)
{
    string msg = szmsg;
    if(widetoo)
    {
        WorldPacket data(SMSG_AREA_TRIGGER_MESSAGE, 6+msg.size());
        data << uint32(0);
        data << msg.c_str();
        data << uint8(0);
        plr->GetSession()->SendPacket(&data);
    }   

    WorldPacket * data = sChatHandler.FillSystemMessageData(msg.c_str());
    plr->GetSession()->SendPacket(data);
    delete data;    
}

void Battleground::UpdatePVPData()
{
    std::list<Player*>::iterator itr = m_Players.begin();
    for(;itr!=m_Players.end();++itr)
    {
        WorldPacket *data = BuildPVPLogDataPacket((*itr),GetBattleGroundStatus(), GetBattleGameWin());
        (*itr)->GetSession()->SendPacket(data);
        delete data;
    }
    
    if(GetBattleGroundStatus() && m_Active) // The game ended lets call ending fuction
    {
        m_Active = false;
        End();
    }
    
    sLog.outDebug("Battleground: Updated players PVP log data.");
}

void Battleground::EventRemove()
{
    sEventMgr.RemoveEvents(this,EVENT_BATTLEGROUND_REVIVE_PLAYERS);
    
    switch(m_MapId)
    {
        case 489:
        {
            ((WarsongGulch*)this)->Remove();
            //sBattlegroundMgr.CreateWSGBattleground();
            // remove the instance
            sEventMgr.AddEvent(BattlegroundMgr::getSingletonPtr(), &BattlegroundMgr::EventCreateBattleground,
            (uint32)489, EVENT_BATTLEGROUND_REMOVE, 30000, 1);
        }break;
        case 529:
        {
            ((ArathiBasin*)this)->Remove();
            //sBattlegroundMgr.CreateABBattleground();
      
            for(int i=0;i<5;i++)
            sEventMgr.RemoveEvents((ArathiBasin*)this,EVENT_BATTLEGROUND_STABLE_RCONTEST+i);
                
            sEventMgr.RemoveEvents((ArathiBasin*)this, EVENT_BATTLEGROUND_UPDATE_RESOURCES_ALLIANCE);
            sEventMgr.RemoveEvents((ArathiBasin*)this, EVENT_BATTLEGROUND_UPDATE_RESOURCES_HORDE);
            sEventMgr.AddEvent(BattlegroundMgr::getSingletonPtr(), &BattlegroundMgr::EventCreateBattleground,
            (uint32)489, EVENT_BATTLEGROUND_REMOVE, 30000, 1);
        }break;
        case 30:
        {
            ((AlteracValley*)this)->Remove();
        }break;
        case 562:
        {
            ((Arena*)this)->Remove();
        }break;   
    }   
    
    RemoveAllPlayers(true,true);    
    
    // remove the instance
    sWorldCreator.DestroyBattlegroundInstance(this);

    sBattlegroundMgr.RemoveBattleground(m_ID);
}

void Battleground::End()
{
    // lets root all players :p
    std::list<Player*>::iterator itr = m_Players.begin();
    for(; itr != m_Players.end(); ++itr)
    {
        (*itr)->SetMovement(MOVE_ROOT, 1);
    }

    uint32 teamspell[2] = {0,0};
    uint32 teamwin = GetBattleGameWin() ? 0 : 1;
    
    switch(m_MapId)
    {
        case 489:
            teamspell[teamwin] = 24951; // Create Warsong Mark of Honor (WInner)
            teamspell[teamwin ? 0 : 1] = 24950; // Create Warsong Mark of Honor (Loser)
            break;
        case 529:
            teamspell[teamwin] = 24953; // Arathi Basin Mark of Honor (Winner)
            teamspell[teamwin ? 0 : 1] = 24952; // Arathi Basin Mark of Honor (Loser)
            break;
        case 30:
            teamspell[teamwin] = 24955; // Alterac Valley Mark of Honor (Winner)
            teamspell[teamwin ? 0 : 1] = 24954; // Alterac Valley Mark of Honor (Loser)
            break;
    }
    
    if(teamspell[0] && teamspell[1])
    {
        SpellEntry *winproto = sSpellStore.LookupEntry(teamspell[teamwin]);
        SpellEntry *looseproto = sSpellStore.LookupEntry(teamspell[teamwin ? 0 : 1]);
        SpellEntry *endproto = sSpellStore.LookupEntry(26015);// spell  - Ended Battleground
        
        Spell *sp = NULL;
        
        if(winproto && looseproto)
        {
            for(std::list<Player*>::iterator i=m_Players.begin();i!=m_Players.end();++i)
            {
                if((*i)->m_bgTeam == teamwin)
                    sp = new Spell(*i, winproto, true, NULL);
                else
                    sp = new Spell(*i, looseproto, true, NULL);    
                    
                SpellCastTargets targets;
                targets.m_unitTarget = (*i)->GetGUID();
                sp->prepare(&targets);
                
                
                sp = new Spell(*i, endproto, true, NULL);
                sp->prepare(&targets);
            }
        }
    }

    sEventMgr.AddEvent(this, &Battleground::EventRemove, EVENT_BATTLEGROUND_REMOVE, 120000,1); // remove after 2mins
}

void Battleground::EventAlmostStart()
{
    WorldPacket *data = NULL;
    
    switch(m_MapId)
    {
        case 489:
            data = BuildMessageChat(0x52, 0x29, "The Battle for Warsong Gulch will begin in 30 seconds.", 0); break;
        case 529:
            data = BuildMessageChat(0x52, 0x29, "The Battle for Arathi Basin will begin in 30 seconds.", 0); break;
        case 30:    
            data = BuildMessageChat(0x52, 0x29, "The Battle for Alterac Valley will begin in 30 seconds.", 0); break;
        case 526:
            data = BuildMessageChat(0x52, 0x29, "The Arena Battle will begin in 30 seconds.", 0); break;
    }

    SendPacketToAll(data);
    delete data;
}

void Battleground::EventStart()
{
    switch(m_MapId)
    {
        case 489: ((WarsongGulch*)this)->Start(); break;
        case 529: ((ArathiBasin*)this)->Start(); break;
        case 30:  ((AlteracValley*)this)->Start(); break;
        case 562: ((Arena*)this)->Start(); break;
    }       
}


void Battleground::Start()
{
  // virtual fuction
}

void Battleground::SpawnBattleground()
{
    // virtual fuction
}

void Battleground::SpawnBuff(uint32 typeentry,uint32 bannerslot)
{
    // virtual fuction
}

GameObject* Battleground::SpawnGameObject(uint32 entry,uint32 MapId , float x, float y, float z, float o, uint32 flags, uint32 faction, float scale)
{
    GameObject *go = sObjHolder.Create<GameObject>();


    go->CreateFromProto(entry, MapId, x, y, z, o);
    
    go->SetUInt32Value(GAMEOBJECT_FACTION,faction);
    go->SetFloatValue(OBJECT_FIELD_SCALE_X,scale);    
    go->SetUInt32Value(GAMEOBJECT_FLAGS, flags);
    go->SetInstanceID(GetInstanceID());
    //void Create ( uint32 display_id, uint8 state, uint32 obj_field_entry, float scale, uint16 type, uint16 faction, uint32 mapid, float x, float y, float z, float ang );

    if(!go->IsInWorld())
        go->AddToWorld();
    
    return go;
}

void Battleground::OnPlayerJoined(Player* pPl)
{
    // check for raid group
    /*Group** grp;
    Group* raid;
    
    if (pPl->InGroup())
    {
        Group *group = pPl->GetGroup();
        if(group)
            group->RemovePlayer(pPl);
    }
        
    if(pPl->GetTeam() == 0)
        grp = &m_AllianceGroup;
    else
        grp = &m_HordeGroup;
    if(!*grp)
    {
        *grp = new Group();
        raid = *grp;
    } else {
        raid = *grp;
    }

    if(raid->GetSubGroupCount() != 8)       // expand to raid if not raid
        raid->ExpandToRaid();

    raid->AddMember(pPl);*/
}

void Battleground::OnPlayerLeft(Player* pPl)
{
    /*Group** grp;
    Group* raid;
    
    if (!pPl->InGroup())
        return;

    if(pPl->GetTeam() == 0)
        grp = &m_AllianceGroup;
    else
        grp = &m_HordeGroup;
    
    if(!*grp)
    {
        return;
    } else {
        raid = *grp;
    }

    if(raid->MemberCount() <= 2)
    {
        // this will cause raid to get destroyed
        raid->RemovePlayer(pPl);
        *grp = NULL;
    } else {
        raid->RemovePlayer(pPl);
    }*/
}

void BattlegroundMgr::CleanupBattlegrounds()
{
    sLog.outString("  Shutting Down Battlegrounds...");
    int count = m_Battlegrounds.size();
    std::map<uint32, Battleground*>::iterator it2;
    for(std::map<uint32, Battleground*>::iterator itr = m_Battlegrounds.begin(); itr != m_Battlegrounds.end();)
    {
        Battleground *bg = itr->second;
        it2 = itr;
        ++itr;
        switch(bg->GetMapId())
        {
        case 489:   //WSG
            sLog.outString("    %u - Warsong Gulch", bg->GetID());
            delete ((WarsongGulch*)bg);
            break;
        case 529:   //AB
            sLog.outString("    %u - Arathi Basin", bg->GetID());
            delete ((ArathiBasin*)bg);
            break;
        case 30:   //AV
            sLog.outString("    %u - Alterac Valley", bg->GetID());
            delete ((AlteracValley*)bg);
            break;
        case 562: // Arena
            sLog.outString("    %u - Arena", bg->GetID());
            delete ((Arena*)bg);
            break;        
        default:
            sLog.outString("    %u - Unknown", bg->GetID());
            delete bg;
            break;
        }
    }
    sLog.outString("  %u battlegrounds shutdown.\n",count);
}

void Battleground::CreateInstance()
{
    sWorldCreator.CreateBattlegroundInstance(this);
}

void BattlegroundMgr::EventCreateBattleground(uint32 mapid)
{
    switch(mapid)
    {
    case 489:
        CreateWSGBattleground();
        break;
    case 529:
        CreateABBattleground();
        break;
    case 30:
        CreateAVBattleground();
        break;
    case 562:
        CreateArena();
        break;
    default:
        ASSERT(false);
        break;
    }
}

Battleground* BattlegroundMgr::GetBattlegroundByInstanceID(uint32 ID,uint32 battlegroundtype)
{
    std::map<uint32, Battleground*>::iterator i = m_Battlegrounds.begin();
    for(; i != m_Battlegrounds.end(); ++i)
    {
        if((i->second->GetInstanceID() == ID || ID==0) && i->second->GetBattlegroundType() == battlegroundtype)
            return i->second;
    }
    return NULL;
}

void Battleground::SendSoundToBattleground(uint32 sound_id)
{
    WorldPacket data(SMSG_PLAY_SOUND, 4);
    data << sound_id;
    SendPacketToAll(&data);
}

void Battleground::SpawnSpiritGuide(float x, float y, float z, float o, uint32 horde)
{
    if(horde > 1)
        horde = 1;

    CreatureInfo * pInfo = objmgr.GetCreatureName(13116 + horde);
    if(pInfo == 0)
    {
        return;
    }

    Creature * pCreature = sObjHolder.Create<Creature>();
    if(pCreature == 0)
        return;

    pCreature->Create(pInfo->Name.c_str(), GetMapId(), x, y, z, o);

    pCreature->SetInstanceID(GetInstanceID());
    pCreature->SetUInt32Value(OBJECT_FIELD_ENTRY, 13116 + horde);
    pCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);

    pCreature->SetUInt32Value(UNIT_FIELD_HEALTH, 100);
    pCreature->SetUInt32Value(UNIT_FIELD_POWER1, 4868);
    pCreature->SetUInt32Value(UNIT_FIELD_POWER3, 200);
    pCreature->SetUInt32Value(UNIT_FIELD_POWER5, 2000000);

    pCreature->SetUInt32Value(UNIT_FIELD_HEALTH, 100);
    pCreature->SetUInt32Value(UNIT_FIELD_MAXPOWER1, 4868);
    pCreature->SetUInt32Value(UNIT_FIELD_MAXPOWER3, 200);
    pCreature->SetUInt32Value(UNIT_FIELD_MAXPOWER5, 2000000);

    pCreature->SetUInt32Value(UNIT_FIELD_LEVEL, 60);
    pCreature->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, 84 - horde);
    pCreature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0 | (2 << 8) | (1 << 16));

    pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 22802);
    pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, 2 | (0xA << 8) | (2 << 16) | (0x11 << 24));
    pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO_01, 2);

    pCreature->SetUInt32Value(UNIT_FIELD_FLAGS, 4928);

    pCreature->SetUInt32Value(UNIT_FIELD_AURA, 22011);
    pCreature->SetUInt32Value(UNIT_FIELD_AURAFLAGS, 9);
    pCreature->SetUInt32Value(UNIT_FIELD_AURALEVELS, 0x3C);
    pCreature->SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS, 0xFF);

    pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 2000);
    pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_01, 2000);
    pCreature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.208f);
    pCreature->SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);

    pCreature->SetUInt32Value(UNIT_FIELD_DISPLAYID, 13337 + horde);
    pCreature->SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, 13337 + horde);

    pCreature->SetUInt32Value(UNIT_CHANNEL_SPELL, 22011);
    pCreature->SetUInt32Value(UNIT_MOD_CAST_SPEED, 1065353216);

    pCreature->SetUInt32Value(UNIT_NPC_FLAGS, 64);
    pCreature->SetUInt32Value(UNIT_FIELD_BYTES_2, 1 | (0x10 << 8));

    pCreature->DisableAI();
    pCreature->AddToWorld();
}
