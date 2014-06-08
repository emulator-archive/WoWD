#include "StdAfx.h"

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

// =====================================================================
// Pet.cpp - Handles all pet-related functions inside of Unit class.
// =====================================================================
#define GROWL_RANK_1 2649
#define GROWL_RANK_2 14916

void Pet::CreateAsSummon(uint32 entry, CreatureInfo *ci, Creature* created_from_creature, Unit *owner, SpellEntry* created_by_spell, uint32 type, uint32 expiretime)
{
    SetIsPet(true);

    std::string myname = sWorld.GenerateName();

    if(!ci) return;
    m_Owner = static_cast<Player*>(owner);
    m_OwnerGuid = m_Owner->GetGUID();

    // Create ourself    
    Create(myname.c_str(), owner->GetMapId(), owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ(), owner->GetOrientation());
    creature_info = ci;
    SetUInt32Value(OBJECT_FIELD_ENTRY, entry);
    SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);   // better set this one

    // Fields common to both lock summons and pets
    if(type & 0x2 && created_from_creature != NULL)
        SetUInt32Value(UNIT_FIELD_LEVEL, created_from_creature->getLevel());
    else
        SetUInt32Value(UNIT_FIELD_LEVEL,owner->GetUInt32Value(UNIT_FIELD_LEVEL));

    SetUInt32Value(UNIT_FIELD_DISPLAYID,  ci->DisplayID);
    SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, ci->DisplayID);
    SetUInt64Value(UNIT_FIELD_SUMMONEDBY, owner->GetGUID());
    SetUInt64Value(UNIT_FIELD_CREATEDBY, owner->GetGUID());
    
    if(type & 0x1 && created_by_spell != NULL)
        SetUInt64Value(UNIT_CREATED_BY_SPELL, created_by_spell->Id);

    BaseStats[0] = uint32(20+getLevel()*1.55);
    BaseStats[1] = uint32(20+getLevel()*0.64);
    BaseStats[2] = uint32(20+getLevel()*1.27);
    BaseStats[3] = uint32(20+getLevel()*0.18);
    BaseStats[4] = uint32(20+getLevel()*0.36);

    if(type & 0x1 || created_from_creature == NULL)
    {
        Summon = true;
        float scale = 1.0f;

        switch(GetEntry())
        {
        case 416: //Imp
            {
                scale = 0.4f;
            }break;
        case 417: //Felhunter
            {
                scale = 0.7f;
            }break;
        case 1860: //VoidWalker
            {
                scale = 0.75f;
            }break;
        case 1863: //Succubus
            {
                scale = 1.0f;
            }break;
        }
        SetFloatValue(OBJECT_FIELD_SCALE_X, scale);

        uint32 hp=uint32(28 + 10 * getLevel());
        SetUInt32Value(UNIT_FIELD_MAXHEALTH, hp);
        SetUInt32Value(UNIT_FIELD_BASE_HEALTH,hp);
        uint32 mp=uint32(28 + 10 * getLevel());
        SetUInt32Value(UNIT_FIELD_MAXPOWER1,mp );
        SetUInt32Value(UNIT_FIELD_BASE_MANA,mp);
        
        SetFloatValue(OBJECT_FIELD_SCALE_X, scale);
        SetUInt32Value(UNIT_FIELD_BYTES_0, 2048 | (0 << 24));
        SetUInt32Value(UNIT_FIELD_FLAGS, 8);
        SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 2000);
        SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_01, 2000);
        SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.5f);
        SetFloatValue(UNIT_FIELD_COMBATREACH, 0.75f);
        SetFloatValue(UNIT_FIELD_MINDAMAGE,float(getLevel() * 1.88));
        SetFloatValue(UNIT_FIELD_MAXDAMAGE,float(getLevel() * 3));
        SetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, float(GetFloatValue(UNIT_FIELD_MINDAMAGE)*0.5));
        SetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, float(GetFloatValue(UNIT_FIELD_MAXDAMAGE)*0.5));

        SetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE,float((getLevel() * 1.88)/2));
        SetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE,float((getLevel() * 3)/2));
        
        SetUInt32Value(UNIT_FIELD_ATTACK_POWER, uint32(getLevel() * 1.7));
        SetUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER, uint32(getLevel() * 2.4));
        
        for(int x=0;x<5;x++)
            SetUInt32Value(UNIT_FIELD_STAT0 + x, BaseStats[x]);

        SetUInt32Value(UNIT_FIELD_RESISTANCES, 180);
        SetUInt32Value(UNIT_FIELD_BYTES_2, 0x01);

        SetUInt32Value(UNIT_FIELD_MAXPOWER1, uint32(28 + 10 * getLevel()));
        SetUInt32Value(UNIT_FIELD_PETNUMBER, GetGUIDLow());
    } else {
        uint32 hp=created_from_creature->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
        SetUInt32Value(UNIT_FIELD_MAXHEALTH, hp);
        SetUInt32Value(UNIT_FIELD_BASE_HEALTH,hp);

        SetFloatValue(OBJECT_FIELD_SCALE_X, created_from_creature->GetFloatValue(OBJECT_FIELD_SCALE_X));
        SetUInt32Value(UNIT_FIELD_BYTES_0, created_from_creature->GetUInt32Value(UNIT_FIELD_BYTES_0));
        SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, created_from_creature->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
        SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_01, created_from_creature->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME_01));
        SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, created_from_creature->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS));
        SetFloatValue(UNIT_FIELD_COMBATREACH, created_from_creature->GetFloatValue(UNIT_FIELD_COMBATREACH));
        SetFloatValue(UNIT_FIELD_MINDAMAGE, created_from_creature->GetFloatValue(UNIT_FIELD_MINDAMAGE));
        SetFloatValue(UNIT_FIELD_MAXDAMAGE, created_from_creature->GetFloatValue(UNIT_FIELD_MAXDAMAGE));
        SetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE, created_from_creature->GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE));
        SetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE, created_from_creature->GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE));
        SetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE, created_from_creature->GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE));
        SetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE, created_from_creature->GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE));
        SetUInt32Value(UNIT_FIELD_ATTACK_POWER, created_from_creature->GetUInt32Value(UNIT_FIELD_ATTACK_POWER));
        SetUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER, created_from_creature->GetUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER));;

        SetUInt32Value(UNIT_FIELD_MAXPOWER1, 0);
        SetUInt32Value(UNIT_FIELD_BASE_MANA, 0);

        SetUInt32Value(UNIT_FIELD_MAXPOWER2, 0);
        SetUInt32Value(UNIT_FIELD_MAXPOWER3, 200);
        SetUInt32Value(UNIT_FIELD_MAXPOWER4, 0);

        SetUInt32Value(UNIT_FIELD_RESISTANCES, created_from_creature->GetUInt32Value(UNIT_FIELD_RESISTANCES));
        
        // These need to be checked.
        SetUInt32Value(UNIT_FIELD_FLAGS, 0x00000828);
        SetUInt32Value(UNIT_FIELD_POWER5, 600000);
        SetUInt32Value(UNIT_FIELD_MAXPOWER5, 1000000);
        SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
        SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, GetNextLevelXP(getLevel()));

        // Change the power type to FOCUS
        SetPowerType(POWER_TYPE_FOCUS);
        
        for(int x=0;x<5;x++)
            SetUInt32Value(UNIT_FIELD_STAT0 + x, BaseStats[x]);

        // create our spellz
        SetDefaultSpells();
    }

    BaseDamage[0]=GetFloatValue(UNIT_FIELD_MINDAMAGE);
    BaseDamage[1]=GetFloatValue(UNIT_FIELD_MAXDAMAGE);
    BaseOffhandDamage[0]=GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE);
    BaseOffhandDamage[1]=GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE);
    BaseRangedDamage[0]=GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE);
    BaseRangedDamage[1]=GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE);

    SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, owner->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));

    // Set all values to full.
    SetUInt32Value(UNIT_FIELD_HEALTH, GetUInt32Value(UNIT_FIELD_MAXHEALTH));
    SetUInt32Value(UNIT_FIELD_POWER1, GetUInt32Value(UNIT_FIELD_MAXPOWER1));
    SetUInt32Value(UNIT_FIELD_POWER2, GetUInt32Value(UNIT_FIELD_MAXPOWER2));
    SetUInt32Value(UNIT_FIELD_POWER3, GetUInt32Value(UNIT_FIELD_MAXPOWER3));
    SetUInt32Value(UNIT_FIELD_POWER4, GetUInt32Value(UNIT_FIELD_MAXPOWER4));

    // Set base values
    SetUInt32Value(UNIT_FIELD_BASE_HEALTH, GetUInt32Value(UNIT_FIELD_HEALTH));
    SetUInt32Value(UNIT_FIELD_BASE_MANA, GetUInt32Value(UNIT_FIELD_POWER1));
    memcpy(m_baseUint32Values, m_uint32Values,m_valuesCount*sizeof(uint32));
    
    m_PartySpellsUpdateTimer = 0;

    m_PetNumber = static_cast<Player*>(owner)->GeneratePetNumber();
    SetUInt32Value(UNIT_FIELD_PETNUMBER, m_GuidLow);

    m_ExpireTime = expiretime;
    bExpires = m_ExpireTime > 0 ? true : false;

    if(!bExpires)
    {
        // Create PlayerPet struct (Rest done by UpdatePetInfo)
        PlayerPet *pp = new PlayerPet;
        pp->number = m_PetNumber;
        static_cast<Player*>(owner)->AddPlayerPet(pp, pp->number);
    }    

    InitializeMe(true);
}


Pet::Pet(uint32 high, uint32 low) : Creature(high, low)
{
    m_PetXP = 0;
    Summon = false;
    memset(ActionBar, 0, sizeof(uint32)*10);

    m_AutoCombatSpell = 0;

    m_PartySpellsUpdateTimer = 0;
    m_LoyaltyTimer = PET_LOYALTY_UPDATE_TIMER;
    m_PetNumber = 0;

    m_State = PET_STATE_DEFENSIVE;
    m_Action = PET_ACTION_FOLLOW;
    bHasLoyalty = false;
    m_ExpireTime = 0;
    bExpires = false;
    m_Diet = 0;
    m_Action = PET_ACTION_FOLLOW;
    m_State = PET_STATE_DEFENSIVE;
}

Pet::~Pet()
{
    for(std::map<uint32, AI_Spell*>::iterator itr = m_AISpellStore.begin(); itr != m_AISpellStore.end(); ++itr)
        delete itr->second;

    GetAIInterface()->SetDefaultSpell(NULL);

    sEventMgr.RemoveEvents(this);

    if(IsInWorld())
        static_cast<Pet*>(this)->Remove();
}

void Pet::Update(uint32 time)
{
    Creature::Update(time); // passthrough

    if(bHasLoyalty)
    {
        if(m_LoyaltyTimer == 0)
        {
            // Update loyalty.....
            uint32 val = GetUInt32Value(UNIT_FIELD_POWER5);
            if(PET_LOYALTY_UPDATE_VALUE > val)
                val = 0;
            else
                val -= PET_LOYALTY_UPDATE_VALUE;
            
            // Set the value
            SetUInt32Value(UNIT_FIELD_POWER5, val);

            if(val <= 0)
            {
                Dismiss();
                return;
            }

            // reset timer
            m_LoyaltyTimer = PET_LOYALTY_UPDATE_TIMER;
        } else {
            if(time > m_LoyaltyTimer)
                m_LoyaltyTimer = 0;
            else
                m_LoyaltyTimer -= time;
        }
    }

    if(bExpires)
    {
        if(m_ExpireTime == 0)
        {
            // remove
            Dismiss(false);
            return;
        } else {
            if(time > m_ExpireTime)
            {
                m_ExpireTime = 0;
            } else {
                m_ExpireTime -= time;
            }
        }
    }
}

void Pet::SendSpellsToOwner()
{
    WorldPacket * data = new WorldPacket(SMSG_PET_SPELLS, mSpells.size() * 4 + 20);
    *data << GetGUID();
    *data << uint32(0x00000000);//unk1
    *data << uint32(0x00000101);//unk2

    // Send the actionbar
    for(uint32 i = 0; i < 10; ++i)
    {
        if(ActionBar[i] & 0x4000000)        // Command
            *data << uint32(ActionBar[i]);
        else
            *data << uint16(ActionBar[i]) << GetSpellState(ActionBar[i]);
    }

    // Send the rest of the spells.
    *data << uint8(mSpells.size());
    for(PetSpellMap::iterator itr = mSpells.begin(); itr != mSpells.end(); ++itr)
    {
        *data << uint16(itr->first->Id) << uint16(itr->second);
    }

    //m_Owner->GetSession()->SendPacket(&data);
    //sEventMgr.AddEvent(m_Owner, &Player::SendDelayedPacket, data, true, EVENT_PLAYER_SEND_PACKET, 1500, 1);
    m_Owner->delayedPackets.add(data);
}

void Pet::SendNullSpellsToOwner()
{
    WorldPacket data;
    data.SetOpcode(SMSG_PET_SPELLS);
    data << uint64(0);
    m_Owner->GetSession()->SendPacket(&data);
}

void Pet::InitializeSpells()
{
    for(PetSpellMap::iterator itr = mSpells.begin(); itr != mSpells.end(); ++itr)
    {
        SpellEntry *info = itr->first;

        // Check that the spell isn't passive
        if(info->Attributes & 64)
        {
            // Cast on self..
            Spell * sp = new Spell(this, info, true, false);
            SpellCastTargets targets(this->GetGUID());
            sp->prepare(&targets);

            continue;
        }

        CreateAISpell(info);
    }

    SendSpellsToOwner();
}

void Pet::CreateAISpell(SpellEntry * info)
{
    // Create an AI_Spell
    AI_Spell *sp = new AI_Spell;
    sp->agent = AGENT_SPELL;
    sp->entryId = GetEntry();
    sp->floatMisc1 = 0;
    sp->maxrange = GetMaxRange(sSpellRange.LookupEntry(info->rangeIndex));
    sp->minrange = GetMinRange(sSpellRange.LookupEntry(info->rangeIndex));
    sp->Misc2 = 0;
    sp->procChance = 0;
    sp->procCount = 0;
    sp->procCounter = 0;
    sp->procEvent = 0;
    sp->spellCooldown = 0;
    sp->spellCooldownTimer = 0;
    sp->spellId = info->Id;
    sp->spellType = STYPE_DAMAGE;
    sp->spelltargetType = TTYPE_SINGLETARGET;
    if(info->Effect[0] == SPELL_EFFECT_APPLY_AURA)
        sp->agent = STYPE_BUFF;

    if(info->EffectImplicitTargetA[0] == 24)
    {
        float radius = ::GetRadius(sSpellRadius.LookupEntry(info->EffectRadiusIndex[0]));
        sp->maxrange = radius;
        sp->spelltargetType = TTYPE_SOURCE;
    }

    m_AISpellStore[info->Id] = sp;
}

void Pet::LoadFromDB(Player* owner, PlayerPet * pi)
{
    m_Owner = owner;
    m_OwnerGuid = m_Owner->GetGUID();
    mPi = pi;

    Create(pi->name.c_str(), owner->GetMapId(), owner->GetPositionX() + 2 , owner->GetPositionY() +2, owner->GetPositionZ(), owner->GetOrientation());

    LoadValues(mPi->fields.c_str());
    m_PetNumber = mPi->number;
    m_PetXP = mPi->xp;
    Summon = mPi->summon;
    SetPowerType(POWER_TYPE_FOCUS);
    SetIsPet(true);

    // Setup actionbar
    uint32 pos = 0;
    string::size_type sp = mPi->actionbar.find(",");
    string::size_type lp = 0;
    while(sp != string::npos)
    {
        uint32 spell = atol(mPi->actionbar.substr(lp, sp).c_str());
        ActionBar[pos] = spell;
        ++pos;

        lp = sp+1;
        sp = mPi->actionbar.find(",", lp);
    }

    SetIsPet(true);
    InitializeMe(false);
    bExpires = false;   
    if(pi->autocastspell)
    {
        AI_Spell * sp = GetAISpellForSpellId(pi->autocastspell);
        if(sp)
            m_aiInterface->SetDefaultSpell(sp);
    }
}

void Pet::InitializeMe(bool first)
{
    // set up ai and shit
    
    GetAIInterface()->Init(this,AITYPE_PET,MOVEMENTTYPE_NONE,m_Owner);
    GetAIInterface()->SetUnitToFollow(m_Owner);
    GetAIInterface()->SetFollowDistance(3.0f);

    SetCreatureName(objmgr.GetCreatureName(GetEntry()));
    m_Owner->SetSummon(this);
    m_Owner->SetUInt64Value(UNIT_FIELD_SUMMON, this->GetGUID());
    SetUInt32Value(UNIT_FIELD_PETNUMBER, GetGUIDLow());
    SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, (uint32)time(NULL));

    bHasLoyalty = m_Owner->getClass() == HUNTER ? true : false;

    // Load our spells
    if(Summon)
    {
        SetDefaultSpells();
    }
    else
    {
        // Pull from database... :/
        QueryResult * query = sDatabase.Query("SELECT * FROM playerpetspells WHERE ownerguid=%u and petnumber=%u",
            m_Owner->GetGUIDLow(), m_PetNumber);
        if(query)
        {
            do 
            {
                Field * f = query->Fetch();
                SpellEntry* spell = sSpellStore.LookupEntry(f[2].GetUInt32());
                uint16 flags = f[3].GetUInt16();
                mSpells.insert ( make_pair( spell, flags ) );
            } while(query->NextRow());
        }
        delete query;
    }

    SetPetDiet();
    _setFaction();
    m_State = 1;        // dont set agro on spawn
    
    if(first)
    {
        // Set up default actionbar
        SetDefaultActionbar();
    }
    
    InitializeSpells();    
    AddToWorld();       // Add into the world

    sEventMgr.AddEvent(this, &Pet::Update, (uint32)100, EVENT_PET_UPDATE, 100, 0);

    // set to active
    if(!bExpires)
        UpdatePetInfo(false);
}

void Pet::UpdatePetInfo(bool bSetToOffline)
{
    if(bExpires)
        return;    // don't update expiring pets

    PlayerPet *pi = m_Owner->GetPlayerPet(m_PetNumber);
    pi->active = !bSetToOffline;
    pi->entry = GetEntry();
    std::stringstream ss;
    for( uint32 index = 0; index < UNIT_END; index ++ )
        ss << GetUInt32Value(index) << " ";
    pi->fields = ss.str();
    pi->name = GetName();
    pi->number = m_PetNumber;
    pi->xp = m_PetXP;
    pi->level = GetUInt32Value(UNIT_FIELD_LEVEL);
    pi->loyalty = GetUInt32Value(UNIT_FIELD_POWER5);
    pi->loyaltyupdate = m_LoyaltyTimer;

    // save actionbar
    ss.rdbuf()->str("");
    for(uint32 i = 0; i < 10; ++i)
    {
        ss << ActionBar[i] << ",";
    }

    pi->actionbar = ss.str();
    pi->summon = Summon;
    AI_Spell * sp = m_aiInterface->GetDefaultSpell();
    if(sp)
    {
        if(sp->agent == AGENT_SPELL)
            pi->autocastspell = sp->spellId;
        else
            pi->autocastspell = 0;
    }
}

void Pet::Dismiss(bool bSafeDelete)
{
    // Delete any petspells for us.
    if(!Summon)
    {
        sDatabase.Execute("DELETE FROM playerpetspells WHERE ownerguid=%u AND petnumber=%u",
            m_Owner->GetGUIDLow(), m_PetNumber);
    }

    if(m_Owner)
        m_Owner->RemovePlayerPet(m_PetNumber);

    // find out playerpet entry, delete it
    Remove(bSafeDelete, false);
}

void Pet::Remove(bool bSafeDelete, bool bUpdate)
{
    if(m_Owner)
    {
        // remove association with player
        m_Owner->SetUInt64Value(UNIT_FIELD_SUMMON, 0);

        if(bUpdate && !bExpires) UpdatePetInfo(true);
        m_Owner->SetSummon(NULL);
        SendNullSpellsToOwner();
    }

    if(bSafeDelete)
        sEventMgr.AddEvent(this, &Pet::PetSafeDelete, EVENT_CREATURE_SAFE_DELETE, 1, 1);
    else
        PetSafeDelete();
}

void Pet::PetSafeDelete()
{
    if(this->IsInWorld())
    {
        // remove from world, and delete
        RemoveFromWorld();
    }

    sObjHolder.Delete<Pet>(this);
}

void Pet::DelayedRemove(bool bTime, bool bDeath)
{
    m_Owner = World::GetPlayer(m_OwnerGuid);
    if(bTime)
    {
        if(GetUInt32Value(UNIT_CREATED_BY_SPELL) > 0 || bDeath)
            Dismiss(true);  // remove us..
        else
            Remove(true, true);
    }
    else
        sEventMgr.AddEvent(this, &Pet::DelayedRemove, true, bDeath, EVENT_PET_DELAYED_REMOVE, PET_DELAYED_REMOVAL_TIME, 1);
}

void Pet::GiveXP(uint32 xp)
{
    if(getLevel() >= 60) return;
        
    int32 petxp = GetUInt32Value(UNIT_FIELD_PETEXPERIENCE) + xp;
    int32 petnextlevelxp = GetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP);
    
    if(petxp >= petnextlevelxp)
    {
        // levelup
        LevelUp(petxp - petnextlevelxp);
    } else {
        ModUInt32Value(UNIT_FIELD_PETEXPERIENCE, xp);
    }
}

uint32 Pet::GetNextLevelXP(uint32 currentlevel)
{
    uint32 level = currentlevel + 1;
    uint32 nextLvlXP = 0;
    if( level > 0 && level <= 30 )
    {
        nextLvlXP = ((int)((((double)(8 * level * ((level * 5) + 45)))/100)+0.5))*100;
    }
    else if( level == 31 )
    {
        nextLvlXP = ((int)((((double)(((8 * level) + 3) * ((level * 5) + 45)))/100)+0.5))*100;
    }
    else if( level == 32 )
    {
        nextLvlXP = ((int)((((double)(((8 * level) + 6) * ((level * 5) + 45)))/100)+0.5))*100;
    }
    else
    {
        nextLvlXP = ((int)((((double)(((8 * level) + ((level - 30) * 5)) * ((level * 5) + 45)))/100)+0.5))*100;
    }
    return nextLvlXP;
}

void Pet::LevelUp(int32 extraxp)
{
    ModUInt32Value(UNIT_FIELD_LEVEL, 1);    // increase actual level
    SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, extraxp);
    SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, GetNextLevelXP(getLevel()));

#define gainstat(field, gain) ModUInt32Value(field, gain); ModBaseUInt32Value(field, gain)
#define gainfstat(field, gain) ModFloatValue(field, gain); ModBaseFloatValue(field, gain)

    gainstat(UNIT_FIELD_STAT0, GAIN_STAT0);
    gainstat(UNIT_FIELD_STAT1, GAIN_STAT1);
    gainstat(UNIT_FIELD_STAT2, GAIN_STAT2);
    gainstat(UNIT_FIELD_STAT3, GAIN_STAT3);
    gainstat(UNIT_FIELD_STAT4, GAIN_STAT4);

    for(int x = 0; x < 5; ++x)
    {
        BaseStats[x] = GetUInt32Value(UNIT_FIELD_STAT0 + x);
        CalcStat(x);
    }

    gainfstat(UNIT_FIELD_MINDAMAGE, GAIN_MINDAMAGE);
    gainfstat(UNIT_FIELD_MAXDAMAGE, GAIN_MAXDAMAGE);

    gainstat(UNIT_FIELD_MAXHEALTH, GAIN_HEALTH);
    gainstat(UNIT_FIELD_MAXPOWER1, GAIN_MANA);
    gainstat(UNIT_FIELD_ATTACK_POWER, GAIN_ATTACKPOWER);
    gainstat(UNIT_FIELD_RANGED_ATTACK_POWER, GAIN_RANGEDATTACKPOWER);

    SetUInt32Value(UNIT_FIELD_HEALTH, GetUInt32Value(UNIT_FIELD_MAXHEALTH));
    SetUInt32Value(UNIT_FIELD_POWER1, GetUInt32Value(UNIT_FIELD_MAXPOWER1));
}

void Pet::SetDefaultSpells()
{
    if(Summon)
    {
        // this one's easy :p we just pull em from the owner.
        map<uint32, set<uint32> >::iterator it1;
        set<uint32>::iterator it2;
        it1 = m_Owner->SummonSpells.find(GetEntry());
        if(it1 != m_Owner->SummonSpells.end())
        {
            it2 = it1->second.begin();
            for(; it2 != it1->second.end(); ++it2)
            {
                AddSpell((*it2));
            }
        }
    }
    else
    {
        // add growl (rank 1 for <=10, rank 2 for > 10)
        /*if(getLevel() <= 10)
            AddSpell(GROWL_RANK_1);
        else
            AddSpell(GROWL_RANK_2);

        // pull from objectmgr
        set<SpellEntry*> * spells = objmgr.GetDefaultPetSpells(GetEntry());
        if(spells != 0)
        {
            for(set<SpellEntry*>::iterator itr = spells->begin(); itr != spells->end(); ++itr)
            {
                AddSpell((*itr));
            }
        }*/

        uint32 Line = GetCreatureName()->SpellDataID;
        if(Line)
        {
            CreatureSpellDataEntry * SpellData = CreatureSpellDataStore::getSingleton().LookupEntry(Line);
            if(SpellData)
            {
                for(uint32 i = 0; i < 3; ++i)
                {
                    if(SpellData->Spells[i] != 0)
                    {
                        // add it to the player, so they can teach it to other pets
                        if(!m_Owner->HasSpell(SpellData->Spells[i]))
                        {
                            m_Owner->addSpell(SpellData->Spells[i]);
                        }

                        AddSpell(SpellData->Spells[i]);
                    }
                }
            }
        }
    }
}

void Pet::AddSpell(SpellEntry * sp)
{
    // Cast on self if we're a passive spell
    if(sp->Attributes & 64)
    {
        Spell * spell = new Spell(this, sp, true, false);
        SpellCastTargets targets(this->GetGUID());
        spell->prepare(&targets);
        mSpells[sp] = 0x0100;
    }
    else
    {
        mSpells[sp] = DEFAULT_SPELL_STATE;
        // Create the AI_Spell
        CreateAISpell(sp);
    }
    
    if(IsInWorld())
        SendSpellsToOwner();
}

void Pet::SetSpellState(SpellEntry* sp, uint16 State)
{
    PetSpellMap::iterator itr = mSpells.find(sp);
    if(itr == mSpells.end())
        return;

    itr->second = State;
}

uint16 Pet::GetSpellState(SpellEntry* sp)
{
    PetSpellMap::iterator itr = mSpells.find(sp);
    if(itr == mSpells.end())
        return DEFAULT_SPELL_STATE;

    return itr->second;
}

void Pet::SetDefaultActionbar()
{
    // Set up the default actionbar.
    ActionBar[0] = PET_SPELL_ATTACK;
    ActionBar[1] = PET_SPELL_FOLLOW;
    ActionBar[2] = PET_SPELL_STAY;

    // Fill up 4 slots with our spells
    if(mSpells.size() > 0)
    {
        PetSpellMap::iterator itr = mSpells.begin();
        uint32 pos = 0;
        for(; itr != mSpells.end() && pos < 4; ++itr, ++pos)
            ActionBar[3+pos] = itr->first->Id;
    }

    ActionBar[7] = PET_SPELL_AGRESSIVE;
    ActionBar[8] = PET_SPELL_DEFENSIVE;
    ActionBar[9] = PET_SPELL_PASSIVE;
}

uint32 Pet::GetSpellCooldown(uint32 SpellId)
{
    map<uint32, uint32>::iterator itr = m_spellCooldown.find(SpellId);
    if(itr != m_spellCooldown.end())
    {
        uint32 mstime = getMSTime();
        uint32 cotime = itr->second;
        if(mstime > cotime)
        {
            // empty cooldown -> erase it
            m_spellCooldown.erase(itr);
            return 0;
        }
        else
            return cotime - mstime;
    }
    return 0;
}

void Pet::AddSpellCooldown(uint32 SpellId)
{
    uint32 Cooldown = objmgr.GetPetSpellCooldown(SpellId);
    if(Cooldown == 0)
    {
        // Try to pull a cooldown value from spelldbc.
        SpellEntry * sp = sSpellStore.LookupEntry(SpellId);
        if(sp)
        {
            Cooldown = sp->CategoryRecoveryTime > sp->RecoveryTime ? sp->CategoryRecoveryTime : sp->RecoveryTime;
        }

        // if still 0
        if(!Cooldown)
            Cooldown = 1000;    // hack..
    }

    m_spellCooldown[SpellId] = getMSTime() + Cooldown;
}

void Pet::RemoveSpell(SpellEntry * sp)
{
    mSpells.erase(sp);
    map<uint32, AI_Spell*>::iterator itr = m_AISpellStore.find(sp->Id);
    if(itr != m_AISpellStore.end())
    {
        delete itr->second;
        if(GetAIInterface()->GetDefaultSpell() == itr->second)
            GetAIInterface()->SetDefaultSpell(0);

        m_AISpellStore.erase(itr);
    }
}

void Pet::Rename(string NewName)
{
    m_name = NewName;
    // update petinfo
    UpdatePetInfo(false);

    // update timestamp to force a re-query
    SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, time(NULL));
}
