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

#ifndef __AUCTIONHOUSE_H
#define __AUCTIONHOUSE_H
struct AuctionEntry
{
    uint32 auctionhouse;
    uint64 item;
    uint64 owner;
    uint32 bid;
    uint32 buyout;
    time_t time;
    uint64 bidder;
    uint32 Id;
    uint32 deposit;
};



/*
SMSG_AUCTION_BIDDER_NOTIFICATION
uint32 auctionhouse;
uint32 auctionID;
uint64 bidderguid;
uint32 unk;
uint32 unk2;
uint32 itemid;
uint32 unk3



*/
struct bidentry
{
    uint32 AHid;
    uint32 AuctionID;
    uint32 amt;
};

class AuctionSystem: public WowdThread
{
public:
    AuctionSystem();
    void run();
};

enum AuctionRemoveType
{
    AUCTION_REMOVE_EXPIRED,
    AUCTION_REMOVE_WON,
    AUCTION_REMOVE_CANCELLED,
};
enum AUCTIONRESULT
{
    AUCTION_CREATE,// = 1
    AUCTION_CANCEL,// = 2
    AUCTION_BID,
    AUCTION_BUYOUT,

};
enum AuctionMailResult
{
    AUCTION_OUTBID,
    AUCTION_WON,
    AUCTION_SOLD,
    AUCTION_EXPIRED,
    AUCTION_EXPIRED2,
    AUCTION_CANCELLED,
};

class AuctionHouse
{
public:
    AuctionHouse(uint32 aid,uint32 guid, uint32 t, uint32 c);
    ~AuctionHouse();
    uint32 GetAuctioneer() { return auctioneerentry;};
    WorldPacket GenerateItemList(WorldPacket & recv_data);
    WorldPacket GenerateOwnerList(Player *pl);
    WorldPacket GenerateBidderList(Player *pl);
    void CancelAuction(WorldPacket & recv_data);
    std::map<uint32,AuctionEntry*>::iterator GetAuctionsBegin() { return Auctions.begin();};
    std::map<uint32,AuctionEntry*>::iterator GetAuctionsEnd() { return Auctions.end();};
    WorldPacket AddAuctionToPacket(WorldPacket pkt,AuctionEntry* ac,Item* it);
    WorldPacket BuildResultPacket(uint32 aid,uint32 result);
    WorldPacket BuildBidderNotificationPacket(AuctionEntry* ac);
    void SaveAuction(AuctionEntry* auction);
    AuctionEntry* GetAuction(uint32 id);
    void UpdateAuction(uint32 id);
    void Update();
    void RemoveAuction(uint32 id,uint32 reason);
    void LoadFromDB();
    void SellItem(WorldPacket & recv_data, Player *pl);
    void PlaceBid(WorldPacket & recv_data, Player *pl);
    uint32 GetId() { return Id;};
    uint32 GetMaxId() { return maxId++;};
    void LoadMaxId();
private:
    uint32 maxId;
    std::map<uint32,AuctionEntry*>Auctions;
    uint32 Id;
    uint32 auctioneerentry;
    uint32 tax;
    uint32 cut;
};

#endif
