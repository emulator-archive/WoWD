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

Corpse::Corpse(uint32 high, uint32 low) : Object(high, low)
{
    m_objectType |= TYPE_CORPSE;
    m_objectTypeId = TYPEID_CORPSE;

    m_valuesCount = CORPSE_END;
    m_state = CORPSE_STATE_BODY;
    _loadedfromdb = false;
}

Corpse::~Corpse()
{
}

void Corpse::Create()
{
    Object::_Create(m_GuidLow, m_GuidHigh);
}

void Corpse::Create( Player *owner, uint32 mapid, float x, float y, float z, float ang )
{
    Object::_Create(m_GuidLow, m_GuidHigh, mapid, x, y, z, ang);

    SetFloatValue( OBJECT_FIELD_SCALE_X, 1 );
    SetFloatValue( CORPSE_FIELD_POS_X, x );
    SetFloatValue( CORPSE_FIELD_POS_Y, y );
    SetFloatValue( CORPSE_FIELD_POS_Z, z );
    SetFloatValue( CORPSE_FIELD_FACING, ang );
    SetUInt64Value( CORPSE_FIELD_OWNER, owner->GetGUID() );
    _loadedfromdb = false;  // can't be created from db ;)
}

void Corpse::SaveToDB()
{
    //save corpse to DB
    std::stringstream ss;
    ss << "DELETE FROM corpses WHERE guid = " << GetGUIDLow();
    sDatabase.Execute( ss.str( ).c_str( ) );

    ss.rdbuf()->str("");
    ss << "INSERT INTO corpses (guid, positionX, positionY, positionZ, orientation, zoneId, mapId, data, instanceId) VALUES ("
        << GetGUIDLow() << ", '" << GetPositionX() << "', '" << GetPositionY() << "', '" << GetPositionZ() << "', '" << GetOrientation() << "', '" << GetZoneId() << "', '" << GetMapId() << "', '";

    for(uint16 i = 0; i < m_valuesCount; i++ )
        ss << GetUInt32Value(i) << " ";

    ss << "', " << GetInstanceID() << " )";

    sDatabase.Execute( ss.str().c_str() );
}

void Corpse::DeleteFromDB()
{
    //delete corpse from db when its not needed anymore
    char sql[256];

    sprintf(sql, "DELETE FROM corpses WHERE guid=%u", GetGUIDLow());
    sDatabase.Execute(sql);
}

void CorpseData::DeleteFromDB()
{
    char sql[256];

    sprintf(sql, "DELETE FROM corpses WHERE guid=%u", LowGuid);
    sDatabase.Execute(sql);
}

void Corpse::Despawn()
{
    if(this->IsInWorld())
    {
        RemoveFromWorld();
    }
}

void Corpse::generateLoot()
{
    loot.gold = rand() % 150 + 50; // between 50c and 1.5s, need to fix this!
}
