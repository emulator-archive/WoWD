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

#ifndef _LOOTMGR_H
#define _LOOTMGR_H

class LootRoll
{
public:
    LootRoll(uint32 timer, uint32 groupcount, uint64 guid, uint32 slotid, uint32 itemid, uint32 itemunk1, uint32 itemunk2);
    ~LootRoll();
    void PlayerRolled(Player *player, uint8 choice);
    void Finalize();

private:
    std::map<Player*, uint32> NeedRolls;
    std::map<Player*, uint32> GreedRolls;
    uint32 _groupcount;
    uint32 _slotid;
    uint32 _itemid;
    uint32 _itemunk1;
    uint32 _itemunk2;
    uint64 _guid;
};

typedef struct
{
    uint32    prop;
    float    chance;
}LootProp;

typedef struct
{
    LootProp * pProps;
    uint32    iPropsCount;
}LootPropTable;


typedef struct
{
    uint32 itemid;
    uint32 displayid;
}_LootItem;

typedef struct
{
    _LootItem item;
    uint32 iItemsCount;
    uint32 iRandomProperty;
    LootRoll *roll;
}__LootItem;


typedef struct
{
    _LootItem item;
    float chance;
    LootPropTable *prop;
}StoreLootItem;


typedef struct 
{
    uint32 count;
    StoreLootItem*items;
}StoreLootList;

typedef std::set<Player*> LooterSet;

typedef struct
{
    std::vector<__LootItem> items;
    uint32 gold;
    LooterSet looters;
}Loot;


//////////////////////////////////////////////////////////////////////////////////////////


typedef HM_NAMESPACE::hash_map<uint32, StoreLootList > LootStore;  
typedef HM_NAMESPACE::hash_map<uint32,  LootPropTable*> PropStore; 

#define PARTY_LOOT_FFA      0
#define PARTY_LOOT_MASTER   2
#define PARTY_LOOT_RR       1
#define PARTY_LOOT_NBG      4
#define PARTY_LOOT_GROUP    3



class LootMgr : public Singleton < LootMgr >
{
public:
    LootMgr();
    ~LootMgr();

    typedef HM_NAMESPACE::hash_map<uint32, StoreLootList > LootStore;  
    typedef HM_NAMESPACE::hash_map<uint32,  LootPropTable*> PropStore; 
    
    void FillProfessionLoot(LootStore * store,Loot * loot,uint32 loot_id);
    void FillCreatureLoot(Loot * loot,uint32 loot_id);
    void FillGOLoot(Loot * loot,uint32 loot_id);
    void FillItemLoot(Loot *loot, uint32 loot_id);

    bool CanGODrop(uint32 LootId,uint32 itemid);
    bool IsSkinnable(uint32 creatureId);
    bool IsFishable(uint32 zoneid);

    void LoadLoot();
    void LoadLootProp(uint32);
    
    LootStore    CreatureLoot;
    
    LootStore    FishingLoot;
    LootStore    SkinningLoot;
    LootStore    GOLoot;
    LootStore    ItemLoot;
    std::map<uint32, std::set<uint32> > quest_loot_go;
 
private:
    void LoadLootTables(const char * szTableName,LootStore * LootTable);
    void PushLoot(StoreLootList *list,Loot * loot);
    PropStore    LootProperties;
    uint32 _propCount;
};

#define lootmgr LootMgr::getSingleton()

#endif
