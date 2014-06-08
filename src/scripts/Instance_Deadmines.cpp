//////////////////////////////////////////////////
// Copyright (C) 2006 Burlex for WoWd Project
//////////////////////////////////////////////////

#include "StdAfx.h"

/* Script Export Declarations */

extern "C" WOWD_SCRIPT_DECL CreatureAIScript* CreateCreatureAIClass(uint32 uEntryId, Creature* creature);
extern "C" WOWD_SCRIPT_DECL InstanceScript* CreateInstanceClass(uint32 uMapId, MapMgr* instance);
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

// Rhakzor - Sends sound and talk message on enter combat, opens door on death.

class RhahkZorAI : public CreatureAIScript
{
public:
    RhahkZorAI(Creature* pCreature) : CreatureAIScript(pCreature) {}

    void OnDied(Unit * mKiller)
    {
        // Find "Factory Door"
        // GetGameObjectNearestCoords works here too.

        GameObject *pDoor = _unit->GetMapMgr()->GetInterface()->GetObjectNearestCoords<GameObject, TYPEID_GAMEOBJECT>(
            13965, -190.860092f, -456.332184f, 54.496822f);
        if(pDoor == 0)
            return;

        // Open the door
        pDoor->SetUInt32Value(GAMEOBJECT_FLAGS, 33);
        pDoor->SetUInt32Value(GAMEOBJECT_STATE, 0);
    }

    void OnCombatStart(Unit* mTarget)
    {
        _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Van Cleef pay big for your heads!");
        _unit->PlaySoundToSet(5774);
    }

    void Destroy()
    {
        delete (RhahkZorAI*)this;
    }
};

// Sneeds Shredder -> Opens Mast Room door on death.

class SneedsShredderAI : public CreatureAIScript
{
public:
    SneedsShredderAI(Creature* pCreature) : CreatureAIScript(pCreature) {}
    void OnDied(Unit * mKiller)
    {
        // Find "Mast Room Door"
        // GetGameObjectNearestCoords works here too.

        GameObject *pDoor = _unit->GetMapMgr()->GetInterface()->GetObjectNearestCoords<GameObject, TYPEID_GAMEOBJECT>(
            16400, -289.691650f, -535.988953f, 49.440678f);
        if(pDoor == 0)
            return;

        // Open the door
        pDoor->SetUInt32Value(GAMEOBJECT_FLAGS, 33);
        pDoor->SetUInt32Value(GAMEOBJECT_STATE, 0);
    }

    void Destroy()
    {
        delete (SneedsShredderAI*)this;
    }
};

// Gilnid -> Opens Foundary door on death.

class GilnidAI : public CreatureAIScript
{
public:
    GilnidAI(Creature* pCreature) : CreatureAIScript(pCreature) {}
    void OnDied(Unit * mKiller)
    {
        // Find "Foundry Door"
        // GetGameObjectNearestCoords works here too.

        GameObject *pDoor = _unit->GetMapMgr()->GetInterface()->GetObjectNearestCoords<GameObject, TYPEID_GAMEOBJECT>(
            16399, -176.569f, -577.640991f, 19.311600f);
        if(pDoor == 0)
            return;

        // Open the door
        pDoor->SetUInt32Value(GAMEOBJECT_FLAGS, 33);
        pDoor->SetUInt32Value(GAMEOBJECT_STATE, 0);
    }

    void Destroy()
    {
        delete (GilnidAI*)this;
    }
};

class VanCleefAI : public CreatureAIScript
{
public:
    VanCleefAI(Creature *pCreature) : CreatureAIScript(pCreature)
    {
        mPhase = 0;
    }

    void OnCombatStart(Unit* mTarget)
    {
        _unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "None may challenge the brotherhood.");
        _unit->PlaySoundToSet(5780);    // EdwinVanCleefAggro01.wav
    }

    void OnTargetDied(Unit* mTarget)
    {
        char msg[200];
        if(mTarget->GetTypeId() == TYPEID_PLAYER)
            sprintf(msg, "And stay down, %s.", static_cast<Player*>(mTarget)->GetName());
        else if (mTarget->GetTypeId() == TYPEID_UNIT)
            sprintf(msg, "And stay down, %s.", static_cast<Creature*>(mTarget)->GetName());

        _unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, msg);
        _unit->PlaySoundToSet(5781);    // EdwinVanCleefSlay01.wav
    }

    void OnDamageTaken(Unit* mAttacker, float fAmount)
    {
        if(fAmount < 5) return;

        // <100% hp -> We go to phase 1
        if(_unit->GetHealthPct() <= 100 && mPhase == 0) {
            ChangeToPhase1();
        }

        // <67% hp -> We go to phase 2
        if(_unit->GetHealthPct() <= 67 && mPhase == 1) {
            ChangeToPhase2();
        }

        // <34% hp -> We go to phase 3
        if(_unit->GetHealthPct() <= 34 && mPhase == 2) {
            ChangeToPhase3();
        }
    }

    void ChangeToPhase1()
    {
        // Set phase var
        mPhase = 1;
        
        // Play the sound, and send text.
        _unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "Lap dogs, all of you.");
        _unit->PlaySoundToSet(5782);    // EdwinVanCleefHealth01.wav
    }

    void ChangeToPhase2()
    {
        // Set phase var
        mPhase = 2;

        // Play sound, and send text.
        _unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "Fools! Our cause is righteous.");
        _unit->PlaySoundToSet(5783);    // EdwinVanCleefHealth02.wav
    }

    void ChangeToPhase3()
    {
        // Set phase var
        mPhase = 3;

        // Play sound, and send text.
        _unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "The brotherhood shall remain.");
        _unit->PlaySoundToSet(5784);    // EdwinVanCleefHealth03.wav
    }

    void Destroy()
    {
        delete (VanCleefAI*)this;
    }
protected:
    uint32 mPhase;  // NPC has 3 phases
};

class MrSmiteAI : public CreatureAIScript
{
public:
    MrSmiteAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
        mPhase = 0;
    }

    void OnCombatStart(Unit* mTarget)
    {
        // This guy has 2 messages he can say upon entering combat. Let's
        // make a 50/50 chance of using one.

        bool altmsg = false;
        if(Rand(50.0f))
            altmsg = true;
        char msg[200];
        uint32 soundid;

        if(altmsg) {
            sprintf(msg, "You there! Check out that noise.");
            soundid = 5775;
        } else {
            sprintf(msg, "We're under attack! Repel the invaders!");
            soundid = 5777;
        }

        _unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, msg);
        _unit->PlaySoundToSet(soundid);
    }
    
    // 2 phases, someone needs to script the combat changes
    void OnDamageTaken(Unit* mAttacker, float fAmount)
    {
        if(fAmount < 5) return;

        // <100% hp -> We go to phase 1
        if(_unit->GetHealthPct() <= 100 && mPhase == 0) {
            ChangeToPhase1();
        }

        // <50% hp -> We go to phase 2
        if(_unit->GetHealthPct() <= 50 && mPhase == 1) {
            ChangeToPhase2();
        }
    }

    void ChangeToPhase1()
    {
        mPhase = 1;
        _unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "You land lovers are tougher than I thought! I'll have to improvise.");
        _unit->PlaySoundToSet(5778);
    }

    void ChangeToPhase2()
    {
        mPhase = 2;
        _unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "Now you're making me angry!");
        _unit->PlaySoundToSet(5779);
    }

protected:
    uint32 mPhase;
};

/************************************************************************/
/* Creature AI Factory Functions                                        */
/************************************************************************/

CreatureAIScript* CreateCreatureAIClass(uint32 uEntryId, Creature* creature)
{
    // Create appropriate AI instance.
    switch(uEntryId)
    {
    case 644:   // RhahkZorAI
        return new RhahkZorAI(creature);
        break;
    case 642:   // SneedsShredderAI
        return new SneedsShredderAI(creature);
        break;
    case 1763:  // Gilnid
        return new GilnidAI(creature);
        break;
    case 639:   // VancleefAI
        return new VanCleefAI(creature);
        break;
    case 646:   // MrSmite
        return new MrSmiteAI(creature);
        break;
    }

    return 0;
}

/************************************************************************/
/* Instance Script  (Deadmines)                                         */
/************************************************************************/

class Deadmines : public InstanceScript
{
public:
    Deadmines(MapMgr *mapmgr) : InstanceScript(mapmgr)
    {

    }

    GameObject * GetObjectForOpenLock(Player *pCaster, Spell* pSpell, SpellEntry* pProto)
    {
        // This makes the defias cannon return the correct gameobject and blast
        // open the door.

        if(pProto->Id == 6250)
        {
            GameObject * Target = _instance->GetInterface()->GetGameObjectNearestCoords(-100.563477f, -667.747314f, 7.422109f);
            if(Target == 0) return 0;

            pCaster->PlaySpellVisual(Target->GetGUID(), pProto->SpellVisual);
            return Target;
        }
        return 0;
    }

    uint32 GetRespawnTimeForCreature(uint32 uEntryId, Creature *pCreature)
    {
        //TODO: Formula for respawn time based on elite level of creature and
        // level.

        // Default: no respawn.
        return 0;
    }

    void SetLockOptions(uint32 uEntryId, GameObject* pGameObject)
    {
        // These doors are opened by either mobs dying, or 
        // by a cannon, so we don't want to fucken leave them open
        // for newbs to be lazy with.

        switch(uEntryId)
        {
        /* case 16937: // Iron Clad Door */     // already locked
        case 16399: // Foundary Door
        case 16400: // Mast Room Door
        case 13965: // Factory Door
            {
                pGameObject->SetUInt32Value(GAMEOBJECT_STATE, 1);
                pGameObject->SetUInt32Value(GAMEOBJECT_FLAGS, 35);
            }break;
        }
    }

    void Destroy()
    {
        delete (Deadmines*)this;
    }
};

/************************************************************************/
/* Instance Class Factory                                               */
/************************************************************************/

InstanceScript* CreateInstanceClass(uint32 uMapId, MapMgr* instance)
{
    switch(uMapId)
    {
    case 36:    // Deadmines
        return new Deadmines(instance);
        break;
    }

    return 0;
}

/************************************************************************/
/* GameObject Script Declarations                                       */
/************************************************************************/

// This stuff seems to be done via spells.

/************************************************************************/
/* Script Initialize - Bind entries to this module.                     */
/************************************************************************/

void ScriptInitialize(ScriptModule *mod)
{
    sScriptMgr.ScriptAddCreatureModule(644, mod);   // Rhahk'Zor
    sScriptMgr.ScriptAddCreatureModule(642, mod);   // Sneeds's Shredder
    sScriptMgr.ScriptAddCreatureModule(1763, mod);  // Gilnid
    sScriptMgr.ScriptAddCreatureModule(639, mod);   // Van Cleef {BOSS}
    sScriptMgr.ScriptAddCreatureModule(646, mod);   // Mr Smite {PREBOSS}

    sScriptMgr.ScriptAddInstanceModule(36, mod);
}