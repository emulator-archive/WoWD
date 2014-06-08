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

#include "StdAfx.h"

AIInterface::AIInterface()
{
    m_canMove = true;
    m_destinationX = m_destinationY = m_destinationZ = 0;
    m_nextPosX = m_nextPosY = m_nextPosZ = 0;
    UnitToFollow = NULL;
    FollowDistance = 0.0f;
    m_fallowAngle = M_PI/2;
    m_timeToMove = 0;
    m_timeMoved = 0;
    m_moveTimer = 0;
    m_nWaypoints = 0;
    m_WayPointsShowing = false;
    m_WayPointsShowBackwards = false;
    m_currentWaypoint = 0;
    m_moveBackward = false;
    m_moveType = 0;
    m_moveRun = false;
    m_moveSprint = false;
    m_moveFly = false;
    m_creatureState = STOPPED;
    m_canCallForHelp = false;
    m_hasCalledForHelp = false;
    m_fleeTimer = 0;
    m_FleeDuration = 0;
    m_canFlee = false;
    m_hasFleed = false;
    m_canRangedAttack = false;
    m_FleeHealth = m_CallForHelpHealth = 0.0f;
    m_AIState = STATE_IDLE;

    m_updateAssist = false;
    m_updateTargets = false;
    m_updateAssistTimer = 1;
    m_updateTargetsTimer = TARGET_UPDATE_INTERVAL;

    m_hasOnEnterCombatSpells = m_hasOnLeaveCombatSpells = m_hasOnDamageTakenSpells = m_hasOnTargetCastSpellSpells = m_hasOnTargetParryedSpells = m_hasOnTargetDodgedSpells = m_hasOnTargetBlockedSpells =
        m_hasOnTargetCritHitSpells = m_hasOnTargetDiedSpells = m_hasOnUnitParryedSpells = m_hasOnUnitDodgedSpells = m_hasOnUnitBlockedSpells = m_hasOnUnitCritHitSpells = m_hasOnUnitDiedSpells =
        m_hasOnAssistTargetDiedSpells = m_hasOnFollowOwnerSpells = m_hasCooldownOnEnterCombatSpells = m_hasCooldownOnLeaveCombatSpells = m_hasCooldownOnDamageTakenSpells = m_hasCooldownOnTargetCastSpellSpells = m_hasCooldownOnTargetParryedSpells =
        m_hasCooldownOnTargetDodgedSpells = m_hasCooldownOnTargetBlockedSpells = m_hasCooldownOnTargetCritHitSpells = m_hasCooldownOnTargetDiedSpells = m_hasCooldownOnUnitParryedSpells = m_hasCooldownOnUnitDodgedSpells =
        m_hasCooldownOnUnitBlockedSpells = m_hasCooldownOnUnitCritHitSpells = m_hasCooldownOnUnitDiedSpells = m_hasCooldownOnAssistTargetDiedSpells = m_hasCooldownOnFollowOwnerSpells = false;


    m_nextSpell = NULL;
    m_nextTarget = NULL;
    m_Unit = NULL;
    m_PetOwner = NULL;
    m_aiCurrentAgent = AGENT_NULL;
    m_moveSpeed = 0.0f;
    UnitToFear = NULL;
    firstLeaveCombat = true;
    m_outOfCombatRange = 2500;

    tauntedBy = NULL;
    isTaunted = false;
    m_AllowedToEnterCombat = true;
    m_DefaultSpell = m_DefaultMeleeSpell = NULL;
    m_totalMoveTime = 0;
    m_lastFollowX = m_lastFollowY = 0;
    m_FearTimer = 0;
    m_WanderTimer = 0;
    m_totemspelltime = 0;
    m_totemspelltimer = 0;
    m_formationFollowAngle = 0.0f;
    m_formationFollowDistance = 0.0f;
    m_formationLinkTarget = 0;
    m_formationLinkSqlId = 0;
}

void AIInterface::Init(Unit *un, AIType at, MovementType mt)
{
    ASSERT(at != AITYPE_PET);

    m_AIType = at;
    m_MovementType = mt;

    m_AIState = STATE_IDLE;
    m_MovementState = MOVEMENTSTATE_STOP;

    m_Unit = un;

    m_moveSpeed = m_Unit->m_runSpeed*0.001f;
    if(!m_DefaultMeleeSpell)
    {
        m_DefaultMeleeSpell = new AI_Spell;
        m_DefaultMeleeSpell->entryId = 0;
        m_DefaultMeleeSpell->spellType = 0;
        m_DefaultMeleeSpell->agent = AGENT_MELEE;
        m_DefaultSpell = m_DefaultMeleeSpell;
    }
    m_sourceX = un->GetPositionX();
    m_sourceY = un->GetPositionY();
    m_sourceZ = un->GetPositionZ();
    m_guardTimer = getMSTime();
}

AIInterface::~AIInterface()
{
    // cleanup all spell maps
    CleanupSpellMap(&m_OnEnterCombatSpells);
    CleanupSpellMap(&m_OnLeaveCombatSpells);
    CleanupSpellMap(&m_OnDamageTakenSpells);
    CleanupSpellMap(&m_OnTargetCastSpellSpells);
    CleanupSpellMap(&m_OnTargetParryedSpells);
    CleanupSpellMap(&m_OnTargetDodgedSpells);
    CleanupSpellMap(&m_OnTargetBlockedSpells);
    CleanupSpellMap(&m_OnTargetCritHitSpells);
    CleanupSpellMap(&m_OnTargetDiedSpells);
    CleanupSpellMap(&m_OnUnitParryedSpells);
    CleanupSpellMap(&m_OnUnitDodgedSpells);
    CleanupSpellMap(&m_OnUnitCritHitSpells);
    CleanupSpellMap(&m_OnUnitBlockedSpells);
    CleanupSpellMap(&m_OnUnitDiedSpells);
    CleanupSpellMap(&m_OnAssistTargetDiedSpells);
    CleanupSpellMap(&m_OnFollowOwnerSpells);

    if(m_DefaultMeleeSpell)
        delete m_DefaultMeleeSpell;

#ifndef PRECACHE_LOAD
    for(WayPointMap::iterator iter = m_waypoints.begin(); iter != m_waypoints.end(); ++iter)
    {
        delete iter->second;
    }
#endif
    m_waypoints.clear();
}

void AIInterface::Init(Unit *un, AIType at, MovementType mt, Unit *owner)
{
    ASSERT(at == AITYPE_PET || at == AITYPE_TOTEM);

    m_AIType = at;
    m_MovementType = mt;

    m_AIState = STATE_IDLE;
    m_MovementState = MOVEMENTSTATE_STOP;

    m_Unit = un;
    m_PetOwner = owner;

    m_moveSpeed = m_Unit->m_runSpeed*0.001f;
    if(!m_DefaultMeleeSpell)
    {
        m_DefaultMeleeSpell = new AI_Spell;
        m_DefaultMeleeSpell->agent = AGENT_MELEE;
        m_DefaultSpell = m_DefaultMeleeSpell;
    }
    m_sourceX = un->GetPositionX();
    m_sourceY = un->GetPositionY();
    m_sourceZ = un->GetPositionZ();
}

void AIInterface::HandleEvent(uint32 event, Unit* pUnit, uint32 misc1)
{
    if(!pUnit)
        return;
    
    if(m_AIState != STATE_EVADE)
    {
        switch(event)
        {
        case EVENT_ENTERCOMBAT:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnCombatStart)(pUnit);
                
                // Stop the emote
                m_Unit->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                m_returnX = m_Unit->GetPositionX();
                m_returnY = m_Unit->GetPositionY();
                m_returnZ = m_Unit->GetPositionZ();

                m_moveRun = true; //run to the target

                if(m_AIState != STATE_ATTACKING)
                    StopMovement(0);

                m_AIState = STATE_ATTACKING;
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
                firstLeaveCombat = true;
                if(pUnit)
                {
                    m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, pUnit->GetGUID());
                }
            }break;
        case EVENT_LEAVECOMBAT:
            {
                // restart emote
                if(m_Unit->GetBaseUInt32Value(UNIT_NPC_EMOTESTATE) != 0)
                    m_Unit->SetUInt32Value(UNIT_NPC_EMOTESTATE, m_Unit->GetBaseUInt32Value(UNIT_NPC_EMOTESTATE));

                if(m_AIType == AITYPE_PET)
                {
                    m_AIState = STATE_FOLLOWING;
                    UnitToFollow = m_PetOwner;
                    FollowDistance = 3.0f;
                    m_lastFollowX = m_lastFollowY = 0;
                    ((Pet*)m_Unit)->SetPetAction(PET_ACTION_FOLLOW);
                    HandleEvent(EVENT_FOLLOWOWNER, 0, 0);
                }
                else
                {
                    CALL_SCRIPT_EVENT(m_Unit, OnCombatStop)(UnitToFollow);
                    m_AIState = STATE_EVADE;
                    m_Unit->setAttackTarget(NULL);

                    UnitToFollow = NULL;
                    FollowDistance = 0.0f;
                    m_lastFollowX = m_lastFollowY = 0;
                }

                m_aiTargets.clear();            
                m_fleeTimer = 0;
                m_hasFleed = false;
                m_hasCalledForHelp = false;
                m_nextSpell = NULL;
                m_nextTarget = NULL;
                
                m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);

                resetSpellCounter();
                firstLeaveCombat = false;

                // Scan for a new target before moving back on waypoint path
                Unit * Target = FindTarget();
                if(Target != NULL)
                    AttackReaction(Target, 1, 0, 0);
                else
                    firstLeaveCombat = true;

                /*SpellEntry* spell = getSpellEntry(2054);
                Affect* aff = new Affect(spell, 6000, m_Unit->GetGUID());
                aff->SetHealPerTick(uint16(m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH)/4), 2000);
                m_Unit->AddAffect(aff);*/
            }break;
        case EVENT_DAMAGETAKEN:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnDamageTaken)(pUnit, misc1);
                if(!modThreatByPtr(pUnit, misc1))
                {
                    m_aiTargets.insert(TargetMap::value_type(pUnit, misc1));
                }
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
            }break;
        case EVENT_TARGETCASTSPELL:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnCastSpell)(0);  //fixme
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
            }break;
        case EVENT_TARGETPARRYED:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnTargetParried)(pUnit);
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
            }break;
        case EVENT_TARGETDODGED:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnTargetDodged)(pUnit);
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
            }break;
        case EVENT_TARGETBLOCKED:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnTargetBlocked)(pUnit,misc1);
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
            }break;
        case EVENT_TARGETCRITHIT:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnTargetCritHit)(pUnit,(float)misc1);
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
            }break;
        case EVENT_TARGETDIED:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnTargetDied)(pUnit);
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
            }break;
        case EVENT_UNITPARRYED:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnParried)(pUnit);
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
            }break;
        case EVENT_UNITDODGED:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnDodged)(pUnit);
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
            }break;
        case EVENT_UNITBLOCKED:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnBlocked)(pUnit, misc1);
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
            }break;
        case EVENT_UNITCRITHIT:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnCritHit)(pUnit, (float)misc1);
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
            }break;
        case EVENT_ASSISTTARGETDIED:
            {
                CALL_SCRIPT_EVENT(m_Unit, OnAssistTargetDied)(pUnit);
                m_nextSpell = getSpellByEvent(event);
                m_nextTarget = FindTargetForSpell(m_nextSpell);
            }break;
        case EVENT_FOLLOWOWNER:
            {
                m_AIState = STATE_FOLLOWING;
                if(m_Unit->GetGUIDHigh() == HIGHGUID_PET)
                    ((Pet*)m_Unit)->SetPetAction(PET_ACTION_FOLLOW);
                UnitToFollow = m_PetOwner;
                m_lastFollowX = m_lastFollowY = 0;
                FollowDistance = 4.0f;

                m_aiTargets.clear();
                m_fleeTimer = 0;
                m_hasFleed = false;
                m_hasCalledForHelp = false;
                m_nextSpell = NULL;
                m_nextTarget = NULL;
                m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
                resetSpellCounter();
            }break;
        case EVENT_PET_ATTACK:
            {
                m_AIState = STATE_ATTACKING;
                m_Unit->GetAIInterface()->AttackReaction(pUnit,1,0);
            }break;
        case EVENT_FEAR:
            {   
                CALL_SCRIPT_EVENT(m_Unit, OnFear)(pUnit, 0);
                m_AIState = STATE_FEAR;
                StopMovement(1);
                UnitToFollow = NULL;
                m_lastFollowX = m_lastFollowY = 0;
                FollowDistance = 0.0f;

                m_aiTargets.clear();            
                m_fleeTimer = 0;
                m_hasFleed = false;
                m_hasCalledForHelp = false;
                m_moveRun = true;
                
                // update speed
                m_Unit->m_runSpeed /= 2;
                getMoveFlags();

                m_nextSpell = NULL;
                m_nextTarget = NULL;
                
                m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
                //m_Unit->setAttackTarget(NULL);

                resetSpellCounter();
            }break;
        case EVENT_UNFEAR:
            {
                // update speed
                m_Unit->m_runSpeed *= 2;
                getMoveFlags();
            }break;
        case EVENT_WANDER:
            {   
                //CALL_SCRIPT_EVENT(m_Unit, OnWander)(pUnit, 0); FIXME
                m_AIState = STATE_WANDER;
                StopMovement(1);
                UnitToFollow = NULL;
                m_lastFollowX = m_lastFollowY = 0;
                FollowDistance = 0.0f;

                m_aiTargets.clear();            
                m_fleeTimer = 0;
                m_hasFleed = false;
                m_hasCalledForHelp = false;
                m_moveRun = true;
                
                // update speed
                m_Unit->m_runSpeed /= 2;
                getMoveFlags();

                m_nextSpell = NULL;
                m_nextTarget = NULL;
                
                m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
                //m_Unit->setAttackTarget(NULL);

                resetSpellCounter();
            }break;
        case EVENT_UNWANDER:
            {
                // update speed
                m_Unit->m_runSpeed *= 2;
                getMoveFlags();
            }break;       
        default:
            {
            }break;
        }
    }
    if(event != EVENT_UNITDIED)
        m_Unit->setAttackTarget(m_nextTarget);

    //Should be able to do this stuff even when evading
    switch(event)
    {
        case EVENT_UNITDIED:
        {
            CALL_SCRIPT_EVENT(m_Unit, OnDied)(pUnit);
            m_AIState = STATE_IDLE;

            StopMovement(0);
            m_aiTargets.clear();
            UnitToFollow = NULL;
            m_lastFollowX = m_lastFollowY = 0;
            UnitToFear = NULL;
            FollowDistance = 0.0f;
            m_fleeTimer = 0;
            m_hasFleed = false;
            m_hasCalledForHelp = false;
            m_nextSpell = NULL;
            m_nextTarget = NULL;
        
            //reset waypoint to 0
            m_currentWaypoint = 0;
                
            m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
            
            // There isn't any need to do any attacker checks here, as
            // they should all be taken care of in DealDamage

            resetSpellCounter();
            //BURLEXFIXME: check state, and follow owner again if appropriate
            if(m_AIType == AITYPE_PET)
            {
                SetUnitToFollow(m_PetOwner);
                SetFollowDistance(3.0f);
                HandleEvent(EVENT_FOLLOWOWNER, m_Unit, 0);
            }
        }break;
    }
}

void AIInterface::Update(uint32 p_time)
{
    if(m_AIType == AITYPE_TOTEM)
    {
        assert(m_DefaultSpell != 0 && totemspell != 0);
        if(p_time >= m_totemspelltimer)
        {
            // these will *almost always* be AoE, so no need to find a target here.
            SpellCastTargets targets(m_Unit->GetGUID());
            Spell * pSpell = new Spell(m_Unit, totemspell, true, 0);
            pSpell->prepare(&targets);

            // need proper cooldown time!
            m_totemspelltimer = m_totemspelltime;
        }
        else
        {
            m_totemspelltimer -= p_time;
        }
        return;
    }

    _UpdateTimer(p_time);
    _UpdateTargets();
    if(m_Unit->isAlive() && m_AIState != STATE_IDLE 
        && m_AIState != STATE_FOLLOWING && m_AIState != STATE_FEAR 
        && m_AIState != STATE_WANDER && m_AIState != STATE_SCRIPTMOVE)
    {
        if(m_AIType == AITYPE_PET )
        {
            Pet * pPet = static_cast<Pet*>(m_Unit);

            if(pPet->GetPetAction() == PET_ACTION_ATTACK || pPet->GetPetState() != PET_STATE_PASSIVE)
            {
                // todo: some check for autocast here
                /*if(m_DefaultSpell && !m_nextSpell)
                    m_nextSpell = m_DefaultSpell;*/
                if(m_DefaultSpell && !m_nextSpell)
                {
                    if(m_DefaultSpell->agent == AGENT_SPELL)
                    {
                        // Check cooldown on this spell
                        if(pPet->GetSpellCooldown(m_DefaultSpell->spellId) == 0)
                        {
                            // We're fine to cast.
                            m_nextSpell = m_DefaultSpell;
                        }                            
                        else
                        {
                            // Waiting for cooldown to expire....
                        }
                    }
                }

                _UpdateCombat(p_time);
            }
        }
        else
        {
            _UpdateCombat(p_time);
        }
    }
    _UpdateMovement(p_time);

    if(m_AIState == STATE_EVADE)
    {
        if(m_creatureState != MOVING)
        {
            if(m_AIType == AITYPE_PET && m_PetOwner != NULL)
            {
                m_returnX = m_PetOwner->GetPositionX()+(3*(cosf(m_fallowAngle+m_PetOwner->GetOrientation())));
                m_returnY = m_PetOwner->GetPositionY()+(3*(sinf(m_fallowAngle+m_PetOwner->GetOrientation())));
                m_returnZ = m_PetOwner->GetPositionZ();
            }

            if(m_returnX != 0.0f && m_returnY != 0.0f && m_returnZ != 0.0f)
            {
                //return to last position before attacking
                MoveTo(m_returnX,m_returnY,m_returnZ,m_Unit->GetOrientation());
            }
        }
        //else
        //{
            if(m_returnX !=0.0f && m_returnY != 0.0f)
            {
                if(m_Unit->GetDistanceSq(m_returnX,m_returnY,m_returnZ) < 4.0f/*2.0*/)
                {
                    m_AIState = STATE_IDLE;
                    m_returnX = m_returnY = m_returnZ = 0.0f;
                    if(hasWaypoints())
                    {
                        if(m_moveBackward)
                        {
                            if(m_currentWaypoint != m_nWaypoints-1)
                                m_currentWaypoint++;
                        }
                        else
                        {
                            if(m_currentWaypoint != 0)
                                m_currentWaypoint--;
                        }

                    }
                    // Set health to full if they at there last location before attacking
                    if(m_Unit->GetMapId() > 1 && m_Unit->GetMapId() != 530)
                        m_Unit->SetUInt32Value(UNIT_FIELD_HEALTH,m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH));
                }
            }
        //}
    }
    if(m_fleeTimer)
    {
        if(m_fleeTimer > p_time)
        {
            m_fleeTimer -= p_time;
            _CalcDestinationAndMove(m_nextTarget, 5.0f);
        }
        else
        {
            m_fleeTimer = 0;
            m_nextTarget = FindTargetForSpell(m_nextSpell);
        }
    }


    //Pet Dismiss after a certian ditance away
    if(m_AIType == AITYPE_PET && m_PetOwner != NULL)
    {
        float dist = m_Unit->GetDistanceSq(m_PetOwner);
        if(dist > 8100.0f/*90.0f*/) //90 yard away we Dismissed
        {
            DismissPet();
            return;
        }
    }
}

void AIInterface::_UpdateTimer(uint32 p_time)
{
    if(m_updateAssistTimer > p_time)
    {
        m_updateAssistTimer -= p_time;
    }else
    {
        m_updateAssist = true;
        m_updateAssistTimer = TARGET_UPDATE_INTERVAL * 2 - m_updateAssistTimer - p_time;
    }

    if(m_updateTargetsTimer > p_time)
    {
        m_updateTargetsTimer -= p_time;
    }else
    {
        m_updateTargets = true;
        m_updateTargetsTimer = TARGET_UPDATE_INTERVAL * 2 - m_updateTargetsTimer - p_time;
    }

    _UpdateCooldownTimers(p_time);
}

void AIInterface::_UpdateCooldownTimers(uint32 p_time)
{
    _UpdateCooldownTimer(&m_hasCooldownOnEnterCombatSpells, &m_OnEnterCombatSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnLeaveCombatSpells, &m_OnLeaveCombatSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnDamageTakenSpells, &m_OnDamageTakenSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnTargetCastSpellSpells, &m_OnTargetCastSpellSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnTargetParryedSpells, &m_OnTargetParryedSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnTargetDodgedSpells, &m_OnTargetDodgedSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnTargetBlockedSpells, &m_OnTargetBlockedSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnTargetCritHitSpells, &m_OnTargetCritHitSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnTargetDiedSpells, &m_OnTargetDiedSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnUnitParryedSpells, &m_OnUnitParryedSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnUnitDodgedSpells, &m_OnUnitDodgedSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnUnitBlockedSpells, &m_OnUnitBlockedSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnUnitCritHitSpells, &m_OnUnitCritHitSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnUnitDiedSpells, &m_OnUnitDiedSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnAssistTargetDiedSpells, &m_OnAssistTargetDiedSpells, p_time);
    _UpdateCooldownTimer(&m_hasCooldownOnFollowOwnerSpells, &m_OnFollowOwnerSpells, p_time);
}

void AIInterface::_UpdateTargets()
{
    if(!m_Unit->GetCreatureName() || m_Unit->GetCreatureName()->Type == CRITTER)
        return;

    AssistTargetSet::iterator i, i2;
    TargetMap::iterator itr, itr2;

    // Find new Assist Targets and remove old ones
    if(m_AIState == STATE_FLEEING)
    {
        FindFriends(100.0f/*10.0*/);
    }
    else if(m_AIState != STATE_IDLE && m_AIState != STATE_SCRIPTIDLE)
    {
        FindFriends(16.0f/*4.0f*/);
    }

    if(m_updateAssist)
    {
        m_updateAssist = false;
        //modified for vs2005 compatibility
        for(i = m_assistTargets.begin(); i != m_assistTargets.end();)
        {
            i2 = i;
            ++i;
            if(m_Unit->GetDistanceSq((*i2)) > 2500.0f/*50.0f*/ || !(*i2)->isAlive() || !(*i2)->isInCombat())
            {
                m_assistTargets.erase(i2);
            }
        }
    }

    if(m_updateTargets)
    {
        m_updateTargets = false;
        //modified for vs2005 compatibility
        for(itr = m_aiTargets.begin(); itr != m_aiTargets.end();)
        {
            itr2 = itr;
            ++itr;

            //if(!itr->target)        // we shouldnt get to here, i'm guessing.
                //continue;

            if(!itr2->first->isAlive() || m_Unit->GetDistanceSq(itr2->first) >= 6400.0f/*80.0f*/)
            {
                m_aiTargets.erase(itr2);
            }
        }
        if(m_aiTargets.size() == 0 
            && m_AIState != STATE_IDLE && m_AIState != STATE_FOLLOWING 
            && m_AIState != STATE_EVADE && m_AIState != STATE_FEAR 
            && m_AIState != STATE_WANDER && m_AIState != STATE_SCRIPTIDLE)
        {
            if(firstLeaveCombat)
            {
                Unit* target = FindTarget();
                if(target)
                {
                    AttackReaction(target, 1, 0);
                }else
                {
                    firstLeaveCombat = false;
                }
            }
            /*else
            {
                HandleEvent(EVENT_LEAVECOMBAT, m_Unit, 0);
            }*/
        }
        else if(m_aiTargets.size() == 0 && m_AIType == AITYPE_PET && static_cast<Pet*>(m_Unit)->GetPetState() == PET_STATE_AGGRESSIVE)
        {
             Unit* target = FindTarget();
             if(target)
                 AttackReaction(target, 1, 0);

        }
    }
    // Find new Targets when we are ooc
    if((m_AIState == STATE_IDLE || m_AIState == STATE_SCRIPTIDLE) && m_assistTargets.size() == 0)
    {
        Unit* target = FindTarget();
        if(target)
        {
            AttackReaction(target, 1, 0);
        }
    }
}

///====================================================================
///  Desc: Updates Combat Status of m_Unit
///====================================================================
void AIInterface::_UpdateCombat(uint32 p_time)
{
    uint16 agent = m_aiCurrentAgent;
    
    /*if(m_AIType == AITYPE_PET)                  // pets - default spell
    {
        if(m_DefaultSpell != NULL)
        {
            if(m_DefaultSpell != m_nextSpell)
            {
                m_nextSpell = m_DefaultSpell;
            }
        }
    }*/ // no need to do this anymore

    // If creature is very far from spawn point return to spawnpoint
    // If at instance dont return -- this is wrong ... (mmcs) instance creatures always returns to spawnpoint, dunno how do you got this ideia. 

    if(m_AIType != AITYPE_PET 
        && (m_Unit->GetDistanceSq(m_returnX,m_returnY,m_returnZ) > m_outOfCombatRange/*oocr*/) 
        && m_AIState != STATE_EVADE
        && m_AIState != STATE_SCRIPTMOVE)
    {
        HandleEvent(EVENT_LEAVECOMBAT, m_Unit, 0);
    }
    else if(!m_nextTarget && m_AIState != STATE_FOLLOWING && m_AIState != STATE_SCRIPTMOVE)
    {
        m_nextTarget = FindTargetForSpell(m_nextSpell);
        if(!m_nextTarget)
        {
            if(m_Unit->GetMapMgr())
            {
                if(m_Unit->GetMapMgr()->GetMapInfo())
                {
                    if(m_Unit->GetMapMgr()->GetMapInfo()->type == 0)
                    {
                        HandleEvent(EVENT_LEAVECOMBAT, m_Unit, 0);
                    }
                    else
                        m_AIState = STATE_IDLE;
                }
                else
                    HandleEvent(EVENT_LEAVECOMBAT, m_Unit, 0);
            }
            else
            {
                HandleEvent(EVENT_LEAVECOMBAT, m_Unit, 0);
            }
        }
    }

    bool cansee;
    if(m_nextTarget)
        cansee=((Creature*)m_Unit)->CanSee(m_nextTarget);
    else 
        cansee=false;

    if(cansee && m_nextTarget && m_nextTarget->isAlive() && m_AIState != STATE_EVADE && !m_Unit->isCasting())
    {
        if(agent == AGENT_NULL)
        {
            if(m_canFlee && !m_hasFleed 
                && ((m_Unit->GetUInt32Value(UNIT_FIELD_HEALTH) / m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH)) < m_FleeHealth ))
                agent = AGENT_FLEE;
            else if(m_canCallForHelp 
                && !m_hasCalledForHelp 
                && (m_CallForHelpHealth > (m_Unit->GetUInt32Value(UNIT_FIELD_HEALTH) / (m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH) > 0 ? m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH) : 1))))
                agent = AGENT_CALLFORHELP;
            else if(m_nextSpell)
            {
                if(m_nextSpell->agent != AGENT_NULL)
                {
                    agent = m_nextSpell->agent;
                }
                else
                {
                    agent = AGENT_MELEE;
                }
            }
            else
            {
                agent = AGENT_MELEE;
            }
        }
        if(agent == AGENT_RANGED || agent == AGENT_MELEE)
        {
            if(m_canRangedAttack)
            {
                agent = AGENT_MELEE;
                if(m_nextTarget->GetTypeId() == TYPEID_PLAYER)
                {
                    float dist = m_Unit->GetDistanceSq(m_nextTarget);
                    if(((Player*)m_nextTarget)->m_currentMovement == MOVE_ROOT || dist >= 64.0f)
                    {
                        agent =  AGENT_RANGED;
                    }
                }
                else if(m_nextTarget->m_canMove == false || m_Unit->GetDistanceSq(m_nextTarget) >= 64.0f)
                {
                   agent = AGENT_RANGED;
                }
            }
            else
            {
                agent = AGENT_MELEE;
            }
        }
  
        switch(agent)
        {
        case AGENT_MELEE:
            {
                float combatReach[2]; // Calculate Combat Reach
                float distance = m_Unit->CalcDistance(m_nextTarget);

                combatReach[0] = 0.0f;
                combatReach[1] = _CalcCombatRange(m_nextTarget, false);

                if(distance >= combatReach[0] && distance <= combatReach[1]) // Target is in Range -> Attack
                {
                    if(UnitToFollow != NULL)
                    {
                        UnitToFollow = NULL; //we shouldn't be following any one
                        m_lastFollowX = m_lastFollowY = 0;
                        m_Unit->setAttackTarget(NULL);  // remove ourselves from any target that might have been followed
                    }
                    
                    FollowDistance = 0.0f;
                    m_moveRun = false;
                    //FIXME: offhand shit
                    if(m_Unit->isAttackReady(false) && !m_fleeTimer)
                    {
                        m_creatureState = ATTACKING;
                        bool infront = m_Unit->isInFront(m_nextTarget);

                        if(!infront) // set InFront
                        {
                            //prevent mob from rotating while stunned
                            if(!m_Unit->IsStunned ())
                            {
                                setInFront(m_nextTarget);
                                infront = true;
                            }                            
                        }
                        if(infront)
                        {
                            m_Unit->setAttackTimer(0, false);
                            m_Unit->Strike(m_nextTarget, (agent==AGENT_MELEE)?0:2,NULL,0,0,0);
                        }
                    }
                }
                else // Target out of Range -> Run to it
                {
                    //calculate next move
                    float dist = 1.0f;
                    dist = combatReach[1]-1.15f;
                    m_moveRun = true;
                    _CalcDestinationAndMove(m_nextTarget, dist);
                }
            }break;
        case AGENT_RANGED:
            {
                float combatReach[2]; // Calculate Combat Reach
                float distance = m_Unit->CalcDistance(m_nextTarget);

                combatReach[0] = 8.0f;
                combatReach[1] = 30.0f;

                if(distance >= combatReach[0] && distance <= combatReach[1]) // Target is in Range -> Attack
                {
                    if(UnitToFollow != NULL)
                    {
                        UnitToFollow = NULL; //we shouldn't be following any one
                        m_lastFollowX = m_lastFollowY = 0;
                        m_Unit->setAttackTarget(NULL);  // remove ourselves from any target that might have been followed
                    }
                    
                    FollowDistance = 0.0f;
                    m_moveRun = false;
                    //FIXME: offhand shit
                    if(m_Unit->isAttackReady(false) && !m_fleeTimer)
                    {
                        m_creatureState = ATTACKING;
                        bool infront = m_Unit->isInFront(m_nextTarget);

                        if(!infront) // set InFront
                        {
                            //prevent mob from rotating while stunned
                            if(!m_Unit->IsStunned ())
                            {
                                setInFront(m_nextTarget);
                                infront = true;
                            }                            
                        }

                        if(infront)
                        {
                            m_Unit->setAttackTimer(0, false);
                            SpellEntry *info = sSpellStore.LookupEntry(SPELL_RANGED_BOW);
                            if(info)
                            {
                                Spell *sp = new Spell(m_Unit, info, false, NULL);
                                SpellCastTargets targets;
                                targets.m_unitTarget = m_nextTarget->GetGUID();
                                sp->prepare(&targets);
                                //Lets make spell handle this
                                //m_Unit->Strike(m_nextTarget, (agent==AGENT_MELEE)?0:2,NULL,0,0,0);
                            }
                        }
                    }
                }
                else // Target out of Range -> Run to it
                {
                    //calculate next move
                    float dist = 1.0f;

                    if(distance < combatReach[0])// Target is too near
                        dist = 9.0f;
                    else
                        dist = 20.0f;

                    m_moveRun = true;
                    _CalcDestinationAndMove(m_nextTarget, dist);
                }
            }break;
        case AGENT_SPELL:
            {
                if(!m_nextSpell || !m_nextTarget)
                    return;  // this shouldnt happen

                float distance = m_Unit->GetDistanceSq(m_nextTarget);
                if((distance <= powf(m_nextSpell->maxrange,2)  && distance >= powf(m_nextSpell->minrange,2)) || m_nextSpell->maxrange == 0) // Target is in Range -> Attack
                {
                    uint32 spellId = m_nextSpell->spellId;
                    SpellEntry* spellInfo = getSpellEntry(spellId);
                    SpellCastTargets targets = setSpellTargets(spellInfo, m_nextTarget);
                    CastSpell(m_Unit, spellInfo, targets);
                    increaseProcCounter(m_nextSpell->procEvent, m_nextSpell);
                    m_nextSpell = NULL;

                    if(m_AIType == AITYPE_PET)      // Add cooldown
                        ((Pet*)m_Unit)->AddSpellCooldown(spellId);
                }
                else // Target out of Range -> Run to it
                {
                    //calculate next move
                    m_moveRun = true;
                    _CalcDestinationAndMove(m_nextTarget, m_nextSpell->maxrange - 5.0f);
                    /*Destination* dst = _CalcDestination(m_nextTarget, dist);
                    MoveTo(dst->x, dst->y, dst->z,0);
                    delete dst;*/
                }
            }break;
        case AGENT_FLEE:
            {
                //float dist = 5.0f;

                m_moveRun = false;
                if(m_fleeTimer == 0)
                    m_fleeTimer = m_FleeDuration;

                /*Destination* dst = _CalcDestination(m_nextTarget, dist);
                MoveTo(dst->x, dst->y, dst->z,0);
                delete dst;*/
                _CalcDestinationAndMove(m_nextTarget, 5.0f);
                if(!m_hasFleed) // to avoid lua excuting spam
                    CALL_SCRIPT_EVENT(m_Unit, OnFlee)(m_nextTarget);

                m_AIState = STATE_FLEEING;
                m_nextTarget = m_Unit;

                m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
                m_Unit->setAttackTarget(0);
                m_Unit->clearAttackers(true);

                // Send message
                //char msg[200];
                //sprintf(msg, "\%s attempts to run away in fear!");

                WorldPacket data(SMSG_MESSAGECHAT, 100);
                string msg = "%s attempts to run away in fear!";
                data << (uint8)CHAT_MSG_MONSTER_EMOTE;
                data << (uint32)LANG_UNIVERSAL;
                data << (uint32)(m_Unit->GetCreatureName()->Name.size() + 1);
                data << m_Unit->GetCreatureName()->Name;
                data << (uint64)0;
                data << (uint32)(msg.size() + 1);
                data << msg;
                data << uint8(0);

                m_Unit->SendMessageToSet(&data, false);

                //m_Unit->SendChatMessage(CHAT_MSG_MONSTER_EMOTE, LANG_UNIVERSAL, msg);
                //sChatHandler.FillMessageData(&data, CHAT_MSG_MONSTER_EMOTE, LANG_UNIVERSAL, msg, m_Unit->GetGUID());               
               
                m_hasFleed = true;
            }break;
        case AGENT_CALLFORHELP:
            {
                FindFriends(400.0f/*20.0f*/);
                m_hasCalledForHelp = true; // We only want to call for Help once in a Fight.
                CALL_SCRIPT_EVENT(m_Unit, OnCallForHelp)();
            }break;
        }
    }
    else  if(!m_nextTarget || !m_nextTarget->isAlive() || !cansee)
    {
        m_nextTarget = NULL;
        m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);

        // no more target
        //m_Unit->setAttackTarget(NULL);
    }
}

void AIInterface::DismissPet()
{
    //BURLEXFIXME
    /*
    if(m_AIType != AITYPE_PET)
        return;

    if(!m_PetOwner)
        return;
    
    if(m_PetOwner->GetTypeId() != TYPEID_PLAYER)
        return;

    if(m_Unit->GetUInt32Value(UNIT_CREATED_BY_SPELL) == 0)
        ((Player*)m_PetOwner)->SetFreePetNo(false, (int)m_Unit->GetUInt32Value(UNIT_FIELD_PETNUMBER));
    ((Player*)m_PetOwner)->SetPet(NULL);
    ((Player*)m_PetOwner)->SetPetName("");
    
    //FIXME:Check hunter pet or not
    //FIXME:Check enslaved creature
    m_PetOwner->SetUInt64Value(UNIT_FIELD_SUMMON, 0);
    
    WorldPacket data;
    data.Initialize(SMSG_PET_SPELLS);
    data << (uint64)0;
    ((Player*)m_PetOwner)->GetSession()->SendPacket(&data);
    
    sEventMgr.RemoveEvents(((Creature*)m_Unit));
    if(m_Unit->IsInWorld())
    {
        m_Unit->RemoveFromWorld();
    }
    //setup an event to delete the Creature
    sEventMgr.AddEvent(((Creature*)this->m_Unit), &Creature::DeleteMe, EVENT_DELETE_TIMER, 1, 1);*/
}

void AIInterface::AttackReaction(Unit* pUnit, uint32 damage_dealt, uint32 state, uint32 spellId)
{
    if(pUnit->GetTypeId() == TYPEID_PLAYER &&
        m_Unit->GetMapMgr() && m_Unit->GetMapMgr()->GetUpdaterThread() &&
        m_Unit->GetTypeId() == TYPEID_UNIT && !m_Unit->_Activated &&
        UINT32_LOPART(m_Unit->GetGUIDHigh()) != HIGHGUID_PET)
    {
        // we must be in an active cell to be attacked..
        m_Unit->GetMapMgr()->GetUpdaterThread()->AddObject<Creature>(static_cast<Creature*>(m_Unit));
        m_Unit->_Activated = true;
    }
    else
    {
        m_Unit->_Activated = false;
    }

    if(m_AIState == STATE_EVADE || m_fleeTimer != 0 || !pUnit 
        || m_Unit->IsPacified() || m_Unit->IsStunned() || !m_Unit->isAlive())
    {
        return;
    }
    if(m_Unit == pUnit)
    {
        return;
    }
    
    bool state_reduced = false;
    if(state >= 0xFF)
    {
        state -= 0xFF;
        state_reduced = true;
    }

    // this isn't right if we're dealing damage.. we dont want to add threat for when we're attacking someone? or do we?
    //modThreatByGUID(pUnit->GetGUID(), _CalcThreat(damage_dealt, spellId, pUnit));

    if(m_AIState == STATE_IDLE || m_AIState == STATE_FOLLOWING)
    {
        WipeTargetList();
        
        HandleEvent(EVENT_ENTERCOMBAT, pUnit, 0);
        if(pUnit->GetTypeId() == TYPEID_PLAYER)
        {
            // Show attack animation icon
            //((Player*)pUnit)->SetFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_ATTACK_ANIMATION);
        //    if(((Player*)pUnit)->GetUInt64Value(PLAYER_SELECTION) == 0)
          //      ((Player*)pUnit)->SetUInt64Value(PLAYER_SELECTION, m_Unit->GetGUID());
        //Disabled by Phantomas -> this is wrong we should not set player selection
        }

        //these 2 are done already in deal damage
        //right before this func is called
        //there should be a way not to do this twice....

        // no need to do this
        /*if(pUnit != m_Unit)     // this shouldnt happen
        {
            pUnit->AddAttacker(m_Unit);
            m_Unit->m_AttackTarget = pUnit;
        } else {
            char msg[100];
            sprintf(msg, "AI: Tried to add self to attacker set "I64FMT, m_Unit->GetGUID());
            sWorld.SendIRCMessage(msg);
            sLog.outString(msg);
        }*/
    }

    switch(state)
    {
    case 1: // Target dodged
        {
            HandleEvent(EVENT_TARGETDODGED, pUnit, 0);
        }break;
    case 2: // Target blocked
        {
            HandleEvent(EVENT_TARGETBLOCKED, pUnit, 0);
        }break;
    case 3: // Target parryed
        {
            HandleEvent(EVENT_TARGETPARRYED, pUnit, 0);
        }break;
    case 5: // Unit critted
        {
            HandleEvent(EVENT_UNITCRITHIT, pUnit, 0);
        }break;
    }
    if(!state_reduced)
    {
        HandleEvent(EVENT_DAMAGETAKEN, pUnit, _CalcThreat(damage_dealt, spellId, pUnit));
    }

    //burlex: WHAT the fuck is this for??!!!?!!
    /*// check for a Pet/Summon owner and add him to Targetlist
    if(pUnit->GetUInt64Value(UNIT_FIELD_SUMMONEDBY))
    {
        Unit* petOwner = NULL;
        petOwner = sObjHolder.GetObject<Player>(pUnit->GetUInt64Value(UNIT_FIELD_SUMMONEDBY));
        if(!petOwner)
        {
            petOwner = sObjHolder.GetObject<Creature>(pUnit->GetUInt64Value(UNIT_FIELD_SUMMONEDBY));
        }
        if(!petOwner)
        {
            return;
        }
        AttackReaction(petOwner, 1, 0);
    }*/
}

bool AIInterface::HealReaction(Unit* caster, Unit* victim, uint32 amount)
{
    if(!caster || !victim)
    {
        printf("!!!BAD POINTER IN AIInterface::HealReaction!!!\n");
        return false;
    }

    int casterInList = 0, victimInList = 0;

    if(m_aiTargets.find(caster) != m_aiTargets.end())
        casterInList = 1;

    if(m_aiTargets.find(victim) != m_aiTargets.end())
        victimInList = 1;

    /*for(i = m_aiTargets.begin(); i != m_aiTargets.end(); i++)
    {
        if(casterInList && victimInList)
        { // no need to check the rest, just break that
            break;
        }
        if(i->target == victim)
        {
            victimInList = true;
        }
        if(i->target == caster)
        {
            casterInList = true;
        }
    }*/
    if(!victimInList && !casterInList) // none of the Casters is in the Creatures Threat list
    {
        return false;
    }
    if(!casterInList && victimInList) // caster is not yet in Combat but victim is
    {
        // get caster into combat if he's hostile
        if(isHostile(m_Unit, caster))
        {
            //AI_Target trgt;
            //trgt.target = caster;
            //trgt.threat = amount;
            //m_aiTargets.push_back(trgt);
            m_aiTargets.insert(TargetMap::value_type(caster, amount));
            return true;
        }
        return false;
    }
    else if(casterInList && victimInList) // both are in combat already
    {
        // mod threat for caster
        modThreatByPtr(caster, amount);
        return true;
    }
    else // caster is in Combat already but victim is not
    {
        modThreatByPtr(caster, amount);
        // both are players so they might be in the same group
        if(caster->GetTypeId() == TYPEID_PLAYER && victim->GetTypeId() == TYPEID_PLAYER)
        {
            if(((Player*)caster)->GetGroup() == ((Player*)victim)->GetGroup())
            {
                // get victim into combat since they are both
                // in the same party
                if(isHostile(m_Unit, victim))
                {
                    m_aiTargets.insert(TargetMap::value_type(victim, 1));
                    return true;
                }
                return false;
            }
        }
    }

    return false;
}

void AIInterface::OnDeath(Object* pKiller)
{
    if(pKiller->GetTypeId() == TYPEID_PLAYER || pKiller->GetTypeId() == TYPEID_UNIT)
        HandleEvent(EVENT_UNITDIED, static_cast<Unit*>(pKiller), 0);
    else
        HandleEvent(EVENT_UNITDIED, m_Unit, 0);
}

Unit* AIInterface::FindTarget()
{// find nearest hostile Target to attack
    
    if( !m_AllowedToEnterCombat ) 
        return NULL;
    CreatureInfo* ci = m_Unit->GetCreatureName();
    if(!ci) 
        return NULL;
    Unit* target = NULL;
    Unit* critterTarget = NULL;
    float distance = 999999.0f; // that should do it.. :p

    std::set<Object*>::iterator itr, it2;
    Object *pObj;
    Unit *pUnit;
    float dist;

    for( itr = m_Unit->GetInRangeOppFactsSetBegin(); itr != m_Unit->GetInRangeOppFactsSetEnd(); )
    {
        it2 = itr;
        ++itr;

        pObj = (*it2);
        if(pObj->GetTypeId() == TYPEID_PLAYER)
        {
            if(static_cast<Player*>(pObj)->GetTaxiState())      // skip players on taxi
                continue;
        }
        else if(pObj->GetTypeId() != TYPEID_UNIT)
                continue;
        
        pUnit = static_cast<Unit*>(pObj);
        
        if(pUnit->m_invisible) // skip invisible units
            continue;
        
        if(!pUnit->isAlive()
            || m_Unit == pUnit /* wtf? */
            || m_Unit->GetUInt64Value(UNIT_FIELD_CREATEDBY) == pUnit->GetGUID())
            continue;

        dist = m_Unit->GetDistanceSq(pUnit);
        if(!pUnit->m_faction || !pUnit->m_factionDBC)
            continue;

        if(pUnit->m_faction->FactionId == 28)// only Attack a critter if there is no other Enemy in range
        {
            if(dist < 225.0f)    // was 10
                critterTarget = pUnit;
            continue;
        }

        if(dist > distance)     // we want to find the CLOSEST target
            continue;

        if(dist <= _CalcAggroRange(pUnit) )
        {
            distance = dist;
            target = pUnit;
        }
    }

    if( !target )
    {
        target = critterTarget;
    }

    if( target )
    {
        AttackReaction(target, 1, 0);
        if(target->GetUInt32Value(UNIT_FIELD_CREATEDBY) != 0)
        {
            Unit* target2 = sObjHolder.GetObject<Player>(target->GetUInt32Value(UNIT_FIELD_CREATEDBY));
            if(!target2)
            {
                target2 = sObjHolder.GetObject<Player>(target->GetUInt32Value(UNIT_FIELD_CREATEDBY));
            }
            if(target2)
            {
                AttackReaction(target2, 1, 0);
            }
        }
    }
    return target;
}

Unit* AIInterface::FindTargetForSpell(AI_Spell *sp)
{
    if(!sp)
    {
        m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
        return NULL;
    }

    TargetMap::iterator itr, itr2;

    if(sp->spellType == STYPE_HEAL)
    {
        uint32 cur = m_Unit->GetUInt32Value(UNIT_FIELD_HEALTH);
        uint32 max = m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
        float healthPercent = float(long2float(cur) / long2float(max));
        if(healthPercent <= sp->floatMisc1) // Heal ourselves cause we got too low HP
        {
            m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
            return m_Unit;
        }
        for(AssistTargetSet::iterator i = m_assistTargets.begin(); i != m_assistTargets.end(); i++)
        {
            if(!(*i)->isAlive())
            {
                continue;
            }
            cur = (*i)->GetUInt32Value(UNIT_FIELD_HEALTH);
            max = (*i)->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
            healthPercent = float(long2float(cur) / long2float(max));
            if(healthPercent <= sp->floatMisc1) // Heal ourselves cause we got too low HP
            {
                m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, (*i)->GetGUID());
                return (*i); // heal Assist Target which has low HP
            }
        }
    }
    if(sp->spellType == STYPE_BUFF)
    {
        m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
        return m_Unit;
    }


    Unit* target = NULL;
    uint32 threat = 0;
    //modified for vs2005 compatibility

    if(GetIsTaunted())
    {
        m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, tauntedBy->GetGUID());
        return tauntedBy;
    }

    for (itr = m_aiTargets.begin(); itr != m_aiTargets.end();) // Find Target and Cleanup Targetlist
    {
        itr2 = itr;
        ++itr;
        uint32 curThreat = 1;
        if((itr2->second + itr2->first->GetThreatModifyer()) > 0) // threat may be < 0 due to some modifyers.
        {
            curThreat = (itr2->second + itr2->first->GetThreatModifyer());
        }
        if(curThreat > threat)
        {
            if(itr2->first->isAlive())
            {
                target = itr2->first;
                threat = curThreat;
            }
            else
            {
                m_aiTargets.erase(itr2);
            }
        }
    }
    if(target)
    {
        m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, target->GetGUID());
        return target;
    }

    return NULL;
}

bool AIInterface::FindFriends(float dist)
{
    bool result = false;
    TargetMap::iterator it;

    std::set<Object*>::iterator itr;
    Unit *pUnit;

    for( itr = m_Unit->GetInRangeSetBegin(); itr != m_Unit->GetInRangeSetEnd(); itr++ )
    {
        if((*itr)->GetTypeId() != TYPEID_UNIT)
            continue;

        pUnit = static_cast<Unit*>((*itr));
        if(!pUnit->isAlive())
            continue;

        if(isCombatSupport(m_Unit, pUnit) 
            && (pUnit->GetAIInterface()->getAIState() == STATE_IDLE || pUnit->GetAIInterface()->getAIState() == STATE_SCRIPTIDLE))//Not sure
        {
            if(m_Unit->GetDistanceSq(pUnit) < dist)
            {
                if(m_assistTargets.count(pUnit) > 0)    // already have him
                    break;

                result = true;
                    
                //addAssistTargets(pUnit);
                m_assistTargets.insert(pUnit);
                    
                for(it = m_aiTargets.begin(); it != m_aiTargets.end(); ++it)
                {
                    ((Unit*)(*itr))->GetAIInterface()->AttackReaction(it->first, 1, 0);
                }
            }
        }
    }

    // check if we're a civillan, in which case summon guards on a despawn timer
    /*if(m_Unit->GetCreatureName() && m_Unit->GetCreatureName()->Civilian && getMSTime() > m_guardTimer && m_Unit->GetMapId() < 2)
    {
        switch(m_Unit->GetCreatureName()->Family)
        {
        case UNDEAD:
        case HUMANOID:
            break;
        default:
            return result;
            break;
        }

        m_Unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "Guards, help me!");
        
        uint32 guardid = 1423;
        if(m_Unit->m_factionDBC->factionGroupReputationFrame == 67) // horde
            guardid = 3296; // ogri
        CreatureSpawnTemplate * pSpawnTemplate = objmgr.GetCreatureSpawnTemplate(guardid);
        if(pSpawnTemplate != 0)
        {
            Creature *pCreature = sObjHolder.Create<Creature>();
            // FIXME: Summon hardcoded stormwind guard/ogrimmar grunt
            float x = m_Unit->GetPositionX() + (float)( (float)(rand() % 150 + 100) / 1000.0f );
            float y = m_Unit->GetPositionY() + (float)( (float)(rand() % 150 + 100) / 1000.0f );
            float z = m_Unit->GetPositionZ();
            float adt_z = m_Unit->GetMapMgr()->GetLandHeight(x, y);
            if(fabs(z - adt_z) < 3)
                z = adt_z;

            pCreature->Create(pSpawnTemplate, m_Unit->GetMapId(), x, y, z, 0);
            pCreature->SetInstanceID(m_Unit->GetInstanceID());
            pCreature->SetZoneId(m_Unit->GetZoneId());
            
            pCreature->AddToWorld();

            // set despawn timer
            // at 5mins
            sEventMgr.AddEvent(pCreature, &Creature::Despawn, EVENT_CREATURE_SAFE_DELETE, 60*5*1000, 1);

            for(it = m_aiTargets.begin(); it != m_aiTargets.end(); ++it)
            {
                pCreature->GetAIInterface()->AttackReaction(it->first, 1, 0);
            }
        }

        m_guardTimer = getMSTime() + 15000;
    }*/

    return result;
}

float AIInterface::_CalcAggroRange(Unit* target)
{
    //float baseAR = 15.0f; // Base Aggro Range
                    // -8     -7     -6     -5     -4     -3     -2     -1     0      +1     +2     +3     +4     +5     +6     +7    +8
    //float baseAR[17] = {29.0f, 27.5f, 26.0f, 24.5f, 23.0f, 21.5f, 20.0f, 18.5f, 17.0f, 15.5f, 14.0f, 12.5f, 11.0f,  9.5f,  8.0f,  6.5f, 5.0f};
    float baseAR[17] = {19.0f, 18.5f, 18.0f, 17.5f, 17.0f, 16.5f, 16.0f, 15.5f, 15.0f, 14.5f, 12.0f, 10.5f, 8.5f,  7.5f,  6.5f,  6.5f, 5.0f};
    // Lvl Diff -8 -7 -6 -5 -4 -3 -2 -1 +0 +1 +2  +3  +4  +5  +6  +7  +8
    // Arr Pos   0  1  2  3  4  5  6  7  8  9 10  11  12  13  14  15  16
    int8 lvlDiff = target->getLevel() - m_Unit->getLevel();
    uint8 realLvlDiff = lvlDiff;
    if(lvlDiff > 8)
    {
        lvlDiff = 8;
    }
    if(lvlDiff < -8)
    {
        lvlDiff = -8;
    }
    if(!((Creature*)m_Unit)->CanSee(target))
        return 0;
    
    float AggroRange = baseAR[lvlDiff + 8];
    if(realLvlDiff > 8)
    {
        AggroRange += AggroRange * ((lvlDiff - 8) * 5 / 100);
    }

    // Multiply by elite value
    if(m_Unit->GetCreatureName()->Rank > 0)
        AggroRange *= (m_Unit->GetCreatureName()->Rank) * 2;

    if(AggroRange > 40.0f) // cap at 40.0f
    {
        AggroRange = 40.0f;
    }
  /*  //printf("aggro range: %f , stealthlvl: %d , detectlvl: %d\n",AggroRange,target->GetStealthLevel(),m_Unit->m_stealthDetectBonus);
    if(! ((Creature*)m_Unit)->CanSee(target))
    {
        AggroRange =0;
    //    AggroRange *= ( 100.0f - (target->m_stealthLevel - m_Unit->m_stealthDetectBonus)* 20.0f ) / 100.0f;
    }
*/
    // SPELL_AURA_MOD_DETECT_RANGE
    int32 modDetectRange = target->getDetectRangeMod(m_Unit->GetGUID());
    AggroRange += modDetectRange;
    if(AggroRange < 3.0f)
    {
        AggroRange = 3.0f;
    }
    if(AggroRange > 40.0f) // cap at 40.0f
    {
        AggroRange = 40.0f;
    }

    return powf(AggroRange, 2);
}

void AIInterface::_CalcDestinationAndMove(Unit *target, float dist)
{
    if(!m_canMove || m_Unit->IsStunned())
    {
        StopMovement(0); //Just Stop
        return;
    }
    
    if(target->GetTypeId() == TYPEID_UNIT || target->GetTypeId() == TYPEID_PLAYER)
    {
        float angle = m_Unit->calcAngle(m_Unit->GetPositionX(), m_Unit->GetPositionY(), target->GetPositionX(), target->GetPositionY()) * M_PI / 180.0f;

        float x = dist * cosf(angle);
        float y = dist * sinf(angle);

        m_nextPosX = target->GetPositionX() - x;
        m_nextPosY = target->GetPositionY() - y;
        m_nextPosZ = target->GetPositionZ();
    }
    else
    {
        target = NULL;
        m_nextPosX = m_Unit->GetPositionX();
        m_nextPosY = m_Unit->GetPositionY();
        m_nextPosZ = m_Unit->GetPositionZ();
    }

    float dx = m_nextPosX - m_Unit->GetPositionX();
    float dy = m_nextPosY - m_Unit->GetPositionY();
    if(dy != 0.0f)
    {
        float angle = atan2(dx, dy);
        m_Unit->SetOrientation(angle);
    }

    if(m_creatureState != MOVING)
        UpdateMove();
}

float AIInterface::_CalcCombatRange(Unit* target, bool ranged)
{
    if(!target)
    {
        return 0;
    }
    float range = 0.0f;
    float rang = 1.5f;
    if(ranged)
    {
        rang = 5.0f;
    }
//    float targetreach = target->GetFloatValue(UNIT_FIELD_COMBATREACH);
    float selfreach = m_Unit->GetFloatValue(UNIT_FIELD_COMBATREACH);
    float targetradius = target->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS);
    float selfradius = m_Unit->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS);
    float targetscale = target->GetFloatValue(OBJECT_FIELD_SCALE_X);
    float selfscale = m_Unit->GetFloatValue(OBJECT_FIELD_SCALE_X);

    range = (((powf(targetradius,2)*targetscale) + selfreach) + ((powf(selfradius,2)*selfscale) + rang));
    return range;
}

float AIInterface::_CalcDistanceFromHome()
{
    if (m_AIType == AITYPE_PET)
    {
        return m_Unit->GetDistanceSq(m_PetOwner);
    }
    else if(m_Unit->GetTypeId() == TYPEID_UNIT)
    {

        if(m_returnX !=0.0f && m_returnY != 0.0f)
        {
            return m_Unit->GetDistanceSq(m_returnX,m_returnY,m_returnZ);
        }
    }

    return 0.0f;
}

void AIInterface::SendMoveToPacket(float toX, float toY, float toZ, float toO, uint32 time, uint32 MoveFlags)
{
    //this should NEVER be called directly !!!!!!
    //use MoveTo()

    /*
    Move Flags
    0x00000000 - Walk
    0x00000100 - Run
    0x00000300 - Fly
    */
    WorldPacket data(SMSG_MONSTER_MOVE, 50);   // 50 bytes should be more than enough
    data << m_Unit->GetNewGUID();
    data << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
    data << getMSTime();
    
    // Check if we have an orientation
    if(toO != 0.0f)
    {
        data << uint8(4);
        data << toO;
    } else {
        data << uint8(0);
    }
    data << MoveFlags;
    data << time;
    data << uint32(1);      // 1 waypoint
    data << toX << toY << toZ;

    /*WorldPacket data;

    uint8 DontMove = 0;
    data.Initialize( SMSG_MONSTER_MOVE );
    data << m_Unit->GetNewGUID();
    data << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
    data << getMSTime();
    data << uint8(DontMove);
    if(DontMove != 1)
    {
        //data << uint32(run ? 0x00000100 : 0x00000000);
        data << uint32(MoveFlags);
        data << time;
        
        //if(toO != 0.0f)
        //{
        //data << uint32(2);
        //}
        //else
        //{
        data << uint32(1); //Number of Waypoints
        //}
        data << toX << toY << toZ;
        //if(toO != 0.0f)
        //{
        //data << toO;
        //}

    }*/
    if(m_Unit->GetTypeId() == TYPEID_PLAYER)
        m_Unit->SendMessageToSet( &data, true );
    else
        m_Unit->SendMessageToSet( &data, false );
}

/*
void AIInterface::SendMoveToSplinesPacket(std::list<Waypoint> wp, bool run)
{
    if(!m_canMove)
    {
        return;
    }

    WorldPacket data;

    uint8 DontMove = 0;
    uint32 travelTime = 0;
    for(std::list<Waypoint>::iterator i = wp.begin(); i != wp.end(); i++)
    {
        travelTime += i->time;
    }

    data.Initialize( SMSG_MONSTER_MOVE );
    data << m_Unit->GetNewGUID();
    data << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
    data << getMSTime();
    data << uint8(DontMove);
    data << uint32(run ? 0x00000100 : 0x00000000);
    data << travelTime;
    data << (uint32)wp.size();
    for(std::list<Waypoint>::iterator i = wp.begin(); i != wp.end(); i++)
    {
        data << i->x;
        data << i->y;
        data << i->z;
    }

    m_Unit->SendMessageToSet( &data, false );
}
*/
bool AIInterface::StopMovement(uint32 time)
{
    m_moveTimer = time; //set pause after stopping
    m_creatureState = STOPPED;

    m_destinationX = m_destinationY = m_destinationZ = 0;
    m_nextPosX = m_nextPosY = m_nextPosZ = 0;
    m_timeMoved = 0;
    m_timeToMove = 0;

    /*WorldPacket data;
    uint8 DontMove = 1;
    data.Initialize( SMSG_MONSTER_MOVE );
    data << m_Unit->GetNewGUID();
    data << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
    data << getMSTime();
    data << uint8(DontMove);*/

    WorldPacket data(26);
    data.SetOpcode(SMSG_MONSTER_MOVE);
    data << m_Unit->GetNewGUID();
    data << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
    data << getMSTime();
    data << uint8(1);   // "DontMove = 1"

    m_Unit->SendMessageToSet( &data, false );
    return true;
}

void AIInterface::MoveTo(float x, float y, float z, float o)
{
    m_sourceX = m_Unit->GetPositionX();
    m_sourceY = m_Unit->GetPositionY();
    m_sourceZ = m_Unit->GetPositionZ();

    if(!m_canMove || m_Unit->IsStunned())
    {
        StopMovement(0); //Just Stop
        return;
    }

    m_nextPosX = x;
    m_nextPosY = y;
    m_nextPosZ = z;

    if(m_creatureState != MOVING)
        UpdateMove();
}

bool AIInterface::IsFlying()
{
    if(m_moveFly)
        return true;
    
    /*float z = m_Unit->GetMapMgr()->GetLandHeight(m_Unit->GetPositionX(), m_Unit->GetPositionY());
    if(z)
    {
        if(m_Unit->GetPositionZ() >= (z + 1.0f)) //not on ground? Oo
        {
            return true;
        }
    }
    return false;*/
    if(m_Unit->GetTypeId() == TYPEID_PLAYER)
        return ((Player*)m_Unit)->FlyCheat;

    return false;
}

uint32 AIInterface::getMoveFlags()
{
    uint32 MoveFlags = 0;
    if(m_moveFly == true) //Fly
    {
        m_moveSpeed = m_Unit->m_flySpeed*0.001f;
        MoveFlags = 0x300;
    }
    else if(m_moveSprint == true) //Sprint
    {
        m_moveSpeed = (m_Unit->m_runSpeed+5.0f)*0.001f;
        MoveFlags = 0x100;
    }
    else if(m_moveRun == true) //Run
    {
        m_moveSpeed = m_Unit->m_runSpeed*0.001f;
        MoveFlags = 0x100;
    }
    else //Walk
    {
        m_moveSpeed = m_Unit->m_walkSpeed*0.001f;
        MoveFlags = 0x000;
    }
    return MoveFlags;
}

void AIInterface::UpdateMove()
{
    //this should NEVER be called directly !!!!!!
    //use MoveTo()
    // burlex: not sure what to do on this one.. :/
    float distance = m_Unit->CalcDistance(m_nextPosX,m_nextPosY,m_nextPosZ);
    
    if(distance < 1.0f) return; //we don't want little movements here and there

    m_destinationX = m_nextPosX;
    m_destinationY = m_nextPosY;
    m_destinationZ = m_nextPosZ;
    
    if(m_moveFly != true)
    {
        if(m_Unit->GetMapMgr())
        {
            float adt_Z = m_Unit->GetMapMgr()->GetLandHeight(m_destinationX, m_destinationY);
            if(fabsf(adt_Z - m_destinationZ) < 1.5)
                m_destinationZ = adt_Z;
        }
    }
    m_nextPosX = m_nextPosY = m_nextPosZ = 0;

    uint32 moveTime = (uint32) (distance / m_moveSpeed);
    m_totalMoveTime = moveTime;

    if(m_Unit->GetTypeId() == TYPEID_UNIT)
    {
        Creature *creature = static_cast<Creature*>(m_Unit);
        // check if we're returning to our respawn location. if so, reset back to default
        // orientation
        if(creature->respawn_cord[0] == m_destinationX &&
            creature->respawn_cord[1] == m_destinationY)
        {
            creature->SetOrientation(creature->respawn_cord[3]);
        } else {
            // Calculate the angle to our next position

            float dx = (float)m_destinationX - m_Unit->GetPositionX();
            float dy = (float)m_destinationY - m_Unit->GetPositionY();
            if(dy != 0.0f)
            {
                float angle = atan2(dy, dx);
                m_Unit->SetOrientation(angle);
            }
        }
    }
    SendMoveToPacket(m_destinationX, m_destinationY, m_destinationZ, m_Unit->GetOrientation(), moveTime, getMoveFlags());

    m_timeToMove = moveTime;
    m_timeMoved = 0;
    if(m_moveTimer == 0)
    {
        m_moveTimer =  UNIT_MOVEMENT_INTERPOLATE_INTERVAL; // update every few msecs
    }

    m_creatureState = MOVING;
}

void AIInterface::SendCurrentMove(Player* plyr/*uint64 guid*/)
{
    if(m_destinationX == 0.0f && m_destinationY == 0.0f && m_destinationZ == 0.0f) return; //invalid move 
    ByteBuffer *splineBuf = new ByteBuffer(20*4);
    *splineBuf << uint32(getMSTime()); // spline flags
    *splineBuf << uint32((m_totalMoveTime - m_timeToMove)+m_moveTimer); //Time Passed (start Position) //should be generated/save 
    *splineBuf << uint32(m_totalMoveTime); //Total Time //should be generated/save
    *splineBuf << uint32(0); //Unknown
    *splineBuf << uint32(4); //Spline Count    // lets try this

    *splineBuf << m_sourceX << m_sourceY << m_sourceZ;
    *splineBuf << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
    *splineBuf << m_destinationX << m_destinationY << m_destinationZ;
    *splineBuf << m_destinationX << m_destinationY << m_destinationZ;
    *splineBuf << m_destinationX << m_destinationY << m_destinationZ;

    plyr->AddSplinePacket(m_Unit->GetGUID(), splineBuf);

    //This should only be called by Players AddInRangeObject() ONLY
    //using guid cuz when i atempted to use pointer the player was deleted when this event was called some times
    //Player* plyr = World::GetPlayer(guid);
    //if(!plyr) return;

    /*if(m_destinationX == 0.0f && m_destinationY == 0.0f && m_destinationZ == 0.0f) return; //invalid move 
    uint32 moveTime = m_timeToMove-m_timeMoved;
    //uint32 moveTime = (m_timeToMove-m_timeMoved)+m_moveTimer;
    WorldPacket data(50);
    data.SetOpcode( SMSG_MONSTER_MOVE );
    data << m_Unit->GetNewGUID();
    data << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
    data << getMSTime();
    data << uint8(0);
    data << getMoveFlags();

    //float distance = m_Unit->CalcDistance(m_destinationX, m_destinationY, m_destinationZ);
    //uint32 moveTime = (uint32) (distance / m_moveSpeed);

    data << moveTime;
    data << uint32(1); //Number of Waypoints
    data << m_destinationX << m_destinationY << m_destinationZ;
    plyr->GetSession()->SendPacket(&data);*/

}

bool AIInterface::setInFront(Unit* target) // not the best way to do it, though
{
    //angle the object has to face
    float angle = m_Unit->calcAngle(m_Unit->GetPositionX(), m_Unit->GetPositionY(), target->GetPositionX(), target->GetPositionY() ); 
    //Change angle slowly 2000ms to turn 180 deg around
    if(angle > 180) angle += 90;
    else angle -= 90; //angle < 180
    m_Unit->getEasyAngle(angle);
    //Convert to Blizzards Format
    float orientation = angle / (360 / float(2 * M_PI));
    //Update Orentation Server Side
    m_Unit->SetPosition(m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ(), orientation);
    //Update Orentation Client Side
    //bool Send = m_Unit->isInFront(target);
    //return true;
    
    return m_Unit->isInFront(target);
}

bool AIInterface::addWayPoint(WayPoint* wp)
{
    if(!wp) 
        return false;
    if(wp->id <= 0)
        return false; //not valid id

    if(m_waypoints.find( wp->id ) == m_waypoints.end( ))
    {
        //m_waypoints[wp->id] = wp;
        m_waypoints.insert(WayPointMap::value_type(wp->id, wp));
        m_nWaypoints = m_waypoints.size();
        return true;
    }
    return false;
}

void AIInterface::setWaypointMap(HM_NAMESPACE::hash_map<uint32, WayPoint*> wpm)
{
    m_waypoints = wpm;
    m_nWaypoints = m_waypoints.size();
}

void AIInterface::changeWayPointID(uint32 oldwpid, uint32 newwpid)
{
    if(newwpid <= 0) 
        return; //not valid id
    if(newwpid > m_waypoints.size()) 
        return; //not valid id
    if(newwpid == oldwpid) 
        return; //same spot

    //already wp with that id ?
    WayPoint* originalwp = getWayPoint(newwpid);
    if(!originalwp) 
        return;
    WayPoint* oldwp = getWayPoint(oldwpid);
    if(!oldwp) 
        return;
//    uint32 startsize = m_waypoints.size();

    //we have valid wps in the positions
    WayPoint* wpnext = NULL;
    WayPoint* wp = NULL;
    uint32 i = 0;
    uint32 endat = 0;
    //reorder freeing newwpid's spot
    if((oldwpid+1 == newwpid) || (newwpid+1 == oldwpid))
    {
        //within 1 place of eachother just swap
        oldwp->id = newwpid;
        originalwp->id = oldwpid;
        m_waypoints[newwpid] = oldwp;
        m_waypoints[oldwpid] = originalwp;
    }
    //        4          2
    else if(oldwpid > newwpid) // move others up
    {
        //2       4         2
        endat = (oldwpid-newwpid);

        for(i = 0; i < endat;i++)
        {
            //                  2
            wpnext = getWayPoint(newwpid+i+1); //waypoint 3

            if(i == 0)
            {
                //First move
                oldwp->id = newwpid;
                m_waypoints[newwpid] = oldwp; //position 2

                originalwp->id = newwpid+1;
                m_waypoints[newwpid+1] = originalwp; //position 3
            }
            else //i > 0
            {
                if(wp)
                {
                    wp->id = newwpid+1+i;
                    m_waypoints[newwpid+1+i] = wp;
                }
            }
            wp = wpnext;
        }
    }
    //        2          4
    else if(oldwpid < newwpid) //move others down
    {
        //2       4         2
        endat = (newwpid-oldwpid);

        for(i = 0; i < endat;i++)
        {
            //                  2
            wp = getWayPoint(oldwpid+i+1); //waypoint 3
            if(wp)
            {
                wp->id = oldwpid+i;
                m_waypoints[oldwpid+i] = wp; //position 2
            }
            if(i == endat-1)
            {
                oldwp->id = newwpid;
                m_waypoints[newwpid] = oldwp; //position 4
            }
        }
    }

    //SaveAll to db
    saveWayPoints(0);
}

void AIInterface::deleteWayPoint(uint32 wpid, bool save)
{
    if(wpid <= 0) 
        return; //not valid id
    if(wpid > m_waypoints.size()) 
        return; //not valid id

    uint32 startsize = m_waypoints.size();
    
    //Delete Waypoint
    WayPointMap::iterator iter = m_waypoints.find( wpid );
    if( iter != m_waypoints.end() )
    {
        delete iter->second;
        m_waypoints.erase( iter );
    }
    
    WayPoint* wp = NULL;
    //Reorginise the rest
    if(wpid <= m_waypoints.size()) //non existant wp
    {
        uint32 i = 0;
        for(i = 0; i < startsize-wpid;i++)
        {
            wp = m_waypoints[wpid+i+1];
            if(wp)
            {
                wp->id = wpid+i;
                m_waypoints[wpid+i] = wp;
            }
        }
        m_waypoints.erase( wpid+i );
    }

    m_nWaypoints = m_waypoints.size();
    //SaveAll to db
    if(save)
        saveWayPoints(0);
}

bool AIInterface::showWayPoints(uint32 wpid, Player* pPlayer, bool Backwards)
{
//    if(wpid < 0) return false; //not valid id
    if(wpid > m_waypoints.size()) 
        return false; //not valid id

    //wpid of 0 == all
    WayPointMap::const_iterator itr;
    if((m_WayPointsShowing == true) && (wpid == 0)) 
        return false;
    m_WayPointsShowing = true;

    WayPoint* wp = NULL;
    for (itr = m_waypoints.begin(); itr != m_waypoints.end(); itr++)
    {
        if( (itr->second->id == wpid) || (wpid == 0) )
        {
            wp = itr->second;

            //Create
            Creature* pWayPoint = new Creature(HIGHGUID_WAYPOINT ,wp->id);
            pWayPoint->CreateWayPoint(wp->id,pPlayer->GetMapId(),wp->x,wp->y,wp->z,0);
            pWayPoint->SetUInt32Value(OBJECT_FIELD_ENTRY, 300000);
            pWayPoint->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.5f);
            if(Backwards)
            {
                uint32 DisplayID = (wp->backwardskinid == 0)? GetUnit()->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID) : wp->backwardskinid;
                pWayPoint->SetUInt32Value(UNIT_FIELD_DISPLAYID, DisplayID);
                pWayPoint->SetUInt32Value(UNIT_NPC_EMOTESTATE, wp->backwardemoteid);
            }
            else
            {
                uint32 DisplayID = (wp->forwardskinid == 0)? GetUnit()->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID) : wp->forwardskinid;
                pWayPoint->SetUInt32Value(UNIT_FIELD_DISPLAYID, DisplayID);
                pWayPoint->SetUInt32Value(UNIT_NPC_EMOTESTATE, wp->forwardemoteid);
            }
            pWayPoint->SetUInt32Value(UNIT_FIELD_LEVEL, wp->id);
            pWayPoint->SetUInt32Value(UNIT_NPC_FLAGS, 0);
            pWayPoint->SetUInt32Value(UNIT_FIELD_AURA+32, 8326); //invisable & deathworld look
            pWayPoint->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE , pPlayer->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
            pWayPoint->SetUInt32Value(UNIT_FIELD_HEALTH, 1);
            pWayPoint->SetUInt32Value(UNIT_FIELD_MAXHEALTH, 1);

            //Create on client
            ByteBuffer buf(2500);
            uint32 count = pWayPoint->BuildCreateUpdateBlockForPlayer(&buf, pPlayer);
            pPlayer->PushUpdateData(&buf, count);

            //root the object
            WorldPacket data1;
            data1.Initialize(SMSG_FORCE_MOVE_ROOT);
            data1 << pWayPoint->GetNewGUID();
            pPlayer->GetSession()->SendPacket( &data1 );

            //Cleanup
            //delete data;
            delete pWayPoint;
            if(wpid != 0) return true;
        }
    }
    return true;
}

bool AIInterface::hideWayPoints(uint32 wpid, Player* pPlayer)
{
//    if(wpid < 0) return false; //not valid id
    if(wpid > m_waypoints.size()) return false; //not valid id

    //wpid of 0 == all
    if(m_WayPointsShowing != true) return false;
    m_WayPointsShowing = false;
    WayPointMap::const_iterator itr;

    // slightly better way to do this
    uint64 guid;

    for (itr = m_waypoints.begin(); itr != m_waypoints.end(); itr++)
    {
        if( (itr->second->id == wpid) || (wpid == 0) )
        {
            // avoid C4293
            guid = ((uint64)HIGHGUID_WAYPOINT << 32) | itr->second->id;
            WoWGuid wowguid(guid);
            pPlayer->PushOutOfRange(wowguid);
            if(wpid != 0) return true;
        }
    }
    return true;
}

bool AIInterface::saveWayPoints(uint32 wpid)
{
//    if(wpid < 0) return false; //not valid id
    if(wpid > m_waypoints.size()) return false; //not valid id
    if(!GetUnit()) return false;
    if(GetUnit()->GetTypeId() != TYPEID_UNIT) return false;

    //wpid of 0 == all
    std::stringstream ss;
    if(wpid == 0)
    {
        //Delete
        ss << "DELETE FROM creature_waypoints WHERE creatureid = " << ((Creature*)GetUnit())->GetSQL_id() << "\0";
        sDatabase.Query( ss.str().c_str() );
    }

    WayPointMap::const_iterator itr;

    WayPoint* wp = NULL;
    for (itr = m_waypoints.begin(); itr != m_waypoints.end(); itr++)
    {
        if(itr == m_waypoints.end()) 
            break;
        if(!itr->second) 
            continue;

        if( (itr->second->id == wpid) || (wpid == 0) )
        {
            wp = itr->second;

            if(wpid != 0)
            {
                //Delete
                ss.str("");
                ss << "DELETE FROM creature_waypoints WHERE creatureid = " << ((Creature*)GetUnit())->GetSQL_id() << " and waypointid = " << wp->id << "\0";
                sDatabase.Query( ss.str().c_str() );
            }

            //Save
            ss.str("");
            ss << "INSERT INTO creature_waypoints ";
            ss << "(creatureid,waypointid,x,y,z,waittime,flags,forwardemoteoneshot,forwardemoteid,backwardemoteoneshot,backwardemoteid,forwardskinid,backwardskinid) VALUES (";
            ss << ((Creature*)GetUnit())->GetSQL_id() << ", ";
            ss << wp->id << ", ";
            ss << wp->x << ", ";
            ss << wp->y << ", ";
            ss << wp->z << ", ";
            ss << wp->waittime << ", ";
            ss << wp->flags << ", ";
            ss << wp->forwardemoteoneshot << ", ";
            ss << wp->forwardemoteid << ", ";
            ss << wp->backwardemoteoneshot << ", ";
            ss << wp->backwardemoteid << ", ";
            ss << wp->forwardskinid << ", ";
            ss << wp->backwardskinid << ")\0";
            sDatabase.Query( ss.str().c_str() );

            if(wpid != 0) 
                return true;
        }
    }
    return false;
}

WayPoint* AIInterface::getWayPoint(uint32 wpid)
{
//    if(wpid < 0) return NULL; //not valid id
    if(wpid > m_waypoints.size()) 
        return NULL; //not valid id

    WayPointMap::const_iterator itr = m_waypoints.find( wpid );
    if( itr != m_waypoints.end( ) )
        return itr->second;
    return NULL;
}

void AIInterface::_UpdateMovement(uint32 p_time)
{
    if(!m_Unit->isAlive())
    {
        StopMovement(0);
        return;
    }

    uint32 timediff = 0;

    if(m_moveTimer > 0)
    {
        if(p_time >= m_moveTimer)
        {
            timediff = p_time - m_moveTimer;
            m_moveTimer = 0;
        }
        else
            m_moveTimer -= p_time;
    }

    if(m_timeToMove > 0)
    {
        m_timeMoved = m_timeToMove <= p_time + m_timeMoved ? m_timeToMove : p_time + m_timeMoved;
    }

    if(m_creatureState == MOVING)
    {
        if(!m_moveTimer)
        {
            if(m_timeMoved == m_timeToMove) //reached destination
            {
                if(m_moveType == MOVEMENTTYPE_WANTEDWP)//We reached wanted wp stop now
                    m_moveType = MOVEMENTTYPE_DONTMOVEWP;

                float wayO = 0.0f;

                if((m_nWaypoints != 0) && (m_AIState == STATE_IDLE || m_AIState == STATE_SCRIPTMOVE)) //if we attacking don't use wps
                {
                    WayPoint* wp = getWayPoint(getCurrentWaypoint());
                    if(wp)
                    {
                        CALL_SCRIPT_EVENT(m_Unit, OnReachWP)(wp->id, !m_moveBackward);
                        //Lets face to correct orientation
                        wayO = wp->o;
                        m_moveTimer = wp->waittime; //wait before next move
                        if(!m_moveBackward)
                        {
                            if(wp->forwardemoteoneshot)
                            {
                                GetUnit()->Emote(EmoteType(wp->forwardemoteid));
                            }
                            else
                            {
                                if(GetUnit()->GetUInt32Value(UNIT_NPC_EMOTESTATE) != wp->forwardemoteid)
                                {
                                    GetUnit()->SetUInt32Value(UNIT_NPC_EMOTESTATE, wp->forwardemoteid);
                                }
                            }
                        }
                        else
                        {
                            if(wp->backwardemoteoneshot)
                            {
                                GetUnit()->Emote(EmoteType(wp->backwardemoteid));
                            }
                            else
                            {
                                if(GetUnit()->GetUInt32Value(UNIT_NPC_EMOTESTATE) != wp->backwardemoteid)
                                {
                                    GetUnit()->SetUInt32Value(UNIT_NPC_EMOTESTATE, wp->backwardemoteid);
                                }
                            }
                        }
                    }
                    else
                        m_moveTimer = sRand.randInt(m_moveRun ? 5000 : 10000); // wait before next move
                }

                m_creatureState = STOPPED;
                m_moveSprint = false;

                //if(m_fleeTimer != 0)
                   // m_moveTimer = 0;

                /*float dx = m_destinationX - m_Unit->GetPositionX();
                float dy = m_destinationY - m_Unit->GetPositionY();
                if(dy != 0.0f)
                    m_Unit->SetPosition(m_destinationX, m_destinationY, m_destinationZ, atanf(dy/dx), true);
                else*/
                if(m_MovementType = MOVEMENTTYPE_DONTMOVEWP)
                    m_Unit->SetPosition(m_destinationX, m_destinationY, m_destinationZ, wayO, true);
                else
                    m_Unit->SetPosition(m_destinationX, m_destinationY, m_destinationZ, m_Unit->GetOrientation(), true);

                m_destinationX = m_destinationY = m_destinationZ = 0;
                m_timeMoved = 0;
                m_timeToMove = 0;
            }
            else
            {
                //Move Server Side Update
                float q = (float)m_timeMoved / (float)m_timeToMove;
                float x = m_Unit->GetPositionX() + (m_destinationX - m_Unit->GetPositionX()) * q;
                float y = m_Unit->GetPositionY() + (m_destinationY - m_Unit->GetPositionY()) * q;
                float z = m_Unit->GetPositionZ() + (m_destinationZ - m_Unit->GetPositionZ()) * q;
                
                if(m_moveFly != true)
                {
                    if(m_Unit->GetMapMgr())
                    {
                        float adt_Z = m_Unit->GetMapMgr()->GetLandHeight(x, y);
                        if(fabsf(adt_Z - z) < 1.5)
                            z = adt_Z;
                    }
                }
                
                /*float dx = m_destinationX - m_Unit->GetPositionX();
                float dy = m_destinationY - m_Unit->GetPositionY();
                if(dy != 0.0f)
                    m_Unit->SetPosition(x, y, z, atanf(dy/dx));
                else*/
                    m_Unit->SetPosition(x, y, z, m_Unit->GetOrientation());
                
                m_timeToMove -= m_timeMoved;
                m_timeMoved = 0;
                m_moveTimer = (UNIT_MOVEMENT_INTERPOLATE_INTERVAL < m_timeToMove) ? UNIT_MOVEMENT_INTERPOLATE_INTERVAL : m_timeToMove;
            }
            //**** Movement related stuff that should be done after a move update (Keeps Client and Server Synced) ****//
            //**** Process the Pending Move ****//
            if(m_nextPosX != 0.0f && m_nextPosY != 0.0f)
            {
                UpdateMove();
            }
        }
    }
    else if(m_creatureState == STOPPED && (m_AIState == STATE_IDLE || m_AIState == STATE_SCRIPTMOVE) && !m_moveTimer && !m_timeToMove && UnitToFollow == NULL) //creature is stopped and out of Combat
    {
        if(sWorld.getAllowMovement() == false) //is creature movement enabled?
            return;

        if(m_Unit->GetUInt32Value(UNIT_FIELD_DISPLAYID) == 5233) //if Spirit Healer don't move
            return;

        // do we have a formation?
        if(m_formationLinkSqlId != 0)
        {
            if(!m_formationLinkTarget)
            {
                // haven't found our target yet
                Creature * c = static_cast<Creature*>(m_Unit);
                if(!c->haslinkupevent)
                {
                    // register linkup event
                    c->haslinkupevent = true;
                    sEventMgr.AddEvent(c, &Creature::FormationLinkUp, m_formationLinkSqlId, 
                        EVENT_CREATURE_FORMATION_LINKUP, 1000, 0);
                }
            }
            else
            {
                // we've got a formation target, set unittofollow to this
                UnitToFollow = m_formationLinkTarget;
                FollowDistance = m_formationFollowDistance;
                m_fallowAngle = m_formationFollowAngle;
            }
        }
        if(UnitToFollow == 0)
        {
            // no formation, use waypoints
            int destpoint = -1;

            // If creature has no waypoints just wander aimlessly around spawnpoint
            if(m_nWaypoints==0) //no waypoints
            {
                /*    if(m_moveRandom)
                {
                if((rand()%10)==0)                                                                                                                                    
                {                                                                                                                                                                  
                float wanderDistance = rand()%4 + 2;
                float wanderX = ((wanderDistance*rand()) / RAND_MAX) - wanderDistance / 2;                                                                                                               
                float wanderY = ((wanderDistance*rand()) / RAND_MAX) - wanderDistance / 2;                                                                                                               
                float wanderZ = 0; // FIX ME ( i dont know how to get apropriate Z coord, maybe use client height map data)                                                                                                             

                if(m_Unit->CalcDistance(m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ(), ((Creature*)m_Unit)->respawn_cord[0], ((Creature*)m_Unit)->respawn_cord[1], ((Creature*)m_Unit)->respawn_cord[2])>15)                                                                           
                {   
                //return home                                                                                                                                                 
                MoveTo(((Creature*)m_Unit)->respawn_cord[0],((Creature*)m_Unit)->respawn_cord[1],((Creature*)m_Unit)->respawn_cord[2],false);
                }   
                else
                {
                MoveTo(m_Unit->GetPositionX() + wanderX, m_Unit->GetPositionY() + wanderY, m_Unit->GetPositionZ() + wanderZ,false);
                }    
                }    
                }
                */    
                return;                                                                                                                                                   
            }                                                                                                                                                          
            else //we do have waypoints
            {
                if(m_moveType == MOVEMENTTYPE_RANDOMWP) //is random move on if so move to a random waypoint
                {
                    if(m_nWaypoints > 1)
                        destpoint = sRand.randInt(m_nWaypoints);
                }
                else if (m_moveType == MOVEMENTTYPE_CIRCLEWP) //random move is not on lets follow the path in circles
                {
                    // 1 -> 10 then 1 -> 10
                    m_currentWaypoint++;
                    if (m_currentWaypoint > m_nWaypoints) m_currentWaypoint = 1; //Happens when you delete last wp seems to continue ticking
                    destpoint = m_currentWaypoint;
                    m_moveBackward = false;
                }
                else if(m_moveType == MOVEMENTTYPE_WANTEDWP)//Move to wanted wp
                {
                    if(m_currentWaypoint)
                    {
                        if(m_nWaypoints > 0)
                        {
                            destpoint = m_currentWaypoint;
                        }
                        else
                            destpoint = -1;
                    }
                }
                else if(m_moveType != MOVEMENTTYPE_QUEST && m_moveType != MOVEMENTTYPE_DONTMOVEWP)//4 Unused
                {
                    // 1 -> 10 then 10 -> 1
                    if (m_currentWaypoint > m_nWaypoints) m_currentWaypoint = 1; //Happens when you delete last wp seems to continue ticking
                    if (m_currentWaypoint == m_nWaypoints) // Are we on the last waypoint? if so walk back
                        m_moveBackward = true;
                    if (m_currentWaypoint == 1) // Are we on the first waypoint? if so lets goto the second waypoint
                        m_moveBackward = false;
                    if (!m_moveBackward) // going 1..n
                        destpoint = ++m_currentWaypoint;
                    else                // going n..1
                        destpoint = --m_currentWaypoint;
                }

                if(destpoint != -1)
                {
                    WayPoint* wp = getWayPoint(destpoint);
                    if(wp)
                    {
                        if(!m_moveBackward)
                        {
                            if((wp->forwardskinid != 0) && (GetUnit()->GetUInt32Value(UNIT_FIELD_DISPLAYID) != wp->forwardskinid))
                            {
                                GetUnit()->SetUInt32Value(UNIT_FIELD_DISPLAYID, wp->forwardskinid);
                            }
                        }
                        else
                        {
                            if((wp->backwardskinid != 0) && (GetUnit()->GetUInt32Value(UNIT_FIELD_DISPLAYID) != wp->backwardskinid))
                            {
                                GetUnit()->SetUInt32Value(UNIT_FIELD_DISPLAYID, wp->backwardskinid);
                            }
                        }
                        m_moveFly = (wp->flags == 768) ? 1 : 0;
                        m_moveRun = (wp->flags == 256) ? 1 : 0;
                        MoveTo(wp->x, wp->y, wp->z, 0);
                    }
                }
            }
        }
    }

    //Fear Code
    if(m_AIState == STATE_FEAR && UnitToFear != NULL && m_creatureState == STOPPED)
    {
        if(getMSTime() > m_FearTimer)   // Wait at point for x ms ;)
        {
            float Fx;
            float Fy;
            float Fz;
            // Calculate new angle to target.
            float Fo = m_Unit->calcRadAngle(UnitToFear->GetPositionX(), UnitToFear->GetPositionY(), m_Unit->GetPositionX(), m_Unit->GetPositionY());
                
            Fx = m_Unit->GetPositionX() + 10*cosf(Fo);
            Fy = m_Unit->GetPositionY() + 10*sinf(Fo);
    
            // Check if this point is in water.
            float wl = m_Unit->GetMapMgr()->GetWaterHeight(Fx, Fy);
            uint8 wt = m_Unit->GetMapMgr()->GetWaterType(Fx, Fy);
    
            Fz = m_Unit->GetMapMgr()->GetLandHeight(Fx, Fy);
            if(!(fabs(m_Unit->GetPositionZ() - Fz) > 4 || Fz < (wl-2))/* && wt & 0x1*/)
            {        
                MoveTo(Fx, Fy, Fz, Fo);
            }
            m_FearTimer = m_totalMoveTime + getMSTime() + 200;
        }
    }
    
    if(m_AIState == STATE_WANDER && m_creatureState == STOPPED)
    {
        float wanderO = sRand.randInt(6);
        float wanderX = m_Unit->GetPositionX() + cosf(wanderO);                                                                                                             
        float wanderY = m_Unit->GetPositionY() + sinf(wanderO);                                                                                                              
        float wanderZ;
            


        // Check if this point is in water.
        float wl = m_Unit->GetMapMgr()->GetWaterHeight(wanderX, wanderY);
        uint8 wt = m_Unit->GetMapMgr()->GetWaterType(wanderX, wanderY);

        wanderZ = m_Unit->GetMapMgr()->GetLandHeight(wanderX, wanderY);
        if(!(fabs(m_Unit->GetPositionZ() - wanderZ) > 4 || wanderZ < (wl-2))/* && wt & 0x1*/)
        {
            MoveTo(wanderX,wanderY,wanderZ,wanderO);
        }   
    }

    //Unit Follow Code
    if(UnitToFollow != NULL && (m_AIState == STATE_IDLE || m_AIState == STATE_FOLLOWING))
    {
        float dist = m_Unit->GetDistanceSq(UnitToFollow);

        // re-calculate orientation based on target's movement
        if(m_lastFollowX != UnitToFollow->GetPositionX() ||
            m_lastFollowY != UnitToFollow->GetPositionY())
        {
            float dx = UnitToFollow->GetPositionX() - m_Unit->GetPositionX();
            float dy = UnitToFollow->GetPositionY() - m_Unit->GetPositionY();
            if(dy != 0.0f)
            {
                float angle = atan2(dx,dy);
                m_Unit->SetOrientation(angle);
            }
            m_lastFollowX = UnitToFollow->GetPositionX();
            m_lastFollowY = UnitToFollow->GetPositionY();
        }

        if (dist > powf(FollowDistance,2)) //if out of range
        {
            m_AIState = STATE_FOLLOWING;
            
            if(dist > 25.0f) //25 yard away lets run else we will loose the them
                m_moveRun = true;
            else 
                m_moveRun = false;

            if(m_AIType == AITYPE_PET || UnitToFollow == m_formationLinkTarget) //Unit is Pet/formation
            {
                if(dist > 900.0f/*30*/)
                    m_moveSprint = true;

                // estimate position based on orientation
                /*float delta_x = -(sinf(UnitToFollow->GetOrientation()) * 5);
                float delta_y = -(cosf(UnitToFollow->GetOrientation()) * 5);

                delta_x += UnitToFollow->GetPositionX();
                delta_y += UnitToFollow->GetPositionY();*/
                float delta_x = UnitToFollow->GetPositionX();
                float delta_y = UnitToFollow->GetPositionY();
                float d = 3;
                if(m_formationLinkTarget)
                    d = m_formationFollowDistance;

                MoveTo(delta_x+(d*(cosf(m_fallowAngle+UnitToFollow->GetOrientation()))),
                       delta_y+(d*(sinf(m_fallowAngle+UnitToFollow->GetOrientation()))),
                       UnitToFollow->GetPositionZ(),UnitToFollow->GetOrientation());
            }
            else
            {
                _CalcDestinationAndMove(UnitToFollow, FollowDistance);
                /*Destination* dst = _CalcDestination(UnitToFollow, FollowDistance);
                MoveTo(dst->x, dst->y, dst->z,0);
                delete dst;*/
            }
        }
    }
}

void AIInterface::CastSpell(Unit* caster, SpellEntry *spellInfo, SpellCastTargets targets)
{
    // check for spell id
    //printf("Spell: %u cast by "I64FMT"\n", spellInfo->Id, caster->GetGUID());
    
    // Stop movement while casting.
    /*int32 CastTime = GetCastTime(spellInfo->CastingTimeIndex);
    if(CastTime > 0)
        StopMovement(CastTime);*/

    m_AIState = STATE_CASTING;
    Spell *spell = new Spell(caster, spellInfo, false, NULL);
    spell->prepare(&targets);
}

SpellEntry *AIInterface::getSpellEntry(uint32 spellId)
{
    SpellEntry *spellInfo = sSpellStore.LookupEntry(spellId );

    if(!spellInfo)
    {
        sLog.outError("WORLD: unknown spell id %i\n", spellId);
        return NULL;
    }

    return spellInfo;
}

SpellCastTargets AIInterface::setSpellTargets(SpellEntry *spellInfo, Unit* target)
{
    SpellCastTargets targets;
    targets.m_unitTarget = 0;
    targets.m_itemTarget = 0;
    targets.m_srcX = 0;
    targets.m_srcY = 0;
    targets.m_srcZ = 0;
    targets.m_destX = 0;
    targets.m_destY = 0;
    targets.m_destZ = 0;

    if(m_nextSpell->spelltargetType == TTYPE_SINGLETARGET)
    {
        targets.m_targetMask = 2;
        targets.m_unitTarget = target->GetGUID();
    }
    else if(m_nextSpell->spelltargetType == TTYPE_SOURCE)
    {
        targets.m_targetMask = 32;
        targets.m_srcX = m_Unit->GetPositionX();
        targets.m_srcY = m_Unit->GetPositionY();
        targets.m_srcZ = m_Unit->GetPositionZ();
    }
    else if(m_nextSpell->spelltargetType == TTYPE_DESTINATION)
    {
        targets.m_targetMask = 64;
        targets.m_destX = target->GetPositionX();
        targets.m_destY = target->GetPositionY();
        targets.m_destZ = target->GetPositionZ();
    }

    return targets;
}

AI_Spell *AIInterface::getSpellByEvent(uint32 event)
{
    SpellMap::iterator i;
    switch(event)
    {
    case EVENT_ENTERCOMBAT:
        {
            if(!m_hasOnEnterCombatSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnEnterCombatSpells.begin();i != m_OnEnterCombatSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_LEAVECOMBAT:
        {
            if(!m_hasOnLeaveCombatSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnLeaveCombatSpells.begin();i != m_OnLeaveCombatSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_DAMAGETAKEN:
        {
            if(!m_hasOnDamageTakenSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnDamageTakenSpells.begin();i != m_OnDamageTakenSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_TARGETCASTSPELL:
        {
            if(!m_hasOnTargetCastSpellSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnTargetCastSpellSpells.begin();i != m_OnTargetCastSpellSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_TARGETPARRYED:
        {
            if(!m_hasOnTargetParryedSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnTargetParryedSpells.begin();i != m_OnTargetParryedSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_TARGETDODGED:
        {
            if(!m_hasOnTargetDodgedSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnTargetDodgedSpells.begin();i != m_OnTargetDodgedSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_TARGETBLOCKED:
        {
            if(!m_hasOnTargetBlockedSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnTargetBlockedSpells.begin();i != m_OnTargetBlockedSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_TARGETCRITHIT:
        {
            if(!m_hasOnTargetCritHitSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnTargetCritHitSpells.begin();i != m_OnTargetCritHitSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            i->second->spellCooldownTimer = 500;
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_TARGETDIED:
        {
            if(!m_hasOnTargetDiedSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnTargetDiedSpells.begin();i != m_OnTargetDiedSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_UNITPARRYED:
        {
            if(!m_hasOnUnitParryedSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnUnitParryedSpells.begin();i != m_OnUnitParryedSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_UNITDODGED:
        {
            if(!m_hasOnUnitDodgedSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnUnitDodgedSpells.begin();i != m_OnUnitDodgedSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_UNITBLOCKED:
        {
            if(!m_hasOnUnitBlockedSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnUnitBlockedSpells.begin();i != m_OnUnitBlockedSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_UNITCRITHIT:
        {
            if(!m_hasOnUnitCritHitSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnUnitCritHitSpells.begin();i != m_OnUnitCritHitSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_UNITDIED:
        {
            if(!m_hasOnUnitDiedSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnUnitDiedSpells.begin();i != m_OnUnitDiedSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_ASSISTTARGETDIED:
        {
            if(!m_hasOnAssistTargetDiedSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnAssistTargetDiedSpells.begin();i != m_OnAssistTargetDiedSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    case EVENT_FOLLOWOWNER:
        {
            if(!m_hasOnFollowOwnerSpells) // has Spells for that Event -- used to reduce iterations
            {
                break;
            }
            for(i = m_OnFollowOwnerSpells.begin();i != m_OnFollowOwnerSpells.end(); i++)
            {
                if(i->second->procCount == 0 || i->second->procCounter < i->second->procCount) // procCount for that spell isnt expired yet
                {
                    if(i->second->spellCooldownTimer == 0) // there is no Cooldown for that Spell
                    {
                        if(Rand(i->second->procChance)) // proc this Spell
                        {
                            return i->second;
                        }
                    }
                }
            }
        }break;
    }

    /*AI_Spell *sp = new AI_Spell;
    sp->agent = AGENT_MELEE;          // warning... LEAKY:D
    return sp;*/
    return m_DefaultSpell;
}

void AIInterface::resetSpellCounter()
{
    SpellMap::iterator i;
    if(m_hasCooldownOnEnterCombatSpells == true)
    {
        m_hasCooldownOnEnterCombatSpells = false;
        for(i = m_OnEnterCombatSpells.begin();i != m_OnEnterCombatSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnLeaveCombatSpells == true)
    {
        m_hasCooldownOnLeaveCombatSpells = false;
        for(i = m_OnLeaveCombatSpells.begin();i != m_OnLeaveCombatSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnDamageTakenSpells == true)
    {
        m_hasCooldownOnDamageTakenSpells = false;
        for(i = m_OnDamageTakenSpells.begin();i != m_OnDamageTakenSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnTargetCastSpellSpells == true)
    {
        m_hasCooldownOnTargetCastSpellSpells = false;
        for(i = m_OnTargetCastSpellSpells.begin();i != m_OnTargetCastSpellSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnTargetParryedSpells == true)
    {
        m_hasCooldownOnTargetParryedSpells = false;
        for(i = m_OnTargetParryedSpells.begin();i != m_OnTargetParryedSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnTargetDodgedSpells == true)
    {
        m_hasCooldownOnTargetDodgedSpells = false;
        for(i = m_OnTargetDodgedSpells.begin();i != m_OnTargetDodgedSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnTargetBlockedSpells == true)
    {
        m_hasCooldownOnTargetBlockedSpells = false;
        for(i = m_OnTargetBlockedSpells.begin();i != m_OnTargetBlockedSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnTargetCritHitSpells == true)
    {
        m_hasCooldownOnTargetCritHitSpells = false;
        for(i = m_OnTargetCritHitSpells.begin();i != m_OnTargetCritHitSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnTargetDiedSpells == true)
    {
        m_hasCooldownOnTargetDiedSpells = false;
        for(i = m_OnTargetDiedSpells.begin();i != m_OnTargetDiedSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnUnitParryedSpells == true)
    {
        m_hasCooldownOnUnitParryedSpells = false;
        for(i = m_OnUnitParryedSpells.begin();i != m_OnUnitParryedSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnUnitDodgedSpells == true)
    {
        m_hasCooldownOnUnitDodgedSpells = false;
        for(i = m_OnUnitDodgedSpells.begin();i != m_OnUnitDodgedSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnUnitBlockedSpells == true)
    {
        m_hasCooldownOnUnitBlockedSpells = false;
        for(i = m_OnUnitBlockedSpells.begin();i != m_OnUnitBlockedSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnUnitCritHitSpells == true)
    {
        m_hasCooldownOnUnitCritHitSpells = false;
        for(i = m_OnUnitCritHitSpells.begin();i != m_OnUnitCritHitSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnUnitDiedSpells == true)
    {
        m_hasCooldownOnUnitDiedSpells = false;
        for(i = m_OnUnitDiedSpells.begin();i != m_OnUnitDiedSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnAssistTargetDiedSpells == true)
    {
        m_hasCooldownOnAssistTargetDiedSpells = false;
        for(i = m_OnAssistTargetDiedSpells.begin();i != m_OnAssistTargetDiedSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
    if(m_hasCooldownOnFollowOwnerSpells == true)
    {
        m_hasCooldownOnFollowOwnerSpells = false;
        for(i = m_OnFollowOwnerSpells.begin();i != m_OnFollowOwnerSpells.end(); i++)
        {
            i->second->spellCooldownTimer = 0;
        }
    }
}

void AIInterface::increaseProcCounter(uint32 event, AI_Spell *sp)
{
    SpellMap::iterator i;
    switch(event)
    {
    case EVENT_ENTERCOMBAT:
        {
            i = m_OnEnterCombatSpells.find(sp->spellId);
            if(i != m_OnEnterCombatSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnEnterCombatSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_LEAVECOMBAT:
        {
            i = m_OnLeaveCombatSpells.find(sp->spellId);
            if(i != m_OnLeaveCombatSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnLeaveCombatSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_DAMAGETAKEN:
        {
            i = m_OnDamageTakenSpells.find(sp->spellId);
            if(i != m_OnDamageTakenSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnDamageTakenSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_TARGETCASTSPELL:
        {
            i = m_OnTargetCastSpellSpells.find(sp->spellId);
            if(i != m_OnTargetCastSpellSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnTargetCastSpellSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_TARGETPARRYED:
        {
            i = m_OnTargetParryedSpells.find(sp->spellId);
            if(i != m_OnTargetParryedSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnTargetParryedSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_TARGETDODGED:
        {
            i = m_OnTargetDodgedSpells.find(sp->spellId);
            if(i != m_OnTargetDodgedSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnTargetDodgedSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_TARGETBLOCKED:
        {
            i = m_OnTargetBlockedSpells.find(sp->spellId);
            if(i != m_OnTargetBlockedSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnTargetBlockedSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_TARGETCRITHIT:
        {
            i = m_OnTargetCritHitSpells.find(sp->spellId);
            if(i != m_OnTargetCritHitSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnTargetCritHitSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_TARGETDIED:
        {
            i = m_OnTargetDiedSpells.find(sp->spellId);
            if(i != m_OnTargetDiedSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnTargetDiedSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_UNITPARRYED:
        {
            i = m_OnUnitParryedSpells.find(sp->spellId);
            if(i != m_OnUnitParryedSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnUnitParryedSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_UNITDODGED:
        {
            i = m_OnUnitDodgedSpells.find(sp->spellId);
            if(i != m_OnUnitDodgedSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnUnitDodgedSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_UNITBLOCKED:
        {
            i = m_OnUnitBlockedSpells.find(sp->spellId);
            if(i != m_OnUnitBlockedSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnUnitBlockedSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_UNITCRITHIT:
        {
            i = m_OnUnitCritHitSpells.find(sp->spellId);
            if(i != m_OnUnitCritHitSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnUnitCritHitSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_UNITDIED:
        {
            i = m_OnUnitDiedSpells.find(sp->spellId);
            if(i != m_OnUnitDiedSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnUnitDiedSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_ASSISTTARGETDIED:
        {
            i = m_OnAssistTargetDiedSpells.find(sp->spellId);
            if(i != m_OnAssistTargetDiedSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnAssistTargetDiedSpells = true;
                i->second->procCounter++;
            }
        }break;
    case EVENT_FOLLOWOWNER:
        {
            i = m_OnFollowOwnerSpells.find(sp->spellId);
            if(i != m_OnFollowOwnerSpells.end())
            {
                i->second->spellCooldownTimer = i->second->spellCooldown;
                if(i->second->spellCooldown > 0)
                    m_hasCooldownOnFollowOwnerSpells = true;
                i->second->procCounter++;
            }
        }break;
    }
}

void AIInterface::addSpellToList(AI_Spell *sp)
{
    switch(sp->procEvent)
    {
    case EVENT_ENTERCOMBAT:
        {
            m_OnEnterCombatSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnEnterCombatSpells = true;
        }break;
    case EVENT_LEAVECOMBAT:
        {
            m_OnLeaveCombatSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnLeaveCombatSpells = true;
        }break;
    case EVENT_DAMAGETAKEN:
        {
            m_OnDamageTakenSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnDamageTakenSpells = true;
        }break;
    case EVENT_TARGETCASTSPELL:
        {
            m_OnTargetCastSpellSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnTargetCastSpellSpells = true;
        }break;
    case EVENT_TARGETPARRYED:
        {
            m_OnTargetParryedSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnTargetParryedSpells = true;
        }break;
    case EVENT_TARGETDODGED:
        {
            m_OnTargetDodgedSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnTargetDodgedSpells = true;
        }break;
    case EVENT_TARGETBLOCKED:
        {
            m_OnTargetBlockedSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnTargetBlockedSpells = true;
        }break;
    case EVENT_TARGETCRITHIT:
        {
            m_OnTargetCritHitSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnTargetCritHitSpells = true;
        }break;
    case EVENT_TARGETDIED:
        {
            m_OnTargetDiedSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnTargetDiedSpells = true;
        }break;
    case EVENT_UNITPARRYED:
        {
            m_OnUnitParryedSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnUnitParryedSpells = true;
        }break;
    case EVENT_UNITDODGED:
        {
            m_OnUnitDodgedSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnUnitDodgedSpells = true;
        }break;
    case EVENT_UNITBLOCKED:
        {
            m_OnUnitBlockedSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnUnitBlockedSpells = true;
        }break;
    case EVENT_UNITCRITHIT:
        {
            m_OnUnitCritHitSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnUnitCritHitSpells = true;
        }break;
    case EVENT_UNITDIED:
        {
            m_OnUnitDiedSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnUnitDiedSpells = true;
        }break;
    case EVENT_ASSISTTARGETDIED:
        {
            m_OnAssistTargetDiedSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnAssistTargetDiedSpells = true;
        }break;
    case EVENT_FOLLOWOWNER:
        {
            m_OnFollowOwnerSpells.insert(SpellMap::value_type(sp->spellId, sp));
            m_hasOnFollowOwnerSpells = true;
        }break;
    }

    m_Unit->m_SpellList.insert(sp->spellId); // add to list
}

uint32 AIInterface::getThreatByGUID(uint64 guid)
{
    /*std::list<AI_Target>::iterator i;
    for(i = m_aiTargets.begin(); i != m_aiTargets.end(); i++)
    {
        if(i->target->GetGUID() == guid)
        {
            return i->threat;
        }
    }*/
    Unit *obj = World::GetUnit(guid);
    if(obj)
        return getThreatByPtr(obj);

    return 0;
}

uint32 AIInterface::getThreatByPtr(Unit* obj)
{
    TargetMap::iterator it = m_aiTargets.find(obj);
    if(it != m_aiTargets.end())
    {
        return it->second;
    }
    return 0;
}

bool AIInterface::modThreatByGUID(uint64 guid, int32 mod)
{
    if (!m_aiTargets.size())
        return false;

    /*std::list<AI_Target>::iterator i;

    for(i = m_aiTargets.begin(); i != m_aiTargets.end(); i++)
    {
        if(i->target && i->target->GetGUID() == guid)
        {
            i->threat += mod;
            return true;
        }
    }*/
    Unit *obj = World::GetUnit(guid);
    if(obj)
        return modThreatByPtr(obj, mod);

    return false;
}

bool AIInterface::modThreatByPtr(Unit* obj, int32 mod)
{
    TargetMap::iterator it = m_aiTargets.find(obj);
    if(it != m_aiTargets.end())
    {
        it->second += mod;
        return true;
    }
    return false;
}

void AIInterface::addAssistTargets(Unit* Friend)
{
    // stop adding stuff that gives errors on linux!
    m_assistTargets.insert(Friend);
}

void AIInterface::WipeHateList()
{
    for(TargetMap::iterator itr = m_aiTargets.begin(); itr != m_aiTargets.end(); ++itr)
        itr->second = 0;
}

void AIInterface::WipeTargetList()
{
    m_nextTarget = NULL;
    
    m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);

    m_nextSpell = NULL;
    m_aiTargets.clear();
}

bool AIInterface::taunt(Unit* caster, bool apply)
{
    if(apply)
    {
        if(!caster)
        {
            isTaunted = false;
            return false;
        }
        isTaunted = true;
        tauntedBy = caster;
    }
    else
    {
        isTaunted = false;
        tauntedBy = NULL;
    }

    return true;
}

Unit* AIInterface::getTauntedBy()
{
    if(GetIsTaunted())
    {
        return tauntedBy;
    }
    else
    {
        return NULL;
    }
}

bool AIInterface::GetIsTaunted()
{
    if(isTaunted)
    {
        if(!tauntedBy)
        {
            isTaunted = false;
            tauntedBy = NULL;
        }
        if(!tauntedBy->isAlive())
        {
            isTaunted = false;
            tauntedBy = NULL;
        }
    }
    return isTaunted;
}

void AIInterface::CheckTarget(Unit* target)
{
    if(target == UnitToFollow)          // fix for crash here
    {
        UnitToFollow = NULL;
        m_lastFollowX = m_lastFollowY = 0;
    }

    if (target == m_nextTarget)     // no need to cast on these.. mem addresses are still the same
    {
        m_nextTarget = NULL;
        m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
        m_nextSpell = NULL;
    }

    if(target == UnitToFear)
        UnitToFear = NULL;


    AssistTargetSet::iterator  itr = m_assistTargets.find(target);
    if(itr != m_assistTargets.end())
        m_assistTargets.erase(itr);

    if (!m_aiTargets.size())
        return;

    TargetMap::iterator it2 = m_aiTargets.find(target);
    if(it2 != m_aiTargets.end())
    {
        m_aiTargets.erase(it2);
    }
}

uint32 AIInterface::_CalcThreat(uint32 damage, uint32 spellId, Unit* Attacker)
{
    int32 mod = 0;
    if(spellId != 0)
    {
        mod = objmgr.GetAIThreatToSpellId(spellId);
        if(mod == 0)
        {
            mod = damage;
        }
    }
    else
    {
        mod = damage;
    }

    // modify mod by Affects
    mod += (mod * Attacker->GetGeneratedThreatModifyer() / 100);

    return mod;
}

void AIInterface::WipeReferences()
{
    m_nextSpell = 0;
    m_nextTarget = 0;
    m_aiTargets.clear();
    m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
    UnitToFear = 0;
    UnitToFollow = 0;
    tauntedBy = 0;
}
