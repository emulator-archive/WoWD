//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////

// Seal of Righteousness spell script

#include "StdAfx.h"

/* Script Export Declarations */

extern "C" WOWD_SCRIPT_DECL bool HandleDummyAura(uint32 uSpellId, uint32 i, Aura* pAura, bool apply);
extern "C" WOWD_SCRIPT_DECL void ScriptInitialize(ScriptModule *mod);

// Build version info function
BUILD_VERSIONINFO_DATA(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);

/* This is needed because windows is a piece of shit. ;) */
#ifdef WIN32

    BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
    {
        return TRUE;
    }

#endif

/* Actual Aura Code */
bool HandleDummyAura(uint32 uSpellId, uint32 i, Aura* pAura, bool apply)
{
    if(i != 0) return false;

	// omfg, so god damn fucking hacky.. but i guess w/ dummy spells
    // you cant expect everything to be in dbc :D

    // well, i could parse the strings :P but bah i'll just hardcode it here,
    // that's the point of scripts, right? ;)

    uint32 applyId = 0;
    Unit * u_caster = pAura->GetUnitCaster();
    if(u_caster == 0) return false;

    switch(uSpellId)
    {
        
    case 20154:         // Rank 1: "Melee attacks cause an additional $/87;20187s3 to $/25;20187s3 Holy damage."
        applyId = 20187;
        break; 
    case 20287:         // Rank 2: $/25;20280s3 
       applyId = 20280;
       break;
    case 20288:         // Rank 3:
        applyId = 20281;
        break;
    case 20289:         // Rank 4
        applyId = 20282;
        break;
    case 20290:         // Rank 5
        applyId = 20283;
        break;
    case 20291:         // Rank 6
        applyId = 20284;
        break;
    case 20292:         // Rank 7
        applyId = 20285;
        break;
    case 20293:         // Rank 8
        applyId = 20286;
        break;
    case 27155:         // Rank 9
        applyId = 27157;
        break;
    }

    SpellEntry * m_spellInfo = sSpellStore.LookupEntry(applyId);
    
    if(apply == true)
    {
        int32 value = 0;
        float randomPointsPerLevel = m_spellInfo->EffectDicePerLevel[2];
        int32 basePoints = m_spellInfo->EffectBasePoints[2] + 1;
        int32 randomPoints = m_spellInfo->EffectDieSides[2];
        if(u_caster)
            randomPoints += u_caster->getLevel() * (int32)randomPointsPerLevel;

        if(randomPoints <= 1)
            value = basePoints;
        else
            value = basePoints + rand()  %randomPoints;

        //this may be dangerous but let it be
        /*if(m_spellInfo->SpellGroupType)
        {
            SM_FIValue(u_caster->SM_FDummy,&value,m_spellInfo->SpellGroupType);
            SM_PIValue(u_caster->SM_PDummy,&value,m_spellInfo->SpellGroupType);
        }*/

        // add spell damage!
        uint32 max_dmg = value / 21;
        uint32 min_dmg = value / 27;
        u_caster->AddOnStrikeSpellDamage(uSpellId, min_dmg, max_dmg);

        // set the seal business
        if(u_caster->GetTypeId() == TYPEID_PLAYER)
        {
            ((Player*)u_caster)->judgespell = applyId;
            ((Player*)u_caster)->Seal = uSpellId;
        }
        u_caster->SetFlag(UNIT_FIELD_AURASTATE, 16);
    }
    else
    {
        u_caster->RemoveOnStrikeSpellDamage(uSpellId);
        // set the seal business
        if(u_caster->GetTypeId() == TYPEID_PLAYER)
        {
            ((Player*)u_caster)->judgespell = 0;
            ((Player*)u_caster)->Seal = 0;
        }
        u_caster->RemoveFlag(UNIT_FIELD_AURASTATE, 16);
    }

    return true;
}

void ScriptInitialize(ScriptModule *mod)
{
    sScriptMgr.ScriptAddAuraModule(20154, mod);
    sScriptMgr.ScriptAddAuraModule(20287, mod);
    sScriptMgr.ScriptAddAuraModule(20288, mod);
    sScriptMgr.ScriptAddAuraModule(20289, mod);
    sScriptMgr.ScriptAddAuraModule(20290, mod);
    sScriptMgr.ScriptAddAuraModule(20291, mod);
    sScriptMgr.ScriptAddAuraModule(20292, mod);
    sScriptMgr.ScriptAddAuraModule(20293, mod);
    sScriptMgr.ScriptAddAuraModule(27155, mod);
}