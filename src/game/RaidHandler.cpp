// Copyright (C) 2005 WoW Daemon
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

void WorldSession::HandleConvertGroupToRaidOpcode(WorldPacket & recv_data)
{
    // This is just soooo easy now   
    Group *pGroup = _player->GetGroup();
    if(!pGroup) return;

    if ( pGroup->GetLeader() != _player )   //access denied
    {
        SendPartyCommandResult(_player, 0, "", ERR_PARTY_YOU_ARE_NOT_LEADER);
        return;
    }

    pGroup->ExpandToRaid();
    SendPartyCommandResult(_player, 0, "", ERR_PARTY_NO_ERROR);
}

void WorldSession::HandleGroupChangeSubGroup(WorldPacket & recv_data)
{
    std::string name;
    uint8 subGroup;

    recv_data >> name;
    recv_data >> subGroup;

    Player *plyr = objmgr.GetPlayer(name.c_str());
    if(!plyr)
        return;

    Group *pGroup = _player->GetGroup();
    if(!pGroup)
        return;

    pGroup->MovePlayer(plyr, subGroup);
}

void WorldSession::HandleGroupAssistantLeader(WorldPacket & recv_data)
{
    //80

    std::string name;
    uint8 subGroup;

    recv_data >> name;
    recv_data >> subGroup;

    Player *plyr = objmgr.GetPlayer(name.c_str());
    if(!plyr)
        return;

    Group *pGroup = _player->GetGroup();
    if(!pGroup) return;

    pGroup->SetSubGroupLeader(plyr,subGroup);
}

void WorldSession::HandleRequestRaidInfoOpcode(WorldPacket & recv_data)
{  
    //          SMSG_RAID_INSTANCE_INFO             = 716,  //(0x2CC)    
}

void WorldSession::HandleReadyCheckOpcode(WorldPacket& recv_data)
{
    // Seems to be looping. Will re-enable later...

    /*
    Raid *raid = _player->GetCurrentRaid();
    if(!raid) return;
    if(raid->GetRaidLeader()->GetGUID() == _player->GetGUID())
    {
        // Forward the packet onto all raid members. This will cause the box to come up, yes/no.
        WorldPacket *data = new WorldPacket;
        data->Initialize(CMSG_RAID_READYCHECK);
        for(int i=0;i<8;i++)
        {
            RaidGroup *group = raid->GetRaidGroup(i);
            for(std::set<Player*>::iterator it = group->SubGroupMembers.begin(); it!=group->SubGroupMembers.end(); ++it)
                if((*it)->GetGUID() != _player->GetGUID()) (*it)->GetSession()->SendPacket(data);       // dont send to self
        }
        delete data;
    } else {
        uint8 ready = 0;
        if(recv_data.size() >= 1) recv_data >> ready;
        // Build a packet, and send to all (but self, again)

        WorldPacket *data = new WorldPacket;
        data->Initialize(CMSG_RAID_READYCHECK);
        *data << _player->GetGUID() << ready;
        for(int i=0;i<8;i++)
        {
            RaidGroup *group = raid->GetRaidGroup(i);
            for(std::set<Player*>::iterator it = group->SubGroupMembers.begin(); it!=group->SubGroupMembers.end(); ++it)
                if((*it)->GetGUID() != _player->GetGUID()) (*it)->GetSession()->SendPacket(data);       // dont send to self
        }
        delete data;
    }*/
}
