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


AuctionHouse::AuctionHouse(uint32 aid,uint32 entry,uint32 c, uint32 t)
{
    Id = aid;
    tax = t;
    cut = c;
    auctioneerentry = entry;
    maxId = 0;
    LoadFromDB();
    LoadMaxId();
}
AuctionHouse::~AuctionHouse()
{

}

void WorldSession::HandleAuctionListBidderItems( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 16);
    uint64 guid;
    //uint32 unk1,unk2;
    recv_data >> guid;// >> unk1 >> unk2;
    Creature* auctioneer = sObjHolder.GetObject<Creature>(guid);
    WorldPacket out = sAuctionMgr.GetAuctionHouseByEntry(auctioneer->GetEntry())->GenerateBidderList(GetPlayer());
    SendPacket(&out);
}

AuctionSystem::AuctionSystem()
{
    this->ThreadType = WOWD_THREADTYPE_AUCTIONHOUSE;
}

void AuctionSystem::run()
{
    SetThreadName("Auction House Operations");
    WOWD_THREAD_TRY_EXECUTION2

    int count = 0;
    while(this->ThreadState != WOWD_THREADSTATE_TERMINATE)
    {
        if(ThreadState == WOWD_THREADSTATE_TERMINATE)
            return;

        ThreadState = WOWD_THREADSTATE_BUSY;
        count++;
        if(count == 30) // every 60 sec
        {
            count = 0;
            sAuctionMgr.Update();
        }

        if(ThreadState == WOWD_THREADSTATE_TERMINATE)
            return;

        ThreadState = WOWD_THREADSTATE_SLEEPING;
        
        Sleep(2000);//update Auctions each minute
    }

    WOWD_THREAD_HANDLE_CRASH2
}
void AuctionHouse::UpdateAuction(uint32 id)
{
    AuctionEntry *ae = GetAuction(id);
    if(ae)
    {
        if (time(NULL) > (ae->time))
        {
            if (ae->bidder == 0)
                RemoveAuction(ae->Id,AUCTION_REMOVE_EXPIRED);
            else
                RemoveAuction(ae->Id,AUCTION_REMOVE_WON);            
        }
    }
}
void AuctionHouse::Update()
{
    std::map<uint32,AuctionEntry*>::iterator itr,next;
    for (itr = GetAuctionsBegin(); itr != GetAuctionsEnd();itr = next)
    {
        next = itr;
        next++;
        UpdateAuction(itr->second->Id);
    }
}
AuctionEntry* AuctionHouse::GetAuction(uint32 id)
{
    std::map<uint32,AuctionEntry*>::iterator iter;
    iter = Auctions.find(id);
    if(iter != Auctions.end())
        return iter->second;
    return NULL;
}
void AuctionHouse::RemoveAuction(uint32 id,uint32 reason)
{
    std::map<uint32,AuctionEntry*>::iterator itr;
    itr = Auctions.find(id);
    char sub[64];

    if(reason == AUCTION_REMOVE_EXPIRED)
    {
        sprintf(sub,"%u:%u:%u",objmgr.GetAItem(itr->second->item)->GetEntry(),0,AUCTION_EXPIRED);
        sMailSystem.SendAutomatedMessage(AUCTION, GetId(), itr->second->owner, sub, "", 0, 0, itr->second->item,62);
        //SendMailToPlayer(itr->second->owner,itr->second->owner,sub,"",0,0,objmgr.GetAItem(itr->second->item),62,AUCTION,GetId());
    }
    else if (reason == AUCTION_REMOVE_WON)
    {
        Player *pl = sObjHolder.GetObject<Player>(itr->second->bidder);
        if(pl)
            pl->GetSession()->SendPacket(&BuildBidderNotificationPacket(itr->second));
        sprintf(sub,"%u:%u:%u",objmgr.GetAItem(itr->second->item)->GetEntry(),0,AUCTION_SOLD);
        //sMailSystem.SendAutomatedMessage(AUCTION, GetId(), itr->second->bidder, sub, "", 0, 0, itr->second->item, 62);
        //SendMailToPlayer(itr->second->bidder,itr->second->owner,sub,"",itr->second->bid-((cut / 100.0f)*objmgr.GetAItem(itr->second->item)->GetProto()->SellPrice)+itr->second->deposit,0,NULL,62,AUCTION,GetId());

        uint32 amount = itr->second->bid-((cut / 100.0f)*objmgr.GetAItem(itr->second->item)->GetProto()->SellPrice)+itr->second->deposit;
        sMailSystem.SendAutomatedMessage(AUCTION, GetId(), itr->second->owner, sub, "", amount, 0, 0, 62);

        sprintf(sub,"%u:%u:%u",objmgr.GetAItem(itr->second->item)->GetEntry(),0,AUCTION_WON);
        sMailSystem.SendAutomatedMessage(AUCTION, GetId(), itr->second->bidder, sub, "", 0, 0, itr->second->item, 62);

        //SendMailToPlayer(itr->second->owner,itr->second->bidder,sub,"",0,0,objmgr.GetAItem(itr->second->item),62,AUCTION,GetId());
    } else if (reason == AUCTION_REMOVE_CANCELLED)
    {
        Player *pl = sObjHolder.GetObject<Player>(itr->second->owner);
        if(pl)
            if(pl->GetUInt32Value(PLAYER_FIELD_COINAGE) > floor(((cut / 100.0f)*itr->second->bid)))
                pl->SetUInt32Value(PLAYER_FIELD_COINAGE,pl->GetUInt32Value(PLAYER_FIELD_COINAGE) - floor(((cut / 100.0f)*itr->second->bid)));
            else
                return;
        else
            return;
        sprintf(sub,"%u:%u:%u",objmgr.GetAItem(itr->second->item)->GetEntry(),0,AUCTION_CANCELLED);
        if(itr->second->bidder)
        {
            sMailSystem.SendAutomatedMessage(AUCTION, GetId(), itr->second->owner, sub, "", itr->second->bid, 0, itr->second->item, 62);
            sMailSystem.SendAutomatedMessage(AUCTION, GetId(), itr->second->owner, sub, "", itr->second->bid, 0, 0, 62);
            
            //SendMailToPlayer(itr->second->owner,itr->second->owner,sub,"",itr->second->bid,0,objmgr.GetAItem(itr->second->item),62,AUCTION,GetId());
            //SendMailToPlayer(itr->second->bidder,itr->second->owner,sub,"",itr->second->bid,0,NULL,62,AUCTION,GetId());        
        }
        else
        {
            sMailSystem.SendAutomatedMessage(AUCTION, GetId(), itr->second->owner, sub, "", 0, 0, itr->second->item, 62);
             //SendMailToPlayer(itr->second->owner,itr->second->owner,sub,"",0,0,objmgr.GetAItem(itr->second->item),62,AUCTION,GetId());
        }            
    }
    sDatabase.Execute("DELETE FROM auctions WHERE auctionId = %u", itr->second->Id);
    sDatabase.Execute("DELETE FROM bids WHERE ID = %u", itr->second->Id );
    objmgr.RemoveAItem(itr->second->item);
    if(itr != Auctions.end())
        Auctions.erase(itr);    
}

void AuctionHouse::SaveAuction(AuctionEntry* auction)
{
    std::stringstream sql;
    sql << "INSERT INTO auctions SET auctionId = '" << auction->Id << "', auctionhouse = '" << auction->auctionhouse << "', item = '" << auction->item << "', owner = '" << auction->owner << "', buyout = '" << auction->buyout << "', time = '" << auction->time << "', bidder = '" << auction->bidder << "', bid = '" << auction->bid << "', deposit = '" << auction->deposit << "'";
    sDatabase.Execute("DELETE FROM auctions WHERE auctionId = %u", auction->Id);
    sDatabase.Execute(sql.str().c_str());

}
void AuctionHouse::PlaceBid(WorldPacket &recv_data, Player *pl)
{
    uint32 auction,price;
    recv_data >> auction >> price;

    AuctionEntry* ah = GetAuction(auction);
    char sub[64];
    if(!ah) return;

    if (ah->owner != pl->GetGUID())
    {
        if (ah->bidder != 0)
        {
            sprintf(sub,"%u:%u:%u",objmgr.GetAItem(ah->item)->GetEntry(),0,AUCTION_CANCELLED);
            sMailSystem.SendAutomatedMessage(AUCTION, GetId(), ah->bidder, sub, "", ah->bid, 0, 0, 62);
            //SendMailToPlayer(ah->owner,ah->bidder,sub,"",ah->bid,0,NULL,62,AUCTION,GetId());
        }
        ah->bidder = pl->GetGUID();
        ah->bid = price;
        pl->SetUInt32Value(PLAYER_FIELD_COINAGE,(pl->GetUInt32Value(PLAYER_FIELD_COINAGE) - price));
        if (((price < ah->buyout) || (ah->buyout == 0)))
        {
            bidentry *be = new bidentry;
            be->AuctionID = auction;
            be->amt = price;
            be->AHid = GetId();
            pl->AddBid(be);
        }
        else
        {
            pl->GetSession()->SendPacket(&BuildResultPacket(ah->Id,AUCTION_BID));
            RemoveAuction(ah->Id,AUCTION_REMOVE_WON);
            return;
        } 
    }
    UpdateAuction(auction);
    SaveAuction(ah);
    pl->GetSession()->SendPacket(&BuildResultPacket(ah->Id,AUCTION_BID));
}
WorldPacket AuctionHouse::AddAuctionToPacket(WorldPacket data,AuctionEntry* ae,Item* it)
{
        data << ae->Id;
        data << it->GetUInt32Value(OBJECT_FIELD_ENTRY);

        for (uint32 i = 0; i < 6; i++)
        {
            data << (uint32)it->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + (3 * i));   // Enchantment ID
            data << (uint32)it->GetEnchantmentApplytime(i);                         // Unknown / maybe ApplyTime
            data << (uint32)it->GetUInt32Value(ITEM_FIELD_SPELL_CHARGES + i);       // Charges
        }
        data << (uint32)it->GetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID);        // -ItemRandomSuffix / random property     : If the value is negative its ItemRandomSuffix if its possitive its RandomItemProperty
        data << (uint32)0;                                                          // when ItemRandomSuffix is used this is the modifier


        /**Capt: ****************** ItemRandomSuffix***************************
        * For what I have seen ItemRandomSuffix is like RandomItemProperty
        * The only difference is has is that it has a modifier.
        * That is the result of jewelcrafting, the effect is that the
        * enchantment is variable. That means that a enchantment can be +1 and 
        * with more Jem's +12 or so.
        * Decription for lookup: You get the enchantmentSuffixID and search the
        * DBC for the last 1 - 3 value's(depending on the enchantment).
        * That value is what I call EnchantmentValue. You guys might find a 
        * better name but for now its good enough. The formula to calculate
        * The ingame "GAIN" is:
        * (Modifier / 10000) * enchantmentvalue = EnchantmentGain;    
        */
        
        data << uint32(it->GetUInt32Value(ITEM_FIELD_STACK_COUNT)); // Amount
        data << uint32(0);                  // Unknown
        data << uint32(0);                  // Unknown
        data << ae->owner;                  // Owner guid
        data << ae->bid;                    // Current prize
        // hehe I know its evil, this creates a nice trough put of money
        data << uint32(50);                  // Next bid value modifier, like current bid + this value
        data << ae->buyout;                 // Buyout
        data << uint32((ae->time - time(NULL)) * 1000); // Time left
        data << uint64(ae->bidder);         // Last bidder
        data << ae->bid;                    // The bid of the last bidder
        return data;
}
WorldPacket AuctionHouse::GenerateBidderList(Player *pl)
{
    WorldPacket data;
    std::list<bidentry*>::iterator itr;
    std::list<bidentry*>tempList;
    for (itr = pl->GetBidBegin(); itr != pl->GetBidEnd(); itr++)
    {
        AuctionEntry* ae = GetAuction((*itr)->AuctionID);
        if(ae)
        {
            if(ae->auctionhouse == GetId())
            {
                tempList.push_back((*itr));
            }
        }
    }
    data.Initialize( SMSG_AUCTION_BIDDER_LIST_RESULT );
    data << uint32(tempList.size());
    for (itr = tempList.begin(); itr != tempList.end(); itr++)
    {
        AuctionEntry *ae = GetAuction((*itr)->AuctionID);
        Item *it = objmgr.GetAItem(ae->item);
        data = AddAuctionToPacket(data,ae,it);
    }
    return data;
}

WorldPacket AuctionHouse::BuildBidderNotificationPacket(AuctionEntry* ac)
{
    uint32 unk1,unk2,unk3;
    unk1 = 0;
    unk2 = 0;
    unk3 = 0;
    WorldPacket data;
    data.Initialize(SMSG_AUCTION_BIDDER_NOTIFICATION);
    data << uint32(this->GetId());
    data << uint32(ac->Id);
    data << uint64(ac->bidder);
    data << uint32(unk1);
    data << uint32(unk2);
    data << uint32(objmgr.GetAItem(ac->item)->GetEntry());
    data << uint32(unk3);
    return data;
}

WorldPacket AuctionHouse::BuildResultPacket(uint32 aid,uint32 result)
{
    WorldPacket data;
    data.Initialize(SMSG_AUCTION_COMMAND_RESULT);
    data << uint32(aid);
    if(result != AUCTION_BUYOUT)
        data << uint32(result);
    else
        data << uint32(result-1);
    data << uint32(0);
    if(result == AUCTION_BUYOUT)
        data << uint32(0);
    return data;
}
void WorldSession::HandleAuctionPlaceBid( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 8);
    uint64 guid;
    recv_data >> guid;
    Creature* auctioneer = sObjHolder.GetObject<Creature>(guid);
    sAuctionMgr.GetAuctionHouseByEntry(auctioneer->GetEntry())->PlaceBid(recv_data,GetPlayer());
    WorldPacket out = sAuctionMgr.GetAuctionHouseByEntry(auctioneer->GetEntry())->GenerateBidderList(GetPlayer());
    SendPacket(&out);
}

void AuctionHouse::LoadMaxId()
{
    std::stringstream sql;
    QueryResult* result;
    sql << "SELECT MAX(auctionId) FROM auctions WHERE auctionhouse = '" << GetId() << "'";
    result = sDatabase.Query( sql.str().c_str() );
    if( result )
    {
        maxId = result->Fetch()[0].GetUInt32()+1;
        delete result;
    }
    else
    {
        maxId = 0;
    }
}

void WorldSession::HandleCancelAuction( WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 12);
    uint32 auctionID;
    uint64 guid;
    recv_data >> guid;
    recv_data >> auctionID;
    Creature* auctioneer = sObjHolder.GetObject<Creature>(guid);
    if(!auctioneer) return;

    AuctionHouse *AH = sAuctionMgr.GetAuctionHouseByEntry(auctioneer->GetEntry());
    if(!AH) return;

    AH->RemoveAuction(auctionID,AUCTION_REMOVE_CANCELLED);
    WorldPacket data = AH->GenerateOwnerList(GetPlayer());
    SendPacket(&data);
    SendPacket(&AH->BuildResultPacket(auctionID,AUCTION_CANCEL));
}

void AuctionHouse::SellItem( WorldPacket & recv_data, Player *pl)
{
    uint64 item;
    uint32 bid,buyout,etime;
    recv_data >> item;
    recv_data >> bid >> buyout >> etime;

    Item* it = pl->GetItemInterface()->GetItemByGUID(item);
    if(!it) return;

    float deposit = floor(tax * it->GetProto()->SellPrice / 100.0);

    deposit *= etime/120;

    if (pl->GetUInt32Value(PLAYER_FIELD_COINAGE) < deposit)
        return;
    AuctionEntry* ah = new AuctionEntry;
    ah->deposit = deposit;
    ah->auctionhouse = GetId();
    ah->item = item;
    ah->owner = pl->GetGUID();
    ah->bid = bid;
    ah->bidder = 0;
    ah->buyout = buyout;
    time_t base = time(NULL);
    ah->time = ((time_t)(etime * 60)) + base;
    ah->Id = GetMaxId();
    
    it = pl->GetItemInterface()->SafeRemoveAndRetreiveItemByGuid(item, true);
    if(!it)
    {
        return;
    }
    pl->SetUInt32Value(PLAYER_FIELD_COINAGE,(pl->GetUInt32Value(PLAYER_FIELD_COINAGE) - deposit));
    Auctions[ah->Id] = ah;
    objmgr.AddAItem(it);
    it->SetOwner(NULL);
    it->SaveToDB(INVENTORY_SLOT_NOT_SET, 0);
    SaveAuction(ah);
    pl->GetSession()->SendPacket(&BuildResultPacket(ah->Id,AUCTION_CREATE));
}
void WorldSession::HandleAuctionSellItem( WorldPacket & recv_data )
{
    uint64 guid;
    recv_data >> guid;
    Creature* auctioneer = sObjHolder.GetObject<Creature>(guid);
    if(!auctioneer) return;

    AuctionHouse *AH = sAuctionMgr.GetAuctionHouseByEntry(auctioneer->GetEntry());
    if(!AH) return;

    AH->SellItem(recv_data,GetPlayer());
    WorldPacket data = AH->GenerateOwnerList(GetPlayer());
    SendPacket(&data);
}
WorldPacket AuctionHouse::GenerateOwnerList(Player *pl)
{
    WorldPacket data;
    std::map<uint32,AuctionEntry*>::iterator itr;
    std::list<AuctionEntry*>tempList;
    std::list<AuctionEntry*>::iterator tempitr;
    uint32 cnt = 0;
    for (itr = GetAuctionsBegin();itr != GetAuctionsEnd();itr++)
    {
        if (itr->second->owner == pl->GetGUID())
        {
            tempList.push_back(itr->second);
        }
    }
    sLog.outString("sending owner list with %u items",cnt);
    data.Initialize( SMSG_AUCTION_OWNER_LIST_RESULT );
    if (tempList.size() < 51)
    {
        data << uint32(tempList.size());
    }
    else
    {
        data << uint32(50);
    }
    uint32 cnter = 1;
    for (tempitr = tempList.begin();tempitr != tempList.end();tempitr++)
    {
        if ((*tempitr)->owner == pl->GetGUID())
        {
            AuctionEntry *ae = GetAuction((*tempitr)->Id);
            Item *it = objmgr.GetAItem(ae->item);
            data = AddAuctionToPacket(data,ae,it);
        }
    }
    return data;

}
void WorldSession::HandleAuctionListOwnerItems( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 12);
    uint64 guid;
    uint32 unk1;
    recv_data >> guid >> unk1;
    Creature* auctioneer = sObjHolder.GetObject<Creature>(guid);
    if(!auctioneer) return;
    AuctionHouse * AH = sAuctionMgr.GetAuctionHouseByEntry(auctioneer->GetEntry());
    if(!AH) return;

    WorldPacket out = AH->GenerateOwnerList(GetPlayer());
    SendPacket(&out);
}

WorldPacket AuctionHouse::GenerateItemList( WorldPacket & recv_data )
{
   std::string auctionString;
    uint8 levelRange1, levelRange2, usableCheck;
    uint32 cnt, auctionSlotID, auctionMainCatagory, auctionSubCatagory, rarityCheck,temp;
    int32 startID;

    recv_data >> temp;
    recv_data >> auctionString;
    recv_data >> levelRange1 >> levelRange2;
    recv_data >> auctionSlotID >> auctionMainCatagory >> auctionSubCatagory;
    recv_data >> rarityCheck >> usableCheck;

    startID = temp;

    WorldPacket data;
    std::map<uint32,AuctionEntry*>::iterator itr;
    std::list<AuctionEntry*>templist;
    cnt = 0;
    levelRange2 == 0?levelRange2 = 100:levelRange2=levelRange2;
    uint32 tempcat1, tempcat2, temprarity, tempslot;
    for (itr = GetAuctionsBegin();itr != GetAuctionsEnd() && cnt < 50;itr++)
    {
        AuctionEntry *Aentry = itr->second;
        Item *it = objmgr.GetAItem(Aentry->item);
        tempcat1 = auctionMainCatagory;
        tempcat2 = auctionSubCatagory;
        temprarity = rarityCheck;
        tempslot = auctionSlotID;

        if (auctionMainCatagory == (0xffffffff))
            auctionMainCatagory = it->GetProto()->Class;

        if (auctionSlotID == (0xffffffff))
            auctionSlotID = it->GetProto()->InventoryType;
        
        if (rarityCheck == (0xffffffff))
            rarityCheck = it->GetProto()->Quality;
        
        if (auctionSubCatagory == (0xffffffff))
            auctionSubCatagory = it->GetProto()->SubClass;
        
        if ((startID-- <= 0) && (it->GetProto()->InventoryType == auctionSlotID) &&(it->GetProto()->Quality == rarityCheck) && (it->GetProto()->ItemLevel >= levelRange1) && (it->GetProto()->ItemLevel <= levelRange2) && (it->GetProto()->Class == auctionMainCatagory) && (it->GetProto()->SubClass == auctionSubCatagory))
            templist.push_back(itr->second);
        
        auctionMainCatagory = tempcat1;
        auctionSubCatagory = tempcat2;
        rarityCheck = temprarity;
        auctionSlotID = tempslot;
        tempcat1 = 0;
        tempcat2 = 0;
        temprarity = 0;
        tempslot = 0;
    }
    data.Initialize( SMSG_AUCTION_LIST_RESULT );
    data << uint32(templist.size());
    uint32 cnter = 0;
    std::list<AuctionEntry*>::iterator tempitr;
    for (tempitr = templist.begin();tempitr!=templist.end();tempitr++)
    {
        AuctionEntry *ae = GetAuction((*tempitr)->Id);
        Item *it = objmgr.GetAItem(ae->item);
        data = AddAuctionToPacket(data,ae,it);
    }
    templist.clear();
    return data;
}

void WorldSession::HandleAuctionListItems( WorldPacket & recv_data )
{
    uint64 guid;
    recv_data >> guid;
    Creature* auctioneer = sObjHolder.GetObject<Creature>(guid);
    if(!auctioneer) return;
    AuctionHouse * AH = sAuctionMgr.GetAuctionHouseByEntry(auctioneer->GetEntry());

    WorldPacket out = AH->GenerateItemList(recv_data);
    SendPacket(&out);
}

void AuctionHouse::LoadFromDB()
{
    std::stringstream sql;
    sql << "SELECT * FROM auctions WHERE auctionhouse = '" << GetId() << "'";
    QueryResult *result = sDatabase.Query( sql.str().c_str());

    if( !result )
        return;

    AuctionEntry* aItem;

    do
    {
        aItem = new AuctionEntry;
        Field *fields = result->Fetch();

        aItem->Id           = fields[0].GetUInt32();
        aItem->item         = fields[2].GetUInt64();
    
        Item * it=objmgr.LoadItem(aItem->item);
        if(it)
        objmgr.AddAItem(it);
    
        aItem->owner        = fields[3].GetUInt64();
        aItem->buyout       = fields[4].GetUInt32();
        aItem->time         = fields[5].GetUInt32();
        aItem->bidder       = fields[6].GetUInt64();
        aItem->bid          = fields[7].GetUInt32();
        aItem->deposit      = fields[8].GetUInt32();
        aItem->auctionhouse = fields[1].GetUInt32();
        Auctions[aItem->Id] = aItem;
    } while (result->NextRow());
    delete result;
}
