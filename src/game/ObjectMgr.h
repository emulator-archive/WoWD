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

#ifndef _OBJECTMGR_H
#define _OBJECTMGR_H

enum SpellSpecialType
{
    NOTHING=0,
    SEAL=1,
    BLESSING=2,
    CURSE=3,
    ASPECT=4,
    STING=5,
};

struct SpellExtraInfo
{
    uint32 Id;
    uint32 specialtype;
    uint32 enchantableslots;
    uint32 ExtraFlags;
};

struct GM_Ticket
{
    uint64 guid;
    std::string name;
    uint32 level;
    uint32 type;
    float posX;
    float posY;
    float posZ;
    std::string message;
    uint32 timestamp;
};

struct ThreatToSpellId
{
    uint32 spellId;
    int32 mod;
};

struct TrainerSpellOverride
{
    uint32 SpellID;
    uint32 Cost;
    uint32 RequiredSpell;
    uint32 DeleteSpell;
    uint32 RequiredSkill;
    uint32 RequiredSkillValue;
    uint8 RequiredLevel;
    int32 RequiredClass;
};

struct FishingZoneEntry
{
    uint32 MinSkill;
    uint32 MaxSkill;
};

struct ItemPage
{
    std::string text;
    uint32 next_page;
};

struct SpellReplacement
{
    uint32 count;
    uint32 *spells;
};

class Group;

struct GossipMenuItem
{
    uint32 Id;
    uint32 IntId;
    uint16 Icon;
    string Text;
};

struct SpellEntry;
struct TrainerSpell
{

    uint32 SpellID;
    SpellEntry* pSpell;

    uint32 SpellRank;
    
    uint32 RequiredSpell;
    uint32 DeleteSpell;

    uint32 RequiredSkillLine;
    uint32 RequiredSkillLineValue;
    uint32 TeachingLine;
    uint32 IsProfession;

    uint32 Cost;
    uint32 RequiredLevel;

    uint32 SpellType;
    uint32 TeachingSpellID;
    SpellEntry* pTrainingSpell;

    int32 RequiredClass;
};

struct Trainer
{
    uint32 SpellCount;
    TrainerSpell** SpellList;

    char* TalkMessage;
    uint32 TrainerType;
    uint32 RequiredClass;
};

struct LevelInfo
{
    uint32 HP;
    uint32 Mana;
    uint32 Stat[5];
    uint32 XPToNextLevel;
};

class WOWD_SERVER_DECL GossipMenu
{
public:
    GossipMenu(uint64 Creature_Guid, uint32 Text_Id);
    void AddItem(GossipMenuItem* GossipItem);
    void AddItem(uint16 Icon, const char* Text, int32 Id = -1);
    void BuildPacket(WorldPacket& Packet);
    void SendTo(Player* Plr);
    GossipMenuItem GetItem(uint32 Id);
    inline void SetTextID(uint32 TID) { TextId = TID; }

protected:
    uint32 TextId;
    uint64 CreatureGuid;
    std::vector<GossipMenuItem> Menu;
};

#define MAX_CONTINENTS 500

class WOWD_SERVER_DECL ObjectMgr : public Singleton < ObjectMgr >, public EventableObject
{
public:
    ObjectMgr();
    ~ObjectMgr();

    // other objects
    typedef std::set< Group * > GroupSet;
    typedef HM_NAMESPACE::hash_map<uint64, Item*> ItemMap;
    typedef HM_NAMESPACE::hash_map<uint32, GossipText*> GossipTextMap;
    typedef HM_NAMESPACE::hash_map<uint32, CorpseData*> CorpseCollectorMap;
    typedef HM_NAMESPACE::hash_map<uint32, GraveyardTeleport*> GraveyardMap;
    typedef HM_NAMESPACE::hash_map<uint32, PlayerInfo*> PlayerNameMap;
    typedef HM_NAMESPACE::hash_map<uint32, CreatureInfo*> CreatureNameMap;
    typedef HM_NAMESPACE::hash_map<uint32, GameObjectInfo*> GameObjectNameMap;
    typedef HM_NAMESPACE::hash_map<uint32, CreatureSpawnTemplate*> CreatureSpawnTemplateMap;
    typedef HM_NAMESPACE::hash_map<uint32, ItemPrototype*> ItemPrototypeMap;
    typedef HM_NAMESPACE::hash_map<uint32, AuctionEntry*> AuctionEntryMap;
    typedef HM_NAMESPACE::hash_map<uint32, PlayerCreateInfo*> PlayerCreateInfoMap;
    typedef HM_NAMESPACE::hash_map<uint32, Guild*> GuildMap;
    typedef HM_NAMESPACE::hash_map<uint32, TeleportCoords*> TeleportMap;
    typedef HM_NAMESPACE::hash_map<uint32, skilllinespell*> SLMap;
    typedef HM_NAMESPACE::hash_map<uint32, std::vector<CreatureItem>*> VendorMap;
    typedef HM_NAMESPACE::hash_map<uint32, PvPArea*> PvPAreaMap;
    typedef HM_NAMESPACE::hash_map<uint32, FishingZoneEntry*> FishingZoneMap;
    typedef HM_NAMESPACE::hash_map<uint32, ItemPage*> ItemPageMap;
    typedef HM_NAMESPACE::hash_map<uint32, SpellExtraInfo*> SpellExtraInfoMap;
    typedef HM_NAMESPACE::hash_map<uint32, Creature*> CreatureSqlIdMap;
    
    typedef HM_NAMESPACE::hash_map<uint32, Trainer*> TrainerMap;
    typedef HM_NAMESPACE::hash_map<uint32, std::vector<TrainerSpell*> > TrainerSpellMap;
    typedef std::map<uint32, LevelInfo*> LevelMap;
    typedef std::map<pair<uint32, uint32>, LevelMap* > LevelInfoMap;

    typedef std::map<uint32, std::list<ItemPrototype*>* > ItemSetContentMap;
    typedef std::map<uint32, uint32> NpcToGossipTextMap;
    typedef std::map<uint32, set<SpellEntry*> > PetDefaultSpellMap;
    typedef std::map<uint32, uint32> PetSpellCooldownMap;
    typedef std::map<uint32, SpellEntry*> TotemSpellMap;
    
    std::list<GM_Ticket*> GM_TicketList;
    std::list<Charter*> Guild_CharterList;
    TotemSpellMap m_totemSpells;
    std::list<ThreatToSpellId*> threatToSpells;

    Player* GetPlayer(const char* name, bool caseSensitive = true);
    Player* GetPlayer(uint64 guid);

    Item * CreateItem(uint32 entry,Player * owner);
    Item * LoadItem(uint32 guid);
    Creature* GetCreature(uint64 guid);

    void LoadSpellExtraData();
    SpellExtraInfo* GetSpellExtraData(uint32 id);

    //map entry extra data for each spellid
    SpellExtraInfoMap mExtraSpellData;

    //Creature Templates
    void AddCreatureSpawnTemplate(CreatureSpawnTemplate *ct);
    CreatureSpawnTemplate* GetCreatureSpawnTemplate(uint32 entryid) const;
    bool RemoveCreatureSpawnTemplate(uint32 entryid);
    
    // Groups
    Group * GetGroupByLeader(Player *pPlayer);
    inline uint32 GenerateGroupId()
    {
        m_hiGroupId++;
        return m_hiGroupId;
    }
    void AddGroup(Group* group) { mGroupSet.insert( group ); }
    void RemoveGroup(Group* group) { mGroupSet.erase( group ); }

    void AddAuction(AuctionEntry *ah);
    AuctionEntry* GetAuction(uint32 id) const;
    bool RemoveAuction(uint32 id);

    // player names
    void AddPlayerInfo(PlayerInfo *pn);
    PlayerInfo *GetPlayerInfo( uint64 guid );
    PlayerInfo *GetPlayerInfoByName(const char*name);

    // creature names
    uint32 AddCreatureName(const char* name);
    uint32 AddCreatureName(const char* name, uint32 displayid);
    void AddCreatureName(uint32 id, const char* name);
    void AddCreatureName(uint32 id, const char* name, uint32 displayid);
    void AddCreatureName(CreatureInfo *cinfo);
    CreatureInfo *GetCreatureName( uint32 id );

    //Sub names
    uint32 AddCreatureSubName(const char* name, const char* subname, uint32 displayid);

    // item prototypes
    ItemPrototype* GetItemPrototype(uint32 id) const;
    void AddItemPrototype(ItemPrototype *itemProto);
    Item* GetAItem(uint64 id);
    void AddAItem(Item* it);
    bool RemoveAItem(uint64 id);
    AuctionEntryMap::iterator GetAuctionsBegin() {return mAuctions.begin();}
    AuctionEntryMap::iterator GetAuctionsEnd() {return mAuctions.end();}

    // Create Player Info
    void AddPlayerCreateInfo(PlayerCreateInfo *playerCreate);
    PlayerCreateInfo* GetPlayerCreateInfo(uint8 race, uint8 class_) const;

  

    // DK:Guild
    void AddGuild(Guild *pGuild);
    uint32 GetTotalGuildCount();
    bool RemoveGuild(uint32 guildId);
    Guild* GetGuild(uint32 guildId);  
    Guild* GetGuildByLeaderGuid(uint64 leaderGuid);  
    Guild* GetGuildByGuildName(std::string guildName);

    // DK:Charter
    void AddCharter(Charter* gc);
    void DeleteCharter(uint64 leaderGuid);
    Charter *GetGuildCharter(uint64 leaderGuid);
    Charter *GetGuildCharterByCharterGuid(uint32 itemGuid);
    void DeletePlayerSignature(uint64);
    bool SignedCharter(uint64);
    void SaveCharter(Charter *gc);

    //Corpse Stuff
    Corpse *GetCorpseByOwner(Player *pOwner);
    void CorpseCollectorLoad();
    void CorpseCollectorUnload();
    void DespawnCorpse(uint64 Guid);
    void CorpseAddEventDespawn(Corpse *pCorpse);
    void DelinkPlayerCorpses(Player *pOwner);

    //Gossip Stuff
    void AddGossipText(GossipText *pGText);
    GossipText *GetGossipText(uint32 ID);
    uint32 GetGossipTextForNpc(uint32 ID);

    //Death stuff
    void AddGraveyard(GraveyardTeleport *pgrave);
    GraveyardMap::iterator GetGraveyardListBegin() { return mGraveyards.begin(); }
    GraveyardMap::iterator GetGraveyardListEnd() { return mGraveyards.end(); }

    //Teleport Stuff
    void AddTeleportCoords(TeleportCoords* TC);
    TeleportCoords* GetTeleportCoords(uint32 id) const;

    // Gm Tickets
    void AddGMTicket(GM_Ticket *ticket);
    void remGMTicket(uint64 guid);
    GM_Ticket* GetGMTicket(uint64 guid);

    //GameObject names
    GameObjectInfo *GetGameObjectName(uint32 ID);
    GameObjectInfo *GetGameObjectNameNULL(uint32 ID);
    void AddGameObjectName( GameObjectInfo *goinfo);

    skilllinespell* GetSpellSkill(uint32 id);

    //PVP
    void AddPvPArea(PvPArea* pvparea);
    PvPArea* GetPvPArea(uint32 AreaId);

    //Vendors
    std::vector<CreatureItem> *GetVendorList(uint32 entry);
    void SetVendorList(uint32 Entry, std::vector<CreatureItem>* list_);

    //Totem
    SpellEntry* GetTotemSpell(uint32 spellId);

    // AI Threat by SpellId
    int32 GetAIThreatToSpellId(uint32 spellId);

    // Fishing
    FishingZoneEntry* GetFishingZone(uint32 zoneid);
    inline uint32 GetFishingZoneMinSkill(uint32 zoneid) { return GetFishingZone(zoneid)->MinSkill; };
    inline uint32 GetFishingZoneMaxSkill(uint32 zoneid) { return GetFishingZone(zoneid)->MaxSkill; };

    std::list<ItemPrototype*>* GetListForItemSet(uint32 setid);

    inline void AddItemPage(uint32 id, ItemPage* page) { mItemPages[id] = page; }
    inline ItemPage* GetItemPage(uint32 id)
    {
        ItemPageMap::iterator itr = mItemPages.find(id);
        return (itr != mItemPages.end()) ? itr->second : NULL;
    }

    Creature * GetCreatureBySqlId(uint32 Sql_Id);
    void SetCreatureBySqlId(uint32 Sql_Id, Creature * pCreature);

    // Serialization
    Creature* LoadCreature(uint32 guid, int32 instanceid);
    Creature* LoadCreature(CreatureTemplate *t, int32 instanceid);
    GameObject* LoadGameObject(uint32 guid, int32 instanceid);
    GameObject* LoadGameObject(GameObjectTemplate *t, int32 instanceid);
    void LoadQuests();
    void LoadPlayersInfo();
    void LoadCreatureNames();
    void LoadGameObjectNames();
    void SaveCreatureNames();
    void LoadItemPrototypes();
    void LoadPlayerCreateInfo();
    void LoadGuilds();
    void LoadCharters();
    Corpse* LoadCorpse(uint32 guid);
    Corpse* LoadCorpse(CorpseTemplate *t);
    void LoadGossipText();
    void LoadGraveyards();
    void LoadTeleportCoords();
    void LoadGMTickets();
    void SaveGMTicket(uint64 guid);
    void LoadAuctions();
    void LoadAuctionItems();
    void LoadCreatureSpawnTemplates();
    void LoadSpellSkills();
    void LoadPvPAreas();
    void LoadVendors();
    void LoadTotemSpells();
    void LoadAIThreatToSpellId();
    void LoadFishingZones();
    void LoadItemPages();

    void SetHighestGuids();
    uint32 GenerateLowGuid(uint32 guidhigh);
    uint32 GenerateAuctionID();
    uint32 GenerateMailID();
    Unit* GetUnit(uint64 guid);
    Unit* GetUnit(WoWGuid guid);

    void LoadTransporters();
    void ProcessGameobjectQuests();
    void CleanupObjectHolders();
    template <class T>
    void CleanupObjectHolder();

    void GenerateTrainerSpells();
    bool AddTrainerSpell(uint32 entry, SpellEntry *pSpell);
    void LoadTrainers();
    Trainer* GetTrainer(uint32 Entry);

    void CreateGossipMenuForPlayer(GossipMenu** Location, uint64 Guid, uint32 TextID, Player* Plr); 
    void ReloadTables();

    void ReloadCreatureNames();
    void ReloadCreatureSpawnTemplates();
    void ReloadItems();
    void ReloadQuests();
    void ReloadGameObjects();

    LevelInfo * GetLevelInfo(uint32 Race, uint32 Class, uint32 Level);
    void GenerateLevelUpInfo();
    void LoadDefaultPetSpells();
    set<SpellEntry*>* GetDefaultPetSpells(uint32 Entry);
    uint32 GetPetSpellCooldown(uint32 SpellId);
    void LoadPetSpellCooldowns();
    void LoadSpellFixes();

protected:
    uint32 m_auctionid;
    uint32 m_mailid;
    // highest GUIDs, used for creating new objects
    Mutex m_guidGenMutex;
    uint32 m_hiCharGuid;
    uint32 m_hiCreatureGuid;
    uint32 m_hiItemGuid;
    uint32 m_hiGoGuid;
    uint32 m_hiDoGuid;
    uint32 m_hiNameGuid;
    uint32 m_hiGoNameGuid;
    uint32 m_hiGroupId;
    
    HM_NAMESPACE::hash_map<uint32,PlayerInfo> m_playersinfo;

    ///// Object Tables ////
    // These tables are modified as creatures are created and destroyed in the world

    // Group List
    GroupSet            mGroupSet;

    // Map of all item types in the game
    ItemMap             mItems;

    // Map of auction item intances
    ItemMap                mAitems;

    // Map of mailed itesm
    ItemMap                mMitems;

    // Map of all item types in the game
    ItemPrototypeMap    mItemPrototypes;

    // Map of auctioned items
    AuctionEntryMap        mAuctions;

        // map entry to a creature name
    CreatureNameMap     mCreatureNames;

    //Map entry to a gameobject query name
    GameObjectNameMap   mGameObjectNames;

    // map entry to a creature template
    CreatureSpawnTemplateMap mCreatureSpawnTemplates;

    // Map of all starting infos needed for player creation
    PlayerCreateInfoMap mPlayerCreateInfo;

    // DK: Map of all Guild's
    GuildMap mGuild;

    // Map of all vendor goods
    VendorMap mVendors;

    // Maps for Gossip stuff
    GossipTextMap       mGossipText;
    NpcToGossipTextMap  mNpcToGossipText;

    // Death Stuff
    GraveyardMap        mGraveyards;

    // Teleport Stuff
    TeleportMap            mTeleports;

    SLMap                mSpellSkills;

    //PVP Stuff
    PvPAreaMap            mPvPAreas;

    // cached fishing zones
    FishingZoneMap         mFishingZones;
    ItemPageMap            mItemPages;

    //Corpse Collector
    CorpseCollectorMap mCorpseCollector;

    ItemSetContentMap mItemSets;

    TrainerSpellMap mNormalSpells;
    TrainerSpellMap mPetSpells;

    TrainerMap mTrainers;
    LevelInfoMap mLevelInfo;
    PetDefaultSpellMap mDefaultPetSpells;
    PetSpellCooldownMap mPetSpellCooldowns;
    CreatureSqlIdMap mCreatureSqlIds;
    Mutex CreatureSqlIdMapMutex;
};


#define objmgr ObjectMgr::getSingleton()

void SetProgressBar(int, int, const char*);
void ClearProgressBar();

#endif
