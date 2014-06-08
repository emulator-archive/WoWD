//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////
// Mage Dummy Spells

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

void ColdSnap(uint32 uSpellId, uint32 i, Spell * pSpell)
{
    if(!pSpell->p_caster) return;
    pSpell->p_caster->ClearCooldownsOnLine(6, pSpell->m_spellInfo->Id);
}

/* Actual Spell Code */
bool HandleDummySpell(uint32 uSpellId, uint32 i, Spell *pSpell)
{
    switch(uSpellId)
    {
    case 12472:
        ColdSnap(uSpellId, i, pSpell);
        break;
    }

    return true;
}

/* Module info */
void ScriptInitialize(ScriptModule *mod)
{
    ADD_SPELL(12472);       // Cold Snap
}