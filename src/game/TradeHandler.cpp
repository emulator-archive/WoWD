/************************************************************************/
/* Copyright (C) 2006 Burlex for WoWd Project                           */
/************************************************************************/
#include "StdAfx.h"

void WorldSession::HandleInitiateTrade(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 8);
    Player * pTarget = World::GetPlayer(*(uint64*)&(recv_data.contents()[0]));
    uint32 TradeStatus = TRADE_STATUS_PROPOSED;
    WorldPacket data(SMSG_TRADE_STATUS, 12);

    if(pTarget == 0)
    {
        TradeStatus = TRADE_STATUS_PLAYER_NOT_FOUND;
        OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
        return;
    }

    // Handle possible error outcomes
    if(pTarget->CalcDistance(_player) > 10.0f)        // This needs to be checked
        TradeStatus = TRADE_STATUS_TOO_FAR_AWAY;
    else if(pTarget->isDead())
        TradeStatus = TRADE_STATUS_DEAD;
    else if(pTarget->mTradeTarget != 0)
        TradeStatus = TRADE_STATUS_ALREADY_TRADING;
    else if(pTarget->GetTeam() != _player->GetTeam() && GetPermissionCount() == 0)
        TradeStatus = TRADE_STATUS_WRONG_FACTION;

    data << TradeStatus;

    if(TradeStatus == TRADE_STATUS_PROPOSED)
    {
        _player->ResetTradeVariables();
        pTarget->ResetTradeVariables();

        pTarget->mTradeTarget = _player;
        _player->mTradeTarget = pTarget;

        pTarget->mTradeStatus = TradeStatus;
        _player->mTradeStatus = TradeStatus;

        data << _player->GetGUID();
    }

    pTarget->m_session->SendPacket(&data);
}

void WorldSession::HandleBeginTrade(WorldPacket & recv_data)
{
    uint32 TradeStatus = TRADE_STATUS_INITIATED;

    if(_player->mTradeTarget == 0)
    {
        TradeStatus = TRADE_STATUS_PLAYER_NOT_FOUND;
        OutPacket(TRADE_STATUS_PLAYER_NOT_FOUND, 4, &TradeStatus);
        return;
    }

    OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
    _player->mTradeTarget->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);

    _player->mTradeTarget->mTradeStatus = TradeStatus;
    _player->mTradeStatus = TradeStatus;
}

void WorldSession::HandleBusyTrade(WorldPacket & recv_data)
{
    uint32 TradeStatus = TRADE_STATUS_PLAYER_BUSY;

    if(_player->mTradeTarget == 0)
    {
        TradeStatus = TRADE_STATUS_PLAYER_NOT_FOUND;
        OutPacket(TRADE_STATUS_PLAYER_NOT_FOUND, 4, &TradeStatus);
        return;
    }

    OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
    _player->mTradeTarget->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);

    _player->mTradeTarget->mTradeStatus = TradeStatus;
    _player->mTradeStatus = TradeStatus;

    _player->mTradeTarget->mTradeTarget = 0;
    _player->mTradeTarget = 0;
}

void WorldSession::HandleIgnoreTrade(WorldPacket & recv_data)
{
    uint32 TradeStatus = TRADE_STATUS_PLAYER_IGNORED;

    if(_player->mTradeTarget == 0)
    {
        TradeStatus = TRADE_STATUS_PLAYER_NOT_FOUND;
        OutPacket(TRADE_STATUS_PLAYER_NOT_FOUND, 4, &TradeStatus);
        return;
    }

    OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
    _player->mTradeTarget->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);

    _player->mTradeTarget->mTradeStatus = TradeStatus;
    _player->mTradeStatus = TradeStatus;

    _player->mTradeTarget->mTradeTarget = 0;
    _player->mTradeTarget = 0;
}

void WorldSession::HandleCancelTrade(WorldPacket & recv_data)
{
    if(_player->mTradeTarget == 0 || _player->mTradeStatus == TRADE_STATUS_COMPLETE)
        return;

    uint32 TradeStatus = TRADE_STATUS_CANCELLED;
    OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
    _player->mTradeTarget->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
    
    _player->mTradeTarget->mTradeTarget = 0;
    _player->mTradeTarget = 0;
}

void WorldSession::HandleUnacceptTrade(WorldPacket & recv_data)
{
    if(_player->mTradeTarget == 0)
        return;

    uint32 TradeStatus = TRADE_STATUS_UNACCEPTED;
    OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
    _player->mTradeTarget->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);

    _player->mTradeTarget->mTradeTarget = 0;
    _player->mTradeTarget = 0;
}

void WorldSession::HandleSetTradeItem(WorldPacket & recv_data)
{
    if(_player->mTradeTarget == 0)
        return;

    CHECK_PACKET_SIZE(recv_data, 3);

    uint8 TradeSlot = recv_data.contents()[0];
    uint8 SourceBag = recv_data.contents()[1];
    uint8 SourceSlot = recv_data.contents()[2];

    Item * pItem = _player->GetItemInterface()->GetInventoryItem(SourceBag, SourceSlot);
    if(pItem == 0 || TradeSlot > 6)
        return;

    _player->mTradeItems[TradeSlot] = pItem;
    _player->SendTradeUpdate();
}

void WorldSession::HandleSetTradeGold(WorldPacket & recv_data)
{
    if(_player->mTradeTarget == 0)
        return;

    uint32 Gold = *(uint32*)recv_data.contents();

    if(_player->mTradeGold != Gold)
    {
        _player->mTradeGold = (Gold > _player->GetUInt32Value(PLAYER_FIELD_COINAGE) ? _player->GetUInt32Value(PLAYER_FIELD_COINAGE) : Gold);
        _player->SendTradeUpdate();
    }
}

void WorldSession::HandleClearTradeItem(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 1);
    if(_player->mTradeTarget == 0)
        return;

    uint8 TradeSlot = recv_data.contents()[0];
    if(TradeSlot > 6)
        return;

    _player->mTradeItems[TradeSlot] = 0;
    _player->SendTradeUpdate();
}

void WorldSession::HandleAcceptTrade(WorldPacket & recv_data)
{
    if(_player->mTradeTarget == 0)
        return;

    uint32 TradeStatus = TRADE_STATUS_ACCEPTED;
    
    // Tell the other player we're green.
    _player->mTradeTarget->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
    _player->mTradeStatus = TradeStatus;

    if(_player->mTradeTarget->mTradeStatus == TRADE_STATUS_ACCEPTED)
    {
        // Ready!
        uint32 ItemCount = 0;
        uint32 TargetItemCount = 0;
        Player * pTarget = _player->mTradeTarget;

        // Calculate Item Count
        for(uint32 Index = 0; Index < 7; ++Index)
        {
            if(_player->mTradeItems[Index] != 0)    ++ItemCount;
            if(pTarget->mTradeItems[Index] != 0)    ++TargetItemCount;
        }

        if( (_player->m_ItemInterface->CalculateFreeSlots(NULL) + ItemCount) < TargetItemCount ||
            (pTarget->m_ItemInterface->CalculateFreeSlots(NULL) + TargetItemCount) < ItemCount )
        {
            // Not enough slots on one end.
            TradeStatus = TRADE_STATUS_CANCELLED;
        }
        else
        {
            TradeStatus = TRADE_STATUS_COMPLETE;
            uint64 Guid;
            Item * pItem;
            
            // Remove all items from the players inventory
            for(uint32 Index = 0; Index < 6; ++Index)
            {
                Guid = _player->mTradeItems[Index] ? _player->mTradeItems[Index]->GetGUID() : 0;
                if(Guid != 0)
                    pItem = _player->m_ItemInterface->SafeRemoveAndRetreiveItemByGuid(Guid, true);

                Guid = pTarget->mTradeItems[Index] ? pTarget->mTradeItems[Index]->GetGUID() : 0;
                if(Guid != 0)
                    pTarget->m_ItemInterface->SafeRemoveAndRetreiveItemByGuid(Guid, true);
            }

            // Dump all items back into the opposite players inventory
            for(uint32 Index = 0; Index < 6; ++Index)
            {
                pItem = _player->mTradeItems[Index];
                if(pItem != 0)
                {
                    pItem->SetOwner(pTarget);
                    pTarget->m_ItemInterface->AddItemToFreeSlot(pItem);
                }

                pItem = pTarget->mTradeItems[Index];
                if(pItem != 0)
                {
                    pItem->SetOwner(pTarget);
                    _player->m_ItemInterface->AddItemToFreeSlot(pItem);
                }
            }

            // Trade Gold
            if(pTarget->mTradeGold)
            {
                _player->ModUInt32Value(PLAYER_FIELD_COINAGE, pTarget->mTradeGold);
                pTarget->ModUInt32Value(PLAYER_FIELD_COINAGE, -(int32)pTarget->mTradeGold);
            }

            if(_player->mTradeGold)
            {
                pTarget->ModUInt32Value(PLAYER_FIELD_COINAGE, _player->mTradeGold);
                _player->ModUInt32Value(PLAYER_FIELD_COINAGE, -(int32)_player->mTradeGold);
            }

            // Close Window
            TradeStatus = TRADE_STATUS_COMPLETE;
            OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);
            _player->mTradeTarget->m_session->OutPacket(SMSG_TRADE_STATUS, 4, &TradeStatus);

            _player->mTradeStatus = TRADE_STATUS_COMPLETE;
            _player->mTradeTarget->mTradeStatus = TRADE_STATUS_COMPLETE;

            // Reset Trade Vars
            _player->ResetTradeVariables();
            pTarget->ResetTradeVariables();
            
            _player->mTradeTarget->mTradeTarget = 0;
            _player->mTradeTarget = 0;
        }
    }
}
