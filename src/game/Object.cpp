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
#include "Unit.h"
using namespace std;

//#define DEG2RAD (M_PI/180.0)
#define M_PI       3.14159265358979323846

Object::Object( uint32 high, uint32 low ) : m_position(0,0,0,0), m_spawnLocation(0,0,0,0)
{
    m_GuidLow = low;
    m_GuidHigh = high;

    m_objectTypeId = TYPEID_OBJECT;
    m_objectType = TYPE_OBJECT;

    m_mapId = 0;
    m_zoneId = 0;

    m_uint32Values = 0;
    m_baseUint32Values = 0;

    m_inWorld = false;
    m_objectUpdated = false;

    m_minZ = -500;

    m_valuesCount = 0;

    //official Values
    m_walkSpeed = 2.5f;
    m_runSpeed = 7.0f;
    m_flySpeed = 7.0f;
    m_backFlySpeed = 4.5f;

    m_backWalkSpeed = 2.5f;
    m_swimSpeed = 4.722222f;
    m_backSwimSpeed = 2.5f;
    m_turnRate = 3.141593f;

    m_mapMgr = 0;
    m_mapCell = 0;

    mSemaphoreTeleport = false;

    m_SpawnedObject = false;

    m_faction = NULL;
    m_factionDBC = NULL;

    m_instanceId = 0;
    _Activated = false;

    //sWorld.AddGlobalObject(this);
}

Object::~Object( )
{
    if (this->m_inWorld)
        this->RemoveFromWorld();
    /*else
        sWorld.RemoveGlobalObject(this);*/

    if(m_uint32Values)
        delete [] m_uint32Values;

    if(m_baseUint32Values)
        delete [] m_baseUint32Values;

    if(m_mapMgr)
        m_mapMgr->RemoveObject(this);
    m_updateMask.Clear();
}

void Object::_Create( uint32 guidlow, uint32 guidhigh )
{
    _InitValues();

    SetUInt32Value( OBJECT_FIELD_GUID, guidlow );
    SetUInt32Value( OBJECT_FIELD_GUID+1, guidhigh );
    SetUInt32Value( OBJECT_FIELD_TYPE, m_objectType );

    m_wowGuid.Init(GetGUID());
}

void Object::_Create( uint32 guidlow, uint32 guidhigh, uint32 mapid, float x, float y, float z, float ang )
{
    _InitValues();

    SetUInt32Value( OBJECT_FIELD_GUID, guidlow );
    SetUInt32Value( OBJECT_FIELD_GUID+1, guidhigh );
    SetUInt32Value( OBJECT_FIELD_TYPE, m_objectType );

    m_wowGuid.Init(GetGUID());

    m_mapId = mapid;
    m_position.ChangeCoords(x, y, z, ang);
    m_spawnLocation.ChangeCoords(x, y, z, ang);
}

uint32 Object::BuildCreateUpdateBlockForPlayer(ByteBuffer *data, Player *target)
{
    uint8 flags = 0;
    uint32 flags2 = 0;

    uint8 updatetype = UPDATETYPE_CREATE_OBJECT;

    // any other case
    switch(m_objectTypeId)
    {
        // items + containers: 0x8
    case TYPEID_ITEM:
    case TYPEID_CONTAINER:
        flags = 0x8;
        break;
        
        // player/unit: 0x68 (except self)
    case TYPEID_UNIT:
        flags = 0x68;
        break;

    case TYPEID_PLAYER:
        flags = 0x60;
        break;

        // gameobject/dynamicobject
    case TYPEID_GAMEOBJECT:
    case TYPEID_DYNAMICOBJECT:
    case TYPEID_CORPSE:
        flags = 0x48;
        break;

        // anyone else can get fucked and die!
    }

    if(target == this)
    {
        // player creating self
        flags |= 0x01;
        updatetype = UPDATETYPE_CREATE_YOURSELF;
    }

    // gameobject stuff
    if(m_objectTypeId == TYPEID_GAMEOBJECT)
    {
        switch(m_uint32Values[GAMEOBJECT_TYPE_ID])
        {
            case GAMEOBJECT_TYPE_MO_TRANSPORT:  
                {
                    if(UINT32_LOPART(GetGUIDHigh()) != HIGHGUID_TRANSPORTER)
                        return 0;   // bad transporter
                    else
                        flags = 0x4A;
                }break;

            case GAMEOBJECT_TYPE_DUEL_ARBITER:
                {
                    // duel flags have to stay as updatetype 3, otherwise
                    // it won't animate
                    updatetype = UPDATETYPE_CREATE_YOURSELF;
                }break;
        }
    }

    // build our actual update
    *data << updatetype;

    // we shouldn't be here, under any cercumstances, unless we have a wowguid..
    ASSERT(m_wowGuid.GetNewGuidLen());
    *data << m_wowGuid;
    
    *data << m_objectTypeId;

    _BuildMovementUpdate(data, flags, flags2, target);

    // we have dirty data, or are creating for ourself.
    UpdateMask updateMask;
    updateMask.SetCount( m_valuesCount );
    _SetCreateBits( &updateMask, target );

    // this will cache automatically if needed
    _BuildValuesUpdate( data, &updateMask, target );

    // update count: 1 ;)
    return 1;
}


//That is dirty fix it actually creates update of 1 field with
//the given value ignoring existing changes in fields and so on
//usefull if we want update this field for certain players
//NOTE: it does not change fields. This is also very fast method
WorldPacket *Object::BuildFieldUpdatePacket( uint32 index,uint32 value)
{
   // uint64 guidfields = GetGUID();
   // uint8 guidmask = 0;
    WorldPacket * packet=new WorldPacket(1500);
    packet->SetOpcode( SMSG_UPDATE_OBJECT );
    
    *packet << (uint32)1;//number of update/create blocks
    *packet << (uint8)0;//unknown

    *packet << (uint8) UPDATETYPE_VALUES;        // update type == update
    *packet << GetNewGUID();

    uint32 mBlocks = index/32+1;
    *packet << (uint8)mBlocks;
    
    for(uint32 dword_n=mBlocks-1;dword_n;dword_n--)
    *packet <<(uint32)0;

    *packet <<(((uint32)(1))<<(index%32));
    *packet << value;
    
    return packet;
}

void Object::BuildFieldUpdatePacket(Player* Target, uint32 Index, uint32 Value)
{
    ByteBuffer buf(500);
    buf << uint8(UPDATETYPE_VALUES);
    buf << GetNewGUID();

    uint32 mBlocks = Index/32+1;
    buf << (uint8)mBlocks;

    for(uint32 dword_n=mBlocks-1;dword_n;dword_n--)
        buf <<(uint32)0;

    buf <<(((uint32)(1))<<(Index%32));
    buf << Value;

    Target->PushUpdateData(&buf, 1);
}

void Object::BuildFieldUpdatePacket(ByteBuffer * buf, uint32 Index, uint32 Value)
{
    *buf << uint8(UPDATETYPE_VALUES);
    *buf << GetNewGUID();

    uint32 mBlocks = Index/32+1;
    *buf << (uint8)mBlocks;

    for(uint32 dword_n=mBlocks-1;dword_n;dword_n--)
        *buf <<(uint32)0;

    *buf <<(((uint32)(1))<<(Index%32));
    *buf << Value;
}

uint32 Object::BuildValuesUpdateBlockForPlayer(ByteBuffer *data, Player *target)
{
    // returns: update count
    *data << (uint8) UPDATETYPE_VALUES;        // update type == update

    ASSERT(m_wowGuid.GetNewGuidLen());
    *data << m_wowGuid;

    UpdateMask updateMask;
    updateMask.SetCount( m_valuesCount );
    _SetUpdateBits( &updateMask, target );
    _BuildValuesUpdate( data, &updateMask, target );

    // 1 update.
    return 1;
}

void Object::DestroyForPlayer(Player *target) const
{
    ASSERT(target);

    WorldPacket data(SMSG_DESTROY_OBJECT, 8);
    data << GetGUID();

    target->GetSession()->SendPacket( &data );
}


///////////////////////////////////////////////////////////////
/// Build the Movement Data portion of the update packet
/// Fills the data with this object's movement/speed info
/// TODO: rewrite this stuff, document unknown fields and flags
void Object::_BuildMovementUpdate(ByteBuffer * data, uint8 flags, uint32 flags2, Player* target )
{
    ByteBuffer *splinebuf = target->GetAndRemoveSplinePacket(GetGUID());
    *data << (uint8)flags;

    Player * pThis = 0;
    if(m_objectTypeId == TYPEID_PLAYER)
        pThis = static_cast<Player*>(this);

    if (flags & 0x20)
    {
        if(pThis && pThis->m_TransporterGUID != 0)
            flags2 |= 0x200;

        /*if(splinebuf)
        {
            flags2 |= 0x00800001;       //1=move forward
            if(GetTypeId() == TYPEID_UNIT)
            {
                if(static_cast<Unit*>(this)->GetAIInterface()->m_moveRun == false)
                    flags2 |= 0x100;    //100=walk
            }            
        }*/

        if(GetTypeId() == TYPEID_UNIT)
        {
            // burlex: Don't know what this is, but I've only seen it applied to spirit healers.
            //         maybe some sort of invisibility flag? :/

            switch(GetEntry())
            {
            case 6491:  // Spirit Healer
            case 13116: // Alliance Spirit Guide
            case 13117: // Horde Spirit Guide
                {
                    flags2 |= 0x10000000;
                }break;
            }
        
            if(static_cast<Unit*>(this)->GetAIInterface()->IsFlying())
                flags2 |= 0x800;
        }

        *data << (uint32)flags2;
        *data << getMSTime(); // this appears to be time in ms but can be any thing
    }

    if (flags & 0x40)
    {
        if(flags & 0x2)
        {
            *data << (float)0;
            *data << (float)0;
            *data << (float)0;
            *data << (float)m_position.o;
        }
        else
        {
            *data << m_position;
            *data << m_position.o;
        }

        if(flags & 0x20 && flags2 & 0x0200)
        {
            *data << pThis->m_TransporterGUID;
            *data << pThis->m_TransporterX << pThis->m_TransporterY << pThis->m_TransporterZ << pThis->m_TransporterO;
            *data << pThis->m_TransporterUnk;
        }
    }

    if (flags & 0x20)
    {
        *data << (uint32)0;
    }

    if (flags & 0x20 && flags2 & 0x2000)
    {
        *data << (float)0;
        *data << (float)1.0;
        *data << (float)0;
        *data << (float)0;
    }

    if (flags & 0x20)
    {
        *data << m_walkSpeed;     // walk speed
        *data << m_runSpeed;      // run speed
        *data << m_backWalkSpeed; // backwards walk speed
        *data << m_swimSpeed;     // swim speed
        *data << m_backSwimSpeed; // backwards swim speed
        *data << m_flySpeed;        // fly speed
        *data << m_backFlySpeed;    // back fly speed
        *data << m_turnRate;      // turn rate
    }

    if(splinebuf)
    {
        /*data->append(*splinebuf);*/
        delete splinebuf;
    }

    if(flags & 0x8)
    {
        *data << GetGUIDHigh();
    }

    if(flags & 0x2)
    {
        *data << getMSTime();
    }
}


//=======================================================================================
//  Creates an update block with the values of this object as
//  determined by the updateMask.
//=======================================================================================
void Object::_BuildValuesUpdate(ByteBuffer * data, UpdateMask *updateMask, Player* target)
{
    _valueLock.Acquire();
    bool activate_quest_object = false;
    bool reset = false;
    uint32 oldflags;

    if(updateMask->GetBit(OBJECT_FIELD_GUID) && target)       // We're creating.
    {
        Creature * pThis = static_cast<Creature*>(this);
        if(GetTypeId() == TYPEID_UNIT && pThis->Tagged && (pThis->loot.gold || pThis->loot.items.size()))
        {
            // Let's see if we're the tagger or not.
            oldflags = m_uint32Values[UNIT_DYNAMIC_FLAGS];
            uint32 Flags = m_uint32Values[UNIT_DYNAMIC_FLAGS];
            uint32 oldFlags;

            if(pThis->TaggerGuid == target->GetGUID())
            {
                // Our target is our tagger.
                oldFlags = U_DYN_FLAG_TAGGED_BY_OTHER;

                if(Flags & U_DYN_FLAG_TAGGED_BY_OTHER)
                    Flags &= ~oldFlags;

                if(!(Flags & U_DYN_FLAG_LOOTABLE))
                    Flags |= U_DYN_FLAG_LOOTABLE;
            }
            else
            {
                // Target is not the tagger.
                oldFlags = U_DYN_FLAG_LOOTABLE;

                if(!(Flags & U_DYN_FLAG_TAGGED_BY_OTHER))
                    Flags |= U_DYN_FLAG_TAGGED_BY_OTHER;

                if(Flags & U_DYN_FLAG_LOOTABLE)
                    Flags &= ~oldFlags;
            }

            m_uint32Values[UNIT_DYNAMIC_FLAGS] = Flags;
            if(updateMask->GetBit(UNIT_DYNAMIC_FLAGS) == false)
                updateMask->SetBit(UNIT_DYNAMIC_FLAGS);
            
            reset = true;
        }

        if(target && GetTypeId() == TYPEID_GAMEOBJECT)
        {
            GameObject *go = ((GameObject*)this);
            QuestLogEntry *qle;
            GameObjectInfo *info;
            if(go->HasQuests()) {
                activate_quest_object = true;
            } else {
                info = go->GetInfo();
                if(info &&
                    (info->goMap.size() || info->itemMap.size()) )
                {
                    for(GameObjectGOMap::iterator itr = go->GetInfo()->goMap.begin();
                        itr != go->GetInfo()->goMap.end();
                        ++itr)
                    {
                        if(qle = target->GetQuestLogForEntry(itr->first->id))
                        {
                            for(int i = 0; i < qle->GetQuest()->count_required_mob; ++i)
                            {
                                if(qle->GetQuest()->required_mob[i] == go->GetEntry() &&
                                    qle->GetMobCount(i) < qle->GetQuest()->required_mobcount[i])
                                {
                                    activate_quest_object = true;
                                    break;
                                }
                            }
                            if(activate_quest_object)
                                break;
                        }
                    }

                    if(!activate_quest_object)
                    {
                        for(GameObjectItemMap::iterator itr = go->GetInfo()->itemMap.begin();
                            itr != go->GetInfo()->itemMap.end();
                            ++itr)
                        {
                            for(std::map<uint32, uint32>::iterator it2 = itr->second.begin();
                                it2 != itr->second.end(); 
                                ++it2)
                            {
                                if(qle = target->GetQuestLogForEntry(itr->first->id))
                                {
                                    if(target->GetItemInterface()->GetItemCount(it2->first) < it2->second)
                                    {
                                        activate_quest_object = true;
                                        break;
                                    }
                                }
                            }
                            if(activate_quest_object)
                                break;
                        }
                    }
                }
            }
        }
    }


    if(activate_quest_object)
    {
        oldflags = m_uint32Values[GAMEOBJECT_DYN_FLAGS];
        if(!updateMask->GetBit(GAMEOBJECT_DYN_FLAGS))
            updateMask->SetBit(GAMEOBJECT_DYN_FLAGS);
        m_uint32Values[GAMEOBJECT_DYN_FLAGS] = 1;
        reset = true;
    }

    WPAssert(updateMask && updateMask->GetCount() == m_valuesCount);
    uint32 bc;
    uint32 values_count;
    if(m_valuesCount>(2*0x20))//if number of blocks > 2->  unit and player+item container
    {
        bc=updateMask->GetUpdateBlockCount();
        values_count=min(bc*32,m_valuesCount);

    }else
    {
        bc=updateMask->GetBlockCount();
        values_count=m_valuesCount;
    }

    *data << (uint8)bc;
    data->append( updateMask->GetMask(), bc*4 );
      
    for( uint32 index = 0; index < values_count; index ++ )
    {
        if( updateMask->GetBit( index ) )
            *data << m_uint32Values[ index ];
    }

    if(reset)
    {
        switch(GetTypeId())
        {
        case TYPEID_UNIT:
            m_uint32Values[UNIT_DYNAMIC_FLAGS] = oldflags;
            break;
        case TYPEID_GAMEOBJECT:
            m_uint32Values[GAMEOBJECT_DYN_FLAGS] = oldflags;
            break;
        }
    }

    _valueLock.Release();
}

void Object::BuildHeartBeatMsg(WorldPacket *data) const
{
    data->Initialize(MSG_MOVE_HEARTBEAT);

    *data << GetGUID();

    *data << uint32(0); // flags
    *data << uint32(0); // mysterious value #1

    *data << m_position;
    *data << m_position.o;
}

WorldPacket * Object::BuildTeleportAckMsg(const LocationVector & v)
{
    ///////////////////////////////////////
    //Update player on the client with TELEPORT_ACK
    ((Player*)(this))->SetPlayerStatus(TRANSFER_PENDING);

    WorldPacket * data = new WorldPacket(MSG_MOVE_TELEPORT_ACK, 50);
    *data << GetNewGUID();

    //First 4 bytes = no idea what it is
    *data << uint32(5); // flags
    *data << uint32(0x010); // mysterious value #1

    *data << float(0);
    *data << v;
    *data << v.o;
    *data << uint32(0x0);
    return data;
}

bool Object::SetPosition(const LocationVector & v, bool allowPorting /* = false */)
{
    bool updateMap = false, result = true;

    if (m_position.x != v.x || m_position.y != v.y)
        updateMap = true;

    m_position = const_cast<LocationVector&>(v);

    if (!allowPorting && v.z < m_minZ)
    {
        m_position.z = 500;
        sLog.outError( "setPosition: fell through map; height ported" );

        result = false;
    }

    if (IsInWorld() && updateMap)
    {
        m_mapMgr->ChangeObjectLocation(this);
    }

    return result;
}

bool Object::SetPosition( float newX, float newY, float newZ, float newOrientation, bool allowPorting )
{
    bool updateMap = false, result = true;

    if (m_position.x != newX || m_position.y != newY)
        updateMap = true;

    m_position.ChangeCoords(newX, newY, newZ, newOrientation);

    if (!allowPorting && newZ < m_minZ)
    {
        m_position.z = 500;
        sLog.outError( "setPosition: fell through map; height ported" );

        result = false;
    }

    if (IsInWorld() && updateMap)
    {
        m_mapMgr->ChangeObjectLocation(this);
    }

    return result;
}

void Object::SetRotation( uint64 guid )
{
    WorldPacket data(SMSG_AI_REACTION, 12);
    data << guid;
    data << uint32(2);
    SendMessageToSet(&data, false);
}

inline void Object::SendMessageToSet(WorldPacket *data, bool bToSelf)
{
    if(!IsInWorld())
        return;

    if(bToSelf && m_objectTypeId == TYPEID_PLAYER)
    {
        static_cast<Player*>(this)->GetSession()->SendPacket(data);        
    }

    std::set<Player*>::iterator itr = m_inRangePlayers.begin();
    std::set<Player*>::iterator it_end = m_inRangePlayers.end();
    int gm = (m_objectTypeId == TYPEID_PLAYER ? ((Player*)this)->m_isGmInvisible : 0);
    for(; itr != it_end; ++itr)
    {
        ASSERT((*itr)->GetSession());
        if(gm)
        {
            if((*itr)->GetSession()->GetPermissionCount() > 0)
                (*itr)->GetSession()->SendPacket(data);
        } else {
            (*itr)->GetSession()->SendPacket(data);
        }
    }

    /*std::set<Object*>::iterator itr = m_objectsInRange.begin();
    std::set<Object*>::iterator it_end = m_objectsInRange.end();
    for(; itr != it_end; ++itr)
    {
        if((*itr)->GetTypeId() == TYPEID_PLAYER)
        {
            static_cast<Player*>(*itr)->GetSession()->SendPacket(data);
        }
    }*/
}

inline void Object::SendMessageToGms(WorldPacket *data, bool bToSelf)
{

}

inline void Object::SendMessageToNonGms(WorldPacket *data, bool bToSelf)
{
    // BURLEX NOTE : FIX ME!
}

////////////////////////////////////////////////////////////////////////////
/// Fill the object's Update Values from a space deliminated list of values.
void Object::LoadValues(const char* data)
{
    if(!m_uint32Values || !m_baseUint32Values) _InitValues();

    /*char *val;
    int index;
    char *ndata = new char[strlen(data) + 1];
    strcpy(ndata,data);

    val = strtok(ndata," ");

    for (index = 0; index < m_valuesCount && val != NULL; index ++)
    {
        if(m_uint32Values[index] == 0)
            m_uint32Values[index] = atol(val);
        if(m_baseUint32Values[index] == 0)
            m_baseUint32Values[index] = atol(val);
        val = strtok(NULL," ");
    }

    delete [] ndata;*/

    // thread-safe ;) strtok is not.
    std::string ndata = data;
    std::string::size_type last_pos = 0, pos = 0;
    int index = 0;
    uint32 val;
    do 
    {
        // prevent overflow
        if(index >= m_valuesCount)
        {
            //sLog.outError("More fields than m_valuesCount when trying to load.");
            break;
        }
        pos = ndata.find(" ", last_pos);
        val = atol(ndata.substr(last_pos, (pos-last_pos)).c_str());
        if(m_uint32Values[index] == 0)
            m_uint32Values[index] = val;
        if(m_baseUint32Values[index] == 0)
            m_baseUint32Values[index] = val;
        last_pos = pos+1;
        ++index;
    } while(pos != std::string::npos);
}

void Object::_SetUpdateBits(UpdateMask *updateMask, Player *target) const
{
    *updateMask = m_updateMask;
}


void Object::_SetCreateBits(UpdateMask *updateMask, Player *target) const
{
    for( uint16 index = 0; index < m_valuesCount; index++ )
    {
        if(GetUInt32Value(index) != 0)
            updateMask->SetBit(index);
    }
}

void Object::AddToWorld()
{
    ASSERT(!m_inWorld);

    _PlaceOnMap();

    // instance add failed
    if(!m_mapMgr)
        return;

    m_inWorld = true;

    event_Relocate();
}

void Object::RemoveFromWorld()
{
    uint32 oldInstance = m_mapMgr->GetInstanceID();
    ASSERT(m_inWorld);
    
    if(_Activated)
    {
        switch(UINT32_LOPART(GetGUIDHigh()))
        {
        case HIGHGUID_GAMEOBJECT:
            if(m_mapMgr->GetUpdaterThread())
                m_mapMgr->GetUpdaterThread()->RemoveObject<GameObject>(static_cast<GameObject*>(this));
            break;
        case HIGHGUID_UNIT:
            if(m_mapMgr->GetUpdaterThread())
                m_mapMgr->GetUpdaterThread()->RemoveObject<Creature>(static_cast<Creature*>(this));
            break;
        }
        _Activated = false;
    }

    m_inWorld = false;
    _RemoveFromMap();
    
    // update our event holder
    event_Relocate();
}

void Object::_PlaceOnMap()
{
    ASSERT(!m_mapMgr);

    MapMgr *mapMgr = sWorldCreator.GetInstance(m_mapId, this);
    if(!mapMgr)
        return; //instance add failed

    m_mapMgr = mapMgr;
    mapMgr->AddObject(this);

    // correct incorrect instance id's
    if(m_mapMgr->GetInstanceID() != m_instanceId)
        m_instanceId = m_mapMgr->GetInstanceID();

    mSemaphoreTeleport = false;
}


void Object::_RemoveFromMap()
{
    ASSERT(m_mapMgr);

    mSemaphoreTeleport = true;

    m_mapMgr->RemoveObject(this);
    m_mapMgr = 0;
    m_mapCell = 0;

    //sWorld.AddGlobalObject(this);
}


//! Set uint32 property
void Object::SetUInt32Value( const uint32 index, const uint32 &value )
{
    ASSERT( index < m_valuesCount );
    // save updating when val isn't changing.
    if(m_uint32Values[index] == value)
        return;

    _valueLock.Acquire();
    m_uint32Values[ index ] = value;
    _valueLock.Release();

    if(m_inWorld)
    {
        m_updateMask.SetBit( index );

        if(!m_objectUpdated)
        {
            m_mapMgr->ObjectUpdated(this);
            m_objectUpdated = true;
        }
    }
}
//must be in %
void Object::ModPUInt32Value(const uint32 index, const int32 value, bool apply )
{
    ASSERT( index < m_valuesCount );
    _valueLock.Acquire();
    int32 basevalue = (int32)m_uint32Values[ index ];
    if(apply)
        m_uint32Values[ index ] += ((basevalue*value)/100);
    else
        m_uint32Values[ index ] = (basevalue*100)/(100+value);
    _valueLock.Release();

    if(m_inWorld)
    {
        m_updateMask.SetBit( index );

        if(!m_objectUpdated )
        {
            m_mapMgr->ObjectUpdated(this);
            m_objectUpdated = true;
        }
    }
}

void Object::ModUInt32Value(const uint32 index, const int32 value )
{
    ASSERT( index < m_valuesCount );
    if(value == 0)
        return;

    _valueLock.Acquire();
    m_uint32Values[ index ] += value;
    _valueLock.Release();

    if(m_inWorld)
    {
        m_updateMask.SetBit( index );

        if(!m_objectUpdated)
        {
            m_mapMgr->ObjectUpdated(this);
            m_objectUpdated = true;
        }
    }
}

void Object::ModPFloatValue(const uint32 index, const float value, bool apply)
{
    ASSERT( index < m_valuesCount );
    _valueLock.Acquire();
    float basevalue = m_floatValues[ index ];
    if(apply)
        m_floatValues[ index ] += ((basevalue*value)/100);
    else
        m_floatValues[ index ] = (basevalue*100)/(100+value);
    _valueLock.Release();

    if(m_inWorld)
    {
        m_updateMask.SetBit( index );

        if(!m_objectUpdated )
        {
            m_mapMgr->ObjectUpdated(this);
            m_objectUpdated = true;
        }
    }
}

void Object::ModFloatValue(const uint32 index, const float value )
{
    ASSERT( index < m_valuesCount );
    _valueLock.Acquire();
    m_floatValues[ index ] += value;
    _valueLock.Release();

    if(m_inWorld)
    {
        m_updateMask.SetBit( index );

        if(!m_objectUpdated)
        {
            m_mapMgr->ObjectUpdated(this);
            m_objectUpdated = true;
        }
    }
}
//! Set uint64 property
void Object::SetUInt64Value( const uint32 index, const uint64 &value )
{
    ASSERT( index + 1 < m_valuesCount );
    if(m_uint32Values[index] == GUID_LOPART(value) && m_uint32Values[index+1] == GUID_HIPART(value))
        return;

    _valueLock.Acquire();
    m_uint32Values[ index ] = *((uint32*)&value);
    m_uint32Values[ index + 1 ] = *(((uint32*)&value) + 1);
    _valueLock.Release();

    if(m_inWorld)
    {
        m_updateMask.SetBit( index );
        m_updateMask.SetBit( index + 1 );

        if(!m_objectUpdated)
        {
            m_mapMgr->ObjectUpdated(this);
            m_objectUpdated = true;
        }
    }
}

//! Set float property
void Object::SetFloatValue( const uint32 index, const float &value )
{
    ASSERT( index < m_valuesCount );
    if(m_floatValues[index] == value)
        return;

    _valueLock.Acquire();
    m_floatValues[ index ] = value;
    _valueLock.Release();

    if(m_inWorld)
    {
        m_updateMask.SetBit( index );

        if(!m_objectUpdated)
        {
            m_mapMgr->ObjectUpdated(this);
            m_objectUpdated = true;
        }
    }
}


void Object::SetFlag( const uint32 index, uint32 newFlag )
{
    ASSERT( index < m_valuesCount );
    _valueLock.Acquire();
    m_uint32Values[ index ] |= newFlag;
    _valueLock.Release();

    if(m_inWorld)
    {
        m_updateMask.SetBit( index );

        if(!m_objectUpdated)
        {
            m_mapMgr->ObjectUpdated(this);
            m_objectUpdated = true;
        }
    }
}


void Object::RemoveFlag( const uint32 index, uint32 oldFlag )
{
    ASSERT( index < m_valuesCount );
    _valueLock.Acquire();
    m_uint32Values[ index ] &= ~oldFlag;
    _valueLock.Release();

    if(m_inWorld)
    {
        m_updateMask.SetBit( index );

        if(!m_objectUpdated)
        {
            m_mapMgr->ObjectUpdated(this);
            m_objectUpdated = true;
        }
    }
}

////////////////////////////////////////////////////////////

float Object::CalcDistance(Object *Ob)
{
    return CalcDistance(this->GetPositionX(), this->GetPositionY(), this->GetPositionZ(), Ob->GetPositionX(), Ob->GetPositionY(), Ob->GetPositionZ());
}
float Object::CalcDistance(float ObX, float ObY, float ObZ)
{
    return CalcDistance(this->GetPositionX(), this->GetPositionY(), this->GetPositionZ(), ObX, ObY, ObZ);
}
float Object::CalcDistance(Object *Oa, Object *Ob)
{
    return CalcDistance(Oa->GetPositionX(), Oa->GetPositionY(), Oa->GetPositionZ(), Ob->GetPositionX(), Ob->GetPositionY(), Ob->GetPositionZ());
}
float Object::CalcDistance(Object *Oa, float ObX, float ObY, float ObZ)
{
    return CalcDistance(Oa->GetPositionX(), Oa->GetPositionY(), Oa->GetPositionZ(), ObX, ObY, ObZ);
}

float Object::CalcDistance(float OaX, float OaY, float OaZ, float ObX, float ObY, float ObZ)
{
    float xdest = OaX - ObX;
    float ydest = OaY - ObY;
    float zdest = OaZ - ObZ;
    return sqrtf(zdest*zdest + ydest*ydest + xdest*xdest);
}

float Object::calcAngle( float Position1X, float Position1Y, float Position2X, float Position2Y )
{
    float dx = Position2X-Position1X;
    float dy = Position2Y-Position1Y;
    float angle=0.0f;

    // Calculate angle
    if (dx == 0.0)
    {
        if (dy == 0.0)
            angle = 0.0;
        else if (dy > 0.0)
            angle = M_PI * 0.5 /* / 2 */;
        else
            angle = M_PI * 3.0 * 0.5/* / 2 */;
    }
    else if (dy == 0.0)
    {
        if (dx > 0.0)
            angle = 0.0;
        else
            angle = M_PI;
    }
    else
    {
        if (dx < 0.0)
            angle = atanf(dy/dx) + M_PI;
        else if (dy < 0.0)
            angle = atanf(dy/dx) + (2*M_PI);
        else
            angle = atanf(dy/dx);
    }

    // Convert to degrees
    angle = angle * 180 / M_PI;

    // Return
    return angle;
}

float Object::calcRadAngle( float Position1X, float Position1Y, float Position2X, float Position2Y )
{
    float dx = Position2X-Position1X;
    float dy = Position2Y-Position1Y;
    float angle=0.0f;

    // Calculate angle
    if (dx == 0.0)
    {
        if (dy == 0.0)
            angle = 0.0;
        else if (dy > 0.0)
            angle = M_PI * 0.5/*/ 2.0*/;
        else
            angle = M_PI * 3.0 * 0.5/*/ 2.0*/;
    }
    else if (dy == 0.0)
    {
        if (dx > 0.0)
            angle = 0.0;
        else
            angle = M_PI;
    }
    else
    {
        if (dx < 0.0)
            angle = atanf(dy/dx) + M_PI;
        else if (dy < 0.0)
            angle = atanf(dy/dx) + (2*M_PI);
        else
            angle = atanf(dy/dx);
    }

    // Return
    return angle;
}

float Object::getEasyAngle( float angle )
{
    while ( angle < 0 ) {
        angle = angle + 360;
    }
    while ( angle >= 360 ) {
        angle = angle - 360;
    }
    return angle;
}

bool Object::inArc(float Position1X, float Position1Y, float FOV, float Orientation, float Position2X, float Position2Y )
{
    float angle = calcAngle( Position1X, Position1Y, Position2X, Position2Y );
    float lborder = getEasyAngle( ( Orientation - (FOV*0.5/*/2*/) ) );
    float rborder = getEasyAngle( ( Orientation + (FOV*0.5/*/2*/) ) );
    //sLog.outDebug("Orientation: %f Angle: %f LeftBorder: %f RightBorder %f",Orientation,angle,lborder,rborder);
    if(((angle >= lborder) && (angle <= rborder)) || ((lborder > rborder) && ((angle < rborder) || (angle > lborder))))
    {
        return true;
    }
    else
    {
        return false;
    }
} 

bool Object::isInFront(Object* target)
{
    float m_orientation = m_position.o;

    while(m_orientation<0)m_orientation+=2*M_PI;
    while(m_orientation>2*M_PI)m_orientation-=2*M_PI;
    m_position.o = m_orientation;

    float dx=target->GetPositionX()-this->GetPositionX();
    float dy=target->GetPositionY()-this->GetPositionY();
    
    float d=m_orientation;
    
    if(dy>=0.0)
    {
        d-=atan(dy/dx);

        if(dx<0.0)
        d-=M_PI;
    }else
    {
        
        d-=3*M_PI/2 - atan(dx/dy);
        
    }
    if(d<-M_PI/2 || d > M_PI/2)
        return false;
    else return true;

}


bool Object::isInBack(Object* target)
{
    float dx=target->GetPositionX()-this->GetPositionX();
    float dy=target->GetPositionY()-this->GetPositionY();
    
    float d=target->GetOrientation();
    while(d<0)d+=2*M_PI;
    while(d>2*M_PI)d-=2*M_PI;
    
    if(dy>=0.0)
    {
        d-=atan(dy/dx);

        if(dx<0.0)
        d-=M_PI;
    }else
    {
        
        d-=3*M_PI/2 - atan(dx/dy);
        
    }
    if(d<-M_PI/4 || d > M_PI/4)
        return false;
    else return true;

}



bool Object::isInRange(Object* target, float range)
{
    float dist = CalcDistance(target);
    return (dist <= range);
}

bool Object::IsPet()
{
    if(this->GetTypeId()!=TYPEID_UNIT)
        return false;

    if(((Unit*)this)->m_isPet && m_uint32Values[UNIT_FIELD_CREATEDBY] != 0 && m_uint32Values[UNIT_FIELD_SUMMONEDBY] != 0)
        return true;

    return false;
}

void Object::_setFaction()
{
    FactionTemplateDBC* factT = NULL;

    if(GetTypeId() == TYPEID_UNIT || GetTypeId() == TYPEID_PLAYER)
    {
        factT = sFactionTmpStore.LookupEntry(GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
    }
    
    if(GetTypeId() == TYPEID_GAMEOBJECT)
    {
        factT = sFactionTmpStore.LookupEntry(GetUInt32Value(GAMEOBJECT_FACTION));
    }

    if(!factT)
    {
        return;
    }
    m_faction = factT;
    m_factionDBC = sFactionStore.LookupEntry(factT->FactionId);
}

void Object::UpdateOppFactionSet()
{
    m_oppFactsInRange.clear();
    for(Object::InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); ++i)
    {
        if (((*i)->GetTypeId() == TYPEID_UNIT) || ((*i)->GetTypeId() == TYPEID_PLAYER) || ((*i)->GetTypeId() == TYPEID_GAMEOBJECT))
        {
            if (isHostile(this, (*i)))
            {
                if(!(*i)->IsInRangeOppFactSet(this))
                    (*i)->m_oppFactsInRange.insert(this);
                if (!IsInRangeOppFactSet((*i)))
                    m_oppFactsInRange.insert((*i));
                
            }
            else
            {
                if((*i)->IsInRangeOppFactSet(this))
                    (*i)->m_oppFactsInRange.erase(this);
                if (IsInRangeOppFactSet((*i)))
                    m_oppFactsInRange.erase((*i));
            }
        }
    }
}

void Object::DealDamage(Unit *pVictim, uint32 damage, uint32 targetEvent, uint32 unitEvent, uint32 spellId, bool no_remove_auras)
{
    if(!pVictim || !pVictim->isAlive())
        return;

    if(pVictim->GetTypeId() == TYPEID_PLAYER && static_cast<Player*>(pVictim)->GodModeCheat == true)
        return;
    if(pVictim->IsSpiritHealer())
        return;
    
    if( pVictim->GetStandState())//not standing-> standup
    {
        pVictim->SetStandState(STANDSTATE_STAND);//probably mobs also must standup
    }

    // This one is easy. If we're attacking a hostile target, and we're not flagged, flag us.
    if(pVictim->IsPlayer() && IsPlayer())
    {
        if( isHostile( this, pVictim ) )
            ((Player*)this)->SetPvPFlag();
    }

    if(!no_remove_auras)
        pVictim->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN);

    if(this->IsUnit())
    {
        if(pVictim->GetTypeId()==TYPEID_UNIT && IsPlayer()) // Units can't tag..
        {
            // Tagging
            Creature *victim = static_cast<Creature*>(pVictim);
            bool taggable = true;
            if(victim->GetCreatureName() && victim->GetCreatureName()->Type == CRITTER)
                taggable = false;

            if(!victim->Tagged && taggable)
            {
                victim->Tagged = true;
                victim->TaggerGuid = GetGUID();

                // For new players who get a create object
                uint32 Flags = pVictim->m_uint32Values[UNIT_DYNAMIC_FLAGS];
                Flags |= U_DYN_FLAG_TAPPED_BY_PLAYER;

                pVictim->m_uint32Values[UNIT_DYNAMIC_FLAGS] |= U_DYN_FLAG_TAGGED_BY_OTHER;

                // Update existing players.
                ByteBuffer buf(500);
                ByteBuffer buf1(500);

                pVictim->BuildFieldUpdatePacket(&buf1, UNIT_DYNAMIC_FLAGS, Flags);
                pVictim->BuildFieldUpdatePacket(&buf, UNIT_DYNAMIC_FLAGS, pVictim->m_uint32Values[UNIT_DYNAMIC_FLAGS]);

                // Loop inrange set, append to their update data.
                for(std::set<Player*>::iterator itr = m_inRangePlayers.begin(); itr != m_inRangePlayers.end(); ++itr)
                {
                    if (static_cast<Player*>(this)->InGroup())
                    {
                        if (static_cast<Player*>(*itr)->GetGroup() && static_cast<Player*>(this)->GetGroup()->GetID() == static_cast<Player*>(*itr)->GetGroup()->GetID())
                        {
                            (*itr)->PushUpdateData(&buf1, 1);
                        } 
                        else
                        {
                            (*itr)->PushUpdateData(&buf, 1);
                        }
                        
                    } 
                    else
                    {
                        (*itr)->PushUpdateData(&buf, 1);
                    }
                }

                // Update ourselves
                static_cast<Player*>(this)->PushUpdateData(&buf1, 1);

            }
        }        

        if(pVictim->IsUnit())
        {
            // Set our attack target to the victim.
            static_cast<Unit*>(this)->setAttackTarget(pVictim);
        }
    }

    // Awake sleeping Creatures.
    if( damage && pVictim->m_sleep)
    {
        pVictim->RemoveAura(pVictim->m_sleep);
    }
    // break entangling roots
/*    if( damage && pVictim->m_rooted)
    {
        if(Rand(35.0f)) // 35% chance to break the roots
            pVictim->RemoveAura(pVictim->m_rooted);
    }
*/    
    ///Rage
    uint32 val;

    if(pVictim->GetPowerType() == POWER_TYPE_RAGE && !spellId && pVictim != this)  // burlex fix: stop giving rage on spell dmg, and falling
    {    
        val = pVictim->GetUInt32Value(UNIT_FIELD_POWER2)+(damage*20)/(pVictim->getLevel()*3);
        pVictim->SetUInt32Value(UNIT_FIELD_POWER2, val>=1000?1000:val);
    }
    //

    if(pVictim->IsPlayer())
    {
        Player *pThis = static_cast<Player *>(pVictim);
        /*if(pVictim->isInCombat() || static_cast<Unit *>(this) != pVictim)//You cant get in combat by hitting yourself
        {
            pThis->CombatModeDelay = COMBAT_DECAY_TIME;
        }*/
        if(pThis->cannibalize)
        {
            sEventMgr.RemoveEvents(pVictim, EVENT_CANNIBALIZE);
            pThis->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
            pThis->cannibalize = false;
        }
    }
    /*else if(pVictim->IsPet())
    {
        Player*p = ((Pet *)(pVictim))->GetPetOwner();
        if(p)
            p->CombatModeDelay = COMBAT_DECAY_TIME;
    }*/

    //* BATTLEGROUND DAMAGE COUNTER *//
    if(IsPlayer() && ((Player*)this)->m_bgScore != 0 && pVictim != this)
        ((Player*)this)->m_bgScore->DamageDone += damage;
   
    //Log::getSingleton( ).outError("DealDamageStart");
    uint32 health = pVictim->GetUInt32Value(UNIT_FIELD_HEALTH );
    //uint32 maxhealth = pVictim->GetUInt32Value(UNIT_FIELD_MAXHEALTH );

    /*------------------------------------ DUEL HANDLERS --------------------------*/
    if((pVictim->IsPlayer()) && (this->IsPlayer()) && static_cast<Player*>(pVictim)->DuelingWith == static_cast<Player*>(this) ) //Both Players
    {
        if((health <= damage) && static_cast<Player*>(this)->DuelingWith != NULL)
        {
            //Player in Duel and Player Victim has lost
            uint32 NewHP = pVictim->GetUInt32Value(UNIT_FIELD_MAXHEALTH)/100;

            if(NewHP < 5) 
                NewHP = 5;

            //Set there health to 1% or 5 if 1% is lower then 5
            pVictim->SetUInt32Value(UNIT_FIELD_HEALTH, NewHP);
            //End Duel
            static_cast<Player*>(this)->EndDuel(DUEL_WINNER_KNOCKOUT);

            return;          
        }
    }

    if((pVictim->IsPlayer()) && (IsPet())) 
    {
        if((health <= damage) && static_cast<Player*>(pVictim)->DuelingWith == static_cast<Pet*>(this)->GetPetOwner())
        {
            Player *petOwner = static_cast<Pet*>(this)->GetPetOwner();
            if(petOwner)
            {
                //Player in Duel and Player Victim has lost
                uint32 NewHP = pVictim->GetUInt32Value(UNIT_FIELD_MAXHEALTH)/100;
                if(NewHP < 5) NewHP = 5;
                
                //Set there health to 1% or 5 if 1% is lower then 5
                pVictim->SetUInt32Value(UNIT_FIELD_HEALTH, NewHP);
                //End Duel
                petOwner->EndDuel(DUEL_WINNER_KNOCKOUT);
                return;
            }
        }
    }
    /*------------------------------------ DUEL HANDLERS END--------------------------*/

    //if ( (pVictim->GetCreatureName()->Type == CRITTER || health <= damage) && pVictim->isAlive())
    bool isCritter = false;
    if(pVictim->GetTypeId() == TYPEID_UNIT)
    {
        if(pVictim->GetCreatureName())
            if(pVictim->GetCreatureName()->Type == CRITTER)
                isCritter = true;
    }
    /* -------------------------- HIT THAT CAUSES VICTIM TO DIE ---------------------------*/
    if ((isCritter || health <= damage) )
    {
        /* victim died! */
        if(pVictim->IsPlayer())
            ((Player*)pVictim)->KillPlayer();
        else
            pVictim->setDeathState(JUST_DIED);

        if(pVictim->IsPlayer() && !IsPlayer())
        {
            ((Player*)pVictim)->DeathDurabilityLoss(0.10);
        }

        if(pVictim->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT) > 0)
        {
            if(pVictim->GetCurrentSpell())
            {
                Spell *spl = pVictim->GetCurrentSpell();
                for(int i = 0; i < 3; i++)
                {
                    if(spl->m_spellInfo->Effect[i] == SPELL_EFFECT_PERSISTENT_AREA_AURA)
                    {
                        DynamicObject *dObj = sObjHolder.GetObject<DynamicObject>(pVictim->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT));
                        if(!dObj)
                            return;
                        WorldPacket data(SMSG_GAMEOBJECT_DESPAWN_ANIM, 8);
                        data << dObj->GetGUID();
                        dObj->SendMessageToSet(&data, false);
                        if(dObj->IsInWorld())
                        {
                            dObj->RemoveFromWorld();
                        }
                        sObjHolder.Delete(dObj);
                    }
                }
                if(spl->m_spellInfo->ChannelInterruptFlags == 48140) spl->cancel();
            }
        }
        
        /* Remove all Auras */
        pVictim->DropAurasOnDeath();

        /* Stop victim from attacking */
        pVictim->smsg_AttackStop(((Unit*)this));
        if(pVictim->GetTypeId() == TYPEID_PLAYER)
            ((Player*)pVictim)->EventAttackStop();

        /* Set victim health to 0 */
        pVictim->SetUInt32Value(UNIT_FIELD_HEALTH, 0);
        if(pVictim->IsPlayer())
            if(((Player*)pVictim)->SoulStone)
                pVictim->SetUInt32Value(PLAYER_SELF_RES_SPELL,((Player*)pVictim)->SoulStone);

        // Wipe our attacker set on death
        pVictim->clearAttackers(true);

        // Captnoord 
        // TODO: Check how this works on official
        // if unit is player and player is in group and just killed a creature
        // send log packet

        // burlex: sent to set. don't send it to the party, becuase if they're out of
        //         range they won't know this guid exists -> possible 132.

        /*if (this->IsPlayer())
            if (((Player*)this)->InGroup())
                ((Player*)this)->GetGroup()->SendPartyKillLog(this,pVictim);*/

        /* Stop Unit from attacking */
        if(this->IsPlayer())
            ((Player*)this)->EventAttackStop();
       
        if(this->IsUnit())
        {
            ((Unit*)this)->smsg_AttackStop(pVictim);
        
            /* Tell Unit that it's target has Died */
            ((Unit*)this)->addStateFlag(UF_TARGET_DIED);

            // We will no longer be attacking this target, as it's dead.
            static_cast<Unit*>(this)->setAttackTarget(NULL);
        }
        /* -------------------------------- HONOR + BATTLEGROUND CHECKS ------------------------ */
        if(this->IsPlayer())        // Honor System Checks
        {
            if(pVictim->IsPlayer() && ((Player*)pVictim)->GetCurrentBattleground() != NULL && ((Player*)this)->GetCurrentBattleground() != NULL)
            {
                if(((Player*)pVictim)->m_bgInBattleground)
                {                    
                    if(!pVictim->HasFlag(UNIT_FIELD_FLAGS,U_FIELD_FLAG_SKINNABLE))
                        pVictim->SetFlag(UNIT_FIELD_FLAGS,U_FIELD_FLAG_SKINNABLE);

                    // set loot
                    pVictim->loot.gold = rand() % 150 + 50;
                }

                if(((Player*)pVictim)->GetCurrentBattleground()->GetMapId() == 489)
                    ((Player*)pVictim)->GetCurrentBattleground()->HandleBattlegroundEvent(this, pVictim, BGEVENT_WSG_PLAYER_KILL);
                else if(((Player*)pVictim)->GetCurrentBattleground()->GetMapId() == 529)
                    ((Player*)pVictim)->GetCurrentBattleground()->HandleBattlegroundEvent(this, pVictim, BGEVENT_AB_PLAYER_KILL);
                else if(((Player*)pVictim)->GetCurrentBattleground()->GetMapId() == 30)
                    ((Player*)pVictim)->GetCurrentBattleground()->HandleBattlegroundEvent(this, pVictim, BGEVENT_AV_PLAYER_KILL);
            }
            else if( pVictim->IsUnit() && ((Player*)this)->GetCurrentBattleground() != NULL )
            {
                if(pVictim->GetCreatureName())
                {
                    if(pVictim->GetCreatureName()->Id == 11946) // Horde General Drek'Thar
                        ((Player*)this)->GetCurrentBattleground()->HandleBattlegroundEvent(this, pVictim, BGEVENT_AV_GENERAL_DEAD_HORDE);
                    else if(pVictim->GetCreatureName()->Id == 11948) // Alliance General Vanndar Stormpike
                        ((Player*)this)->GetCurrentBattleground()->HandleBattlegroundEvent(this, pVictim, BGEVENT_AV_GENERAL_DEAD_ALLIANCE);
                }
            }
            
            if((((Unit*)pVictim)->getLevel() - ((Unit*)this)->getLevel()) >= -8 )
                HonorHandler::OnPlayerKilledUnit(static_cast<Player*>(this), pVictim);
        }
        /* -------------------------------- HONOR + BATTLEGROUND CHECKS END------------------------ */

        uint64 victimGuid = pVictim->GetGUID();

        if(pVictim->GetTypeId() == TYPEID_UNIT)
        {
            pVictim->GetAIInterface()->OnDeath(this);
            if(GetTypeId() == TYPEID_PLAYER)
            {
                WorldPacket data(SMSG_PARTYKILLLOG, 16);
                data << GetGUID() << pVictim->GetGUID();
                SendMessageToSet(&data, true);
            }            

            // it Seems that pets some how dont get a name and cause a crash here
            //bool isCritter = (pVictim->GetCreatureName() != NULL)? pVictim->GetCreatureName()->Type : 0;

            //-----------------------------------LOOOT--------------------------------------------
            if ((!pVictim->IsPet())&& ( !isCritter ))
            {
                Creature * victim = static_cast<Creature*>(pVictim);
                // fill loot vector.
                victim->generateLoot();

                Player *owner = 0;
                if(victim->TaggerGuid)
                    owner = World::GetPlayer(victim->TaggerGuid);

                if(owner == 0)  // no owner
                {
                    // donno why this would happen, but anyway.. anyone can loot ;p
                    // no owner no loot
                    //victim->SetFlag(UNIT_DYNAMIC_FLAGS, U_DYN_FLAG_LOOTABLE);
                }
                else
                {
                    // Build the actual update.
                    ByteBuffer buf(500);

                    uint32 Flags = victim->m_uint32Values[UNIT_DYNAMIC_FLAGS];
                    Flags |= U_DYN_FLAG_LOOTABLE;

                    victim->BuildFieldUpdatePacket(&buf, UNIT_DYNAMIC_FLAGS, Flags);

                    // Check for owner's group.
                    Group * pGroup = owner->GetGroup();
                    if(pGroup != 0)
                    {
                        // Owner was in a party.
                        // Check loot method.
                        switch(pGroup->GetMethod())
                        {
                        case PARTY_LOOT_FFA:
                        case PARTY_LOOT_GROUP:
                        case PARTY_LOOT_RR:
                        case PARTY_LOOT_NBG:
                            {
                                // Loop party players and push update data.
                                GroupMembersSet::iterator itr;
                                SubGroup * sGrp;
                                for(uint32 Index = 0; Index < pGroup->GetSubGroupCount(); ++Index)
                                {
                                    sGrp = pGroup->GetSubGroup(Index);
                                    itr = sGrp->GetGroupMembersBegin();
                                    for(; itr != sGrp->GetGroupMembersEnd(); ++itr)
                                    {
                                        if((*itr)->IsVisible(victim))       // Save updates for non-existant creatures
                                            (*itr)->PushUpdateData(&buf, 1);
                                    }
                                }
                            }break;
                        case PARTY_LOOT_MASTER:
                            {
                                // Master loot: only the loot master gets the update.
                                Player * pLooter = pGroup->GetLooter();
                                if(pLooter == 0)
                                    pLooter = pGroup->GetLeader();

                                if(pLooter->IsVisible(victim))  // Save updates for non-existant creatures
                                    pLooter->PushUpdateData(&buf, 1);

                            }break;
                        }
                    }
                    else
                    {
                        // Owner killed the mob solo.
                        if(owner->IsVisible(victim))
                            owner->PushUpdateData(&buf, 1);
                    }
                }
            }
            //---------------------------------looot-----------------------------------------  
            
            if (GetTypeId() == TYPEID_PLAYER && 
                pVictim->GetUInt64Value(UNIT_FIELD_CREATEDBY) == 0 && 
                pVictim->GetUInt64Value(OBJECT_FIELD_CREATED_BY) == 0 &&
                !pVictim->IsPet())
            {
                sLog.outDebug("Calculate XP");
                // Is this player part of a group
                if(((Player*)this)->InGroup())
                {
                    //Calc Group XP
                    ((Player*)this)->GiveGroupXP(pVictim,(Player*)this);
                }
                else
                {
                    uint32 xp = CalculateXpToGive(pVictim, (Unit*)this);
                    if(xp)
                    {
                        sLog.outDebug("XP: %u",xp);
                        ((Player*)this)->GiveXP(xp, victimGuid, true);
                    }
                }

                if (pVictim->GetTypeId() != TYPEID_PLAYER)
                    sQuestMgr.OnPlayerKill((Player*)this, (Creature*)pVictim);
            }
            else /* is Creature or GameObject*/
            {
                /* ----------------------------- PET XP HANDLING -------------- */
                if(IsPet() && !pVictim->IsPet())
                {
                    Player *petOwner = static_cast<Pet*>(this)->GetPetOwner();
                    if(petOwner && petOwner->GetTypeId() == TYPEID_PLAYER)
                    {
                        if(GetUInt32Value(UNIT_CREATED_BY_SPELL) > 0)
                        {
                            if(petOwner->InGroup())
                            {
                                //Calc Group XP
                                ((Unit*)this)->GiveGroupXP(pVictim,petOwner);
                            }
                            else
                            {
                                uint32 xp = CalculateXpToGive(pVictim, petOwner);
                                sLog.outDebug("Pet XP: %u",xp);
                                petOwner->GiveXP(xp, victimGuid, true);
                            }
                        }
                        else
                        {
                            //FIXME: Tame creature xp
                        }
                        if (pVictim->GetTypeId() != TYPEID_PLAYER && pVictim->GetTypeId() == TYPEID_UNIT)
                            sQuestMgr.OnPlayerKill(petOwner, (Creature*)pVictim);
                    }
                }
                /* ----------------------------- PET XP HANDLING END-------------- */

                /* ----------------------------- PET DEATH HANDLING -------------- */
                if(pVictim->IsPet())
                {
                    static_cast<Pet*>(pVictim)->DelayedRemove(false, true);
                }
                /* ----------------------------- PET DEATH HANDLING END -------------- */
            }
        }
        else if (pVictim->GetTypeId() == TYPEID_PLAYER)
        {
            
            /* -------------------- RESET BREATH STATE ON DEATH -------------- */
            ((Player*)pVictim)->m_UnderwaterTime = 0;
            ((Player*)pVictim)->m_UnderwaterState = 0;
            ((Player*)pVictim)->m_BreathDamageTimer = 0;
            ((Player*)pVictim)->m_SwimmingTime = 0;
            /* -------------------- KILL PET WHEN PLAYER DIES ---------------*/
            if(static_cast<Player*>(pVictim)->GetSummon() != NULL)
            {
                if(pVictim->GetUInt32Value(UNIT_CREATED_BY_SPELL) > 0)
                    static_cast<Player*>(pVictim)->GetSummon()->Dismiss(true);
                else
                    static_cast<Player*>(pVictim)->GetSummon()->Remove(true, true);
            }
            /* -------------------- KILL PET WHEN PLAYER DIES END---------------*/
        }
        else Log::getSingleton( ).outError("DealDamage for Unknown Object.");
    }
    else /* ---------- NOT DEAD YET --------- */
    {
        if(pVictim != this /* && updateskill */)
        {
            // Send AI Reaction UNIT vs UNIT
            if (GetTypeId() ==TYPEID_UNIT)
            {
                ((Unit*)this)->GetAIInterface()->AttackReaction(pVictim, damage, unitEvent + 0xFF, spellId);
            }
            
            // Send AI Victim Reaction
            if(this->IsPlayer() || this->GetTypeId()==TYPEID_UNIT)
            if (pVictim->GetTypeId() != TYPEID_PLAYER)
            {
                ((Creature*)pVictim)->GetAIInterface()->AttackReaction((Unit*)this, damage, targetEvent, spellId);
            }
        }
        
        // TODO: Mark victim as a HK
        if(((Player*)pVictim)->GetCurrentBattleground() != NULL && ((Player*)this)->GetCurrentBattleground() != NULL)
        {
            
        }
        
        // reduce victims HP
        pVictim->SetUInt32Value(UNIT_FIELD_HEALTH , health - damage);
    }
}

void Object::SpellNonMeleeDamageLog(Unit *pVictim, uint32 spellID, uint32 damage, bool allowProc)
{
    if(!pVictim || !pVictim->isAlive())
        return;
   
//    uint32 dmgabs = 0;

    SpellEntry *spellInfo = sSpellStore.LookupEntry( spellID );
    if(!spellInfo)
            return;
    SpellCastTime *sd = sCastTime.LookupEntry(spellInfo->CastingTimeIndex);

    // affect the plus damage by duration
    float castaff = GetCastTime(sd);
    if(castaff > 3500) castaff = 3500;
    else if(castaff < 1500) castaff = 1500;
    float dmgdoneaffectperc = castaff / 3500;
        
   // if(pVictim->HasSpellDamageSplit())
     //   damage = pVictim->SplitDamagePCT(damage, 126);
    uint32 school = spellInfo->School;
    float res = float(damage);
    uint32 aproc = PROC_ON_ANY_HOSTILE_ACTION;
    uint32 vproc = PROC_ON_ANY_HOSTILE_ACTION | PROC_ON_ANY_DAMAGE_VICTIM | PROC_ON_SPELL_HIT_VICTIM;
    bool critical = false;

    if(this->IsUnit())
    {
        static_cast<Unit*>(this)->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_START_ATTACK);
        
        if(IsPlayer())
            res += static_cast<Player*>(this)->SpellDmgDoneByInt[school] * static_cast<Player*>(this)->GetUInt32Value(UNIT_FIELD_STAT3);
        
        int32 bonus_damage = FL2UINT(float( float(static_cast<Unit*>(this)->GetDamageDoneMod(school)) * dmgdoneaffectperc ));
        if(spellInfo->SpellGroupType)
        {
            SM_FIValue(static_cast<Unit*>(this)->SM_FPenalty, &bonus_damage, spellInfo->SpellGroupType);
            res += bonus_damage;

            int32 ures = (int32)res;
            SM_FIValue(static_cast<Unit*>(this)->SM_FDamageBonus, &ures, spellInfo->SpellGroupType);
            SM_PIValue(static_cast<Unit*>(this)->SM_PDamageBonus, &ures, spellInfo->SpellGroupType);
            res = (float)ures;
        }
        else
        {
            res += bonus_damage;
        }


        if(res < 0)
            res = 0;
        else
        {
            res *= static_cast<Unit*>(this)->GetDamageDonePctMod(school) * pVictim->DamageTakenPctMod[school];
            res += float(pVictim->DamageTakenMod[school]);
            if(res < 0) res = 0;		
            float CritChance = static_cast<Unit*>(this)->spellcritperc + static_cast<Unit*>(this)->SpellCritChanceSchool[school];





            if(spellInfo->SpellGroupType)
                SM_FFValue(static_cast<Unit*>(this)->SM_CriticalChance, &CritChance, spellInfo->SpellGroupType);

            critical = Rand(CritChance);

            if (critical) //Spell Crictical Hit
            {		
                int32 dmgbonus = 0;
                if(spellInfo->SpellGroupType)
                    SM_FIValue(static_cast<Unit*>(this)->SM_PCriticalDamage, &dmgbonus, spellInfo->SpellGroupType);

                res *= 1.5f;
                res = (res*(100+dmgbonus))/100;

                pVictim->Emote(EMOTE_ONESHOT_WOUNDCRITICAL);
                vproc |=  PROC_ON_SPELL_CRIT_HIT_VICTIM;
            }
        }
    
        /*if(this->IsPlayer())
        {
            static_cast<Player*>(this)->CombatModeDelay = COMBAT_DECAY_TIME;
        }
        else if(this->IsPet())
        {
            Player *p = ((Pet*)this)->GetPetOwner();
            if(p)
                p->CombatModeDelay = COMBAT_DECAY_TIME;
        }*/
    }
	uint32 ress=(uint32)res;
    uint32 abs_dmg = pVictim->AbsorbDamage(school, &ress);
    res=ress;
    dealdamage dmg;
    dmg.damage_type = school;
    dmg.full_damage = res;
    dmg.resisted_damage = 0;
    // make it say resisted :p
    if(res == 0)
        dmg.resisted_damage = dmg.full_damage = 1;
    
    if(this->IsUnit())
    {
        static_cast<Unit*>(this)->CalculateResistanceReduction(pVictim,&dmg);
        res = dmg.full_damage - dmg.resisted_damage;
    }

    if(pVictim->GetTypeId() == TYPEID_PLAYER && static_cast<Player*>(pVictim)->GodModeCheat == true)
    {
        res = dmg.full_damage;
        dmg.resisted_damage = dmg.full_damage;
    }
    
    // Send spell log
    SendSpellNonMeleeDamageLog(this, pVictim, spellID, res, school, abs_dmg, dmg.resisted_damage, false, 0, critical, IsPlayer());
    
    if(this->IsUnit() && allowProc)
    {
        static_cast<Unit*>(this)->HandleProc(aproc,pVictim,spellInfo,res);
        pVictim->HandleProc(vproc,(Unit*)this,spellInfo,res);
    }            
    
    DealDamage(pVictim, res,  2, 0, spellID);

    if (pVictim->GetCurrentSpell())
        pVictim->GetCurrentSpell()->AddTime();

    //VampiricEmbrace
    if(school == SHADOW_DAMAGE)
        if(this->GetGUID() == pVictim->VampEmbCaster && IsUnit())
            static_cast<Unit*>(this)->VampiricEmbrace(res, pVictim);    
}

//*****************************************************************************************
//* SpellLog packets just to keep the code cleaner and better to read
//*****************************************************************************************

void Object::SendSpellLog(Object *Caster, Object *Target,uint32 Ability, uint8 SpellLogType)
{
    if ((!Caster || !Target) && Ability)
        return;

    WorldPacket data(SMSG_SPELLLOGMISS,28);
    data << Ability;                                        // spellid
    data << Caster->GetGUID();                              // caster / player
    data << (uint8)1;                                       // unknown but I think they are const
    data << (uint32)1;                                      // unknown but I think they are const
    data << Target->GetGUID();                              // target
    data << SpellLogType;                                   // spelllogtype
    Caster->SendMessageToSet(&data, true);
}


void Object::SendSpellNonMeleeDamageLog(Object * Caster, Object * Target,uint32 SpellID,uint32 Damage, uint8 Damage_Type,uint32 AbsorbedDamage, uint32 ResistedDamage,bool PhysicalDamage, uint32 BlockedDamage, bool CriticalHit, bool bToset)
{
    if ((!Caster || !Target) && SpellID)
        return;

    WorldPacket data(SMSG_SPELLNONMELEEDAMAGELOG,40);
    data << Target->GetNewGUID();
    data << Caster->GetNewGUID();
    data << SpellID;                                        // Spell / ability ID
    data << Damage;                                         // All damage
    data << Damage_Type;                                    // Damage type
    data << AbsorbedDamage;                                 // Absorbed Damage
    data << ResistedDamage;                                 // Resisted Damage
    data << BlockedDamage;                                  // Blocked Damage
    data << uint8(PhysicalDamage);                          // Physical Damage true / false
    data << uint8(0);                                       // unknown or it binds with PhysicalDamage
    data << uint8(CriticalHit ? 2 : 0);                     // If critical hit this field is == 2
    data << uint8(5);                                       // unknown const
    data << uint32(0);    
    Caster->SendMessageToSet( &data, bToset );
}

int32 Object::event_GetInstanceID()
{
    // return -1 for non-inworld.. so we get our shit moved to the right thread
    if(!m_inWorld)
        return -1;
    else
        return m_instanceId;
}

void Object::UpdateOfflinePlayerData()
{
    stringstream ss;
    ss << "UPDATE characters SET data = '";

    for(uint32 i = 0; i < m_valuesCount; i++ )
        ss << m_uint32Values[i] << " ";
    
    ss << "' WHERE guid=" << GetGUIDLow();
    sDatabase.WaitExecute(ss.str().c_str());
}

void Object::EventSpellDamage(uint64 Victim, uint32 SpellID, uint32 Damage)
{
    Unit * pUnit = World::GetUnit(Victim);
    if(pUnit == 0) return;

    SpellNonMeleeDamageLog(pUnit, SpellID, Damage, true);
}
