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

DynamicObject::DynamicObject(uint32 high, uint32 low) : Object(high, low)
{
    m_objectType |= TYPE_DYNAMICOBJECT;
    m_objectTypeId = TYPEID_DYNAMICOBJECT;
    m_valuesCount = DYNAMICOBJECT_END;

    m_parentSpell=NULL;
    m_aliveDuration = 0;
    u_caster = 0;
    m_spellProto = 0;
    p_caster = 0;
    
    sEventMgr.AddEvent(this, &DynamicObject::UpdateTargets, EVENT_DYNAMICOBJECT_UPDATE, 100, 0);

}

DynamicObject::~DynamicObject()
{
    // remove aura from all targets
    DynamicObjectList::iterator jtr  = targets.begin();
    DynamicObjectList::iterator jend = targets.end();
    Unit * target;

    while(jtr != jend)
    {
        target = *jtr;
        ++jtr;
        target->RemoveAura(m_spellProto->Id);
    }

    if(u_caster->dynObj == this)
        u_caster->dynObj = 0;
}

void DynamicObject::Create(Unit * caster, Spell * pSpell, float x, float y, float z, uint32 duration, float radius)
{
    Object::_Create(m_GuidLow, m_GuidHigh, caster->GetMapId(),x, y, z, 0);
    SetInstanceID(caster->GetInstanceID());
    if(pSpell->g_caster)
    {
        m_parentSpell = pSpell;
    }
    p_caster = pSpell->p_caster;

    m_spellProto = pSpell->m_spellInfo;
    SetUInt64Value(DYNAMICOBJECT_CASTER, caster->GetGUID());

    m_uint32Values[OBJECT_FIELD_ENTRY] = m_spellProto->Id;
    m_uint32Values[DYNAMICOBJECT_BYTES] = 0x01eeeeee;
    m_uint32Values[DYNAMICOBJECT_SPELLID] = m_spellProto->Id;

    m_floatValues[OBJECT_FIELD_SCALE_X] = 1.0f;
    m_floatValues[DYNAMICOBJECT_RADIUS] = radius;
    m_floatValues[DYNAMICOBJECT_POS_X]  = x;
    m_floatValues[DYNAMICOBJECT_POS_Y]  = y;
    m_floatValues[DYNAMICOBJECT_POS_Z]  = z;

    m_aliveDuration = duration;
    u_caster = caster;
    m_faction = caster->m_faction;
    m_factionDBC = caster->m_factionDBC;
    if(caster->dynObj != 0)
    {
        // expire next update
        caster->dynObj->m_aliveDuration = 1;
        caster->dynObj->UpdateTargets();
    }

    caster->dynObj = this;
    AddToWorld();
}

void DynamicObject::AddInRangeObject(Object* pObj)
{
    if(pObj->IsUnit())
    {
        bool attackable;
        if(p_caster)
            attackable = isAttackable(p_caster, pObj);
        else
            attackable = isAttackable(this, pObj);
        
        if(attackable)
            m_inRangeOppFactions.insert(((Unit*)pObj));
    }
    Object::AddInRangeObject(pObj);
}

void DynamicObject::RemoveInRangeObject(Object* pObj)
{
    if(pObj->IsUnit())
    {
        bool attackable;
        if(p_caster)
            attackable = isAttackable(p_caster, pObj);
        else
            attackable = isAttackable(this, pObj);

        if(attackable)
            m_inRangeOppFactions.erase(((Unit*)pObj));

        targets.erase(((Unit*)pObj));
    }
    Object::RemoveInRangeObject(pObj);
}

void DynamicObject::UpdateTargets()
{
    if(m_aliveDuration == 0)
        return;

    if(m_aliveDuration >= 100)
    {
        FactionRangeList::iterator itr  = m_inRangeOppFactions.begin();
        FactionRangeList::iterator iend = m_inRangeOppFactions.end();
        Unit * target;
        Aura * pAura;
        float radius = powf(m_floatValues[DYNAMICOBJECT_RADIUS], 2);

        while(itr != iend)
        {
            target = *itr;
            ++itr;

            // skip units already hit, their range will be tested later
            if(targets.find(target) != targets.end())
                continue;

            if(GetDistanceSq(target) <= radius)
            {
                pAura = new Aura(m_spellProto, m_aliveDuration, u_caster, target);
                for(uint32 i = 0; i < 3; ++i)
                {
                    if(m_spellProto->Effect[i] == 27)
                    {
                        pAura->AddMod(m_spellProto->EffectApplyAuraName[i],
                            m_spellProto->EffectBasePoints[i]+1, m_spellProto->EffectMiscValue[i], i);
                    }
                }
                target->AddAura(pAura);

                // add to target list
                targets.insert(target);
            }
        }

        // loop the targets, check the range of all of them
        DynamicObjectList::iterator jtr  = targets.begin();
        DynamicObjectList::iterator jtr2;
        DynamicObjectList::iterator jend = targets.end();
        
        while(jtr != jend)
        {
            target = *jtr;
            jtr2 = jtr;
            ++jtr;

            if(GetDistanceSq(target) > radius)
            {
                targets.erase(jtr2);
                target->RemoveAura(m_spellProto->Id);
            }
        }

        m_aliveDuration -= 100;
    }
    else
    {
        m_aliveDuration = 0;
    }

    if(m_aliveDuration == 0)
    {
        DynamicObjectList::iterator jtr  = targets.begin();
        DynamicObjectList::iterator jend = targets.end();
        Unit * target;

        while(jtr != jend)
        {
            target = *jtr;
            ++jtr;
            target->RemoveAura(m_spellProto->Id);
        }

        Remove();
    }
}

void DynamicObject::Remove()
{
    if(IsInWorld())
        RemoveFromWorld();
    sObjHolder.Delete(this);
}

