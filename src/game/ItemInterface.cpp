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
//
// Item interface
//
#include "StdAfx.h"
#include "Item.h"
#include "Container.h"
#include "ItemPrototype.h"
#include "Player.h"
//
//-------------------------------------------------------------------//
ItemInterface::ItemInterface( Player *pPlayer )
{
    m_pOwner = pPlayer;
    memset(m_pItems, 0, sizeof(Item*)*MAX_INVENTORY_SLOT);
    memset(m_pBuyBack, 0, sizeof(Item*)*MAX_BUYBACK_SLOT);
}

//-------------------------------------------------------------------//
ItemInterface::~ItemInterface()
{
    for(int i = 0; i < MAX_INVENTORY_SLOT; i++)
    {
        if(m_pItems[i])
        {
            if(m_pItems[i]->IsContainer())
            {
                delete ((Container*)(m_pItems[i]));
            }
            else
            {
                delete m_pItems[i];
            }
        }
    }
}

//-------------------------------------------------------------------// 100%
uint32 ItemInterface::m_CreateForPlayer(ByteBuffer *data)
{
    ASSERT(m_pOwner != NULL);
    uint32 count = 0;

    for(int i = 0; i < MAX_INVENTORY_SLOT; i++)
    {
        if(m_pItems[i])
        {
            if(m_pItems[i]->IsContainer())
            {
                count += ((Container*)(m_pItems[i]))->BuildCreateUpdateBlockForPlayer(data, m_pOwner);

                if(m_pItems[i]->GetProto())
                {
                    for(int e=0; e < m_pItems[i]->GetProto()->ContainerSlots; e++)
                    {
                        Item *pItem = ((Container*)(m_pItems[i]))->GetItem(e);
                        if(pItem)
                        {
                            if(pItem->IsContainer())
                            {
                                count += ((Container*)(pItem))->BuildCreateUpdateBlockForPlayer( data, m_pOwner );
                            }
                            else
                            {
                                count += pItem->BuildCreateUpdateBlockForPlayer( data, m_pOwner );
                            }
                        }
                    }
                }
            }
            else
            {
                count += m_pItems[i]->BuildCreateUpdateBlockForPlayer(data, m_pOwner);
            }
        }
    }
    return count;
}

//-------------------------------------------------------------------// 100%
void ItemInterface::m_DestroyForPlayer()
{
    ASSERT(m_pOwner != NULL);

    for(int i = 0; i < MAX_INVENTORY_SLOT; i++)
    {
        if(m_pItems[i])
        {
            if(m_pItems[i]->IsContainer())
            {
                if(m_pItems[i]->GetProto())
                {
                    for(int e=0; e < m_pItems[i]->GetProto()->ContainerSlots; e++)
                    {
                        Item *pItem = ((Container*)(m_pItems[i]))->GetItem(e);
                        if(pItem)
                        {
                            if(pItem->IsContainer())
                            {
                                ((Container*)(pItem))->DestroyForPlayer( m_pOwner );
                            }
                            else
                            {
                                pItem->DestroyForPlayer( m_pOwner );
                            }
                        }
                    }
                }
                ((Container*)(m_pItems[i]))->DestroyForPlayer( m_pOwner );
            }
            else
            {
                m_pItems[i]->DestroyForPlayer( m_pOwner );
            }
        }
    }
}

//-------------------------------------------------------------------//
//Description: Creates and adds a item that can be manipulated after          
//-------------------------------------------------------------------//
Item *ItemInterface::SafeAddItem(uint32 ItemId, int8 ContainerSlot, int8 slot)
{
    Item *pItem;
    ItemPrototype *pProto = objmgr.GetItemPrototype(ItemId);
    if(!pProto) { return NULL; }

    if(pProto->InventoryType == INVTYPE_BAG)
    {
        pItem = (Item*)new Container();
        static_cast<Container*>(pItem)->Create(objmgr.GenerateLowGuid(HIGHGUID_CONTAINER), ItemId, m_pOwner);
        if(m_AddItem(pItem, ContainerSlot, slot))
        {
            return pItem;
        }
        else
        {
            delete pItem;
            return NULL;
        }
    }
    else
    {
        pItem = new Item();
        pItem->Create(objmgr.GenerateLowGuid(HIGHGUID_ITEM), ItemId, m_pOwner);
        if(m_AddItem(pItem, ContainerSlot, slot))
        {
            return pItem;
        }
        else
        {
            delete pItem;
            return NULL;
        }
    }
    return NULL;
}

//-------------------------------------------------------------------//
//Description: Creates and adds a item that can be manipulated after          
//-------------------------------------------------------------------//
bool ItemInterface::SafeAddItem(Item *pItem, int8 ContainerSlot, int8 slot)
{
    return m_AddItem(pItem, ContainerSlot, slot);
}

//-------------------------------------------------------------------//
//Description: adds items to player inventory, this includes all types of slots.          
//-------------------------------------------------------------------//
bool ItemInterface::m_AddItem(Item *item, int8 ContainerSlot, int8 slot)
{
    ASSERT(slot < MAX_INVENTORY_SLOT);
    ASSERT(ContainerSlot < MAX_INVENTORY_SLOT);
    ASSERT(item != NULL);

    if(item->GetProto())
    {
        //case 1, item is from backpack container
        if(ContainerSlot == INVENTORY_SLOT_NOT_SET)
        {
            //ASSERT(m_pItems[slot] == NULL);
            if(GetInventoryItem(slot) != NULL)
            {
                //sLog.outError("bugged inventory: %u %u", m_pOwner->GetName(), item->GetGUID());
                SlotResult result = this->FindFreeInventorySlot(item->GetProto());
                
                // send message to player
                sChatHandler.BlueSystemMessageToPlr(m_pOwner, "A duplicated item, `%s` was found in your inventory. We've attempted to add it to a free slot in your inventory, if there is none this will fail. It will be attempted again the next time you log on.",
                    item->GetProto()->Name1.c_str());
                if(result.Result == true)
                {
                    // Found a new slot for that item.
                    slot = result.Slot;
                    ContainerSlot = result.ContainerSlot;
                }
                else
                {
                    // don't leak memory!
                    if(item->IsContainer())
                        delete ((Container*)item);
                    else
                        delete item;

                    return false;
                }
            }

            if(!GetInventoryItem(slot)) //slot is free, add item.
            {
                item->SetOwner( m_pOwner );
                item->SetUInt64Value(ITEM_FIELD_CONTAINED, m_pOwner->GetGUID());
                m_pItems[slot] = item;

                if (item->GetProto()->Bonding == ITEM_BIND_ON_PICKUP) 
                    item->SoulBind();

                if( m_pOwner->IsInWorld() && !item->IsInWorld())
                {
                    item->AddToWorld();
                    ByteBuffer buf(2500);
                    uint32 count = item->BuildCreateUpdateBlockForPlayer( &buf, m_pOwner );
                    m_pOwner->PushUpdateData(&buf, count);
                }
                m_pOwner->SetUInt64Value(PLAYER_FIELD_INV_SLOT_HEAD + (slot*2), item->GetGUID());
            }
            else
            {
                return false;
            }
        }
        else //case 2: item is from a bag container
        {
            if(GetInventoryItem(ContainerSlot) && GetInventoryItem(ContainerSlot)->IsContainer())//container exists
            {
                bool result = static_cast<Container*>(m_pItems[ContainerSlot])->AddItem(slot, item);
                if(!result)
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }

    if ( slot < EQUIPMENT_SLOT_END && ContainerSlot == INVENTORY_SLOT_NOT_SET )
    {
        int VisibleBase = PLAYER_VISIBLE_ITEM_1_0 + (slot * 16);
        m_pOwner->SetUInt32Value(VisibleBase, item->GetUInt32Value(OBJECT_FIELD_ENTRY));
        m_pOwner->SetUInt32Value(VisibleBase + 1, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT));
        m_pOwner->SetUInt32Value(VisibleBase + 2, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 3));
        m_pOwner->SetUInt32Value(VisibleBase + 3, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 6));
        m_pOwner->SetUInt32Value(VisibleBase + 4, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 9));
        m_pOwner->SetUInt32Value(VisibleBase + 5, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 12));
        m_pOwner->SetUInt32Value(VisibleBase + 6, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 15));
        m_pOwner->SetUInt32Value(VisibleBase + 7, item->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 18));
        m_pOwner->SetUInt32Value(VisibleBase + 8, item->GetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID));
    }
    if( m_pOwner->IsInWorld() && slot < INVENTORY_SLOT_BAG_END && ContainerSlot == INVENTORY_SLOT_NOT_SET)
    {
        m_pOwner->ApplyItemMods( item,slot, true );
    }

    if(slot == EQUIPMENT_SLOT_OFFHAND && item->GetProto()->Class == ITEM_CLASS_WEAPON)
        m_pOwner->SetDuelWield(true);

    return true;
}

//-------------------------------------------------------------------//
//Description: Checks if the slot is a Bag slot
//-------------------------------------------------------------------//
bool ItemInterface::IsBagSlot(int8 slot)
{
    if((slot >= INVENTORY_SLOT_BAG_START && slot < INVENTORY_SLOT_BAG_END) || (slot >= BANK_SLOT_BAG_START && slot < BANK_SLOT_BAG_END))
    {
        return true;
    }
    return false;
}

//-------------------------------------------------------------------//
//Description: removes the item safely and returns it back for usage
//-------------------------------------------------------------------//
Item *ItemInterface::SafeRemoveAndRetreiveItemFromSlot(int8 ContainerSlot, int8 slot, bool destroy)
{
    ASSERT(slot < MAX_INVENTORY_SLOT);
    ASSERT(ContainerSlot < MAX_INVENTORY_SLOT);
    Item *pItem = NULL;

    if(ContainerSlot == INVENTORY_SLOT_NOT_SET)
    {
        pItem = GetInventoryItem(ContainerSlot,slot);

        if (pItem == NULL) { return NULL; }

        m_pItems[slot] = NULL;
        m_pOwner->SetUInt64Value(PLAYER_FIELD_INV_SLOT_HEAD  + (slot*2), 0 );

        if ( slot < EQUIPMENT_SLOT_END )
        {
            m_pOwner->ApplyItemMods( pItem, slot, false );
            int VisibleBase = PLAYER_VISIBLE_ITEM_1_0 + (slot * 16);
            for (int i = VisibleBase; i < VisibleBase + 12; ++i)
            {
                m_pOwner->SetUInt32Value(i, 0);
                m_pOwner->SetBaseUInt32Value(i, 0);
            }
        }

        if(slot == EQUIPMENT_SLOT_OFFHAND)
            m_pOwner->SetDuelWield(false);

        if(destroy)
        {
            if (pItem->IsInWorld())
            {
                pItem->RemoveFromWorld();
            }
            pItem->DeleteFromDB();
        }
    }
    else
    {
        Item *pContainer = GetInventoryItem(ContainerSlot);
        if(pContainer && pContainer->IsContainer())
        {
            pItem = static_cast<Container*>(pContainer)->SafeRemoveAndRetreiveItemFromSlot(slot, destroy);
            if (pItem == NULL) { return NULL; }
        }
    }

    return pItem;
}

//-------------------------------------------------------------------//
//Description: removes the item safely by guid and returns it back for usage, supports full inventory
//-------------------------------------------------------------------//
Item *ItemInterface::SafeRemoveAndRetreiveItemByGuid(uint64 guid, bool destroy)
{
    int8 i = 0;

    for(i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if (item && item->GetGUID() == guid)
        {
            return this->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, i, destroy);
        }
    }

    for(i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if (item && item->GetGUID() == guid)
        {
            return this->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, i, destroy);
        }
    }

    for(i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++)
    {
        Item *item = GetInventoryItem(i);

        if (item && item->GetGUID() == guid)
        {
            return this->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, i, destroy);
        }
    }

    for(i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if(item && item->GetGUID() == guid)
        {
            return this->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, i, destroy);
        }
        else
        {
            if(item && item->IsContainer() && item->GetProto())
            {
                for (int8 j =0; j < item->GetProto()->ContainerSlots; j++)
                {
                    Item *item2 = ((Container*)item)->GetItem(j);
                    if (item2 && item2->GetGUID() == guid)
                    {
                        return ((Container*)item)->SafeRemoveAndRetreiveItemFromSlot(j, destroy);
                    }
                }
            }
        }
    }

    for(i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if (item && item->GetGUID() == guid)
        {
            return this->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, i, destroy);
        }
    }

    for(i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if (item && item->GetGUID() == guid)
        {
            return this->SafeRemoveAndRetreiveItemFromSlot(INVENTORY_SLOT_NOT_SET, i, destroy);
        }
        else
        {
            if(item && item->IsContainer() && item->GetProto())
            {
                for (int8 j =0; j < item->GetProto()->ContainerSlots; j++)
                {
                    Item *item2 = ((Container*)item)->GetItem(j);
                    if (item2 && item2->GetGUID() == guid)
                    {
                        return ((Container*)item)->SafeRemoveAndRetreiveItemFromSlot(j, destroy);
                    }
                }
            }
        }
    }

    return NULL;
}

//-------------------------------------------------------------------//
//Description: completely removes item from player
//Result: true if item removal was succefull
//-------------------------------------------------------------------//
bool ItemInterface::SafeFullRemoveItemFromSlot(int8 ContainerSlot, int8 slot)
{
    ASSERT(slot < MAX_INVENTORY_SLOT);
    ASSERT(ContainerSlot < MAX_INVENTORY_SLOT);

    if(ContainerSlot == INVENTORY_SLOT_NOT_SET)
    {
        Item *pItem = GetInventoryItem(slot);

        if (pItem == NULL) { return false; }

        m_pItems[slot] = NULL;
        m_pOwner->SetUInt64Value(PLAYER_FIELD_INV_SLOT_HEAD  + (slot*2), 0 );

        if ( slot < EQUIPMENT_SLOT_END )
        {
            m_pOwner->ApplyItemMods(pItem, slot, false );
            int VisibleBase = PLAYER_VISIBLE_ITEM_1_0 + (slot * 16);
            for (int i = VisibleBase; i < VisibleBase + 12; ++i)
            {
                m_pOwner->SetUInt32Value(i, 0);
                m_pOwner->SetBaseUInt32Value(i, 0);
            }
        }

        if(slot == EQUIPMENT_SLOT_OFFHAND)
            m_pOwner->SetDuelWield(false);

        if (pItem->IsInWorld())
        {
            pItem->RemoveFromWorld();
        }

        pItem->DeleteFromDB();
        delete pItem;
    }
    else
    {
        Item *pContainer = GetInventoryItem(ContainerSlot);
        if(pContainer && pContainer->IsContainer())
        {
            static_cast<Container*>(pContainer)->SafeFullRemoveItemFromSlot(slot);
        }
    }
    return true;
}

//-------------------------------------------------------------------//
//Description: removes the item safely by guid, supports full inventory
//-------------------------------------------------------------------//
bool ItemInterface::SafeFullRemoveItemByGuid(uint64 guid)
{
    int8 i = 0;

    for(i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if (item && item->GetGUID() == guid)
        {
            return this->SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
        }
    }

    for(i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if (item && item->GetGUID() == guid)
        {
            return this->SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
        }
    }

    for(i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++)
    {
        Item *item = GetInventoryItem(i);

        if (item && item->GetGUID() == guid)
        {
            return this->SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
        }
    }

    for(i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if(item && item->GetGUID() == guid)
        {
            return this->SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
        }
        else
        {
            if(item && item->IsContainer() && item->GetProto())
            {
                for (int8 j =0; j < item->GetProto()->ContainerSlots; j++)
                {
                    Item *item2 = ((Container*)item)->GetItem(j);
                    if (item2 && item2->GetGUID() == guid)
                    {
                        return ((Container*)item)->SafeFullRemoveItemFromSlot(j);
                    }
                }
            }
        }
    }

    for(i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; i++)
    {
        Item *item = GetInventoryItem(i);

        if (item && item->GetGUID() == guid)
        {
            return this->SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
        }
    }

    for(i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if(item && item->GetGUID() == guid)
        {
            return this->SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
        }
        else
        {
            if(item && item->IsContainer() && item->GetProto())
            {
                for (int8 j =0; j < item->GetProto()->ContainerSlots; j++)
                {
                    Item *item2 = ((Container*)item)->GetItem(j);
                    if (item2 && item2->GetGUID() == guid)
                    {
                        return ((Container*)item)->SafeFullRemoveItemFromSlot(j);
                    }
                }
            }
        }
    }
    return false;
}

//-------------------------------------------------------------------//
//Description: Gets a item from Inventory
//-------------------------------------------------------------------//
Item *ItemInterface::GetInventoryItem(int8 slot)
{
    if(slot == INVENTORY_SLOT_NOT_SET || slot > MAX_INVENTORY_SLOT)
    {
        return NULL;
    }

    return m_pItems[slot];
}

//-------------------------------------------------------------------//
//Description: Gets a Item from inventory or container
//-------------------------------------------------------------------//
Item *ItemInterface::GetInventoryItem(int8 ContainerSlot, int8 slot)
{

    if(ContainerSlot == INVENTORY_SLOT_NOT_SET)
    {
        if(slot == INVENTORY_SLOT_NOT_SET || slot > MAX_INVENTORY_SLOT)
        {
            return NULL;
        }
        return m_pItems[slot];
    }
    else
    {
        if(IsBagSlot(ContainerSlot))
        {
            if(m_pItems[ContainerSlot])
            {
                return static_cast<Container*>(m_pItems[ContainerSlot])->GetItem(slot);
            }
        }
    }
    return NULL;
}

//-------------------------------------------------------------------//
//Description: checks for stacks that didnt reached max capacity
//-------------------------------------------------------------------//
Item* ItemInterface::FindItemLessMax(uint32 itemid, uint32 cnt, bool IncBank)
{
    int8 i = 0;
    for(i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if (item)
        {
            
            if((item->GetProto()->ItemId == itemid) && (item->GetProto()->MaxCount >= (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + cnt)))
            {
                return item; 
            }
        }
    }

    for(i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if(item)
        {
            if(item->IsContainer())
            {
                for (int8 j =0; j < item->GetProto()->ContainerSlots; j++)
                {
                    Item *item2 = ((Container*)item)->GetItem(j);
                    if (item2)
                    {
                        if((item2->GetProto()->ItemId == itemid) && (item2->GetProto()->MaxCount >= (item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + cnt)))
                        {
                            return item2;
                        }
                    }
                }
            }
        }
    }

    if(IncBank)
    {
        for(i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; i++)
        {
            Item *item = GetInventoryItem(i);
            if (item)
            {
            
                if((item->GetProto()->ItemId == itemid) && (item->GetProto()->MaxCount >= (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + cnt)))
                {
                    return item; 
                }
            }
        }

        for(i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++)
        {
            Item *item = GetInventoryItem(i);
            if(item)
            {
                if(item->IsContainer())
                {
                    for (int8 j =0; j < item->GetProto()->ContainerSlots; j++)
                    {
                        Item *item2 = ((Container*)item)->GetItem(j);
                        if (item2)
                        {
                            if((item2->GetProto()->ItemId == itemid) && (item2->GetProto()->MaxCount >= (item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + cnt)))
                            {
                                return item2;
                            }
                        }
                    }
                }
            }
        }
    }

    return NULL;
}

//-------------------------------------------------------------------//
//Description: finds item ammount on inventory, banks not included
//-------------------------------------------------------------------//
uint32 ItemInterface::GetItemCount(uint32 itemid, bool IncBank)
{
    uint32 cnt = 0;
    int8 i = 0;
    for(i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        Item *item = GetInventoryItem(i);

        if (item)
        {
            if(item->GetProto()->ItemId == itemid)
            {
                cnt += item->GetUInt32Value(ITEM_FIELD_STACK_COUNT); 
            }
        }
    }

    for(i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if(item)
        {
            if(item->IsContainer())
            {
                for (int8 j =0; j < item->GetProto()->ContainerSlots;j++)
                {
                    Item *item2 = ((Container*)item)->GetItem(j);
                    if (item2)
                    {
                        if (item2->GetProto()->ItemId == itemid)
                        {
                            cnt += item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT); 
                        }
                    }
                }
            }
        }
    }

    for(i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++)
    {
        Item *item = GetInventoryItem(i);

        if (item)
        {
            if(item->GetProto()->ItemId == itemid)
            {
                cnt += item->GetUInt32Value(ITEM_FIELD_STACK_COUNT); 
            }
        }
    }

    if(IncBank)
    {
        for(i = BANK_SLOT_ITEM_START; i < BANK_SLOT_BAG_END; i++)
        {
            Item *item = GetInventoryItem(i);
            if (item)
            {
                if(item->GetProto()->ItemId == itemid)
                {
                    cnt += item->GetUInt32Value(ITEM_FIELD_STACK_COUNT); 
                }
            }
        }

        for(i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++)
        {
            Item *item = GetInventoryItem(i);
            if(item)
            {
                if(item->IsContainer())
                {
                    for (int8 j =0; j < item->GetProto()->ContainerSlots; j++)
                    {
                        Item *item2 = ((Container*)item)->GetItem(j);
                        if (item2)
                        {
                            if(item2->GetProto()->ItemId == itemid)
                            {
                                cnt += item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT); 
                            }
                        }
                    }
                }
            }
        }
    }
    return cnt;
}

//-------------------------------------------------------------------//
//Description: Removes a ammount of items from inventory
//-------------------------------------------------------------------//
bool ItemInterface::RemoveItemAmt(uint32 id, uint32 amt)
{
    if (GetItemCount(id) < amt)
    {
        return false;
    }
    int8 i;

    for(i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if (item)
        {
            if(item->GetProto()->ItemId == id)
            {
                if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > amt)
                {
                    item->SetCount(item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) - amt);
                    return true;
                }
                else if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT)== amt)
                {
                    bool result = SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
                    if(result)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    amt -= item->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
                    bool result = SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
                    if(result)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }

    for(i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if(item)
        {
            for (int8 j =0; j < item->GetProto()->ContainerSlots;j++)
            {
                Item *item2 = ((Container*)item)->GetItem(j);
                if (item2)
                {
                    if (item2->GetProto()->ItemId == id)
                    {
                        if (item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > amt)
                        {
                            item2->SetCount(item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT) - amt);
                            return true;
                        }
                        else if (item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT)== amt)
                        {
                            bool result = SafeFullRemoveItemFromSlot(i, j);
                            if(result)
                            {
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        }
                        else
                        {
                            amt -= item2->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
                            bool result = SafeFullRemoveItemFromSlot(i, j);
                            if(result)
                            {
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    for(i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if (item)
        {
            if(item->GetProto()->ItemId == id)
            {
                if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) > amt)
                {
                    item->SetCount(item->GetUInt32Value(ITEM_FIELD_STACK_COUNT) - amt);
                    return true;
                }
                else if (item->GetUInt32Value(ITEM_FIELD_STACK_COUNT)== amt)
                {
                    bool result = SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
                    if(result)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    amt -= item->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
                    bool result = SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, i);
                    if(result)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }
    return false;
}


//-------------------------------------------------------------------//
//Description: Gets slot number by itemid, banks not included
//-------------------------------------------------------------------//
int8 ItemInterface::GetInventorySlotById(uint32 ID)
{
    for(int8 i=0;i<INVENTORY_SLOT_ITEM_END;i++)
    {
        if(m_pItems[i])
        {
            if(m_pItems[i]->GetProto()->ItemId == ID)
            {
                return i;
            }
        }
    }

    for(int8 i=INVENTORY_KEYRING_START; i<INVENTORY_KEYRING_END; i++)
    {
        if(m_pItems[i])
        {
            if(m_pItems[i]->GetProto()->ItemId == ID)
            {
                return i;
            }
        }
    }
    return ITEM_NO_SLOT_AVAILABLE;
}

//-------------------------------------------------------------------//
//Description: Gets slot number by item guid, banks not included
//-------------------------------------------------------------------//
int8 ItemInterface::GetInventorySlotByGuid(uint64 guid)
{
    for(int8 i=EQUIPMENT_SLOT_START ;i<INVENTORY_SLOT_ITEM_END;i++)
    {
        if(m_pItems[i])
        {
            if(m_pItems[i]->GetGUID() == guid)
            {
                return i;
            }
        }
    }

    for(int8 i=INVENTORY_KEYRING_START; i<INVENTORY_KEYRING_END; i++)
    {
        if(m_pItems[i])
        {
            if(m_pItems[i]->GetGUID() == guid)
            {
                return i;
            }
        }
    }

    return ITEM_NO_SLOT_AVAILABLE; //was changed from 0 cuz 0 is the slot for head
}

//-------------------------------------------------------------------//
//Description: Adds a Item to a free slot
//-------------------------------------------------------------------//
bool ItemInterface::AddItemToFreeSlot(Item *item)
{
    if(!item) { return false; }
    int8 i = 0;

    //detect special bag item
    if(item->GetProto()->BagFamily)
    {
        if(item->GetProto()->BagFamily == ITEM_TYPE_KEYRING)
        {
            for(int8 i=INVENTORY_KEYRING_START; i<INVENTORY_KEYRING_END; i++)
            {
                if(m_pItems[i] == NULL)
                {
                    bool result = SafeAddItem(item, INVENTORY_SLOT_NOT_SET, i);
                    if(result) { return true; }
                }
            }
        }
        else
        {
            for(i=INVENTORY_SLOT_BAG_START;i<INVENTORY_SLOT_BAG_END;i++)
            {
                if(m_pItems[i])
                {
                    if (m_pItems[i]->GetProto()->BagFamily == item->GetProto()->BagFamily)
                    {
                        if(m_pItems[i]->IsContainer())
                        {
                            bool result = static_cast<Container*>(m_pItems[i])->AddItemToFreeSlot(item);
                            if (result) { return true; }
                        }
                    }
                }
            }
        }
    }

    //INVENTORY
    for(i=INVENTORY_SLOT_ITEM_START;i<INVENTORY_SLOT_ITEM_END;i++)
    {
        if(m_pItems[i] == NULL)
        {
            return SafeAddItem(item, INVENTORY_SLOT_NOT_SET, i);
        }
    }

    //INVENTORY BAGS
    for(i=INVENTORY_SLOT_BAG_START;i<INVENTORY_SLOT_BAG_END;i++)
    {
        if(m_pItems[i] != NULL && m_pItems[i]->GetProto()->BagFamily == 0) //special bags ignored
        {
            for (int8 j =0; j < m_pItems[i]->GetProto()->ContainerSlots;j++)
            {
                Item *item2 = ((Container*)m_pItems[i])->GetItem(j);
                if (item2 == NULL)
                {
                    return SafeAddItem(item, i, j);
                }
            }
        }
    }
    return false;
}

//-------------------------------------------------------------------//
//Description: Calculates inventory free slots, bag inventory slots not included
//-------------------------------------------------------------------//
uint32 ItemInterface::CalculateFreeSlots(ItemPrototype *proto)
{
    uint32 count = 0;
    int8 i;

    if(proto)
    {
        if(proto->BagFamily)
        {
            if(proto->BagFamily == ITEM_TYPE_KEYRING)
            {
                for(int8 i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++)
                {
                    if(m_pItems[i] == NULL)
                    {
                        count++;
                    }
                }
            }
            else
            {
                for(int8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END;i++)
                {
                    if(m_pItems[i] && m_pItems[i]->IsContainer())
                    {
                        if (m_pItems[i]->GetProto()->BagFamily == proto->BagFamily)
                        {
                            int8 slot = ((Container*)m_pItems[i])->FindFreeSlot();
                            if(slot != ITEM_NO_SLOT_AVAILABLE) 
                            {
                                count++;
                            }
                        }
                    }
                }
            }
        }
    }

    for(i=INVENTORY_SLOT_ITEM_START;i<INVENTORY_SLOT_ITEM_END;i++)
    {
        if(m_pItems[i] == NULL)
        {
            count++;
        }
    }

    for(i=INVENTORY_SLOT_BAG_START;i<INVENTORY_SLOT_BAG_END;i++)
    {
        if(m_pItems[i] != NULL )
        {
            if(m_pItems[i]->IsContainer() && !m_pItems[i]->GetProto()->BagFamily)
            {

                for (int8 j =0; j < m_pItems[i]->GetProto()->ContainerSlots;j++)
                {
                    Item *item2 = ((Container*)m_pItems[i])->GetItem(j);
                    if (item2 == NULL)
                    {
                        count++;
                    }
                }
            }
        }
    }
    return count;
}

//-------------------------------------------------------------------//
//Description: finds a free slot on the backpack
//-------------------------------------------------------------------//
int8 ItemInterface::FindFreeBackPackSlot()
{
    //search for backpack slots
    for(int8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if(!item) 
        { 
            return i; 
        }
    }

    return ITEM_NO_SLOT_AVAILABLE; //no slots available
}

//-------------------------------------------------------------------//
//Description: converts bank bags slot ids into player bank byte slots(0-5)
//-------------------------------------------------------------------//
int8 ItemInterface::GetInternalBankSlotFromPlayer(int8 islot)
{
    switch(islot)
    {
    case BANK_SLOT_BAG_1:
        {
            return 1;
        }
    case BANK_SLOT_BAG_2:
        {
            return 2;
        }
    case BANK_SLOT_BAG_3:
        {
            return 3;
        }
    case BANK_SLOT_BAG_4:
        {
            return 4;
        }
    case BANK_SLOT_BAG_5:
        {
            return 5;
        }
    case BANK_SLOT_BAG_6:
        {
            return 6;
        }
    case BANK_SLOT_BAG_7:
        {
            return 7;
        }
    default:
        return 8;
    }
}

//-------------------------------------------------------------------//
//Description: checks if the item can be equiped on a specific slot
//-------------------------------------------------------------------//
int8 ItemInterface::CanEquipItemInSlot(int8 DstInvSlot, int8 slot, ItemPrototype *proto, bool ignore_combat)
{
    uint32 type=proto->InventoryType;
    
    if((slot < INVENTORY_SLOT_BAG_END && DstInvSlot == INVENTORY_SLOT_NOT_SET) || (slot >= BANK_SLOT_BAG_START && slot < BANK_SLOT_BAG_END && DstInvSlot == INVENTORY_SLOT_NOT_SET))
    {
        if (!ignore_combat && m_pOwner->isInCombat())
            return INV_ERR_CANT_DO_IN_COMBAT;

        // Check to see if we have the correct race
        if(!(proto->AllowableRace& (1<<m_pOwner->getRace())))
            return INV_ERR_YOU_CAN_NEVER_USE_THAT_ITEM;

        // Check to see if we have the correct class
        if(!(proto->AllowableClass & (1<<(m_pOwner->getClass()-1))))
            return INV_ERR_YOU_CAN_NEVER_USE_THAT_ITEM2;

        // Check to see if we have the reqs for that reputation(disabled atm)
        if(proto->RequiredFaction)
            return INV_ERR_YOU_CAN_NEVER_USE_THAT_ITEM;

        // Check to see if we have the correct level.
        if(proto->RequiredLevel>m_pOwner->GetUInt32Value(UNIT_FIELD_LEVEL))
            return INV_ERR_YOU_MUST_REACH_LEVEL_N;

        if(proto->Class == 4)
        {

            if(!(m_pOwner->GetArmorProficiency()&(((uint32)(1))<<proto->SubClass)))
                return INV_ERR_NO_REQUIRED_PROFICIENCY;

        }
        else if(proto->Class == 2)
        {
            if(!(m_pOwner->GetWeaponProficiency()&(((uint32)(1))<<proto->SubClass)))
                return INV_ERR_NO_REQUIRED_PROFICIENCY;

        }
    /*
    uint32 SubClassSkill = GetSkillByProto(proto);

    // removed those loops, use the skill lookup functions
    if(SubClassSkill)
    {
    if(!HasSkillLine(SubClassSkill))
    return INV_ERR_NO_REQUIRED_PROFICIENCY;
    }*/

        if(proto->RequiredSkill)
            if (proto->RequiredSkillRank > m_pOwner->GetSkillAmt(proto->RequiredSkill))
                return INV_ERR_SKILL_ISNT_HIGH_ENOUGH;

        // You are dead !
        if(m_pOwner->getDeathState() == DEAD)
            return INV_ERR_YOU_ARE_DEAD;
    }

    switch(slot)
    {
    case EQUIPMENT_SLOT_HEAD:
        {
            if(type == INVTYPE_HEAD)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_NECK:
        {
            if(type == INVTYPE_NECK)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_SHOULDERS:
        {
            if(type == INVTYPE_SHOULDERS)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_BODY:
        {
            if(type == INVTYPE_BODY)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_CHEST:
        {
            if(type == INVTYPE_CHEST || type == INVTYPE_ROBE)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_WAIST:
        {
            if(type == INVTYPE_WAIST)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_LEGS:
        {
            if(type == INVTYPE_LEGS)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_FEET:
        {
            if(type == INVTYPE_FEET)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_WRISTS:
        {
            if(type == INVTYPE_WRISTS)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_HANDS:
        {
            if(type == INVTYPE_HANDS)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_FINGER1:
    case EQUIPMENT_SLOT_FINGER2:
        {
            if(type == INVTYPE_FINGER)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_TRINKET1:
    case EQUIPMENT_SLOT_TRINKET2:
        {
            if(type == INVTYPE_TRINKET)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_BACK:
        {
            if(type == INVTYPE_CLOAK)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_MAINHAND:
        {
            if(type == INVTYPE_WEAPON || type == INVTYPE_WEAPONMAINHAND ||
                (type == INVTYPE_2HWEAPON && !GetInventoryItem(EQUIPMENT_SLOT_OFFHAND)))
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_OFFHAND:
        {
            if(type == INVTYPE_WEAPON || type == INVTYPE_WEAPONOFFHAND)
            {
                Item* mainweapon = GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
                if(mainweapon) //item exists
                {
                    if(mainweapon->GetProto())
                    {
                        if(mainweapon->GetProto()->InventoryType != INVTYPE_2HWEAPON)
                        {
                            if(m_pOwner->HasSkillLine(SKILL_DUAL_WIELD))
                                return 0;
                            else
                                return INV_ERR_CANT_DUAL_WIELD;
                        }
                        else
                            return INV_ERR_CANT_EQUIP_WITH_TWOHANDED;
                    }
                }
                else
                {
                    if(m_pOwner->HasSkillLine(SKILL_DUAL_WIELD))
                        return 0;
                    else
                        return INV_ERR_CANT_DUAL_WIELD;
                }
            }
            else if(type == INVTYPE_SHIELD || type == INVTYPE_HOLDABLE)
            {
                Item* mainweapon = GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
                if(mainweapon) //item exists
                {
                    if(mainweapon->GetProto())
                    {
                        if(mainweapon->GetProto()->InventoryType != INVTYPE_2HWEAPON)
                        {
                            return 0;
                        }
                        else
                            return INV_ERR_CANT_EQUIP_WITH_TWOHANDED;
                    }
                }
                else
                {
                    return 0;
                }
            }
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;
        }
    case EQUIPMENT_SLOT_RANGED:
        {
            if(type == INVTYPE_RANGED || type == INVTYPE_THROWN || type == INVTYPE_RANGEDRIGHT || type == INVTYPE_RELIC)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT;//6;
        }
    case EQUIPMENT_SLOT_TABARD:
        {
            if(type == INVTYPE_TABARD)
                return 0;
            else
                return INV_ERR_ITEM_DOESNT_GO_TO_SLOT; // 6;
        }
    case BANK_SLOT_BAG_1:
    case BANK_SLOT_BAG_2:
    case BANK_SLOT_BAG_3:
    case BANK_SLOT_BAG_4:
    case BANK_SLOT_BAG_5:
    case BANK_SLOT_BAG_6:
    case BANK_SLOT_BAG_7:
        {
            uint32 bytes,slots;
            int8 islot;

            if(!GetInventoryItem(INVENTORY_SLOT_NOT_SET,slot))
            {
                //check if player got that slot.
                bytes = m_pOwner->GetUInt32Value(PLAYER_BYTES_2);
                slots =(uint8) (bytes >> 16);
                islot = GetInternalBankSlotFromPlayer(slot);
                if(slots < islot)
                {
                    return INV_ERR_MUST_PURCHASE_THAT_BAG_SLOT;
                }

                //in case bank slot exists, check if player can put the item there
                if(type == INVTYPE_BAG)
                {
                    return 0;
                }
                else
                {
                    return INV_ERR_NOT_A_BAG;
                }
            }
            else
            {
                if(GetInventoryItem(INVENTORY_SLOT_NOT_SET,slot)->GetProto()->BagFamily)
                {
                    if((IsBagSlot(slot) && DstInvSlot == INVENTORY_SLOT_NOT_SET))
                    {
                        if(proto->InventoryType == INVTYPE_BAG )
                        {
                            return 0;
                        }
                    }

                    if(proto->BagFamily == GetInventoryItem(INVENTORY_SLOT_NOT_SET,slot)->GetProto()->BagFamily)
                    {
                        return 0;
                    }
                    else
                    {
                        return INV_ERR_ITEM_DOESNT_GO_INTO_BAG;
                    }
                }
                else
                {
                    return 0;
                }
            }
        }
    case INVENTORY_SLOT_BAG_1:    
    case INVENTORY_SLOT_BAG_2:
    case INVENTORY_SLOT_BAG_3:
    case INVENTORY_SLOT_BAG_4:
        {
            if(GetInventoryItem(INVENTORY_SLOT_NOT_SET,slot))
            {
                if(GetInventoryItem(INVENTORY_SLOT_NOT_SET,slot)->GetProto()->BagFamily)
                {
                    if((IsBagSlot(slot) && DstInvSlot == INVENTORY_SLOT_NOT_SET))
                    {
                        if(proto->InventoryType == INVTYPE_BAG )
                        {
                            return 0;
                        }
                    }
                    
                    if(proto->BagFamily == GetInventoryItem(INVENTORY_SLOT_NOT_SET,slot)->GetProto()->BagFamily)
                    {
                        return 0;
                    }
                    else
                    {
                        return INV_ERR_ITEM_DOESNT_GO_INTO_BAG;
                    }
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                if(type == INVTYPE_BAG)
                {
                    return 0;
                }
                else
                {
                    return INV_ERR_NOT_A_BAG;
                }
            }
        }
    case INVENTORY_SLOT_ITEM_1:
    case INVENTORY_SLOT_ITEM_2:
    case INVENTORY_SLOT_ITEM_3:
    case INVENTORY_SLOT_ITEM_4:
    case INVENTORY_SLOT_ITEM_5:
    case INVENTORY_SLOT_ITEM_6:
    case INVENTORY_SLOT_ITEM_7:
    case INVENTORY_SLOT_ITEM_8:
    case INVENTORY_SLOT_ITEM_9:
    case INVENTORY_SLOT_ITEM_10:
    case INVENTORY_SLOT_ITEM_11:
    case INVENTORY_SLOT_ITEM_12:
    case INVENTORY_SLOT_ITEM_13:
    case INVENTORY_SLOT_ITEM_14:
    case INVENTORY_SLOT_ITEM_15:
    case INVENTORY_SLOT_ITEM_16:
        {
            return 0;
        }
    case INVENTORY_KEYRING_1:
    case INVENTORY_KEYRING_2:
    case INVENTORY_KEYRING_3:
    case INVENTORY_KEYRING_4:
    case INVENTORY_KEYRING_5:
    case INVENTORY_KEYRING_6:
    case INVENTORY_KEYRING_7:
    case INVENTORY_KEYRING_8:
    case INVENTORY_KEYRING_9:
    case INVENTORY_KEYRING_10:
    case INVENTORY_KEYRING_11:
    case INVENTORY_KEYRING_12:
    case INVENTORY_KEYRING_13:
    case INVENTORY_KEYRING_14:
    case INVENTORY_KEYRING_15:
    case INVENTORY_KEYRING_16:
	case INVENTORY_KEYRING_17:
	case INVENTORY_KEYRING_18:
	case INVENTORY_KEYRING_19:
	case INVENTORY_KEYRING_20:
	case INVENTORY_KEYRING_21:
	case INVENTORY_KEYRING_22:
	case INVENTORY_KEYRING_23:
	case INVENTORY_KEYRING_24:
	case INVENTORY_KEYRING_25:
	case INVENTORY_KEYRING_26:
	case INVENTORY_KEYRING_27:
	case INVENTORY_KEYRING_28:
	case INVENTORY_KEYRING_29:
	case INVENTORY_KEYRING_30:
	case INVENTORY_KEYRING_31:
	case INVENTORY_KEYRING_32:
        {
            if(proto->BagFamily == ITEM_TYPE_KEYRING )
            {
                    return 0;
            }
            else
            {
                    return INV_ERR_ITEM_DOESNT_GO_INTO_BAG;
            }
        }
    default:
        return 0;
    }
}

//-------------------------------------------------------------------//
//Description: Checks if player can receive the item
//-------------------------------------------------------------------//
int8 ItemInterface::CanReceiveItem(ItemPrototype * item, uint32 amount)
{
    if(!item)
    {
        return NULL;
    }

    if(item->Unique)
    {
        uint32 count = GetItemCount(item->ItemId, true);
        if(count == item->Unique || ((count + amount) > item->Unique))
        {
            return INV_ERR_CANT_CARRY_MORE_OF_THIS;
        }
    }
    return NULL;
}

void ItemInterface::BuyItem(ItemPrototype *item, uint32 amount)
{
    m_pOwner->ModUInt32Value(PLAYER_FIELD_COINAGE, -(GetBuyPriceForItem(item, amount, amount)));
    ItemExtendedCostEntry *ex = ItemExtendedCostStore::getSingleton().LookupEntry(item->ItemExtendedCost);
    for(int i = 0;i<5;i++)
    {
        if(ex->item[i])
        {
            m_pOwner->GetItemInterface()->RemoveItemAmt(ex->item[i],ex->count[i]);
        }
    }
    m_pOwner->ModUInt32Value(PLAYER_FIELD_HONOR_CURRENCY, -(ex->honor));
    m_pOwner->ModUInt32Value(PLAYER_FIELD_ARENA_CURRENCY, -(ex->arena));
}

int8 ItemInterface::CanAffordItem(ItemPrototype *item,uint32 amount)
{
    if(item->ItemExtendedCost)
    {
        ItemExtendedCostEntry *ex = ItemExtendedCostStore::getSingleton().LookupEntry(item->ItemExtendedCost);
        for(int i = 0;i<5;i++)
        {
            if(ex->item[i])
            {
                if(m_pOwner->GetItemInterface()->GetItemCount(ex->item[i]) < ex->count[i], false)
                    return INV_ERR_NOT_ENOUGH_MONEY;
            }
        }
        if(m_pOwner->GetUInt32Value(PLAYER_FIELD_HONOR_CURRENCY) < ex->honor)
            return INV_ERR_NOT_ENOUGH_MONEY;
        if(m_pOwner->GetUInt32Value(PLAYER_FIELD_ARENA_CURRENCY ) < ex->arena)
            return INV_ERR_NOT_ENOUGH_MONEY;
    }
    if(item->BuyPrice)
    {
        int32 price = GetBuyPriceForItem(item, amount, amount);
        if(m_pOwner->GetUInt32Value(PLAYER_FIELD_COINAGE) < price)
        {
            return INV_ERR_NOT_ENOUGH_MONEY;
        }
    }
    if(item->RequiredFaction)
    {
        return INV_ERR_ITEM_REPUTATION_NOT_ENOUGH;
    }
    return NULL;
}

//-------------------------------------------------------------------//
//Description: Gets the Item slot by item type
//-------------------------------------------------------------------//
int8 ItemInterface::GetItemSlotByType(uint32 type)
{
    switch(type)
    {
    case INVTYPE_NON_EQUIP:
        return ITEM_NO_SLOT_AVAILABLE; 
    case INVTYPE_HEAD:
        {
            return EQUIPMENT_SLOT_HEAD;
        }
    case INVTYPE_NECK:
        {
            return EQUIPMENT_SLOT_NECK;
        }
    case INVTYPE_SHOULDERS:
        {
            return EQUIPMENT_SLOT_SHOULDERS;
        }
    case INVTYPE_BODY:
        {
            return EQUIPMENT_SLOT_BODY;
        }
    case INVTYPE_CHEST:
        {
            return EQUIPMENT_SLOT_CHEST;
        }
    case INVTYPE_ROBE: // ???
        {
            return EQUIPMENT_SLOT_CHEST;
        }
    case INVTYPE_WAIST:
        {
            return EQUIPMENT_SLOT_WAIST;
        }
    case INVTYPE_LEGS:
        {
            return EQUIPMENT_SLOT_LEGS;
        }
    case INVTYPE_FEET:
        {
            return EQUIPMENT_SLOT_FEET;
        }
    case INVTYPE_WRISTS:
        {
            return EQUIPMENT_SLOT_WRISTS;
        }
    case INVTYPE_HANDS:
        {
            return EQUIPMENT_SLOT_HANDS;
        }
    case INVTYPE_FINGER:
        {
            if (!GetInventoryItem(EQUIPMENT_SLOT_FINGER1))
                return EQUIPMENT_SLOT_FINGER1;
            else if (!GetInventoryItem(EQUIPMENT_SLOT_FINGER2))
                return EQUIPMENT_SLOT_FINGER2;
            else
                return EQUIPMENT_SLOT_FINGER1; //auto equips always in finger 1
        }
    case INVTYPE_TRINKET:
        {
            if (!GetInventoryItem(EQUIPMENT_SLOT_TRINKET1))
                return EQUIPMENT_SLOT_TRINKET1;
            else if (!GetInventoryItem(EQUIPMENT_SLOT_TRINKET2))
                return EQUIPMENT_SLOT_TRINKET2;
            else
                return EQUIPMENT_SLOT_TRINKET1; //auto equips always on trinket 1
        }
    case INVTYPE_CLOAK:
        {
            return EQUIPMENT_SLOT_BACK;
        }
    case INVTYPE_WEAPON:
        {
            if (!GetInventoryItem(EQUIPMENT_SLOT_MAINHAND) )
                return EQUIPMENT_SLOT_MAINHAND;
            else if(!GetInventoryItem(EQUIPMENT_SLOT_OFFHAND))
                return EQUIPMENT_SLOT_OFFHAND;
            else
                return EQUIPMENT_SLOT_MAINHAND;
        }
    case INVTYPE_SHIELD:
        {
            return EQUIPMENT_SLOT_OFFHAND;
        }
    case INVTYPE_RANGED:
        {
            return EQUIPMENT_SLOT_RANGED;
        }
    case INVTYPE_2HWEAPON:
        {
            return EQUIPMENT_SLOT_MAINHAND;
        }
    case INVTYPE_TABARD:
        {
            return EQUIPMENT_SLOT_TABARD;
        }
    case INVTYPE_WEAPONMAINHAND:
        {
            return EQUIPMENT_SLOT_MAINHAND;
        }
    case INVTYPE_WEAPONOFFHAND:
        {
            return EQUIPMENT_SLOT_OFFHAND;
        }
    case INVTYPE_HOLDABLE:
        {
            return EQUIPMENT_SLOT_OFFHAND;
        }
    case INVTYPE_THROWN:
        return EQUIPMENT_SLOT_RANGED; // ?
    case INVTYPE_RANGEDRIGHT:
        return EQUIPMENT_SLOT_RANGED; // ?
    case INVTYPE_RELIC:
        return EQUIPMENT_SLOT_RANGED;
    case INVTYPE_BAG:
        {
            for (int8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
            {
                if (!GetInventoryItem(i))
                    return i;
            }
            return ITEM_NO_SLOT_AVAILABLE; //bags are not suposed to be auto-equiped when slots are not free
        }
    default:
        return ITEM_NO_SLOT_AVAILABLE;
    }
}

//-------------------------------------------------------------------//
//Description: Gets a Item by guid
//-------------------------------------------------------------------//
Item* ItemInterface::GetItemByGUID(uint64 Guid)
{
    int8 i ;

    //EQUIPMENT
    for(i=EQUIPMENT_SLOT_START;i<EQUIPMENT_SLOT_END;i++)
    {
        if(m_pItems[i] != 0)
        {
            if( m_pItems[i]->GetGUID() == Guid)
            {
                return m_pItems[i];
            }
        }
    }

    //INVENTORY BAGS
    for(i=INVENTORY_SLOT_BAG_START;i<INVENTORY_SLOT_BAG_END;i++)
    {
        if(m_pItems[i] != NULL)
        {
            if(m_pItems[i]->GetGUID()==Guid) { return m_pItems[i]; }

            for (int8 j =0; j < m_pItems[i]->GetProto()->ContainerSlots;j++)
            {
                Item *item2 = ((Container*)m_pItems[i])->GetItem(j);
                if (item2)
                {
                    if (item2->GetGUID() == Guid)
                    {
                        return item2;
                    }
                }
            }
        }
    }

    //INVENTORY
    for(i=INVENTORY_SLOT_ITEM_START;i<INVENTORY_SLOT_ITEM_END;i++)
    {
        if(m_pItems[i] != 0)
        {
            if(m_pItems[i]->GetGUID() == Guid)
            {
                return m_pItems[i];
            }
        }
    }

    //Keyring
    for(i=INVENTORY_KEYRING_START;i<INVENTORY_KEYRING_END;i++)
    {
        if(m_pItems[i] != 0)
        {
            if(m_pItems[i]->GetGUID() == Guid)
            {
                return m_pItems[i];
            }
        }
    }
    return NULL;
}

//-------------------------------------------------------------------//
//Description: Inventory Error report
//-------------------------------------------------------------------//
void ItemInterface::BuildInventoryChangeError(Item *SrcItem, Item *DstItem, uint8 Error)
{
    WorldPacket data;

    data.Initialize( SMSG_INVENTORY_CHANGE_FAILURE );
    data << Error;

    if(Error == 1) 
    {
        if(SrcItem)
        {
            data << SrcItem->GetProto()->RequiredLevel;
        }
    }


    data << (SrcItem ? SrcItem->GetGUID() : uint64(0));
    data << (DstItem ? DstItem->GetGUID() : uint64(0));
    data << uint8(0);

    m_pOwner->GetSession()->SendPacket( &data );
}

void ItemInterface::EmptyBuyBack()
{
     for (uint32 j = 0;j < 12;j++)
     {
         if (m_pBuyBack[j] != NULL)
         {
             m_pBuyBack[j]->DestroyForPlayer(m_pOwner);
             m_pBuyBack[j]->DeleteFromDB();

             if(m_pBuyBack[j]->IsContainer())
             {
                delete static_cast<Container*>(m_pBuyBack[j]);
             }
             else
             {
                delete m_pBuyBack[j];
             }

             m_pOwner->SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + (2*j),0);
             m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + j,0);
             m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j,0);
             m_pBuyBack[j] = NULL;
         }
         else
             break;
     }
}

void ItemInterface::AddBuyBackItem(Item *it,uint32 price)
{
    int i;
    if ((m_pBuyBack[11] != NULL) && (m_pOwner->GetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + 22) != 0))
    {
        if(m_pBuyBack[0] != NULL)
        {
            m_pBuyBack[0]->DestroyForPlayer(m_pOwner);
            m_pBuyBack[0]->DeleteFromDB();
            if(m_pBuyBack[0]->IsContainer())
            {
                delete static_cast<Container*>(m_pBuyBack[0]);
            }
            else
            {
                delete m_pBuyBack[0];
            }
            m_pBuyBack[0] = NULL;
        }

        for (int j = 0;j < 11;j++)
        {
            //SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + (2*j),buyback[j+1]->GetGUID());
            m_pOwner->SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + (2*j),m_pOwner->GetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + ((j+1)*2) ) );
            m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + j,m_pOwner->GetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + j+1));
            m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j,m_pOwner->GetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j+1));
            m_pBuyBack[j] = m_pBuyBack[j+1];
        }
        m_pBuyBack[11] = it;

        m_pOwner->SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + (2*(11)),m_pBuyBack[11]->GetGUID());
        m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + 11,price);
        m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + 11,time(NULL));
        return;
    }

    for(i=0; i < 24;i+=2)
    {
        if((m_pOwner->GetUInt32Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + i) == 0) || (m_pBuyBack[i/2] == NULL))
        {
            Log::getSingleton().outDetail("setting buybackslot %u\n",i/2);
            m_pBuyBack[i/2] = it;

            m_pOwner->SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + i,m_pBuyBack[i/2]->GetGUID());
            //SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + i,it->GetGUID());
            m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + (i/2),price);
            m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + (i/2),time(NULL));
            return;
        }
    }
}

void ItemInterface::RemoveBuyBackItem(uint32 index)
{
    int32 j = 0;
    for (j = index;j < 11;j++)
    {
        if (m_pOwner->GetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + (j*2)) != 0)
        {
            m_pOwner->SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + (2*j), m_pOwner->GetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + ((j+1)*2)));
            m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + j, m_pOwner->GetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + j+1));
            m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j, m_pOwner->GetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j+1));
            
            if ((m_pBuyBack[j+1] != NULL) && (m_pOwner->GetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + ((j+1)*2)) != 0))
            {
                m_pBuyBack[j] = m_pBuyBack[j+1];
            }
            else
            {
                if(m_pBuyBack[j])
                {
                        m_pBuyBack[j] = NULL;
                }

                Log::getSingleton().outDetail("nulling %u\n",(j));
            }
        }
        else
            return;
    }
    j = 11;
    m_pOwner->SetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + (2*j), m_pOwner->GetUInt64Value(PLAYER_FIELD_VENDORBUYBACK_SLOT_1 + ((j+1)*2)));
    m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + j, m_pOwner->GetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + j+1));
    m_pOwner->SetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j, m_pOwner->GetUInt32Value(PLAYER_FIELD_BUYBACK_TIMESTAMP_1 + j+1));
    if(m_pBuyBack[11])
    {
        m_pBuyBack[11] = NULL;
    }
    
}

//-------------------------------------------------------------------//
//Description: swap inventory slots
//-------------------------------------------------------------------//
void ItemInterface::SwapItemSlots(int8 srcslot, int8 dstslot)
{
    ASSERT(srcslot < MAX_INVENTORY_SLOT && srcslot >= NULL);
    ASSERT(dstslot < MAX_INVENTORY_SLOT && dstslot >= NULL);

    Item *SrcItem = GetInventoryItem(srcslot);
    Item *DstItem = GetInventoryItem(dstslot);

    if(SrcItem && DstItem && SrcItem->GetEntry()==DstItem->GetEntry()&& SrcItem->GetProto()->MaxCount>1)
    {
        uint32 total=SrcItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT)+DstItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
        if(total<=DstItem->GetProto()->MaxCount)
        {
            DstItem->ModUInt32Value(ITEM_FIELD_STACK_COUNT, SrcItem->GetUInt32Value(ITEM_FIELD_STACK_COUNT));
            SafeFullRemoveItemFromSlot(INVENTORY_SLOT_NOT_SET, srcslot);
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

    m_pItems[srcslot] = DstItem;
    m_pItems[dstslot] = SrcItem;

    if(m_pItems[dstslot])
    {
        m_pOwner->SetUInt64Value( (uint16)(PLAYER_FIELD_INV_SLOT_HEAD  + (dstslot*2)),  m_pItems[dstslot]->GetGUID()  );
    }
    else
    {
        m_pOwner->SetUInt64Value( (uint16)(PLAYER_FIELD_INV_SLOT_HEAD  + (dstslot*2)), 0 );
    }

    if( m_pItems[srcslot])
    {
        m_pOwner->SetUInt64Value( PLAYER_FIELD_INV_SLOT_HEAD  + srcslot*2, m_pItems[srcslot]->GetGUID() );
    }
    else
    {
        m_pOwner->SetUInt64Value( (uint16)(PLAYER_FIELD_INV_SLOT_HEAD  + (srcslot*2)), 0 );
    }

    bool noreapply=( srcslot < EQUIPMENT_SLOT_END &&  dstslot < EQUIPMENT_SLOT_END);//just move inside inv, no equip/unequip

    if(srcslot < EQUIPMENT_SLOT_END)    // source item is inside inventory
    {
        // remove mods from original item
        if(!noreapply)
        {
            if(m_pItems[dstslot])        // Remove mods from the old item in this slot.
                m_pOwner->ApplyItemMods(m_pItems[dstslot], srcslot, false);

            if(m_pItems[srcslot])        // Apply mods from the new item into this slot
                m_pOwner->ApplyItemMods(m_pItems[srcslot], dstslot, true);
        }

        if(m_pItems[srcslot]) // dstitem goes into here.
        {
            int VisibleBase = PLAYER_VISIBLE_ITEM_1_0 + (srcslot * 16);
            m_pOwner->SetUInt32Value(VisibleBase, m_pItems[srcslot]->GetUInt32Value(OBJECT_FIELD_ENTRY));
            m_pOwner->SetUInt32Value(VisibleBase + 1, m_pItems[srcslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT));
            m_pOwner->SetUInt32Value(VisibleBase + 2, m_pItems[srcslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 3));
            m_pOwner->SetUInt32Value(VisibleBase + 3, m_pItems[srcslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 6));
            m_pOwner->SetUInt32Value(VisibleBase + 4, m_pItems[srcslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 9));
            m_pOwner->SetUInt32Value(VisibleBase + 5, m_pItems[srcslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 12));
            m_pOwner->SetUInt32Value(VisibleBase + 6, m_pItems[srcslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 15));
            m_pOwner->SetUInt32Value(VisibleBase + 7, m_pItems[srcslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 18));
            m_pOwner->SetUInt32Value(VisibleBase + 8, m_pItems[srcslot]->GetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID));

            // handle bind on equip
            if(m_pItems[srcslot]->GetProto()->Bonding == ITEM_BIND_ON_EQUIP)
                m_pItems[srcslot]->SoulBind();

        } 
        else 
        {
            int VisibleBase = PLAYER_VISIBLE_ITEM_1_0 + (srcslot * 16);
            m_pOwner->SetUInt32Value(VisibleBase, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 1, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 2, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 3, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 4, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 5, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 6, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 7, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 8, 0);
        }
    }  

    if(dstslot < EQUIPMENT_SLOT_END)    // source item is inside inventory
    {
        // remove mods from original item
        if(!noreapply)
        {
            if(m_pItems[srcslot])        // Remove mods from the old item in this slot.
                m_pOwner->ApplyItemMods(m_pItems[srcslot], dstslot, false);

            if(m_pItems[dstslot])        // Apply mods from the new item into this slot
                m_pOwner->ApplyItemMods(m_pItems[dstslot], srcslot, true);
        }

        if(m_pItems[dstslot]) // srcitem goes into here.
        {
            int VisibleBase = PLAYER_VISIBLE_ITEM_1_0 + (dstslot * 16);
            m_pOwner->SetUInt32Value(VisibleBase, m_pItems[dstslot]->GetUInt32Value(OBJECT_FIELD_ENTRY));
            m_pOwner->SetUInt32Value(VisibleBase + 1, m_pItems[dstslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT));
            m_pOwner->SetUInt32Value(VisibleBase + 2, m_pItems[dstslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 3));
            m_pOwner->SetUInt32Value(VisibleBase + 3, m_pItems[dstslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 6));
            m_pOwner->SetUInt32Value(VisibleBase + 4, m_pItems[dstslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 9));
            m_pOwner->SetUInt32Value(VisibleBase + 5, m_pItems[dstslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 12));
            m_pOwner->SetUInt32Value(VisibleBase + 6, m_pItems[dstslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 15));
            m_pOwner->SetUInt32Value(VisibleBase + 7, m_pItems[dstslot]->GetUInt32Value(ITEM_FIELD_ENCHANTMENT + 18));
            m_pOwner->SetUInt32Value(VisibleBase + 8, m_pItems[dstslot]->GetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID));

            // handle bind on equip
            if(m_pItems[dstslot]->GetProto()->Bonding == ITEM_BIND_ON_EQUIP)
                m_pItems[dstslot]->SoulBind();

        } else {
            int VisibleBase = PLAYER_VISIBLE_ITEM_1_0 + (dstslot * 16);
            m_pOwner->SetUInt32Value(VisibleBase, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 1, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 2, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 3, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 4, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 5, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 6, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 7, 0);
            m_pOwner->SetUInt32Value(VisibleBase + 8, 0);
        }
    }  
    if(dstslot == EQUIPMENT_SLOT_OFFHAND || srcslot == EQUIPMENT_SLOT_OFFHAND)
    {
        if(m_pItems[EQUIPMENT_SLOT_OFFHAND] != 0 && m_pItems[EQUIPMENT_SLOT_OFFHAND]->GetProto()->Class == ITEM_CLASS_WEAPON)
            m_pOwner->SetDuelWield(true);
        else
            m_pOwner->SetDuelWield(false);
    }
}

//-------------------------------------------------------------------//
//Description: Item Loading
//-------------------------------------------------------------------//
void ItemInterface::mLoadItemsFromDatabase()
{
    int8 containerslot, slot;
    Item* item;
    ItemPrototype *proto;

    std::stringstream invq;
    invq << "SELECT * FROM playeritems WHERE ownerguid=" << m_pOwner->GetGUIDLow() << " ORDER BY containerslot ASC";
    QueryResult *result = sDatabase.Query( invq.str().c_str() );
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();

            containerslot = fields[10].GetInt8();
            slot = fields[11].GetInt8();
            proto = objmgr.GetItemPrototype(fields[2].GetUInt32());

            if(proto)
            {
                if(proto->InventoryType == INVTYPE_BAG)
                {
                    item=new Container();
                    ((Container*)item)->LoadFromDB(fields);

                }
                else
                {
                    item = new Item();
                    item->LoadFromDB(fields, m_pOwner, false);

                }
                SafeAddItem(item, containerslot, slot);
            }
        }
        while( result->NextRow() );
    }

    delete result;
}

//-------------------------------------------------------------------//
//Description: Item saving
//-------------------------------------------------------------------//
void ItemInterface::mSaveItemsToDatabase(bool first)
{
    int8 x;

    for(x = EQUIPMENT_SLOT_START; x < INVENTORY_KEYRING_END; ++x)
    {
        if(GetInventoryItem(x) != NULL)
        {
            if(!( (m_pOwner->GetItemInterface()->GetInventoryItem(x)->GetProto()->Flags)&2 )) // skip conjured item on save
            {
                if(IsBagSlot(x) && GetInventoryItem(x)->IsContainer())
                {
                    ((Container*)GetInventoryItem(x))->SaveBagToDB(x, first);
                }
                else
                {
                    GetInventoryItem(x)->SaveToDB(INVENTORY_SLOT_NOT_SET, x, first);
                }
            }
        }
    }
}

bool ItemInterface::AddItemToFreeBankSlot(Item *item)
{
    //special items first
    for(int8 i=BANK_SLOT_BAG_START;i<BANK_SLOT_BAG_END;i++)
    {
        if(m_pItems[i])
        {
            if (m_pItems[i]->GetProto()->BagFamily == item->GetProto()->BagFamily)
            {
                if(m_pItems[i]->IsContainer())
                {
                    bool result =  static_cast<Container*>(m_pItems[i])->AddItemToFreeSlot(item);
                    if(result) { return true; }
                }
            }
        }
    }

    for(int8 i= BANK_SLOT_ITEM_START;i< BANK_SLOT_ITEM_END;i++)
    {
        if(m_pItems[i] == NULL)
        {
            return SafeAddItem(item, INVENTORY_SLOT_NOT_SET, i);
        }
    }

    for(int8 i=BANK_SLOT_BAG_START;i<BANK_SLOT_BAG_END;i++)
    {
        if(m_pItems[i] != NULL && m_pItems[i]->GetProto()->BagFamily == 0) //special bags ignored
        {
            for (int8 j =0; j < m_pItems[i]->GetProto()->ContainerSlots;j++)
            {
                Item *item2 = ((Container*)m_pItems[i])->GetItem(j);
                if (item2 == NULL)
                {
                    return SafeAddItem(item, i, j);
                }
            }
        }
    }
    return false;
}

int8 ItemInterface::FindSpecialBag(Item *item)
{
    for(int8 i=INVENTORY_SLOT_BAG_START;i<INVENTORY_SLOT_BAG_END;i++)
    {
        if(m_pItems[i] != NULL)
        {
            if (m_pItems[i]->GetProto()->BagFamily == item->GetProto()->BagFamily)
            {
                return i;
            }
        }
    }
    return ITEM_NO_SLOT_AVAILABLE;
}

int8 ItemInterface::FindFreeKeyringSlot()
{
    for(int8 i=INVENTORY_KEYRING_START;i<INVENTORY_KEYRING_END;i++)
    {
        if(m_pItems[i] == NULL)
        {
            return i;
        }
    }
    return ITEM_NO_SLOT_AVAILABLE;
}

SlotResult ItemInterface::FindFreeInventorySlot(ItemPrototype *proto)
{
    //special item
    //special slots will be ignored of item is not set
    if(proto)
    {
        if(proto->BagFamily)
        {
            if(proto->BagFamily == ITEM_TYPE_KEYRING)
            {
                for(int8 i = INVENTORY_KEYRING_START; i < INVENTORY_KEYRING_END; i++)
                {
                    if(m_pItems[i] == NULL)
                    {
                        result.ContainerSlot = ITEM_NO_SLOT_AVAILABLE;
                        result.Slot = i;
                        result.Result = true;
                        return result;
                    }
                }
            }
            else
            {
                for(int8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END;i++)
                {
                    if(m_pItems[i] && m_pItems[i]->IsContainer())
                    {
                        if (m_pItems[i]->GetProto()->BagFamily == proto->BagFamily)
                        {
                            int8 slot = ((Container*)m_pItems[i])->FindFreeSlot();
                            if(slot != ITEM_NO_SLOT_AVAILABLE) 
                            {
                                result.ContainerSlot = i;
                                result.Slot = slot;
                                result.Result = true;
                                return result;
                            }
                        }
                    }
                }
            }
        }
    }

    //backpack
    for(int8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if(!item) 
        {
            result.ContainerSlot = ITEM_NO_SLOT_AVAILABLE;
            result.Slot = i;
            result.Result = true;
            return result;
        }
    }

    //bags
    for(int8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if(item)
        {
            if(item->IsContainer() && !item->GetProto()->BagFamily)
            {
                int8 slot = ((Container*)m_pItems[i])->FindFreeSlot();
                if(slot != ITEM_NO_SLOT_AVAILABLE) 
                { 
                    result.ContainerSlot = i;
                    result.Slot = slot;
                    result.Result = true;
                    return result;
                }
            }
        }
    }

    result.ContainerSlot = ITEM_NO_SLOT_AVAILABLE;
    result.Slot = ITEM_NO_SLOT_AVAILABLE;
    result.Result = false;

    return result;
}

SlotResult ItemInterface::FindFreeBankSlot(ItemPrototype *proto)
{
    //special item
    //special slots will be ignored of item is not set
    if(proto)
    {
        if(proto->BagFamily)
        {
            for(int8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END;i++)
            {
                if(m_pItems[i] && m_pItems[i]->IsContainer())
                {
                    if (m_pItems[i]->GetProto()->BagFamily == proto->BagFamily)
                    {
                        int8 slot = ((Container*)m_pItems[i])->FindFreeSlot();
                        if(slot != ITEM_NO_SLOT_AVAILABLE) 
                        {
                            result.ContainerSlot = i;
                            result.Slot = slot;
                            result.Result = true;
                            return result;
                        }
                    }
                }
            }
        }
    }

    //backpack
    for(int8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_ITEM_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if(!item) 
        {
            result.ContainerSlot = ITEM_NO_SLOT_AVAILABLE;
            result.Slot = i;
            result.Result = true;
            return result;
        }
    }

    //bags
    for(int8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; i++)
    {
        Item *item = GetInventoryItem(i);
        if(item)
        {
            if(item->IsContainer() && !item->GetProto()->BagFamily)
            {
                int8 slot = ((Container*)m_pItems[i])->FindFreeSlot();
                if(slot != ITEM_NO_SLOT_AVAILABLE) 
                { 
                    result.ContainerSlot = i;
                    result.Slot = slot;
                    result.Result = true;
                    return result;
                }
            }
        }
    }

    result.ContainerSlot = ITEM_NO_SLOT_AVAILABLE;
    result.Slot = ITEM_NO_SLOT_AVAILABLE;
    result.Result = false;

    return result;
}

void ItemInterface::ReduceItemDurability()
{
   uint32 f = sRand.randInt(100);
   if(f <= 10) //10% chance to loose 1 dur from a random valid item.
   {
       int32 slot = sRand.randInt(EQUIPMENT_SLOT_END);
       Item *pItem = GetInventoryItem(INVENTORY_SLOT_NOT_SET, slot);
       if(pItem)
       {
           if(pItem->GetUInt32Value(ITEM_FIELD_DURABILITY) && pItem->GetUInt32Value(ITEM_FIELD_MAXDURABILITY))
           {
               pItem->SetUInt32Value(ITEM_FIELD_DURABILITY, (pItem->GetUInt32Value(ITEM_FIELD_DURABILITY)-1));
               //check final durabiity
               if(!pItem->GetUInt32Value(ITEM_FIELD_DURABILITY)) //no dur left
               {
                    this->GetOwner()->ApplyItemMods(pItem,slot,false);
               }
           }
       }
   }
}

