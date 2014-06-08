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
initialiseSingleton(CreatureDataCache);

void ObjectTemplate::Load(Field *field)
{
    _guid = field[0].GetUInt32();
    _x = field[1].GetFloat();
    _y = field[2].GetFloat();
    _z = field[3].GetFloat();
    _o = field[4].GetFloat();
    _zone = field[5].GetUInt32();
    _map = field[6].GetUInt32();
    _fields = new char[strlen(field[7].GetString()) + 1];
    strcpy(_fields, field[7].GetString());
}

void ObjectTemplate::Create(Object *obj)
{
    _guid = obj->GetGUIDLow();
    _x = obj->GetPositionX();
    _y = obj->GetPositionY();
    _z = obj->GetPositionZ();
    _o = obj->GetOrientation();
    _zone = obj->GetZoneId();
    _map = obj->GetMapId();

    std::string str;
    for(uint16 i = 0; i < obj->GetValuesCount(); i++ )
    {
        str += obj->GetUInt32Value(i);
        str += " ";
    }

    _fields = new char[str.length() + 1];
    strcpy(_fields, str.c_str());
}

void CorpseTemplate::Load(Field *field)
{
    ObjectTemplate::Load(field);
    _loadedfromdb = true;
}

void CorpseTemplate::Create(Object *obj)
{
    ObjectTemplate::Create(obj);
    _loadedfromdb = false;
}

void GameObjectTemplate::Load(Field *field)
{
    ObjectTemplate::Load(field);
    _entry = field[8].GetUInt32();
    //sQuestMgr.LoadGOQuests(this);
    _quests = sQuestMgr.GetGOQuestList(_entry);
}

void CreatureTemplate::Load(Field *field)
{
    ObjectTemplate::Load(field);
    _entry = field[8].GetUInt32();
    _moverandom = field[9].GetUInt32();
    _running = field[10].GetBool();

    _quests = sQuestMgr.GetCreatureQuestList(_entry);
    _items = objmgr.GetVendorList(_entry);

    //_LoadMovement();
    //_LoadHealth();
    //_LoadFormation();
    //_LoadAIAgents();

    CreatureDataCache::getSingleton().ApplyFormation(this);
    CreatureDataCache::getSingleton().ApplyHealth(this);
    CreatureDataCache::getSingleton().ApplyMovement(this);
    CreatureDataCache::getSingleton().ApplySpells(this);
}

CreatureDataCache::CreatureDataCache()
{
    // Load Movement
    printf("   Caching Creature Movement Data....                ");
    QueryResult * result = sDatabase.Query("SELECT * FROM creature_waypoints");
    if(result)
    {
        Field *fields = result->Fetch();
        uint32 entry;
        WayPoint* wp;
        do 
        {
            wp = new WayPoint;
            entry = fields[0].GetUInt32();
            wp->id = fields[1].GetUInt32();
            wp->x = fields[2].GetFloat();
            wp->y = fields[3].GetFloat();
            wp->z = fields[4].GetFloat();
            wp->waittime = fields[5].GetUInt32();
            wp->flags = fields[6].GetUInt32();
            wp->forwardemoteoneshot = fields[7].GetBool();
            wp->forwardemoteid = fields[8].GetUInt32();
            wp->forwardemoteoneshot = fields[9].GetBool();
            wp->backwardemoteid = fields[10].GetUInt32();
            wp->forwardskinid = fields[11].GetUInt32();
            wp->backwardskinid = fields[12].GetUInt32();

            if( waypointData[entry].find( wp->id ) == waypointData[entry].end() )
                waypointData[entry].insert( make_pair( wp->id, wp ) );
            else
                delete wp;

        } while(result->NextRow());
        delete result;
    }

    printf("\r   Caching Creature Spell Data....                ");
    result = sDatabase.Query( "SELECT * FROM ai_agents" );

    if(result)
    {
        AI_Spell *sp;
        Field *fields = result->Fetch();

        do
        {
            sp = new AI_Spell;
            sp->entryId = fields[0].GetUInt32();
            sp->agent = fields[1].GetUInt16();
            sp->procEvent = fields[2].GetUInt32();
            sp->procChance = fields[3].GetUInt32();
            sp->procCount = fields[4].GetUInt32();
            sp->spellId = fields[5].GetUInt32();
            sp->spellType = fields[6].GetUInt32();;
            sp->spelltargetType = fields[7].GetUInt32();
            sp->spellCooldown = fields[8].GetUInt32();
            sp->floatMisc1 = fields[9].GetFloat();
            sp->Misc2 = fields[10].GetUInt32();
            sp->spellCooldownTimer = 0;
            sp->procCounter = 0;
            sp->minrange = GetMinRange(sSpellRange.LookupEntry(sSpellStore.LookupEntry(sp->spellId)->rangeIndex));
            sp->maxrange = GetMaxRange(sSpellRange.LookupEntry(sSpellStore.LookupEntry(sp->spellId)->rangeIndex));

            spellData[sp->entryId].push_back(sp);

        } while( result->NextRow() );

        delete result;
    }

    printf("\r   Caching Creature Health Data....                ");
    result = sDatabase.Query("SELECT * FROM tempdamhe");
    if(result)
    {
        Field * fields = result->Fetch();
        temphealth health;
        do 
        {
            health.mindmg = fields[1].GetFloat();
            health.maxdmg = fields[2].GetFloat();
            health.maxhealth = fields[3].GetFloat();
            health.maxmana = fields[4].GetFloat();
            health.attack_times[0] = fields[5].GetFloat();
            health.attack_times[1] = fields[6].GetFloat();

            healthData[fields[0].GetUInt32()] = health;
        } while(result->NextRow());
        delete result;
    }

    printf("\r   Caching Creature Formation Data....                ");
    result = sDatabase.Query("SELECT * FROM creature_formations");
    if(result)
    {
        tempformation form;
        Field * fields = result->Fetch();
        do 
        {
            form.follow_id = fields[1].GetUInt32();
            form.follow_angle = fields[2].GetFloat();
            form.follow_dist = fields[3].GetFloat();

            formationData[fields[0].GetUInt32()] = form;
        } while(result->NextRow());
        delete result;
    }

    printf("\r                                                      \r");
}

void CreatureDataCache::ApplyFormation(CreatureTemplate * t)
{
    map<uint32, tempformation>::iterator itr = formationData.find(t->_guid);
    if(itr != formationData.end())
    {
        t->formation_angle = itr->second.follow_angle;
        t->formation_dist = itr->second.follow_dist;
        t->formation_sqlid = itr->second.follow_id;
    }
    else
    {
        t->formation_angle = t->formation_dist = 0;
        t->formation_sqlid = 0;
    }
}

void CreatureDataCache::ApplyHealth(CreatureTemplate * t)
{
    map<uint32, temphealth>::iterator itr = healthData.find(t->_entry);
    if(itr != healthData.end())
    {
        t->healthdata = itr->second;
    }
    else
    {
        t->healthdata.attack_times[0] = 0;
        t->healthdata.attack_times[1] = 0;
        t->healthdata.maxdmg = 0;
        t->healthdata.maxhealth = 0;
        t->healthdata.maxmana = 0;
        t->healthdata.mindmg = 0;
    }
}

void CreatureDataCache::ApplySpells(CreatureTemplate * t)
{
    map<uint32, vector<AI_Spell*> >::iterator itr = spellData.find(t->_entry);
    if(itr != spellData.end())
    {
        t->_aiSpells = itr->second;
    }
}

void CreatureDataCache::ApplyMovement(CreatureTemplate * t)
{
    map<uint32, HM_NAMESPACE::hash_map<uint32, WayPoint*> >::iterator itr = waypointData.find(t->_guid);
    if(itr != waypointData.end())
    {
        t->_movement = itr->second;
    }
}

CreatureDataCache::~CreatureDataCache()
{
    {
        map<uint32, HM_NAMESPACE::hash_map<uint32, WayPoint*> >::iterator itr = waypointData.begin();
        for(; itr != waypointData.end(); ++itr)
        {
            HM_NAMESPACE::hash_map<uint32, WayPoint*>::iterator it2 = itr->second.begin();
            for(; it2 != itr->second.end(); ++it2)
                if(it2->second)
                    delete it2->second;
            itr->second.clear();
        }
    }

    {
        map<uint32, vector<AI_Spell*> >::iterator itr = spellData.begin();
        for(; itr != spellData.end(); ++itr)
        {
            vector<AI_Spell*>::iterator it2 = itr->second.begin();
            for(; it2 != itr->second.end(); ++it2)
				if((*it2)) // hope that helps :P
                    delete (*it2);
            itr->second.clear();
        }
    }
}
