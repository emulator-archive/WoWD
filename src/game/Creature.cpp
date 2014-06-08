
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

#define M_PI       3.14159265358979323846

Creature::Creature(uint32 high, uint32 low) : Unit(high, low)
{
    m_quests = NULL;

    m_corpseDelay = 130000;
    m_respawnDelay = 240000;

    m_respawnTimer = 0;
    m_deathTimer = 0;

    m_valuesCount = UNIT_END;
    m_objectTypeId = TYPEID_UNIT;

    m_regenTimer=0;
    m_useAI = true;
    respawn_cord[0] = 0.0f;
    respawn_cord[1] = 0.0f;
    respawn_cord[2] = 0.0f;
    respawn_cord[3] = 0.0f;
    mTaxiNode = 0;
    m_name = "";
    m_nameEntry = 0;
    m_sqlid = 0;

    Tagged = false;
    TaggerGuid = 0;

    Skinned = false;
    
    m_enslaveCount = 0;
    m_enslaveSpell = 0;
    
    for(uint32 x=0;x<7;x++)
    {
        FlatResistanceMod[x]=0;
        BaseResistanceModPct[x]=0;
        ResistanceModPct[x]=0;
        ModDamageDone[x]=0;
        ModDamageDonePct[x]=1.0;
    }

    for(uint32 x=0;x<5;x++)
    {
        TotalStatModPct[x]=0;
        StatModPct[x]=0;
        FlatStatMod[x]=0;
    }
    m_runSpeed=PLAYER_NORMAL_RUN_SPEED;

    totemOwner = NULL;
    totemSlot = -1;

    m_PickPocketed = false;
    m_BeingRemoved = true;
    m_SellItems = NULL;
    _myScriptClass = NULL;
    m_TaxiNode = 0;
    _gossipScript = NULL;
    _Activated = false;
    sWorld.mLoadedCreatures[0]++;
    myFamily = 0;

    loot.gold = 0;
    haslinkupevent = false;
}


Creature::~Creature()
{
    sWorld.mLoadedCreatures[0]--;
    sEventMgr.RemoveEvents(this);

    if(IsTotem())
        totemOwner->m_TotemSlots[totemSlot] = 0;

    // already destroyed
    if(!ScriptMgr::getSingletonPtr()) return;

    if(_gossipScript != 0)
        _gossipScript->Destroy();
    
    if(_myScriptClass != 0)
        _myScriptClass->Destroy();
}

void Creature::Update( uint32 p_time )
{
    Unit::Update( p_time );
    if(IsTotem() && isDead())
    {
        RemoveFromWorld();
        SafeDelete();
    }
}

void Creature::SafeDelete()
{
    sEventMgr.RemoveEvents(this);
    sEventMgr.AddEvent(this, &Creature::DeleteMe, EVENT_CREATURE_SAFE_DELETE, 1, 1);
}

void Creature::DeleteMe()
{ 
    if(IsInWorld())
        RemoveFromWorld();

    //Made so that an event can de made to delete a creature without causing a crash
    sObjHolder.Delete(this);
}

void Creature::OnJustDied()
{
    //        UpdateObject();
    // remove me as an attacker from the AttackMap
    GetAIInterface()->SetUnitToFollow(NULL);
    m_deathState = CORPSE;

    sEventMgr.AddEvent(this, &Creature::OnRemoveCorpse, EVENT_CREATURE_REMOVE_CORPSE, m_corpseDelay, 1);

    //TODO: remove all timers that should be removed on death
}

void Creature::OnRemoveCorpse()
{
    // time to respawn!
    if (IsInWorld())
    {
        sLog.outDetail("Removing corpse of "I64FMT"...", GetGUID());

        if(IsInWorld())
        {
            RemoveFromWorld();
        }
        // instanced creatures don't respawn
        if(m_respawnDelay > 0)
            sEventMgr.AddEvent(this, &Creature::OnRespawn, EVENT_CREATURE_RESPAWN, m_respawnDelay, 1);
        else
            sEventMgr.AddEvent(this, &Creature::DeleteMe, EVENT_CREATURE_SAFE_DELETE, 1000, 1);

        setDeathState(DEAD);

        /*m_positionX = respawn_cord[0];
        m_positionY = respawn_cord[1];
        m_positionZ = respawn_cord[2];
        m_orientation = respawn_cord[3];*/
        m_position = m_spawnLocation;
    }
    else
    {
        // if we got here it's pretty bad
    }
}

void Creature::OnRespawn()
{
    if (!IsInWorld())
    {
        sLog.outDetail("Respawning "I64FMT"...", GetGUID());
        SetUInt32Value(UNIT_FIELD_HEALTH, GetUInt32Value(UNIT_FIELD_MAXHEALTH));
        SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0); // not tagging shiat
        if(HasFlag(UNIT_FIELD_FLAGS,U_FIELD_FLAG_SKINNABLE))
            RemoveFlag(UNIT_FIELD_FLAGS,U_FIELD_FLAG_SKINNABLE);
        GetAIInterface()->resetSpellCounter();
        Skinned = false;
        Tagged = false;
        TaggerGuid = 0;

        //empty loot
        loot.items.clear();

        AddToWorld();
        setDeathState(ALIVE);
        GetAIInterface()->StopMovement(0); // after respawn monster can move
        m_PickPocketed = false;
    }
    else
    {
        // if we got here it's pretty bad
    }
}

void Creature::Despawn()
{
    GetAIInterface()->SetUnitToFollow(NULL);
    if(IsInWorld())
    {
        RemoveFromWorld();
    }
    sEventMgr.AddEvent(this, &Creature::OnRespawn, EVENT_CREATURE_RESPAWN, m_respawnDelay, 1);

    setDeathState(DEAD);

    /*m_positionX = respawn_cord[0];
    m_positionY = respawn_cord[1];
    m_positionZ = respawn_cord[2];    
    m_orientation = respawn_cord[3];*/
    m_position = m_spawnLocation;
}

void Creature::Create (const char* name, uint32 mapid, float x, float y, float z, float ang)
{
    Object::_Create(m_GuidLow, m_GuidHigh, mapid, x, y, z, ang);

    respawn_cord[0] = x;
    respawn_cord[1] = y;
    respawn_cord[2] = z;
    respawn_cord[3] = ang;

    m_name = name;
}

void Creature::CreateWayPoint (uint32 WayPointID, uint32 mapid, float x, float y, float z, float ang)
{
    Object::_Create(WayPointID, HIGHGUID_WAYPOINT, mapid, x, y, z, ang);
}

///////////
/// Looting

void Creature::generateLoot()
{
    lootmgr.FillCreatureLoot(&loot,GetEntry());
    //For now let fill according to entry
    CreatureInfo *info=GetCreatureName();
    if (info && info->Type != BEAST)
        loot.gold = (uint32)((info->Rank+1)*getLevel()*(rand()%5 + 1)*sWorld.getRate(RATE_DROP)); //generate copper

}

void Creature::SaveToDB()
{
    uint32 creatureEntry = GetUInt32Value(OBJECT_FIELD_ENTRY);
    if (!m_sqlid)
        m_sqlid = objmgr.GenerateLowGuid(HIGHGUID_UNIT);
    objmgr.SetCreatureBySqlId(m_sqlid, this);

    std::stringstream ss;
    ss << "DELETE FROM creatures WHERE id=" << m_sqlid;
    sDatabase.Execute(ss.str().c_str());

    ss.rdbuf()->str("");
    ss << "INSERT INTO creatures (id, mapId, zoneId, name_id, positionX, positionY, positionZ, orientation, moverandom, running, data) VALUES ( "
        << m_sqlid << ", "
        << GetMapId() << ", "
        << GetZoneId() << ", "
        << GetUInt32Value(OBJECT_FIELD_ENTRY) << ", "
        << m_position.x << ", "
        << m_position.y << ", "
        << m_position.z << ", "
        << m_position.o << ", "
        << GetAIInterface()->getMoveType() << ", "
        << GetAIInterface()->getMoveRunFlag() << ", '";
    for( uint16 index = 0; index < m_valuesCount; index ++ )
        ss << GetUInt32Value(index) << " ";

    ss << "' )";

    sDatabase.Execute( ss.str( ).c_str( ) );

    ss.rdbuf()->str("");
    ss << "UPDATE tempdamhe SET maxhealth=" << GetUInt32Value(UNIT_FIELD_BASE_HEALTH);
    ss << " WHERE entryid=" << creatureEntry;

    sDatabase.Execute( ss.str( ).c_str( ) );

    // TODO: save vendor items etc?
}

void Creature::SaveToFile(std::stringstream & name)
{
    FILE * OutFile;

    OutFile = fopen(name.str().c_str(), "wb");
    if (!OutFile) return;
    
    uint32 creatureEntry = GetUInt32Value(OBJECT_FIELD_ENTRY);
    if (!m_sqlid)
        m_sqlid = objmgr.GenerateLowGuid(HIGHGUID_UNIT);

    std::stringstream ss;
    ss << "DELETE FROM creatures WHERE id=" << m_sqlid;
    fwrite(ss.str().c_str(),1,ss.str().size(),OutFile);

    ss.rdbuf()->str("");
    ss << "\nINSERT INTO creatures (id, mapId, zoneId, name_id, positionX, positionY, positionZ, orientation, moverandom, running, data) VALUES ( "
        << m_sqlid << ", "
        << GetMapId() << ", "
        << GetZoneId() << ", "
        << GetUInt32Value(OBJECT_FIELD_ENTRY) << ", "
        << m_position.x << ", "
        << m_position.y << ", "
        << m_position.z << ", "
        << m_position.o << ", "
        << GetAIInterface()->getMoveType() << ", "
        << GetAIInterface()->getMoveRunFlag() << ", '";
    for( uint16 index = 0; index < m_valuesCount; index ++ )
        ss << GetUInt32Value(index) << " ";

    ss << "' )";
    fwrite(ss.str().c_str(),1,ss.str().size(),OutFile);
    fclose(OutFile);
}

void Creature::LoadProperties()
{
    // HACK! set call for help on civ health @ 100%
    if(creature_info && creature_info->Civilian >= 1)
        m_aiInterface->m_CallForHelpHealth = 100;

    //load resistances
    for(uint32 x=0;x<7;x++)
        BaseResistance[x]=GetUInt32Value(UNIT_FIELD_RESISTANCES+x);
    for(uint32 x=0;x<5;x++)
        BaseStats[x]=GetUInt32Value(UNIT_FIELD_STAT0+x);

    BaseDamage[0]=GetFloatValue(UNIT_FIELD_MINDAMAGE);
    BaseDamage[1]=GetFloatValue(UNIT_FIELD_MAXDAMAGE);
    BaseOffhandDamage[0]=GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE);
    BaseOffhandDamage[1]=GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE);
    BaseRangedDamage[0]=GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE);
    BaseRangedDamage[1]=GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE);

    SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);   // better set this one

#ifndef PRECACHE_LOAD
    Unit::LoadAIAgents();
#endif

    if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TAXIVENDOR) )
        m_TaxiNode = sTaxiMgr.GetNearestTaxiNode( m_position.x, m_position.y, m_position.z, GetMapId() );

    // init Faction
    _setFaction();

    // Set DYNAMIC_FLAGS to 0. Otherwise, if the mob was tagged when logged, it'll be tagged in our server.
    m_uint32Values[UNIT_DYNAMIC_FLAGS] = 0;

    if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP) && _gossipScript == 0 )
        SetGossipScript( sScriptMgr.GetGossipScript( GetEntry() ) );

    LoadScript();
    mTrainer = objmgr.GetTrainer(GetEntry());

    // find my family
    if(creature_info != 0)
    {
        myFamily = sCreatureFamilyStore.LookupEntry(creature_info->Family);
    }

    //HACK!
    if(m_uint32Values[UNIT_FIELD_DISPLAYID] == 17743 ||
        m_uint32Values[UNIT_FIELD_DISPLAYID] == 20242 ||
        m_uint32Values[UNIT_FIELD_DISPLAYID] == 15435 ||
        (creature_info && creature_info->Family == UNIT_TYPE_MISC))
    {
        m_useAI = false;
    }

    // load formation data
#ifndef PRECACHE_LOAD
    m_aiInterface->m_formationLinkSqlId = 0;
    m_aiInterface->m_formationFollowDistance = 0;
    m_aiInterface->m_formationFollowAngle = 0;

    QueryResult * result = sDatabase.Query("SELECT * FROM creature_formations WHERE creature_sqlid=%u", GetSQL_id());
    if(result)
    {
        Field * fields = result->Fetch();
        uint32 sid     = fields[1].GetUInt32();
        float dist     = fields[3].GetFloat();
        float angle    = fields[2].GetFloat();
        m_aiInterface->m_formationLinkSqlId = sid;
        m_aiInterface->m_formationFollowDistance = dist;
        m_aiInterface->m_formationFollowAngle = angle;
        delete result;
    }
#endif

    if(m_aiInterface->m_formationLinkSqlId)
    {
        // add event
        sEventMgr.AddEvent(this, &Creature::FormationLinkUp, m_aiInterface->m_formationLinkSqlId,
            EVENT_CREATURE_FORMATION_LINKUP, 1000, 0);
        haslinkupevent = true;
    }
}

// TODO: use full guids
bool Creature::LoadFromDB(uint32 guid)
{
    QueryResult *result = sDatabase.Query("SELECT * FROM creatures WHERE id=%u",guid);
    if(result == 0)
        return false;

    if(result->GetFieldCount() < 11)
    {
        sLog.outError("Can't load creature %u, %u fields instead of 11", guid, result->GetFieldCount());
        char text[300];
        sprintf(text, "Can't load creature %u, %u fields instead of 11", guid, result->GetFieldCount());
        sWorld.SendWorldText(text);
        sWorld.SendWorldWideScreenText(text);
        delete result;
        return false;
    }

    Field *fields = result->Fetch();

    //    _Create( guid, 0 );
    creature_info = objmgr.GetCreatureName(fields[8].GetUInt32());
    if(!creature_info)
    {
        sLog.outError("Creature %u failed due to no creature name. entry: %u", guid, fields[8].GetUInt32());
        delete result;
        return false;
    }
    Create(creature_info->Name.c_str(), fields[6].GetUInt32(),
    fields[1].GetFloat(), fields[2].GetFloat(), fields[3].GetFloat(), fields[4].GetFloat());

    m_sqlid = fields[0].GetUInt32();
    m_zoneId = fields[5].GetUInt32();
    m_nameEntry = fields[8].GetUInt32();
    GetAIInterface()->setMoveType(fields[9].GetUInt32());
    GetAIInterface()->setMoveRunFlag(fields[10].GetBool());

    LoadValues(fields[7].GetString());

    SetUInt32Value( OBJECT_FIELD_GUID, m_GuidLow );
    SetUInt32Value( OBJECT_FIELD_GUID+1, m_GuidHigh );

    delete result;

    if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VENDOR ) )
        m_SellItems = objmgr.GetVendorList(m_uint32Values[OBJECT_FIELD_ENTRY]);

    if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER ) )
        _LoadQuests();

    // Temp:LoadHealth
    _LoadHealth();
    _LoadMovement();

    LoadProperties();

    return true;
}

void Creature::LoadScript()
{
    _myScriptClass = sScriptMgr.CreateAIScriptClassForEntry(GetEntry(), this);
}

bool Creature::LoadFromDB(CreatureTemplate *t)
{
    creature_info = objmgr.GetCreatureName(t->_entry);
    if(!creature_info)
        return false;
    Create(creature_info->Name.c_str(), t->_map,
        t->_x, t->_y, t->_z, t->_o);

    m_sqlid = t->_guid;
    m_zoneId = t->_zone;
    m_nameEntry = t->_entry;
    m_aiInterface->m_formationLinkSqlId = t->formation_sqlid;
    m_aiInterface->m_formationFollowDistance = t->formation_dist;
    m_aiInterface->m_formationFollowAngle = t->formation_angle;

    m_aiInterface->setMoveType(t->_moverandom);
    m_aiInterface->setMoveRunFlag(t->_running);
    LoadValues(t->_fields);

    SetUInt32Value( OBJECT_FIELD_GUID, m_GuidLow );
    SetUInt32Value( OBJECT_FIELD_GUID+1, m_GuidHigh );

    if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VENDOR ) )
        m_SellItems = t->_items;

    if ( HasFlag( UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER ) )
        m_quests = t->_quests;

    _ApplyHealth(t);
    GetAIInterface()->setWaypointMap(t->_movement);

    LoadProperties();
    LoadAIAgents(t);
    return true;
}

//Temp:LoadHealth()
void Creature::_LoadHealth()
{
    uint32 creatureEntry = GetUInt32Value(OBJECT_FIELD_ENTRY);
    std::stringstream ss;
    ss << "SELECT * FROM tempdamhe WHERE entryid=" << creatureEntry;

    QueryResult *result = sDatabase.Query( ss.str().c_str() );

    CreatureInfo *ci = GetCreatureName();
    if(!ci) return;
    Field *fields;
    if(result)
    {
        fields = result->Fetch();
        uint32 level = GetUInt32Value(UNIT_FIELD_LEVEL);
        SetFloatValue(UNIT_FIELD_MINDAMAGE,fields[1].GetFloat());
        SetFloatValue(UNIT_FIELD_MAXDAMAGE,fields[2].GetFloat());
        if(GetFloatValue(UNIT_FIELD_MINDAMAGE) > 1500
            || GetFloatValue(UNIT_FIELD_MINDAMAGE) == 0)
        {
            if(level > 60)
                SetFloatValue(UNIT_FIELD_MINDAMAGE,float((level * 1.88)+level));
            else
                SetFloatValue(UNIT_FIELD_MINDAMAGE,float(level * 1.88));
        }
        if(GetFloatValue(UNIT_FIELD_MAXDAMAGE) > 5000
            || GetFloatValue(UNIT_FIELD_MAXDAMAGE) == 0)
        {
            if(level > 60)
                SetFloatValue(UNIT_FIELD_MAXDAMAGE,float((level * 3)+level));
            else
                SetFloatValue(UNIT_FIELD_MAXDAMAGE,float(level * 3));
        }
        //the Code Below has been added to make sure a creature is not too Powerfull
        //remove once the table in db is better
        uint32 OldHP = fields[3].GetUInt32();
        uint32 NewHP = 0;
        uint32 MinHP = 64;
        //using bounding Radius so bigger creatures get bigger values
        uint32 boundingradius = uint32(GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS)+1);
        //sLog.outDebug("Bounding Radius%u",boundingradius);
        //(20+boundingradius+level)*level
        if(ci)
        {
            if(ci->Type != CRITTER)
            {
                if(ci->Rank == 0) //Normal Mob ( Rare, Elite, Boss Keep Values in Table)
                {
                    NewHP = ((22+boundingradius+level)*level);
                    //NewHP = (64+(40*level))+25;
                }
                else
                {
                    NewHP = OldHP;
                }
            }
            else NewHP = 1;
        }
        else // don't have a name
        {
            NewHP = ((22+boundingradius+level)*level);
            //NewHP = (64+(40*level))+25;
        }
        //sLog.outDebug("Level %u, OldHP %u, NewHP %u", level, OldHP, NewHP);
        if(NewHP == 1) 
        { 
            //do nothing as it's a critter
        }
        
        else if(NewHP > OldHP || NewHP < MinHP)
        {
            NewHP = OldHP;
        }
        
        SetUInt32Value(UNIT_FIELD_HEALTH, NewHP);
        SetUInt32Value(UNIT_FIELD_MAXHEALTH, NewHP);
        SetUInt32Value(UNIT_FIELD_BASE_HEALTH, NewHP);

        delete result;
    }
    else
    {
        uint32 ulevel = GetUInt32Value(UNIT_FIELD_LEVEL);
        SetFloatValue(UNIT_FIELD_MINDAMAGE,float(ulevel * 1.88));
        SetFloatValue(UNIT_FIELD_MAXDAMAGE,float(ulevel * 3));
        /*SetUInt32Value(UNIT_FIELD_HEALTH,level * 56);
        SetUInt32Value(UNIT_FIELD_MAXHEALTH,level * 56);
        SetUInt32Value(UNIT_FIELD_BASE_HEALTH,level * 56);*/
        float level = ulevel;
        float randnum = sRand.rand(16) + 86;
        static float multipliers[5] = { 1.8f, 6.2f, 8.0f, 73.0f, 131.0f };
        uint32 rank = creature_info ? creature_info->Rank : 0;
        if(rank > 4) rank = 4;
        float multiplier = multipliers[rank];

        float health = ( (level * randnum) / 3.0f ) + ( level * multiplier );
        uint32 uhealth = FL2UINT(health);
        SetUInt32Value(UNIT_FIELD_MAXHEALTH, uhealth);
        SetUInt32Value(UNIT_FIELD_HEALTH, uhealth);
        SetUInt32Value(UNIT_FIELD_BASE_HEALTH, uhealth);
    }    
}

void Creature::_ApplyHealth(CreatureTemplate * t)
{
    CreatureInfo *ci = GetCreatureName();
    if(!ci) return;

    if(t->healthdata.mindmg || t->healthdata.maxdmg || t->healthdata.maxhealth)
    {
        uint32 level = GetUInt32Value(UNIT_FIELD_LEVEL);
        SetFloatValue(UNIT_FIELD_MINDAMAGE,t->healthdata.mindmg);
        SetFloatValue(UNIT_FIELD_MAXDAMAGE,t->healthdata.maxdmg);
        if(GetFloatValue(UNIT_FIELD_MINDAMAGE) > 1500
            || GetFloatValue(UNIT_FIELD_MINDAMAGE) == 0)
        {
            if(level > 60)
                SetFloatValue(UNIT_FIELD_MINDAMAGE,float((level * 1.88)+level));
            else
                SetFloatValue(UNIT_FIELD_MINDAMAGE,float(level * 1.88));
        }
        if(GetFloatValue(UNIT_FIELD_MAXDAMAGE) > 5000
            || GetFloatValue(UNIT_FIELD_MINDAMAGE) == 0)
        {
            if(level > 60)
                SetFloatValue(UNIT_FIELD_MAXDAMAGE,float((level * 3)+level));
            else
                SetFloatValue(UNIT_FIELD_MAXDAMAGE,float(level * 3));
        }
        //the Code Below has been added to make sure a creature is not too Powerfull
        //remove once the table in db is better
        uint32 OldHP = t->healthdata.maxhealth;
        uint32 NewHP = 0;
        uint32 MinHP = 64;
        //using bounding Radius so bigger creatures get bigger values
        uint32 boundingradius = uint32(GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS)+1);
        //sLog.outDebug("Bounding Radius%u",boundingradius);
        //(20+boundingradius+level)*level
        if(ci)
        {
            if(ci->Type != CRITTER)
            {
                if(ci->Rank == 0) //Normal Mob ( Rare, Elite, Boss Keep Values in Table)
                {
                    NewHP = ((22+boundingradius+level)*level);
                    //NewHP = (64+(40*level))+25;
                }
            }
            else NewHP = 1;
        }
        else // don't have a name 
        {
            NewHP = ((22+boundingradius+level)*level);
            //NewHP = (64+(40*level))+25;
        }
        //sLog.outDebug("Level %u, OldHP %u, NewHP %u", level, OldHP, NewHP);
        if(NewHP == 1) 
        { 
            //do nothing as it's a critter
        }
        else if(NewHP > OldHP || NewHP < MinHP)
        {
            NewHP = OldHP;
        }

        SetUInt32Value(UNIT_FIELD_HEALTH, NewHP);
        SetUInt32Value(UNIT_FIELD_MAXHEALTH, NewHP);
        SetUInt32Value(UNIT_FIELD_BASE_HEALTH, NewHP);
    }
    else
    {
        /*uint32 level = GetUInt32Value(UNIT_FIELD_LEVEL);
        SetFloatValue(UNIT_FIELD_MINDAMAGE,float(level * 1.88));
        SetFloatValue(UNIT_FIELD_MAXDAMAGE,float(level * 3));
        SetUInt32Value(UNIT_FIELD_HEALTH,level * 56);
        SetUInt32Value(UNIT_FIELD_MAXHEALTH,level * 56);
        SetUInt32Value(UNIT_FIELD_BASE_HEALTH,level * 56);*/
        uint32 ulevel = GetUInt32Value(UNIT_FIELD_LEVEL);
        SetFloatValue(UNIT_FIELD_MINDAMAGE,float(ulevel * 1.88));
        SetFloatValue(UNIT_FIELD_MAXDAMAGE,float(ulevel * 3));
        float level = ulevel;
        float randnum = sRand.rand(16) + 86;
        static float multipliers[5] = { 1.8f, 6.2f, 8.0f, 73.0f, 131.0f };
        uint32 rank = creature_info ? creature_info->Rank : 0;
        if(rank > 4) rank = 4;
        float multiplier = multipliers[rank];

        float health = ( (level * randnum) / 3.0f ) + ( level * multiplier );
        uint32 uhealth = FL2UINT(health);
        SetUInt32Value(UNIT_FIELD_MAXHEALTH, uhealth);
        SetUInt32Value(UNIT_FIELD_HEALTH, uhealth);
        SetUInt32Value(UNIT_FIELD_BASE_HEALTH, uhealth);
    }
}

void Creature::_LoadMovement()
{
      QueryResult *result = 
        sDatabase.Query("SELECT * FROM creature_waypoints WHERE creatureid=%u", m_sqlid );
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();

            WayPoint* wp = new WayPoint;
            wp->id = fields[1].GetUInt32();
            wp->x = fields[2].GetFloat();
            wp->y = fields[3].GetFloat();
            wp->z = fields[4].GetFloat();
            wp->waittime = fields[5].GetUInt32();
            wp->flags = fields[6].GetUInt32();
            wp->forwardemoteoneshot = fields[7].GetBool();
            wp->forwardemoteid = fields[8].GetUInt32();
            wp->backwardemoteoneshot = fields[9].GetBool();
            wp->backwardemoteid = fields[10].GetUInt32();
            wp->forwardskinid = fields[11].GetUInt32();
            wp->backwardskinid = fields[12].GetUInt32();

            GetAIInterface()->addWayPoint(wp);
            //addWaypoint( fields[0].GetFloat(), fields[1].GetFloat(), fields[2].GetFloat());
        }
        while( result->NextRow() );

        delete result;
    }
}

void Creature::DeleteFromDB()
{
    sDatabase.Execute("DELETE FROM creatures WHERE id=%u", m_sqlid);
    sDatabase.Execute("DELETE FROM creature_waypoints WHERE creatureid=%u", m_sqlid);
}


/////////////
/// Quests

void Creature::AddQuest(QuestRelation *Q)
{
    m_quests->push_back(Q);
}

void Creature::DeleteQuest(QuestRelation *Q)
{
    list<QuestRelation *>::iterator it;
    for ( it = m_quests->begin(); it != m_quests->end(); ++it )
    {
        if (((*it)->type == Q->type) && ((*it)->qst == Q->qst ))
        {
            delete (*it);
            m_quests->erase(it);
            break;
        }
    }
}

Quest* Creature::FindQuest(uint32 quest_id, uint8 quest_relation)
{   
    list<QuestRelation *>::iterator it;
    for (it = m_quests->begin(); it != m_quests->end(); ++it)
    {
        QuestRelation *ptr = (*it);

        if ((ptr->qst->id == quest_id) && (ptr->type & quest_relation))
        {
            return ptr->qst;
        }
    }
    return NULL;
}

uint16 Creature::GetQuestRelation(uint32 quest_id)
{
    uint16 quest_relation = 0;
    list<QuestRelation *>::iterator it;

    for (it = m_quests->begin(); it != m_quests->end(); ++it)
    {
        if ((*it)->qst->id == quest_id)
        {
            quest_relation |= (*it)->type;
        }
    }
    return quest_relation;
}

uint32 Creature::NumOfQuests()
{
    return m_quests->size();
}

void Creature::_LoadQuests()
{
    sQuestMgr.LoadNPCQuests(this);
}

void Creature::setDeathState(DeathState s) 
{
    m_deathState = s;
    if(s == JUST_DIED) 
    {
        //dunno why, but it was on next cycle, so i just left it that way
        sEventMgr.AddEvent(this, &Creature::OnJustDied, EVENT_CREATURE_JUSTDIED, 1, 1);
    }
}

void Creature::AddToWorld()
{
    /*if(UINT32_LOPART(m_uint32Values[1]) != HIGHGUID_PET)
        sEventMgr.AddEvent(this, &Creature::Update, (uint32)100, EVENT_CREATURE_UPDATE, 100, 0);*/

    // force set faciton
    if(m_faction == 0 || m_factionDBC == 0)
        _setFaction();
    
    if(m_faction == 0 || m_factionDBC == 0)
        return;

    sEventMgr.RemoveEvents(this, EVENT_CREATURE_RESPAWN);
    Unit::AddToWorld();

    // check if we got added or not
    if(m_mapMgr == 0)
    {
        // we did not get added! self-destruct :p
        sEventMgr.AddEvent(this, &Creature::SafeDelete, EVENT_CREATURE_SAFE_DELETE, 1, 1);
        return;
    }

    if(!_Activated && m_mapCell->IsActive() && UINT32_LOPART(GetGUIDHigh()) != HIGHGUID_PET)
        m_mapMgr->GetUpdaterThread()->AddObject<Creature>(this);
    else
        _Activated = true;  // so we get activated next time the cell goes active

    // Set respawn timer
    if(m_mapMgr->GetScript() != 0)
        m_respawnDelay = m_mapMgr->GetScript()->GetRespawnTimeForCreature(GetEntry(), this);

    if(_myScriptClass)
        _myScriptClass->OnLoad();

    objmgr.SetCreatureBySqlId(m_sqlid, this);
}

void Creature::RemoveFromWorld()
{
    if(IsInWorld())
    {
        _Activated = false;
        if(m_mapMgr->GetUpdaterThread())
        m_mapMgr->GetUpdaterThread()->RemoveObject<Creature>(this);

        RemoveAllAuras();
        /*sEventMgr.RemoveEvents(this, EVENT_CREATURE_UPDATE);
        sEventMgr.RemoveEvents(this, EVENT_CREATURE_JUSTDIED);
        sEventMgr.RemoveEvents(this, EVENT_CREATURE_REMOVE_CORPSE);*/
        //Disabled by Phantomas, if this is needed turn it back on
        sEventMgr.RemoveEvents(this);
        Unit::RemoveFromWorld();
    }
    objmgr.SetCreatureBySqlId(m_sqlid, 0);
}

void Creature::EnslaveExpire()
{
    if(!GetCreatureName()) return;
    
    m_enslaveCount++;
    Player *caster = objmgr.GetPlayer(GetUInt64Value(UNIT_FIELD_CHARMEDBY));
    if(caster)
    {
        caster->SetUInt64Value(UNIT_FIELD_CHARM, 0);
        caster->SetUInt64Value(UNIT_FIELD_SUMMON, 0);
        WorldPacket data;
        data.Initialize(SMSG_PET_SPELLS);
        data << uint64(0);
        caster->GetSession()->SendPacket(&data);
    }
    SetUInt64Value(UNIT_FIELD_CHARMEDBY, 0);
    SetUInt64Value(UNIT_FIELD_SUMMONEDBY, 0);
    SetIsPet(false);

    m_walkSpeed = 2.5f;
    m_runSpeed = 6.0f;

    switch(GetCreatureName()->Type)
    {
    case DEMON:
        SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, 90);
        break;
    default:
        SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, 954);
        break;
    };
    _setFaction();

    GetAIInterface()->Init(((Unit *)this), AITYPE_AGRO, MOVEMENTTYPE_NONE);

    // Add To Map
    //CreatureTemplate *t = new CreatureTemplate;
    //t->Create(this);
    //GetMapMgr()->GetBaseMap()->GetTemplate()->AddIndex<Creature>(this, t);
    // Update InRangeSet
    UpdateOppFactionSet();
}

bool Creature::RemoveEnslave()
{
    return RemoveAura(m_enslaveSpell);
}

void Creature::AddInRangeObject(Object* pObj)
{
    Unit::AddInRangeObject(pObj);
}

void Creature::RemoveInRangeObject(Object* pObj)
{
    if(totemOwner == pObj)        // player gone out of range of the totem
    {
        // Expire next loop.
        event_ModifyTimeLeft(EVENT_TOTEM_EXPIRE, 1);
    }

    Unit::RemoveInRangeObject(pObj);
}

void Creature::ClearInRangeSet()
{
    Unit::ClearInRangeSet();
}

void Creature::CalcResistance(uint32 type)
{
    int32 res = (BaseResistance[type] * (100 + BaseResistanceModPct[type])) / 100;
    if(res < 0) res = 0;
    res += FlatResistanceMod[type];
    if(res < 0) res = 0;
    res += (res * ResistanceModPct[type]) / 100;
    if(type==0)res+=GetUInt32Value(UNIT_FIELD_STAT1)*2;//fix armor from agi
    SetUInt32Value(UNIT_FIELD_RESISTANCES + type, res > 0 ? res : 0);

}

void Creature::CalcStat(uint32 type)
{
    int32 res=(BaseStats[type]*(100+StatModPct[type]))/100;
        
    res+=FlatStatMod[type];
    if(res<0)res=0;
        
    res+=(res*(TotalStatModPct[type]))/100;
    SetUInt32Value(UNIT_FIELD_STAT0+type,res>0?res:0);
}


void Creature::RegenerateHealth()
{
    uint32 cur=GetUInt32Value(UNIT_FIELD_HEALTH);
    uint32 mh=GetUInt32Value(UNIT_FIELD_MAXHEALTH);
    if(cur>=mh)return;
    
    //though creatures have their stats we use some wierd formula for amt
    float amt=(getLevel()*2)*PctRegenModifier;
        
    //Apply shit from conf file
    amt*=sWorld.getRate(RATE_HEALTH);
    
    if(amt<=1.0)//this fixes regen like 0.98
        cur++;
    else
        cur+=(uint32)amt;
    SetUInt32Value(UNIT_FIELD_HEALTH,(cur>=mh)?mh:cur);


}

void Creature::RegenerateMana()
{
    float amt;
    if (m_interruptRegen)
        return;
   
    uint32 cur=GetUInt32Value(UNIT_FIELD_POWER1);
    uint32 mm=GetUInt32Value(UNIT_FIELD_MAXPOWER1);
    if(cur>=mm)return;
    amt=(getLevel()+10)*PctPowerRegenModifier;
    
    //Apply shit from conf file
    amt*=sWorld.getRate(RATE_POWER1);
    if(amt<=1.0)//this fixes regen like 0.98
        cur++;
    else
        cur+=(uint32)amt;
    SetUInt32Value(UNIT_FIELD_POWER1,(cur>=mm)?mm:cur);
}

void Creature::RegenerateFocus()
{
    if (m_interruptRegen)
        return;

    uint32 cur=GetUInt32Value(UNIT_FIELD_POWER3);
    uint32 mm=GetUInt32Value(UNIT_FIELD_MAXPOWER3);
    if(cur>=mm)return;
    float amt = 10.0 * PctPowerRegenModifier;
    cur+=(uint32)amt;
    SetUInt32Value(UNIT_FIELD_POWER3,(cur>=mm)?mm:cur);
}

void Creature::Create(CreatureSpawnTemplate *pSpawnTemplate, uint32 mapid, float x, float y, float z, float ang )
{
    CreatureInfo *pInfo = objmgr.GetCreatureName(pSpawnTemplate->EntryID);
    Create(pInfo ? pInfo->Name.c_str() : "Unknown Creature", mapid, x, y, z, ang);

    if(pInfo)
        SetCreatureName(pInfo);

    // Set stats/abilities
    SetUInt32Value(OBJECT_FIELD_ENTRY, pSpawnTemplate->EntryID);
    SetFloatValue(OBJECT_FIELD_SCALE_X, pSpawnTemplate->Scale);
    SetUInt32Value(UNIT_FIELD_DISPLAYID, pInfo ? pInfo->DisplayID : 0);
    SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, pInfo ? pInfo->DisplayID : 0);
    SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, pSpawnTemplate->MountModelID);
    SetUInt32Value(UNIT_NPC_FLAGS, pSpawnTemplate->Flag);
    SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE , pSpawnTemplate->Faction);
    SetUInt32Value(UNIT_FIELD_HEALTH, pSpawnTemplate->MaxHealth);
    SetUInt32Value(UNIT_FIELD_MAXHEALTH, pSpawnTemplate->MaxHealth);
    SetUInt32Value(UNIT_FIELD_RESISTANCES, pSpawnTemplate->Armor);
    uint8 Race = 0;
    uint8 Class = 0;
    uint8 Gender = 0;
    uint8 PowerType = POWER_TYPE_MANA;
    if(pSpawnTemplate->MaxMana == 0)
    {
        SetUInt32Value(UNIT_FIELD_MAXPOWER4, 100);
        SetUInt32Value(UNIT_FIELD_POWER4, 100);
        PowerType = POWER_TYPE_ENERGY;
    }
    else
    {
        SetUInt32Value(UNIT_FIELD_MAXPOWER1, pSpawnTemplate->MaxMana );
        SetUInt32Value(UNIT_FIELD_POWER1, pSpawnTemplate->MaxMana );
    }
    SetUInt32Value(UNIT_FIELD_BYTES_0, ( ( Race ) | ( Class << 8 ) | ( Gender << 16 ) | ( PowerType << 24 ) ) );
    SetUInt32Value(UNIT_FIELD_LEVEL, pSpawnTemplate->Level );
    SetFloatValue(UNIT_FIELD_COMBATREACH , pSpawnTemplate->CombatReach);
    SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS , pSpawnTemplate->BoundingRadius);
    SetFloatValue(UNIT_FIELD_MINDAMAGE , pSpawnTemplate->MinDamage);
    SetFloatValue(UNIT_FIELD_MAXDAMAGE ,  pSpawnTemplate->MaxDamage);
    SetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE , pSpawnTemplate->MinRangedDamage);
    SetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE , pSpawnTemplate->MaxRangedDamage);

    SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, pSpawnTemplate->BaseAttackTime);
    SetUInt32Value(UNIT_FIELD_RANGEDATTACKTIME, pSpawnTemplate->RangedAttackTime);

    if(pSpawnTemplate->Slot1Model != 0)
    {
        SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, pSpawnTemplate->Slot1Model);
        SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, ( ( uint8(pSpawnTemplate->Slot1Info1) ) | ( uint8(pSpawnTemplate->Slot1Info2) << 8 ) | ( uint8(pSpawnTemplate->Slot1Info3) << 16 ) | ( uint8(pSpawnTemplate->Slot1Info4) << 24 ) ) );
        SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+1, ( ( uint8(pSpawnTemplate->Slot1Info5) ) | ( uint8(0) << 8 ) | ( uint8(0) << 16 ) | ( uint8(0) << 24 ) ) );
    }
    if(pSpawnTemplate->Slot2Model != 0)
    {
        SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, pSpawnTemplate->Slot2Model);
        SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+2, ( ( uint8(pSpawnTemplate->Slot2Info1) ) | ( uint8(pSpawnTemplate->Slot2Info2) << 8 ) | ( uint8(pSpawnTemplate->Slot2Info3) << 16 ) | ( uint8(pSpawnTemplate->Slot2Info4) << 24 ) ) );
        SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+3, ( ( uint8(pSpawnTemplate->Slot2Info5) ) | ( uint8(0) << 8 ) | ( uint8(0) << 16 ) | ( uint8(0) << 24 ) ) );
    }
    if(pSpawnTemplate->Slot3Model != 0)
    {
        SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+2, pSpawnTemplate->Slot3Model);
        SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+4, ( ( uint8(pSpawnTemplate->Slot3Info1) ) | ( uint8(pSpawnTemplate->Slot3Info2) << 8 ) | ( uint8(pSpawnTemplate->Slot3Info3) << 16 ) | ( uint8(pSpawnTemplate->Slot3Info4) << 24 ) ) );
        SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+5, ( ( uint8(pSpawnTemplate->Slot3Info5) ) | ( uint8(0) << 8 ) | ( uint8(0) << 16 ) | ( uint8(0) << 24 ) ) );
    }
    SetUInt32Value(UNIT_FIELD_BASE_HEALTH, pSpawnTemplate->MaxHealth);
    SetUInt32Value(UNIT_FIELD_BASE_MANA, pSpawnTemplate->MaxMana);
    if(HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
        _LoadQuests();

    _LoadHealth();
    LoadProperties();
}

void Creature::CallScriptUpdate()
{
    ASSERT(_myScriptClass);
    if(!IsInWorld())
        return;

    _myScriptClass->AIUpdate();
}

void Creature::AddVendorItem(uint32 itemid, uint32 amount)
{
    CreatureItem ci;
    ci.amount = amount;
    ci.itemid = itemid;
    if(!m_SellItems)
    {
        m_SellItems = new vector<CreatureItem>;
        objmgr.SetVendorList(GetEntry(), m_SellItems);
    }
    m_SellItems->push_back(ci);
}

void Creature::TotemExpire()
{
    totemOwner->m_TotemSlots[totemSlot] = 0;
    totemSlot = -1;
    totemOwner = 0;

    RemoveFromWorld();
    SafeDelete();
}

void Creature::FormationLinkUp(uint32 SqlId)
{
    Creature * creature = objmgr.GetCreatureBySqlId(SqlId);
    if(creature != 0)
    {
        m_aiInterface->m_formationLinkTarget = creature;
        haslinkupevent = false;
        event_RemoveEvents(EVENT_CREATURE_FORMATION_LINKUP);
    }
}

void Creature::LoadAIAgents(CreatureTemplate * t)
{
    vector<AI_Spell*>::iterator itr = t->_aiSpells.begin();
    AI_Spell * sp;
    for(; itr != t->_aiSpells.end(); ++itr)
    {
        sp = *itr;

        if(sp->agent == AGENT_RANGED)
        {
            GetAIInterface()->m_canRangedAttack = true;
        }
        else if(sp->agent == AGENT_FLEE)
        {
            GetAIInterface()->m_canFlee = true;
            if(sp->floatMisc1)
            {
                GetAIInterface()->m_FleeHealth = sp->floatMisc1;
            }
            else
            {
                GetAIInterface()->m_FleeHealth = 0.2f;
            }
            if(sp->Misc2)
            {
                GetAIInterface()->m_FleeDuration = sp->Misc2;
            }
            else
            {
                GetAIInterface()->m_FleeDuration = 10000;
            }
        }
        else if(sp->agent == AGENT_CALLFORHELP)
        {
            GetAIInterface()->m_canCallForHelp = true;
            if(sp->floatMisc1)
                GetAIInterface()->m_CallForHelpHealth = sp->floatMisc1;
            else
                GetAIInterface()->m_CallForHelpHealth = 0.2f;
        }
        else
        {
            GetAIInterface()->addSpellToList(sp);
        }
    }
}

void Creature::LoadAIAgents()
{
    std::stringstream ss;
    ss << "SELECT * FROM ai_agents where entryId=" << GetUInt32Value(OBJECT_FIELD_ENTRY);
    QueryResult *result = sDatabase.Query( ss.str().c_str() );

    if( !result )
        return;

    AI_Spell *sp;

    do
    {
        Field *fields = result->Fetch();

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

        if(sp->agent == AGENT_RANGED)
        {
            GetAIInterface()->m_canRangedAttack = true;
        }
        else if(sp->agent == AGENT_FLEE)
        {
            GetAIInterface()->m_canFlee = true;
            if(sp->floatMisc1)
            {
                GetAIInterface()->m_FleeHealth = sp->floatMisc1;
            }
            else
            {
                GetAIInterface()->m_FleeHealth = 0.2f;
            }
            if(sp->Misc2)
            {
                GetAIInterface()->m_FleeDuration = sp->Misc2;
            }
            else
            {
                GetAIInterface()->m_FleeDuration = 10000;
            }
        }
        else if(sp->agent == AGENT_CALLFORHELP)
        {
            GetAIInterface()->m_canCallForHelp = true;
            if(sp->floatMisc1)
                GetAIInterface()->m_CallForHelpHealth = sp->floatMisc1;
            else
                GetAIInterface()->m_CallForHelpHealth = 0.2f;
        }
        else
        {
            GetAIInterface()->addSpellToList(sp);
        }
    } while( result->NextRow() );

    delete result;
}

WayPoint * Creature::CreateWaypointStruct()
{
    return new WayPoint();
}
