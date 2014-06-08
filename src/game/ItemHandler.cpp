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

void WorldSession::HandleSplitOpcode(WorldPacket& recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 5);
    int8 DstInvSlot=0, DstSlot=0, SrcInvSlot=0, SrcSlot=0, count=0;
    bool result;

    recv_data >> SrcInvSlot >> SrcSlot >> DstInvSlot >> DstSlot >> count;
    if(!GetPlayer())
        return;

    int32 c=count;
    Item *i1 =_player->GetItemInterface()->GetInventoryItem(SrcInvSlot,SrcSlot);
    if(!i1)
        return;
    Item *i2=_player->GetItemInterface()->GetInventoryItem(DstInvSlot,DstSlot);

    if(i2)//smth already in this slot
    {
        if(i1->GetEntry()==i2->GetEntry() )
        {
            //check if player has the required stacks to avoid exploiting.
            //safe exploit check
            if(c < i1->GetUInt32Value(ITEM_FIELD_STACK_COUNT))
            {
                //check if there is room on the other item.
                if(((c + i2->GetUInt32Value(ITEM_FIELD_STACK_COUNT)) <= i2->GetProto()->MaxCount))
                {
                    i1->ModUInt32Value(ITEM_FIELD_STACK_COUNT, -c);
                    i2->ModUInt32Value(ITEM_FIELD_STACK_COUNT, +c);
                }
                else
                {
                     GetPlayer()->GetItemInterface()->BuildInventoryChangeError(i1, i2, INV_ERR_ITEM_CANT_STACK);
                }
            }
            else
            {
                //error cant split item
                _player->GetItemInterface()->BuildInventoryChangeError(i1, i2, INV_ERR_COULDNT_SPLIT_ITEMS);
            }
        }
        else
        {
             GetPlayer()->GetItemInterface()->BuildInventoryChangeError(i1, i2, INV_ERR_ITEM_CANT_STACK);
        }
    }
    else
    {
        if(c < i1->GetUInt32Value(ITEM_FIELD_STACK_COUNT))
        {
            i1->ModUInt32Value(ITEM_FIELD_STACK_COUNT,-c);

            i2=objmgr.CreateItem(i1->GetEntry(),_player);
            i2->SetUInt32Value(ITEM_FIELD_STACK_COUNT,c);

            result = _player->GetItemInterface()->SafeAddItem(i2,DstInvSlot,DstSlot);
            if(!result)
            {
                printf("HandleBuyItemInSlot: Error while adding item to dstslot");
            }
        }
        else
        {
            _player->GetItemInterface()->BuildInventoryChangeError(i1, i2, INV_ERR_COULDNT_SPLIT_ITEMS);
        }
    }
}

void WorldSession::HandleSwapItemOpcode(WorldPacket& recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 4);
    WorldPacket data;
    WorldPacket packet;
    Item *SrcItem = NULL;
    Item *DstItem = NULL;

    Item *SrcTemp = NULL;
    Item *DstTemp = NULL;

    int8 DstInvSlot=0, DstSlot=0, SrcInvSlot=0, SrcSlot=0, error=0;
    //     20           5            255      26

    if(!GetPlayer())
        return;
    
    recv_data >> DstInvSlot >> DstSlot >> SrcInvSlot >> SrcSlot;

    sLog.outDetail("ITEM: swap, DstInvSlot %u DstSlot %u SrcInvSlot %u SrcSlot %u", (uint32)DstInvSlot, (uint32)DstSlot, (uint32)SrcInvSlot, (uint32)SrcSlot);

    if(DstInvSlot == SrcSlot && SrcInvSlot == -1) // player trying to add self container to self container slots
    {
        GetPlayer()->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_ITEMS_CANT_BE_SWAPPED);
        return;
    }

    SrcItem=_player->GetItemInterface()->GetInventoryItem(SrcInvSlot,SrcSlot);
    if(!SrcItem)
        return;

    DstItem=_player->GetItemInterface()->GetInventoryItem(DstInvSlot,DstSlot);

    if(DstItem)
    {   //check if it will go to equipment slot
        if(SrcInvSlot==INVENTORY_SLOT_NOT_SET)//not bag
        {
            if(DstItem->IsContainer())
            {
                if(((Container*)DstItem)->HasItems())
                {
                    if(SrcSlot < INVENTORY_SLOT_BAG_START || SrcSlot >= INVENTORY_SLOT_BAG_END || SrcSlot < BANK_SLOT_BAG_START || SrcSlot >= BANK_SLOT_BAG_END)
                    {
                        _player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
                        return;
                    }
                }
            }

            if(SrcSlot <  INVENTORY_KEYRING_END)
            {
                if(error=GetPlayer()->GetItemInterface()->CanEquipItemInSlot(SrcInvSlot, SrcSlot, DstItem->GetProto()))
                {
                    _player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, error);
                    return;
                }
            }
        }
        else
        {
            if(DstItem->IsContainer())
            {
                if(((Container*)DstItem)->HasItems())
                {
                    _player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
                    return;
                }
            }

            if(error=GetPlayer()->GetItemInterface()->CanEquipItemInSlot(SrcInvSlot, SrcInvSlot, DstItem->GetProto()))
            {
                _player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, error);
                return;
            }
        }
    }

    if(SrcItem)
    {   //check if it will go to equipment slot
        if(DstInvSlot==INVENTORY_SLOT_NOT_SET)//not bag
        {
            if(SrcItem->IsContainer())
            {
                if(((Container*)SrcItem)->HasItems())
                {
                    if(DstSlot < INVENTORY_SLOT_BAG_START || DstSlot >= INVENTORY_SLOT_BAG_END || DstSlot < BANK_SLOT_BAG_START || DstSlot >= BANK_SLOT_BAG_END)
                    {
                        _player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
                        return;
                    }
                }
            }

            if(DstSlot <  INVENTORY_KEYRING_END)
            {
                if(error=GetPlayer()->GetItemInterface()->CanEquipItemInSlot(DstInvSlot, DstSlot, SrcItem->GetProto()))
                {
                    _player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, error);
                    return;
                }
            } 
        }
        else
        {
            if(SrcItem->IsContainer())
            {
                if(((Container*)SrcItem)->HasItems())
                {
                    _player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
                    return;
                }
            }

            if(error=GetPlayer()->GetItemInterface()->CanEquipItemInSlot(DstInvSlot, DstInvSlot, SrcItem->GetProto()))
            {
                _player->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, error);
                return;
            }
        }
    }

    if(GetPlayer()->isInCombat())
    {
        GetPlayer()->GetItemInterface()->BuildInventoryChangeError(SrcItem, NULL, INV_ERR_CANT_DO_IN_COMBAT);
        return;
    }

    if(SrcInvSlot==DstInvSlot)//in 1 bag
    {
        if(SrcInvSlot == INVENTORY_SLOT_NOT_SET)//in backpack
        {
            _player->GetItemInterface()->SwapItemSlots(SrcSlot,DstSlot);
        }
        else//in bag
        {
            ((Container*)_player->GetItemInterface()->GetInventoryItem(SrcInvSlot))->SwapItems(SrcSlot,DstSlot);
        }
    }
    else
    {
        // this is done in CanEquipItemInSlot ;)

        /*if (DstInvSlot != INVENTORY_SLOT_NOT_SET) 
        {
            uint32 DstInvSubClass = _player->GetItemInterface()->GetInventoryItem(DstInvSlot)->GetProto()->SubClass;
            uint32 SrcItemClass = SrcItem->GetProto()->Class;
            uint32 SrcItemSubClass = SrcItem->GetProto()->SubClass;
            uint32 DstInvClass = _player->GetItemInterface()->GetInventoryItem(DstInvSlot)->GetProto()->Class;

            // if its not ammo/arrows it shouldnt go there
            if( DstInvSubClass != 0  && SrcItemSubClass != DstInvSubClass || 
                ( SrcItemClass == 11 && DstInvClass == 11 ) ) 
            {
                _player->GetItemInterface()->BuildInventoryChangeError(SrcItem, NULL, INV_ERR_ONLY_AMMO_CAN_GO_HERE);
                return;            
            }
        }*/

        //Check for stacking
        if(DstItem && SrcItem->GetEntry()==DstItem->GetEntry() && SrcItem->GetProto()->MaxCount>1)
        {
            uint32 total=SrcItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT)+DstItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
            if(total<=DstItem->GetProto()->MaxCount)
            {
                DstItem->ModUInt32Value(ITEM_FIELD_STACK_COUNT,SrcItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT));
                bool result = _player->GetItemInterface()->SafeFullRemoveItemFromSlot(SrcInvSlot,SrcSlot);
                if(!result)
                {
                    GetPlayer()->GetItemInterface()->BuildInventoryChangeError(SrcItem, DstItem, INV_ERR_ITEM_CANT_STACK);
                }
                return;
            }
            else
            {
                if(DstItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT) == DstItem->GetProto()->MaxCount)
                {

                }
                else
                {
                    int32 delta=DstItem->GetProto()->MaxCount-DstItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
                    DstItem->SetUInt32Value(ITEM_FIELD_STACK_COUNT,DstItem->GetProto()->MaxCount);
                    SrcItem->ModUInt32Value(ITEM_FIELD_STACK_COUNT,-delta);
                    return;
                }
            }
        }
       
        if(SrcItem)
            SrcItem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInvSlot,SrcSlot, false);

        if(DstItem)
            DstItem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(DstInvSlot,DstSlot, false);

        if(SrcItem)
        {
            bool result =_player->GetItemInterface()->SafeAddItem(SrcItem,DstInvSlot,DstSlot);
            if(!result)
            {
                printf("HandleSwapItem: Error while adding item to dstslot\n");
            }
        }

        if(DstItem)
        {
            bool result = _player->GetItemInterface()->SafeAddItem(DstItem,SrcInvSlot,SrcSlot);
            if(!result)
            {
                printf("HandleSwapItem: Error while adding item to srcslot\n");
            }
        }
    }
}

void WorldSession::HandleSwapInvItemOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 2);
    WorldPacket data;
    int8 srcslot=0, dstslot=0;
    int8 error=0;

    recv_data >> srcslot >> dstslot;

    if(!GetPlayer())
        return;

    sLog.outDetail("ITEM: swap, src slot: %u dst slot: %u", (uint32)srcslot, (uint32)dstslot);

    if(dstslot == srcslot) // player trying to add item to the same slot
    {
        GetPlayer()->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_ITEMS_CANT_BE_SWAPPED);
        return;
    }

    Item * dstitem = _player->GetItemInterface()->GetInventoryItem(dstslot);
    Item * srcitem = _player->GetItemInterface()->GetInventoryItem(srcslot);
    
    // allow weapon switching in combat
    bool skip_combat = false;
    if( srcslot < EQUIPMENT_SLOT_END || dstslot < EQUIPMENT_SLOT_END )      // We're doing an equip swap.
    {
        if(_player->isInCombat())
        {
            if( srcslot < EQUIPMENT_SLOT_MAINHAND || dstslot < EQUIPMENT_SLOT_MAINHAND )    // These can't be swapped
            {
                _player->GetItemInterface()->BuildInventoryChangeError(srcitem, dstitem, INV_ERR_CANT_DO_IN_COMBAT);
                return;
            }
            skip_combat= true;
        }
    }

    if (!srcitem)
    {
        _player->GetItemInterface()->BuildInventoryChangeError(srcitem,dstitem,INV_ERR_YOU_CAN_NEVER_USE_THAT_ITEM);
        return;
    }

    if (srcslot == dstslot)
    {
        _player->GetItemInterface()->BuildInventoryChangeError(srcitem,dstitem, INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
        return;
    }
    
    if(error=_player->GetItemInterface()->CanEquipItemInSlot(INVENTORY_SLOT_NOT_SET, dstslot, srcitem->GetProto(), skip_combat))
    {
        if(dstslot < INVENTORY_KEYRING_END)
        {
            _player->GetItemInterface()->BuildInventoryChangeError(srcitem,dstitem, error);
            return;
        }
    }

    if(dstitem)
    {
        if(error=_player->GetItemInterface()->CanEquipItemInSlot(INVENTORY_SLOT_NOT_SET, srcslot, dstitem->GetProto(), skip_combat))
        {
            if(srcslot < INVENTORY_KEYRING_END)
            {
                data.Initialize( SMSG_INVENTORY_CHANGE_FAILURE );
                data << error;
                if(error == 1) 
                {
                    data << dstitem->GetProto()->RequiredLevel;
                }
                data << (srcitem ? srcitem->GetGUID() : uint64(0));
                data << (dstitem ? dstitem->GetGUID() : uint64(0));
                data << uint8(0);

                SendPacket( &data );
                return;
            }
        }
    }

    if(srcitem->IsContainer())
    {
        //source has items and dst is a backpack or bank
        if(((Container*)srcitem)->HasItems())
            if(!_player->GetItemInterface()->IsBagSlot(dstslot))
            {
                _player->GetItemInterface()->BuildInventoryChangeError(srcitem,dstitem, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
                return;
            }

        if(dstitem)
        {
            //source is a bag and dst slot is a bag inventory and has items
            if(dstitem->IsContainer())
            {
                if(((Container*)dstitem)->HasItems() && !_player->GetItemInterface()->IsBagSlot(srcslot))
                {
                    _player->GetItemInterface()->BuildInventoryChangeError(srcitem,dstitem, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
                    return;
                }
            }
            else
            {
                //dst item is not a bag, swap impossible
                _player->GetItemInterface()->BuildInventoryChangeError(srcitem,dstitem,INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
                return;
            }
        }

        //dst is bag inventory
        if(dstslot < INVENTORY_SLOT_BAG_END)
        {
            if(srcitem->GetProto()->Bonding==ITEM_BIND_ON_EQUIP)
                srcitem->SoulBind();
        }
    }

    // swap items
    _player->GetItemInterface()->SwapItemSlots(srcslot, dstslot);
}

void WorldSession::HandleDestroyItemOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 2);
    Player *plyr = GetPlayer();
       if(!GetPlayer())
        return;

    int8 SrcInvSlot, SrcSlot;

    recv_data >> SrcInvSlot >> SrcSlot;

    sLog.outDetail("ITEM: destroy, SrcInv Slot: %u Src slot: %u", (uint32)SrcInvSlot, (uint32)SrcSlot);
    Item *it = _player->GetItemInterface()->GetInventoryItem(SrcInvSlot,SrcSlot);

    if(it)
    {
        if(it->GetProto()->ItemId == 5863)
        {
            Charter *gc = objmgr.GetGuildCharter( _player->GetGUID() );
            if(gc)
            {
                for(uint32 x=0;x<9;x++)
                    {    
                        uint64 guid=gc->signer[x];
                        if(!guid)continue;
                        Player *plr = objmgr.GetPlayer(guid);
                        if(plr)plr->UnSetCharterSigned();
                    }
                    
                      objmgr.DeleteCharter(plyr->GetGUID());
                   
            }
            plyr->UnSetCharterSigned();
        }
        uint32 mail_id = it->GetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID);
        if(mail_id)
            sMailSystem.RemoveMessageIfDeleted(mail_id, _player->GetGUID());
        
        bool result =  _player->GetItemInterface()->SafeFullRemoveItemFromSlot(SrcInvSlot,SrcSlot);
        if(!result)
        {
            sLog.outDetail("ITEM: Destroy, SrcInv Slot: %u Src slot: %u Failed", (uint32)SrcInvSlot, (uint32)SrcSlot);
        }
    }
}

void WorldSession::HandleAutoEquipItemOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 2);
    WorldPacket data;

       bool result;
    int8 SrcInvSlot, SrcSlot, error=0;
    
    if(!GetPlayer())
        return;

    recv_data >> SrcInvSlot >> SrcSlot;

    sLog.outDetail("ITEM: autoequip, Inventory slot: %u Source Slot: %u", (uint32)SrcInvSlot, (uint32)SrcSlot);  

    Item *eitem=_player->GetItemInterface()->GetInventoryItem(SrcInvSlot,SrcSlot);

    if(!eitem) 
    {
        _player->GetItemInterface()->BuildInventoryChangeError(eitem, NULL, INV_ERR_ITEM_NOT_FOUND);
        return;
    }

    int8 Slot = _player->GetItemInterface()->GetItemSlotByType(eitem->GetProto()->InventoryType);
    if(Slot == ITEM_NO_SLOT_AVAILABLE)
    {
        _player->GetItemInterface()->BuildInventoryChangeError(eitem,NULL,INV_ERR_ITEM_CANT_BE_EQUIPPED);
        return;
    }

    if(error = _player->GetItemInterface()->CanEquipItemInSlot(INVENTORY_SLOT_NOT_SET, Slot, eitem->GetProto()))
    {
        _player->GetItemInterface()->BuildInventoryChangeError(eitem,NULL, error);
        return;
    }

    Item*oitem=NULL;

    if(SrcInvSlot == INVENTORY_SLOT_NOT_SET)
    {
        _player->GetItemInterface()->SwapItemSlots(SrcSlot, Slot);
    }
    else
    {
        eitem=_player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInvSlot,SrcSlot, false);
        oitem=_player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, Slot, false);
        if(oitem)
        {
            result = _player->GetItemInterface()->SafeAddItem(oitem,SrcInvSlot,SrcSlot);
            if(!result)
            {
                printf("HandleAutoEquip: Error while adding item to SrcSlot");
            }
        }
        result = _player->GetItemInterface()->SafeAddItem(eitem, INVENTORY_SLOT_NOT_SET, Slot);
        if(!result)
        {
            printf("HandleAutoEquip: Error while adding item to Slot");
        }
        
    }

    if(eitem->GetProto()->Bonding==ITEM_BIND_ON_EQUIP)
        eitem->SoulBind();       
}

void WorldSession::HandleItemQuerySingleOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 12);
    if(!GetPlayer())
        return;

    int i;
    uint32 itemid=0, guidlow=0, guidhigh=0;
    recv_data >> itemid >> guidlow >> guidhigh; // guid is the guid of the ITEM OWNER - NO ITS NOT

    sLog.outDetail( "WORLD: Recvd CMSG_ITEM_QUERY_SINGLE for item id %d, guid 0x%.8X 0x%.8X",
        itemid, guidlow, guidhigh );

    ItemPrototype *itemProto = objmgr.GetItemPrototype(itemid);
    if(!itemProto)
    {
        sLog.outError( "WORLD: Unknown item id 0x%.8X", itemid );
        return;
    } 

    WorldPacket data(SMSG_ITEM_QUERY_SINGLE_RESPONSE, 515 + itemProto->Name1.length() + itemProto->Description.length() );
    data << itemProto->ItemId;
    data << itemProto->Class;
    data << itemProto->SubClass;
    data << itemProto->unknown_bc;
    data << itemProto->Name1.c_str();
    data << uint8(0) << uint8(0) << uint8(0); // name 2,3,4
    data << itemProto->DisplayInfoID;
    data << itemProto->Quality;
    data << itemProto->Flags;
    data << itemProto->BuyPrice;
    data << itemProto->SellPrice;
    data << itemProto->InventoryType;
    data << itemProto->AllowableClass;
    data << itemProto->AllowableRace;
    data << itemProto->ItemLevel;
    data << itemProto->RequiredLevel;
    data << itemProto->RequiredSkill;
    data << itemProto->RequiredSkillRank;
    data << itemProto->RequiredSkillSubRank;
    data << itemProto->RequiredPlayerRank1;
    data << itemProto->RequiredPlayerRank2;
    data << itemProto->RequiredFaction;
    data << itemProto->RequiredFactionStanding;
    data << itemProto->Unique;
    data << itemProto->MaxCount;
    data << itemProto->ContainerSlots;
    for(i = 0; i < 10; i++)
    {
        data << itemProto->ItemStatType[i];
        data << itemProto->ItemStatValue[i];
    }
    for(i = 0; i < 5; i++)
    {
        data << itemProto->DamageMin[i];
        data << itemProto->DamageMax[i];
        data << itemProto->DamageType[i];
    }
    data << itemProto->Armor;
    data << itemProto->HolyRes;
    data << itemProto->FireRes;
    data << itemProto->NatureRes;
    data << itemProto->FrostRes;
    data << itemProto->ShadowRes;
    data << itemProto->ArcaneRes;
    data << itemProto->Delay;
    data << itemProto->AmmoType;
    data << itemProto->Range;
    for(i = 0; i < 5; i++) {
        data << itemProto->SpellId[i];
        data << itemProto->SpellTrigger[i];
        data << itemProto->SpellCharges[i];
        data << itemProto->SpellCooldown[i];
        data << itemProto->SpellCategory[i];
        data << itemProto->SpellCategoryCooldown[i];
    }
    data << itemProto->Bonding;
    data << itemProto->Description.c_str();
    data << itemProto->PageId;
    data << itemProto->PageLanguage;
    data << itemProto->PageMaterial;
    data << itemProto->QuestId;
    data << itemProto->LockId;
    data << itemProto->LockMaterial;
    data << itemProto->Field108;
    data << itemProto->RandomPropId;
    data << itemProto->RandomPropId_2;
    data << itemProto->Block;
    data << itemProto->ItemSet;
    data << itemProto->MaxDurability;
    data << itemProto->ZoneNameID;
    data << itemProto->Field114;
    data << itemProto->BagFamily;
    data << itemProto->ToolCategory;
    data << itemProto->SocketColor1;
    data << itemProto->Unk201_3;
    data << itemProto->SocketColor2;
    data << itemProto->Unk201_5;
    data << itemProto->SocketColor3;
    data << itemProto->Unk201_7;
    data << itemProto->SocketBonus;
    data << itemProto->GemProperties;
    data << itemProto->ItemExtendedCost;
    data << itemProto->DisenchantReqSkill;
    //WPAssert(data.size() == 453 + itemProto->Name1.length() + itemProto->Description.length());
    SendPacket( &data );
}

void WorldSession::HandleBuyBackOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 8);
    WorldPacket data;
    uint64 guid;
    uint32 stuff;
    Item* add ;
    bool result;

    recv_data >> guid >> stuff;
    stuff -= 74;

    // prevent crashes
    if(stuff < 0 || stuff > MAX_BUYBACK_SLOT)
        return;

    //what a magical number 69???
    Item *it = _player->GetItemInterface()->GetBuyBack(stuff);
    if (it)
    {
        // Find free slot and break if inv full
        uint32 amount = it->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
        uint32 itemid = it->GetUInt32Value(OBJECT_FIELD_ENTRY);
      
        add = _player->GetItemInterface()->FindItemLessMax(itemid,amount, false);
     
           uint32 FreeSlots = _player->GetItemInterface()->CalculateFreeSlots(it->GetProto());
        if ((FreeSlots == 0) && (!add))
        {
            _player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_INVENTORY_FULL);
            return;
        }
        
        // Check for gold
        int32 cost =_player->GetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + stuff);
        if(_player->GetUInt32Value(PLAYER_FIELD_COINAGE) < cost )
        {
            data.Initialize( SMSG_BUY_FAILED );
            data << uint64(guid);
            data << uint32(itemid);
            data << uint8(2); //not enough money
            SendPacket( &data );
            return;
        }
        _player->ModUInt32Value( PLAYER_FIELD_COINAGE , -cost);
        if (!add)
        {
            result = _player->GetItemInterface()->AddItemToFreeSlot(it);
            if(!result)
            {
                printf("HandleBuyBack: Error while adding item to free slot");
            }
        }
        else
        {
            add->SetCount(add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + amount);
        }

        data.Initialize( SMSG_BUY_ITEM );
        data << uint64(guid);
        data << uint32(itemid) << uint32(amount);
        SendPacket( &data );
    }
    _player->GetItemInterface()->RemoveBuyBackItem(stuff);
}

void WorldSession::HandleSellItemOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 17);
    sLog.outDetail( "WORLD: Recieved CMSG_SELL_ITEM" );
    if(!GetPlayer())
        return;

    WorldPacket data;
    uint64 vendorguid=0, itemguid=0;
    int8 amount=0;
    //uint8 slot = INVENTORY_NO_SLOT_AVAILABLE;
    //uint8 bagslot = INVENTORY_NO_SLOT_AVAILABLE;
    //int check = 0;

    recv_data >> vendorguid;
    recv_data >> itemguid;
    recv_data >> amount;

    if(_player->isCasting())
        _player->InterruptSpell();

    // Check if item exists // burlex: we should really have an enum for these..
    if(!itemguid)
    {
        SendSellItem(vendorguid, itemguid, 1);
        return;
    }

    Creature *unit = sObjHolder.GetObject<Creature>(vendorguid);
    // Check if Vendor exists
    if (unit == NULL)
    {
        SendSellItem(vendorguid, itemguid, 3);
        return;
    }

    Item* item = _player->GetItemInterface()->GetItemByGUID(itemguid);
    if(!item)
    {
        SendSellItem(vendorguid, itemguid, 1);
        return; //our player doesn't have this item
    }

    ItemPrototype *it = item->GetProto();
    if(!it)
    {
        SendSellItem(vendorguid, itemguid, 2);
        return; //our player doesn't have this item
    }

    // Check if item can be sold
    if (it->SellPrice == 0)
    {
        SendSellItem(vendorguid, itemguid, 2);
        return;
    }
    
    uint32 stackcount = item->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
    uint32 quantity = 0;

    if (amount != 0)
    {
        quantity = amount;
    }
    else
    {
        quantity = stackcount; //allitems
    }

    if(quantity > stackcount) quantity = stackcount; //make sure we don't over do it
    
    uint32 price = GetSellPriceForItem(it, quantity);

    _player->ModUInt32Value(PLAYER_FIELD_COINAGE,price);
 
    if(quantity < stackcount)
    {
        item->SetCount(stackcount - quantity);
    }
    else
    {
        //removing the item from the char's inventory
        item = _player->GetItemInterface()->SafeRemoveAndRetreiveItemByGuid(itemguid, false); //again to remove item from slot
        if(item)
        {
            _player->GetItemInterface()->AddBuyBackItem(item,(it->SellPrice) * quantity);
        }
    }

    data.Initialize( SMSG_SELL_ITEM );
    data << vendorguid << itemguid << uint8(0); 
    SendPacket( &data );

    sLog.outDetail( "WORLD: Sent SMSG_SELL_ITEM" );
}

void WorldSession::HandleBuyItemInSlotOpcode( WorldPacket & recv_data ) // drag & drop
{
    CHECK_PACKET_SIZE(recv_data, 22);
    sLog.outDetail( "WORLD: Recieved CMSG_BUY_ITEM_IN_SLOT" );

    if(!GetPlayer())
        return;

    WorldPacket data;
    uint64 srcguid, bagguid;
    uint32 itemid;
    int8 slot, newslot=0;
    uint8 amount=0;
    uint8 error;
    bool result;

    recv_data >> srcguid >> itemid;
    recv_data >> bagguid; 
    recv_data >> slot;
    recv_data >> amount;

    if(_player->isCasting())
        _player->InterruptSpell();

    Creature *unit = sObjHolder.GetObject<Creature>(srcguid);
    if (unit == NULL || !unit->HasItems())
        return;
    Container*c=NULL;

    //if slot is diferent than -1, check for validation, else continue for auto storing.
    if(slot != INVENTORY_SLOT_NOT_SET)
    {
        if(!(bagguid>>32))//buy to bakcpack
        {
            if(slot > INVENTORY_SLOT_ITEM_END)
            {
                //hackers!
                _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
                return;
            }
            else
            {
                //player trying to put a item in a empty bag slot
                if(slot >=  INVENTORY_SLOT_BAG_START && slot <  INVENTORY_SLOT_BAG_END && !_player->GetItemInterface()->GetInventoryItem(slot))
                {
                    _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
                    return;
                }
                if(_player->GetItemInterface()->GetInventoryItem(slot))
                {
                    _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
                    return;//non empty
                }
            }
        }
        else
        {
            c=(Container*)_player->GetItemInterface()->GetItemByGUID(bagguid);
            if(!c)return;

            if(c->GetProto() && slot > c->GetProto()->ContainerSlots)
            {
                _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
                return;
            }
            else
            {
                if(c->GetItem(slot))
                {
                    _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
                    return;//non empty
                }
            }
        }
    }
    else
    {
        if((bagguid>>32))
        {
            c=(Container*)_player->GetItemInterface()->GetItemByGUID(bagguid);
            if(!c)
            {
                _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_ITEM_NOT_FOUND);
                return;//non empty
            }
        }
    }
    
    CreatureItem itemd;
    unit->GetSellItemByItemId(itemid, itemd);

    if(itemd.itemid == 0)
    {
        // vendor does not sell this item.. bitch about cheaters?
        return;
    }

    ItemPrototype *it = objmgr.GetItemPrototype(itemid);
    if(!it) return;

    if(error = _player->GetItemInterface()->CanReceiveItem(it, amount))
    {
        _player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, error);
        return;
    }

    if(error = _player->GetItemInterface()->CanAffordItem(it,amount))
    {
        SendBuyFailed(srcguid, itemd.itemid, 0x02);
        return;
    }


    int8 cslot = _player->GetItemInterface()->GetInventorySlotByGuid(bagguid);
    if(error = _player->GetItemInterface()->CanEquipItemInSlot(cslot, ((cslot) ? cslot : slot), it))
    {
        _player->GetItemInterface()->BuildInventoryChangeError(NULL,NULL, error);
        return;
    }

    for(uint32 i = 0; i < amount; ++i)
    {

        Item *item = objmgr.CreateItem( itemid, _player);
     
        if(!item)
        {
            _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_DONT_OWN_THAT_ITEM);
            return;
        }

        item->SetUInt32Value(ITEM_FIELD_STACK_COUNT,itemd.amount);
        
        if(c)
        {
            if(slot == INVENTORY_SLOT_NOT_SET)
            {
                newslot = c->FindFreeSlot();
                if(newslot == ITEM_NO_SLOT_AVAILABLE)
                {
                    _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_BAG_FULL);
                    return;
                }
                else
                {
                    c->AddItem( newslot, item );
                }
            }
            else
            {
                c->AddItem( slot, item );
            }
        }
        else
        {
            if(slot == INVENTORY_SLOT_NOT_SET)
            {
                newslot = _player->GetItemInterface()->FindFreeBackPackSlot();
                if(newslot == ITEM_NO_SLOT_AVAILABLE)
                {
                    _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
                    return;
                }
                else
                {
                    result = _player->GetItemInterface()->SafeAddItem(item, INVENTORY_SLOT_NOT_SET, newslot);
                    if(!result)
                    {
                        printf("HandleBuyItemInSlot: Error while adding item to newslot");
                    }
                }
            }
            else
            {
                result = _player->GetItemInterface()->SafeAddItem(item, INVENTORY_SLOT_NOT_SET, slot);
                if(!result)
                {
                    printf("HandleBuyItemInSlot: Error while adding item to slot");
                }
            }
        }
        BuildItemPushResult(&data, _player->GetGUID(), ITEM_PUSH_TYPE_RECEIVE, itemd.amount, itemid, 0);
        SendPacket(&data);
    }

    _player->GetItemInterface()->BuyItem(it,amount*itemd.amount);

    data.Initialize( SMSG_BUY_ITEM );
    data << uint64(srcguid);
    data << uint32(itemid) << uint32(amount);
 
    SendPacket( &data );
    sLog.outDetail( "WORLD: Sent SMSG_BUY_ITEM" );
}

void WorldSession::HandleBuyItemOpcode( WorldPacket & recv_data ) // right-click on item
{
    CHECK_PACKET_SIZE(recv_data, 14);
    sLog.outDetail( "WORLD: Received CMSG_BUY_ITEM" );

    if(!GetPlayer())
        return;

    WorldPacket data;
    uint64 srcguid=0;
    uint32 itemid=0;
    int8 slot=0;
    uint8 amount=0;
    int8 playerslot = 0;
    int8 bagslot = 0;
    Item *add = NULL;
    uint8 error = 0;
    SlotResult slotresult;
    bool result;

    recv_data >> srcguid >> itemid;
    recv_data >> amount >> slot;

    if(_player->isCasting())
        _player->InterruptSpell();

    Creature *unit = sObjHolder.GetObject<Creature>(srcguid);
    if (unit == NULL || !unit->HasItems())
        return;

    CreatureItem item;
    unit->GetSellItemByItemId(itemid, item);
    if((amount == 1) && (item.amount > amount))
        amount = item.amount;
    if(item.itemid == 0)
    {
        // vendor does not sell this item.. bitch about cheaters?
        _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_DONT_OWN_THAT_ITEM);
        return;
    }

    ItemPrototype *it = objmgr.GetItemPrototype(itemid);
       if(!it) 
    {
        _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_DONT_OWN_THAT_ITEM);
        return;
    }

    if(error = _player->GetItemInterface()->CanReceiveItem(it, amount))
    {
        _player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, error);
        return;
    }

    if(error = _player->GetItemInterface()->CanAffordItem(it,amount))
    {
        SendBuyFailed(srcguid, itemid, 0x02);
        return;
    }
 
    // Find free slot and break if inv full
    add = _player->GetItemInterface()->FindItemLessMax(itemid,amount, false);
    if (!add)
    {
        slotresult = _player->GetItemInterface()->FindFreeInventorySlot(it);
    }
    if ((!slotresult.Result) && (!add))
    {
        //Our User doesn't have a free Slot in there bag
        _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
        return;
    }

    if(!add)
    {
        Item *itm = objmgr.CreateItem(item.itemid, _player);
        if(!itm)
        {
            _player->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_DONT_OWN_THAT_ITEM);
            return;
        }

        itm->SetUInt32Value(ITEM_FIELD_STACK_COUNT, amount);

        if(slotresult.ContainerSlot == ITEM_NO_SLOT_AVAILABLE)
        {
            result = _player->GetItemInterface()->SafeAddItem(itm, INVENTORY_SLOT_NOT_SET, slotresult.Slot);
            if(!result)
            {
                printf("HandleBuyItem: Error while adding item to playerslot");
            }
        }
        else 
        {
            if(Item *bag = _player->GetItemInterface()->GetInventoryItem(slotresult.ContainerSlot))
            {
                ((Container*)bag)->AddItem(slotresult.Slot, itm);
            }
        }
    }
    else
    {
        add->ModUInt32Value(ITEM_FIELD_STACK_COUNT, amount);
    }

     BuildItemPushResult(&data, _player->GetGUID(), ITEM_PUSH_TYPE_RECEIVE, amount, itemid, 0);
     SendPacket(&data);

     data.Initialize( SMSG_BUY_ITEM );
     data << uint64(srcguid);
     data << uint32(itemid) << uint32(amount);
     SendPacket( &data );
        
     _player->GetItemInterface()->BuyItem(it,amount);
}

void WorldSession::HandleListInventoryOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 8);
    sLog.outDetail( "WORLD: Recvd CMSG_LIST_INVENTORY" );
    uint64 guid=0;

    recv_data >> guid;

    Creature *unit = sObjHolder.GetObject<Creature>(guid);
    if (unit == NULL)
        return;

    SendInventoryList(unit);
}

void WorldSession::SendInventoryList(Creature* unit)
{
    if(!unit->HasItems())
    {
        sChatHandler.BlueSystemMessageToPlr(_player, "No sell template found. Report this to devs: %d (%s)", unit->GetEntry(), unit->GetName());
        return;
    }

    WorldPacket data(((unit->GetSellItemCount() * 28) + 9));       // allocate

    data.SetOpcode( SMSG_LIST_INVENTORY );
    data << unit->GetGUID();
    data << uint8( 0 ); // placeholder for item count

    ItemPrototype * curItem;
    uint32 counter = 0;

    for(std::vector<CreatureItem>::iterator itr = unit->GetSellItemBegin(); itr != unit->GetSellItemEnd(); ++itr)
    {
        if(itr->itemid)
        {
            if(curItem = objmgr.GetItemPrototype(itr->itemid))
            {
                data << (counter + 1);
                data << curItem->ItemId;
                data << curItem->DisplayInfoID;
                data << (int32)(-1); //we dont suport this kind of buy, make them infinite
                data << GetBuyPriceForItem(curItem, itr->amount, itr->amount);
                data << uint32(0x00);
                data << (int16)itr->amount;
                data << uint16(0x00);

                ++counter;
            }
        }
    }

    const_cast<uint8*>(data.contents())[8] = (uint8)counter;    // set count

    SendPacket( &data );
    sLog.outDetail( "WORLD: Sent SMSG_LIST_INVENTORY" );
}
void WorldSession::HandleAutoStoreBagItemOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 3);
    sLog.outDetail( "WORLD: Recvd CMSG_AUTO_STORE_BAG_ITEM" );
    
    if(!GetPlayer())
        return;

    WorldPacket data;
    WorldPacket packet;
    int8 SrcInv=0, Slot=0, DstInv=0;
    Item *item= NULL;
    Item *srcitem = NULL;
    Item *dstitem= NULL;
    int8 NewSlot = 0;
    int8 error;
    bool result;

    recv_data >> SrcInv >> Slot >> DstInv;

    srcitem = _player->GetItemInterface()->GetInventoryItem(SrcInv, Slot);

    //source item exists
    if(srcitem)
    {
        //src containers cant be moved if they have items inside
        if(srcitem->IsContainer() && static_cast<Container*>(srcitem)->HasItems())
        {
            _player->GetItemInterface()->BuildInventoryChangeError(srcitem, 0, INV_ERR_NONEMPTY_BAG_OVER_OTHER_BAG);
            return;
        }
        //check for destination now before swaping.
        //destination is backpack
        if(DstInv == INVENTORY_SLOT_NOT_SET)
        {
            //check for space
            NewSlot = _player->GetItemInterface()->FindFreeBackPackSlot();
            if(NewSlot == ITEM_NO_SLOT_AVAILABLE)
            {
                _player->GetItemInterface()->BuildInventoryChangeError(srcitem, 0, INV_ERR_BAG_FULL);
                return;
            }
            else
            {
                //free space found, remove item and add it to the destination
                srcitem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInv, Slot, false);
                result = _player->GetItemInterface()->SafeAddItem(srcitem, INVENTORY_SLOT_NOT_SET, NewSlot);
                if(!result)
                {
                    printf("HandleAutoStoreBagItem: Error while adding item to newslot");
                }
            }
        }
        else
        {
            if(error=_player->GetItemInterface()->CanEquipItemInSlot(DstInv,  DstInv, srcitem->GetProto()))
            {
                if(DstInv < INVENTORY_KEYRING_END)
                {
                    _player->GetItemInterface()->BuildInventoryChangeError(srcitem,dstitem, error);
                    return;
                }
            }

            //destination is a bag
            dstitem = _player->GetItemInterface()->GetInventoryItem(DstInv);
            if(dstitem)
            {
                //dstitem exists, detect if its a container
                if(dstitem->IsContainer())
                {
                    NewSlot = static_cast<Container*>(dstitem)->FindFreeSlot();
                    if(NewSlot == ITEM_NO_SLOT_AVAILABLE)
                    {
                        _player->GetItemInterface()->BuildInventoryChangeError(srcitem, 0, INV_ERR_BAG_FULL);
                        return;
                    }
                    else
                    {
                        srcitem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInv, Slot, false);
                        result = _player->GetItemInterface()->SafeAddItem(srcitem, DstInv, NewSlot);
                        if(!result)
                        {
                            printf("HandleBuyItemInSlot: Error while adding item to newslot");
                        }        
                    }
                }
                else
                {
                    _player->GetItemInterface()->BuildInventoryChangeError(srcitem, 0,  INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
                    return;
                }
            }
            else
            {
                _player->GetItemInterface()->BuildInventoryChangeError(srcitem, 0, INV_ERR_ITEM_DOESNT_GO_TO_SLOT);
                return;
            }
        }
    }
    else
    {
        _player->GetItemInterface()->BuildInventoryChangeError(srcitem, 0, INV_ERR_ITEM_NOT_FOUND);
        return;
    }
}

void WorldSession::HandleReadItemOpcode(WorldPacket &recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 2);
    WorldPacket data;
    int8 uslot=0, slot=0;
    recvPacket >> uslot >> slot;

    if(!GetPlayer())
        return;

    Item *item = _player->GetItemInterface()->GetInventoryItem(slot);
    sLog.outDebug("Recieved CMSG_READ_ITEM %d", slot);

    if(item)
    {
        // Check if it has pagetext
       
        if(item->GetProto()->PageId)
        {
            data.Initialize(SMSG_READ_ITEM_OK);
            data << item->GetGUID();
            SendPacket(&data);
            sLog.outDebug("Sent SMSG_READ_OK %d", item->GetGUID());
        }    
    }
}

void WorldSession::HandleRepairItemOpcode(WorldPacket &recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 12);
    if(!GetPlayer())
        return;

    WorldPacket data;
    uint64 npcguid;
    uint64 itemguid;

    recvPacket >> npcguid >> itemguid;

    if(!itemguid) 
    {
        for (int8 i = 0; i < EQUIPMENT_SLOT_END; i++)
              if(_player->GetItemInterface()->GetInventoryItem(i))
            {
                // maxdurability - currentdurability
                // it its 0 no durabiliti needs to be set
                uint32 dDurability = _player->GetItemInterface()->GetInventoryItem(i)->GetDurabilityMax() - _player->GetItemInterface()->GetInventoryItem(i)->GetDurability();
                uint32 cDurability = _player->GetItemInterface()->GetInventoryItem(i)->GetDurability();
                if (dDurability)
                {
                    // the amount of durability that is needed to be added is the amount of money to be payed
                    if (dDurability <= _player->GetUInt32Value(PLAYER_FIELD_COINAGE))
                    {
                       _player->ModUInt32Value( PLAYER_FIELD_COINAGE , -(int32)dDurability );
                       _player->GetItemInterface()->GetInventoryItem(i)->SetDurabilityToMax();

                       /*if (cDurability <= 0)
                           _player->ApplyItemMods(_player->GetItemInterface()->GetInventoryItem(i), i, true);*/
                    }
                    else
                    {
                        // not enough money
                    }
                }
            }
    }
    else 
    {
        Item *item = _player->GetItemInterface()->GetItemByGUID(itemguid);
        if(item)
        {
            uint32 dDurability = item->GetDurabilityMax() - item->GetDurability();
            uint32 cDurability = item->GetDurability();

            if (dDurability)
            {
                // the amount of durability that is needed to be added is the amount of money to be payed
                if (dDurability <= _player->GetUInt32Value(PLAYER_FIELD_COINAGE))
                {
                    _player->ModUInt32Value( PLAYER_FIELD_COINAGE , -(int32)dDurability );
                    item->SetDurabilityToMax();
                    
                    if(cDurability <= 0)
                    _player->ApplyItemMods(item, _player->GetItemInterface()->GetInventorySlotByGuid(itemguid), true);
                }
                else
                {
                    // not enough money
                }
            }
        }
    }
    sLog.outDebug("Recieved CMSG_REPAIR_ITEM %d", itemguid);
}

void WorldSession::HandleBuyBankSlotOpcode(WorldPacket& recvPacket) 
{
    //CHECK_PACKET_SIZE(recvPacket, 12);
    uint32 bytes,slots;
    int32 price;
    sLog.outDebug("WORLD: CMSG_BUY_bytes_SLOT");

    bytes = GetPlayer()->GetUInt32Value(PLAYER_BYTES_2);
    slots =(uint8) (bytes >> 16);

    sLog.outDetail("PLAYER: Buy bytes bag slot, slot number = %d", slots);

    // Prices Hardcoded
    switch (slots) 
    {
        case 0:
            price = 1000;
            break;
        case 1:
            price = 10000;
            break;
        case 2:
            price = 100000;
            break;
        case 3:
            price = 250000;
            break;
        case 4:
            price = 250000;
            break;
        case 5:
            price = 250000;
            break;
        case 6:
            price = 250000;
            break;
        default:
            return;
    }

    if (_player->GetUInt32Value(PLAYER_FIELD_COINAGE) >= price) 
    {
       _player->SetUInt32Value(PLAYER_BYTES_2, (bytes&0xff00ffff) | ((slots+1) << 16) );
       _player->ModUInt32Value(PLAYER_FIELD_COINAGE, -price);
    }
}

void WorldSession::HandleAutoBankItemOpcode(WorldPacket &recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 2);
    sLog.outDebug("WORLD: CMSG_AUTO_BANK_ITEM");

    WorldPacket data;

    bool result;
    SlotResult slotresult;
    int8 SrcInvSlot, SrcSlot, error=0;

    if(!GetPlayer())
        return;

    recvPacket >> SrcInvSlot >> SrcSlot;

    sLog.outDetail("ITEM: Auto Bank, Inventory slot: %u Source Slot: %u", (uint32)SrcInvSlot, (uint32)SrcSlot);

    Item *eitem=_player->GetItemInterface()->GetInventoryItem(SrcInvSlot,SrcSlot);

    if(!eitem) 
    {
        _player->GetItemInterface()->BuildInventoryChangeError(eitem, NULL, INV_ERR_ITEM_NOT_FOUND);
        return;
    }

    slotresult =  _player->GetItemInterface()->FindFreeBankSlot(eitem->GetProto());

    if(!slotresult.Result)
    {
        _player->GetItemInterface()->BuildInventoryChangeError(eitem, NULL, INV_ERR_BANK_FULL);
        return;
    }
    else
    {
        if(slotresult.ContainerSlot == INVENTORY_SLOT_NOT_SET)
        {
            _player->GetItemInterface()->SwapItemSlots(SrcSlot, slotresult.Slot);
        }
        else
        {
            eitem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInvSlot,SrcSlot, false);
            result = _player->GetItemInterface()->SafeAddItem(eitem, slotresult.ContainerSlot, slotresult.Slot);
            if(!result)
            {
                printf("Auto Bank Item: Error while adding item to one of the bank bags!\n");
                _player->GetItemInterface()->SafeAddItem(eitem, SrcInvSlot, SrcSlot);
            }
        }
    }
}

void WorldSession::HandleAutoStoreBankItemOpcode(WorldPacket &recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 2);
    sLog.outDebug("WORLD: CMSG_AUTOSTORE_BANK_ITEM");

    WorldPacket data;

    bool result;
    int8 SrcInvSlot, SrcSlot, error=0, slot=-1, specialbagslot=-1;

    if(!GetPlayer())
        return;

    recvPacket >> SrcInvSlot >> SrcSlot;

    sLog.outDetail("ITEM: AutoStore Bank Item, Inventory slot: %u Source Slot: %u", (uint32)SrcInvSlot, (uint32)SrcSlot);

    Item *eitem=_player->GetItemInterface()->GetInventoryItem(SrcInvSlot,SrcSlot);

    if(!eitem) 
    {
        _player->GetItemInterface()->BuildInventoryChangeError(eitem, NULL, INV_ERR_ITEM_NOT_FOUND);
        return;
    }

    SlotResult slotresult = _player->GetItemInterface()->FindFreeInventorySlot(eitem->GetProto());

    if(!slotresult.Result)
    {
        _player->GetItemInterface()->BuildInventoryChangeError(eitem, NULL, INV_ERR_INVENTORY_FULL);
        return;
    }
    else
    {
        if(slotresult.ContainerSlot == INVENTORY_SLOT_NOT_SET)
        {
            _player->GetItemInterface()->SwapItemSlots(SrcSlot, slotresult.Slot);
        }
        else
        {
                eitem = _player->GetItemInterface()->SafeRemoveAndRetreiveItemFromSlot(SrcInvSlot, SrcSlot, false);
                result = _player->GetItemInterface()->AddItemToFreeSlot(eitem);
        }
    }
}

void WorldSession::HandleCancelTemporaryEnchantmentOpcode(WorldPacket &recvPacket)
{
    uint32 inventory_slot;
    recvPacket >> inventory_slot;

    Item * item = _player->GetItemInterface()->GetInventoryItem(inventory_slot);
    if(!item) return;

    item->RemoveAllEnchantments(true);
}
