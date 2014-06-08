// Copyright (C) 2004 WoWD Team

#ifndef _PLAYER_H
#define _PLAYER_H

struct BattlegroundScore;
class Channel;
class Creature;
class Battleground;
class TaxiPath;
class GameObject;
class Transporter;
class Corpse;
class Guild;
class Pet;
struct LevelInfo;

#define myabs(a) (a<0)?(-a):a

#define MAX_PET_NO 3

#define PLAYER_NORMAL_RUN_SPEED 7.0
#define PLAYER_NORMAL_SWIM_SPEED 4.722222
#define PLAYER_NORMAL_FLIGHT_SPEED 7.0
#define PLAYER_LEVEL_CAP 70
#define PLAYER_HONORLESS_TARGET_SPELL 2479

//====================================================================
//  Inventory
//  Holds the display id and item type id for objects in
//  a character's inventory
//====================================================================

enum Classes
{
    WARRIOR = 1,
    PALADIN = 2,
    HUNTER = 3,
    ROGUE = 4,
    PRIEST = 5,
    SHAMAN = 7,
    MAGE = 8,
    WARLOCK = 9,
    DRUID = 11,
};

enum Races
{
    RACE_HUMAN = 1,
    RACE_ORC = 2,
    RACE_DWARF = 3,
    RACE_NIGHTELF = 4,
    RACE_UNDEAD = 5,
    RACE_TAUREN = 6,
    RACE_GNOME = 7,
    RACE_TROLL = 8,
    RACE_BLOODELF = 10,
    RACE_DRAENEI = 11,
};

enum PlayerStatus
{
    NONE             = 0,
    TRANSFER_PENDING = 1,
};

struct spells
{
    uint16 spellId;
    uint16 slotId;
};

struct aurloads
{
    uint16 id;
    uint32 dur;
};

struct actions
{
    uint8 button;
    uint8 type;
    uint8 misc;
    uint16 action;
};

enum PvPAreaStatus
{
    AREA_ALLIANCE = 1,
    AREA_HORDE = 2,
    AREA_CONTESTED = 3,
    AREA_PVPARENA = 4,
};

struct PvPArea {
    uint32 AreaId;
    std::string AreaName;
    uint16 PvPType;

    bool HomeFor(Player* plr);
};

struct CreateInfo_ItemStruct
{
    uint32 protoid;
    uint8 slot;
    uint32 amount;
};

struct CreateInfo_SkillStruct
{
    uint32 skillid;
    uint32 currentval;
    uint32 maxval;
};

struct CreateInfo_ActionBarStruct
{
    uint32 button;
    uint32 action;
    uint32 type;
    uint32 misc;
};

struct PlayerCreateInfo{
    uint8 index;
    uint8 race;
    uint32 factiontemplate;
    uint8 class_;
    uint32 mapId;
    uint32 zoneId;
    float positionX;
    float positionY;
    float positionZ;
    uint16 displayId;
    uint8 strength;
    uint8 ability;
    uint8 stamina;
    uint8 intellect;
    uint8 spirit;
    uint32 health;
    uint32 mana;
    uint32 rage;
    uint32 focus;
    uint32 energy;
    uint32 attackpower;
    float mindmg;
    float maxdmg;
    std::list<CreateInfo_ItemStruct> items;
    std::list<CreateInfo_SkillStruct> skills;
    std::list<CreateInfo_ActionBarStruct> actionbars;
    std::list<uint16> spell_list;
    //uint32 item[10];
    //uint8 item_slot[10];
    //uint16 spell[10];
};

enum PlayerMovementType
{
    MOVE_ROOT       = 1,
    MOVE_UNROOT     = 2,
    MOVE_WATER_WALK = 3,
    MOVE_LAND_WALK  = 4,
};

enum PlayerSpeedType
{
    RUN      = 1,
    RUNBACK  = 2,
    SWIM     = 3,
    SWIMBACK = 4,
    WALK     = 5,
    FLY      = 6,
};

enum Standing
{
    STANDING_HATED,
    STANDING_HOSTILE,
    STANDING_UNFRIENDLY,
    STANDING_NEUTRAL,
    STANDING_FRIENDLY,
    STANDING_HONORED,
    STANDING_REVERED,
    STANDING_EXALTED
};

enum PlayerFlags
{
    PLAYER_FLAG_PARTY_LEADER            = 0x01,
    PLAYER_FLAG_AFK                     = 0x02,
    PLAYER_FLAG_DND                     = 0x04,
    PLAYER_FLAG_GM                      = 0x08,
    PLAYER_FLAG_DEATH_WORLD_ENABLE      = 0x10,
    PLAYER_FLAG_RESTING                 = 0x20,
    PLAYER_FLAG_FREE_FOR_ALL_PVP        = 0x80,
    PLAYER_FLAG_PVP_TOGGLE              = 0x200,
    PLAYER_FLAG_NOHELM                  = 0x400,
    PLAYER_FLAG_NOCLOAK                 = 0x800,
    PLAYER_FLAG_NEED_REST_3_HOURS       = 0x1000,
    PLAYER_FLAG_NEED_REST_5_HOURS       = 0x2000,
};

struct Reputation
{
    int32 standing;
    uint8 flag;
};

struct Faction
{
    uint8 race;
    uint8 id;
    uint8 state;
    uint32 standing;
};

struct PlayerInfo
{
    uint64 guid;
    std::string name;
    uint32 race;
    uint32 gender;
    uint32 cl;
    uint32 team;
    
    //guild stuff
    uint32 Rank;//guild rank
    std::string publicNote;
    std::string officerNote;
    time_t lastOnline;
    uint32 lastZone;
    uint32 lastLevel;
};

struct PlayerPet
{
    string name;
    uint32 entry;
    string fields;
    uint32 xp;
    bool active;
    uint32 number;
    uint32 level;
    uint32 loyalty;
    uint32 loyaltyupdate;
    string actionbar;
    bool summon;
    uint32 autocastspell;
};

enum MeetingStoneQueueStatus
{
    MEETINGSTONE_STATUS_NONE                    = 0,
    MEETINGSTONE_STATUS_JOINED_MEETINGSTONE_QUEUE_FOR        = 1,
    MEETINGSTONE_STATUS_PARTY_MEMBER_LEFT_LFG            = 2,
    MEETINGSTONE_STATUS_PARTY_MEMBER_REMOVED_PARTY_REMOVED    = 3,
    MEETINGSTONE_STATUS_LOOKING_FOR_NEW_PARTY_IN_QUEUE        = 4,
    MEETINGSTONE_STATUS_NONE_UNK                    = 5,
};

enum ItemPushResultTypes
{
    ITEM_PUSH_TYPE_LOOT             = 0x00000000,
    ITEM_PUSH_TYPE_RECEIVE          = 0x00000001,
    ITEM_PUSH_TYPE_CREATE           = 0x00000002,
};

struct WeaponModifier
{
    uint32 wclass;
    uint32 subclass;
    float value;
};

struct PetActionBar
{
    uint32 spell[10];
};

struct AmmoAttackSpeed
{
    uint32 rangedAmmoAttackSpeedMod;
    uint32 itemId;
};

struct ItemCooldown
{
    uint16 SpellID;
    uint16 ItemEntry;
    uint16 SpellCategory;       //this maybe got from spell id, maybe saved for speed
    uint32 CooldownTimeStamp;
    uint32 Cooldown;
};

class Spell;
class Item;
class Container;
class WorldSession;
class ItemInterface;
class GossipMenu;
struct TaxiPathNode;

#define RESTSTATE_RESTED             1
#define RESTSTATE_NORMAL             2
#define RESTSTATE_TIRED100             3
#define RESTSTATE_TIRED50             4
#define RESTSTATE_EXHAUSTED             5

#define UNDERWATERSTATE_NONE         0
#define UNDERWATERSTATE_SWIMMING     1
#define UNDERWATERSTATE_UNDERWATER   2
#define UNDERWATERSTATE_RECOVERING   4
#define UNDERWATERSTATE_TAKINGDAMAGE 8
#define UNDERWATERSTATE_FATIGUE      16
#define UNDERWATERSTATE_LAVA         32
#define UNDERWATERSTATE_SLIME        64

enum TRADE_STATUS
{
    TRADE_STATUS_PLAYER_BUSY       = 0x00,
    TRADE_STATUS_PROPOSED          = 0x01,
    TRADE_STATUS_INITIATED         = 0x02,
    TRADE_STATUS_CANCELLED         = 0x03,
    TRADE_STATUS_ACCEPTED          = 0x04,
    TRADE_STATUS_ALREADY_TRADING   = 0x05,
    TRADE_STATUS_PLAYER_NOT_FOUND  = 0x06,
    TRADE_STATUS_STATE_CHANGED     = 0x07,
    TRADE_STATUS_COMPLETE          = 0x08,
    TRADE_STATUS_UNACCEPTED        = 0x09,
    TRADE_STATUS_TOO_FAR_AWAY      = 0x0A,
    TRADE_STATUS_WRONG_FACTION     = 0x0B,
    TRADE_STATUS_FAILED            = 0x0C,
    TRADE_STATUS_DEAD              = 0x0D,
    TRADE_STATUS_PETITION          = 0x0E,
    TRADE_STATUS_PLAYER_IGNORED    = 0x0F,
};

enum TRADE_DATA
{
    TRADE_GIVE        = 0x00,
    TRADE_RECEIVE     = 0x01,
};

enum DUEL_STATUS
{
    DUEL_STATUS_OUTOFBOUNDS,
    DUEL_STATUS_INBOUNDS
};

enum DUEL_STATE
{
    DUEL_STATE_REQUESTED,
    DUEL_STATE_STARTED,
    DUEL_STATE_FINISHED
};

enum DUEL_WINNER
{
    DUEL_WINNER_KNOCKOUT,
    DUEL_WINNER_RETREAT,
};

//====================================================================
//  Player
//  Class that holds every created character on the server.
//
//  TODO:  Attach characters to user accounts
//====================================================================

typedef std::set<uint32> SpellSet;
typedef std::set<ItemCooldown*> ItemCooldownSet;
typedef std::set<uint32> SaveSet;
typedef std::map<uint64, ByteBuffer*> SplineMap;

class WOWD_SERVER_DECL Player : public Unit
{
    friend class WorldSession;
public:
    Player ( uint32 high, uint32 low );
    ~Player ( );

    void AddToWorld();
    void RemoveFromWorld();

    bool Create ( WorldPacket &data );
    bool Create ( uint32 guidlow, WorldPacket &data );

    void Update( uint32 time );

    void BuildEnumData( WorldPacket * p_data );

    std::string m_afk_reason;
    void SetAFKReason(std::string reason) { m_afk_reason = reason; };

    inline const char* GetName() { return m_name.c_str(); }
    inline std::string GetNameString() { return m_name; }

    void Die();
    //void KilledMonster(uint32 entry, const uint64 &guid);
    void GiveXP(uint32 xp, const uint64 &guid, bool allowbonus);   // to stop rest xp being given

    // Taxi
    void TaxiStart(TaxiPath *path, uint32 modelid, uint32 start_node);
    uint32 lastNode;
    inline TaxiPath* GetTaxiPath() { return m_CurrentTaxiPath; }
    void EventDismount(uint32 money, float x, float y, float z);
    void SetTaxiState(bool state) { m_onTaxi = state; }
    inline bool GetTaxiState() { return m_onTaxi; }
    const uint32& GetTaximask( uint8 index ) const { return m_taximask[index]; }
    void SetTaximask( uint8 index, uint32 value ) { m_taximask[index] = value; }
    void LoadTaxiMask(const char* data);
    void EventTaxiInterpolate();
    inline void SetTaxiPath(TaxiPath *path) { m_CurrentTaxiPath = path; }
    void SetTaxiPos()
    {
        m_taxi_pos_x = m_position.x;
        m_taxi_pos_y = m_position.y;
        m_taxi_pos_z = m_position.z;
    }
    void UnSetTaxiPos()
    {
        m_taxi_pos_x = 0;
        m_taxi_pos_y = 0;
        m_taxi_pos_z = 0;
    }

    //Quest stuff
    bool HasQuests() 
    {
        for(int i = 0; i < 20; ++i)
        {
            if(m_questlog[i] != 0)
                return true;
        }
        return false;
    }
    int32 GetOpenQuestSlot();
    QuestLogEntry* GetQuestLogForEntry(uint32 quest);
    inline QuestLogEntry* GetQuestLogInSlot(uint32 slot) { return m_questlog[slot]; }
    void SetQuestLogSlot(QuestLogEntry *entry, uint32 slot);
    inline void PushToRemovedQuests(uint32 questid)
    {
        m_removequests.insert(questid);
    }

    //void ResetQuestSlots();
    void AddToFinishedQuests(uint32 quest_id);
    bool HasFinishedQuest(uint32 quest_id);
    bool HasQuestForItem(uint32 itemid);

    inline uint32 GetTeam()
    {
        uint32 r=getRace();
        if(r==RACE_DWARF || r== RACE_GNOME || r==RACE_HUMAN || r==RACE_NIGHTELF || r == RACE_DRAENEI)
            return 0;
        else return 1;
    }

    const uint64& GetSelection( ) const { return m_curSelection; }
    const uint64& GetTarget( ) const { return m_curTarget; }

    void SetSelection(const uint64 &guid) { m_curSelection = guid; }
    void SetTarget(const uint64 &guid) { m_curTarget = guid; }

    // Bid
    void AddBid(bidentry *be);
    bidentry* GetBid(uint32 id);
    std::list<bidentry*>::iterator GetBidBegin() { return m_bids.begin();};
    std::list<bidentry*>::iterator GetBidEnd() { return m_bids.end();};
    uint32 GetBidSize() { return m_bids.size();};

    // Spells
    bool HasSpell(uint32 spell);
    bool HasDeletedSpell(uint32 spell);
    void smsg_InitialSpells();
    void addSpell(uint32 spell_idy);
    bool removeSpell(uint32 SpellID, bool MoveToDeleted, bool SupercededSpell, uint32 SupercededSpellID);

    //action bar
    void addAction(uint8 button, uint16 action, uint8 type, uint8 misc);
    //void addLoadAur(uint16 id, uint32 dur);
    inline std::list<struct actions> getActionList() { return m_actions; };
    bool m_actionsDirty;
    //std::list<struct aurloads>::iterator GetaurBegin() { return m_aurloads.begin();};
    //std::list<struct aurloads>::iterator GetaurEnd() { return m_aurloads.end();};

    void removeAction(uint8 button);
    void smsg_InitialActions();

    // factions
    void smsg_InitialFactions();
    void modReputation(uint8 id, int32 mod, bool updateclient);
    void modPercAllReputation(int32 mod, bool updateclient);
    int32 pctReputationMod;
    uint8 GetStandingById(uint8 id);
    uint8 GetStandingByFactionTemplate(uint32 fctTmp);
    uint32 GetFactionId();
    void SetFactionState(uint8 id, uint8 state);
    inline uint8 GetPVPRank()
    {
        return (uint8)((GetUInt32Value(PLAYER_BYTES_3) >> 24) & 0xFF);
    }
    inline void SetPVPRank(int newrank)
    {
        SetUInt32Value(PLAYER_BYTES_3, ((GetUInt32Value(PLAYER_BYTES_3) & 0x00FFFFFF) | (uint8(newrank) << 24)));
    }

    // groups
    void SetInviter(uint64 pInviter) { m_GroupInviter = pInviter; }
    uint64 GetInviter() { return m_GroupInviter; }

    inline bool InGroup() { return (m_Group != NULL && !m_GroupInviter); }
    bool IsGroupLeader()
    {
        if(m_Group != NULL)
        {
            if(m_Group->GetLeader() == this)
                return true;
        }
        return false;
    }

    int HasBeenInvited() { return m_GroupInviter != 0; }

    Group* GetGroup() { return m_Group; }
    void SetGroup(Group* grp) { m_Group = grp; }
    SubGroup* GetSubGroup() { return m_SubGroup; }
    void SetSubGroup(SubGroup* group) { m_SubGroup = group; }

    std::set<uint32> OnMeleeAuras;

    // DK isGroupMember(plyr)
    bool IsGroupMember(Player *plyr);

    //DK ban
    void SetBanned() { m_banned = 4;}
    void SetBanned(string Reason) { m_banned = 4; m_banreason = Reason;}
    void UnSetBanned() { m_banned = 0; }
    inline bool IsBanned() { return ((m_banned > 0) ? true : false); }
    string GetBanReason() {return m_banreason;}

    //DK:Guild
    inline  bool IsInGuild() {return (bool)GetUInt32Value(PLAYER_GUILDID);}
    inline uint32 GetGuildId() { return m_uint32Values[PLAYER_GUILDID]; }
    void SetGuildId(uint32 guildId);
    inline uint32 GetGuildRank() { return m_uint32Values[PLAYER_GUILDRANK]; }
    inline void SetGuildRank(uint32 guildRank) { SetUInt32Value(PLAYER_GUILDRANK, guildRank); }
    uint64 GetGuildInvitersGuid() { return m_invitersGuid; }
    void SetGuildInvitersGuid( uint64 guid ) { m_invitersGuid = guid; }
    void UnSetGuildInvitersGuid() { m_invitersGuid = 0; }
  
    bool CanSignCharter() {return !(m_signedCharter||IsInGuild());}
    void SetCharterSigned() {m_signedCharter=true;}
    void UnSetCharterSigned() { m_signedCharter =  false; }


    //Duel
    Player* DuelingWith;
    
    void RequestDuel(Player *pTarget);
    void DuelBoundaryTest();
    void EndDuel(uint8 WinCondition);

    void DuelCountdown();

    void SetDuelStatus(uint8 status) { m_duelStatus = status; }
    inline uint8 GetDuelStatus() { return m_duelStatus; }
    void SetDuelState(uint8 state) { m_duelState = state; }
    inline uint8 GetDuelState() { return m_duelState; }
    void SendTradeUpdate(void);

    inline void ResetTradeVariables()
    {
        mTradeGold = 0;
        memset(&mTradeItems, 0, sizeof(Item*) * 8);
        mTradeStatus = 0;
    }

    //Pet
    inline void SetSummon(Pet *pet) { m_Summon = pet; }
    inline Pet* GetSummon(void) { return m_Summon; }

    uint32 GeneratePetNumber(void);
    void RemovePlayerPet(uint32 pet_number);
    inline void AddPlayerPet(PlayerPet* pet, uint32 index) { m_Pets[index] = pet; }
    inline PlayerPet* GetPlayerPet(uint32 idx)
    {
        std::map<uint32, PlayerPet*>::iterator itr = m_Pets.find(idx);
        if(itr != m_Pets.end()) return itr->second;
        else
            return NULL;
    }

    void SpawnPet(uint32 pet_number);
    void DespawnPet();
    uint32 GetFirstPetNumber(void)
    {
        if(m_Pets.size() == 0) return 0;
        std::map<uint32, PlayerPet*>::iterator itr = m_Pets.begin();
        return itr->first;
    }
    inline PlayerPet* GetFirstPet(void) { return GetPlayerPet(GetFirstPetNumber()); }
    inline void SetStableSlotCount(uint8 count) { m_StableSlotCount = count; }
    inline uint8 GetStableSlotCount(void) { return m_StableSlotCount; }

    // Items
    //Item Interface
    inline ItemInterface* GetItemInterface() { return m_ItemInterface; } // Player Inventory Item storage
    inline void ApplyItemMods(Item *item, int8 slot, bool apply) {  _ApplyItemMods(item, slot, apply); }

    // looting
    inline const uint64& GetLootGUID() const { return m_lootGuid; }
    void SetLootGUID(const uint64 &guid) { m_lootGuid = guid; }
    void SendLoot(uint64 guid,uint8 loot_type);

    inline WorldSession* GetSession() const { return m_session; }
    void SetSession(WorldSession *s) { m_session = s; }
    void SetBindPoint(float x, float y, float z, uint32 m, uint32 v) { m_bind_pos_x = x; m_bind_pos_y = y; m_bind_pos_z = z; m_bind_mapid = m; m_bind_zoneid = v;}

    void SendDelayedPacket(WorldPacket *data, bool bDeleteOnSend)
    {
        if(data == NULL) return;
        if(GetSession() != NULL) GetSession()->SendPacket(data);
        if(bDeleteOnSend) delete data;
    }

    float offhand_dmg_mod;
    float GetSpellTimeMod(uint32 id);
    int GetSpellDamageMod(uint32 id);
    int32 GetSpellManaMod(uint32 id);
    
    // Talents
    // These functions build a specific type of A9 packet
    uint32 __fastcall BuildCreateUpdateBlockForPlayer( ByteBuffer *data, Player *target );
    void DestroyForPlayer( Player *target ) const;

    // Serialize character to db
    void SaveToDB(bool bNewCharacter =false);
    bool LoadFromDB(uint32 guid);
    void LoadFromDB_Light(Field *fields, uint32 guid);
    void LoadPropertiesFromDB();
    void LoadNamesFromDB(uint32 guid);
    void DeleteFromDB();
    bool m_FirstLogin;

    //Death Stuff
    void SpawnCorpseBody();
    void SpawnCorpseBones();
    void CreateCorpse();
    void KillPlayer();
    void ResurrectPlayer();
    void BuildPlayerRepop();
    void RepopRequestedPlayer();
    void DeathDurabilityLoss(double percent);
    void RepopAtGraveyard(float ox, float oy, float oz, uint32 mapid);

    //Movement stuff
    void SetMovement(uint8 pType, uint32 flag);
    void SetPlayerSpeed(uint8 SpeedType, float value, uint32 flag, bool forced=false);
    float GetPlayerSpeed(){return m_runSpeed;}
    uint8 m_currentMovement;
    bool m_isMoving;

    //Invisibility stuff
    bool m_isGmInvisible;

    //Channel stuff
    void JoinedChannel(Channel *c);
    void LeftChannel(Channel *c);
    void CleanupChannels();

    //Attack stuff
    void EventAttackStart();
    void EventAttackStop();
    void EventAttackUpdateSpeed() { }

    void EventDeath();

    // skilllines
    bool HasSkillLine(uint32 id);
    void AddSkillLine(uint32 id, uint32 currVal, uint32 maxVal);
    void AdvanceSkillLine(uint32 id);
    void ModSkillMax(uint32 id, uint32 amt, uint32 setcur = 0);
    //Note:ModSkillLine -> value+=amt;ModSkillMax -->value=amt; --wierd
    float GetSkillUpChance(uint32 id);
    uint32 GetSkillAmt(uint32 id);
    uint32 GetSkillPlace(uint32 id);
    uint32 GetSkillMax(uint32 id);
    void RemoveSkillLine(uint32 id);
    uint32 GetBaseSkillAmt(uint32 id);
    void ModSkillBonus(uint32 id,int32 bonus);
    void UpdateMaxSkills();
    //inline std::list<struct skilllines>getSkillLines() { return m_skilllines; }

    void UpdateChances();
    void UpdateStats();
    void UpdateHit(int32 hit);
   

    bool canCast(SpellEntry *m_spellInfo);

    inline float GetBlockFromSpellPCT() { return m_blockfromspellPCT; }
    inline float GetBlockFromSpell() { return m_blockfromspell; }
    inline float GetSpellCritFromSpell() { return m_spellcritfromspell; }
    inline float GetHitFromMeleeSpell() { return m_hitfrommeleespell; }
    inline float GetHitFromSpell() { return m_hitfromspell; }
    inline float GetParryFromSpell() { return m_parryfromspell; }
    inline float GetDodgeFromSpell() { return m_dodgefromspell; }
    void SetBlockFromSpellPCT(float value) { m_blockfromspellPCT = value; }
    void SetBlockFromSpell(float value) { m_blockfromspell = value; }
    void SetSpellCritFromSpell(float value) { m_spellcritfromspell = value; }
    void SetParryFromSpell(float value) { m_parryfromspell = value; }
    void SetDodgeFromSpell(float value) { m_dodgefromspell = value; }
    void SetHitFromMeleeSpell(float value) { m_hitfrommeleespell = value; }
    void SetHitFromSpell(float value) { m_hitfromspell = value; }

    inline uint32 GetHealthFromSpell() { return m_healthfromspell; }
    inline uint32 GetManaFromSpell() { return m_manafromspell; }
    void SetHealthFromSpell(uint32 value) { m_healthfromspell = value;}
    void SetManaFromSpell(uint32 value) { m_manafromspell = value;}

    uint32 CalcTalentResetCost(uint32 resetnum);
    void SendTalentResetConfirm();
    uint32 GetTalentResetTimes() { return m_talentresettimes; }
    inline void SetTalentResetTimes(uint32 value) { m_talentresettimes = value; }

    void SetPlayerStatus(uint8 pStatus) { m_status = pStatus; }
    inline uint8 GetPlayerStatus() { return m_status; }

    const float& GetBindPositionX( ) const { return m_bind_pos_x; }
    const float& GetBindPositionY( ) const { return m_bind_pos_y; }
    const float& GetBindPositionZ( ) const { return m_bind_pos_z; }
    const uint32& GetBindMapId( ) const { return m_bind_mapid; }
    const uint32& GetBindZoneId( ) const { return m_bind_zoneid; }

    //inline uint32 GetEating() { return eating; }
    //void SetEating(uint32 spell) { eating = spell; }
    //Showing Units WayPoints
    AIInterface* waypointunit;

    void SaveGuild();
    
    uint32 m_nextSave;

    //Tutorials
    uint32 GetTutorialInt(uint32 intId );
    void SetTutorialInt(uint32 intId, uint32 value);

    //Base stats calculations
    //void CalcBaseStats();

    // Rest
    void AddRestXP(uint32 amount);
    uint32 SubtractRestXP(uint32 amount);
    uint32 CalculateRestXP(uint32 seconds);
    uint32 m_lastRestUpdate;
    void EventPlayerRest();
    void ApplyPlayerRestState(bool apply);
    void UpdateRestState();

    //falling?
    uint32 m_fallTime;
    bool bSafeFall;
    // Gossip
    GossipMenu* CurrentGossipMenu;
    void CleanupGossipMenu();
    void Gossip_Complete();
    void Gossip_SendPOI(float X, float Y, uint32 Icon, uint32 Flags, uint32 Data, const char* Name);

    uint32 m_lastShotTime;
    
    bool m_bUnlimitedBreath;
    uint32 m_UnderwaterTime;
    uint32 m_UnderwaterState;
    uint32 m_SwimmingTime;
    uint32 m_BreathDamageTimer;

    // Visible objects
    bool CanSee(Object* obj);
    inline bool IsVisible(Object* pObj) { return !(m_visibleObjects.find(pObj) == m_visibleObjects.end()); }
    void AddInRangeObject(Object* pObj);
    void RemoveInRangeObject(Object* pObj);
    void ClearInRangeSet();
    void AddVisibleObject(Object* pObj) { m_visibleObjects.insert(pObj); }
    void RemoveVisibleObject(Object* pObj) { m_visibleObjects.erase(pObj); }
    void UpdateVisibility(bool destroy);
    inline InRangeSet::iterator GetVisibleSetBegin() { return m_visibleObjects.begin(); }
    inline InRangeSet::iterator GetVisibleSetEnd() { return m_visibleObjects.end(); }
    std::set<Player*> m_KnownGroupMembers;
    inline bool KnowsGroupMember(Player *plr) { return m_KnownGroupMembers.count(plr) > 0 ? true : false; }
    
    //Transporters
    bool m_lockTransportVariables;
    uint64 m_TransporterGUID;
    float m_TransporterX;
    float m_TransporterY;
    float m_TransporterZ;
    float m_TransporterO;
    float m_TransporterUnk;

    // Misc
    void EventCannibalize(uint32 amount);
    void EventReduceDrunk(bool full);
    bool m_AllowAreaTriggerPort;
    void EventAllowTiggerPort(bool enable);
    int32 m_rangedattackspeedmod;
    std::list<AmmoAttackSpeed*> m_ammoAttackSpeed;
    uint32 GetAmmoAttackSpeed();
    int32 m_meleeattackspeedmod;
    void EventFieldUpdateExpire();

    void SendInitialLogonPackets();
    void Reset_Spells();
    void Reset_Talents();
    void Reset_ToLevel1();

    // Battlegrounds xD
    bool m_bgInBattleground;
    uint32 m_bgBattlegroundID;
    uint32 m_bgLastBattlegroundID;
    uint32 m_bgEntryPointMap;
    float m_bgEntryPointX;    
    float m_bgEntryPointY;
    float m_bgEntryPointZ;
    float m_bgEntryPointO;
    int32 m_bgEntryPointInstance;
    uint32 m_bgTeam;
    Battleground* GetCurrentBattleground();
    bool m_bgHasFlag;

    void EventRepeatSpell(SpellEntry* spellinfo, uint32 duration, uint64 guid);
    void EventCastRepeatedSpell(uint32 spellid, Unit *target);
    bool CanShootRangedWeapon(uint32 spellid, Unit *target, bool autoshot);

    uint64 m_AutoShotTarget;
    float m_AutoShotStartX;
    float m_AutoShotStartY;
    float m_AutoShotStartZ;
    void _InitialReputation();
    void EventActivateGameObject(GameObject* obj);
    void EventDeActivateGameObject(GameObject* obj);
    void UpdateNearbyGameObjects();
    void EventTimedQuestExpire(Quest *qst, QuestLogEntry *qle, uint32 log_slot);
    uint32 timed_quest_slot;    

    void CalcResistance(uint32 type);
    uint32 FlatResistanceModifierPos[7];
    uint32 FlatResistanceModifierNeg[7];
    uint32 BaseResistanceModPctPos[7];
    uint32 BaseResistanceModPctNeg[7];
    uint32 ResistanceModPctPos[7];
    uint32 ResistanceModPctNeg[7];
    float SpellDmgDoneByInt[7];
    float SpellHealDoneByInt[7];
    
    uint32 FlatStatModPos[5];
    uint32 FlatStatModNeg[5];
    uint32 StatModPctPos[5];
    uint32 StatModPctNeg[5];
    uint32 TotalStatModPctPos[5];
    uint32 TotalStatModPctNeg[5];
    
    union {
        float mRatingToPct[37];
        uint32 mRatingToPoint[37]; //block, skill.. cant be decimal values
    };

    inline uint32* GetPlayedtime() { return m_playedtime; };
    void CalcStat(uint32 t);
    void CalcRating(uint32 t);
    void RecalcAllRatings();
    void RegenerateMana();
    void RegenerateHealth();
    void RegenerateEnergy();
    void LooseRage();
    PvPArea * currentPVPArea;
    uint32 SoulStone;
    map<uint32, WeaponModifier> damagedone;
    map<uint32, WeaponModifier> tocritchance;
    uint32 Seal;
    uint32 judgespell;
    bool cannibalize;
    uint8 cannibalizeCount;
    int32 rageFromDamageDealt;

    // GameObject commands
    GameObject *m_GM_SelectedGO;
    
    void _Relocate(uint32 mapid,const LocationVector & v, bool sendpending, bool force_new_world);
    void AddItemsToWorld();
    void RemoveItemsFromWorld();

    bool bShouldHaveLootableOnCorpse;
    uint32 m_ShapeShifted;
    uint32 m_MountSpellId;
    bool bHasBindDialogOpen;
    void RemoveSpellsFromLine(uint32 skill_line);
    bool bGMTagOn;
    uint32 TrackingSpell;

    void _EventCharmAttack();
    void _Kick();
    void Kick(uint32 delay = 0);
    uint32 m_KickDelay;
    Creature * m_CurrentCharm;
    Transporter * m_CurrentTransporter;
    
    Object * GetSummonedObject () {return m_SummonedObject;};
    void SetSummonedObject (Object * t_SummonedObject) {m_SummonedObject = t_SummonedObject;};
    uint32 roll;
    std::set<LootRoll*> m_rolls;

    void AddCooldown(uint32 spell,uint32 time);
    void AddCategoryCooldown(uint32 cat, uint32 time);
    void AddGlobalCooldown(uint32 time);
    uint32 GetGlobalCooldown() { return GlobalCooldown; }
    void SetGlobalCooldown(uint32 time) { GlobalCooldown = time; }

    void ClearCooldownsOnLine(uint32 skill_line, uint32 called_from);
    void ClearCooldownForSpell(uint32 spell_id);

    bool bProcessPending;
    Mutex _bufferS;

    void PushUpdateData(ByteBuffer *data, uint32 updatecount);
    void PushOutOfRange(const WoWGuid & guid);
    void ProcessPendingUpdates();
    bool __fastcall CompressAndSendUpdateBuffer(uint32 size, const uint8* update_buffer);
    void ClearAllPendingUpdates();

    bool CanFinishQuest(Quest* qst);

    uint32 GetArmorProficiency() { return armor_proficiency; }
    uint32 GetWeaponProficiency() { return weapon_proficiency; }

    void ResetHeartbeatCoords();
    float _lastHeartbeatX;
    float _lastHeartbeatY;
    float _lastHeartbeatZ;
    uint32 _lastHeartbeatTime;
    void AddSplinePacket(uint64 guid, ByteBuffer* packet);
    ByteBuffer* GetAndRemoveSplinePacket(uint64 guid);
    void ClearSplinePackets();

    bool ExitInstance();
    void SaveEntryPoint();

    bool CooldownCheat;
    bool CastTimeCheat;
    bool GodModeCheat;
    bool PowerCheat;
    bool FlyCheat;

    void ZoneUpdate(uint32 ZoneId);
    inline uint32 GetAreaID() { return m_AreaID; }
    void SetAreaID(uint32 area) { m_AreaID = area; }

    SpellSet mSpells;
    SpellSet mDeletedSpells;
    
    std::string Lfgcomment;
    uint16 LfgDungeonId[3];
    uint8 LfgType[3];
    bool m_Autojoin;
    bool m_AutoAddMem;
    void StopMirrorTimer(uint32 Type);
    BattlegroundScore* m_bgScore;

    void UpdateChanceFields();

    //Honor Variables
    uint32 m_honorToday;
    uint32 m_honorYesterday;
    
    uint32 m_honorPoints;
    uint32 m_killsToday;
    uint32 m_killsYesterday;
    uint32 m_killsLifetime;

    uint32 m_lastSeenWeather;
    set<Object*> m_visibleFarsightObjects;

    void EventTeleport(uint32 mapid, float x, float y, float z);
    void ApplyLevelInfo(LevelInfo* Info, uint32 Level);

    void BroadcastMessage(const char* Format, ...);
    map<uint32, set<uint32> > SummonSpells;
    map<uint32, PetSpellMap*> PetSpells;

    void AddSummonSpell(uint32 Entry, uint32 SpellID);
    void RemoveSummonSpell(uint32 Entry, uint32 SpellID);
    set<uint32>* GetSummonSpells(uint32 Entry);
    LockedQueue<WorldPacket*> delayedPackets;
    set<Player *> gmTargets;
    uint32 m_UnderwaterMaxTime;
    uint32 m_UnderwaterLastDmg;
    inline void setMyCorpse(Corpse * corpse) { myCorpse = corpse; }
    inline Corpse * getMyCorpse() { return myCorpse; }
    bool bCorpseCreateable;
    uint32 m_resurrectHealth, m_resurrectMana;
    bool blinked;
    uint16 m_speedhackChances;
    uint32 m_explorationTimer;

    // DBC stuff
    CharRaceEntry * myRace;
    CharClassEntry * myClass;

    void RemoveLanguages();
    void AddLanguages();

    Unit * linkTarget;
    bool stack_cheat;

    bool SafeTeleport(uint32 MapID, uint32 InstanceID, float X, float Y, float Z, float O);
    bool SafeTeleport(uint32 MapID, uint32 InstanceID, const LocationVector & vec);
    Guild * myGuild;

    /*****************
      PVP Stuff
    ******************/
    uint32 m_pvpTimer;
    
    //! Is PVP flagged?
    inline bool IsPvPFlagged() { return HasFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_PVP); }
    inline void SetPvPFlag()
    {
        // reset the timer as well..
        StopPvPTimer();
        if(!HasFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_PVP))
            SetFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_PVP);
    }

    //! Removal
    inline void RemovePvPFlag()
    {
        StopPvPTimer();
        if(HasFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_PVP))
            RemoveFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_PVP);            
    }

    //! Do this on /pvp off
    inline void ResetPvPTimer();

    //! Stop the timer for pvp off
    inline void StopPvPTimer() { m_pvpTimer = 0; }
    
    //! Called at login to add the honorless buff, etc.
    void LoginPvPSetup();

    //! Update our pvp area (called when zone changes)
    void UpdatePvPArea();

    //! PvP Toggle (called on /pvp)
    void PvPToggle();

    bool CanCastDueToCooldown(SpellEntry * spellid);

protected:
    void _SetCreateBits(UpdateMask *updateMask, Player *target) const;
    void _SetUpdateBits(UpdateMask *updateMask, Player *target) const;
    void _SetVisibleBits(UpdateMask *updateMask, Player *target) const;

/* Update system components */

    ByteBuffer bUpdateBuffer;
    uint32 mUpdateCount;
    uint32 mOutOfRangeIdCount;
    ByteBuffer mOutOfRangeIds;
    SplineMap _splineMap;

/* End update system */

    void _LoadTutorials();
    void _SaveTutorials();
    void _SaveInventory(bool firstsave);
    void _LoadBagInventory(uint32 playerguid, uint8 bagslot);
    void _SaveActions();
    void _SaveQuestLogEntry();
    void _LoadQuestLogEntry();
    void _SaveAuras();
    void _SaveBids();
    void _LoadBids();
    void _SaveAuctions();
    void _LoadInventory();
    void _LoadInventoryLight();
    void _LoadActions();
    //void _LoadAuras();
    void _SaveFinishedQuests();
    void _LoadFinishedQuests();
    // DK
    void _LoadGuild();
    void _LoadCharters();
    void _LoadPet();
    void _LoadPetNo();
    void _LoadPetSpells();
    void _SavePet();
    void _SavePetSpells();

    //capt
    void _SaveItemCooldown();
    void _LoadItemCooldown();


    void _ApplyItemMods(Item *item, int8 slot,bool apply);
    void _RemoveAllItemMods();
    void _ApplyAllItemMods();

    void _EventAttack(bool offhand);
    void _EventExploration();

    void UpdateCooldowns();

    // Cooldown stuff
    std::map<uint32,uint32>SpellCooldownMap;
    std::map<uint32,uint32>SpellCooldownCategoryMap;
    ItemCooldownSet m_itemcooldown;
    uint32 GlobalCooldown;

    /************************************************************************/
    /* Trade                                                                */
    /************************************************************************/
    Item* mTradeItems[8];
   
    uint32 mTradeGold;
    Player* mTradeTarget;
    uint32 mTradeStatus;

    PlayerCreateInfo *info;

    uint32 m_AttackMsgTimer;    // "too far away" and "wrong facing" timer
    bool m_attacking;
    
    uint64 m_lootGuid;

    //uint32 eating;

    std::string m_name;    // max 21 character name

    uint32 m_Tutorials[8];
    //uint32 m_mod_damage_done[7];
 
    uint64 m_invitersGuid; // It is guild inviters guid ,0 when its not used
    bool m_signedCharter;

    // Character Ban
    uint32 m_banned;
    string m_banreason;

    uint32 m_AreaID;

    Pet* m_Summon;
    uint32 m_PetNumberMax;
    std::map<uint32, PlayerPet*> m_Pets;

    //Taxi
    bool m_onTaxi;
    uint32 m_taximask[8];
    TaxiPath* m_CurrentTaxiPath;
    float m_taxi_pos_x;
    float m_taxi_pos_y;
    float m_taxi_pos_z;
    uint32 m_taxi_ride_time;

    float m_bind_pos_x;
    float m_bind_pos_y;
    float m_bind_pos_z;
    uint32 m_bind_mapid;
    uint32 m_bind_zoneid;
    std::list<ItemSet> m_itemsets;
    //Duel
    uint32 m_duelCountdownTimer;
    uint8 m_duelStatus;
    uint8 m_duelState;

    // Rested State Stuff
    uint32 m_timeLogoff;
    // Played time
    uint32 m_playedtime[3];

    uint8 m_isResting;
    uint8 m_restState;
    uint32 m_restAmount;

    //combat mods
    float m_blockfromspell;
    float m_blockfromspellPCT;
    float m_critfromspell;
    float m_spellcritfromspell;
    float m_dodgefromspell;
    float m_parryfromspell;
    float m_hitfromspell;
    float m_hitfrommeleespell;

    //stats mods
    uint32 m_healthfromspell;
    uint32 m_manafromspell;
    uint32 m_healthfromitems;
    uint32 m_manafromitems;
    
    uint32  armor_proficiency;
    uint32  weapon_proficiency;

    // Talents
    uint32 m_talentresettimes;

    // STATUS
    uint8 m_status;

    // guid of current target
    uint64 m_curTarget;

    // guid of current selection
    uint64 m_curSelection;

    //Quests
    QuestLogEntry* m_questlog[25];
    std::set<uint32> m_removequests;
    std::set<uint32> m_finishedQuests;

    // Raid
    uint8 m_targetIcon;

    std::list<struct aurloads> m_aurloads;
    // items the player has bid on
    std::list<bidentry*> m_bids;

    //Player Action Bar
    std::list<struct actions> m_actions;

    // Player Reputation
    std::map<uint32, Reputation> m_reputation;
    SaveSet m_changedreputation;
    SaveSet m_deletereputation;
    SaveSet m_addreputation;

    // Pointer to this char's game client
    WorldSession *m_session;

    // Channels
    std::list<Channel*> m_channels;

    // Visible objects
    std::set<Object*> m_visibleObjects;

    // Groups/Raids
    Group* m_Group;
    SubGroup* m_SubGroup;
    uint64 m_GroupInviter;
    uint8 m_StableSlotCount;

    std::set<uint32> m_QuestGOInProgress;

    // Fishing related
    Object *m_SummonedObject;

    // loot related
    Object *m_currentLoot;

    ItemInterface *m_ItemInterface;
    MapMgr * source;
    Corpse * myCorpse;
    uint32 m_cooldownTimer;

    void JumpToEndTaxiNode(TaxiPath * path);
};

#endif

