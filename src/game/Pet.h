// Copyright (C) 2004 WoW Daemon
// Copyright (C) DK
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

#ifndef _PET_H
#define _PET_H

/* Taken from ItemPetFood.dbc
 * Each value is equal to a flag
 * so 1 << PET_FOOD_BREAD for example
 * will result in a usable value.
 */
enum PET_FOOD
{
    PET_FOOD_NONE,
    PET_FOOD_MEAT,
    PET_FOOD_FISH,
    PET_FOOD_CHEESE,
    PET_FOOD_BREAD,
    PET_FOOD_FUNGUS,
    PET_FOOD_FRUIT,
    PET_FOOD_RAW_MEAT, // not used in pet diet
    PET_FOOD_RAW_FISH  // not used in pet diet
};

/* Pet Diet:
   taken from http://www.wowwiki.com/Pet_(Hunter)
   filled with "0" for unknown / non existing creature family's
   how to use: 
   value = PetDiet[number_creature_family];
*/
static const unsigned char PetDiet[30] = { 0,1,3,1,63,63,3,3,58,48,0,1,52,0,0,0,0,0,0,0,1,178,0,0,48,33,1,14,0};

enum PET_ACTION
{
    PET_ACTION_STAY,
    PET_ACTION_FOLLOW,
    PET_ACTION_ATTACK,
    PET_ACTION_DISMISS,
    PET_ACTION_CASTING,
};

enum PET_STATE
{
    PET_STATE_PASSIVE,
    PET_STATE_DEFENSIVE,
    PET_STATE_AGGRESSIVE
};

enum PET_SPELL
{
    PET_SPELL_PASSIVE = 0x06000000,
    PET_SPELL_DEFENSIVE,
    PET_SPELL_AGRESSIVE,
    PET_SPELL_STAY = 0x07000000,
    PET_SPELL_FOLLOW,
    PET_SPELL_ATTACK
};

enum StableState
{
    STABLE_STATE_ACTIVE = 1,
    STABLE_STATE_PASSIVE = 3
};
/* Levelling Info */

#define BASE_STAT0 22
#define BASE_STAT1 20
#define BASE_STAT2 22
#define BASE_STAT3 20
#define BASE_STAT4 20

#define BASE_POWER 24

#define BASE_HEALTH 28
#define BASE_MANA 28

#define GAIN_STAT0 4
#define GAIN_STAT1 1
#define GAIN_STAT2 3
#define GAIN_STAT3 1
#define GAIN_STAT4 1

#define GAIN_ATTACKPOWER 2
#define GAIN_RANGEDATTACKPOWER 2

#define GAIN_MINDAMAGE 2
#define GAIN_MAXDAMAGE 3

#define GAIN_MINRANGEDAMAGE (GAIN_MINDAMAGE/2)
#define GAIN_MAXRANGEDAMAGE (GAIN_MAXDAMAGE/2)

#define GAIN_HEALTH 10
#define GAIN_MANA 10

#define PET_DELAYED_REMOVAL_TIME 60000  // 1 min

#define DEFAULT_SPELL_STATE 0x8100

typedef map<SpellEntry*, uint16> PetSpellMap;
struct PlayerPet;

class WOWD_SERVER_DECL Pet : public Creature
{
    friend class Player;
    friend class Creature;
    friend class WorldSession;

public:
    Pet(uint32 high, uint32 low);
    ~Pet();

    void LoadFromDB(Player* owner, PlayerPet * pi);
    void CreateAsSummon(uint32 entry, CreatureInfo *ci, Creature *created_from_creature, Unit* owner, SpellEntry *created_by_spell, uint32 type, uint32 expiretime);

    virtual void Update(uint32 time);
    inline uint32 GetXP(void) { return m_PetXP; }

    void InitializeSpells();
    void ReInitializeSpells();
    void InitializeMe(bool first);
    void SendSpellsToOwner();
    void SendNullSpellsToOwner();

    inline void SetPetAction(uint32 act) { m_Action = act; }
    inline uint32 GetPetAction(void) { return m_Action; }

    inline void SetPetState(uint32 state) { m_State = state; }
    inline uint32 GetPetState(void) { return m_State; }

    inline void SetPetDiet(uint32 diet) { m_Diet = diet; }
    inline void SetPetDiet() { m_Diet = PetDiet[this->creature_info->Family]; }

    inline uint32 GetPetDiet(void) { return m_Diet; }
    
    inline AI_Spell* GetAISpellForSpellId(uint32 spellid)
    {
        std::map<uint32, AI_Spell*>::iterator itr = m_AISpellStore.find(spellid);
        if(itr != m_AISpellStore.end())
            return itr->second;
        else
            return NULL;
    }

    void UpdatePetInfo(bool bSetToOffline);
    void Remove(bool bSafeDelete = false, bool bUpdate = true);
    void Dismiss(bool bSafeDelete = false);

    void DelayedRemove(bool bTime, bool bDeath);

    inline Player* GetPetOwner() { return m_Owner; }
    void GiveXP(uint32 xp);
    void LevelUp(int32 extraxp);
    uint32 GetNextLevelXP(uint32 currentlevel);

    void PetSafeDelete();

    void SetDefaultSpells();
    void SetDefaultActionbar();

    void LoadSpells();
    void AddSpell(SpellEntry * sp);
    void RemoveSpell(SpellEntry * sp);
    void SetSpellState(SpellEntry * sp, uint16 State);
    uint16 GetSpellState(SpellEntry * sp);

    inline void AddSpell(uint32 SpellID)
    {
        SpellEntry * sp = sSpellStore.LookupEntry(SpellID);
        if(sp) AddSpell(sp);
    }
    inline void RemoveSpell(uint32 SpellID)
    {
        SpellEntry * sp = sSpellStore.LookupEntry(SpellID);
        if(sp) RemoveSpell(sp);
    }
    inline void SetSpellState(uint32 SpellID, uint16 State)
    {
        SpellEntry * sp = sSpellStore.LookupEntry(SpellID);
        if(sp) SetSpellState(sp, State);
    }
    inline uint16 GetSpellState(uint32 SpellID)
    {
        if(SpellID == 0)
            return DEFAULT_SPELL_STATE;

        SpellEntry * sp = sSpellStore.LookupEntry(SpellID);
        if(sp)
            return GetSpellState(sp);
        return DEFAULT_SPELL_STATE;
    }
    
    uint32 __fastcall GetSpellCooldown(uint32 SpellId);
    void __fastcall AddSpellCooldown(uint32 SpellId);

    void CreateAISpell(SpellEntry * info);
    inline PetSpellMap* GetSpells() { return &mSpells; }
    inline bool IsSummon() { return Summon; }

    void __fastcall SetAutoCastSpell(AI_Spell * sp);
    void Rename(string NewName);

protected:
    bool bHasLoyalty;
    Player *m_Owner;
    uint32 m_PetXP;
    PetSpellMap mSpells;
    PlayerPet * mPi;
    uint32 ActionBar[10];   // 10 slots
    
    std::map<uint32, AI_Spell*> m_AISpellStore;
    map<uint32, uint32> m_spellCooldown;

    uint32 m_AutoCombatSpell;

    uint32 m_PartySpellsUpdateTimer;
    uint32 m_LoyaltyTimer;
    uint32 m_PetNumber;

    uint32 m_Action;
    uint32 m_State;
    uint32 m_ExpireTime;
    uint32 m_Diet;
    uint64 m_OwnerGuid;
    bool bExpires;
    bool Summon;
};

#define PET_LOYALTY_UPDATE_TIMER 120000
#define PET_LOYALTY_UPDATE_VALUE 12000
#define PET_PARTY_SPELLS_UPDATE_TIMER 10000

#define PET_ACTION_ACTION   0x700
#define PET_ACTION_STATE    0x600

//TODO: burlex see the way pet spells contain the same flag?
#define PET_ACTION_SPELL    0xC100
#define PET_ACTION_SPELL_1  0x8100
#define PET_ACTION_SPELL_2  0x0100
#endif
