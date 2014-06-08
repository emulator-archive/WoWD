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

#ifndef WOWSERVER_ITEM_H
#define WOWSERVER_ITEM_H

#include "Skill.h"

struct EnchantmentInstance
{
    EnchantEntry * Enchantment;
    bool BonusApplied;
    uint32 Slot;
    uint32 AddAmount;
    time_t ApplyTime;
    uint32 Duration;
    bool RemoveAtLogout;
};

typedef map<uint32, EnchantmentInstance> EnchantmentMap;

#define APPLY true
#define REMOVE false

class WOWD_SERVER_DECL Item : public Object
{
public:
    Item ( );
    ~Item();

    void Create( uint32 guidlow, uint32 itemid, Player* owner );

    inline ItemPrototype* GetProto() const { return m_itemProto; }

    inline Player* GetOwner() const { return m_owner; }
    void SetOwner(Player *owner);

    inline bool IsContainer(){return (m_objectTypeId == TYPEID_CONTAINER) ? true : false; }
    
    //! DB Serialization
    void LoadFromDB(Field *fields, Player* plr, bool light);
    void SaveToDB(int8 containerslot, int8 slot, bool firstsave = false);
    bool LoadAuctionItemFromDB(uint64 guid);
    void DeleteFromDB();
    
    inline void SoulBind()
    {
        if(!this->HasFlag(ITEM_FIELD_FLAGS,1))
            this->SetFlag(ITEM_FIELD_FLAGS,1);
    }
    inline bool IsSoulbound()
    {
        return HasFlag(ITEM_FIELD_FLAGS, 1);
    }

    inline uint32 GetChargesLeft()
    {
        for(uint32 x=0;x<5;x++)
            if(m_itemProto->SpellId[x])
                return GetUInt32Value(ITEM_FIELD_SPELL_CHARGES+x);

        return 0;
    }

    inline uint32 GetEnchantmentApplytime(uint32 slot)
    {
         return Enchantments[slot].ApplyTime;
    }

    //! Adds an enchantment to the item.
    int32 AddEnchantment(EnchantEntry * Enchantment, uint32 Duration, bool Perm = false, bool apply = true, bool RemoveAtLogout = false);

    //! Removes an enchantment from the item.
    void RemoveEnchantment(uint32 EnchantmentSlot);
    
    // Removes related temporary enchants
    void RemoveRelatedEnchants(EnchantEntry * newEnchant);

    //! Adds the bonus on an enchanted item.
    void ApplyEnchantmentBonus(uint32 Slot, bool Apply);

    //! Applies all enchantment bonuses (use on equip)
    void ApplyEnchantmentBonuses();

    //! Removes all enchantment bonuses (use on dequip)
    void RemoveEnchantmentBonuses();

    //! Event to remove an enchantment.
    void EventRemoveEnchantment(uint32 Slot);

    //! Check if we have an enchantment of this id?
    int32 HasEnchantment(uint32 Id);

    //! Modify the time of an existing enchantment.
    void ModifyEnchantmentTime(uint32 Slot, uint32 Duration);

    //! Find free enchantment slot.
    int32 FindFreeEnchantSlot(EnchantEntry * Enchantment);

    //! Removes all enchantments.
    void RemoveAllEnchantments(bool OnlyTemporary);

    //! Sends SMSG_ITEM_UPDATE_ENCHANT_TIME
    void SendEnchantTimeUpdate(uint32 Slot, uint32 Duration);

    //! Applies any random properties the item has.
    void ApplyRandomProperties();

    inline void SetCount(uint32 amt) { SetUInt32Value(ITEM_FIELD_STACK_COUNT,amt); }
    inline void SetDurability(uint32 Value) { SetUInt32Value(ITEM_FIELD_DURABILITY,Value); };
    inline void SetDurabilityToMax() { SetUInt32Value(ITEM_FIELD_DURABILITY,GetUInt32Value(ITEM_FIELD_MAXDURABILITY)); }
    inline uint32 GetDurability() { return GetUInt32Value(ITEM_FIELD_DURABILITY); }
    inline uint32 GetDurabilityMax() { return GetUInt32Value(ITEM_FIELD_MAXDURABILITY); }

    void RemoveFromWorld();
    void AddToWorld();

    Loot *loot;
    bool locked;

protected:

    ItemPrototype *m_itemProto;
    uint32 _dbguid;
    EnchantmentMap Enchantments;

    Player *m_owner; // let's not bother the manager with unneeded requests
};

uint32 GetSkillByProto(ItemPrototype *proto);

uint32 GetSellPriceForItem(ItemPrototype *proto, uint32 count);
uint32 GetBuyPriceForItem(ItemPrototype *proto, uint32 count, uint32 vendorcount);

uint32 GetSellPriceForItem(uint32 itemid, uint32 count);
uint32 GetBuyPriceForItem(uint32 itemid, uint32 count, uint32 vendorcount);

#endif
