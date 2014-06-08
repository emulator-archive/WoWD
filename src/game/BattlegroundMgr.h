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

#ifndef __BATTLEGROUNDMGR_H
#define __BATTLEGROUNDMGR_H

class BattlegroundMgr;
class Battleground;
class MapMgr;
class Player;
class Map;
class Group;

struct BattlegroundScore
{
    uint32 teamid;
    uint32 KillingBlows;
    uint32 HonorableKills;
    uint32 Deaths;
    uint32 DamageDone;
    uint32 HealingDone;
    uint32 FlagCaptures;
    uint32 FlagReturns;
    uint32 BonusHonor;
};

enum BattleGroundTypes
{
    BATTLEGROUND_ALTERAC_VALLEY = 1,
    BATTLEGROUND_WARSUNG_GULCH,
    BATTLEGROUND_ARATHI_BASIN,
    BATTLEGROUND_NAGRAND_ARENA,
    BATTLEGROUND_BLADES_EDGE_ARENA,
    BATTLEGROUND_ALL_ARENAS,
    BATTLEGROUND_EYE_OF_THE_STORM,
};

enum BattleGround_Events
{
    BGEVENT_WSG_NULL,
    BGEVENT_WSG_PICKUP_FLAG,
    BGEVENT_WSG_DROP_FLAG,
    BGEVENT_WSG_PLAYER_DEATH,
    BGEVENT_WSG_CAPTURE_FLAG,
    BGEVENT_WSG_RETURN_FLAG,
    BGEVENT_WSG_PLAYER_DIED_WITH_FLAG,
    BGEVENT_WSG_SCORE_FLAG,
    BGEVENT_WSG_ALLIANCE_WIN,
    BGEVENT_WSG_HORDE_WIN,
    BGEVENT_WSG_PLAYER_KILL,
    BGEVENT_AB_NULL,
    BGEVENT_AB_PLAYER_DEATH,
    BGEVENT_AB_PLAYER_KILL,
    BGEVENT_AV_NULL,
    BGEVENT_AV_PLAYER_DEATH,
    BGEVENT_AV_PLAYER_KILL,
    BGEVENT_AV_GENERAL_DEAD_HORDE,
    BGEVENT_AV_GENERAL_DEAD_ALLIANCE,
};

class WOWD_SERVER_DECL BattlegroundMgr :  public Singleton < BattlegroundMgr >, public EventableObject
{
public:
    /* Construction */
    BattlegroundMgr();
    ~BattlegroundMgr();
    
    /* Packet Building */
    WorldPacket * BuildBattlegroundStatusPacket(uint32 Battleground, uint32 BattleGroundType, uint8 InstanceID, uint8 StatusID, uint32 Time, bool Faction);
    WorldPacket * BuildPlayerLeftBattlegroundPacket(Player* plr);
    WorldPacket * BuildPlayerJoinedBattlegroundPacket(Player* plr);
    WorldPacket * BuildBattlegroundListPacket(uint64 guid, Player* plr,uint32 BattlegroundType);
    WorldPacket * BuildWorldStateUpdatePacket(uint16 field, uint16 value);

    /* Battlegrounds */
    inline std::map<uint32, Battleground*>::iterator GetBattlegroundsBegin() { return m_Battlegrounds.begin(); };
    inline std::map<uint32, Battleground*>::iterator GetBattlegroundsEnd() { return m_Battlegrounds.end(); };
    Battleground* GetBattleground(uint8 ID)
    {
        std::map<uint32, Battleground*>::iterator i = m_Battlegrounds.find(ID);
        if(i != m_Battlegrounds.end())
            return i->second;
        else
            return NULL;
    }
    Battleground* GetBattlegroundByInstanceID(uint32 ID,uint32 battlegroundtype);
        
    uint32 CreateBattleground(uint32 MaxPlayersPerTeam, uint32 LevelMin, uint32 LevelMax, std::string BattlegroundName, uint32 MapID, float Team1StartLocX, float Team1StartLocY, float Team1StartLocZ, float Team1StartLocO, float Team2StartLocX, float Team2StartLocY, float Team2StartLocZ, float Team2StartLocO);
    inline void AddBattleground(uint32 ID, Battleground* BG) { m_Battlegrounds[ID] = BG; };
    void RemoveBattleground(uint32 ID);
    void CreateWSGBattleground();
    void CreateABBattleground();
    void CreateAVBattleground();
    void CreateArena();
    void CreateInitialBattlegrounds();

    uint32 GenerateTeamByRace(uint8 Race);
    uint32 GeneratePlayerKillMoney(Player *pKiller, Player* pVictim);
    
    void CleanupBattlegrounds();

    void EventCreateBattleground(uint32 mapid);

    uint32 GetBattleGroundMapIdByType(uint32 BattleGroundType)
    {
        switch (BattleGroundType)
        {
            case BATTLEGROUND_ALTERAC_VALLEY: //Alterac Valley
                return 30;
                break;
            case BATTLEGROUND_WARSUNG_GULCH: //Warsong Gulch
                return 489;
                break;
            case BATTLEGROUND_ARATHI_BASIN: //Arathi Basin
                return 529;
                break;
            case BATTLEGROUND_NAGRAND_ARENA: //Nagrand Arena
                return 559;
                break;
            case BATTLEGROUND_BLADES_EDGE_ARENA: //Blade's Edge Arena
                return 562;
                break;
            case BATTLEGROUND_ALL_ARENAS: //Netherstorm BG
                return 566;
                break;
            default:
                return 0;
        }
    };

    uint32 GetBattleGroundTypeByMapId(uint32 MapID)
    {
        switch (MapID)
        {
            case 30: //Alterac Valley
                return BATTLEGROUND_ALTERAC_VALLEY;
                break;
            case 489: //Warsong Gulch
                return BATTLEGROUND_WARSUNG_GULCH;
                break;
            case 529: //Arathi Basin
                return BATTLEGROUND_ARATHI_BASIN;
                break;
            case 559: //Nagrand Arena
                return BATTLEGROUND_NAGRAND_ARENA;
                break;
            case 562: //Blade's Edge Arena
                return BATTLEGROUND_BLADES_EDGE_ARENA;
                break;
            case 566: //Netherstorm BG
                return BATTLEGROUND_ALL_ARENAS;
                break;
            default:
                return 0;
                break;
        }
    };

private:

    /* Battlegrounds */
    std::map<uint32, Battleground*> m_Battlegrounds;
};


#define PVPRANK_GRAND_MARSHAL               18
#define PVPRANK_FIELD_MARSHAL               17
#define PVPRANK_MARSHAL                     16
#define PVPRANK_COMMANDER                   15
#define PVPRANK_LIEUTENANT_COMMANDER        14
#define PVPRANK_KNIGHT_CHAMPION             13
#define PVPRANK_KNIGHT_CAPTAIN              12
#define PVPRANK_KNIGHT_LIEUTENANT           11
#define PVPRANK_KNIGHT                      10
#define PVPRANK_SERGEANT_MAJOR              9
#define PVPRANK_MASTER_SERGEANT             8
#define PVPRANK_SERGEANT                    7
#define PVPRANK_CORPORAL                    6
#define PVPRANK_PRIVATE                     5
#define PVPRANK_DISHONORED                  4
#define PVPRANK_EXILED                      3
#define PVPRANK_OUTLAW                      2
#define PVPRANK_PARIAH                      1

#define SOUND_BATTLEGROUND_BEGIN            0xD6F
#define SOUND_HORDE_SCORES                  0x1FED
#define SOUND_ALLIANCE_SCORES               0x1FEE
#define SOUND_ALLIANCE_CAPTURE              0x2014
#define SOUND_HORDE_CAPTURE                 0x2015
#define SOUND_HORDEWINS                     0x2106
#define SOUND_ALLIANCEWINS                  0x2107
#define SOUND_HORDE_BGALMOSTEND             0x2108
#define SOUND_ALLIANCE_BGALMOSTEND          0x2109



// AB define's
#define AB_CAPTURED_STABLES_ALLIANCE        0x6E7
#define AB_CAPTURED_STABLES_HORDE           0x6E8
#define AB_CAPTURING_STABLES_ALLIANCE       0x6E9
#define AB_CAPTURING_STABLES_HORDE          0x6EA
                                         // 0x6EB is unknown
#define AB_CAPTURED_FARM_ALLIANCE           0x6EC // 1 is captured by the alliance
#define AB_CAPTURED_FARM_HORDE              0x6ED // 1 is captured by the horde
#define AB_CAPTURING_FARM_ALLIANCE          0x6EE // 1 is capturing by the alliance
#define AB_CAPTURING_FARM_HORDE             0x6EF // 1 is capturing by the horde


#define AB_ALLIANCE_RESOURCES               0x6F0
#define AB_HORDE_RESOURCES                  0x6F1
#define AB_HORDE_CAPTUREBASE                0x6F2
#define AB_ALLIANCE_CAPTUREBASE             0x6F3
#define AB_MAX_SCORE                        0x6F4

#define AB_CAPTURED_BLACKSMITH_ALLIANCE     0x6F6
#define AB_CAPTURED_BLACKSMITH_HORDE        0x6F7
#define AB_CAPTURING_BLACKSMITH_ALLIANCE    0x6F8
#define AB_CAPTURING_BLACKSMITH_HORDE       0x6F9
                                         // 0x6FA is unknown
#define AB_CAPTURED_GOLDMINE_ALLIANCE       0x6FB
#define AB_CAPTURED_GOLDMINE_HORDE          0x6FC
#define AB_CAPTURING_GOLDMINE_ALLIANCE      0x6FD
#define AB_CAPTURING_GOLDMINE_HORDE         0x6FE
                                         // 0x6FF is unknown
#define AB_CAPTURED_LUMBERMILL_ALLIANCE     0x700
#define AB_CAPTURED_LUMBERMILL_HORDE        0x701
#define AB_CAPTURING_LUMBERMILL_ALLIANCE    0x702
#define AB_CAPTURING_LUMBERMILL_HORDE       0x703

#define AB_SHOW_STABLE_ICON                 0x732
#define AB_SHOW_GOLDMINE_ICON               0x733
#define AB_SHOW_LUMBERMILL_ICON             0x734
#define AB_SHOW_FARM_ICON                   0x735
#define AB_SHOW_BACKSMITH_ICON              0x736

// WSG define's
#define WSG_ALLIANCE_FLAG_CAPTURED          0x922
#define WSG_HORDE_FLAG_CAPTURED             0x923
#define WSG_CURRENT_HORDE_SCORE             0x62E
#define WSG_CURRENT_ALLIANCE_SCORE          0x62D
#define WSG_MAX_SCORE                       0x641

// AV define's
#define AV_UNCONTROLED_SNOWFALL_GRAVE       0x7AE //1 -> show uncontrolled

#define AV_CONTROLED_ICEBLOOD_TOWER_HORDE   0x569 //1 -> horde controled
#define AV_CONTROLED_TOWER_POINT_HORDE      0x568 //1 -> horde controled
#define AV_CONTROLED_FROSTWOLF_RELIFHUNT_HORDE 0x532 //1 -> horde controled
#define AV_CONTROLED_EAST_FROSTWOLF_TOWER_HORDE 0x567 //1 -> horde controled
#define AV_CONTROLED_WEST_FROSTWOLF_TOWER_HORDE 0x566 //1 -> horde controled
#define AV_CONTROLED_ICEBLOOD_GRAVE_HORDE   0x543 //1 -> horde controled
#define AV_CONTROLED_FROSTWOLF_GRAVE_HORDE   0x53A //1 -> horde controled

#define AV_CONTROLED_IRONDEEP_MINE_TROGG    0x550 //1 -> trogg controled
#define AV_CONTROLED_COLDTHOOT_MINE_KOBOLT  0x54D //1 -> kobolt controled

#define AV_CONTROLED_STORMPIKE_GRAVE_ALLIANCE  0x535 //1 -> alliance controled
#define AV_CONTROLED_STONEHEART_BUNKER_ALLIANCE  0x554 //1 -> alliance controled
#define AV_CONTROLED_ICEWING_BUNKER_ALLIANCE  0x553 //1 -> alliance controled
#define AV_CONTROLED_DUBALDER_NORTH_BUNKER_ALLIANCE  0x552 //1 -> alliance controled
#define AV_CONTROLED_DUBALDER_SOUTH_BUNKER_ALLIANCE  0x551 //1 -> alliance controled
#define AV_CONTROLED_STORMPIKE_AID_STATION_ALLIANCE  0x52D //1 -> alliance controled
#define AV_CONTROLED_STONEHEART_GRAVE_ALLIANCE  0x516 //1 -> alliance controled

class WOWD_SERVER_DECL Battleground : public EventableObject
{
    friend class BattlegroundMgr;
public:
    /* Construction */
    Battleground();
    ~Battleground();

    /* Battleground */
    bool m_Active;
    bool m_start;
    inline void SetName(std::string Name) { m_Name = Name; };
    inline std::string GetName() { return m_Name; };
    inline void SetID(uint32 ID) { m_ID = ID; };
    inline uint32 GetID() { return m_ID; };
    inline void SetMaxPlayers(uint32 MaxPlayers) { m_MaxPlayers = MaxPlayers; };
    inline uint32 GetMaxPlayers() { return m_MaxPlayers; };
    inline void SetLevelRange(uint32 min, uint32 max)
    {
        m_LevelMin = min;
        m_LevelMax = max;
    }
    inline uint32 GetMinLevel() { return m_LevelMin; };
    inline uint32 GetMaxLevel() { return m_LevelMax; };
    
    inline void SetMaxPlayersPerTeam(uint32 MaxPlayers) { m_MaxPlayersPerTeam = MaxPlayers; };
    inline uint32 GetMaxPlayersPerTeam() { return m_MaxPlayersPerTeam; };

    bool HasFreeSlots(uint32 Team);
    
    /* Player lists */
    inline std::list<Player*>::iterator GetPlayersBegin() { return m_Players.begin(); };
    inline std::list<Player*>::iterator GetPlayersEnd() { return m_Players.end(); };

    inline std::list<Player*>::iterator GetQueuedPlayersBegin() { return m_QueuedPlayers.begin(); };
    inline std::list<Player*>::iterator GetQueuedPlayersEnd() { return m_QueuedPlayers.end(); };

    std::map<uint64, Player*> m_ReviveQueue;                       // for area spirit healers
    uint32 m_LastResurrect;

    void AddPlayer(Player* plr, bool Transport = false, bool SendPacket = false);
    void RemovePlayer(Player *plr, bool Transport = false, bool SendPacket = false, bool BeforeEnd = true);
    void RemoveAllPlayers(bool Transport = false, bool SendPacket = false);

    /* Packet Building */
    WorldPacket* BuildPlayerPositionsPacket(Player* Source);     // Because we don't include source in packet
    WorldPacket* BuildPVPLogDataPacket(Player *Source, uint8 BGGameStatus = 0, bool BGGameWin = false);// Because we don't include source in packet
    void UpdatePVPData();                                       // Updating after changes

    /* Battleground Events */
    virtual void HandleBanner(Player *p_caster,GameObject *go,uint32 spellid);
    virtual void HandleBattlegroundEvent(Object *src, Object *dst, uint16 EventID, uint32 params1 = 0, uint32 params2 = 0);
    virtual void HandleBattlegroundAreaTrigger(Player *plr, uint32 TriggerID);
    virtual void SetupBattleground();
    virtual void Start();
    virtual void SpawnBattleground();
    virtual void SpawnBuff(uint32 typeentry,uint32 bannerslot);
    
    void End();
    
    void EventResurrectPlayers();
    void EventRemove();
    void EventAlmostStart();
    void EventStart();

    /* Location */
    inline void SetMapId(uint32 MapID) { m_MapId = MapID; };
    inline uint32 GetMapId() { return m_MapId; };

    void SetTeamStartLoc(uint32 TeamID, float X, float Y, float Z, float O);
    inline float GetTeamStartLocX(uint32 TeamID) { return m_TeamStartLocX[TeamID]; };
    inline float GetTeamStartLocY(uint32 TeamID) { return m_TeamStartLocY[TeamID]; };
    inline float GetTeamStartLocZ(uint32 TeamID) { return m_TeamStartLocZ[TeamID]; };
    inline float GetTeamStartLocO(uint32 TeamID) { return m_TeamStartLocO[TeamID]; };

    /* Packet Transfer */
    void SendPacketToTeam(uint32 TeamID, WorldPacket *packet);
    void SendPacketToAll(WorldPacket *packet);

    /* Worldstates */
    std::map<uint32, uint32> WorldStateVars;
    WorldPacket* BuildWorldStateInit();
    WorldPacket* BuildclearWorldStates(uint32 mapid);
    WorldPacket* BuildMessageChat(uint32 type, uint32 p2, const char* msg, uint8 p3, uint64 guid = 0);
    void SetWorldStateValue(uint32 field, uint32 value);
    void _EventUpdateWorldStates();

    void SendMessageToPlayer(Player *plr, std::string msg, bool widetoo);
    void SendCMessageToPlayer(Player *plr, const char* szmsg, bool widetoo);
    void SendMessageToAll(std::string msg, bool widetoo);
    void SendCMessageToAll(const char* szmsg, bool widetoo);
        
    GameObject* SpawnGameObject(uint32 entry, uint32 MapId, float x, float y, float z, float o, uint32 flags, uint32 faction, float scale);

    inline uint32 GetTeamScore(uint32 TeamID)
    {
        ASSERT(TeamID > 1);
        return m_TeamScores[TeamID];
    }

    inline void AddPoint(uint32 TeamID, uint32 Points = 1)
    {
        ASSERT(TeamID > 1);
        m_TeamScores[TeamID] += Points;

        // TODO: Update all players
    }

    inline void RemovePoint(uint32 TeamID, uint32 Points = 1)
    {
        ASSERT(TeamID > 1);
        m_TeamScores[TeamID] -= Points;

        // TODO: Update all players
    }

    inline WorldPacket MakeAreaTriggerMsg(const char* text, ...)
    {
        WorldPacket data;
        char txt[256];
        va_list ap;
        va_start(ap, text);
        vsprintf(txt, text, ap);
        va_end(ap);
        data.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
        data << (uint32)0 << txt << (uint8)0x00;
        return data;
    }  

    void SendSoundToBattleground(uint32 sound_id);

    bool bSpawned;
    bool bStarted;
    void OnPlayerJoined(Player *pPl);
    void OnPlayerLeft(Player* pPl);
    bool m_GameStatus; // false if game is running true if game has ended
    bool m_GameWin; // false if horde wins true of alliance wins

    inline bool GetBattleGroundStatus(){return m_GameStatus;}
    inline void SetBattleGroundStatus(bool value){m_GameStatus = value;}

    inline bool GetBattleGameWin(){return m_GameWin;}
    inline void SetBattleGameWin(bool value){m_GameWin = value;}

    inline MapMgr * GetMapMgr() { return m_MapMgr; }
    inline void SetMapMgr(MapMgr* mapmgr) { m_MapMgr = mapmgr; }
    inline void SetMap(Map* map_) { m_Map = map_; }
    inline Map * GetMap() { return m_Map; }

    inline void SetInstanceID(int32 instanceid) { m_InstanceId = instanceid; }
    inline int32 GetInstanceID() { return m_InstanceId; }
    
    void CreateInstance();

    inline uint32 GetBattlegroundType(void)
    {
        return m_BattlegroundType;
    }

    void SpawnSpiritGuide(float x, float y, float z, float o, uint32 horde);

protected:
    /* Battleground */
    uint32 m_ID;
    int32 m_InstanceId;
    std::string m_Name;
    uint32 m_LevelMin;
    uint32 m_LevelMax;
    uint32 m_BattlegroundType;   // 1 = Warsong Gulch, 2 = Arathi Basin, 3 = Alterac Valley
    Group* m_HordeGroup;
    Group* m_AllianceGroup;

    /* Scorekeeping */
public:
    uint32 m_TeamScores[2];                                // Usually Alliance/Horde
    std::map<uint64, BattlegroundScore> m_PlayerScores;    // Player scores.. usually I would say by Player* pointer, but it's easier to build with guid
    

    /* Player lists */
    std::list<Player*> m_Players;
    std::list<Player*> m_QueuedPlayers;

    int32 event_GetInstanceID() { return m_InstanceId; }

protected:
    uint32 m_MaxPlayersPerTeam;
    uint32 m_MaxPlayers;

    /* Location */
    uint32 m_MapId;
    float m_TeamStartLocX[2];
    float m_TeamStartLocY[2];
    float m_TeamStartLocZ[2];
    float m_TeamStartLocO[2];

    MapMgr *m_MapMgr;
    Map * m_Map;

};
    
#define sBattlegroundMgr BattlegroundMgr::getSingleton()
    
#endif
