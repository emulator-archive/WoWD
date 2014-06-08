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

initialiseSingleton(LootMgr);

struct loot_tb
{
    uint32 itemid;
    float chance;
};

bool Rand(float chance)
{
    int32 val = sRand.randInt(10000);
    int32 p = int32(chance * 100);
    return p >= val;
}

LootMgr::LootMgr()
{}

void LootMgr::LoadLoot()
{
    //THIS MUST BE CALLED AFTER LOADING OF ITEMS
    LoadLootProp(0);
    LoadLootTables("creatureloot",&CreatureLoot);
    LoadLootTables("objectloot",&GOLoot);
    LoadLootTables("skinningloot",&SkinningLoot);
    LoadLootTables("fishingloot",&FishingLoot);
    LoadLootTables("itemloot", &ItemLoot);
}

void LootMgr::LoadLootProp(uint32 id)
{    
    //std::stringstream ss;
    //ss << "SELECT * FROM lootrandomprop WHERE entryid=" << id << " ORDER BY percentchance DESC";
    //QueryResult *result = sDatabase.Query( ss.str().c_str() );
    QueryResult * result = sDatabase.Query("SELECT * FROM lootrandomprop ORDER BY entryid");
    if(!result)return;

    Field *fields = result->Fetch();
    map<uint32, vector< pair< uint32, float > > > propcache;
    map<uint32, vector< pair< uint32, float > > >::iterator itr;
    uint32 i = 0;
    uint32 m = result->GetRowCount();
    do 
    {
        propcache[fields[0].GetUInt32()].push_back( make_pair( fields[1].GetUInt32(), fields[2].GetFloat() ) );
        ++i;
        if(!(i % 100))
            SetProgressBar(i, m, "random props");
    } while(result->NextRow());
    delete result;

    i = 0;
    m = propcache.size();

    for(itr = propcache.begin(); itr != propcache.end(); ++itr)
    {
        LootPropTable * prop = new LootPropTable;
        prop->iPropsCount = itr->second.size();
        prop->pProps = new LootProp[prop->iPropsCount];
        vector< pair<uint32, float> >::iterator it2 = itr->second.begin();
        uint32 j = 0;
        for(; it2 != itr->second.end(); ++it2, ++j)
        {
            prop->pProps[j].chance = it2->second;
            prop->pProps[j].prop = it2->first;
        }
        LootProperties[itr->first] = prop;
        ++i;
        if(!(i % 10))
            SetProgressBar(i, m, "random props");
    }

    ClearProgressBar();

    
    /*LootPropTable * prop=new LootPropTable;
    prop->iPropsCount =result->GetRowCount();
    prop->pProps=new LootProp[prop->iPropsCount];
    uint32 ind=0;
    do
    {
        fields = result->Fetch();
        prop->pProps[ind].prop =fields[1].GetUInt32();
        prop->pProps[ind].chance =fields[2].GetUInt32();
    } while (result->NextRow());
    delete result;
    LootProperties[id]=prop;*/
}

LootMgr::~LootMgr()
{
    sLog.outString("  Deleting Loot Tables...");
  for(LootStore::iterator iter=CreatureLoot.begin(); iter != CreatureLoot.end(); ++iter)
  delete [] iter->second.items;
  CreatureLoot.clear();

  for(LootStore::iterator iter=FishingLoot.begin(); iter != FishingLoot.end(); ++iter)
  delete [] iter->second.items;
  FishingLoot.clear();

  for(LootStore::iterator iter=SkinningLoot.begin(); iter != SkinningLoot.end(); ++iter)
  delete [] iter->second.items;
  SkinningLoot.clear();

  for(LootStore::iterator iter=GOLoot.begin(); iter != GOLoot.end(); ++iter)
  delete [] iter->second.items;
  GOLoot.clear();

  for(LootStore::iterator iter=ItemLoot.begin(); iter != ItemLoot.end(); ++iter)
      delete [] iter->second.items;
  ItemLoot.clear();

  for(PropStore::iterator iter = LootProperties.begin(); iter != LootProperties.end(); ++iter)
  {
      delete iter->second->pProps;
      delete iter->second;
  }
  LootProperties.clear();
}

void LootMgr::LoadLootTables(const char * szTableName,LootStore * LootTable)
{
    DBCFile *dbc = new DBCFile();

    dbc->open("DBC/ItemRandomProperties.dbc");
    _propCount = dbc->getRecordCount();
    delete dbc;
    //HM_NAMESPACE::hash_map<uint32, std::vector<loot_tb> > loot_db;
    //HM_NAMESPACE::hash_map<uint32, std::vector<loot_tb> >::iterator itr;
    vector< pair< uint32, vector< pair<uint32, float> > > > db_cache;
    vector< pair< uint32, vector< pair<uint32, float> > > >::iterator itr;
    db_cache.reserve(10000);
    LootStore::iterator tab;

    printf("Waiting on SQL...\r");
    QueryResult *result =sDatabase.Query("SELECT entryid, itemid, percentchance FROM %s ORDER BY entryid ASC",szTableName);
    if(!result)
    {
        sLog.outError("\rWARNING: Loading loot from table %s failed.", szTableName);
        return;
    }
    uint32 entry_id = 0;
    uint32 last_entry = 0;

    uint32 total =(uint32) result->GetRowCount();
    int pos = 0;
    vector< pair<uint32, float> > ttab;
    do 
    {     
        Field *fields = result->Fetch();
        entry_id = fields[0].GetUInt32();
        if(entry_id < last_entry)
        {
            sLog.outError("WARNING: Out of order loot table being loaded.\n");
            return;
        }
        if(entry_id != last_entry)
        {
            if(last_entry != 0)
                db_cache.push_back( make_pair( last_entry, ttab) );
            ttab.clear();
        }

        ttab.push_back( make_pair(fields[1].GetUInt32(), fields[2].GetFloat()) );
        

        /*loot_tb t;           
        t.itemid = fields[1].GetUInt32();
        t.chance = fields[2].GetFloat();
        loot_db[fields[0].GetUInt32()].push_back(t);*/

        last_entry = entry_id;
        ++pos;
        if(!(pos % 10000))
            SetProgressBar(pos, total, szTableName);
    } while( result->NextRow() );
    ClearProgressBar();
    pos = 0;
    total = db_cache.size();
    ItemPrototype* proto;
    uint32 itemid;

    //for(itr=loot_db.begin();itr!=loot_db.end();++itr)
    for( itr = db_cache.begin(); itr != db_cache.end(); ++itr)
    {
        entry_id = (*itr).first;
        if(LootTable->end()==LootTable->find(entry_id))
        {
            StoreLootList list;
            //list.count = itr->second.size();            
            list.count = (*itr).second.size();
            list.items=new StoreLootItem[list.count];
        
            uint32 ind=0;
            //for(std::vector<loot_tb>::iterator itr2=itr->second.begin();itr2!=itr->second.end();++itr2)
            for(vector< pair<uint32, float> >::iterator itr2 = (*itr).second.begin(); itr2 != (*itr).second.end(); ++itr2)
            {
                //Omit items that are not in db to prevent future bugs
                //uint32 itemid=(*itr2).itemid;
                itemid = (*itr2).first;
                proto=objmgr.GetItemPrototype(itemid);
                if(!proto)
                {
                    list.items[ind].item.itemid=0;
                    //sLog.outDetail("WARNING: Loot %u contains item %u that does not exist in the DB.",entry_id,(*itr2).itemid);
                }
                else
                {
                    list.items[ind].item.itemid=itemid;
                    list.items[ind].item.displayid=proto->DisplayInfoID;
                    //list.items[ind].chance=(*itr2).chance;
                    list.items[ind].chance=(*itr2).second;

                    PropStore::iterator ptab =LootProperties.find(itemid);
                
                    if( LootProperties.end()==ptab)
                        list.items[ind].prop=NULL;
                    else
                        list.items[ind].prop=ptab->second;

                    if(LootTable == &GOLoot)
                    {
                        if(proto->Class == ITEM_CLASS_QUEST)
                        {
                            //printf("Quest item \"%s\" allocated to quest ", proto->Name1.c_str());
                            sQuestMgr.SetGameObjectLootQuest(itr->first, itemid);
							quest_loot_go[entry_id].insert(proto->ItemId);
                        }
                    }
                }
                ind++;
            }
            (*LootTable)[entry_id]=list;
        }
        ++pos;
        if(!(pos % 1000)) SetProgressBar(pos, total, szTableName);
    }

    ClearProgressBar();
    //sLog.outString("  %d loot templates loaded from %s", db_cache.size(), szTableName);
 //   loot_db.clear();
    delete result;
}

void LootMgr::PushLoot(StoreLootList *list,Loot * loot)
{
	int i;
    for(uint32 x =0; x<list->count;x++)
    if(list->items[x].item.itemid)// this check is needed until loot DB is fixed
    {
        ItemPrototype *itemproto = objmgr.GetItemPrototype(list->items[x].item.itemid);
        if(Rand(list->items[x].chance * sWorld.getRate(RATE_DROP)) )//|| itemproto->Class == ITEM_CLASS_QUEST)
		{
			for(i = 0; i < loot->items.size(); ++i)
			{
				//itemid rand match a already placed item, if item is stackable and unique(stack), increment it, otherwise skips
				if((loot->items[i].item.itemid == list->items[x].item.itemid) && itemproto->MaxCount && (loot->items[i].iItemsCount < itemproto->MaxCount))
				{
					if(itemproto->Unique && (loot->items[i].iItemsCount < itemproto->Unique))
					{
						loot->items[i].iItemsCount++;
						break;
					}
					else if (!itemproto->Unique)
					{
						loot->items[i].iItemsCount++;
						break;
					}
				}
			}
			if(i != loot->items.size())
				continue;

			__LootItem itm;
			itm.item =list->items[x].item;
			itm.iItemsCount =1;//(itemproto->MaxCount>1)?(1+rand()%itemproto->MaxCount):1;
			itm.iRandomProperty=0;
            itm.roll = NULL;
		    
			if (list->items[x].prop && itemproto->Quality > 1)
            {
			    for (uint32 k = 0; k < list->items[x].prop->iPropsCount ;k++)
                {
			        if (Rand(list->items[x].prop->pProps[k].chance))
			        {
				        itm.iRandomProperty=list->items[x].prop->pProps[k].prop;
				        break;
			        }
                }
            }
			/*if(itemproto->RandomPropId>0)
			    if(itemproto->Quality > 1)//green +
			        if(Rand(25))//25% chance to get rnd prop
			        {//let's find some rnd bonus
				        RandomProps *rp=NULL;
				        while(1)
				        {
					        uint32 b=rand()%_propCount;//
					        rp= sRandomPropStore.LookupEntry(b);
					        if(rp)
					        {
						        itm.iRandomProperty=rp->ID;
						        break;
					        }
				        }			
			        }		                */        
			loot->items.push_back(itm);
		}    
    }
}



void LootMgr::FillCreatureLoot(Loot * loot,uint32 loot_id)
{
    loot->items.clear ();
    loot->gold =0;
    
    LootStore::iterator tab =CreatureLoot.find(loot_id);
    if( CreatureLoot.end()==tab)return;
    else PushLoot(&tab->second,loot);
}

void LootMgr::FillGOLoot(Loot * loot,uint32 loot_id)
{
    loot->items.clear ();
    loot->gold =0;
    
    LootStore::iterator tab =GOLoot.find(loot_id);
    if( GOLoot.end()==tab)return;
    else PushLoot(&tab->second,loot);
}

//Puts 1 item always, no random properties
void LootMgr::FillProfessionLoot(LootStore * store,Loot * loot,uint32 loot_id)
{
    loot->items.clear ();
    loot->gold =0;
    
	LootStore::iterator tab =store->find(loot_id);
    if( store->end()==tab)return;
	StoreLootList *list=&(tab->second);
	
	// TODO: fix infinite loop with ' while(true) '
	while(true)
	for(uint32 x =0,pass=0; x<list->count; x++,pass++)
    {
	    if(list->items[x].item.itemid)// this check is needed until loot DB is fixed
	    {
//		    ItemPrototype *itemproto = objmgr.GetItemPrototype(list->items[x].item.itemid);
		    if(Rand(list->items[x].chance))// || itemproto->Class == ITEM_CLASS_QUEST)
		    {
			    __LootItem itm;
			    itm.item =list->items[x].item;
				itm.iItemsCount=1;
				/*if(itemproto->MaxCount>1)
				{
					uint32 c=1 + sRand.randInt(itemproto->MaxCount);
					if(Rand(float(100.0/c)))
						itm.iItemsCount=c;
				
				}*/
				
			    itm.iRandomProperty=0;
    		  
			    loot->items.push_back(itm);
			    return;
		    }    
	    }
		
		if(pass>100)
		{
			sLog.outError("WARNING: Loot %u has too low chances",loot_id);
			return;		
		}
    }	
}

bool LootMgr::CanGODrop(uint32 LootId,uint32 itemid)
{
    LootStore::iterator tab =GOLoot.find(LootId);
    if( GOLoot.end()==tab)
    return false;
    StoreLootList *list=&(tab->second);
    for(uint32 x=0;x<list->count;x++)
        if(list->items[x].item.itemid==itemid)
            return true;
    return false;
}

//THIS should be cached 
bool LootMgr::IsSkinnable(uint32 creatureId)
{
    LootStore::iterator tab =SkinningLoot.find(creatureId);
    if( SkinningLoot.end()==tab)return false;
    else return true;
}

//THIS should be cached 
bool LootMgr::IsFishable(uint32 zoneid)
{
    LootStore::iterator tab =FishingLoot.find(zoneid);
    if(FishingLoot.end()==tab)return false;
    else return true;
}

#define NEED 1
#define GREED 2

LootRoll::LootRoll(uint32 timer, uint32 groupcount, uint64 guid, uint32 slotid, uint32 itemid, uint32 itemunk1, uint32 itemunk2)
{
    //sEventMgr.AddEvent(this, &LootRoll::Finalize, EVENT_LOOT_ROLL_FINALIZE, 60000, 1);
    _groupcount = groupcount;
    _guid = guid;
    _slotid = slotid;
    _itemid = itemid;
    _itemunk1 = itemunk1;
    _itemunk2 = itemunk2;
}

LootRoll::~LootRoll()
{

}

void LootRoll::Finalize()
{
    // this we will have to finalize with groups types.. for now
    // we'll just assume need before greed. person with highest roll
    // in need gets the item.

    uint32 highest = 0;
    int8 hightype = -1;
    Player *player = NULL;

    for(std::map<Player*, uint32>::iterator itr = NeedRolls.begin(); itr != NeedRolls.end(); ++itr)
    {
        if(itr->second > highest)
        {
            highest = itr->second;
            player = itr->first;
            hightype = NEED;
        }
    }

    if(highest == 0)
    {
        for(std::map<Player*, uint32>::iterator itr = GreedRolls.begin(); itr != GreedRolls.end(); ++itr)
        {
            if(itr->second > highest)
            {
                highest = itr->second;
                player = itr->first;
                hightype = GREED;
            }
        }
    }

    if(!player)
    {
        delete this;
        return;
    }

    WorldPacket data(34);
    data.SetOpcode(SMSG_LOOT_ROLL_WON);
    data << _guid << _slotid << _itemid << _itemunk1 << _itemunk2;
    data << player->GetGUID() << uint8(highest) << uint8(hightype);
    if(player->InGroup())
        player->GetGroup()->SendPacketToAll(&data);
    else
        player->GetSession()->SendPacket(&data);

    // Distribute the item.. yes i am lazy:D
    WorldPacket *tmp = new WorldPacket(1);
    tmp->SetOpcode(CMSG_AUTOSTORE_LOOT_ITEM);
    *tmp << uint8(_slotid);
    player->GetSession()->QueuePacket(tmp);

    delete this;
}

void LootRoll::PlayerRolled(Player *player, uint8 choice)
{
    if(NeedRolls.find(player) != NeedRolls.end() || GreedRolls.find(player) != GreedRolls.end())
        return; // dont allow cheaters

    std::map<Player*, uint32>* rmap;
    
    if(choice == NEED) {
        rmap = &NeedRolls;
    } else {
        rmap = &GreedRolls;
    }

    int roll = sRand.randInt(100);

    rmap->insert ( std::make_pair(player, roll) );

    // create packet
    WorldPacket data(34);
    data.SetOpcode(SMSG_LOOT_ROLL);
    data << _guid << _slotid << player->GetGUID();
    data << _itemid << _itemunk1 << _itemunk2;
    data << uint8(roll) << choice;
    
    if(player->InGroup())
        player->GetGroup()->SendPacketToAll(&data);
    else
        player->GetSession()->SendPacket(&data);
    
    // check for early completion
    if(NeedRolls.size() + GreedRolls.size() == _groupcount)
    {
        // kill event early
        //sEventMgr.RemoveEvents(this);
        Finalize();
    }
}

void LootMgr::FillItemLoot(Loot *loot, uint32 loot_id)
{
    loot->items.clear ();
    loot->gold =0;

    LootStore::iterator tab =ItemLoot.find(loot_id);
    if( ItemLoot.end()==tab)return;
    else PushLoot(&tab->second,loot);
}

