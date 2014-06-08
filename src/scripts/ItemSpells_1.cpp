//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////
// Various item dummy scripts

#include "StdAfx.h"

/* Script Export Declarations */

extern "C" WOWD_SCRIPT_DECL bool HandleDummyAura(uint32 uSpellId, uint32 i, Aura* pAura, bool apply);
extern "C" WOWD_SCRIPT_DECL bool HandleDummySpell(uint32 uSpellId, uint32 i, Spell * pSpell);
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

void GnomishTransporter(uint32 uSpellId, uint32 i, Spell * pSpell)
{
    Player * plr = pSpell->p_caster;
    if(plr == 0) return;

    plr->EventAttackStop();
    plr->SafeTeleport(1, 0, LocationVector(-7169.41f, -3838.63, 8.72f));
}

void NoggenFoggerElixr(uint32 uSpellId, uint32 i, Spell * pSpell)
{
    uint32 chance = sRand.randInt(3);
    switch(chance)
    {
    case 0:
        pSpell->u_caster->CastSpell(pSpell->u_caster, sSpellStore.LookupEntry(16591), true);
        break;

    case 1:
        pSpell->u_caster->CastSpell(pSpell->u_caster, sSpellStore.LookupEntry(16593), true);
        break;

    case 2:
        pSpell->u_caster->CastSpell(pSpell->u_caster, sSpellStore.LookupEntry(16595), true);
        break;
    }
}

void BanishExile(uint32 uSpellId, uint32 i, Spell * pSpell)
{
    Unit * target = pSpell->GetUnitTarget();
    if(target == 0) return;

    pSpell->m_caster->SpellNonMeleeDamageLog(target, uSpellId, target->GetUInt32Value(UNIT_FIELD_HEALTH), true);
}

void QuestSpellHandler(uint32 uSpellId, uint32 i, Spell *pSpell)
{
    // actual spell handling code will be in here.
    Unit *unitTarget = pSpell->GetUnitTarget();
    Player *p_caster = pSpell->p_caster;
    if(!unitTarget || !p_caster || unitTarget->GetTypeId() != TYPEID_UNIT)
        return;

    switch(uSpellId)
    {
    case 11536:
        {
            WorldPacket data(SMSG_PLAY_OBJECT_SOUND, 12);
            data << uint32(6197) << unitTarget->GetGUID();
            p_caster->SendMessageToSet(&data, true);

            sQuestMgr.OnPlayerKill(p_caster, ((Creature*)unitTarget));  // this should increment in quest log
        }break;
    case 19938:
        {
            WorldPacket data(SMSG_PLAY_OBJECT_SOUND, 12);
            data << uint32(6197) << unitTarget->GetGUID();
            p_caster->SendMessageToSet(&data, true);

            sQuestMgr.OnPlayerKill(p_caster, ((Creature*)unitTarget));  // this should increment in quest log

            // Send chat message
            char msg[100];
            sprintf(msg, "Ow! Ok, I'll get back to work, %s", p_caster->GetName());
            unitTarget->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, msg);
        }break;
    }
}

void HolyShock(uint32 uSpellId, uint32 i, Spell *pSpell)
{
    // actual spell handling code will be in here.
    Unit *unitTarget = pSpell->GetUnitTarget();
    if(!pSpell->p_caster || !unitTarget) return;

    SpellEntry *spInfo = NULL;

    if (isAttackable(pSpell->p_caster,unitTarget))
    {
        uint32 spellid = 0;
        switch(uSpellId)
        {
        case 20473:
            spellid = 25912;
            break;
        case 20929:
            spellid = 25911;
            break;
        case 20930:
            spellid = 25902;
            break;
        }
        spInfo = sSpellStore.LookupEntry(spellid); // if its a enemy
    } else {
        uint32 spellid = 0;
        switch(uSpellId)
        {
        case 20473:
            spellid = 25914;
            break;
        case 20929:
            spellid = 25913;
            break;
        case 20930:
            spellid = 25903;
            break;
        }
        spInfo = sSpellStore.LookupEntry(spellid); // if its a ally
    }

    if(!spInfo)
        return;

    pSpell->p_caster->CastSpell(unitTarget, spInfo, true);
}

void HallowsEndCandy(uint32 uSpellId, uint32 i, Spell * pSpell)
{
    // actual spell handling code will be in here.
    if(!pSpell->p_caster) return;

    int chance = sRand.randInt(4);
    int spellId = 24924;
    
    SpellEntry *spInfo = sSpellStore.LookupEntry(spellId+chance);
    if(!spInfo) return;

    pSpell->p_caster->CastSpell(pSpell->p_caster, spInfo, true);
}

void CookiedDeviateFish(uint32 uSpellId, uint32 i, Spell * pSpell)
{
    // actual spell handling code will be in here.
    Player *playerTarget = pSpell->GetPlayerTarget();
    Unit *unitTarget = pSpell->GetUnitTarget();

    if(!playerTarget) return;
    //(60 min) Yaaarrrr (turns you into a pirate)
    //(60 min) Flip Out (turns you into a ninja)
    //(10 sec) Oops (turns you into a pile of goo)
    //(2 min) Rapid Cast
    //(10 min) Cowardice (causes 5sec "Run Away" fear)
    //(insta) +50 Rage (Warriors Only)
    //(5 min) Fake Death
    //8221,8219,8223,8215,8224,8226

    int chance = 0;
    int spellId = 0;

    if(playerTarget->getClass() == WARRIOR)
        chance = sRand.randInt(7);
    else
        chance = sRand.randInt(6);

    switch(chance)
    {
    case 0:
        spellId = 8215;
        break;
    case 1:
        spellId = 8219;
        break;
    case 2:
        spellId = 8221;
        break;
    case 3:
        spellId = 8223;
        break;
    case 4:
        spellId = 8224;
        break;
    case 5:
        spellId = 8226;
        break;
    default:
        spellId = 0;
        break;
    };

    if(spellId)
    {
        SpellEntry *spInfo = sSpellStore.LookupEntry(spellId);
        if(!spInfo)
            return;

        pSpell->p_caster->CastSpell(pSpell->p_caster, spInfo, true);
    }
    else
    {
        uint32 val=playerTarget->GetUInt32Value(UNIT_FIELD_POWER2)+500;
        playerTarget->SetUInt32Value(UNIT_FIELD_POWER2, val>=1000?1000:val);
    }
}

void NetOMatic(uint32 uSpellId, uint32 i, Spell * pSpell)
{
    Player *playerTarget = pSpell->GetPlayerTarget();
    Unit *unitTarget = pSpell->GetUnitTarget();
    //FIXME: Captures the target in a net for 10 sec. The net has a lot of hooks however and sometimes gets caught in the user's clothing when fired......            
    //Successful Net target: ~50% could be a little less.

    //Net's you instead: ~25%-30%
    //basically roots you for 10 seconds

    //Net's you and target: ~25-30%
    //This is an interesting effect, but if you're chasing a runner it is just as good.  Basically what it does is shoots the net, runs you up to the target and roots both of you. 

    if(!playerTarget)
        return;
    SpellCastTargets tgt,tgt2;
    int spell = (sRand.randInt(100))+1;
    uint64 t1 = 0;
    uint64 t2 = 0;

    if(spell < 50)
        t1 = playerTarget->GetSelection();
    else if(spell < 75)
        t2 = playerTarget->GetGUID();
    else
    {
        t1 = playerTarget->GetSelection();
        t2 = playerTarget->GetGUID();
    }

    SpellEntry *spInfo = sSpellStore.LookupEntry(13099);
    if(!spInfo)
        return;

    if(t1)
    {
        Unit * un = World::GetUnit(t1);
        if(un) pSpell->u_caster->CastSpell(un, spInfo, true);
    }
    
    if(t2)
    {
        Unit * un = World::GetUnit(t2);
        if(un) pSpell->u_caster->CastSpell(un, spInfo, true);
    }
}

void HolidayCheer(uint32 uSpellId, uint32 i, Spell * pSpell)
{
    Unit * pUnit;
    float dist = pSpell->GetRadius(i);
    for(ObjectSet::iterator itr = pSpell->m_caster->GetInRangeSetBegin();
        itr != pSpell->m_caster->GetInRangeSetEnd(); ++itr)
    {
        if((*itr)->IsUnit())
            pUnit = (Unit*)*itr;
        else
            continue;

        if(pSpell->m_caster->CalcDistance(pUnit) > dist || isAttackable(pSpell->m_caster, pUnit))
            continue;

        pUnit->Emote(EMOTE_ONESHOT_LAUGH);
    }
}

void DeviateFish(uint32 uSpellId, uint32 i, Spell * pSpell)
{
    int spell = sRand.randInt(5) + 1;

    SpellEntry *spInfo = sSpellStore.LookupEntry((pSpell->m_spellInfo->Id+spell));
    if(!spInfo)
        return;

    pSpell->u_caster->CastSpell(pSpell->u_caster, spInfo, true);
}

/* Spell Code */
bool HandleDummySpell(uint32 uSpellId, uint32 i, Spell * pSpell)
{
    switch(uSpellId)
    {
    case 23453:
        GnomishTransporter(uSpellId, i, pSpell);
        break;

    case 16589:
        NoggenFoggerElixr(uSpellId, i, pSpell);
        break;

    case 4130:
    case 4131:
    case 4132:  
        BanishExile(uSpellId, i, pSpell);
        break;

    case 11536:
    case 19938:
        QuestSpellHandler(uSpellId, i, pSpell);
        break;

    case 20473:
    case 20939:
    case 20930:
        HolyShock(uSpellId, i, pSpell);
        break;

    case 24430:
        HallowsEndCandy(uSpellId, i, pSpell);
        break;

    case 8213:
        CookiedDeviateFish(uSpellId, i, pSpell);
        break;

    case 8063:
        DeviateFish(uSpellId, i, pSpell);
        break;

    case 26074:
        HolidayCheer(uSpellId, i, pSpell);
        break;

    case 13120:
        NetOMatic(uSpellId, i, pSpell);
        break;
    }

    return true;
}

/* Actual Aura Code */
bool HandleDummyAura(uint32 uSpellId, uint32 i, Aura* pAura, bool apply)
{
    return false;
}

void ScriptInitialize(ScriptModule *mod)
{
    ADD_SPELL(23453);       // Gnomish Transporter
    ADD_SPELL(16589);       // Noggenfogger
    ADD_SPELL(4130);        // Banish Exile
    ADD_SPELL(4131);        // Banish Exile
    ADD_SPELL(4132);        // Banish Exile
    ADD_SPELL(11536);       // Quest spell
    ADD_SPELL(19938);       // Quest spell
    ADD_SPELL(20473);       // Holy Shock 1
    ADD_SPELL(20929);       // Holy Shock 2
    ADD_SPELL(20930);       // Holy Shock 3
    ADD_SPELL(24430);       // Hallows End Candy
    ADD_SPELL(8213);        // Cooked Deviate Fish
    ADD_SPELL(26074);       // Holiday Cheer
    ADD_SPELL(8063);        // Deviate Fish
    ADD_SPELL(13120);       // Net-o-Matic
}
