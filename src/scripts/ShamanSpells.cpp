//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////

// Warrior Dummy Spell Module

#include "StdAfx.h"

/* Script Export Declarations */

extern "C" WOWD_SCRIPT_DECL void ScriptInitialize(ScriptModule *mod);
extern "C" WOWD_SCRIPT_DECL bool HandleDummySpell(uint32 uSpellId, uint32 i, Spell *pSpell);

// Build version info function
BUILD_VERSIONINFO_DATA(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);

#ifdef WIN32

/* This is needed because windows is a piece of shit. ;) */
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    return TRUE;
}

#endif

/************************************************************************/
/* Spell Defs                                                           */
/************************************************************************/

void RockbiterWeapon(uint32 uSpellId, uint32 i, Spell* pSpell)
{
    uint32 enchantment_entry = 0;
    switch(pSpell->m_spellInfo->RankNumber)
    {
    case 1:
        enchantment_entry = 3021;
        break;
    case 2:
        enchantment_entry = 3024;
        break;
    case 3:
        enchantment_entry = 3027;
        break;
    case 4:
        enchantment_entry = 3030;
        break;
    case 5:
        enchantment_entry = 3033;
        break;
    case 6:
        enchantment_entry = 3036;
        break;
    case 7:
        enchantment_entry = 3039;
        break;
    case 8:
        enchantment_entry = 3042;
        break;
    }

    if(!enchantment_entry || !pSpell->p_caster)
        return;

    Item * item = pSpell->p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
    EnchantEntry * enchant = sEnchantStore.LookupEntry(enchantment_entry);
    if(!item || !enchant)
        return;

    int32 Slot = item->HasEnchantment(enchant->Id);
    if(Slot >= 0)
        item->ModifyEnchantmentTime(Slot, 1800);
    else
    {
        Slot = item->AddEnchantment(enchant, 1800, false, true);   // 5min
        if(Slot < 0) return;
        item->ApplyEnchantmentBonus(Slot, APPLY);
    }

    script_debuglog("ShamanSpells.cpp :: Rockbiter Weapon Rank %u, enchant %u, slot %u", pSpell->m_spellInfo->RankNumber,
        enchantment_entry, Slot);
}

/* Actual Spell Code */
bool HandleDummySpell(uint32 uSpellId, uint32 i, Spell *pSpell)
{
    switch(uSpellId)
    {
    case 8017:
    case 8018:
    case 8019:
    case 8020:
    case 8021:
    case 8022:
        RockbiterWeapon(uSpellId, i, pSpell);
        break;

    default:
        return false;
        break;
    }

    return true;
}

/* Module info */
void ScriptInitialize(ScriptModule *mod)
{
    /**** Rockbiter Weapon ****/
    sScriptMgr.ScriptAddSpellModule(8017, mod);     // Rank 1
    sScriptMgr.ScriptAddSpellModule(8018, mod);     // Rank 2
    sScriptMgr.ScriptAddSpellModule(8019, mod);     // Rank 3
    sScriptMgr.ScriptAddSpellModule(8020, mod);     // Rank 4
    sScriptMgr.ScriptAddSpellModule(8021, mod);     // Rank 5
    sScriptMgr.ScriptAddSpellModule(8022, mod);     // Rank 6
}