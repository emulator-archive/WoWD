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
GameObject::GameObject(uint32 high, uint32 low) : Object(high, low)
{
    m_objectType |= TYPE_GAMEOBJECT;
    m_objectTypeId = TYPEID_GAMEOBJECT;

    m_valuesCount = GAMEOBJECT_END;
    m_ritualmembers = NULL;
    counter=0;//not needed at all but to prevent errors that var was not inited, can be removed in release
    
    pcbannerAura = NULL;
    bannerslot = -1;

    m_sqlid = 0;
    m_summonedGo = false;
    invisible = false;
    invisibilityFlag = INVISIBILTY_FLAG_NONE;
    spell = 0;
    m_summoner = NULL;
    charges = (uint32)-1;
    m_ritualcaster = NULL;
    m_ritualtarget = NULL;
    m_ritualmembers = NULL;
    m_ritualspell = 0;

    m_quests = NULL;
    pInfo = NULL;
    myScript = NULL;
    sWorld.mLoadedGameObjects[0]++;
}

GameObject::~GameObject()
{
    sWorld.mLoadedGameObjects[0]--; 
    sEventMgr.RemoveEvents(this);
    delete[] m_ritualmembers;

    uint64 guid;
    if(guid = GetUInt64Value(OBJECT_FIELD_CREATED_BY))
    {
        Player *plr = World::GetPlayer(guid);
        if(plr && plr->GetSummonedObject() == this)
            plr->SetSummonedObject(NULL);
    }

    if (m_summonedGo)
        for(int i = 0; i < 4; i++)
            if (m_summoner->m_ObjectSlots[i] == this)
                m_summoner->m_ObjectSlots[i] = NULL;
}

void GameObject::CreateFromProto(uint32 entry,uint32 mapid, float x, float y, float z, float ang)
{
    GameObjectInfo *info= objmgr.GetGameObjectName(entry);

    Object::_Create(m_GuidLow, m_GuidHigh, mapid, x, y, z, ang);
    SetUInt32Value( OBJECT_FIELD_ENTRY, entry );
    
    SetFloatValue( GAMEOBJECT_POS_X, x);
    SetFloatValue( GAMEOBJECT_POS_Y, y );
    SetFloatValue( GAMEOBJECT_POS_Z, z );
    SetFloatValue( GAMEOBJECT_FACING, ang );
    SetFloatValue( OBJECT_FIELD_SCALE_X, 1);//info->Size  );
     
    //SetUInt32Value( GAMEOBJECT_TIMESTAMP, (uint32)time(NULL));
    SetUInt32Value(GAMEOBJECT_ARTKIT, 0);           //these must be from wdb somewhere i guess
    SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, 0);
    SetUInt32Value( GAMEOBJECT_STATE,1);
    SetUInt32Value( GAMEOBJECT_DISPLAYID,info->DisplayID);
    SetUInt32Value( GAMEOBJECT_TYPE_ID,info->Type );
   
    m_ritualcaster = NULL;
    m_ritualtarget = NULL;
    m_ritualspell = 0;
        
    if(info->Type == GAMEOBJECT_TYPE_RITUAL)
    {    
        m_ritualmembers = new Player*[info->SpellFocus];
        memset(m_ritualmembers,0,sizeof(Player*)*info->SpellFocus);
    }else
    {
        m_ritualmembers = NULL;
    }
    InitAI();
    myScript = sScriptMgr.CreateAIScriptClassForGameObject(GetEntry(), this);
}

void GameObject::Create(uint32 mapid, float x, float y, float z, float ang)
{
    Object::_Create(m_GuidLow, m_GuidHigh, mapid, x, y, z, ang);

    SetFloatValue( GAMEOBJECT_POS_X, x);
    SetFloatValue( GAMEOBJECT_POS_Y, y );
    SetFloatValue( GAMEOBJECT_POS_Z, z );
    SetFloatValue( GAMEOBJECT_FACING, ang );
    //SetUInt32Value( GAMEOBJECT_TIMESTAMP, (uint32)time(NULL));
}

void GameObject::Create( uint32 guidlow, uint32 guidhigh,uint32 displayid, uint8 state, uint32 entryid, float scale,uint32 typeId, uint32 type,uint32 flags, uint32 mapid, float x, float y, float z, float ang )
{
    Object::_Create(guidlow, guidhigh, mapid, x, y, z, ang);

    m_objectType = type;
    SetUInt32Value( OBJECT_FIELD_ENTRY, entryid );
    SetFloatValue( OBJECT_FIELD_SCALE_X, scale );
    SetUInt32Value( GAMEOBJECT_DISPLAYID, displayid );
    SetUInt32Value( GAMEOBJECT_STATE, state  );
    SetUInt32Value( GAMEOBJECT_TYPE_ID, typeId  );
    SetUInt32Value( GAMEOBJECT_FLAGS, flags );
}

void GameObject::TrapSearchTarget()
{
    Update(100);
}

void GameObject::Update(uint32 p_time)
{
    if(spell && (GetUInt32Value(GAMEOBJECT_STATE) == 1))
    {
        if(checkrate > 1)
        {
            if(counter++%checkrate)
                return;
        }
        ObjectSet::iterator itr = GetInRangeSetBegin();
        ObjectSet::iterator it2 = itr;
        ObjectSet::iterator iend = GetInRangeSetEnd();
        Unit * pUnit;
        float dist;
        for(; it2 != iend;)
        {
            itr = it2;
            ++it2;
            dist = GetDistanceSq((*itr));
            if( (*itr) != m_summoner && (*itr)->IsUnit() && dist <= range)
            {
                pUnit = static_cast<Unit*>(*itr);

                if(m_summonedGo)
                if(!isAttackable(m_summoner,pUnit))continue;

                Spell * sp=new Spell((Object*)this,spell,true,NULL);
                SpellCastTargets tgt((*itr)->GetGUID());
                tgt.m_destX = GetPositionX();
                tgt.m_destY = GetPositionY();
                tgt.m_destZ = GetPositionZ();
                sp->prepare(&tgt);
                if(m_summonedGo)
                {
                    Expire();
                    return;
                }

                if(spell->EffectImplicitTargetA[0] == 16 ||
                    spell->EffectImplicitTargetB[0] == 16)
                    return;     // on area dont continue.
            }
        }
    }
}

void GameObject::Spawn()
{
    if(!this->IsInWorld())
        AddToWorld();
    
    CALL_GO_SCRIPT_EVENT(this, OnSpawn)();
}

void GameObject::Despawn(uint32 time)
{
    loot.items.clear();

    //This is for go get deleted while looting
    SetUInt32Value(GAMEOBJECT_STATE, GetBaseUInt32Value(GAMEOBJECT_STATE));
    SetUInt32Value(GAMEOBJECT_FLAGS, GetBaseUInt32Value(GAMEOBJECT_FLAGS));

    WorldPacket data(SMSG_GAMEOBJECT_DESPAWN_ANIM, 8);
    data << GetGUID();
    SendMessageToSet(&data,true);

    if(this->IsInWorld())
        RemoveFromWorld();
  
    if(time)
    {
        sEventMgr.AddEvent(this, &GameObject::Spawn, EVENT_GAMEOBJECT_ITEM_SPAWN, time, 1);
    }
    CALL_GO_SCRIPT_EVENT(this, OnDespawn)();
}

void GameObject::SaveToDB()
{
    std::stringstream ss;
    if (!m_sqlid)
        m_sqlid = objmgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT);

    ss << "DELETE FROM gameobjects WHERE id=" << m_sqlid;
    sDatabase.Execute(ss.str().c_str());

    ss.rdbuf()->str("");
    ss << "INSERT INTO gameobjects VALUES ( "
        << m_sqlid << ", "
        << m_position.x << ", "
        << m_position.y << ", "
        << m_position.z << ", "
        << m_position.o << ", "
        << GetZoneId() << ", "
        << GetMapId() << ", '";

    for( uint32 index = 0; index < m_valuesCount; index ++ )
        ss << GetUInt32Value(index) << " ";

    ss << "', ";
    ss << GetEntry() << ", 0, 0)"; 

    sDatabase.Execute( ss.str( ).c_str( ) );
}

void GameObject::SaveToFile(std::stringstream & name)
{
    std::stringstream ss;
    if (!m_sqlid)
        m_sqlid = objmgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT);

     ss.rdbuf()->str("");
     ss << "INSERT INTO gameobjects VALUES ( "
        << m_sqlid << ", "
        << m_position.x << ", "
        << m_position.y << ", "
        << m_position.z << ", "
        << m_position.o << ", "
        << GetZoneId() << ", "
        << GetMapId() << ", '";

    for( uint32 index = 0; index < m_valuesCount; index ++ )
        ss << GetUInt32Value(index) << " ";

    ss << "', ";
    ss << GetEntry() << ", 0, 0)"; 

    FILE * OutFile;

    OutFile = fopen(name.str().c_str(), "wb");
    if (!OutFile) return;
    fwrite(ss.str().c_str(),1,ss.str().size(),OutFile);
    fclose(OutFile);
}

void GameObject::InitAI()
{
    GameObjectInfo* inf = objmgr.GetGameObjectName( GetEntry() );
    pInfo = inf;

    if(!inf)
        return;
    if(inf->DisplayID == 0)
        return;

    // this fixes those fuckers in booty bay
    if(inf->SpellFocus == 0 &&
        inf->sound1 == 0 &&
        inf->sound2 == 0 &&
        inf->sound3 != 0 &&
        inf->sound5 != 3 &&
        inf->sound9 == 1)
        return;

    if(inf->DisplayID == 1027)//Shaman Shrine
    {
        if(inf->ID != 177964 || inf->ID != 153556)
        {
            //Deactivate
            //SetUInt32Value(GAMEOBJECT_DYN_FLAGS, 0);
        }
    }

    uint32 spellid = 0;
    if(inf->Type==GAMEOBJECT_TYPE_TRAP)
    {    
        spellid = inf->sound3;
    }
    else if(inf->Type == GAMEOBJECT_TYPE_SPELL_FOCUS)//redirect to properties of another go
    {
        uint32 new_entry = inf->sound2;
        if(!new_entry)
            return;
        inf = objmgr.GetGameObjectName( new_entry );
        if(!inf)
            return;
        spellid = inf->sound3;
    }
    // hackfix for bad spell in BWL
    if(!spellid || spellid == 22247)
        return;

    SpellEntry *sp= sSpellStore.LookupEntry(spellid);
    if(!sp)
    {
        spell = NULL;
        return;
    }
    else
    {
        spell = sp;
    }
    //ok got valid spell that will be casted on target when it comes close enough
    //get the range for that 
    
    float r = 0;

    for(uint32 i=0;i<3;i++)
    {
        if(sp->Effect[i])
        {
            float t = GetRadius(sSpellRadius.LookupEntry(sp->EffectRadiusIndex[i]));
            if(t > r)
                r = t;
        }
    }

    if(r < 0.1)//no range
        r = GetMaxRange(sSpellRange.LookupEntry(sp->rangeIndex));

    range = r*r;//square to make code faster
    checkrate = 20;//once in 2 seconds
    
}

bool GameObject::LoadFromDB(uint32 guid)
{
    m_sqlid = guid;

    QueryResult *result = sDatabase.Query("SELECT * FROM gameobjects WHERE id=%u", guid);
    if(result == 0)
        return false;
 
    ASSERT(result);

    Field *fields = result->Fetch();

    Create(fields[6].GetUInt32(), fields[1].GetFloat(), fields[2].GetFloat(), fields[3].GetFloat(), fields[4].GetFloat());

    m_zoneId = fields[5].GetUInt32();

    LoadValues(fields[7].GetString());

    SetUInt32Value( OBJECT_FIELD_GUID, m_GuidLow );
    SetUInt32Value( OBJECT_FIELD_GUID+1, m_GuidHigh );

    InitAI();
    myScript = sScriptMgr.CreateAIScriptClassForGameObject(GetEntry(), this);

    _LoadQuests();
    delete result;
    return true;
}

void GameObject::LoadFromDB(GameObjectTemplate *t)
{
    m_sqlid = t->_guid;
    Create( t->_map, t->_x, t->_y, t->_z, t->_o);
    m_zoneId = t->_zone;
    LoadValues(t->_fields);

    SetUInt32Value( OBJECT_FIELD_GUID, m_GuidLow );
    SetUInt32Value( OBJECT_FIELD_GUID+1, m_GuidHigh );

    m_quests = t->_quests;
/*
    GameObjectInfo* gTemplate = objmgr.GetGameObjectName( GetEntry());
    m_aiType = gTemplate->AiType;

    if( m_aiType == 1 || m_aiType == 2 || m_aiType == 3 || m_aiType == 4 )
        sEventMgr.AddEvent(this, &GameObject::_EnvironmentalDamageUpdate, EVENT_GAMEOBJECT_ENV_DAMAGE, 3800, 0);
  */

    InitAI();
    myScript = sScriptMgr.CreateAIScriptClassForGameObject(GetEntry(), this);
    //FIXME need to add m_quests to ObjectTemplate and set it here
}

void GameObject::DeleteFromDB()
{
    sDatabase.Execute("DELETE FROM gameobjects WHERE id=%u", m_sqlid);
}

void GameObject::EventCloseDoor()
{
    SetUInt32Value(GAMEOBJECT_STATE, 0);
}

void GameObject::UseFishingNode(Player *player)
{
    sEventMgr.RemoveEvents(this);
    if(GetUInt32Value(GAMEOBJECT_STATE))//click on bobber before somth is hooked
    {
        player->GetSession()->OutPacket(SMSG_FISH_NOT_HOOKED);
        EndFishing(player,true);
        return;
    }
    
    uint32 zone=sAreaStore.LookupEntry(GetMapMgr()->GetAreaID(GetPositionX(),GetPositionY()))->ZoneId;

    FishingZoneEntry *entry = objmgr.GetFishingZone(zone);
    if(entry == NULL)
    {
        sLog.outError("ERROR: Fishing zone information for zone %d not found!", zone);
        EndFishing(player, true);
        return;
    }
    uint32 maxskill=entry->MaxSkill;
    uint32 minskill=entry->MaxSkill;

    if(player->GetBaseSkillAmt(SKILL_FISHING)<maxskill)    
        player->AdvanceSkillLine(SKILL_FISHING);

    //Open loot on success, otherwise FISH_ESCAPED.
    if(Rand(((player->GetSkillAmt(SKILL_FISHING)-minskill)*100)/maxskill))
    {              
        lootmgr.FillProfessionLoot(&lootmgr.FishingLoot,&loot,zone);
        player->SendLoot(GetGUID(),3);
        EndFishing(player, false);
    }
    else //failed
    {
        player->GetSession()->OutPacket(SMSG_FISH_ESCAPED);
        EndFishing(player,false);
    }

}

void GameObject::EndFishing(Player* player, bool abort )
{
    Spell * spell = player->GetCurrentSpell();
    
    if(spell)
    {
        if(abort)   // abort becouse of a reason
        {
            //FIXME: here 'failed' should appear over progress bar
            spell->cancel();
        }
        else        // spell ended
        {
            if (!(GetUInt32Value(GAMEOBJECT_FLAGS) & 32)) // if there was no loot
                spell->SendCastResult(SPELL_FAILED_NO_FISH);

            spell->SendChannelUpdate(0);
            spell->finish();

            if(this->IsInWorld())
                RemoveFromWorld();
            sObjHolder.Delete(this);
        }
    }
    else // if this is called, and there is no spell so remove the gameobject
    {
        if(this->IsInWorld())
                RemoveFromWorld();
        sObjHolder.Delete(this);
    }
}

void GameObject::FishHooked(Player * player)
{
    WorldPacket  data;
    data.Initialize(SMSG_GAMEOBJECT_CUSTOM_ANIM); 
    data << GetGUID();
    data << (uint32)(0);
    player->GetSession()->SendPacket(&data);
    SetUInt32Value(GAMEOBJECT_STATE, 0);
    BuildFieldUpdatePacket(player, GAMEOBJECT_FLAGS, 32);
 }

/////////////
/// Quests

void GameObject::AddQuest(QuestRelation *Q)
{
    m_quests->push_back(Q);
}

void GameObject::DeleteQuest(QuestRelation *Q)
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

Quest* GameObject::FindQuest(uint32 quest_id, uint8 quest_relation)
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

uint16 GameObject::GetQuestRelation(uint32 quest_id)
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

uint32 GameObject::NumOfQuests()
{
    return m_quests->size();
}

void GameObject::_LoadQuests()
{
    sQuestMgr.LoadGOQuests(this);
}


/////////////////
// Summoned Go's

void GameObject::Expire()
{
    if(m_inWorld)
        RemoveFromWorld();

    sObjHolder.Delete(this);
}

void GameObject::Deactivate()
{
    SetUInt32Value(GAMEOBJECT_DYN_FLAGS, 0);
}

void GameObject::CallScriptUpdate()
{
    ASSERT(myScript);
    myScript->AIUpdate();
}

void GameObject::AddToWorld()
{
    Object::AddToWorld();
    if(m_mapMgr->GetScript() != 0)
    {
        // Find out if we should lock by default
        m_mapMgr->GetScript()->SetLockOptions(GetEntry(), this);        
    }

    if(!_Activated && m_mapCell && m_mapCell->IsActive() && UINT32_LOPART(GetGUIDHigh()) != HIGHGUID_TRANSPORTER
        && m_uint32Values[GAMEOBJECT_TYPE_ID] != GAMEOBJECT_TYPE_TRAP)  // traps have their own update - faster time
    {
        m_mapMgr->GetUpdaterThread()->AddObject<GameObject>(this);
        _Activated = true;
    }
}