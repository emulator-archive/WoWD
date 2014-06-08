//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////
// Rogue Dummy Spells

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

void Preparation(uint32 uSpellId, uint32 i, Spell * pSpell)
{
    Player * playerTarget = pSpell->p_caster;
    if(playerTarget == 0) return;

    playerTarget->ClearCooldownsOnLine(39, pSpell->m_spellInfo->Id); // line - subtlety
    playerTarget->ClearCooldownsOnLine(38, pSpell->m_spellInfo->Id); // line - combat
    playerTarget->ClearCooldownsOnLine(253, pSpell->m_spellInfo->Id);// line - assassination
}

/* Actual Spell Code */
bool HandleDummySpell(uint32 uSpellId, uint32 i, Spell *pSpell)
{
    switch(uSpellId)
    {
    case 14185:
        Preparation(uSpellId, i, pSpell);
        break;
    }

    return true;
}

/* Module info */
void ScriptInitialize(ScriptModule *mod)
{
    ADD_SPELL(14185);       // Preparation
}