#ifndef __SPELLSTORE_H
#define __SPELLSTORE_H

#include "Common.h"
#include "DataStore.h"
#include "Timer.h"

struct ItemSetEntry
{
    uint32 id;                  //1
    uint32 name;                //2
    uint32 unused_shit[7];      //3 - 9
    uint32 flag;                //10 constant
    uint32 itemid[8];           //11 - 18
    uint32 more_unused_shit[9]; //19 - 27
    uint32 SpellID[8];          //28 - 35
    uint32 itemscount[8];       //36 - 43
    uint32 RequiredSkillID;     //44
    uint32 RequiredSkillAmt;    //45
};

struct Lock
{
	uint32 Id;
	uint32 locktype[5]; // 0 - no lock, 1 - item needed for lock, 2 - min lockping skill needed
	uint32 unk1[3];
	uint32 lockmisc[5]; // if type is 1 here is a item to unlock, else is unknow for now
	uint32 unk2[3];
	uint32 minlockskill[5]; // min skill in lockpiking to unlock.
};

struct emoteentry
{
    uint32 Id;
    uint32 name;
    uint32 textid;
    uint32 textid2;
    uint32 textid3;
    uint32 textid4;
    uint32 unk1;
    uint32 textid5;
    uint32 unk2;
    uint32 textid6;
    uint32 unk3;
    uint32 unk4;
    uint32 unk5;
    uint32 unk6;
    uint32 unk7;
    uint32 unk8;
    uint32 unk9;
    uint32 unk10;
    uint32 unk11;
};

struct skilllinespell
{
    uint32 Id;
    uint32 skilline;
    uint32 spell;
    uint32 unk1;
    uint32 unk2;
    uint32 unk3;
    uint32 unk4;
    uint32 unk5;
    uint32 next;
	uint32 minrank;
    uint32 grey;
    uint32 green;
    uint32 unk10;
    uint32 unk11;
    uint32 unk12;
};
struct EnchantEntry
{
    uint32 Id;
    uint32 type;
    uint32 unk1;
    uint32 unk2;
    uint32 min;
    uint32 unk3;
    uint32 unk4;
    uint32 max;
    uint32 unk5;
    uint32 unk6;
    uint32 spell[2];
    uint32 ukn;
    uint32 Name;
    uint32 NameAlt1;
    uint32 NameAlt2;
    uint32 NameAlt3;
    uint32 NameAlt4;
    uint32 NameAlt5;
    uint32 NameAlt6;
    uint32 NameAlt7;
    uint32 NameFlags;
    uint32 visual;
    uint32 EnchantGroups;

};
struct skilllineentry
{
    uint32 id;
    uint32 type;
    uint32 unk1;
    uint32 Name;
    uint32 NameAlt1;
    uint32 NameAlt2;
    uint32 NameAlt3;
    uint32 NameAlt4;
    uint32 NameAlt5;
    uint32 NameAlt6;
    uint32 NameAlt7;
    uint32 NameFlags;
    uint32 Description;
    uint32 DescriptionAlt1;
    uint32 DescriptionAlt2;
    uint32 DescriptionAlt3;
    uint32 DescriptionAlt4;
    uint32 DescriptionAlt5;
    uint32 DescriptionAlt6;
    uint32 DescriptionAlt7;
    uint32 DescriptionFlags;
    uint32 unk2;

};

// Struct for the entry in Spell.dbc
struct SpellEntry
{
    uint32 Id;                              //1
    uint32 School;                          //2
    uint32 Category;                        //3
    uint32 field4;                          //4 something like spelltype 0: general spells 1: Pet spells 2: Disguise / transormation spells 3: enchantment spells
    uint32 DispelType;                      //5
    uint32 MechanicsType;                   //6
    uint32 Attributes;                      //7
    uint32 AttributesEx;                    //8
    uint32 Flags3;                          //9
    uint32 field10;                         //10 // Flags to
    uint32 field11;                         //11 // Flags....
    uint32 unk201_1;                        //12 // Flags 2.0.1 unknown one
    uint32 UNK13;                           //13 // Flags BitMask for shapeshift spells
    uint32 UNK14;                           //14 // Flags BitMask for which shapeshift forms this spell can NOT be used in.
    uint32 Targets;                         //15 - N / M
    uint32 TargetCreatureType;              //16
    uint32 RequiresSpellFocus;              //17
    uint32 CasterAuraState;                 //18
    uint32 TargetAuraState;                 //19
    uint32 unk201_2;                        //20 2.0.1 unknown two
    uint32 unk201_3;                        //21 2.0.1 unknown three
    uint32 CastingTimeIndex;                //22
    uint32 RecoveryTime;                    //23
    uint32 CategoryRecoveryTime;            //24 recoverytime
    uint32 InterruptFlags;                  //25
    uint32 AuraInterruptFlags;              //26
    uint32 ChannelInterruptFlags;           //27
    uint32 procFlags;                       //28
    uint32 procChance;                      //29
    uint32 procCharges;                     //30
    uint32 maxLevel;                        //31
    uint32 baseLevel;                       //32
    uint32 spellLevel;                      //33
    uint32 DurationIndex;                   //34
    uint32 powerType;                       //35
    uint32 manaCost;                        //36
    uint32 manaCostPerlevel;                //37
    uint32 manaPerSecond;                   //38
    uint32 manaPerSecondPerLevel;           //39
    uint32 rangeIndex;                      //40
    float  speed;                           //41
    uint32 modalNextSpell;                  //42
    uint32 maxstack;                        //43
    uint32 Totem[2];                        //44 - 45
    uint32 Reagent[8];                      //46 - 53
    uint32 ReagentCount[8];                 //54 - 61
    uint32 EquippedItemClass;               //62
    uint32 EquippedItemSubClass;            //63
    uint32 RequiredItemFlags;               //64
    uint32 Effect[3];                       //65 - 67
    uint32 EffectDieSides[3];               //68 - 70
    uint32 EffectBaseDice[3];               //71 - 73
    float  EffectDicePerLevel[3];           //74 - 76
    float  EffectRealPointsPerLevel[3];     //77 - 79
    int32  EffectBasePoints[3];             //80 - 82
    int32  EffectMechanic[3];               //83 - 85       Related to SpellMechanic.dbc
    uint32 EffectImplicitTargetA[3];        //86 - 88
    uint32 EffectImplicitTargetB[3];        //89 - 91
    uint32 EffectRadiusIndex[3];            //92 - 94
    uint32 EffectApplyAuraName[3];          //95 - 97
    uint32 EffectAmplitude[3];              //98 - 100
    float  Effectunknown[3];                //101 - 103     This value is the $ value from description
    uint32 EffectChainTarget[3];            //104 - 106
    uint32 EffectSpellGroupRelation[3];     //107 - 109     Not sure maybe we should rename it. its the relation to field: SpellGroupType
    uint32 EffectMiscValue[3];              //110 - 112
    uint32 EffectTriggerSpell[3];           //113 - 115
    float  EffectPointsPerComboPoint[3];    //116 - 118
    uint32 SpellVisual;                     //119
    uint32 field114;                        //120
    uint32 dummy;                           //121
    uint32 CoSpell;                         //122   activeIconID;
    uint32 spellPriority;                   //123
    uint32 Name;                            //124
    uint32 NameAlt1;                        //125
    uint32 NameAlt2;                        //126
    uint32 NameAlt3;                        //127
    uint32 NameAlt4;                        //128
    uint32 NameAlt5;                        //129
    uint32 NameAlt6;                        //130
    uint32 NameAlt7;                        //131
    uint32 NameFlags;                       //132
    uint32 Rank;                            //133
    uint32 RankAlt1;                        //134
    uint32 RankAlt2;                        //135
    uint32 RankAlt3;                        //136
    uint32 RankAlt4;                        //137
    uint32 RankAlt5;                        //138
    uint32 RankAlt6;                        //139
    uint32 RankAlt7;                        //140
    uint32 RankFlags;                       //141
    uint32 Description;                     //142
    uint32 DescriptionAlt1;                 //143
    uint32 DescriptionAlt2;                 //144
    uint32 DescriptionAlt3;                 //145
    uint32 DescriptionAlt4;                 //146
    uint32 DescriptionAlt5;                 //147
    uint32 DescriptionAlt6;                 //148
    uint32 DescriptionAlt7;                 //149
    uint32 DescriptionFlags;                //150
    uint32 BuffDescription;                 //151
    uint32 BuffDescriptionAlt1;             //152
    uint32 BuffDescriptionAlt2;             //153
    uint32 BuffDescriptionAlt3;             //154
    uint32 BuffDescriptionAlt4;             //155
    uint32 BuffDescriptionAlt5;             //156
    uint32 BuffDescriptionAlt6;             //157
    uint32 BuffDescriptionAlt7;             //158
    uint32 buffdescflags;                   //159
    uint32 ManaCostPercentage;              //160
    uint32 unkflags;                        //161 
    uint32 StartRecoveryTime;               //162
    uint32 StartRecoveryCategory;           //163
    uint32 SpellFamilyName;                 //164
    uint32 SpellGroupType;                  //165   flags 
    uint32 unkne;                           //166   flags hackwow=shit 
    uint32 MaxTargets;                      //167 
    uint32 Spell_Dmg_Type;                  //168   dmg_class Integer      0=None, 1=Magic, 2=Melee, 3=Ranged
    uint32 FG;                              //169   0,1,2 related to Spell_Dmg_Type I think
    int32 FH;                               //170   related to paladin aura's 
    float dmg_multiplier[3];                //171 - 173   if the name is correct I dono
    uint32 FL;                              //174   only one spellid:6994 has this value = 369
    uint32 FM;                              //175   only one spellid:6994 has this value = 4
    //uint32 FN;                              //176   only one spellid:26869  has this flag = 1

    // soz guys, gotta use these 3 vars :p
    //uint32 unk201_4; // these are related to creating a item through a spell
    //uint32 unk201_5;
    //uint32 unk201_6; // related to custom spells, summon spell quest related spells
    
    // custom shit
    uint32 buffType;                        //177

    // this protects players from having >1 rank of a spell
    uint32 RankNumber;                      //178
    uint32 NameHash;                        //179

    uint32 DiminishStatus;
};

struct ItemExtendedCostEntry
{
	uint32 costid;
	uint32 honor;
	uint32 arena;
	uint32 item[5];
	uint32 count[5];
};

struct TalentEntry
{
    uint32  TalentID;
    uint32  TalentTree;
    uint32  Row;
    uint32  Col;
    uint32  RankID[5];
    uint32  unk[4];
    uint32  DependsOn;
    uint32  unk1[2];
    uint32  DependsOnRank;
};
struct Trainerspell
{
    uint32 Id;
    uint32 skilline1;
    uint32 skilline2;
    uint32 skilline3;
    uint32 maxlvl;
    uint32 charclass;
};
struct SpellCastTime
{
    uint32 ID;
    uint32 CastTime;
    uint32 unk1;
    uint32 unk2;
};

struct SpellRadius
{
    uint32 ID;
    float Radius;
    float unk1;
    float Radius2;
};

struct SpellRange
{
    uint32 ID;
    float minRange;
    float maxRange;
    uint32 unks[18];
};

struct SpellDuration
{
    uint32 ID;
    uint32 Duration1;
    uint32 Duration2;
    uint32 Duration3;
};

struct RandomProps
{
    uint32 ID;
    uint32 name1;
    uint32 spells[3];
    uint32 unk1;
    uint32 unk2;
    uint32 name2;
    uint32 RankAlt1;
    uint32 RankAlt2;
    uint32 RankAlt3;
    uint32 RankAlt4;
    uint32 RankAlt5;
    uint32 RankAlt6;
    uint32 RankAlt7;
    uint32 RankFlags;

};

struct AreaTable
{
    uint32 AreaId;
    uint32 mapId;
    uint32 ZoneId;
    uint32 explorationFlag;
    uint32 AreaFlags;
    uint32 unk2;
    uint32 unk3;
    uint32 unk4;
    uint32 EXP;//not XP
    uint32 unk5;
    uint32 level;
    uint32 name;
    uint32 nameAlt1;
    uint32 nameAlt2;
    uint32 nameAlt3;
    uint32 nameAlt4;
    uint32 nameAlt5;
    uint32 nameAlt6;
    uint32 nameAlt7;
    uint32 nameFlags;
    uint32 factiongroup;
    uint32 unk7;
    uint32 unk8;
    uint32 unk9;
    uint32 unk10;
    uint32 unk11;
    uint32 unk12;
};

struct FactionTemplateDBC
{
    uint32 Id;
    uint32 FactionId;
    uint32 factionGroup;
    //uint32 fightSupport;
    //uint32 myFaction;
    uint32 myFaction;
    uint32 fightSupport;
    uint32 hostile;
    uint32 hostileR[4];
    uint32 friendlyR[4];
};

struct AuctionHouseDBC
{
    uint32 id;
    uint32 unk;
    uint32 fee;
    uint32 tax;
    char* name;
    char* nameAlt1;
    char* nameAlt2;
    char* nameAlt3;
    char* nameAlt4;
    char* nameAlt5;
    char* nameAlt6;
    char* nameAlt7;
    char* nameFlags;
};

struct FactionDBC
{
    int32 Id;
    int32 unk1;
    int32 FactionGroup;
    int32 unk2;
    int32 unk3;
    int32 unk4;
    int32 unk5;
    int32 unk6;
    int32 unk7;
    int32 unk8;
    int32 unk9;
    int32 unk10;
    int32 unk11;
    int32 unk12;
    int32 unk13;
    int32 unk14;
    int32 CapitalCity;
    int32 unk15;
    int32 factionGroupReputationFrame;
    char* name;
    char* nameAlt1;
    char* nameAlt2;
    char* nameAlt3;
    char* nameAlt4;
    char* nameAlt5;
    char* nameAlt6;
    char* nameAlt7;
    int32 nameFlags;
    char* description;
    char* descriptionAlt1;
    char* descriptionAlt2;
    char* descriptionAlt3;
    char* descriptionAlt4;
    char* descriptionAlt5;
    char* descriptionAlt6;
    char* descriptionAlt7;
    int32 descriptionFlags;
    int32 unk16;
};

struct WorldMapArea
{
    uint32    ID;
    uint32    mapId;
    uint32    zoneId;
    uint32    zoneName;
    float    y1, y2;
    float    x1, x2;
};

struct WorldMapOverlay
{
    uint32 ID;
    uint32 worldMapAreaID;
    uint32 areaTableID;
    uint32 unk1;
    uint32 unk2;
    uint32 unk3;
    uint32 unk4;
    uint32 unk5;
    uint32 name;
    uint32 areaW; 
    uint32 areaH; 
    uint32 areaX;  
    uint32 areaY;  
    uint32 y1;  
    uint32 x1;
    uint32 y2; 
    uint32 x2; 
};

struct DBCTaxiNode
{
    uint32 id;
    uint32 mapid;
    float x;
    float y;
    float z;
    uint32 unk1;
    uint32 unk2;
    uint32 name;
    uint32 unk3;
    uint32 unk4;
    uint32 unk5;
    uint32 unk6;
    uint32 unk7;
    uint32 unk8;
    uint32 horde_mount;
    uint32 alliance_mount;
};

struct DBCTaxiPath
{
    uint32 id;
    uint32 from;
    uint32 to;
    uint32 price;
};

struct DBCTaxiPathNode
{
    uint32 id;
    uint32 path;
    uint32 seq;
    uint32 mapid;
    float x;
    float y;
    float z;
    uint32 unk1;
    uint32 waittime;
};

struct GraveyardEntry
{
    uint32  someid;
    uint32  mapid;
    float   x;
    float   y;
    float   z;
    uint32  not_used[7];
    uint32  flag;
};

struct NameGenEntry
{
    int32  id;
    uint32  offsetindex;
    int32  unk1;
    bool   unk2;
};

struct CreatureSpellDataEntry
{
    uint32 id;
    uint32 Spells[3];
    uint32 PHSpell;
    uint32 Cooldowns[3];
    uint32 PH;
};

struct CharRaceEntry
{
    uint32 race_id;
    uint32 unk1;
    uint32 faction_id;
    uint32 unk2;
    uint32 unk3;
    uint32 unk4;
    uint32 unk5;
    uint32 unk6;
    uint32 team_id;
    uint32 unk7;
    uint32 unk8;
    uint32 unk9;
    uint32 name1;
    uint32 cinematic_id;
    uint32 name2;
    uint32 unk10;
    uint32 unk11;
    uint32 unk12;
    uint32 unk13;
    uint32 unk14;
    uint32 unk15;
    uint32 unk16;
    uint32 unk17;
    uint32 unk18;
    uint32 unk19;
    uint32 unk20;
    uint32 unk21;
};

struct CharClassEntry
{
    uint32 class_id;
    uint32 unk1;
    uint32 unk2;
    uint32 power_type;
    uint32 unk3;
    uint32 name;
    uint32 unk4;
    uint32 unk5;
    uint32 unk6;
    uint32 unk7;
    uint32 unk8;
    uint32 unk9;
    uint32 unk10;
    uint32 unk11;
    uint32 unk12;
    uint32 unk13;
    uint32 unk14;
    uint32 unk15;
};

struct CreatureFamilyEntry
{
    uint32 ID;
    float minsize;
    uint32 minlevel;
    float maxsize;
    uint32 maxlevel;
    uint32 skilline;
    uint32 tameable;
    uint32 pet_food_id;
    uint32 name;
    uint32 unk7;
    uint32 unk8;
    uint32 unk9;
    uint32 unk10;
    uint32 unk11;
    uint32 unk12;
    uint32 unk13;
    uint32 unk14;
    uint32 unk15;
};

struct MapEntry
{
    uint32 id;
    uint32 name_internal;
    uint32 map_type;
    uint32 unk;
    uint32 real_name;
    uint32 unk_1;
    uint32 unk1;
    uint32 unk2;
    uint32 unk3;
    uint32 unk4;
    uint32 unk5;
    uint32 unk6;
    uint32 unk7;
    uint32 unk8;
    uint32 unk9;
    uint32 unk10;
    uint32 unk11;
    uint32 unk12;
    uint32 unk13;
    uint32 unk14;
    uint32 unk15;
    uint32 unk16;
    uint32 unk17;
    uint32 unk18;
    uint32 unk19;
    uint32 unk20;
    uint32 unk21;
    uint32 unk22;
    uint32 unk23;
    uint32 unk24;
    uint32 unk25;
    uint32 unk26;
    uint32 unk27;
    uint32 unk28;
    uint32 unk29;
    uint32 unk30;
    uint32 unk31;
    uint32 unk32;
    uint32 unk33;
    uint32 unk34;
    uint32 unk35;
    uint32 unk36;
    uint32 unk37;
    uint32 unk38;
    uint32 unk39;
    uint32 unk40;
    uint32 unk41;
    uint32 unk42;
    uint32 unk43;
    uint32 unk44;
    uint32 unk45;
    uint32 unk46;
    uint32 unk47;
    uint32 unk48;
    uint32 unk49;
    uint32 unk50;
    uint32 unk51;
    uint32 unk52;
    uint32 unk53;
    uint32 unk54;
    uint32 unk55;
    uint32 unk56;
    uint32 unk57;
    uint32 unk58;
    uint32 unk59;
    uint32 unk60;
    uint32 unk61;
    uint32 unk62;
    uint32 unk63;
    uint32 unk64;
    uint32 unk65;
    uint32 unk66;
    uint32 unk67;
    uint32 unk68;
    uint32 unk69;
};

inline float GetRadius(SpellRadius *radius)
{
    return radius->Radius;
}
inline uint32 GetCastTime(SpellCastTime *time)
{
    return time->CastTime;
}
inline float GetMaxRange(SpellRange *range)
{
    return range->maxRange;
}
inline float GetMinRange(SpellRange *range)
{
    return range->minRange;
}
inline uint32 GetDuration(SpellDuration *dur)
{
    return dur->Duration1;
}


struct TransportAnimation
{
    uint32 ID;
    uint32 EntryID;
    uint32 Time;
    float PlusX;
    float PlusY;
    float PlusZ;
    uint32 MovementType;
};
// You need two lines like this for every new DBC store

defineIndexedDBCStore(ItemSetStore,ItemSetEntry);
defineIndexedDBCStore(LockStore,Lock);
defineIndexedDBCStore(SpellStore,SpellEntry);
defineIndexedDBCStore(DurationStore,SpellDuration);
defineIndexedDBCStore(RangeStore,SpellRange);
defineIndexedDBCStore(EmoteStore,emoteentry);
defineIndexedDBCStore(RadiusStore,SpellRadius);
defineIndexedDBCStore(CastTimeStore,SpellCastTime);
defineIndexedDBCStore(AreaStore,AreaTable);
defineIndexedDBCStore(WorldMapAreaStore,WorldMapArea);
defineDBCStore(WorldMapOverlayStore,WorldMapOverlay);
defineIndexedDBCStore(FactionTmpStore,FactionTemplateDBC);
defineIndexedDBCStore(FactionStore,FactionDBC);
defineIndexedDBCStore(EnchantStore,EnchantEntry);
defineIndexedDBCStore(RandomPropStore,RandomProps);
defineDBCStore(SkillStore,skilllinespell);
defineIndexedDBCStore(SkillLineStore,skilllineentry);
defineDBCStore(TaxiNodeStore,DBCTaxiNode);
defineDBCStore(TaxiPathStore,DBCTaxiPath);
defineDBCStore(TaxiPathNodeStore,DBCTaxiPathNode);
defineDBCStore(WorldSafeLocsStore,GraveyardEntry);
defineIndexedDBCStore(TransportAnimationStore,TransportAnimation);
defineDBCStore(NameGenStore, NameGenEntry);
defineIndexedDBCStore(AuctionHouseStore,AuctionHouseDBC);
defineDBCStore(TalentStore,TalentEntry);
defineIndexedDBCStore(CreatureSpellDataStore, CreatureSpellDataEntry);
defineIndexedDBCStore(CreatureFamilyStore, CreatureFamilyEntry);
defineIndexedDBCStore(CharClassStore, CharClassEntry);
defineIndexedDBCStore(CharRaceStore, CharRaceEntry);
defineIndexedDBCStore(MapStore, MapEntry);
defineIndexedDBCStore(ItemExtendedCostStore,ItemExtendedCostEntry);

#define sRandomPropStore RandomPropStore::getSingleton()
#define sEnchantStore EnchantStore::getSingleton()
#define sSpellStore SpellStore::getSingleton()
#define sLockStore LockStore::getSingleton()
#define sSkillLineStore SkillLineStore::getSingleton()
#define sSkillStore SkillStore::getSingleton()
#define sEmoteStore EmoteStore::getSingleton()
#define sSpellDuration DurationStore::getSingleton()
#define sSpellRange RangeStore::getSingleton()
#define sSpellRadius RadiusStore::getSingleton()
#define sCastTime CastTimeStore::getSingleton()
#define sAreaStore AreaStore::getSingleton()
#define sWorldMapAreaStore WorldMapAreaStore::getSingleton()
#define sWorldMapOverlayStore WorldMapOverlayStore::getSingleton()
#define sFactionTmpStore FactionTmpStore::getSingleton()
#define sFactionStore FactionStore::getSingleton()
#define sTaxiNodeStore TaxiNodeStore::getSingleton()
#define sTaxiPathStore TaxiPathStore::getSingleton()
#define sTaxiPathNodeStore TaxiPathNodeStore::getSingleton()
#define sItemSetStore ItemSetStore::getSingleton()
#define sWorldSafeLocsStore WorldSafeLocsStore::getSingleton()
#define sTransportAnimationStore TransportAnimationStore::getSingleton()
#define sAuctionHouseStore AuctionHouseStore::getSingleton()
#define sTalentStore TalentStore::getSingleton()
#define sCreatureSpellStore CreatureSpellDataStore::getSingleton()
#define sCreatureFamilyStore CreatureFamilyStore::getSingleton()
#define sCharClassStore CharClassStore::getSingleton()
#define sCharRaceStore CharRaceStore::getSingleton()
#define sMapStore MapStore::getSingleton()
#define sItemExtendedCostStore ItemExtendedCostStore::getSingleton()

#endif
