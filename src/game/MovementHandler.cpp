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
#define SWIMMING_TOLERANCE_LEVEL -0.08f

class MovementInfo
{
    uint32 unk8, unk9, unk10, unk11, unk12;
    uint32 unklast;
public:
    uint32 time;
    float unk6;
    float x, y, z, orientation;
    uint32 flags;
    uint32 FallTime;
    uint64 transGuid;
    float transX, transY, transZ, transO, transUnk;

    MovementInfo(WorldPacket &data)
    {
        transGuid = 0;
        data >> flags >> time;
        data >> x >> y >> z >> orientation;

        if (flags & 0x200) // Transport
        {
            data >> transGuid >> transX >> transY >> transZ >> transO >> transUnk;
        }
        if (flags & 0x200000) // Swimming
        {
            data >> unk6;
        }
        if (flags & 0x2000) // Falling
        {
            data >> FallTime >> unk8 >> unk9 >> unk10;
        }
        if (flags & 0x4000000)
        {
            data >> unk12;
        }

        data >> unklast;
        if(data.rpos() != data.wpos())
        {
            sLog.outDebug("Extra bits of movement packet left");
        }
    }

    MovementInfo &operator >>(WorldPacket &data)
    {
        data << flags << getMSTime();

        data << x << y << z << orientation;
        
        if (flags & 0x200) // Transport
        {
            data << transGuid << transX << transY << transZ << transO << transUnk;
        }
        if (flags & 0x200000) // Swimming
        {
            data << unk6;
        }
        if (flags & 0x2000) // Falling
        {
            data << FallTime << unk8 << unk9 << unk10;
        }
        if (flags & 0x4000000)
        {
            data << unk12;
        }
        data << unklast;
        return *this;
    }
};

void WorldSession::HandleMoveWorldportAckOpcode( WorldPacket & recv_data )
{
    MapInfo *pMapinfo;
    WorldPacket data;

    if(_player->IsInWorld())
    {
        // get outta here
        return;
    }
    sLog.outDebug( "WORLD: got MSG_MOVE_WORLDPORT_ACK." );
    pMapinfo = sWorld.GetMapInformation(GetPlayer()->GetMapId());
    
    GetPlayer()->SendInitialLogonPackets();
    GetPlayer()->clearAttackers(true);
    //only rezz if player is porting to a instance portal

    if(GetPlayer()->isDead())
    {
        if(pMapinfo)
        {
            if(pMapinfo->type != INSTANCE_NULL)
            GetPlayer()->ResurrectPlayer();
        }
    }

    GetPlayer()->AddToWorld();
    GetPlayer()->SetPlayerStatus(NONE);

    if(GetPlayer()->isDead())
    {
        if(GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_DEATH_WORLD_ENABLE))
        {
            SpellCastTargets tgt;
            tgt.m_unitTarget=GetPlayer()->GetGUID();
            if(GetPlayer()->getRace()==RACE_NIGHTELF)
            {
                SpellEntry *inf=sSpellStore.LookupEntry(20584);
                Spell*sp=new Spell(GetPlayer(),inf,true,NULL);
                sp->prepare(&tgt);
                inf=sSpellStore.LookupEntry(9036);
                sp=new Spell(GetPlayer(),inf,true,NULL);
                sp->prepare(&tgt);
            }
            else
            {
                SpellEntry *inf=sSpellStore.LookupEntry(8326);
                Spell*sp=new Spell(GetPlayer(),inf,true,NULL);
                sp->prepare(&tgt);
            }
        }
    }
    if(pMapinfo)
    {
        if(pMapinfo->HasFlag(WMI_INSTANCE_WELCOME))
        {
            std::string welcome_msg;
            welcome_msg = "Welcome to ";
            welcome_msg += pMapinfo->name;
            welcome_msg += ".";
            sChatHandler.SystemMessage(this, welcome_msg.c_str());
        }
        if(pMapinfo->type == INSTANCE_NONRAID)
        {
            data.Initialize(SMSG_INSTANCE_RESET_ACTIVATE);
            data << uint32(0x00);
            _player->GetSession()->SendPacket(&data);

            sInstanceSavingManager.SavePlayerToInstance(_player, pMapinfo->mapid);
        }
        if(pMapinfo->type == INSTANCE_NULL)
        {
            data.Initialize(SMSG_INSTANCE_RESET_ACTIVATE);
            data << uint32(0x01);
            _player->GetSession()->SendPacket(&data);
        }
    }

    _player->ResetHeartbeatCoords();
}

void WorldSession::HandleMoveTeleportAckOpcode( WorldPacket & recv_data )
{
    sLog.outDebug( "WORLD: got MSG_MOVE_TELEPORT_ACK." );
    GetPlayer()->SetPlayerStatus(NONE);
    GetPlayer()->clearAttackers(true);
    GetPlayer()->SetMovement(MOVE_UNROOT,5);
    _player->ResetHeartbeatCoords();

    if(GetPlayer()->GetSummon() != NULL)        // move pet too
        GetPlayer()->GetSummon()->SetPosition((GetPlayer()->GetPositionX() + 2), (GetPlayer()->GetPositionY() + 2), GetPlayer()->GetPositionZ(), M_PI);

}

void WorldSession::HandleMovementOpcodes( WorldPacket & recv_data )
{
    if(_player == 0)
        return;

    if(GetPlayer()->GetPlayerStatus() == TRANSFER_PENDING) //dont update coords
        return;
    if(!_player->IsInWorld())
        return;

    // spell cancel on movement, for now only fishing is added
    Object * t_go = GetPlayer()->m_SummonedObject;
    if (t_go)
        if (t_go->GetEntry() == GO_FISHING_BOBBER)
            ((GameObject*)t_go)->EndFishing(GetPlayer(),true);

    MovementInfo mi(recv_data);
    GetPlayer()->m_isMoving = true;

    // check for bad coords
    if( !((mi.y >= _minY) && (mi.y <= _maxY)) ||
        !((mi.x >= _minX) && (mi.x <= _maxX)) )
    {
        sLog.outError("%s might be cheating, bad coords specified in movement packet.", _player->GetName());
        return;
    }

    if(GetPlayer()->cannibalize)
    {
        sEventMgr.RemoveEvents(GetPlayer(), EVENT_CANNIBALIZE);
        GetPlayer()->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
        GetPlayer()->cannibalize = false;
    }

    //Send packet to other players

    WorldPacket data(90);   // wont be bigger than 90 bytes
    data.SetOpcode( recv_data.GetOpcode() );
    data << m_MoverWoWGuid;
    mi >> data;

    GetPlayer()->SendMessageToSet(&data, false);

    //Setup Transporter Positioning
    if(mi.transGuid != 0 && !_player->m_lockTransportVariables)
    {
        if(!_player->m_TransporterGUID)
        {
            GetPlayer()->m_TransporterGUID = mi.transGuid;
            _player->m_CurrentTransporter = sObjHolder.GetObject<Transporter>(mi.transGuid);
            if(_player->m_CurrentTransporter)
            {
                _player->m_CurrentTransporter->AddPlayer(_player);
            }
        }

        GetPlayer()->m_TransporterX = mi.transX;
        GetPlayer()->m_TransporterY = mi.transY;
        GetPlayer()->m_TransporterZ = mi.transZ;
        GetPlayer()->m_TransporterO = mi.transO;
        GetPlayer()->m_TransporterUnk = mi.transUnk;
        
        float x = mi.x - mi.transX;
        float y = mi.y - mi.transY;
        float z = mi.z - mi.transZ;
        /*Transporter* trans = _player->m_CurrentTransporter;
        if(trans) sChatHandler.SystemMessageToPlr(_player, "Client t pos: %f %f\nServer t pos: %f %f   Diff: %f %f", x,y, trans->GetPositionX(), trans->GetPositionY(), trans->CalcDistance(x,y,z), trans->CalcDistance(mi.x, mi.y, mi.z));*/
    }
    else
    {
        if(_player->m_TransporterGUID && !_player->m_lockTransportVariables)
        {
            // remove us from the porter
            GetPlayer()->m_TransporterGUID = 0;
            GetPlayer()->m_TransporterX = 0.0f;
            GetPlayer()->m_TransporterY = 0.0f;
            GetPlayer()->m_TransporterZ = 0.0f;
            GetPlayer()->m_TransporterO = 0.0f;

            if(_player->m_CurrentTransporter)
                _player->m_CurrentTransporter->RemovePlayer(_player);

            GetPlayer()->m_CurrentTransporter = NULL;
        }
    }
    _HandleBreathing(recv_data, mi);
    _player->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_MOVEMENT);

    if( _player->m_CurrentCharm )
    {
        _player->m_CurrentCharm->SetPosition(mi.x, mi.y, mi.z, mi.orientation);
    }
    else
    {
        if(!_player->m_CurrentTransporter) 
        {
            if( !_player->SetPosition(mi.x, mi.y, mi.z, mi.orientation) )
            {
                WorldPacket * movedata = GetPlayer( )->BuildTeleportAckMsg(LocationVector(GetPlayer()->GetPositionX(),
                    GetPlayer()->GetPositionY(), GetPlayer()->GetPositionZ(), GetPlayer()->GetOrientation()) );

                SendPacket(movedata);
                delete movedata;
            }
        }
        else
        {
            _player->SetPosition(_player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), 
                mi.orientation + mi.transO, false);
        }
    }    
 
    //Falling Handler
    if (mi.flags & 0x2000) // Falling
    {
        if( GetPlayer()->m_fallTime < mi.FallTime)
            GetPlayer()->m_fallTime = mi.FallTime;
        _player->ResetHeartbeatCoords();
    }
    else //once we done falling lets do some damage
    {
        if(GetPlayer()->m_fallTime > 1000 && GetPlayer()->isAlive() && !_player->GodModeCheat)
        {
            //Check if we aren't falling in water
            if(!_player->bSafeFall)
            {
                if( !(mi.flags & 0x200000) && !_player->blinked)
                {
                    uint8 type;
                    uint32 damage;
                    type = DAMAGE_FALL;
                    //10% dmg per sec after first 3 seconds
                    //it rL a*t*t
                    double coeff=0.0000001*_player->m_fallTime*_player->m_fallTime;
                    damage = (uint32)(_player->GetUInt32Value(UNIT_FIELD_MAXHEALTH)*coeff);
                    // if(damage > GetPlayer()->GetUInt32Value(UNIT_FIELD_MAXHEALTH)) { damage = GetPlayer()->GetUInt32Value(UNIT_FIELD_MAXHEALTH); }

                    WorldPacket data(13);
                    data.SetOpcode(SMSG_ENVIRONMENTALDAMAGELOG);
                    data << GetPlayer()->GetGUID();
                    data << type;
                    data << damage;
                    SendPacket(&data);
                    _player->DealDamage(GetPlayer(), damage, 0, 0, 0);
                }
                GetPlayer()->m_fallTime = 0;
                _player->blinked = false;
                _player->ResetHeartbeatCoords();
            }            
        }
        else
        {
            //player is dead, no need to keep increasing falltime
            GetPlayer()->m_fallTime = 0;
            _player->ResetHeartbeatCoords();
            _player->blinked = false;
        }
    }

    // speedhack protection
    if(sWorld.SpeedhackProtection && GetPermissionCount() == 0 && !_player->blinked && !(mi.flags & 0x2000))
        _SpeedCheck(mi);
}

void WorldSession::HandleMoveStopOpcode( WorldPacket & recv_data )
{    
    GetPlayer()->m_isMoving = false;       
    HandleMovementOpcodes( recv_data );
}

void WorldSession::HandleMoveTimeSkippedOpcode( WorldPacket & recv_data )
{
    //uint64 guid;
    //uint32 time_in_ms;
    //recv_data >> guid >> time_in_ms;
}

void WorldSession::HandleMoveNotActiveMoverOpcode( WorldPacket & recv_data )
{
    uint64 guid;
    recv_data >> guid;

    MovementInfo mi(recv_data);
    WorldPacket data;
    data.SetOpcode(MSG_MOVE_TELEPORT);      // meh..
    FastGUIDPack(data, guid);               // faaast.

    mi >> data;
    _player->SendMessageToSet(&data, false);
}


void WorldSession::HandleSetActiveMoverOpcode( WorldPacket & recv_data )
{
    // set current movement object
    uint64 guid;
    recv_data >> guid;

    if(guid != m_MoverWoWGuid.GetOldGuid())
    {
        // generate wowguid
        if(guid != 0)
            m_MoverWoWGuid.Init(guid);
        else
            m_MoverWoWGuid.Init(_player->GetGUID());
    }
}

void WorldSession::HandleMoveSplineCompleteOpcode(WorldPacket &recvPacket)
{

}

void WorldSession::HandleBasicMovementOpcodes( WorldPacket & recv_data )
{
    if(GetPlayer()->GetPlayerStatus() == TRANSFER_PENDING) //don't update coords
        return;
    if(!_player->IsInWorld())
        return;

    MovementInfo mi(recv_data);

    // check for bad coords
    if( !((mi.y >= _minY) && (mi.y <= _maxY)) ||
        !((mi.x >= _minX) && (mi.x <= _maxX)) )
    {
        sLog.outError("%s might be cheating, bad coords specified in movement packet.", _player->GetName());
        return;
    }

    GetPlayer()->m_isMoving = true;
    //Send packet to other players

    WorldPacket data(90);
    data.SetOpcode( recv_data.GetOpcode() );
    data << m_MoverWoWGuid;
    mi >> data;

    GetPlayer()->SendMessageToSet(&data, false);

    //Setup Transporter Positioning
    if(mi.transGuid != 0 && !_player->m_lockTransportVariables)
    {
        if(!_player->m_TransporterGUID)
        {
            GetPlayer()->m_TransporterGUID = mi.transGuid;
            _player->m_CurrentTransporter = sObjHolder.GetObject<Transporter>(mi.transGuid);
            if(_player->m_CurrentTransporter)
            {
                _player->m_CurrentTransporter->AddPlayer(_player);
            }
        }

        GetPlayer()->m_TransporterX = mi.transX;
        GetPlayer()->m_TransporterY = mi.transY;
        GetPlayer()->m_TransporterZ = mi.transZ;
        GetPlayer()->m_TransporterO = mi.transO;
        GetPlayer()->m_TransporterUnk = mi.transUnk;

        float x = mi.x - mi.transX;
        float y = mi.y - mi.transY;
        float z = mi.z - mi.transZ;
        /*Transporter* trans = _player->m_CurrentTransporter;
        if(trans) sChatHandler.SystemMessageToPlr(_player, "Client t pos: %f %f\nServer t pos: %f %f   Diff: %f %f", x,y, trans->GetPositionX(), trans->GetPositionY(), trans->CalcDistance(x,y,z), trans->CalcDistance(mi.x, mi.y, mi.z));*/
    }
    else
    {
        if(_player->m_TransporterGUID && !_player->m_lockTransportVariables)
        {
            // remove us from the porter
            GetPlayer()->m_TransporterGUID = 0;
            GetPlayer()->m_TransporterX = 0.0f;
            GetPlayer()->m_TransporterY = 0.0f;
            GetPlayer()->m_TransporterZ = 0.0f;
            GetPlayer()->m_TransporterO = 0.0f;

            if(_player->m_CurrentTransporter)
                _player->m_CurrentTransporter->RemovePlayer(_player);
            
            GetPlayer()->m_CurrentTransporter = NULL;
        }
    }
    _HandleBreathing(recv_data, mi);

    if( _player->m_CurrentCharm )
    {
        _player->m_CurrentCharm->SetPosition(mi.x, mi.y, mi.z, mi.orientation);
    }
    else
    {
        if( !_player->m_CurrentTransporter)
        {
            if(!GetPlayer( )->SetPosition(mi.x, mi.y, mi.z, mi.orientation) )
            {
                WorldPacket * movedata = GetPlayer( )->BuildTeleportAckMsg(LocationVector(GetPlayer()->GetPositionX(),
                    GetPlayer()->GetPositionY(), GetPlayer()->GetPositionZ(), GetPlayer()->GetOrientation()) );

                SendPacket(movedata);
                delete movedata;
            }
        }
        else
        {
            _player->SetPosition(_player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), 
                mi.orientation + mi.transO, false);
        }
    }

    // speedhack protection
    if(sWorld.SpeedhackProtection && GetPermissionCount() == 0 && !_player->blinked)
        _SpeedCheck(mi);
}

void WorldSession::_HandleBreathing(WorldPacket &recv_data, MovementInfo &mi)
{
    if(!sWorld.BreathingEnabled || _player->FlyCheat || _player->m_bUnlimitedBreath || _player->GodModeCheat)
        return;

    uint8 wtype  = _player->m_mapMgr->GetWaterType(mi.x, mi.y);
    float wlevel = _player->m_mapMgr->GetWaterHeight(mi.x, mi.y);

    if(mi.flags & 0x200000)
    {
        if(!(_player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING))
            _player->m_UnderwaterState |= UNDERWATERSTATE_SWIMMING;
    }
    else
    {
        if(_player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING)
            _player->m_UnderwaterState &= ~UNDERWATERSTATE_SWIMMING;

        if(_player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER)
            _player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;

        return;
    }

    //printf("Water flags: %X\n", wtype);

    if((mi.z - 2) < wlevel)
    {
        // underwater, w000t!
        if(_player->m_MountSpellId)
            _player->RemoveAura(_player->m_MountSpellId);
        
        if(!(_player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER))
        {
            // we only just entered the water
            _player->m_UnderwaterState |= UNDERWATERSTATE_UNDERWATER;

            // send packet
            WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
            data << uint32(1) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(-1) << uint32(0);
            SendPacket(&data);
        }
    }
    else
    {
        // we're not underwater
        if(_player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER && recv_data.GetOpcode() != MSG_MOVE_JUMP)
        {
            WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
            data << uint32(1) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << uint32(10) << uint32(0);
            SendPacket(&data);

            _player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
        }
    }

    //printf("Water flags: %02X\n", GetPlayer()->GetMapMgr()->GetLiquidType(GetPlayer()->GetPositionX(),GetPlayer()->GetPositionY()));
    /*if(mi.flags & 0x200000 && GetPlayer()->isAlive())
    {
        uint8 wtype = GetPlayer()->GetMapMgr()->GetLiquidType(GetPlayer()->GetPositionX(),GetPlayer()->GetPositionY());

        if((GetPlayer()->m_UnderwaterState & UNDERWATERSTATE_SWIMMING) == 0)      // set swimming.
            GetPlayer()->m_UnderwaterState |= UNDERWATERSTATE_SWIMMING;

        // Dismount if mounted
        if(_player->m_MountSpellId)
            _player->RemoveAura(_player->m_MountSpellId);

        if(!wtype) wtype = 0x01;

        if(wtype & 0x2)
        {
            if( !(GetPlayer()->m_UnderwaterState & UNDERWATERSTATE_LAVA) )
                GetPlayer()->m_UnderwaterState |= UNDERWATERSTATE_LAVA;
        }
        else if(mi.unk6 < SWIMMING_TOLERANCE_LEVEL && !(GetPlayer()->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER) && !_player->m_bUnlimitedBreath)  // that's it, we're underwater.
        {
            GetPlayer()->m_UnderwaterState |= UNDERWATERSTATE_UNDERWATER;

            if( !(GetPlayer()->m_UnderwaterState & UNDERWATERSTATE_RECOVERING) )      // fix for "unlimited" bug
                GetPlayer()->m_UnderwaterState &= ~UNDERWATERSTATE_RECOVERING;

            // send start packet
            WorldPacket data(20);
            data.SetOpcode(SMSG_START_MIRROR_TIMER);
            data << uint32(1) << uint32(60000-GetPlayer()->m_UnderwaterTime) << uint32(60000) << uint32(-1) << uint32(0);
            SendPacket(&data);
        }
        else if(mi.unk6 == 0)
        {
            if((GetPlayer()->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER) != 0)
            {
                GetPlayer()->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
                if(GetPlayer()->m_UnderwaterTime && !(GetPlayer()->m_UnderwaterState & UNDERWATERSTATE_RECOVERING) )
                {
                    GetPlayer()->m_UnderwaterState |= UNDERWATERSTATE_RECOVERING;
                    WorldPacket data(20);
                    data.SetOpcode(SMSG_START_MIRROR_TIMER);
                    data << uint32(1) << uint32(60000 - GetPlayer()->m_UnderwaterTime) << uint32(60000) << uint32(10) << uint32(0);
                    SendPacket(&data);
                }
            }

            if( GetPlayer()->m_UnderwaterState & UNDERWATERSTATE_TAKINGDAMAGE )
            {
                GetPlayer()->m_UnderwaterState &= ~UNDERWATERSTATE_TAKINGDAMAGE;
            }
        }

    }
    else if(recv_data.GetOpcode() != MSG_MOVE_JUMP) 
    {     // otherwise cheaters :P        
        if(_player->m_ShapeShifted == 1066)
            _player->RemoveAura(1066);  // remove aquatic form shapeshift

        if( GetPlayer()->m_UnderwaterState & UNDERWATERSTATE_SWIMMING )
        {
            GetPlayer()->m_UnderwaterState &= ~UNDERWATERSTATE_SWIMMING;
            GetPlayer()->m_SwimmingTime = 0;
        }

        if( GetPlayer()->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER )
        {
            GetPlayer()->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
            if(GetPlayer()->m_UnderwaterTime && !(GetPlayer()->m_UnderwaterState & UNDERWATERSTATE_RECOVERING) )
            {
                GetPlayer()->m_UnderwaterState |= UNDERWATERSTATE_RECOVERING;
                WorldPacket data(20);
                data.SetOpcode(SMSG_START_MIRROR_TIMER);
                data << uint32(1) << uint32(60000 - GetPlayer()->m_UnderwaterTime) << uint32(60000) << uint32(10) << uint32(0);
                SendPacket(&data);
            }
        }

        if( GetPlayer()->m_UnderwaterState & UNDERWATERSTATE_TAKINGDAMAGE )
        {
            GetPlayer()->m_UnderwaterState &= ~UNDERWATERSTATE_TAKINGDAMAGE;
        }

        if( GetPlayer()->m_UnderwaterState & UNDERWATERSTATE_LAVA )
            GetPlayer()->m_UnderwaterState &= ~UNDERWATERSTATE_LAVA;
    }*/
}

void WorldSession::_SpeedCheck(MovementInfo &mi)
{
    // beat!

    // calculate distance between last heartbeat and this
    if(_player->_lastHeartbeatTime && _player->_lastHeartbeatX &&
        _player->_lastHeartbeatY && _player->_lastHeartbeatZ)
    {
        uint32 new_time = getMSTime();
        float distance_travelled = _player->CalcDistance(_player->_lastHeartbeatX,
            _player->_lastHeartbeatY, _player->_lastHeartbeatZ, mi.x, mi.y, mi.z);
        // get our time difference
        uint32 time_difference = new_time - _player->_lastHeartbeatTime;

        // do our check calculation
        uint32 move_time = (uint32)((float)distance_travelled / (float)(_player->m_runSpeed*0.001f));

        // check if we're in the correct bounds
        if(move_time > time_difference)
        {
            int32 difference = move_time - time_difference;
            if(difference > 350)    // say this for now
            {
                if(_player->m_speedhackChances)
                {
                    sChatHandler.SystemMessage(this, "Speedhack detected. This has been logged for later processing by the server admins. If you are caught again, you will be kicked from the server. You will be unrooted in 5 seconds.");
                    _player->SetMovement(MOVE_ROOT, 1);
                    sEventMgr.AddEvent(_player, &Player::SetMovement, uint8(MOVE_UNROOT), uint32(1), EVENT_DELETE_TIMER, 5000, 1);
                    _player->ResetHeartbeatCoords();
                    _player->m_speedhackChances--;
                    
                    // TODO: replace with server plugin system later on
                    sCheatLog.writefromsession(this, "Speedhack warning, time diff of %u", time_difference);
                }
                else if(_player->m_speedhackChances == 0)
                {
                    sChatHandler.SystemMessage(this, "You will now be removed from the server for speed hacking. Your account has been flagged for further investigation by the admins.");

                    // TODO: replace with server plugin system later on
                    sCheatLog.writefromsession(this, "Kicked for speedhack, time diff of %u", difference);

                    _player->m_KickDelay = 0;
                    sEventMgr.AddEvent(_player, &Player::_Kick, EVENT_PLAYER_KICK, 10000, 1);

                    // Root movement :p heheheh evil
                    _player->SetMovement(MOVE_ROOT, 1);
                }
            }

            //printf("Move shit: %ums\n", abs(difference));
            //sChatHandler.SystemMessage(this, "Move time : %u / %u, diff: %u", move_time, time_difference, difference);
        }
    }
    _player->_lastHeartbeatTime = getMSTime();
    _player->_lastHeartbeatX = mi.x;
    _player->_lastHeartbeatY = mi.y;
    _player->_lastHeartbeatZ = mi.z;     
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket &recvdata)
{
    WorldPacket data(SMSG_MOUNTSPECIAL_ANIM,8);
    data << _player->GetGUID();
    _player->SendMessageToSet(&data, true);
}