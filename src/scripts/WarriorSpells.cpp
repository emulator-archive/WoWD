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

void Charge(uint32 uSpellId, uint32 i, Spell* pSpell)
{
    uint32 rage_to_gen;
    switch(uSpellId)
    {
    case 100:   // Charge Rank 1
        rage_to_gen = 90;
        break;

    case 6178:  // Charge Rank 2
        rage_to_gen = 120;
        break;

    default:    // Charge Rank 3 +
        rage_to_gen = 150;
        break;
    }

    // Add the rage to the caster
    if(!pSpell->u_caster)
        return;

    uint32 val = pSpell->u_caster->GetUInt32Value(UNIT_FIELD_POWER2);
    uint32 max = pSpell->u_caster->GetUInt32Value(UNIT_FIELD_MAXPOWER2);
    val += rage_to_gen;
    if(val > max)
        val = max;
    
    pSpell->u_caster->SetUInt32Value(UNIT_FIELD_POWER2, val);
    script_debuglog("WarriorSpells.cpp :: Charge generate %u rage on "I64FMT, rage_to_gen, pSpell->u_caster->GetGUID());
}

void Execute(uint32 uSpellId, uint32 i, Spell* pSpell)
{
    uint32 base_dmg = pSpell->damage;
    /*
    Attempt to finish off a wounded foe, causing 125 damage and converting each extra point
    of rage into 3 additional damage.  Only usable on enemies that have less than 20% health.
    */

    Unit * target = pSpell->GetUnitTarget();
    if(!target || !pSpell->u_caster) return;

    // "Only usable on enemies that have less than 20% health."
    if(target->GetHealthPct() > 20)
    {
        // send failed
        pSpell->SendCastResult(SPELL_FAILED_BAD_TARGETS);
        return;
    }

    // get the caster's rage points, and convert them
    // formula is 3 damage * spell rank * rage points
    uint32 add_damage = (3 * pSpell->m_spellInfo->RankNumber);
    add_damage *= pSpell->u_caster->GetUInt32Value(UNIT_FIELD_POWER2) / 10;   // rage is *10 always
    
    // send spell damage log
    pSpell->u_caster->SpellNonMeleeDamageLog(target, uSpellId, base_dmg + add_damage, false);

    // zero rage
    pSpell->u_caster->SetUInt32Value(UNIT_FIELD_POWER2, 0);
}

/* Actual Spell Code */
bool HandleDummySpell(uint32 uSpellId, uint32 i, Spell *pSpell)
{
    switch(uSpellId)
    {
    case 100:
    case 6178:
    case 11578:     // Charge
        Charge(uSpellId, i, pSpell);
        break;

    case 5308:
    case 20660:
    case 20661:
    case 20662:
    case 25234:
    case 25236:     // Execute
        Execute(uSpellId, i, pSpell);
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
    /**** Charge ****/
    sScriptMgr.ScriptAddSpellModule(100, mod);      // Rank 1
    sScriptMgr.ScriptAddSpellModule(6178, mod);     // Rank 2
    sScriptMgr.ScriptAddSpellModule(11578, mod);    // Rank 3

    /**** Execute ****/
    sScriptMgr.ScriptAddSpellModule(5308, mod);     // Rank 1
    sScriptMgr.ScriptAddSpellModule(20660, mod);    // Rank 2
    sScriptMgr.ScriptAddSpellModule(20661, mod);    // Rank 3
    sScriptMgr.ScriptAddSpellModule(20662, mod);    // Rank 4
    sScriptMgr.ScriptAddSpellModule(25234, mod);    // Rank 5
    sScriptMgr.ScriptAddSpellModule(25236, mod);    // Rank 6
}