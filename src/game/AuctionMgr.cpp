#include "StdAfx.h"

initialiseSingleton( AuctionMgr );

AuctionMgr::AuctionMgr()
{
    LoadFromDB();
}
AuctionMgr::~AuctionMgr()
{
    sLog.outString("  Removing Auction Houses...");
    std::set<AuctionHouse*> deleted;
    for(std::map<uint32, AuctionHouse*>::iterator itr = AuctionMap.begin(); itr != AuctionMap.end(); ++itr)
    {
        if(deleted.count(itr->second) == 0)
        {
            deleted.insert(itr->second);
            delete itr->second;
        }
    }

    AuctionMap.clear();
    sLog.outString("");
}
void AuctionMgr::AddAuctionHouse(AuctionHouse* ah)
{
    AuctionMap[ah->GetAuctioneer()] = ah;
}
AuctionHouse* AuctionMgr::GetAuctionHouseByEntry(uint32 entry)
{
    if(AuctionMap[entry])
        return AuctionMap[entry];
    else
        return NULL;
}
AuctionHouse* AuctionMgr::GetAuctionHouse(uint32 ahid)
{
    std::map<uint32,AuctionHouse*>::iterator itr;
    for(itr = AuctionMap.begin();itr != AuctionMap.end();itr++)
    {
        if(itr->second->GetId() == ahid)
            return itr->second;
    }
    return NULL;
}

void AuctionMgr::Update()
{
    std::map<uint32,AuctionHouse*>::iterator itr;
    for(itr = AuctionMap.begin(); itr != AuctionMap.end();itr++)
    {
        if(itr->second)
            itr->second->Update();
    }
}


void AuctionMgr::LoadFromDB()
{
    QueryResult *pResult = sDatabase.Query("SELECT * FROM auctionhouse");
    if(pResult)
    {
        do{
            Field *Field = pResult->Fetch();
            AuctionHouse* AH = GetAuctionHouse(Field[2].GetUInt32());
            if(AH)
                AuctionMap[Field[1].GetUInt32()] = AH;
            else
            {
                AH = new AuctionHouse(Field[2].GetUInt32(),Field[1].GetUInt32(),sAuctionHouseStore.LookupEntry(Field[2].GetUInt32())->tax,sAuctionHouseStore.LookupEntry(Field[2].GetUInt32())->fee);
                AddAuctionHouse(AH);
            }
        }while(pResult->NextRow());
        delete pResult;
    }
}
