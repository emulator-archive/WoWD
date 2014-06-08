//// Copyright (C) 2004 WoW Daemon
////
//// This program is free software; you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation; either version 2 of the License, or
//// (at your option) any later version.
////
//// This program is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with this program; if not, write to the Free Software
//// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//


#ifndef _WOWD_ITEMINTERFACE_H
#define _WOWD_ITEMINTERFACE_H

struct SlotResult
{
    SlotResult() { ContainerSlot = -1, Slot = -1, Result = false; }
    int8 ContainerSlot;
    int8 Slot;
    bool Result;
};

class Item;
class Container;
class Player;
class UpdateData;
class ByteBuffer;

class WOWD_SERVER_DECL ItemInterface
{
private:
    SlotResult result;
    Player *m_pOwner;
    Item* m_pItems[MAX_INVENTORY_SLOT];
    Item* m_pBuyBack[MAX_BUYBACK_SLOT];

    bool m_AddItem(Item *item, int8 ContainerSlot, int8 slot);

public:
    ItemInterface( Player *pPlayer );
    ~ItemInterface();

    Player *GetOwner() { return m_pOwner; }
    bool IsBagSlot(int8 slot);

    uint32 m_CreateForPlayer(ByteBuffer *data);
    void m_DestroyForPlayer();

    void mLoadItemsFromDatabase();
    void mSaveItemsToDatabase(bool first);

    Item *GetInventoryItem(int8 slot);
    Item *GetInventoryItem(int8 ContainerSlot, int8 slot);
    int8 GetInventorySlotById(uint32 ID);
    int8 GetInventorySlotByGuid(uint64 guid);

    Item *SafeAddItem(uint32 ItemId, int8 ContainerSlot, int8 slot);
    bool SafeAddItem(Item *pItem, int8 ContainerSlot, int8 slot);
    Item *SafeRemoveAndRetreiveItemFromSlot(int8 ContainerSlot, int8 slot, bool destroy); //doesnt destroy item from memory
    Item *SafeRemoveAndRetreiveItemByGuid(uint64 guid, bool destroy);
    bool SafeFullRemoveItemFromSlot(int8 ContainerSlot, int8 slot); //destroys item fully
    bool SafeFullRemoveItemByGuid(uint64 guid); //destroys item fully
    bool AddItemToFreeSlot(Item *item);
    bool AddItemToFreeBankSlot(Item *item);
    
    Item* FindItemLessMax(uint32 itemid, uint32 cnt, bool IncBank);
    uint32 GetItemCount(uint32 itemid, bool IncBank = false);
    bool RemoveItemAmt(uint32 id, uint32 amt);
    void BuyItem(ItemPrototype *item, uint32 amt);

    uint32 CalculateFreeSlots(ItemPrototype *proto);
    void ReduceItemDurability();

    //Searching functions
    SlotResult FindFreeInventorySlot(ItemPrototype *proto);
    SlotResult FindFreeBankSlot(ItemPrototype *proto);
    int8 FindFreeBackPackSlot();
    int8 FindFreeKeyringSlot();
    int8 FindSpecialBag(Item *item);


    int8 CanEquipItemInSlot(int8 DstInvSlot, int8 slot, ItemPrototype* item, bool ignore_combat = false);
    int8 CanReceiveItem(ItemPrototype * item, uint32 amount);
    int8 CanAffordItem(ItemPrototype * item,uint32 amount);
    int8 GetItemSlotByType(uint32 type);
    Item* GetItemByGUID(uint64 itemGuid);

    void BuildInventoryChangeError(Item *SrcItem, Item *DstItem, uint8 Error);
    void SwapItemSlots(int8 srcslot, int8 dstslot);

    int8 GetInternalBankSlotFromPlayer(int8 islot); //converts inventory slots into 0-x numbers

    //buyback stuff
    inline Item* GetBuyBack(int32 slot) 
    { 
        if(slot >= 0 && slot <= 12)
            return m_pBuyBack[slot];
        else 
            return NULL;
    }
    void AddBuyBackItem(Item* it, uint32 price);
    void RemoveBuyBackItem(uint32 index);
    void EmptyBuyBack();
};

#endif
