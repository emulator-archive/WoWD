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

#ifndef WOWSERVER_DYNAMICOBJECT_H
#define WOWSERVER_DYNAMICOBJECT_H

struct SpellEntry;

typedef set<Unit*>  DynamicObjectList;
typedef set<Unit*>  FactionRangeList;

class WOWD_SERVER_DECL DynamicObject : public Object
{
public:
    DynamicObject( uint32 high, uint32 low );
    ~DynamicObject( );

    void Create(Unit * caster, Spell * pSpell, float x, float y, float z, uint32 duration, float radius);
    void UpdateTargets();

    void AddInRangeObject(Object* pObj);
    void RemoveInRangeObject(Object* pObj);
    void Remove();

protected:
    
    SpellEntry * m_spellProto;
    Unit * u_caster;
    Player * p_caster;
    Spell* m_parentSpell;
    DynamicObjectList targets;
    FactionRangeList  m_inRangeOppFactions;
    
    uint32 m_aliveDuration;
};

#endif

