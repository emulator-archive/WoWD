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
Item::Item( )
{
    m_objectType |= TYPE_ITEM;
    m_objectTypeId = TYPEID_ITEM;

    m_valuesCount = ITEM_END;

    m_itemProto = NULL;
    m_owner = NULL;
    loot = NULL;
    locked = false;
    _dbguid = 0;
}

Item::~Item()
{
    if(loot != NULL)
        delete loot;
    sEventMgr.RemoveEvents(this);

    EnchantmentMap::iterator itr;
    for(itr = Enchantments.begin(); itr != Enchantments.end(); ++itr)
    {
        if(itr->second.Enchantment->type == 0 && itr->second.Slot == 0 && itr->second.ApplyTime == 0 && itr->second.Duration == 0)
        {
            delete itr->second.Enchantment;
            itr->second.Enchantment = NULL;
        }
    }
}

void Item::Create( uint32 guidlow, uint32 itemid, Player *owner )
{
    Object::_Create( guidlow, HIGHGUID_ITEM );

    SetUInt32Value( OBJECT_FIELD_ENTRY, itemid );
    SetFloatValue( OBJECT_FIELD_SCALE_X, 1.0f );

    SetUInt64Value( ITEM_FIELD_OWNER, owner->GetGUID() );
    SetUInt64Value( ITEM_FIELD_CONTAINED, owner->GetGUID() );
    SetUInt32Value( ITEM_FIELD_STACK_COUNT, 1 );

    m_itemProto = objmgr.GetItemPrototype( itemid );
    ASSERT(m_itemProto);
     
    SetUInt32Value( ITEM_FIELD_SPELL_CHARGES , m_itemProto->SpellCharges[0] );
    SetUInt32Value( ITEM_FIELD_SPELL_CHARGES_01 , m_itemProto->SpellCharges[1] );
    SetUInt32Value( ITEM_FIELD_SPELL_CHARGES_02 , m_itemProto->SpellCharges[2] );
    SetUInt32Value( ITEM_FIELD_SPELL_CHARGES_03 , m_itemProto->SpellCharges[3] );
    SetUInt32Value( ITEM_FIELD_SPELL_CHARGES_04 , m_itemProto->SpellCharges[4] );


    SetUInt32Value( ITEM_FIELD_MAXDURABILITY, m_itemProto->MaxDurability);
    SetUInt32Value( ITEM_FIELD_DURABILITY, m_itemProto->MaxDurability);

    m_owner = owner;
    _dbguid = GetGUID();
    if(m_itemProto->LockId > 1)
        locked = true;
    else
        locked = false;
}

void Item::LoadFromDB(    Field *fields, Player * plr, bool light)
{
    _dbguid = fields[1].GetUInt32();
    Object::_Create( fields[1].GetUInt32(), HIGHGUID_ITEM );
    uint32 itemid=fields[2].GetUInt32();
    m_itemProto = objmgr.GetItemPrototype( itemid );
    ASSERT(m_itemProto);
    
    if(m_itemProto->LockId > 1)
        locked = true;
    else
        locked = false;
    
    SetUInt32Value( OBJECT_FIELD_ENTRY, itemid );
    m_owner = plr;

    SetFloatValue( OBJECT_FIELD_SCALE_X, 1.0f );
  
    SetUInt32Value( ITEM_FIELD_CREATOR, fields[3].GetUInt32() );
    SetUInt32Value( ITEM_FIELD_STACK_COUNT,  fields[4].GetUInt32());
    for(uint32 x=0;x<5;x++)
    if(m_itemProto->SpellId[x])
    {
        SetUInt32Value( ITEM_FIELD_SPELL_CHARGES+x , fields[5].GetUInt32() );
        break;
    }
    
    SetUInt32Value( ITEM_FIELD_FLAGS, fields[6].GetUInt32());
    SetUInt32Value( ITEM_FIELD_RANDOM_PROPERTIES_ID, fields[7].GetUInt32());

    SetUInt32Value( ITEM_FIELD_ITEM_TEXT_ID, fields[8].GetUInt32());

    SetUInt32Value( ITEM_FIELD_MAXDURABILITY, m_itemProto->MaxDurability);
    SetUInt32Value( ITEM_FIELD_DURABILITY, fields[9].GetUInt32());

    if(light) return;

    string enchant_field = fields[12].GetString();
    vector<string> enchants = StrSplit(enchant_field, ";");
    uint32 enchant_id;
    EnchantEntry * entry;
    uint32 time_left;
    uint32 enchslot;

    for(vector<string>::iterator itr = enchants.begin(); itr != enchants.end(); ++itr)
    {
        if(sscanf((*itr).c_str(), "%u,%u,%u", &enchant_id, &time_left, &enchslot) == 3)
        {
            entry = sEnchantStore.LookupEntry(enchant_id);
            if(entry && entry->Id == enchant_id)
                AddEnchantment(entry, time_left, (enchslot != 2) ? false : true, false);
            else
            {
                EnchantEntry *pEnchant = new EnchantEntry;
                memset(pEnchant,0,sizeof(EnchantEntry));

                pEnchant->Id = enchant_id;
                if(enchslot != 2)
                    AddEnchantment(pEnchant,0,true, false);
                else
                    AddEnchantment(pEnchant,0,false,false);
            }
        }
    }    

    ApplyRandomProperties();
}

void Item::ApplyRandomProperties()
{
    // apply random properties
    if(m_uint32Values[ITEM_FIELD_RANDOM_PROPERTIES_ID] != 0)
    {
        RandomProps *rp= sRandomPropStore.LookupEntry(m_uint32Values[ITEM_FIELD_RANDOM_PROPERTIES_ID]);
        if(rp)
        {
            for (int k=0;k<3;k++)
            {
                if (rp->spells[k] != 0)
                {
                    EnchantEntry * ee = sEnchantStore.LookupEntry(rp->spells[k]);
                    if(HasEnchantment(ee->Id) < 0) AddEnchantment(ee, 0, false, false, true);
                }
            }
        }
    }
}

void Item::SaveToDB(int8 containerslot, int8 slot, bool firstsave)
{
    std::stringstream ss;

    if(firstsave || _dbguid == 0)
        ss << "INSERT INTO playeritems VALUES(";
    else
        ss << "REPLACE INTO playeritems VALUES(";

    _dbguid = m_uint32Values[OBJECT_FIELD_ENTRY];

    ss << m_uint32Values[ITEM_FIELD_OWNER] << ",";
    ss << m_uint32Values[OBJECT_FIELD_GUID] << ",";
    ss << m_uint32Values[OBJECT_FIELD_ENTRY] << ",";
    ss << GetUInt32Value(ITEM_FIELD_CREATOR) << ",";
    ss << GetUInt32Value(ITEM_FIELD_STACK_COUNT) << ",";
    ss << GetChargesLeft() << ",";
    ss << GetUInt32Value(ITEM_FIELD_FLAGS) << ",";
    ss << GetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID) << ",";
    ss << GetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID) << ",";
    ss << GetUInt32Value(ITEM_FIELD_DURABILITY) << ",";
    ss << static_cast<int>(containerslot) << ",";
    ss << static_cast<int>(slot) << ",'";

    // Pack together enchantment fields
    EnchantmentMap::iterator itr = Enchantments.begin();
    for(; itr != Enchantments.end(); ++itr)
    {
        uint32 elapsed_duration = time(NULL) - itr->second.ApplyTime;
        int32 remaining_duration = itr->second.Duration - elapsed_duration;
        if(remaining_duration < 0) remaining_duration = 0;
        if(!itr->second.RemoveAtLogout && 
            (remaining_duration > 5 && itr->second.Slot != 2) || itr->second.Slot == 2)  // no point saving stuff with < 5 seconds... unless is perm enchant
        {
            ss << itr->second.Enchantment->Id << ",";
            ss << remaining_duration << ",";
            ss << itr->second.Slot << ";";
        }
    }
    ss << "')";
    
    if(firstsave)
        sDatabase.WaitExecute(ss.str().c_str());
    else
        sDatabase.Execute( ss.str().c_str());
}




void Item::DeleteFromDB()
{
    sDatabase.WaitExecute("DELETE FROM playeritems WHERE guid = %u", GetGUIDLow());
}

uint32 GetSkillByProto(ItemPrototype *proto)
{
    const static uint32 arm_skills[7]={0,SKILL_CLOTH,SKILL_LEATHER,SKILL_MAIL,SKILL_PLATE_MAIL,0,SKILL_SHIELD};
   
    const static uint32 weap_skills[21]={SKILL_AXES,SKILL_2H_AXES,SKILL_BOWS,SKILL_GUNS,SKILL_MACES,SKILL_2H_MACES,
                SKILL_POLEARMS,SKILL_SWORDS,SKILL_2H_SWORDS,0,SKILL_STAVES,0,0,SKILL_FIST_WEAPONS,0,//<-13
                SKILL_DAGGERS,SKILL_THROWN,SKILL_SPEARS,SKILL_CROSSBOWS,SKILL_WANDS,SKILL_FISHING};
    //14--used by professions
    if(proto->Class == 4 && proto->SubClass < 7)
    {
        return arm_skills[proto->SubClass];

    } else if(proto->Class == 2 && proto->SubClass < 21)
    {
        return weap_skills[proto->SubClass];
    }
    return 0;
}

//This map is used for profess.
//Prof packe strcut: {SMSG_SET_PROFICIENCY,(uint8)item_class,(uint32)1<<item_subclass}
//ie: for fishing (it's class=2--weapon, subclass ==20 -- fishing rod) permissive packet
// will have structure 0x2,524288
//this table is needed to get class/subclass by skill, valid classes are 2 and 4

const ItemProf * GetProficiencyBySkill(uint32 skill)
{
    const static ItemProf prof[22]={
        {4,2},{4,4},{4,8},{4,16},{4,64},
        {2,1},{2,2},{2,4},{2,8},{2,16},{2,32},{2,64},{2,128},{2,256},{2,1024},{2,8192},{2,32768},{2,65536},{2,131072},
        {2,262144},{2,524288},{2,1048576}};

    switch(skill)
    {
        case SKILL_CLOTH:
            return &prof[0];
        case SKILL_LEATHER:
            return &prof[1];
        case SKILL_MAIL:
            return &prof[2];
        case SKILL_PLATE_MAIL:
            return &prof[3];
        case SKILL_SHIELD:
            return &prof[4];

        case SKILL_AXES:
            return &prof[5];
        case SKILL_2H_AXES:
            return &prof[6];
        case SKILL_BOWS:
            return &prof[7];
        case SKILL_GUNS:
            return &prof[8];
        case SKILL_MACES:
            return &prof[9];
        case SKILL_2H_MACES:
            return &prof[10];
        case SKILL_POLEARMS:
            return &prof[11];
        case SKILL_SWORDS:
            return &prof[12];
        case SKILL_2H_SWORDS:
            return &prof[13];
        case SKILL_STAVES:
            return &prof[14];
        case SKILL_FIST_WEAPONS:
            return &prof[15];
        case SKILL_DAGGERS:
            return &prof[16];
        case SKILL_THROWN:
            return &prof[17];
        case SKILL_SPEARS:
            return &prof[18];
        case SKILL_CROSSBOWS:
            return &prof[19];
        case SKILL_WANDS:
            return &prof[20];
        case SKILL_FISHING:
            return &prof[21];
        default:
            return NULL;
    }
}

uint32 GetSellPriceForItem(ItemPrototype *proto, uint32 count)
{
    int32 cost;

    switch(proto->Class)
    {
    case ITEM_CLASS_PROJECTILE: // as far as i can tell these can't be sold at all
        {
            cost = ( proto->SellPrice * ((count < 1) ? 1 : count) ) / proto->MaxCount;
        }break;
    case ITEM_CLASS_CONSUMABLE: // same as default, but leaving it like that in case
                                // someone can correct me     - Doron
        {
            cost = ( proto->SellPrice * ((count < 1) ? 1 : count) );
        }break;
    default:
        {
            cost = ( proto->SellPrice * ((count < 1) ? 1 : count) );
        }break;
    }

    return cost;
}

uint32 GetBuyPriceForItem(ItemPrototype *proto, uint32 count, uint32 vendorcount)
{
    int32 cost;

    switch(proto->Class)
    {
    case ITEM_CLASS_PROJECTILE:
    case ITEM_CLASS_CONSUMABLE:
        {
            cost = ( proto->BuyPrice * ((count < 1) ? 1 : count) ) / ((vendorcount < 1) ? proto->MaxCount : vendorcount);
        }break;
    default:
        {
            cost = ( proto->BuyPrice * ((count < 1) ? 1 : count));
        }break;
    }

    return cost;
}

uint32 GetSellPriceForItem(uint32 itemid, uint32 count)
{
    if(ItemPrototype *proto = objmgr.GetItemPrototype(itemid))
        return GetSellPriceForItem(proto, count);
    else
        return 1;
}

uint32 GetBuyPriceForItem(uint32 itemid, uint32 count, uint32 vendorcount)
{
    if(ItemPrototype *proto = objmgr.GetItemPrototype(itemid))
        return GetBuyPriceForItem(proto, count, vendorcount);
    else
        return 1;
}

void Item::AddToWorld()
{
    // we may need to do some special shit here
    if(m_inWorld)
    {
        printf("Warning: Item was tried to be added to world when this had already been done.\n");
        return;
    }

    Object::AddToWorld();
}

void Item::RemoveFromWorld()
{
    // if we have an owner->send destroy
    if(m_owner)
    {
        DestroyForPlayer(m_owner);
    }

    if(!m_inWorld)
    {
        printf("Warning: Item was tried to be removed from world when this had already been done.\n");
        return;
    }

    Object::RemoveFromWorld();        
}

void Item::SetOwner(Player *owner)
{ 
    if(owner)
        SetUInt64Value(ITEM_FIELD_OWNER,((Object*)owner)->GetGUID());
    else SetUInt64Value(ITEM_FIELD_OWNER,0);

    m_owner = owner; 
}

int32 Item::AddEnchantment(EnchantEntry * Enchantment, uint32 Duration, bool Perm /* = false */, bool apply /* = true */, bool RemoveAtLogout /* = false */)
{
    int32 Slot;
    
       if(Perm)
       {
           // put slot 2 as permanent
        Slot = 2;
        RemoveEnchantment(2);
    }
    else
    {
        if(Enchantment->EnchantGroups > 1) // replaceable temp enchants
        {
            Slot = 1;
            RemoveEnchantment(1);
        }
        else
        {
            Slot = FindFreeEnchantSlot(Enchantment);
            // reach max of temp enchants
            if(Slot < 0) return Slot;
        }
    }   
    

    // Create the enchantment struct.
    EnchantmentInstance Instance;
    Instance.ApplyTime = (Enchantment->type ? time(NULL) : 0);
    Instance.BonusApplied = false;
    Instance.Slot = Slot;
    Instance.Enchantment = Enchantment;
    Instance.AddAmount = 0;
    Instance.Duration = Duration;
    Instance.RemoveAtLogout = RemoveAtLogout;

    // Set the enchantment in the item fields.
    uint32 EnchantBase = Slot * 3 + ITEM_FIELD_ENCHANTMENT;
    SetUInt32Value(EnchantBase + 0, Enchantment->Id);
    SetUInt32Value(EnchantBase + 1, Instance.ApplyTime);
    SetUInt32Value(EnchantBase + 2, 0); // charges

    // Add it to our map.
    Enchantments[Slot] = Instance;

    if(m_owner == 0) return Slot;

    // Add the removal event.
    if(Duration)
    {
        sEventMgr.AddEvent(this, &Item::RemoveEnchantment, uint32(Slot),
            EVENT_REMOVE_ENCHANTMENT1 + Slot, Duration * 1000, 1);
    }

    // No need to send the log packet, if the owner isn't in world (we're still loading)
    if(!m_owner->IsInWorld())
        return Slot;

    /*
    {SERVER} Packet: (0x01D7) SMSG_ENCHANTMENTLOG Size = 25
    |------------------------------------------------|----------------|
    |00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
    |------------------------------------------------|----------------|
    |51 46 35 00 00 00 00 00 51 46 35 00 00 00 00 00 |QF5.....QF5.....|
    |9F 4A 00 00 80 06 00 00 00                      |.J.......       |
    -------------------------------------------------------------------
    
    uint64 SourceGuid
    uint64 TargetGuid
    uint32 ItemEntryId
    uint32 EnchantmentId
    uint8  Unk

    */

    if(apply)
    {
        WorldPacket EnchantLog(SMSG_ENCHANTMENTLOG, 25);
        EnchantLog << m_owner->GetGUID();
        EnchantLog << m_owner->GetGUID();
        EnchantLog << m_uint32Values[OBJECT_FIELD_ENTRY];
        EnchantLog << Enchantment->Id;
        EnchantLog << uint8(0);
        m_owner->GetSession()->SendPacket(&EnchantLog);
    
        ApplyEnchantmentBonus(Slot, APPLY);
    }

    return Slot;
}

void Item::RemoveEnchantment(uint32 EnchantmentSlot)
{
    // Make sure we actually exist.
    EnchantmentMap::iterator itr = Enchantments.find(EnchantmentSlot);
    if(itr == Enchantments.end())
        return;

    uint32 Slot = itr->first;
    if(itr->second.BonusApplied)
        ApplyEnchantmentBonus(EnchantmentSlot, REMOVE);

    // Unset the item fields.
    uint32 EnchantBase = Slot * 3 + ITEM_FIELD_ENCHANTMENT;
    SetUInt32Value(EnchantBase + 0, 0);
    SetUInt32Value(EnchantBase + 1, 0);
    SetUInt32Value(EnchantBase + 2, 0);

    // Remove the enchantment event for removal.
    event_RemoveEvents(EVENT_REMOVE_ENCHANTMENT1 + Slot);

    // Remove the enchantment instance.
    Enchantments.erase(itr);
}

void Item::ApplyEnchantmentBonus(uint32 Slot, bool Apply)
{
    EnchantmentMap::iterator itr = Enchantments.find(Slot);
    if(itr == Enchantments.end())
        return;

    EnchantEntry * Entry = itr->second.Enchantment;

    if(itr->second.BonusApplied == Apply)
        return;

    itr->second.BonusApplied = Apply;

    if(Apply)
    {
        // Send the enchantment time update packet.
        SendEnchantTimeUpdate(itr->second.Slot, itr->second.Duration);
    }

    // Apply the visual on the player.
    uint32 ItemSlot = m_owner->GetItemInterface()->GetInventorySlotByGuid(GetGUID()) * 16;
    uint32 VisibleBase = PLAYER_VISIBLE_ITEM_1_0 + ItemSlot;
    m_owner->SetUInt32Value(VisibleBase + 1 + Slot, Apply ? Entry->Id : 0);

    // Depending on the enchantment type, take the appropriate course of action.
    switch(Entry->type)
    {
    case 1:         // Trigger spell on melee attack.
        {
            if(Apply)
            {
                // Create a proc trigger spell

                ProcTriggerSpell TS;
                TS.caster = m_owner->GetGUID();
                TS.origId = 0;
                TS.procFlags = PROC_ON_MELEE_ATTACK;
                TS.procCharges = 0;
                TS.procChance = Entry->min ? Entry->min : 101;

                for(uint32 Index = 0; Index < 2; ++Index)
                {
                    if(Entry->spell[Index] != 0)
                    {
                        TS.spellId = Entry->spell[Index];
                        m_owner->m_procSpells.push_back(TS);
                    }
                }
            }
            else
            {
                // Remove the proctriggerspell
                uint32 SpellId;
                list<struct ProcTriggerSpell>::iterator itr, itr2;
                for(itr = m_owner->m_procSpells.begin();
                    itr != m_owner->m_procSpells.end();)
                {
                    SpellId = itr->spellId;
                    itr2 = itr++;
                    
                    if(SpellId == Entry->spell[0] ||
                        SpellId == Entry->spell[1])
                    {
                        m_owner->m_procSpells.erase(itr2);
                    }
                }
            }
        }break;

    case 2:         // Mod damage done.
        {
            int32 AddAmount;
            if(Apply)
            {
                AddAmount = Entry->min;
                if(Entry->max > AddAmount)
                {
                    uint32 Range = Entry->max - Entry->min;
                    AddAmount += sRand.randInt(Range);
                }
                itr->second.AddAmount = AddAmount;
            }
            else
            {
                AddAmount = -itr->second.AddAmount;
            }

            m_owner->ModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS, AddAmount);
            m_owner->CalcDamage();
        }break;

    case 3:         // Cast spell (usually means apply aura)
        {
            if(Apply)
            {
                SpellCastTargets targets(m_owner->GetGUID());
                SpellEntry * sp;
                Spell * spell;

                for(uint32 Index = 0; Index < 2; ++Index)
                {
                    if(Entry->spell[Index] != 0)
                    {
                        sp = sSpellStore.LookupEntry(Entry->spell[Index]);
                        if(!sp) continue;

                        spell = new Spell(m_owner, sp, true, 0);
                        spell->prepare(&targets);
                    }
                }
            }
            else
            {
                for(uint32 Index = 0; Index < 2; ++Index)
                {
                    if(Entry->spell[Index] != 0)
                    {
                        m_owner->RemoveAura(Entry->spell[Index]);
                    }
                }
            }

        }break;

    case 4:         // Modify physical resistance
        {
            int32 AddAmount;
            if(Apply)
            {
                AddAmount = Entry->min;
                if(Entry->max > AddAmount)
                {
                    uint32 Range = Entry->max - Entry->min;
                    AddAmount += sRand.randInt(Range);
                }
                itr->second.AddAmount = AddAmount;
            }
            else
            {
                AddAmount = -itr->second.AddAmount;
            }

            m_owner->FlatResistanceModifierPos[0] += AddAmount;
            m_owner->CalcResistance(0);
        }break;

    case 5:     // Gems (Socketing)
        {
            // todo
        }break;

    case 6:     // Rockbiter weapon (increase damage per second... how the hell do you calc that)
        {
            int32 AddAmount;
            if(Apply)
            {
                AddAmount = Entry->min;
                if(Entry->max > AddAmount)
                {
                    uint32 Range = Entry->max - Entry->min;
                    AddAmount += sRand.randInt(Range);
                }
                itr->second.AddAmount = AddAmount;
            }
            else
            {
                AddAmount = -itr->second.AddAmount;
            }

            m_owner->ModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS, AddAmount);
            m_owner->CalcDamage();
        }break;

    default:
        {
            sLog.outError("Unknown enchantment type: %u (%u)", Entry->type, Entry->Id);
        }break;
    }
}

void Item::ApplyEnchantmentBonuses()
{
    EnchantmentMap::iterator itr;
    for(itr = Enchantments.begin(); itr != Enchantments.end(); ++itr)
    {
        ApplyEnchantmentBonus(itr->first, APPLY);
    }
}

void Item::RemoveEnchantmentBonuses()
{
    EnchantmentMap::iterator itr;
    for(itr = Enchantments.begin(); itr != Enchantments.end(); ++itr)
    {
        ApplyEnchantmentBonus(itr->first, REMOVE);
    }
}

void Item::EventRemoveEnchantment(uint32 Slot)
{
    // Remove the enchantment.
    RemoveEnchantment(Slot);
}

int32 Item::FindFreeEnchantSlot(EnchantEntry * Enchantment)
{    
    if(!Enchantment) return -1;

    uint32 Slot = Enchantment->type ? 3 : 0;
    for(uint32 Index = ITEM_FIELD_ENCHANTMENT_09; Index < ITEM_FIELD_ENCHANTMENT_32; Index += 3)
    {
        if(m_uint32Values[Index] == 0) return Slot;    
        ++Slot;
    }

    return -1;
}

int32 Item::HasEnchantment(uint32 Id)
{
    uint32 Slot = 0;
    for(uint32 Index = ITEM_FIELD_ENCHANTMENT; Index < ITEM_FIELD_ENCHANTMENT_32; Index += 3)
    {
        if(m_uint32Values[Index] == Id) return Slot;
        ++Slot;
    }

    return -1;
}

void Item::ModifyEnchantmentTime(uint32 Slot, uint32 Duration)
{
    EnchantmentMap::iterator itr = Enchantments.find(Slot);
    if(itr == Enchantments.end())
        return;

    // Reset the apply time.
    itr->second.ApplyTime = time(NULL);
    itr->second.Duration = Duration;

    // Change the event timer.
    event_ModifyTimeAndTimeLeft(EVENT_REMOVE_ENCHANTMENT1 + Slot, Duration * 1000);

    // Send update packet
    SendEnchantTimeUpdate(itr->second.Slot, Duration);
}

void Item::SendEnchantTimeUpdate(uint32 Slot, uint32 Duration)
{
    /*
    {SERVER} Packet: (0x01EB) SMSG_ITEM_ENCHANT_TIME_UPDATE Size = 24
    |------------------------------------------------|----------------|
    |00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |0123456789ABCDEF|
    |------------------------------------------------|----------------|
    |69 32 F0 35 00 00 00 40 01 00 00 00 08 07 00 00 |i2.5...@........|
    |51 46 35 00 00 00 00 00                         |QF5.....        |
    -------------------------------------------------------------------

    uint64 item_guid
    uint32 count?
    uint32 time_in_seconds
    uint64 player_guid
    */

    WorldPacket *data = new WorldPacket(SMSG_ITEM_ENCHANT_TIME_UPDATE, 24);
    *data << GetGUID();
    *data << Slot;
    *data << Duration;
    *data << m_owner->GetGUID();
    m_owner->delayedPackets.add(data);
}

void Item::RemoveAllEnchantments(bool OnlyTemporary)
{
    EnchantmentMap::iterator itr, it2;
    for(itr = Enchantments.begin(); itr != Enchantments.end();)
    {
        it2 = itr++;

        if(OnlyTemporary && it2->second.Duration == 0) 
            continue;
            
        RemoveEnchantment(it2->first);
    }
}

void Item::RemoveRelatedEnchants(EnchantEntry * newEnchant)
{
    EnchantmentMap::iterator itr,itr2;
    for(itr = Enchantments.begin(); itr != Enchantments.end();)
    {
        itr2 = itr++;
        
        if(itr2->second.Enchantment->Id == newEnchant->Id || 
          (itr2->second.Enchantment->EnchantGroups > 1 && newEnchant->EnchantGroups > 1 ))
        { 
            RemoveEnchantment(itr2->first);
        }
    }
}
