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


//////////////////////////////////////////////////////////////
/// This function handles CMSG_GROUP_INVITE
//////////////////////////////////////////////////////////////
void WorldSession::HandleGroupInviteOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 1);
    WorldPacket data;
    std::string membername;
    Player * player = NULL;
    Group *group = NULL;

    recv_data >> membername;
    if(_player->HasBeenInvited())return;

    player = objmgr.GetPlayer(membername.c_str(), false);

    if ( player == NULL || player->GetMapId() != _player->GetMapId())
    {
        SendPartyCommandResult(_player, 0, membername, ERR_PARTY_CANNOT_FIND);
        return;
    }

    if ( _player->InGroup() && !_player->IsGroupLeader() )
    {
        SendPartyCommandResult(_player, 0, "", ERR_PARTY_YOU_ARE_NOT_LEADER);
        return;
    }

    if(player->GetMapId() != _player->GetMapId())
    {
        sChatHandler.RedSystemMessageToPlr(_player, "You cannot invite people on another map to a party with you.");
        return;
    }

    group = _player->GetGroup();
    if ( group != NULL )
    {
        if (group->IsFull())
        {
            SendPartyCommandResult(_player, 0, "", ERR_PARTY_IS_FULL);
            return;
        }
    }

    if ( player->InGroup() )
    {
        SendPartyCommandResult(_player, player->GetGroup()->GetGroupType(), membername, ERR_PARTY_ALREADY_IN_GROUP);
        return;
    }
    
    if(player->GetTeam()!=_player->GetTeam() && _player->GetSession()->GetPermissionCount() == 0)
    {
        SendPartyCommandResult(_player, 0, membername, ERR_PARTY_WRONG_FACTION);
        return;
    }

    if ( player->HasBeenInvited() )
    {
        SendPartyCommandResult(_player, 0, membername, ERR_PARTY_ALREADY_IN_GROUP);
        return;
    }

    if(sSocialMgr.HasIgnore(player, _player))
    {
        SendPartyCommandResult(_player, 0, membername, ERR_PARTY_IS_IGNORING_YOU);
        return;
    }

    // burlex 16/08/06 - change to guid to prevent very unlikely event of a crash in deny, etc
    _player->SetInviter(_player->GetGUID());//bugfix if player invtied 2 people-> he can be in 2 parties

    data.Initialize(SMSG_GROUP_INVITE);
    data << GetPlayer()->GetName();

    player->GetSession()->SendPacket(&data);

    uint32 gtype = 0;
    if(group)
        gtype = group->GetGroupType();

    SendPartyCommandResult(_player, gtype, membername, ERR_PARTY_NO_ERROR);

    // burlex 16/08/06 - change to guid to prevent very unlikely event of a crash in deny, etc
    player->SetInviter(_player->GetGUID());
}

///////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_CANCEL:
///////////////////////////////////////////////////////////////
void WorldSession::HandleGroupCancelOpcode( WorldPacket & recv_data )
{
    sLog.outDebug( "WORLD: got CMSG_GROUP_CANCEL." );
}

////////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_ACCEPT:
////////////////////////////////////////////////////////////////
void WorldSession::HandleGroupAcceptOpcode( WorldPacket & recv_data )
{
    WorldPacket data;

    Player *player = sObjHolder.GetObject<Player>(_player->GetInviter());
    if ( !player )
        return;
    
    player->SetInviter(0);
    _player->SetInviter(0);
    
    Group *grp = player->GetGroup();

    if(grp)
    {
        grp->AddMember(_player);
        return;
    }
    
    // If we're this far, it means we have no existing group, and have to make one.
    grp = new Group;
    grp->AddMember(player);        // add the inviter first, therefore he is the leader
    grp->AddMember(_player);       // add us.

    // Currentgroup and all that shit are set by addmember.
}

///////////////////////////////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_DECLINE:
//////////////////////////////////////////////////////////////////////////////////////
void WorldSession::HandleGroupDeclineOpcode( WorldPacket & recv_data )
{
    WorldPacket data;

    Player *player = sObjHolder.GetObject<Player>(_player->GetInviter());
    if(!player) return;

    data.Initialize( SMSG_GROUP_DECLINE );
    data << GetPlayer()->GetName();

    player->GetSession()->SendPacket( &data );
    player->SetInviter(0);
    _player->SetInviter(0);
}

//////////////////////////////////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_UNINVITE:
//////////////////////////////////////////////////////////////////////////////////////////
void WorldSession::HandleGroupUninviteOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 1);
    WorldPacket data;
    std::string membername;
    Group *group;
    Player * player;

    recv_data >> membername;

    player = objmgr.GetPlayer(membername.c_str(), false);
    if ( player == NULL )
    {
        SendPartyCommandResult(_player, 0, membername, ERR_PARTY_CANNOT_FIND);
        return;
    }

    if ( !_player->InGroup() || _player->GetGroup() != player->GetGroup() )
    {
        SendPartyCommandResult(_player, 0, membername, ERR_PARTY_IS_NOT_IN_YOUR_PARTY);
        return;
    }

    if ( !_player->IsGroupLeader() )
    {
        if(_player != player)
        {
            SendPartyCommandResult(_player, 0, "", ERR_PARTY_YOU_ARE_NOT_LEADER);
            return;
        }
    }

    group = _player->GetGroup();

    if(group)
        group->RemovePlayer(player);
}

//////////////////////////////////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_UNINVITE_GUID:
//////////////////////////////////////////////////////////////////////////////////////////
void WorldSession::HandleGroupUninviteGuildOpcode( WorldPacket & recv_data )
{
    sLog.outDebug( "WORLD: got CMSG_GROUP_UNINVITE_GUID." );
}

//////////////////////////////////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_SET_LEADER:
//////////////////////////////////////////////////////////////////////////////////////////
void WorldSession::HandleGroupSetLeaderOpcode( WorldPacket & recv_data )
{
    // Capt: important note _player->GetName() can be wrong.
    CHECK_PACKET_SIZE(recv_data, 1);
    WorldPacket data;
    uint64 MemberGuid;
    Player * player;

    recv_data >> MemberGuid;
    
    player = objmgr.GetPlayer(MemberGuid);

    if ( player == NULL )
    {
        //SendPartyCommandResult(_player, 0, membername, ERR_PARTY_CANNOT_FIND);
        SendPartyCommandResult(_player, 0, _player->GetName(), ERR_PARTY_CANNOT_FIND);
        return;
    }

    if(!_player->IsGroupLeader())
    {
        SendPartyCommandResult(_player, 0, "", ERR_PARTY_YOU_ARE_NOT_LEADER);
        return;
    }
    
    if(player->GetGroup() != _player->GetGroup())
    {
        //SendPartyCommandResult(_player, 0, membername, ERR_PARTY_IS_NOT_IN_YOUR_PARTY);
        SendPartyCommandResult(_player, 0, _player->GetName(), ERR_PARTY_IS_NOT_IN_YOUR_PARTY);
        return;
    }

    Group *pGroup = _player->GetGroup();
    if(pGroup)
        pGroup->SetLeader(player);
}

//////////////////////////////////////////////////////////////////////////////////////////
///This function handles CMSG_GROUP_DISBAND:
//////////////////////////////////////////////////////////////////////////////////////////
void WorldSession::HandleGroupDisbandOpcode( WorldPacket & recv_data )
{
    Group* pGroup = _player->GetGroup();
    if(!pGroup) return;

    pGroup->RemovePlayer(_player);
}

//////////////////////////////////////////////////////////////////////////////////////////
///This function handles CMSG_LOOT_METHOD:
//////////////////////////////////////////////////////////////////////////////////////////
void WorldSession::HandleLootMethodOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 16);
    uint32 lootMethod;
    uint64 lootMaster;
    uint32 threshold;

    recv_data >> lootMethod >> lootMaster >>threshold;
  
    if(!_player->IsGroupLeader())
    {
        SendPartyCommandResult(_player, 0, "", ERR_PARTY_YOU_ARE_NOT_LEADER);
        return;
    }
    
    Player *plyr = ObjectHolder::getSingleton().GetObject<Player>(lootMaster);

    Group *pGroup = _player->GetGroup();
    if(pGroup)
        pGroup->SetLooter(plyr, lootMethod, threshold);
}

void WorldSession::HandleMinimapPingOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 8);
    if(!_player->InGroup())
    return;
    Group * party= _player->GetGroup();
    if(!party)return;

    float x,y;
    recv_data >> x >>y;
    WorldPacket data;
    data.SetOpcode(MSG_MINIMAP_PING);
    data << _player->GetGUID();
    data << x << y;
    party->SendPacketToAllButOne(&data, _player);
}

void WorldSession::HandleSetPlayerIconOpcode(WorldPacket& recv_data)
{
    //printf("Received Set Player Icon opcode, size: %d\n", recv_data.size());
    if(recv_data.size() < 9 || !_player->m_targetIcon)
    {
        // Clearing our icon
        /*WorldPacket data;
        data.Initialize(MSG_GROUP_SET_PLAYER_ICON);
        data << uint8(0x00) << uint8(0) << _player->GetGUID();

        if(_player->GetGroup())
            _player->GetGroup()->SendPacketToAll(&data);
        
        _player->m_targetIcon = 0;*/

        return;
    }

    uint64 guid;
    uint8 icon;
    recv_data >> icon >> guid;

    Player *plyr = sObjHolder.GetObject<Player>(guid);
    if(!plyr) return;

    plyr->m_targetIcon = icon;
    WorldPacket data;
    data.Initialize(MSG_GROUP_SET_PLAYER_ICON);
    data << uint8(0x00) << uint8(icon) << plyr->GetGUID();
    
    Group *grp = _player->GetGroup();
    if(grp)
        grp->SendPacketToAll(&data);
}

void WorldSession::SendPartyCommandResult(Player *pPlayer, uint32 p1, std::string name, uint32 err)
{
    // Capt: if error message do not work, please sniff it and leave me a message
    if(pPlayer)
    {
        WorldPacket data;
        data.Initialize(SMSG_PARTY_COMMAND_RESULT);
        data << p1;
        if(!name.length())
            data << uint8(0);
        else
            data << name.c_str();

        data << err;
        pPlayer->GetSession()->SendPacket(&data);
    }
}
