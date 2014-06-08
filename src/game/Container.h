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

#ifndef _CONTAINER_H
#define _CONTAINER_H

class WOWD_SERVER_DECL Container : public Item
{
public:
    friend class WorldSession;
    Container( );
    ~Container();

    void Create( uint32 guidlow, uint32 itemid, Player *owner );
    void LoadFromDB( Field*fields);

    bool AddItem(int8 slot, Item *item);
    bool AddItemToFreeSlot(Item *pItem);
    inline Item *GetItem(int8 slot)
    {
        if((uint8)slot < GetProto()->ContainerSlots)
            return m_Slot[slot];
        else
            return 0;
    }
    
    int8 FindFreeSlot();
    bool HasItems();
    
    void SwapItems(int8 SrcSlot,int8 DstSlot);
    Item *SafeRemoveAndRetreiveItemFromSlot(int8 slot, bool destroy); //doesnt destroy item from memory
    bool SafeFullRemoveItemFromSlot(int8 slot); //destroys item fully
   
    void SaveBagToDB(int8 slot, bool first);

protected:
    Item **m_Slot;
};

#endif
