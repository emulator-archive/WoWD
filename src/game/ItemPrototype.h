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

#ifndef _ITEMPROTOTYPE_H
#define _ITEMPROTOTYPE_H
class Spell;

#define MAX_INVENTORY_SLOT 118
#define MAX_BUYBACK_SLOT 12

#define ITEM_NO_SLOT_AVAILABLE -1 //works for all kind of slots now
#define INVENTORY_SLOT_NOT_SET -1

#define EQUIPMENT_SLOT_START         0
#define EQUIPMENT_SLOT_HEAD          0
#define EQUIPMENT_SLOT_NECK          1
#define EQUIPMENT_SLOT_SHOULDERS     2
#define EQUIPMENT_SLOT_BODY          3
#define EQUIPMENT_SLOT_CHEST         4
#define EQUIPMENT_SLOT_WAIST         5
#define EQUIPMENT_SLOT_LEGS          6
#define EQUIPMENT_SLOT_FEET          7
#define EQUIPMENT_SLOT_WRISTS        8
#define EQUIPMENT_SLOT_HANDS         9
#define EQUIPMENT_SLOT_FINGER1       10
#define EQUIPMENT_SLOT_FINGER2       11
#define EQUIPMENT_SLOT_TRINKET1      12
#define EQUIPMENT_SLOT_TRINKET2      13
#define EQUIPMENT_SLOT_BACK          14
#define EQUIPMENT_SLOT_MAINHAND      15
#define EQUIPMENT_SLOT_OFFHAND       16
#define EQUIPMENT_SLOT_RANGED        17
#define EQUIPMENT_SLOT_TABARD        18
#define EQUIPMENT_SLOT_END           19

#define INVENTORY_SLOT_BAG_START     19
#define INVENTORY_SLOT_BAG_1         19
#define INVENTORY_SLOT_BAG_2         20
#define INVENTORY_SLOT_BAG_3         21
#define INVENTORY_SLOT_BAG_4         22
#define INVENTORY_SLOT_BAG_END       23

#define INVENTORY_SLOT_ITEM_START    23
#define INVENTORY_SLOT_ITEM_1        23
#define INVENTORY_SLOT_ITEM_2        24
#define INVENTORY_SLOT_ITEM_3        25
#define INVENTORY_SLOT_ITEM_4        26
#define INVENTORY_SLOT_ITEM_5        27
#define INVENTORY_SLOT_ITEM_6        28
#define INVENTORY_SLOT_ITEM_7        29
#define INVENTORY_SLOT_ITEM_8        30
#define INVENTORY_SLOT_ITEM_9        31
#define INVENTORY_SLOT_ITEM_10       32
#define INVENTORY_SLOT_ITEM_11       33
#define INVENTORY_SLOT_ITEM_12       34
#define INVENTORY_SLOT_ITEM_13       35
#define INVENTORY_SLOT_ITEM_14       36
#define INVENTORY_SLOT_ITEM_15       37
#define INVENTORY_SLOT_ITEM_16       38
#define INVENTORY_SLOT_ITEM_END      39

#define BANK_SLOT_ITEM_START         39
#define BANK_SLOT_ITEM_1             39
#define BANK_SLOT_ITEM_2             40
#define BANK_SLOT_ITEM_3             41
#define BANK_SLOT_ITEM_4             42
#define BANK_SLOT_ITEM_5             43
#define BANK_SLOT_ITEM_6             44
#define BANK_SLOT_ITEM_7             45
#define BANK_SLOT_ITEM_8             46
#define BANK_SLOT_ITEM_9             47
#define BANK_SLOT_ITEM_10            48
#define BANK_SLOT_ITEM_11            49
#define BANK_SLOT_ITEM_12            50
#define BANK_SLOT_ITEM_13            51
#define BANK_SLOT_ITEM_14            52
#define BANK_SLOT_ITEM_15            53
#define BANK_SLOT_ITEM_16            54
#define BANK_SLOT_ITEM_17            55
#define BANK_SLOT_ITEM_18            56
#define BANK_SLOT_ITEM_19            57
#define BANK_SLOT_ITEM_20            58
#define BANK_SLOT_ITEM_21            59
#define BANK_SLOT_ITEM_22            60
#define BANK_SLOT_ITEM_23            61
#define BANK_SLOT_ITEM_24            62
#define BANK_SLOT_ITEM_25            63
#define BANK_SLOT_ITEM_26            64
#define BANK_SLOT_ITEM_27            65
#define BANK_SLOT_ITEM_28            66
#define BANK_SLOT_ITEM_END           67

#define BANK_SLOT_BAG_START          67
#define BANK_SLOT_BAG_1              67
#define BANK_SLOT_BAG_2              68
#define BANK_SLOT_BAG_3              69
#define BANK_SLOT_BAG_4              70
#define BANK_SLOT_BAG_5              71
#define BANK_SLOT_BAG_6              72
#define BANK_SLOT_BAG_7              73
#define BANK_SLOT_BAG_END            74

#define INVENTORY_KEYRING_START      86
#define INVENTORY_KEYRING_1          86
#define INVENTORY_KEYRING_2          87
#define INVENTORY_KEYRING_3          88
#define INVENTORY_KEYRING_4          89
#define INVENTORY_KEYRING_5          90
#define INVENTORY_KEYRING_6          91
#define INVENTORY_KEYRING_7          92
#define INVENTORY_KEYRING_8          93
#define INVENTORY_KEYRING_9          94
#define INVENTORY_KEYRING_10         95
#define INVENTORY_KEYRING_11         96
#define INVENTORY_KEYRING_12         97
#define INVENTORY_KEYRING_13         98
#define INVENTORY_KEYRING_14         99
#define INVENTORY_KEYRING_15         100
#define INVENTORY_KEYRING_16         101
#define INVENTORY_KEYRING_17         102
#define INVENTORY_KEYRING_18         103
#define INVENTORY_KEYRING_19         104
#define INVENTORY_KEYRING_20         105
#define INVENTORY_KEYRING_21         106
#define INVENTORY_KEYRING_22         107
#define INVENTORY_KEYRING_23         108
#define INVENTORY_KEYRING_24         109
#define INVENTORY_KEYRING_25         110
#define INVENTORY_KEYRING_26         111
#define INVENTORY_KEYRING_27         112
#define INVENTORY_KEYRING_28         113
#define INVENTORY_KEYRING_29         114
#define INVENTORY_KEYRING_30         115
#define INVENTORY_KEYRING_31         116
#define INVENTORY_KEYRING_32         117
#define INVENTORY_KEYRING_END        118

enum INV_ERR
{
    INV_ERR_OK,
    INV_ERR_YOU_MUST_REACH_LEVEL_N,
    INV_ERR_SKILL_ISNT_HIGH_ENOUGH,
    INV_ERR_ITEM_DOESNT_GO_TO_SLOT,
    INV_ERR_BAG_FULL,
    INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG,
    INV_ERR_CANT_TRADE_EQUIP_BAGS,
    INV_ERR_ONLY_AMMO_CAN_GO_HERE,
    INV_ERR_NO_REQUIRED_PROFICIENCY,
    INV_ERR_NO_EQUIPMENT_SLOT_AVAILABLE,
    INV_ERR_YOU_CAN_NEVER_USE_THAT_ITEM,
    INV_ERR_YOU_CAN_NEVER_USE_THAT_ITEM2,
    INV_ERR_NO_EQUIPMENT_SLOT_AVAILABLE2,
    INV_ERR_CANT_EQUIP_WITH_TWOHANDED,
    INV_ERR_CANT_DUAL_WIELD,
    INV_ERR_ITEM_DOESNT_GO_INTO_BAG,
    INV_ERR_ITEM_DOESNT_GO_INTO_BAG2,
    INV_ERR_CANT_CARRY_MORE_OF_THIS,
    INV_ERR_NO_EQUIPMENT_SLOT_AVAILABLE3,
    INV_ERR_ITEM_CANT_STACK,
    INV_ERR_ITEM_CANT_BE_EQUIPPED,
    INV_ERR_ITEMS_CANT_BE_SWAPPED,
    INV_ERR_SLOT_IS_EMPTY,
    INV_ERR_ITEM_NOT_FOUND,
    INV_ERR_CANT_DROP_SOULBOUND,
    INV_ERR_OUT_OF_RANGE,
    INV_ERR_TRIED_TO_SPLIT_MORE_THAN_COUNT,
    INV_ERR_COULDNT_SPLIT_ITEMS,
    INV_ERR_MISSING_REAGENT,
    INV_ERR_NOT_ENOUGH_MONEY,
    INV_ERR_NOT_A_BAG,
    INV_ERR_CAN_ONLY_DO_WITH_EMPTY_BAGS,
    INV_ERR_DONT_OWN_THAT_ITEM,
    INV_ERR_CAN_EQUIP_ONLY1_QUIVER,
    INV_ERR_MUST_PURCHASE_THAT_BAG_SLOT,
    INV_ERR_TOO_FAR_AWAY_FROM_BANK,
    INV_ERR_ITEM_LOCKED,
    INV_ERR_YOU_ARE_STUNNED,
    INV_ERR_YOU_ARE_DEAD,
    INV_ERR_CANT_DO_RIGHT_NOW,
    INV_ERR_BAG_FULL2,
    INV_ERR_CAN_EQUIP_ONLY1_QUIVER2,
    INV_ERR_CAN_EQUIP_ONLY1_AMMOPOUCH,
    INV_ERR_STACKABLE_CANT_BE_WRAPPED,
    INV_ERR_EQUIPPED_CANT_BE_WRAPPED,
    INV_ERR_WRAPPED_CANT_BE_WRAPPED,
    INV_ERR_BOUND_CANT_BE_WRAPPED,
    INV_ERR_UNIQUE_CANT_BE_WRAPPED,
    INV_ERR_BAGS_CANT_BE_WRAPPED,
    INV_ERR_ALREADY_LOOTED,
    INV_ERR_INVENTORY_FULL,
    INV_ERR_BANK_FULL,
    INV_ERR_ITEM_IS_CURRENTLY_SOLD_OUT,
    INV_ERR_BAG_FULL3,
    INV_ERR_ITEM_NOT_FOUND2,
    INV_ERR_ITEM_CANT_STACK2,
    INV_ERR_BAG_FULL4,
    INV_ERR_ITEM_SOLD_OUT,
    INV_ERR_OBJECT_IS_BUSY,
    INV_ERR_NONE,
    INV_ERR_CANT_DO_IN_COMBAT,
    INV_ERR_CANT_DO_WHILE_DISARMED,
    INV_ERR_BAG_FULL6,
    INV_ERR_ITEM_RANK_NOT_ENOUGH,
    INV_ERR_ITEM_REPUTATION_NOT_ENOUGH,
    INV_ERR_MORE_THAN1_SPECIAL_BAG,
};

enum ITEM_STAT_TYPE
{
    POWER      = 0,
    HEALTH     = 1,
    UNKNOWN    = 2,
    AGILITY    = 3,
    STRENGHT   = 4,
    INTELLECT  = 5,
    SPIRIT     = 6,
    STAMINA    = 7,
    DEFENSE_RATING = 12,
    DODGE_RATING   = 13,
    PARRY_RATING   = 14,
    SHIELD_BLOCK_RATING = 15,
    MELEE_HIT_RATING    = 16,
    RANGED_HIT_RATING   = 17,
    SPELL_HIT_RATING    = 18,
    MELEE_CRITICAL_STRIKE_RATING = 19,
    RANGED_CRITICAL_STRIKE_RATING = 20,
    SPELL_CRITICAL_STRIKE_RATING  = 21,
    MELEE_HIT_AVOIDANCE_RATING    = 22,
    RANGED_HIT_AVOIDANCE_RATING   = 23,
    SPELL_HIT_AVOIDANCE_RATING    = 24,
    MELEE_CRITICAL_AVOIDANCE_RATING = 25,
    RANGED_CRITICAL_AVOIDANCE_RATING = 26,
    SPELL_CRITICAL_AVOIDANCE_RATING  = 27,
    MELEE_HASTE_RATING               = 28,
    RANGED_HASTE_RATING              = 29,
    SPELL_HASTE_RATING               = 30,
    HIT_RATING                       = 31,
    CRITICAL_STRIKE_RATING           = 32,
    HIT_AVOIDANCE_RATING             = 33,
    CRITICAL_AVOIDANCE_RATING        = 34,
    RESILIENCE_RATING                = 35,
    HASTE_RATING                     = 36,
};

#define PCT_DEFENSE_RATING  1.5
#define PCT_SKILL_RATING 2.5
enum ITEM_RATING_PCT_CONVERSION
{
    PCT_DODGE_RATING   = 12,
    PCT_PARRY_RATING   =  20,
    PCT_SHIELD_BLOCK_RATING = 5,
    PCT_MELEE_HIT_RATING    = 10,
    PCT_RANGED_HIT_RATING   = 10,
    PCT_SPELL_HIT_RATING    = 8,
    PCT_MELEE_CRITICAL_STRIKE_RATING = 14,
    PCT_RANGED_CRITICAL_STRIKE_RATING = 14,
    PCT_SPELL_CRITICAL_STRIKE_RATING  = 14,
    //MELEE_HIT_AVOIDANCE_RATING    = 22,
    //RANGED_HIT_AVOIDANCE_RATING   = 23,
    //SPELL_HIT_AVOIDANCE_RATING    = 24,
    //MELEE_CRITICAL_AVOIDANCE_RATING = 25,
    //RANGED_CRITICAL_AVOIDANCE_RATING = 26,
    //SPELL_CRITICAL_AVOIDANCE_RATING  = 27,
    PCT_MELEE_HASTE_RATING               = 10,
    PCT_RANGED_HASTE_RATING              = 10,
    PCT_SPELL_HASTE_RATING               = 10,
    PCT_HIT_RATING                       = 10, //?affects all 3 types? 
    PCT_CRITICAL_STRIKE_RATING           = 14, //?affects all 3 types? 
    //HIT_AVOIDANCE_RATING             = 33,
    //CRITICAL_AVOIDANCE_RATING        = 34,
    PCT_RESILIENCE_RATING                = 25,
    PCT_HASTE_RATING                     = 10, //?affects all 3 types? 
};

enum ITEM_DAMAGE_TYPE
{
    NORMAL_DAMAGE  = 0,
    HOLY_DAMAGE    = 1,
    FIRE_DAMAGE    = 2,
    NATURE_DAMAGE  = 3,
    FROST_DAMAGE   = 4,
    SHADOW_DAMAGE  = 5,
    ARCANE_DAMAGE  = 6,
};

enum ITEM_SPELLTRIGGER_TYPE
{
    USE           = 0,
    ON_EQUIP      = 1,
    CHANCE_ON_HIT = 2,
    SOULSTONE     = 4,
};

enum ITEM_BONDING_TYPE
{
    ITEM_BIND_NONE      =   0, 
    ITEM_BIND_ON_PICKUP =   1,
    ITEM_BIND_ON_EQUIP  =   2,
    ITEM_BIND_ON_USE    =   3,
    ITEM_BIND_QUEST     =   4
};

enum INVENTORY_TYPES
{
    INVTYPE_NON_EQUIP      = 0x0,
    INVTYPE_HEAD           = 0x1,
    INVTYPE_NECK           = 0x2,
    INVTYPE_SHOULDERS      = 0x3,
    INVTYPE_BODY           = 0x4,
    INVTYPE_CHEST          = 0x5,
    INVTYPE_WAIST          = 0x6,
    INVTYPE_LEGS           = 0x7,
    INVTYPE_FEET           = 0x8,
    INVTYPE_WRISTS         = 0x9,
    INVTYPE_HANDS          = 0xa,
    INVTYPE_FINGER         = 0xb,
    INVTYPE_TRINKET        = 0xc,
    INVTYPE_WEAPON         = 0xd,
    INVTYPE_SHIELD         = 0xe,
    INVTYPE_RANGED         = 0xf,
    INVTYPE_CLOAK          = 0x10,
    INVTYPE_2HWEAPON       = 0x11,
    INVTYPE_BAG            = 0x12,
    INVTYPE_TABARD         = 0x13,
    INVTYPE_ROBE           = 0x14,
    INVTYPE_WEAPONMAINHAND = 0x15,
    INVTYPE_WEAPONOFFHAND  = 0x16,
    INVTYPE_HOLDABLE       = 0x17,
    INVTYPE_AMMO           = 0x18,
    INVTYPE_THROWN         = 0x19,
    INVTYPE_RANGEDRIGHT    = 0x1a,
    INVTYPE_QUIVER         = 0x1b,
    INVTYPE_RELIC          = 0x1c,
    NUM_INVENTORY_TYPES    = 0x1d,
};

enum ITEM_CLASS
{
    ITEM_CLASS_CONSUMABLE = 0,
    ITEM_CLASS_CONTAINER,
    ITEM_CLASS_WEAPON,
    ITEM_CLASS_JEWELRY,
    ITEM_CLASS_ARMOR,
    ITEM_CLASS_REAGENT,
    ITEM_CLASS_PROJECTILE,
    ITEM_CLASS_TRADEGOODS,
    ITEM_CLASS_GENERIC,
    ITEM_CLASS_RECIPE,
    ITEM_CLASS_MONEY,
    ITEM_CLASS_QUIVER,
    ITEM_CLASS_QUEST,
    ITEM_CLASS_KEY,
    ITEM_CLASS_PERMANENT,
    ITEM_CLASS_MISCELLANEOUS
};

enum SPECIAL_ITEM_TYPE
{
    ITEM_TYPE_KEYRING     = 9,
    ITEM_TYPE_ENCHANTMENT = 7,
    ITEM_TYPE_HERBALISM   = 6,
    ITEM_TYPE_SOULSHARD   = 3,
    ITEM_TYPE_GUNAMMO     = 2,
    ITEM_TYPE_BOWAMMO     = 1
};

enum SOCKET_GEM_COLOR
{
    META_SOCKET = 1,
    RED_SOCKET = 2,
    YELLOW_SOCKET = 4,
    BLUE_SOCKET = 8
};

struct ItemPrototype
{
    uint32 ItemId;
    uint32 Class;
    uint32 SubClass;
    uint32 unknown_bc;
    std::string Name1;
    std::string Name2;
    std::string Name3;
    std::string Name4;
    uint32 DisplayInfoID;
    uint32 Quality;
    uint32 Flags;
    uint32 BuyPrice;
    uint32 SellPrice;
    uint32 InventoryType;
    uint32 AllowableClass;
    uint32 AllowableRace;
    uint32 ItemLevel;
    uint32 RequiredLevel;
    uint32 RequiredSkill;
    uint32 RequiredSkillRank;
    uint32 RequiredSkillSubRank;
    uint32 RequiredPlayerRank1;
    uint32 RequiredPlayerRank2;
    uint32 RequiredFaction;
    uint32 RequiredFactionStanding;
    uint32 Unique;
    uint32 MaxCount;
    uint32 ContainerSlots;
    uint32 ItemStatType[10];
    uint32 ItemStatValue[10];
    float DamageMin[5];
    float DamageMax[5];
    uint32 DamageType[5];
    uint32 Armor;
    uint32 HolyRes;
    uint32 FireRes;
    uint32 NatureRes;
    uint32 FrostRes;
    uint32 ShadowRes;
    uint32 ArcaneRes;
    uint32 Delay;
    uint32 AmmoType;
    float  Range;
    uint32 SpellId[5];
    uint32 SpellTrigger[5];
    uint32 SpellCharges[5];
    uint32 SpellCooldown[5];
    uint32 SpellCategory[5];
    uint32 SpellCategoryCooldown[5];
    uint32 Bonding;
    std::string Description;
    uint32 PageId;
    uint32 PageLanguage;
    uint32 PageMaterial;
    uint32 QuestId;
    uint32 LockId;
    uint32 LockMaterial;
    uint32 Field108;
    uint32 RandomPropId;
    uint32 RandomPropId_2;
    uint32 Block;
    uint32 ItemSet;
    uint32 MaxDurability;
    uint32 ZoneNameID;
    uint32 Field114;
    uint32 BagFamily;
    uint32 ToolCategory;
    uint32 SocketColor1;
    uint32 Unk201_3;
    uint32 SocketColor2;
    uint32 Unk201_5;
    uint32 SocketColor3;
    uint32 Unk201_7;
    uint32 SocketBonus;
    uint32 GemProperties;
    uint32 ItemExtendedCost;
    uint32 DisenchantReqSkill;
};

typedef struct {
    uint32 setid;
    uint32 itemscount;
  //  Spell* spell[8];
}ItemSet;

#endif
