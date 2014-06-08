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

Container::Container( ) : Item()
{
    m_objectType |= TYPE_CONTAINER;
    m_objectTypeId = TYPEID_CONTAINER;

    m_valuesCount = CONTAINER_END;

    m_Slot = NULL;
}

Container::~Container( )
{
   for(int8 i = 0; i < m_itemProto->ContainerSlots; i++)
    {
        if(m_Slot[i])
        {
            if(m_Slot[i]->IsContainer())
                delete ((Container*)m_Slot[i]);
            else
                delete m_Slot[i];
        }
    }
    //burlex: memleak fix
    delete [] m_Slot;
}
void Container::LoadFromDB( Field*fields )
{
    _dbguid = fields[1].GetUInt32();

    Object::_Create( fields[1].GetUInt32(), HIGHGUID_CONTAINER );
    uint32 itemid=fields[2].GetUInt32();
    m_itemProto = objmgr.GetItemPrototype( itemid );

    ASSERT(m_itemProto);
    SetUInt32Value( OBJECT_FIELD_ENTRY, itemid );
    

    SetFloatValue( OBJECT_FIELD_SCALE_X, 1.0f );
  
    SetUInt32Value( ITEM_FIELD_CREATOR, fields[3].GetUInt32() );
    SetUInt32Value( ITEM_FIELD_STACK_COUNT, 1);
    
    SetUInt32Value( ITEM_FIELD_FLAGS, fields[6].GetUInt32());
    SetUInt32Value( ITEM_FIELD_RANDOM_PROPERTIES_ID, fields[7].GetUInt32());

    SetUInt32Value( ITEM_FIELD_MAXDURABILITY, m_itemProto->MaxDurability);
    SetUInt32Value( ITEM_FIELD_DURABILITY, fields[9].GetUInt32());
  

    SetUInt32Value( CONTAINER_FIELD_NUM_SLOTS, m_itemProto->ContainerSlots);

    m_Slot = new Item*[m_itemProto->ContainerSlots];
    memset(m_Slot, 0, sizeof(Item*)*(m_itemProto->ContainerSlots));

}

void Container::Create( uint32 guidlow, uint32 itemid, Player *owner )
{
    Object::_Create( guidlow, HIGHGUID_CONTAINER );

    m_itemProto = objmgr.GetItemPrototype( itemid );
    ASSERT(m_itemProto);

    SetUInt32Value( OBJECT_FIELD_ENTRY, itemid );
    SetFloatValue( OBJECT_FIELD_SCALE_X, 1.0f );
    SetUInt64Value( ITEM_FIELD_OWNER, owner->GetGUID() );
    SetUInt64Value( ITEM_FIELD_CONTAINED, owner->GetGUID() );
    SetUInt32Value( ITEM_FIELD_STACK_COUNT, 1 );
    SetUInt32Value( CONTAINER_FIELD_NUM_SLOTS, m_itemProto->ContainerSlots);

    m_Slot = new Item*[m_itemProto->ContainerSlots];
    memset(m_Slot, 0, sizeof(Item*)*(m_itemProto->ContainerSlots));

    m_owner = owner;
}


int8 Container::FindFreeSlot()
{
    int8 TotalSlots = GetUInt32Value( CONTAINER_FIELD_NUM_SLOTS );
    for (int8 i=0; i < TotalSlots; i++)
    {
        if(!m_Slot[i]) 
        { 
            return i; 
        }
    }
    return ITEM_NO_SLOT_AVAILABLE;
}

bool Container::HasItems()
{
    int8 TotalSlots = GetUInt32Value( CONTAINER_FIELD_NUM_SLOTS );
    for (int8 i=0; i < TotalSlots; i++)
    {
        if(m_Slot[i]) 
        { 
            return true; 
        }
    }
    return false;
}

bool Container::AddItem(int8 slot, Item *item)
{
    //ASSERT(m_Slot[slot] == NULL);
    if(m_Slot[slot] != NULL)
    {
        //sLog.outString("Bad container item %u slot %d", item->GetGUID(), slot);
        return false;
    }

    if (!m_owner)
        return false;

    m_Slot[slot] = item;
    item->SetUInt64Value(ITEM_FIELD_CONTAINED, GetGUID());
    item->SetOwner(m_owner);

    if (item->GetProto()->Bonding == ITEM_BIND_ON_PICKUP) 
        item->SoulBind();

    SetUInt64Value(CONTAINER_FIELD_SLOT_1  + (slot*2), item->GetGUID());

    //new version to fix bag issues
    if(m_owner->IsInWorld() && !item->IsInWorld())
    {
        item->AddToWorld();

        ByteBuffer buf(2500);
        uint32 count = item->BuildCreateUpdateBlockForPlayer(&buf, m_owner);
        m_owner->PushUpdateData(&buf, count);
    }
    return true;
}

void Container::SwapItems(int8 SrcSlot, int8 DstSlot)
{
    Item *temp;
    if(m_Slot[DstSlot] &&  m_Slot[SrcSlot]&&m_Slot[DstSlot]->GetEntry()==m_Slot[SrcSlot]->GetEntry() && m_Slot[DstSlot]->GetProto()->MaxCount>1)
    {
        uint32 total=m_Slot[SrcSlot]->GetUInt32Value(ITEM_FIELD_STACK_COUNT)+m_Slot[DstSlot]->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
        if(total<=m_Slot[DstSlot]->GetProto()->MaxCount)
        {
            m_Slot[DstSlot]->ModUInt32Value(ITEM_FIELD_STACK_COUNT,m_Slot[SrcSlot]->GetUInt32Value(ITEM_FIELD_STACK_COUNT));
            SafeFullRemoveItemFromSlot(SrcSlot);
            return;
        }
        else
        {
            if(m_Slot[DstSlot]->GetUInt32Value(ITEM_FIELD_STACK_COUNT) == m_Slot[DstSlot]->GetProto()->MaxCount)
            {

            }
            else
            {
                int32 delta=m_Slot[DstSlot]->GetProto()->MaxCount-m_Slot[DstSlot]->GetUInt32Value(ITEM_FIELD_STACK_COUNT);
                m_Slot[DstSlot]->SetUInt32Value(ITEM_FIELD_STACK_COUNT,m_Slot[DstSlot]->GetProto()->MaxCount);
                m_Slot[SrcSlot]->ModUInt32Value(ITEM_FIELD_STACK_COUNT,-delta);
                return;
            }
        }
    }
   
    temp = m_Slot[SrcSlot];
    m_Slot[SrcSlot] = m_Slot[DstSlot];
    m_Slot[DstSlot] = temp;

    if( m_Slot[DstSlot])
    {
        SetUInt64Value(CONTAINER_FIELD_SLOT_1  + (DstSlot*2),  m_Slot[DstSlot]->GetGUID()  );
    }
    else
    {
        SetUInt64Value(CONTAINER_FIELD_SLOT_1  + (DstSlot*2), 0 );
    }

    if( m_Slot[SrcSlot])
    {
        SetUInt64Value(CONTAINER_FIELD_SLOT_1  + (SrcSlot*2), m_Slot[SrcSlot]->GetGUID() );
    }
    else
    {
        SetUInt64Value(CONTAINER_FIELD_SLOT_1  + (SrcSlot*2), 0 );
    }
}

Item *Container::SafeRemoveAndRetreiveItemFromSlot(int8 slot, bool destroy)
{
    ASSERT(slot < GetProto()->ContainerSlots);

    Item *pItem = m_Slot[slot];

    if (pItem == NULL) return NULL;
    m_Slot[slot] = NULL;

    SetUInt64Value(CONTAINER_FIELD_SLOT_1  + slot*2, 0 );
    pItem->SetUInt64Value(ITEM_FIELD_CONTAINED, 0);

    if(destroy)
    {
        if(pItem->IsInWorld())
        {
            pItem->RemoveFromWorld();
        }
        pItem->DeleteFromDB();
    }

    return pItem;
}

bool Container::SafeFullRemoveItemFromSlot(int8 slot)
{
    ASSERT(slot < GetProto()->ContainerSlots);

    Item *pItem = m_Slot[slot];

    if (pItem == NULL) return false;
    m_Slot[slot] = NULL;

    SetUInt64Value(CONTAINER_FIELD_SLOT_1  + slot*2, 0 );
    pItem->SetUInt64Value(ITEM_FIELD_CONTAINED, 0);

    if(pItem->IsInWorld())
    {
        pItem->RemoveFromWorld();
    }
    pItem->DeleteFromDB();
    delete pItem;

    return true;
}

bool Container::AddItemToFreeSlot(Item *pItem)
{
    int8 slot;
    for(slot = 0; slot < GetProto()->ContainerSlots; slot++)
    {
        if(!m_Slot[slot])
        {
            m_Slot[slot] = pItem;
            pItem->SetUInt64Value(ITEM_FIELD_CONTAINED, GetGUID());
            pItem->SetOwner(m_owner);

            SetUInt64Value(CONTAINER_FIELD_SLOT_1  + (slot*2), pItem->GetGUID());

            if(m_owner->IsInWorld() && !pItem->IsInWorld())
            {
                pItem->AddToWorld();
                ByteBuffer buf(2500);
                uint32 count = pItem->BuildCreateUpdateBlockForPlayer( &buf, m_owner );
                m_owner->PushUpdateData(&buf, count);
            }
            return true;
        }
    }
    return false;
}


void Container::SaveBagToDB(int8 slot, bool first)
{
    ((Item*)this)->SaveToDB(INVENTORY_SLOT_NOT_SET, slot);

    for(int8 i = 0; i < m_itemProto->ContainerSlots; i++)
    {
        if (m_Slot[i])
        {
            m_Slot[i]->SaveToDB(slot, i, first);
        }
    }
}


