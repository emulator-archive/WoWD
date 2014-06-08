#ifndef __AUCTIONMGR_H
#define __AUCTIONMGR_H

class AuctionMgr :  public Singleton < AuctionMgr >
{
public:
    AuctionMgr();
    ~AuctionMgr();
    void AddAuctionHouse(AuctionHouse* ah);
    AuctionHouse* GetAuctionHouseByEntry(uint32 entry);
    AuctionHouse* GetAuctionHouse(uint32 ahid);
    void LoadFromDB();
    void Update();
private:
    std::map<uint32,AuctionHouse*>AuctionMap;

};

#define sAuctionMgr AuctionMgr::getSingleton()

#endif
