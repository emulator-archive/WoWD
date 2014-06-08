// Copyright (C) 2004 WoW Daemon
// Copyright (C) 2005 Oxide
// Copyright (C) 2005 DK
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
#define SPELL_CHANNEL_UPDATE_INTERVAL 1000

extern pSpellEffect SpellEffectsHandler[TOTAL_SPELL_EFFECTS];

void SpellCastTargets::read ( WorldPacket & data,uint64 caster )
{
    m_unitTarget = m_itemTarget = 0;m_srcX = m_srcY = m_srcZ = m_destX = m_destY = m_destZ = 0;
    m_strTarget = "";

    data >> m_targetMask;
    WoWGuid guid;

    //if(m_targetMask & TARGET_FLAG_SELF)
    if(m_targetMask == TARGET_FLAG_SELF)
    {
        m_unitTarget = caster;
    }

    if(m_targetMask & TARGET_FLAG_UNIT)
    {
        data >> guid;
        m_unitTarget = guid.GetOldGuid();
    }

    if(m_targetMask & TARGET_FLAG_CORPSE)
    {
        data >> guid;
        m_unitTarget = guid.GetOldGuid();
    }

    if(m_targetMask & TARGET_FLAG_OBJECT)
    {
        data >> guid;
        m_unitTarget = guid.GetOldGuid();
    }

    if(m_targetMask & TARGET_FLAG_ITEM)
    {
        data >> guid;
        m_itemTarget = guid.GetOldGuid();
    }

    if(m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
    {
        data >> m_srcX >> m_srcY >> m_srcZ;
    }

    if(m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        data >> m_destX >> m_destY >> m_destZ;
    }

    if(m_targetMask & TARGET_FLAG_STRING)
    {
        data >> m_strTarget;
    }
}

void SpellCastTargets::write ( WorldPacket& data)
{
    data << m_targetMask;
    WoWGuid newunit(m_unitTarget);
    WoWGuid newitem(m_itemTarget);

    if(m_targetMask == TARGET_FLAG_SELF)
        data << newunit;

    if(m_targetMask & TARGET_FLAG_UNIT)
        data << newunit;

    if(m_targetMask & TARGET_FLAG_OBJECT)
        data << newunit;

    if(m_targetMask & TARGET_FLAG_ITEM)
        data << newitem;

    if(m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
        data << m_srcX << m_srcY << m_srcZ;

    if(m_targetMask & TARGET_FLAG_DEST_LOCATION)
        data << m_destX << m_destY << m_destZ;

    if(m_targetMask & TARGET_FLAG_STRING)
        data << m_strTarget;
}

Spell::Spell(Object* Caster, SpellEntry *info, bool triggered, Aura* aur)
{
    ASSERT( Caster != NULL && info != NULL );
  
    m_spellInfo = info;
    m_caster = Caster;
    //m_CastItem = NULL;
    switch(Caster->GetTypeId())
    {
        case TYPEID_PLAYER:
        g_caster=NULL;
        i_caster=NULL;
        u_caster=(Unit*)Caster;
        p_caster=(Player*)Caster;
        break;

        case TYPEID_UNIT:
        g_caster=NULL;
        i_caster=NULL;
        p_caster=NULL;
        u_caster=(Unit*)Caster;
        break;

        case TYPEID_ITEM:
        case TYPEID_CONTAINER:
        g_caster=NULL;
        u_caster=NULL;
        p_caster=NULL;
        i_caster=(Item*)Caster;
        break;
        
        case TYPEID_GAMEOBJECT:
        u_caster=NULL;
        p_caster=NULL;
        i_caster=NULL;
        g_caster=(GameObject*)Caster;
        break;
    }

    m_spellState = SPELL_STATE_NULL;

    m_castPositionX = m_castPositionY = m_castPositionZ = 0;
//    TriggerSpellId = 0;
  //  TriggerSpellTarget = 0;
    m_triggeredSpell = triggered;
    m_AreaAura = false;
  
    m_triggeredByAura = aur;

    damageToHit = 0;
    castedItemId = 0;
    
    m_usesMana = false;
    failed = false;
    bDurSet=false;
    bRadSet[0]=false;
    bRadSet[1]=false;
    bRadSet[2]=false;
    
    cancastresult = -1;
    
    m_requiresCP=false;
    unitTarget = NULL;
    MissedTargets.clear();
    itemTarget = NULL;
    gameObjTarget = NULL;
    playerTarget = NULL;
    judgement = false;
    add_damage = 0;
    m_delayed = false;
}

Spell::~Spell()
{
    if(u_caster && u_caster->GetCurrentSpell() == this)
        u_caster->SetCurrentSpell(NULL); 
    
    if(cancastresult == -1)
        RemoveItems();
}

void Spell::FillAllTargetsInArea(std::vector<uint64> *tmpMap,float srcx,float srcy,float srcz, float range)
{
    float r = range*range;
    for(std::set<Object*>::iterator itr = m_caster->GetInRangeSetBegin(); itr != m_caster->GetInRangeSetEnd(); itr++ )
    {
        if(!((*itr)->IsUnit()) || !((Unit*)(*itr))->isAlive())
            continue;
        if(m_spellInfo->TargetCreatureType)
        {
            if((*itr)->GetTypeId()!= TYPEID_UNIT)
                continue;
            CreatureInfo *inf = ((Creature*)(*itr))->GetCreatureName();
            if(!inf)
                continue;
            if(!(1<<(inf->Type-1) & m_spellInfo->TargetCreatureType))
                continue;
        }
        if(IsInrange(srcx,srcy,srcz,(*itr),r))
        {
            if(u_caster)
            {
                if(isAttackable(u_caster, (Unit*)(*itr)))
                {
                    if(DidHit((*itr)->GetGUID()))
                        tmpMap->push_back((*itr)->GetGUID());
                    else
                        MissedTargets.push_back((*itr)->GetGUID());
                }
                    
            }
            else //cast from GO
            {
                if(g_caster && g_caster->GetUInt32Value(OBJECT_FIELD_CREATED_BY) && g_caster->m_summoner)
                {
                    //trap, check not to attack owner and friendly
                    if(isAttackable(g_caster->m_summoner,(Unit*)(*itr)))
                        tmpMap->push_back((*itr)->GetGUID());
                }
                else
                    tmpMap->push_back((*itr)->GetGUID());
            }            
            if(m_spellInfo->MaxTargets)
                if(m_spellInfo->MaxTargets == tmpMap->size())
                    return;
        }
    }    
}

bool Spell::DidHit(uint64 target)
{
    Creature *u_victim = NULL;
    Player *p_victim = NULL;
    switch(UINT32_LOPART(GUID_HIPART(target)))
    {
        case HIGHGUID_UNIT:
            u_victim = sObjHolder.GetObject<Creature>(target);
        case HIGHGUID_PLAYER:
            p_victim = sObjHolder.GetObject<Player>(target);
    }

    float resistPvP[4]={96.0f,95.0f,94.0f,87.0f};
    float resistPvE[4]={96.0f,95.0f,94.0f,89.0f};

    int32 lvldiff = 0;
    float resistchance = 1.0f;
    float miscchance = 0.0f;
    float AverageResistance = 0.0f;
    bool pvp = false;
    if(!u_victim && !p_victim)
	    return false;
    if(!p_caster)
        return false;

    if(p_caster && p_victim) //PvP
    {
        pvp = true;
        miscchance = 7.0f;
    }
    else
        miscchance = 11.0f;

    if(p_victim)
        lvldiff = p_victim->getLevel() - p_caster->getLevel();
    else
        lvldiff = u_victim->getLevel() - p_caster->getLevel();

    if(lvldiff < 0)
        lvldiff = 0;

    if(lvldiff < 3)
    { 
        if(pvp)
            resistchance = resistPvP[lvldiff];
        else
            resistchance = resistPvE[lvldiff];
    }
    else
    {
        if(pvp)
            resistchance = resistPvP[3] - ((lvldiff-3)*7);
        else
            resistchance = resistPvE[3] - ((lvldiff-3)*11);
    }
    if(resistchance < 0)
	    resistchance = 0;

    if(Rand(resistchance))
        return true;

    return false;
}

void Spell::FillTargetMap(uint32 i)
{
    uint32 cur;
    TargetsList *tmpMap=&m_targetUnits[i];
    for(uint32 j=0;j<2;j++)
    {
        if(j==0)
            cur = m_spellInfo->EffectImplicitTargetA[i];
        else // if(j==1)
            cur = m_spellInfo->EffectImplicitTargetB[i];
        
        switch(cur)
        {
        case 0:{
            if(j==0 || (m_caster->IsPet() && j==1))
            {
                if(m_targets.m_unitTarget)
                    SafeAddTarget(tmpMap,m_targets.m_unitTarget);
                else if(m_targets.m_itemTarget)
                    SafeAddTarget(tmpMap,m_targets.m_itemTarget);
                else if( (m_spellInfo->Effect[i] == SPELL_EFFECT_ADD_FARSIGHT
                    || m_spellInfo->Effect[i] == SPELL_EFFECT_SUMMON_DEMON) )
                    SafeAddTarget(tmpMap,m_caster->GetGUID());
            }
               }break;
        case 1:{// Self Target
            SafeAddTarget(tmpMap,m_caster->GetGUID());
               }break;        
            // need more research
        case 4:{ // dono related to "Wandering Plague", "Spirit Steal", "Contagion of Rot", "Retching Plague" and "Copy of Wandering Plague"
               }break;            
        case 5:{// Target: Pet
            if(p_caster)
            {
                if(p_caster->GetSummon())
                    SafeAddTarget(tmpMap,p_caster->GetSummon()->GetGUID());
            }
               }break;
        case 6:{// Single Target Enemy
               if(DidHit(m_targets.m_unitTarget))
                   SafeAddTarget(tmpMap,m_targets.m_unitTarget);
                else
                    SafeAddMissedTarget(m_targets.m_unitTarget);
                if(m_spellInfo->EffectChainTarget[i])
                {
                    uint32 jumps=m_spellInfo->EffectChainTarget[i]-1;
                    float range=GetMaxRange(sSpellRange.LookupEntry(m_spellInfo->rangeIndex));//this is probably wrong
                    range*=range;
                    std::set<Object*>::iterator itr;
                    for( itr = m_caster->GetInRangeSetBegin(); itr != m_caster->GetInRangeSetEnd(); itr++ )
                    {
                        if((*itr)->GetGUID()==m_targets.m_unitTarget)
                            continue;
                        if( !((*itr)->IsUnit()) || !((Unit*)(*itr))->isAlive())
                            continue;

                        if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),(*itr),range))
                        {
                            if(isAttackable(u_caster,(Unit*)(*itr)))
                            {
                                if(DidHit(m_targets.m_unitTarget))
                                    SafeAddTarget(tmpMap,((Unit*)*itr)->GetGUID());
                                else
                                    SafeAddMissedTarget(((Unit*)*itr)->GetGUID());
                                if(!--jumps)
                                    break;
                            }
                        }
                   }
               }
            
            }break;
            //need more research
        // Point Blank Area of Effect
        //case 7:{ // think its wrong, related to 2 spells, "Firegut Fear Storm" and "Mind Probe"
            //FillAllTargetsInArea(tmpMap,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i));
          //     }break;

        case 8:{ // related to Chess Move (DND), Firecrackers, Spotlight, aedm, Spice Mortar
            FillAllTargetsInArea(tmpMap,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i));
               }break;
            // needs more research
        //case 11:  // this is related to spellID: 4, as I think is a gm spell
          //  break;
        case 15: // All Enemies in Area of Effect (TEST)
        case 16:{ // All Enemies in Area of Effect instant (e.g. Flamestrike)
            FillAllTargetsInArea(tmpMap,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i));
                }break;

            // spells like 17278:Cannon Fire and 21117:Summon Son of Flame A
        case 17: // A single target at a xyz location or the target is a possition xyz
            break;

        case 18:{// Land under caster
            if(m_spellInfo->Effect[i] == SPELL_EFFECT_SUMMON_DEMON
                || m_spellInfo->Effect[i] == SPELL_EFFECT_SUMMON_OBJECT_WILD)
            {
                SafeAddTarget(tmpMap,m_caster->GetGUID());
                break;
            }
            FillAllTargetsInArea(tmpMap,m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),GetRadius(i));
                }break;
        case 20:{// All Party Members around the Caster in given range NOT RAID!            
            Player *p=p_caster;
            if(!p)
            {
                if(((Creature*)u_caster)->IsTotem())
                    p=(Player*)((Creature*)u_caster)->GetTotemOwner();
            }
            float r= GetRadius(i);
                
            r*=r;
            if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),p,r))
                SafeAddTarget(tmpMap,p->GetGUID());     
            
            if(SubGroup* subgroup = p->GetSubGroup())
            {                
                for(GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr)
                  {
                      if(m_caster == (*itr)) 
                          continue;
                      if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),(*itr),r))
                          SafeAddTarget(tmpMap,(*itr)->GetGUID());
                  }
             }
                }break;
        case 21: {// Single Target Friend
            Unit *Target = objmgr.GetUnit(m_targets.m_unitTarget);
            if(!Target)
                continue;
            float r= GetMaxRange(sSpellRange.LookupEntry(m_spellInfo->rangeIndex));
            if(IsInrange (m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),Target, r*r))
                SafeAddTarget(tmpMap,m_targets.m_unitTarget);
                 }break;
        case 22:{// Enemy Targets around the Caster//changed party members around you
            // burlex: this is *totally* broken. AoE only attacking friendly party members and self
            // is NOT correct.
                FillAllTargetsInArea(tmpMap,m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),GetRadius(i));
                }break;
            /*if(m_caster->GetTypeId() == TYPEID_GAMEOBJECT)
                FillAllTargetsInArea(tmpMap,m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),GetRadius(i));
            else
            {
                Player *p=p_caster;
                if(!p)
                {
                    if(((Creature*)u_caster)->isTotem())
                        p=(Player*)((Creature*)u_caster)->GetTotemOwner();
                }
                float r= GetRadius(i);
                    
                r*=r;
                if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),p,r))
                    SafeAddTarget(tmpMap,p->GetGUID());     
                
                if(SubGroup* subgroup = p->GetSubGroup())
                {                
                    for(GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr)
                    {
                        if(m_caster == (*itr)) 
                            continue;
                        if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),(*itr),r))
                            SafeAddTarget(tmpMap,(*itr)->GetGUID());
                    }
                }
                    }break;
            }*/
        case 23:{// Gameobject Target
            SafeAddTarget(tmpMap,m_targets.m_unitTarget);
                }break;
        case 24:{// Targets in Front of the Caster
            std::set<Object*>::iterator itr;
            for( itr = m_caster->GetInRangeSetBegin(); itr != m_caster->GetInRangeSetEnd(); itr++ )
            {
                if(!((*itr)->IsUnit()) || !((Unit*)(*itr))->isAlive())
                    continue;
                //is Creature in range
                if(m_caster->isInRange((Unit*)(*itr),GetRadius(i)))
                {
                    if(m_caster->isInFront((Unit*)(*itr)))
                    {
                        if(isAttackable(u_caster, (Unit*)(*itr)))
                        {
                            if(DidHit(m_targets.m_unitTarget))
                                SafeAddTarget(tmpMap,((Unit*)*itr)->GetGUID());
                            else
                                SafeAddMissedTarget(((Unit*)*itr)->GetGUID());
                        }    
                    }
                }
            }
                }break;
        case 25: {// Single Target Friend Used in Duel
            /*  Unit* Target = sObjHolder.GetObject<Player>(m_targets.m_unitTarget);

            if(!Target)

            break;

            //if(_CalcDistance(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),Target->GetPositionX(),Target->GetPositionY(),Target->GetPositionZ()) < GetMaxRange(sSpellRange.LookupEntry(m_spellInfo->rangeIndex)))

            */ 
                SafeAddTarget(tmpMap,m_targets.m_unitTarget);

                 }break;
        case 26:{// Gameobject/Item Target
            if(m_targets.m_unitTarget)
                SafeAddTarget(tmpMap,m_targets.m_unitTarget);

            if(m_targets.m_itemTarget)
                SafeAddTarget(tmpMap,m_targets.m_itemTarget);
                }break;
        case 27:{ // target is owner of pet
            // please correct this if not correct does the caster variablen need a Pet caster variable?
            if(u_caster)
                if (u_caster->IsPet())
                    SafeAddTarget(tmpMap,((Pet*)u_caster)->GetPetOwner()->GetGUID());
                }break;
            //this is handled in DO
        case 28:{// All Enemies in Area of Effect(Blizzard/Rain of Fire/volley) channeled
            FillAllTargetsInArea(tmpMap,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i));
                }break;

        case 31: { // related to scripted effects
            FillAllTargetsInArea(tmpMap,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i));
                 }break;
        case 32:{// Minion Target
            if(m_caster->GetUInt64Value(UNIT_FIELD_SUMMON) == 0)
                SafeAddTarget(tmpMap,m_caster->GetGUID());
            else
                SafeAddTarget(tmpMap,m_caster->GetUInt64Value(UNIT_FIELD_SUMMON));
                }break;
        case 33:{//Party members of totem, inside given range
            if(u_caster)
            {
                if(u_caster->GetTypeId()==TYPEID_UNIT)
                {
                    if(((Creature*)u_caster)->IsTotem())
                    {
                        float r =GetRadius(i);
                        r*=r;

                        Player*p=(Player*) ((Creature*)u_caster)->GetTotemOwner();

                        if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),p,r))
                            SafeAddTarget(tmpMap,p->GetGUID());
                        
                        if( p->GetSubGroup())
                        {
                            for(GroupMembersSet::iterator itr = p->GetSubGroup()->GetGroupMembersBegin(); itr != p->GetSubGroup()->GetGroupMembersEnd(); ++itr)
                            {
                                if(p == (*itr)) 
                                    continue;
                                if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),(*itr),r))
                                SafeAddTarget(tmpMap,(*itr)->GetGUID());
                            }
                        }
                    }
                }
            }
                }break;
        case 35:{// Single Target Party Member
            Unit* Target = sObjHolder.GetObject<Player>(m_targets.m_unitTarget);
            if(!Target)
                break;
            float r=GetMaxRange(sSpellRange.LookupEntry(m_spellInfo->rangeIndex));
            if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),Target,r*r))
                SafeAddTarget(tmpMap,m_targets.m_unitTarget);
                }break;
        case 37:{ // all Members of the targets party
                  // if no group target self
            Player * Target = sObjHolder.GetObject<Player>(m_targets.m_unitTarget);
            if(!Target)
                break;
            
            if(SubGroup* subgroup = Target->GetSubGroup())
            {
                for(GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr)
                      SafeAddTarget(tmpMap,(*itr)->GetGUID());
            }
            else
            {
                SafeAddTarget(tmpMap,Target->GetGUID());
            }}break;
        case 38:{//Dummy Target
            if(m_spellInfo->Id == 12938)
            {
                //FIXME:this ll be immortal targets
                FillAllTargetsInArea(tmpMap,m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,GetRadius(i));
            }
            SafeAddTarget(tmpMap,m_caster->GetGUID());
                }break;
        case 39:{//Fishing
            SafeAddTarget(tmpMap,m_caster->GetGUID());
                }break;
        case 40:{//Activate Object target(probably based on focus)

                }
        case 41:
        case 42:
        case 43:
        case 44:{// Totem
            SafeAddTarget(tmpMap,m_caster->GetGUID());
            /*
            if( m_caster->GetTypeId() == TYPEID_UNIT )
            {
                if( ((Creature *)m_caster)->isTotem() )
                {
                    printf("SPELL: A totem tried to cast a spell !!1\n");
                }
            }*/
                }break;
        case 45:{// Chain,!!only for healing!! for chain lightning =6 
            //if selected target is party member, then jumps on party
            Unit* firstTarget;

            bool PartyOnly=false;
            float range=GetMaxRange(sSpellRange.LookupEntry(m_spellInfo->rangeIndex));//this is probably wrong,
            //this is cast distance, not searhing distance
            range *= range;

            firstTarget = sObjHolder.GetObject<Player>(m_targets.m_unitTarget);
            if(firstTarget)
            {
                if(p_caster->InGroup())
                    if(p_caster->GetSubGroup()==((Player*)firstTarget)->GetSubGroup())
                        PartyOnly=true;                    
            }
            else
            {
                firstTarget = objmgr.GetUnit(m_targets.m_unitTarget);
                if(!firstTarget) 
                    break;
            }
            
            uint32 jumps=m_spellInfo->EffectChainTarget[i];
            if(m_spellInfo->SpellGroupType && u_caster)
            {
                SM_FIValue(u_caster->SM_FAdditionalTargets,(int32*)&jumps,m_spellInfo->SpellGroupType);
            }
            SafeAddTarget(tmpMap,firstTarget->GetGUID());
            if(!jumps)
                break;
            jumps--;
            if(PartyOnly)
            {
                GroupMembersSet::iterator itr;
                for(itr = p_caster->GetSubGroup()->GetGroupMembersBegin(); itr != p_caster->GetSubGroup()->GetGroupMembersEnd(); ++itr)
                {
                    if((*itr)==u_caster)
                        continue;
                    if(IsInrange(u_caster->GetPositionX(),u_caster->GetPositionY(),u_caster->GetPositionZ(),(*itr), range))
                    {
                        SafeAddTarget(tmpMap,(*itr)->GetGUID());
                        if(!--jumps)
                            break;
                    }
                }
            }//find nearby friendly target
            else
            {
                std::set<Object*>::iterator itr;
                for( itr = firstTarget->GetInRangeSetBegin(); itr != firstTarget->GetInRangeSetEnd(); itr++ )
                {
                    if( !(*itr)->IsUnit() || !((Unit*)(*itr))->isAlive())
                        continue;

                    if(IsInrange(firstTarget->GetPositionX(),firstTarget->GetPositionY(),firstTarget->GetPositionZ(),*itr, range))
                    {
                        if(!isAttackable(u_caster,(Unit*)(*itr)))
                        {
                            SafeAddTarget(tmpMap,(*itr)->GetGUID());
                            if(!--jumps)
                                break;
                        }
                    }
                }
            }
                }break;
        case 46:{//Unknown Summon Atal'ai Skeleton
            SafeAddTarget(tmpMap,m_caster->GetGUID());
                }break;
        case 47:{// Portal
            //FIXME: Not sure
            SafeAddTarget(tmpMap,m_caster->GetGUID());
                }break;
        case 52:    // Lightwells, etc
            {
                SafeAddTarget(tmpMap, m_caster->GetGUID());
            }break;
        case 53:{// Target Area by Players CurrentSelection()
            Unit *Target = objmgr.GetUnit(p_caster->GetSelection());
            if(!Target)
                break;
            FillAllTargetsInArea(tmpMap,Target->GetPositionX(),Target->GetPositionY(),Target->GetPositionZ(),GetRadius(i));
                }break;
        case 57:{// Targeted Party Member
            Unit* Target = sObjHolder.GetObject<Player>(m_targets.m_unitTarget);
            if(!Target)
                break;

            float r=GetMaxRange(sSpellRange.LookupEntry(m_spellInfo->rangeIndex));
            if(IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),Target,r*r))
                SafeAddTarget(tmpMap,m_targets.m_unitTarget);
                }break;
        case 61:{ // targets with the same group/raid and the same class
            Player * Target = sObjHolder.GetObject<Player>(m_targets.m_unitTarget);
            if(!Target)
                break;
            
            if(SubGroup* subgroup = Target->GetSubGroup())
                for(GroupMembersSet::iterator itr = subgroup->GetGroupMembersBegin(); itr != subgroup->GetGroupMembersEnd(); ++itr)
                {
                    if(Target->getClass() != (*itr)->getClass()) 
                        continue;
                    SafeAddTarget(tmpMap,(*itr)->GetGUID());
                }
                }break;
            }
        }
}


void Spell::prepare(SpellCastTargets * targets)
{
    chaindamage=0;
    m_targets = *targets;

    m_castTime = GetCastTime(sCastTime.LookupEntry(m_spellInfo->CastingTimeIndex));

    if (m_spellInfo->SpellGroupType && u_caster)
    {
        SM_FIValue(u_caster->SM_FCastTime,(int32*)&m_castTime,m_spellInfo->SpellGroupType);
        SM_PIValue(u_caster->SM_PCastTime,(int32*)&m_castTime,m_spellInfo->SpellGroupType);
    }

    // handle MOD_CAST_TIME
    if(u_caster)
    {
        m_castTime *= u_caster->GetFloatValue(UNIT_MOD_CAST_SPEED);
    }

    if(p_caster)
    {
        if(p_caster->cannibalize)
        {
            sEventMgr.RemoveEvents(p_caster, EVENT_CANNIBALIZE);
            p_caster->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
            p_caster->cannibalize = false;
        }
    }

    if(!m_triggeredSpell && p_caster && p_caster->CastTimeCheat)
        m_castTime = 0;

    m_timer = m_castTime;
    if(p_caster)
        m_castTime -= 100;      // session update time

    SendSpellStart();

    if(!m_triggeredSpell && p_caster && p_caster->CooldownCheat)
        p_caster->ClearCooldownForSpell(m_spellInfo->Id);

    m_spellState = SPELL_STATE_PREPARING;
    
    if(m_triggeredSpell)
           cancastresult = -1;
      else
           cancastresult = CanCast(false);

    if(cancastresult != -1)
    {
        SendCastResult(cancastresult);

        if(m_triggeredByAura)
        {
            SendChannelUpdate(0);
            if(u_caster)
                u_caster->RemoveAura(m_triggeredByAura);
        }
        else
        {
            // HACK, real problem is the way spells are handled
            // when a spell is channeling and a new spell is casted
            // that is a channeling spell, but not triggert by a aura
            // the channel bar/spell is bugged
            if(u_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT) > 0 && u_caster->GetCurrentSpell())
            {
                u_caster->GetCurrentSpell()->cancel();
                SendChannelUpdate(0);
                cancel();
                return;
            }
        }
        finish();
        return;
    }
    else
    {
        // start cooldown handler
        if(p_caster && !m_triggeredSpell)
        {
            if(m_spellInfo->StartRecoveryTime > 0)
                p_caster->AddGlobalCooldown(m_spellInfo->StartRecoveryTime);

            if(m_spellInfo->StartRecoveryCategory > 0)
                p_caster->AddCategoryCooldown(m_spellInfo->Category, m_spellInfo->StartRecoveryCategory);
        }

        if (i_caster)
        {            
            ItemPrototype *proto = NULL;
            proto = i_caster->GetProto();
            if (proto->Class == ITEM_CLASS_CONSUMABLE || proto->Class == ITEM_CLASS_RECIPE
                ||proto->Class == ITEM_CLASS_TRADEGOODS || proto->Flags & 2)//flag&2 --conjured item
            {
                if (((int32)i_caster->GetUInt32Value(ITEM_FIELD_SPELL_CHARGES)) <= 1)//number of charges might be -1,->remove
                {
                    if (i_caster->GetUInt32Value(ITEM_FIELD_STACK_COUNT) <= 1)
                    {
                        if(i_caster->GetProto()->ItemId != 5507) //Ornate SpyGlass
                        {
                              bool result = i_caster->GetOwner()->GetItemInterface()->SafeFullRemoveItemByGuid(i_caster->GetGUID());
                            if(!result)
                            {
                                //should never get here
                                printf("Spell: Prepare, Item destruction failed");
                                this->cancel();
                                return;
                            }
                        }
                    }
                    else
                    {
                        i_caster->SetUInt32Value(ITEM_FIELD_SPELL_CHARGES, proto->SpellCharges[0] );
                        i_caster->ModUInt32Value(ITEM_FIELD_STACK_COUNT,-1);
                        //i_caster->Update();
                    }
                }
                else
                {
                    i_caster->ModUInt32Value(ITEM_FIELD_SPELL_CHARGES,-1);
                }
            }
            else if(proto->Class == ITEM_CLASS_QUEST )
            {                    
                if(proto->SpellCharges[0]!=-1)
                {
                    int32 cha=i_caster->GetUInt32Value(ITEM_FIELD_SPELL_CHARGES);
                    //0 - remove, -1 don't remove otherwise inc count
                    if(cha)
                    {
                        i_caster->ModUInt32Value(ITEM_FIELD_SPELL_CHARGES,1);
                        //i_caster->Update ();
                    }
                        
                    if(!cha && proto->Class != ITEM_CLASS_QUEST)    // quest items shouldn't disappear..
                    {
                        bool result = i_caster->GetOwner()->GetItemInterface()->SafeFullRemoveItemByGuid(i_caster->GetGUID());
                        if(!result)
                        {
                            //should never get here
                            printf("Spell: Prepare, Item destruction failed");
                            this->cancel();
                            return;
                        }
                        i_caster=NULL;
                    }
                }
            }
        }
        else
        {
            if(p_caster && m_timer > 0)
                p_caster->setAttackTimer(m_timer + 1000, false);
        }
    }

    //instant cast(or triggered) and not channeling
    if(u_caster && (m_castTime>0 || m_spellInfo->ChannelInterruptFlags) && !m_triggeredSpell)    
    {
        m_castPositionX = m_caster->GetPositionX();
        m_castPositionY = m_caster->GetPositionY();
        m_castPositionZ = m_caster->GetPositionZ();
    
        u_caster->castSpell( this );
    }
    else
        cast(false);
}

void Spell::cancel()
{
    SendInterrupted(0);
    SendCastResult(SPELL_FAILED_INTERRUPTED);

    if(m_spellState == SPELL_STATE_CASTING)
    {
        if(u_caster)       
            u_caster->RemoveAura(m_spellInfo->Id);
    
        if(m_timer > 0 || m_delayed)
        {
            if(p_caster)
            {
                Unit *pTarget = objmgr.GetCreature(m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT));
                if(!pTarget)
                    pTarget = objmgr.GetPlayer(m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT));
                if(!pTarget)
                    pTarget = (Creature*)objmgr.GetCreature(p_caster->GetSelection());
                if(!pTarget)
                    pTarget = objmgr.GetPlayer(p_caster->GetSelection());
                
                if(pTarget)
                {
                    pTarget->RemoveAura(m_spellInfo->Id, m_caster->GetGUID());
                }
                if(m_AreaAura)//remove of blizz and shit like this
                {
                    uint64 guid=m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT);
                    DynamicObject* dynObj=sObjHolder.GetObject<DynamicObject>(guid);
                    if(dynObj)
                    {
                        if(dynObj->IsInWorld())
                            dynObj->RemoveFromWorld();
                        sObjHolder.Delete(dynObj);
                    }
                }

                if(p_caster->GetSummonedObject())
                {
                    if(p_caster->GetSummonedObject()->IsInWorld())
                        p_caster->GetSummonedObject()->RemoveFromWorld();
                    // for now..
                    ASSERT(p_caster->GetSummonedObject()->GetTypeId() == TYPEID_GAMEOBJECT);
                    sObjHolder.Delete<GameObject>(static_cast<GameObject*>(p_caster->GetSummonedObject()));
                    p_caster->SetSummonedObject(NULL);
                }
                p_caster->setAttackTimer(1000, false);
             }                    
        }
        SendChannelUpdate(0);
    }

    //m_spellState = SPELL_STATE_FINISHED;
    finish();
}

void Spell::AddCooldown()
{
    if(p_caster)
    {
        if(m_spellInfo->RecoveryTime > 0)
            p_caster->AddCooldown(m_spellInfo->Id,m_spellInfo->RecoveryTime);
        if(m_spellInfo->CategoryRecoveryTime > 0)
            p_caster->AddCategoryCooldown(m_spellInfo->Category,m_spellInfo->CategoryRecoveryTime);
    }
}

void Spell::cast(bool check)
{
    sLog.outDebug("Spell::cast %u, Unit: %u", m_spellInfo->Id, m_caster->GetGUID());
    if(check)
    {
        /*if(m_caster->IsPet())
            castResult = -1;
        else*/
            cancastresult = CanCast(true);
    }
    else 
        cancastresult = -1;

    if(cancastresult == -1)
    {
        if(!m_triggeredSpell)
        {
            if(!TakePower()) //not enough mana
            {
                sLog.outDebug("Spell::Not Enough Mana");
                SendInterrupted(SPELL_FAILED_NO_POWER);
                finish();
                return; 
            }
        }

        SendCastResult(cancastresult);
        if(!m_triggeredSpell)
            AddCooldown();
        
        for(uint32 i=0;i<3;i++)
            if(m_spellInfo->Effect[i] && m_spellInfo->Effect[i]!=27)
                 FillTargetMap(i);
        

        /*if (m_spellInfo->RecoveryTime && m_caster->IsPlayer() && m_caster->IsInWorld())
        {
            WorldPacket data(SMSG_SPELL_COOLDOWN, 15);
            data << (uint32)m_spellInfo->Id << m_caster->GetNewGUID();
            data << (uint16)m_spellInfo->RecoveryTime; // from dbc
            ((Player*)m_caster)->GetSession()->SendPacket (&data);
        }*/

        /*SpellExtraInfo* sp = objmgr.GetSpellExtraData(m_spellInfo->Id);
        if(sp)
        {
            Unit *Target = objmgr.GetUnit(m_targets.m_unitTarget);
            if(Target)
                Target->RemoveBySpecialType(sp->specialtype, p_caster->GetGUID());
        }*/

        if(!(m_spellInfo->Attributes & ATTRIBUTE_ON_NEXT_ATTACK  && !m_triggeredSpell))//on next attack
        {
            SendSpellGo();

            //******************** SHOOT SPELLS ***********************
            //* Flags are now 1,4,19,22 (4718610) //0x480012

            if (m_spellInfo->field10 & 0x8000 && m_caster->IsPlayer() && m_caster->IsInWorld())
            {
                WorldPacket data(SMSG_SPELL_COOLDOWN, 15);
                data << (uint32)m_spellInfo->Id << m_caster->GetNewGUID();
                if (m_spellInfo->RecoveryTime)
                    data << (uint16)m_spellInfo->RecoveryTime;
                else
                    data << (uint16)(2300); // hack fix for shoot spells, should be some other resource for it

                ((Player*)m_caster)->GetSession()->SendPacket (&data);
                
                //Captnoord: Hack fix for the channel error on wands
                // this needs to be properly fixed!!!!
            }
            else
            {            
                if(m_spellInfo->ChannelInterruptFlags != 0 && !m_triggeredSpell)
                {
                    m_spellState = SPELL_STATE_CASTING;
                    SendChannelStart(GetDuration());
                    if(p_caster)
                    {
                        //Use channel interrupt flags here
                        if(m_targets.m_targetMask == TARGET_FLAG_DEST_LOCATION || m_targets.m_targetMask == TARGET_FLAG_SOURCE_LOCATION)
                            u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSelection());                    
                        else if(p_caster->GetSelection() == m_caster->GetGUID())
                        {
                            if(p_caster->GetSummon())
                                u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSummon()->GetGUID());
                            else if(m_targets.m_unitTarget)
                                u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, m_targets.m_unitTarget);
                            else
                                u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSelection());
                        }
                        else
                        {
                            if(p_caster->GetSelection())
                                u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSelection());
                            else if(p_caster->GetSummon())
                                u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, p_caster->GetSummon()->GetGUID());
                            else if(m_targets.m_unitTarget)
                                u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, m_targets.m_unitTarget);
                            else
                            {
                                cancel();
                                return;
                            }
                        }
                    }
                }
            }

            std::vector<uint64>::iterator i;
            
            // this is here to avoid double search in the unique list
            bool canreflect = false, reflected = false;
            for(int j=0;j<3;j++)
            {
                switch(m_spellInfo->EffectImplicitTargetA[j])
                {
                    case 6:
                    //case 15:
                    //case 16:
                    case 22:
                    case 24:
                    case 25:
                    //case 28:
                        canreflect = true;
                        break;
                }
                if(canreflect)
                    continue;
                else
                    break;
            }

            if(canreflect)
            {
                for(i= UniqueTargets.begin();i != UniqueTargets.end();i++)
                {
                    Unit *Target = objmgr.GetUnit(*i);
                    if(Target)
                       reflected = reflect(Target);
                }
            }

            if(!reflected)
            {
                for(uint32 x=0;x<3;x++)
                if(m_spellInfo->Effect[x])
                {
                    if(m_spellInfo->Effect[x] == SPELL_EFFECT_PERSISTENT_AREA_AURA)
                        HandleEffects(m_caster->GetGUID(),x);
                    else  
                        for(i= m_targetUnits[x].begin();i != m_targetUnits[x].end();i++)
                            HandleEffects((*i),x);
                }
    
                for(i= UniqueTargets.begin();i != UniqueTargets.end();i++)
                {
                    HandleAddAura((*i));
                }
                
            }
            // we're much better to remove this here, because otherwise spells that change powers etc,
            // don't get applied.

            if(u_caster && !m_triggeredSpell && !m_triggeredByAura)
                u_caster->RemoveAurasByInterruptFlagButSkip(AURA_INTERRUPT_ON_CAST_SPELL, m_spellInfo->Id);

            if(m_spellState != SPELL_STATE_CASTING)
                finish();
        } 
        else //this shit has nothing to do with instant, this only means it will be on NEXT melee hit
        {
            // we're much better to remove this here, because otherwise spells that change powers etc,
            // don't get applied.

            if(u_caster && !m_triggeredSpell && !m_triggeredByAura)
                u_caster->RemoveAurasByInterruptFlagButSkip(AURA_INTERRUPT_ON_CAST_SPELL, m_spellInfo->Id);

            //not sure if it must be there...
            //sEventMgr.RemoveEvents(m_caster, EVENT_PLAYER_REPEAT_SPELL);
            SendCastResult(cancastresult);
            if(u_caster)
            if (u_caster->GetOnMeleeSpell() != m_spellInfo->Id)
                u_caster->SetOnMeleeSpell(m_spellInfo->Id);
            finish();
        }
    }
    else
    {
        SendCastResult(cancastresult);
        finish();
    }
}

void Spell::AddTime()
{    
    if(m_spellInfo->SpellGroupType && u_caster)
    {
        float ch=0;
        SM_FFValue(u_caster->SM_PNonInterrupt,&ch,m_spellInfo->SpellGroupType);
        if(Rand(ch))
            return;
    }
    if(m_spellState==SPELL_STATE_PREPARING)
    {
        uint32 delay = m_castTime/4;
        m_timer+=delay;
        if(m_timer>m_castTime)
            m_timer=m_castTime;
        
        WorldPacket data(SMSG_SPELL_DELAYED, 13);
        data << u_caster->GetNewGUID();
        data << uint32(delay);
        u_caster->SendMessageToSet(&data, true);
        
        if(!p_caster)
        {
            if(m_caster->GetTypeId() == TYPEID_UNIT)
                u_caster->GetAIInterface()->AddStopTime(delay);
        }
    }
    else if(m_spellInfo->ChannelInterruptFlags != 48140)
    {        
        m_timer-=GetDuration()/3;
        m_delayed = true;
        if(m_timer>0)
            SendChannelUpdate(m_timer);
        
    }
}

void Spell::update(uint32 difftime)
{
    // skip cast if we're more than 2/3 of the way through
    if(
        (((float)m_castTime / 1.5f) > (float)m_timer ) && 
//        float(m_castTime)/float(m_timer) >= 2.0f        &&
        (
        m_castPositionX != m_caster->GetPositionX() ||
        m_castPositionY != m_caster->GetPositionY() ||
        m_castPositionZ != m_caster->GetPositionZ()
        )
        )
    {    
        if(u_caster)
        {
            if(u_caster->HasNoInterrupt() == 0)
            {
                cancel();
                return;
            }
        }
    }

    switch(m_spellState)
    {
    case SPELL_STATE_PREPARING:
        {
            if(difftime >= m_timer)
                cast(true);
            else 
            {
                m_timer -= difftime;
                if(difftime >= m_timer)
                {
                    m_timer = 0;
                    cast(true);
                }
            }
            
            
        } break;
    case SPELL_STATE_CASTING:
        {
            if(m_timer > 0)
            {
                if(difftime >= m_timer)
                    m_timer = 0;
                else
                    m_timer -= difftime;
            }
            if(m_timer <= 0)
            {
                SendChannelUpdate(0);
                finish();
            }
        } break;
    }
}

void Spell::finish()
{
    m_spellState = SPELL_STATE_FINISHED;
    if(u_caster)
    {
        u_caster->m_canMove = true;
 
        if(m_usesMana && !failed) 
            u_caster->setRegenTimer(5000); /* 5 Seconds */
    }
    /* Mana Regenerates while in combat but not for 5 seconds after each spell */
    /* Only if the spell uses mana, will it cause a regen delay.
       is thi
s correct? is there any spell that doesn't use mana that does cause a delay?
       this is for creatures as effects like chill (when they have frost armor on) prevents regening of mana    */
    
    //moved to spellhandler.cpp -> remove item when click on it! not when it finishes 

    //enable pvp when attacking another player with spells
    if(p_caster)
    {
        if(m_requiresCP && !failed)
        {
               p_caster->SetCP(0);
        }
        if(m_delayed)
        {
            Unit *pTarget = objmgr.GetCreature(m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT));
            if(!pTarget)
                pTarget = objmgr.GetPlayer(m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT));
            if(!pTarget)
                pTarget = (Creature*)objmgr.GetCreature(p_caster->GetSelection());
            if(!pTarget)
                pTarget = objmgr.GetPlayer(p_caster->GetSelection());
            
            if(pTarget)
            {
                pTarget->RemoveAura(m_spellInfo->Id, m_caster->GetGUID());
            }
        }
    }

    if(m_spellInfo->Effect[0] == SPELL_EFFECT_SUMMON_OBJECT ||
        m_spellInfo->Effect[1] == SPELL_EFFECT_SUMMON_OBJECT ||
        m_spellInfo->Effect[2] == SPELL_EFFECT_SUMMON_OBJECT)
        if(p_caster)
            p_caster->SetSummonedObject(NULL);        
    /*
    We set current spell only if this spell has cast time or is channeling spell
    otherwise it's instant spell and we delete it right after completion
    */

    if(u_caster)
    {
        if(!m_triggeredSpell && (m_spellInfo->ChannelInterruptFlags || m_castTime>0))
            u_caster->SetCurrentSpell(NULL);

        delete this;
    }
}

void Spell::SendCastResult(int8 result)
{
    if(result != -1)
        failed = true;

    if(!m_caster->IsInWorld())
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data(SMSG_CAST_RESULT, 6);
    data << m_spellInfo->Id;

    if(result != -1)
    {
        data << uint8(2);
        data << (uint8)result;
        if(result == SPELL_FAILED_REQUIRES_SPELL_FOCUS)
            data << (uint32)m_spellInfo->RequiresSpellFocus;
    }
    else
        data << uint8(0);
    ((Player*)m_caster)->GetSession()->SendPacket(&data);
}

void Spell::SendSpellStart()
{
    // no need to send this on passive spells
    if(!m_caster->IsInWorld() || m_spellInfo->Attributes & 64 || m_triggeredSpell)
        return;

    WorldPacket data(150);

    uint16 cast_flags = 2;

    if(GetType() == SPELL_TYPE_RANGED)
        cast_flags |= 0x20;

    data.SetOpcode(SMSG_SPELL_START);
    if(i_caster)
        data << i_caster->GetNewGUID() << u_caster->GetNewGUID();
    else
        data << m_caster->GetNewGUID() << m_caster->GetNewGUID();

    data << m_spellInfo->Id;
    data << cast_flags;
    data << (uint32)m_castTime;
        
    m_targets.write( data );
    if(GetType() == SPELL_TYPE_RANGED && p_caster)
    {
        ItemPrototype* ip = NULL;
        switch(m_spellInfo->Id)
        {
        case SPELL_RANGED_THROW:  // throw
            {
                if(Item *itm = p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED))
                    ip=itm->GetProto();
            }break;
        default:
            {
                ip = objmgr.GetItemPrototype(p_caster->GetUInt32Value(PLAYER_AMMO_ID));
            }break;
        }
        
        if(ip)
            data << ip->DisplayInfoID << ip->InventoryType;
    }

    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendSpellGo()
{
    // Fill UniqueTargets
    TargetsList::iterator i,j;
    for(uint32 x=0;x<3;x++)
    {
        if(m_spellInfo->Effect[x])
        {
            for ( i = m_targetUnits[x].begin(); i != m_targetUnits[x].end(); i++ )
            {
                bool add=true;

                for(j = UniqueTargets.begin(); j != UniqueTargets.end(); j++ )
                {
                    if((*j) == (*i))
                    {
                        add = false;
                        break;
                    }
                }
                if(add)
                    UniqueTargets.push_back((*i));
            }
        }
    }
    
    // no need to send this on passive spells
    if(!m_caster->IsInWorld() || m_spellInfo->Attributes & 64)
        return;

    // Start Spell
    WorldPacket data(200);
    data.SetOpcode(SMSG_SPELL_GO);
    uint16 flags = 0;

    if (GetType() == SPELL_TYPE_RANGED) //ranged
        flags |= 0x20;                  // 0x20 RANGED

    if(i_caster)
        flags |= 0x100;                // 0x100 UNIT TARGET

    if(MissedTargets.size() > 0)
        flags |= 0x400;                 //0x400 TARGET MISSES AND OTHER MESSAGES LIKE "Resist"

    if(i_caster && u_caster)            // this is needed for correct cooldown on items
    {
        data << i_caster->GetNewGUID() << u_caster->GetNewGUID();
    } 
    else 
    {
        data << m_caster->GetNewGUID() << m_caster->GetNewGUID();
    }

    data << m_spellInfo->Id;
    data << flags;
    data << (uint8)(UniqueTargets.size()); //number of hits
    
    if (flags & 0x400)
        data << (uint8)(MissedTargets.size()); //number if misses
  
    writeSpellGoTargets(&data);
    
    if (flags & 0x400)
        writeSpellMissedTargets(&data);
    else
        data << uint8(0);

    m_targets.write( data ); // this write is included the target flag

    if (GetType() == SPELL_TYPE_RANGED && p_caster)//ranged
    {
        ItemPrototype* ip=NULL;
        switch(m_spellInfo->Id)
        {
        case SPELL_RANGED_THROW:  // throw
            {
                   Item * it=p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
                if(it)
                {
                    ip = it->GetProto();
                       p_caster->GetItemInterface()->RemoveItemAmt(it->GetEntry(),1);
                }
            }break;
        default:
            {
                ip = objmgr.GetItemPrototype(p_caster->GetUInt32Value(PLAYER_AMMO_ID));
                // removing taken care of in RemoveItems()
            }break;
        }
        if(ip)
            data << ip->DisplayInfoID << ip->InventoryType;
    }
   
    m_caster->SendMessageToSet(&data, true);

    
    // Captnoord:spell log execute is still send 1.12
    // as I see with this combination, need to test it more
    //if (flags != 0x120 && m_spellInfo->Attributes & 16) // not ranged and flag 5
      //  SendLogExecute(0,m_targets.m_unitTarget);
}

void Spell::writeSpellGoTargets( WorldPacket * data )
{
    TargetsList::iterator i;
    for ( i = UniqueTargets.begin(); i != UniqueTargets.end(); i++ )
        *data << (*i);
}

void Spell::writeSpellMissedTargets( WorldPacket * data )
{
    /*
     * The flags at the end known to us so far are.
     * 1 = Miss
     * 2 = Resist
     * 3 = Dodge
     * 4 = Deflect
     * 5 = Block
     * 6 = Evade
     * 7 = Immune
     */
    TargetsList::iterator i;
    for ( i = MissedTargets.begin(); i != MissedTargets.end(); i++ ) {
        *data << (*i);
        *data << (uint8)1;
    }
}

void Spell::SendLogExecute(uint32 damage, uint64 & targetGuid)
{
    WorldPacket data(SMSG_SPELLLOGEXECUTE, 37);
    data << m_caster->GetNewGUID();
    data << m_spellInfo->Id;
    data << uint32(1);
    data << m_spellInfo->SpellVisual;
    data << uint32(1);
    if (m_caster->GetGUID() != targetGuid)
        data << targetGuid;
    if (damage)
        data << damage;
    m_caster->SendMessageToSet(&data,true);
}

void Spell::SendInterrupted(uint8 result)
{
    if(result != (uint8)-1)
        failed = true;
    WorldPacket data(SMSG_SPELL_FAILURE, 20);

    // send the failure to pet owner if we're a pet
    Player *plr = p_caster;
    if(!plr && m_caster->IsPet())
        plr = static_cast<Pet*>(m_caster)->GetPetOwner();

    if(plr)
    {
        data << m_caster->GetNewGUID();
        data << m_spellInfo->Id;
        data << result;
        plr->GetSession()->SendPacket(&data);
    }

    data.Initialize(SMSG_SPELL_FAILED_OTHER);
    data << m_caster->GetNewGUID();
    data << m_spellInfo->Id;
    m_caster->SendMessageToSet(&data, false);
}

void Spell::SendChannelUpdate(uint32 time)
{
    if(time == 0)
    {
        if(u_caster)
        {
            uint64 guid=m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT);
            DynamicObject* dynObj=sObjHolder.GetObject<DynamicObject>(guid);
            if(dynObj)
            {
                if(dynObj->IsInWorld())
                    dynObj->RemoveFromWorld();
                sObjHolder.Delete(dynObj);
            }
            u_caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT,0);
            u_caster->SetUInt32Value(UNIT_CHANNEL_SPELL,0);
        }
    }

    if (!p_caster)
        return;

    WorldPacket data(MSG_CHANNEL_UPDATE, 18);
    data << p_caster->GetNewGUID();
    data << time;

    p_caster->SendMessageToSet(&data, true);    
}

void Spell::SendChannelStart(uint32 duration)
{
    if (m_caster->GetTypeId() != TYPEID_GAMEOBJECT)
    {
        // Send Channel Start
        WorldPacket data(MSG_CHANNEL_START, 22);
        data << m_caster->GetNewGUID();
        data << m_spellInfo->Id;
        data << duration;
        m_caster->SendMessageToSet(&data, true);
    }

    m_castTime = m_timer = duration;

    if(u_caster)
        u_caster->SetUInt32Value(UNIT_CHANNEL_SPELL,m_spellInfo->Id);

    /*
    Unit* target = objmgr.GetObject<Creature>(((Player*)m_caster)->GetSelection());
    if(!target)
        target = objmgr.GetObject<Player>(((Player*)m_caster)->GetSelection());
    if(!target)
        return;
 
    m_caster->SetUInt32Value(UNIT_FIELD_CHANNEL_OBJECT,target->GetGUIDLow());
    m_caster->SetUInt32Value(UNIT_FIELD_CHANNEL_OBJECT+1,target->GetGUIDHigh());
    //disabled by Phantomas, it can be not only creature but GO as well
    //and GO is not selectable, so this method will not work
    //these fields must be filled @ place of call
    */
}

void Spell::SendResurrectRequest(Player* target)
{
    WorldPacket data(SMSG_RESURRECT_REQUEST, 13);
    data << m_caster->GetGUID();
    data << uint32(0) << uint8(0);

    target->GetSession()->SendPacket(&data);
}

bool Spell::TakePower()
{
    int32 powerField;
    if(u_caster)
    if(u_caster->HasFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_TRAINER))
        return true;

    if(p_caster && p_caster->PowerCheat)
        return true;

    switch(m_spellInfo->powerType)
    {
        case POWER_TYPE_HEALTH:{
            powerField = UNIT_FIELD_HEALTH;
                               }break;
        case POWER_TYPE_MANA:{
            powerField = UNIT_FIELD_POWER1;
            m_usesMana=true;
                             }break;
        case POWER_TYPE_RAGE:{
            powerField = UNIT_FIELD_POWER2;
                             }break;
        case POWER_TYPE_FOCUS:{
            powerField = UNIT_FIELD_POWER3;
                              }break;
        case POWER_TYPE_ENERGY:{
            powerField = UNIT_FIELD_POWER4;
                               }break;
        default:{
            sLog.outDebug("unknown power type");
                }break;
    }

    //FIXME: add handler for UNIT_FIELD_POWER_COST_MODIFIER
    //UNIT_FIELD_POWER_COST_MULTIPLIER
    if(u_caster)
    {
        if( m_spellInfo->AttributesEx & 2 ) // Uses %100 mana
        {
            m_caster->SetUInt32Value(powerField, 0);
            return true;
        }
    }
       
    uint32 currentPower = m_caster->GetUInt32Value(powerField);

    int32 cost;
    if(m_spellInfo->ManaCostPercentage)//Percentage spells cost % of !!!BASE!!! mana
    {
        if(m_spellInfo->powerType==POWER_TYPE_MANA)
            cost = (m_caster->GetUInt32Value(UNIT_FIELD_BASE_MANA)*m_spellInfo->ManaCostPercentage)/100;
        else
            cost = (m_caster->GetUInt32Value(UNIT_FIELD_BASE_HEALTH)*m_spellInfo->ManaCostPercentage)/100;
    }
    else 
    {
        cost = m_spellInfo->manaCost;
    }
    
    if(m_spellInfo->powerType==POWER_TYPE_HEALTH)
            cost -= m_spellInfo->baseLevel;//FIX for life tap    
    else if(u_caster)
    {
        if(m_spellInfo->powerType==POWER_TYPE_MANA)
            cost += (cost*u_caster->PowerCostMod[m_spellInfo->School])/100;
        else
            cost += (cost*u_caster->PowerCostMod[0])/100;
    }

    //apply modifiers
    if(m_spellInfo->SpellGroupType && u_caster)
    {
          SM_FIValue(u_caster->SM_FCost,&cost,m_spellInfo->SpellGroupType);
          SM_PIValue(u_caster->SM_PCost,&cost,m_spellInfo->SpellGroupType);
    }
         
    if (cost <=0)
        return true;

    //FIXME:DK:if field value < cost what happens
    if(powerField == UNIT_FIELD_HEALTH)
    {
        m_caster->DealDamage(u_caster, cost, 0, 0, 0);
        return true;
    }
    else
    {
        if(cost <= currentPower) // Unit has enough power (needed for creatures)
        {
            m_caster->SetUInt32Value(powerField, currentPower - cost);
            return true;
        }
        else
            return false;     
    } 
}

void Spell::HandleEffects(uint64 guid, uint32 i)
{   
    unitTarget = NULL;
    switch(UINT32_LOPART(GUID_HIPART(guid)))
    {
    case HIGHGUID_UNIT:
        unitTarget = sObjHolder.GetObject<Creature>(guid);
        break;
    case HIGHGUID_PET:
        unitTarget = sObjHolder.GetObject<Pet>(guid);
        break;
    case HIGHGUID_PLAYER:
        unitTarget = sObjHolder.GetObject<Player>(guid);
        playerTarget = static_cast<Player*>(unitTarget);
        break;
    case HIGHGUID_ITEM:
        itemTarget = p_caster->GetItemInterface()->GetItemByGUID(guid);
        break;
    case HIGHGUID_GAMEOBJECT:
        gameObjTarget = sObjHolder.GetObject<GameObject>(guid);
        break;
    }

    damage = CalculateEffect(i);  

    sLog.outDebug( "WORLD: Spell effect id = %u, damage = %u", m_spellInfo->Effect[i], damage); 

    if(m_spellInfo->Effect[i] != SPELL_EFFECT_DUEL)
    {
        // Handle proc effects on this target
        if(unitTarget && u_caster && !m_triggeredSpell && unitTarget != u_caster)
            u_caster->HandleProc(PROC_ON_CAST_SPELL | PROC_ON_CAST_SPECIFIC_SPELL, unitTarget, m_spellInfo);
    }
    
    if(m_spellInfo->Effect[i]<TOTAL_SPELL_EFFECTS)
         (*this.*SpellEffectsHandler[m_spellInfo->Effect[i]])(i);
    else
        sLog.outError("SPELL: unknown effect %u spellid %u",m_spellInfo->Effect[i], m_spellInfo->Id);
}

void Spell::HandleAddAura(uint64 guid)
{
    Unit *Target = World::GetUnit(guid);
    if(!Target)
        return;

    // Applying an aura to a flagged target will cause you to get flagged.
    if(Target->IsPlayer() && p_caster)
    {
        if(static_cast<Player*>(Target)->IsPvPFlagged())
            p_caster->SetPvPFlag();
    }

    // remove any auras with same type
    if(m_spellInfo->buffType > 0)
        Target->RemoveAurasByBuffType(m_spellInfo->buffType, m_caster->GetGUID());
	// spells that proc on spell cast, some talents
	if(p_caster)
		p_caster->HandleProc(PROC_ON_CAST_SPECIFIC_SPELL,Target, m_spellInfo);
    std::map<uint32,Aura*>::iterator itr=Target->tmpAura.find(m_spellInfo->Id);
    if(itr!=Target->tmpAura.end())
    {
		if(itr->second)
		{
			if(itr->second->GetSpellProto()->procCharges)
			{
			Aura *aur=NULL;
			for(int i=0;i<itr->second->GetSpellProto()->procCharges-1;i++)
			{
			aur = new Aura(itr->second->GetSpellProto(),itr->second->GetDuration(),itr->second->GetCaster(),itr->second->GetTarget());
			Target->AddAura(aur);
			aur=NULL;
			}
			SpellCharge charge;
			charge.count=itr->second->GetSpellProto()->procCharges;
			charge.spellId=itr->second->GetSpellId();
			charge.ProcFlag=itr->second->GetSpellProto()->procFlags;
			charge.lastproc = 0;charge.FromProc=false;
			Target->m_chargeSpells.insert(make_pair(itr->second->GetSpellId(),charge));
			}
			Target->AddAura(itr->second); // the real spell is added last so the modifier is removed last
			Target->tmpAura.erase(itr);
		}
    }
	uint32 spellid = 0;

	if(m_spellInfo->MechanicsType == 25 && m_spellInfo->Id != 25771) // Cast spell Forbearance
		spellid = 25771;
	else if(m_spellInfo->MechanicsType == 16 && m_spellInfo->Id != 11196) // Cast spell Recently Bandaged
		spellid = 11196;
	else if(m_spellInfo->MechanicsType == 19 && m_spellInfo->Id != 6788) // Cast spell Weakened Soul
		spellid = 6788;

	if(spellid && p_caster)
	{
		SpellEntry *spellInfo = sSpellStore.LookupEntry( spellid );
		if(!spellInfo) return;
		Spell *spell = new Spell(p_caster, spellInfo ,true, NULL);
		SpellCastTargets targets;
		targets.m_unitTarget = Target->GetGUID();
		spell->prepare(&targets);    
	}
	}

/*
void Spell::TriggerSpell()
{
    if(TriggerSpellId != 0)
    {
        // check for spell id
        SpellEntry *spellInfo = sSpellStore.LookupEntry(TriggerSpellId );

        if(!spellInfo)
        {
            sLog.outError("WORLD: unknown spell id %i\n", TriggerSpellId);
            return;
        }

        Spell *spell = new Spell(m_caster, spellInfo,false, NULL);
        WPAssert(spell);

        SpellCastTargets targets;
        if(TriggerSpellTarget)
            targets.m_unitTarget = TriggerSpellTarget;
        else
            targets.m_unitTarget = m_targets.m_unitTarget;

        spell->prepare(&targets);
    }
}*/
/*
void Spell::DetermineSkillUp()
{
    skilllinespell* skill = objmgr.GetSpellSkill(m_spellInfo->Id);
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if ((skill) && ((Player*)m_caster)->HasSkillLine(skill->skilline))
        {
            uint32 amt = ((Player*)m_caster)->GetBaseSkillAmt(skill->skilline);
            uint32 max = ((Player*)m_caster)->GetSkillMax(skill->skilline);
            if (amt >= skill->grey) //grey
            {
            }
            else if ((amt >= (((skill->grey - skill->green) / 2) + skill->green)) && (amt < max))
            {
                if (rand()%100 < 33) //green
                    ((Player*)m_caster)->AdvanceSkillLine(skill->skilline);    

            }
            else if ((amt >= skill->green) && (amt < max))
            {
                if (rand()%100 < 66) //yellow
                    ((Player*)m_caster)->AdvanceSkillLine(skill->skilline);        

            }
            else 
            {
                if (amt < max) //brown
                    ((Player*)m_caster)->AdvanceSkillLine(skill->skilline);
            }
        }
    }
}
*/

bool Spell::IsAspect()
{
    return (
        (m_spellInfo->Id ==  2596) || (m_spellInfo->Id ==  5118) || (m_spellInfo->Id == 14320) || (m_spellInfo->Id == 13159) || (m_spellInfo->Id == 13161) || (m_spellInfo->Id == 20190) || 
        (m_spellInfo->Id == 20043) || (m_spellInfo->Id == 14322) || (m_spellInfo->Id == 14321) || (m_spellInfo->Id == 13163) || (m_spellInfo->Id == 14319) || (m_spellInfo->Id == 14318) || (m_spellInfo->Id == 13165)); 
}

bool Spell::IsSeal()
{
    return (
        (m_spellInfo->Id == 13903) || (m_spellInfo->Id == 17177) || (m_spellInfo->Id == 20154) || (m_spellInfo->Id == 20162) || (m_spellInfo->Id == 20164) || (m_spellInfo->Id == 20165) || 
        (m_spellInfo->Id == 20166) || (m_spellInfo->Id == 20287) || (m_spellInfo->Id == 20288) || (m_spellInfo->Id == 20289) || (m_spellInfo->Id == 20290) || (m_spellInfo->Id == 20291) || 
        (m_spellInfo->Id == 20292) || (m_spellInfo->Id == 20293) || (m_spellInfo->Id == 20305) || (m_spellInfo->Id == 20306) || (m_spellInfo->Id == 20307) || (m_spellInfo->Id == 20308) || 
        (m_spellInfo->Id == 20347) || (m_spellInfo->Id == 20348) || (m_spellInfo->Id == 20349) || (m_spellInfo->Id == 20356) || (m_spellInfo->Id == 20357) || (m_spellInfo->Id == 20375) || 
        (m_spellInfo->Id == 20915) || (m_spellInfo->Id == 20918) || (m_spellInfo->Id == 20919) || (m_spellInfo->Id == 20920) || (m_spellInfo->Id == 21082) || (m_spellInfo->Id == 21084)); 
}

int8 Spell::CanCast(bool rangetolerate)
{
    float maxr = 0;
    Unit *target = NULL;
    if(m_targets.m_unitTarget)
    {
        if(p_caster)
        {
            if(!rangetolerate && !p_caster->CanCastDueToCooldown(m_spellInfo))
                return SPELL_FAILED_NOT_READY;
        }
        /*Unit* target = sObjHolder.GetObject<Creature>(m_targets.m_unitTarget);
        if(!target)
            Unit* target = sObjHolder.GetObject<Player>(m_targets.m_unitTarget);*/
        switch(UINT32_LOPART(GUID_HIPART(m_targets.m_unitTarget)))
        {
        case HIGHGUID_UNIT:
            target = sObjHolder.GetObject<Creature>(m_targets.m_unitTarget);
            break;
        case HIGHGUID_PET:
            target = sObjHolder.GetObject<Pet>(m_targets.m_unitTarget);
            break;
        case HIGHGUID_PLAYER:
            target = sObjHolder.GetObject<Player>(m_targets.m_unitTarget);
            break;
        }

        if(target)
        {
            // scripted spell stuff
            switch (m_spellInfo->Id)
            {
            case 603: //curse of doom, can't be casted on players
                {
                if(target->IsPlayer())
                    return SPELL_FAILED_TARGET_IS_PLAYER;
                }break;
            
            case 25997: // Eye for an Eye
                {
                    // do not allow the spell to be cast
                    return SPELL_FAILED_SPELL_UNAVAILABLE;
                }break;
            }
            
            if(target != u_caster && !m_caster->IsPet())
            {
                // Dummy spells check
                if (m_spellInfo->Id == 4130)// Banish Burning Exile
                {
                    if (target->GetEntry()!= 2760) // target needs to be a Burning Exile
                        return SPELL_FAILED_BAD_TARGETS;
                }
                if (m_spellInfo->Id == 4131)// Banish Cresting Exile
                {
                    if (target->GetEntry()!= 2761) // target needs to be a Cresting Exile
                        return SPELL_FAILED_BAD_TARGETS;
                }
                if (m_spellInfo->Id == 4132)// Banish Thundering Exile
                {
                    if (target->GetEntry()!= 2762) // target needs to be a Thundering Exile
                        return SPELL_FAILED_BAD_TARGETS;
                }

                // original spell check code
                /*if(!(GetType() == SPELL_TYPE_MAGIC && !(m_spellInfo->Attributes & 0x10000)))
                    if(!u_caster->isInFront(target))
                        return SPELL_FAILED_NOT_INFRONT;

                // check behind
                if(m_spellInfo->Flags3 & FLAGS3_REQ_BEHIND_TARGET && m_spellInfo->Id != 20271)
                    if(target->isInFront(u_caster))
                        return SPELL_FAILED_NOT_BEHIND;*/


                /***********************************************************
                /* Capt: Inface checks, these are checked in 2 ways
                /* 1e way is check for damage type, as 3 is always ranged
                /* 2e way is trough the data in the extraspell db
                /*
                /**********************************************************/
                if (m_spellInfo->Spell_Dmg_Type == 3) // 3 is ranged so we do not need to check, we just need inface
                {   // our spell is a ranged spell
                    if (!u_caster->isInFront(target))
                        return SPELL_FAILED_NOT_INFRONT;
                }
                else
                {   // our spell is not a ranged spell

                    // get spell extra data
                    SpellExtraInfo* SpellExtra = objmgr.GetSpellExtraData(m_spellInfo->Id);

                    if(SpellExtra)
                    {
                        // Spell extra infront check
                        if (GetType() == SPELL_TYPE_MAGIC && SpellExtra->ExtraFlags & SPELL_EXTRA_INFRONT) // only do spells
                            if (!u_caster->isInFront(target))
                                return SPELL_FAILED_NOT_INFRONT;
                        // Spell extra Behind check
                        if(SpellExtra->ExtraFlags & SPELL_EXTRA_BEHIND)
                            if(target->isInFront(u_caster))
                                return SPELL_FAILED_NOT_BEHIND;
                    }
                }
            }

            // if target is already skinned, don't let it be skinned again
            if (m_spellInfo->Effect[0] == SPELL_EFFECT_SKINNING) // skinning
                if ( (((Creature*)target)->Skinned) )
                    return SPELL_FAILED_TARGET_UNSKINNABLE;

            // all spells with target 61 need to be in group or raid
            // TODO: need to research this if its not handled by the client!!!
            if (
                m_spellInfo->EffectImplicitTargetA[0] == 61 || 
                m_spellInfo->EffectImplicitTargetA[1] == 61 || 
                m_spellInfo->EffectImplicitTargetA[2] == 61)
            {
                if (!((Player*)target)->InGroup())
                    return SPELL_FAILED_NOT_READY;//return SPELL_FAILED_TARGET_NOT_IN_PARTY or SPELL_FAILED_TARGET_NOT_IN_PARTY;
            }

            // pet's owner stuff
            if (m_spellInfo->EffectImplicitTargetA[0] == 27 || 
                m_spellInfo->EffectImplicitTargetA[1] == 27 || 
                m_spellInfo->EffectImplicitTargetA[2] == 27)
            {
                if (!target->IsPlayer())
                    return SPELL_FAILED_TARGET_NOT_PLAYER; //if you are there something is very wrong
            }

            // target 39 is fishing, all fishing spells are handled
            // original code made by phantomas
            if (m_spellInfo->EffectImplicitTargetA[0] == 39 || 
                m_spellInfo->EffectImplicitTargetA[1] == 39 || 
                m_spellInfo->EffectImplicitTargetA[2] == 39)
            {
                uint32 entry = m_spellInfo->EffectMiscValue[0];
                if(entry == GO_FISHING_BOBBER)
                {
                    //uint32 mapid = p_caster->GetMapId();
                    float px=u_caster->GetPositionX();
                    float py=u_caster->GetPositionY();
                    float pz=u_caster->GetPositionZ();
                    float orient = m_caster->GetOrientation();
                    float posx = 0,posy = 0,posz = 0;
                    float co = cos(orient);
                    float si = sin(orient);
                    MapMgr * map = m_caster->GetMapMgr(); 

                    float r;
                    for( r=20;r>10;r--)
                    {
                        posx=px+r*co;
                        posy=py+r*si;
                        /*if(!(map->GetWaterType(posx,posy) & 1))//water 
                            continue;*/
                        posz = map->GetWaterHeight(posx,posy);
                        if(posz > map->GetLandHeight(posx,posy))//water
                            break;
                    }
                    if(r<=10)
                        return SPELL_FAILED_NOT_FISHABLE;

                    //Captnoord: THIS IS A TEMP HACK
                    // if we are already fishing, dont cast it again
                    if (p_caster->GetSummonedObject())
                        if (p_caster->GetSummonedObject()->GetEntry() == GO_FISHING_BOBBER)
                            return SPELL_FAILED_SPELL_IN_PROGRESS;
                }
            }
            
            maxr = GetMaxRange(sSpellRange.LookupEntry(m_spellInfo->rangeIndex));
            
            if(m_spellInfo->SpellGroupType && u_caster)
            {
                SM_FFValue(u_caster->SM_FRange,&maxr,m_spellInfo->SpellGroupType);
                SM_PFValue(u_caster->SM_PRange,&maxr,m_spellInfo->SpellGroupType);
            }

            if(rangetolerate)
                maxr*=1.33f;        
            if(!IsInrange(m_caster->GetPositionX(),m_caster->GetPositionY(),m_caster->GetPositionZ(),target, maxr*maxr))
                return SPELL_FAILED_OUT_OF_RANGE;
        
            if(p_caster)
            {
                if(m_spellInfo->Category==1131)//Hammer of wrath, requires target to have 20- % of hp
                {
                    if(target->GetUInt32Value(UNIT_FIELD_HEALTH) == 0)
                        return SPELL_FAILED_BAD_TARGETS;

                    if(target->GetUInt32Value(UNIT_FIELD_MAXHEALTH)/target->GetUInt32Value(UNIT_FIELD_HEALTH)<5)
                         return SPELL_FAILED_BAD_TARGETS;
                    
                }
                else if(m_spellInfo->Category==672)//Conflagrate, requires immolation spell on victim
                {
                    if(!target->HasAura(46))
                        return SPELL_FAILED_BAD_TARGETS;
                }

                // warning C4309 fix
                if(target->dispels[m_spellInfo->DispelType])
                    return (uint8)SPELL_FAILED_PREVENTED_BY_MECHANIC;
                
                if(target->MechanicsDispels[m_spellInfo->MechanicsType])
                    return (uint8)SPELL_FAILED_PREVENTED_BY_MECHANIC;
            }

            // if we're replacing a higher rank, deny it
            AuraCheckResponse acr = target->AuraCheck(m_spellInfo->NameHash, m_spellInfo->RankNumber);
            if(acr.Error == AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT)
                return (uint8)SPELL_FAILED_AURA_BOUNCED;

            /*SpellReplacement*rp=objmgr.GetReplacingSpells(m_spellInfo->Id);
            if(rp)
            {
                if(isAttackable(u_caster,target))//negative, replace only our own spell
                {
                    for(uint32 x=0;x<rp->count;x++)
                    {
                        if(target->HasActiveAura(rp->spells[x],m_caster->GetGUID()))
                        {
                            return SPELL_FAILED_AURA_BOUNCED;
                        }
                    }
                }
                else
                {
                    for(uint32 x=0;x<rp->count;x++)
                    {
                        if(target->HasActiveAura(rp->spells[x]))
                        {
                            return SPELL_FAILED_AURA_BOUNCED;
                        }
                    }
                }
            }    */        
        }
    }    
    if(m_targets.m_targetMask==TARGET_FLAG_DEST_LOCATION)
    {
        maxr = GetMaxRange (sSpellRange.LookupEntry(m_spellInfo->rangeIndex));
        if(!IsInrange(m_targets.m_destX,m_targets.m_destY,m_targets.m_destZ,m_caster, maxr*maxr))
            return SPELL_FAILED_OUT_OF_RANGE;
    }

    if(p_caster)
    {
        if( m_spellInfo->RequiresSpellFocus)
        {
            if(m_spellInfo->rangeIndex == 1) // somehow range that is needed for brazers for cooking is not in the range spell
                maxr = 3.0f;
            bool found = false;
            for(std::set<Object*>::iterator itr = p_caster->GetInRangeSetBegin(); itr != p_caster->GetInRangeSetEnd(); itr++ )
            {
                if((*itr)->GetTypeId() != TYPEID_GAMEOBJECT)
                    continue;
                if((*itr)->GetUInt32Value( GAMEOBJECT_TYPE_ID) != GAMEOBJECT_TYPE_SPELL_FOCUS)
                    continue;
                if(!IsInrange(p_caster->GetPositionX(),p_caster->GetPositionY(),
                    p_caster->GetPositionZ(),(*itr),maxr*maxr))
                    continue;
                GameObjectInfo *info = objmgr.GetGameObjectName((*itr)->GetEntry());
                if(!info)
                    continue;
                if(info->SpellFocus == m_spellInfo->RequiresSpellFocus)
                {
                    found = true;
                    break;
                }
            }
            if(!found)
                return SPELL_FAILED_REQUIRES_SPELL_FOCUS;
        }
    }
    
    if(u_caster)
    {
        if(u_caster->SchoolCastPrevent[m_spellInfo->School])//
        {    
            uint32 now_=getMSTime();
            if(now_>u_caster->SchoolCastPrevent[m_spellInfo->School])//this limit has expired,remove
                u_caster->SchoolCastPrevent[m_spellInfo->School]=0;
            else 
                return SPELL_FAILED_SILENCED;
        }

        if(u_caster->m_silenced)
            return SPELL_FAILED_SILENCED;

        if(target)
        {
            for(int i=0;i<3;i++) // if is going to cast a spell that breaks stun remove stun auras, looks a bit hacky but is the best way i can find
                if(m_spellInfo->EffectApplyAuraName[i] == 77 && (m_spellInfo->EffectMiscValue[i] == 12 || m_spellInfo->EffectMiscValue[i] == 17))
                {
                    for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_AURAS;x++)
                        if(target->m_auras[x])
                            if(target->m_auras[x]->GetSpellProto()->MechanicsType == m_spellInfo->EffectMiscValue[i])
                                target->m_auras[x]->Remove();
                }
        }

        if(u_caster->IsPacified())
            return SPELL_FAILED_PACIFIED;

        if(u_caster->IsStunned())
            return SPELL_FAILED_STUNNED;
                
    }

    if(i_caster)
    {
        uint32 ss = (u_caster->GetUInt32Value(UNIT_FIELD_BYTES_1)<<8)>>24;
        if(ss && (ss < FORM_BATTLESTANCE || ss > FORM_STEALTH))//druid shapeshift
        {
            return SPELL_FAILED_NO_ITEMS_WHILE_SHAPESHIFTED;    
        }    
    }

    if(u_caster)
    {
        if(u_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT) > 0)
        {
            if(!u_caster->GetCurrentSpell() || !m_triggeredSpell)
                return SPELL_FAILED_SPELL_IN_PROGRESS;
            else
            {
                if(u_caster->GetCurrentSpell())
                {
                    bool pass = false;
                    for(uint32 eff = 0;eff<3;eff++)
                    {
                        if(u_caster->GetCurrentSpell()->m_spellInfo->EffectTriggerSpell[eff] == m_spellInfo->Id)
                        {
                            pass = true;    // Allow it.
                            break;        // No point continuing.
                        }
                    }
                    if(!pass)
                        return SPELL_FAILED_SPELL_IN_PROGRESS;
                }
            }
        }
    }

    if(p_caster)
        return CheckItems();
    else 
        return -1;
}

int8 Spell::CheckItems()
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return int8(-1);

    Player* p_caster = (Player*)m_caster;

    for(uint32 i=0;i<8;i++)
    {
        if((m_spellInfo->Reagent[i] == 0) || (m_spellInfo->ReagentCount[i] == 0))
            continue;
        if (p_caster->GetItemInterface()->GetItemCount(m_spellInfo->Reagent[i]) < m_spellInfo->ReagentCount[i])
            return int8(SPELL_FAILED_ITEM_GONE);
    }

    // Check Totems
    uint32 totems = 2;
    for(uint32 i=0; i < 2; i++)
    {
        if(m_spellInfo->Totem[i] != 0)
        {
            int8 slot = p_caster->GetItemInterface()->GetInventorySlotById(m_spellInfo->Totem[i]);
            if(slot != ITEM_NO_SLOT_AVAILABLE)
            {
                if((slot >= INVENTORY_SLOT_ITEM_START && slot < INVENTORY_SLOT_ITEM_END) || slot == EQUIPMENT_SLOT_MAINHAND)
                    totems -= 1;                    
            }
        }
        else
            totems -= 1;
    }
    if(totems != 0)
        return SPELL_FAILED_ITEM_NOT_FOUND;
    
    if(m_targets.m_itemTarget)
    {
        Item *it=((Player*)m_caster)->GetItemInterface()->GetItemByGUID(m_targets.m_itemTarget);
        //FIXME: we can enchant items not only in player inventory
        if(it)
        {            
            ItemPrototype *proto=it->GetProto();
            ASSERT(proto);
           
            if(m_spellInfo->Id==13262) //check for disenchant, only greeen and better items could be disenchanted
            {
                if(proto->Quality < 2 || proto->InventoryType == INVTYPE_NON_EQUIP
                || proto->InventoryType >= INVTYPE_HOLDABLE)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;
                //disenchant armor,weapon,ring,trinket, and neck                
            }
            else
            {
                if(m_spellInfo->EquippedItemClass != -1)//all items
                    if((m_spellInfo->EquippedItemClass != proto->Class)||
                        !(m_spellInfo->EquippedItemSubClass & (1 << proto->SubClass)) ||
                        
                        //Capt: replaced the dummy code with correct code
                        //(m_spellInfo->dummy && !(m_spellInfo->dummy & (1<<proto->InventoryType))))        
                        (m_spellInfo->RequiredItemFlags && !(m_spellInfo->RequiredItemFlags & (1<<proto->InventoryType))))        
                            return int8(SPELL_FAILED_BAD_TARGETS);
            }
        }
        else 
            return int8(SPELL_FAILED_ITEM_NOT_FOUND);
    }
    return int8(-1);
}

void Spell::RemoveItems()
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = (Player*)m_caster;

    if(m_spellInfo->Flags3 == 131072 || m_spellInfo->field10 & 0x8000) //0x8000 //0x20000
    {
        p_caster->GetItemInterface()->RemoveItemAmt(p_caster->GetUInt32Value(PLAYER_AMMO_ID),1);
    }

    for(uint32 i=0;i<8;i++)
    {
        if(m_spellInfo->Reagent[i] == 0)
            continue;
        p_caster->GetItemInterface()->RemoveItemAmt(m_spellInfo->Reagent[i],m_spellInfo->ReagentCount[i]);
    }
}

int32 Spell::CalculateEffect(uint32 i)
{
    // TODO: Add ARMOR CHECKS; Add npc that have ranged weapons use them;

    // Range checks
    if (m_spellInfo->Id == SPELL_RANGED_GUN) // this includes bow and gun
    {
        if(!u_caster || !unitTarget)
            return 0;

        return ::CalculateDamage(u_caster,unitTarget,RANGED, m_spellInfo->SpellGroupType);//ranged
    }

    int32 value = 0;

//    float basePointsPerLevel = m_spellInfo->EffectRealPointsPerLevel[i];
    float randomPointsPerLevel = m_spellInfo->EffectDicePerLevel[i];
    int32 basePoints = m_spellInfo->EffectBasePoints[i] + 1;//+(m_caster->getLevel()*basePointsPerLevel);

    int32 randomPoints = m_spellInfo->EffectDieSides[i];
    if(u_caster)
        randomPoints += u_caster->getLevel() * (int32)randomPointsPerLevel;
    
    if(randomPoints <= 1)
        value = basePoints;
    else
        value = basePoints + rand()  %randomPoints;

    if(p_caster)
    {        
        int32 comboDamage = (int32)m_spellInfo->EffectPointsPerComboPoint[i];
    
        if(comboDamage)
        {
            value+=(comboDamage * p_caster->GetCP());
            m_requiresCP=true;
        }
     }
    //this may be dangerous but let it be
    if(m_spellInfo->SpellGroupType && u_caster)
    {
        
        SM_FIValue(u_caster->SM_FDummy,&value,m_spellInfo->SpellGroupType);
        SM_PIValue(u_caster->SM_PDummy,&value,m_spellInfo->SpellGroupType);
    }

    return value;
}

void Spell::HandleTeleport(uint32 id, Unit* Target)
{
    if(Target->GetTypeId()!=TYPEID_PLAYER)
        return;

    Player* pTarget = (Player*)Target;
   
    float x,y,z;
    uint32 mapid;
    WorldPacket data;
    
    if (m_spellInfo->Id == 8690 ||m_spellInfo->Id == 556)//556- Astral Recall
    {
        x = pTarget->GetBindPositionX();
        y = pTarget->GetBindPositionY();
        z = pTarget->GetBindPositionZ();
        mapid = pTarget->GetBindMapId();
    }
    else
    {
        /*std::stringstream query;
        query << "SELECT * FROM recall where guid = " << m_spellInfo->Id;
        QueryResult *result = sDatabase.Query( query.str( ).c_str( ) );
        
        if( !result )
            return;

        Field *fields = result->Fetch();
        mapid = fields[3].GetUInt32();
        x = fields[4].GetUInt32();
        y = fields[5].GetUInt32();
        z = fields[6].GetUInt32();*/
        TeleportCoords* TC = objmgr.GetTeleportCoords(id);
        if(!TC)
            return;
         
        x=TC->x;
        y=TC->y;
        z=TC->z;
        mapid=TC->mapId;
    }   

    pTarget->EventAttackStop();
    pTarget->SetSelection(0);   
      
    //pTarget->Relocate(mapid, x, y, z, pTarget->GetOrientation(), true, false);

    // We have to use a teleport event on this one. Reason being because of UpdateCellActivity,
    // the gameobject set of the updater thread WILL Get messed up if we teleport from a gameobject
    // caster.

    if(!sEventMgr.HasEvent(pTarget, EVENT_PLAYER_TELEPORT))
        sEventMgr.AddEvent(pTarget, &Player::EventTeleport, mapid, x, y, z, EVENT_PLAYER_TELEPORT, 1, 1);
}

void Spell::CreateItem(uint32 itemId)
{
    Player* pUnit = (Player*)m_caster;
    Item* newItem;
    Item *add;
    int8 slot = 0;
    SlotResult slotresult;

    if(!itemId)
        return;

     ItemPrototype *m_itemProto;
     m_itemProto = objmgr.GetItemPrototype(itemId);
     if (!m_itemProto)
         return;

    add = pUnit->GetItemInterface()->FindItemLessMax(itemId, 1, false);
    if (!add)
    {
        slotresult = pUnit->GetItemInterface()->FindFreeInventorySlot(m_itemProto);
        if(!slotresult.Result)
        {
             printf("inventory is full to create a item.\n");
             SendCastResult(SPELL_FAILED_TOO_MANY_OF_ITEM);
             return;
        }
        
        newItem = objmgr.CreateItem(itemId, pUnit);
        bool result = pUnit->GetItemInterface()->SafeAddItem(newItem, slotresult.ContainerSlot, slotresult.Slot);
        if(!result)
        {
            printf("Spell: CreateItem failed while trying to add item to free slot\n");
            delete newItem;
            return;
        }

        newItem->SetUInt64Value(ITEM_FIELD_CREATOR,m_caster->GetGUID());
        newItem->SetUInt32Value(ITEM_FIELD_STACK_COUNT, damage);

        WorldPacket data;
        p_caster->GetSession()->BuildItemPushResult(&data, p_caster->GetGUID(), 1, 1, itemId ,0,0xFF,1,0xFFFFFFFF);
        p_caster->SendMessageToSet(&data, true);

    } 
    else 
    {
        add->SetUInt32Value(ITEM_FIELD_STACK_COUNT,add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + damage);
        WorldPacket data;
        p_caster->GetSession()->BuildItemPushResult(&data, p_caster->GetGUID(), 1, 1, itemId ,0,0xFF,1,0xFFFFFFFF);
        p_caster->SendMessageToSet(&data, true);
    }
}

void Spell::_DamageRangeUpdate()
{/*
    if(unitTarget)
    {
        if(unitTarget->isAlive())
        {
            m_caster->SpellNonMeleeDamageLog(unitTarget,m_spellInfo->Id, damageToHit);
        }
        else
        {    if(u_caster)
            if(u_caster->GetCurrentSpell() != this)
            {
                    if(u_caster->GetCurrentSpell() != NULL)
                    {
                        u_caster->GetCurrentSpell()->SendChannelUpdate(0);
                        u_caster->GetCurrentSpell()->cancel();
                    }
            }
            SendChannelUpdate(0);
            cancel();
        }
        sEventMgr.RemoveEvents(this, EVENT_SPELL_DAMAGE_HIT);
        delete this;
    }
    else if(gameObjTarget)
    {
        sEventMgr.RemoveEvents(this, EVENT_SPELL_DAMAGE_HIT);
        delete this;
        //Go Support
    }
    else
    {
        sEventMgr.RemoveEvents(this, EVENT_SPELL_DAMAGE_HIT);
        delete this;
    }*/
}

void Spell::SendHealSpellOnPlayer(Player* caster, Player* target, uint32 dmg,bool critical)
{
    if(!caster || !target)
        return;
    WorldPacket data(SMSG_HEALSPELL_ON_PLAYER_OBSOLETE, 27);
    // Bur: I know it says obsolete, but I just logged this tonight and got this packet.
    
    data << target->GetNewGUID();
    data << caster->GetNewGUID();
    data << uint32(m_spellInfo->Id);
    data << uint32(dmg);    // amt healed
    data << uint8(critical);     //this is crical message

    caster->SendMessageToSet(&data, true);
}

void Spell::Heal(int32 amount)
{
    if(!unitTarget || !unitTarget->isAlive())
        return;
    
    if(p_caster && playerTarget)
    {
        // Healing a flagged target will flag you.
        if(playerTarget->IsPvPFlagged())
            p_caster->SetPvPFlag();
    }

    //Make it critical
    bool critical=false;
    if(u_caster)
    {
        SpellCastTime *sd = sCastTime.LookupEntry(m_spellInfo->CastingTimeIndex);

        // affect the plus damage by duration
        float castaff = GetCastTime(sd);
        if(castaff > 3500) castaff = 3500;
        else if(castaff < 1500) castaff = 1500;
        float healdoneaffectperc = castaff / 3500;
        
        amount += u_caster->HealDoneMod[m_spellInfo->School] * healdoneaffectperc;
        amount += (amount*u_caster->HealDonePctMod[m_spellInfo->School])/100;
        amount += unitTarget->HealTakenMod[m_spellInfo->School];//amt of health that u RECIVE, not heal
        
        float spellCrit = u_caster->spellcritperc + u_caster->SpellCritChanceSchool[m_spellInfo->School];
        if(critical = Rand(spellCrit))
            amount *= 1.5;
    }
    if(amount < 0) amount = 0;

    if(p_caster)  
    {
        amount += p_caster->SpellDmgDoneByInt[m_spellInfo->School] * p_caster->GetUInt32Value(UNIT_FIELD_STAT3);

        if(unitTarget->IsPlayer())
        {
            SendHealSpellOnPlayer(p_caster, ((Player*)unitTarget), amount, critical);
        }
        if(p_caster->m_bgScore != 0 && p_caster != unitTarget)
            p_caster->m_bgScore->HealingDone += amount;
    }
    uint32 curHealth = unitTarget->GetUInt32Value(UNIT_FIELD_HEALTH);
    uint32 maxHealth = unitTarget->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
    if((curHealth + amount) >= maxHealth)
        unitTarget->SetUInt32Value(UNIT_FIELD_HEALTH, maxHealth);
    else
        unitTarget->ModUInt32Value(UNIT_FIELD_HEALTH, amount);
    
    int doneTarget = 0;

    // add threat
    if(u_caster)
    {
        int count = 0;
        Unit *unit;
        std::vector<Unit*> target_threat;
        target_threat.reserve(u_caster->GetInRangeCount()); // this helps speed

        for(std::set<Object*>::iterator itr = u_caster->GetInRangeSetBegin(); itr != u_caster->GetInRangeSetEnd(); ++itr)
        {
            if((*itr)->GetTypeId() != TYPEID_UNIT)
                continue;
            unit = static_cast<Unit*>((*itr));
            if(unitTarget->IsBeingAttackedBy(unit) ||
                unitTarget->getAttackTarget() == unit->GetGUID() ||
                u_caster->IsBeingAttackedBy(unit) ||
                u_caster->getAttackTarget() == unit->GetGUID())
            {
                target_threat.push_back(unit);
                ++count;
            }
        }
        if(count == 0)
            count = 1;  // division against 0 protection
        /* 
        When a tank hold multiple mobs, the threat of a heal on the tank will be split between all the mobs.
        The exact formula is not yet known, but it is more than the Threat/number of mobs.
        So if a tank holds 5 mobs and receives a heal, the threat on each mob will be less than Threat(heal)/5.
        Current speculation is Threat(heal)/(num of mobs *2)
        */
        uint32 threat = amount / (count * 2);
            
        for(std::vector<Unit*>::iterator itr = target_threat.begin(); itr != target_threat.end(); ++itr)
        {
            // for now we'll just use heal amount as threat.. we'll prolly need a formula though
            ((Unit*)(*itr))->GetAIInterface()->HealReaction(u_caster, unitTarget, threat);

            if((*itr)->GetGUID() == u_caster->getAttackTarget())
                doneTarget = 1;
        }

        if(!doneTarget && u_caster->getAttackTarget())
        {
            // this shouldn't happen..
            //printf("wtf in heal()");
            //u_caster->m_AttackTarget->GetAIInterface()->HealReaction(u_caster, unitTarget, threat);
        }
    }   
}

void Spell::DetermineSkillUp(uint32 skillid,uint32 targetlevel)
{
    if(!p_caster)return;
    if(p_caster->GetSkillUpChance(skillid)<0.01)return;//to preven getting higher skill than max
    int32 diff=p_caster->GetBaseSkillAmt(skillid)/5-targetlevel;
    if(diff<0)diff=-diff;
    float chance;
    if(diff<=5)chance=95.0;
    else if(diff<=10)chance=66;
    else if(diff <=15)chance=33;
    else return;
    if(Rand(chance))
        p_caster->AdvanceSkillLine(skillid);
}

void Spell::DetermineSkillUp(uint32 skillid)
{
    //This code is wrong for creating items and disenchanting. 
    if(!p_caster)return;
    float chance = 0;
    skilllinespell* skill = objmgr.GetSpellSkill(m_spellInfo->Id);
    if ((skill) && ((Player*)m_caster)->HasSkillLine(skill->skilline))
    {
        uint32 amt = ((Player*)m_caster)->GetBaseSkillAmt(skill->skilline);
        uint32 max = ((Player*)m_caster)->GetSkillMax(skill->skilline);
        if(amt >= max)
            return;
        if (amt >= skill->grey) //grey
            chance = 0;
        else if ((amt >= (((skill->grey - skill->green) / 2) + skill->green))) //green
            chance = 33;
        else if (amt >= skill->green) //yellow
            chance = 66;
        else //brown
            chance=100;
    }
    if(Rand(chance))
        p_caster->AdvanceSkillLine(skillid);
}

void Spell::SafeAddTarget(TargetsList* tgt,uint64 guid)
{
    for(TargetsList::iterator i=tgt->begin();i!=tgt->end();i++)
        if((*i)==guid)
            return;
    tgt->push_back(guid);
}

void Spell::SafeAddMissedTarget(uint64 guid)
{
    for(TargetsList::iterator i=MissedTargets.begin();i!=MissedTargets.end();i++)
        if((*i)==guid)
            return;
    MissedTargets.push_back(guid);
}
bool Spell::reflect(Unit *refunit)
{
    SpellEntry *refspell = NULL;

    // if the spell to reflect is a reflect spell, do nothing.
    for(int i=0; i<3; i++)
        if(m_spellInfo->Effect[i] == 6 && (m_spellInfo->EffectApplyAuraName[i] == 74 || m_spellInfo->EffectApplyAuraName[i] == 28))
            return false;
    for(std::list<struct ReflectSpellSchool*>::iterator i = refunit->m_reflectSpellSchool.begin();i != refunit->m_reflectSpellSchool.end();i++)
    {
        if((*i)->school == -1 || (*i)->school == m_spellInfo->School)
        {
            if(Rand((float)(*i)->chance))
                refspell = m_spellInfo;
        }
    }

    if(!refspell || m_caster == refunit) return false;

    Spell *spell = new Spell(refunit, refspell, true, NULL);
    SpellCastTargets targets;
    targets.m_unitTarget = m_caster->GetGUID();
    spell->prepare(&targets);
    return true;
}

void ApplyDiminishingReturnTimer(uint32 * Duration, Unit * Target, SpellEntry * spell)
{
    uint32 status = GetDiminishingGroup(spell->NameHash);
    uint32 Grp = status & 0xFFFF;   // other bytes are if apply to pvp
    uint32 PvE = (status >> 16) & 0xFFFF;

    // Make sure we have a group
    if(Grp == 0xFFFF) return;

    // Check if we don't apply to pve
    if(!PvE && Target->GetTypeId() != TYPEID_PLAYER && !Target->IsPet())
        return;

    // TODO: check for spells that should do this
    float Dur = *Duration;

    switch(Target->m_diminishCount[Grp])
    {
    case 0: // Full effect
        break;
        
    case 1: // Reduced by 50%
        Dur *= 0.5f;
        break;

        // TODO Guess what reducing something with 75% is not * 0.15 its * 0.25
    case 2: // Reduced by 75%
        //Dur *= 0.15f;
        Dur *= 0.25f;
        break;

    default:// Target immune to spell
        {
            *Duration = 0;
            return;
        }break;
    }

    // Convert back
    *Duration = FL2UINT(Dur);

    // Reset the diminishing return counter, and add to the aura count (we don't decrease the timer till we
    // have no auras of this type left.
    ++Target->m_diminishAuraCount[Grp];
    ++Target->m_diminishCount[Grp];
}

void UnapplyDiminishingReturnTimer(Unit * Target, SpellEntry * spell)
{
    uint32 status = GetDiminishingGroup(spell->NameHash);
    uint32 Grp = status & 0xFFFF;   // other bytes are if apply to pvp
    uint32 PvE = (status >> 16) & 0xFFFF;

    // Make sure we have a group
    if(Grp == 0xFFFF) return;

    // Check if we don't apply to pve
    if(!PvE && Target->GetTypeId() != TYPEID_PLAYER && !Target->IsPet())
        return;

    Target->m_diminishAuraCount[Grp]--;

    // start timer decrease
    if(!Target->m_diminishAuraCount[Grp])
    {
        Target->m_diminishActive = true;
        Target->m_diminishTimer[Grp] = 15000;
    }
}

uint32 GetDiminishingGroup(uint32 NameHash)
{
    int32 grp = -1;
    bool pve = false;

    switch(NameHash)
    {
    case 0x4219BB33:        // Sap
    case 0xCCC8A100:        // Gouge
        grp = 0;
        break;

    case 0xC76EACCD:        // Cheap Shot
        {
            grp = 1;
            pve = true;
        }break;

    case 0x02663BE6:        // Kidney Shot
        {
            grp = 2;
            pve = true;
        }break;

    case 0x3F73F2FE:        // Bash
        grp = 3;
        break;

    case 0x34C50810:        // Entangling Roots
        grp = 4;
        break;

    case 0xD70F8418:        // Hammer of Justice
        {
            grp = 5;
            pve = true;
        }break;

    case 0xCC6D4182:        // Seal of Justice
        grp = 6;
        break;

    case 0xE30500A1:        // Charge
    case 0x6B84435E:        // Intercept
    case 0xEC828A40:        // Concussion Blow
        {
            grp = 7;
            pve = true;
        }break;

    case 0x328E44DC:        // Fear
    case 0xC264D626:        // Seduction
    case 0x482156A0:        // Howl of Terror
        grp = 8;
        break;

    case 0xB358A86B:        // Frost Nova
        grp = 9;
        break;

    case 0x2266F1F2:        // Polymorph
        {
            grp = 10;
            pve = true;   
        }break;

    case 0x678E2C2F:        // Psychic Scream
        grp = 11;
        break;

    case 0x0D885237:        // Mind Control
        grp = 12;
        break;

    case 0x24D5C95F:        // Frost Shock
        grp = 13;
        break;

    }
    uint32 ret;
    if(pve)
        ret = grp | (1 << 16);
    else
        ret = grp;

    return ret;
}
