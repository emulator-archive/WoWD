//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////

// Warlock Spell: Life Tap
// =======================
// take health from caster, and
// add it to their mana.

#include "../game/StdAfx.h"

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

/* Actual Spell Code */
bool HandleDummySpell(uint32 uSpellId, uint32 i, Spell *pSpell)
{
    script_debuglog("Dummy spell call for spell %u = Life Tap, dmg: %u", uSpellId, pSpell->damage);
    // obtain our target
    Unit *target = pSpell->GetUnitTarget();
    if(target == NULL)
    {
        script_errorlog("Dummy spell %u (Life Tap) called with null unittarget!", uSpellId);
        return false;
    }

    // take the health from the player (20, etc)
    target->DealDamage(target, pSpell->damage, 0, 0, 0);
    
    // add the mana to the players mana (but check if we're bigger than max mana)
    // UNIT_FIELD_POWER1 == Mana

    uint32 new_val = target->GetUInt32Value(UNIT_FIELD_POWER1);
    uint32 max_val = target->GetUInt32Value(UNIT_FIELD_MAXPOWER1);
    new_val += pSpell->damage;

    // (new_val > max_val ? max_val : new_val)
    // means: if(new_val > max_val) use max_val as mana value
    //        else use new_val as the mana val

    target->SetUInt32Value(UNIT_FIELD_POWER1, (new_val > max_val ? max_val : new_val));

    // success.
    return true;
}

/* Module info */
void ScriptInitialize(ScriptModule *mod)
{
    sScriptMgr.ScriptAddSpellModule(1454, mod);		// life tap rank 1
    sScriptMgr.ScriptAddSpellModule(1455, mod);		// life tap rank 2
    sScriptMgr.ScriptAddSpellModule(1456, mod);		// life tap rank 3
    sScriptMgr.ScriptAddSpellModule(11687, mod);	// life tap rank 4
    sScriptMgr.ScriptAddSpellModule(11688, mod);	// life tap rank 5
    sScriptMgr.ScriptAddSpellModule(11689, mod);	// life tap rank 6
    sScriptMgr.ScriptAddSpellModule(27468, mod);	// life tap rank 7
    sScriptMgr.ScriptAddSpellModule(24722, mod);	// life tap rank 8
}