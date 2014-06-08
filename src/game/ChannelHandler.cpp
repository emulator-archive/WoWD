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

initialiseSingleton( ChannelMgr );

void WorldSession::HandleChannelJoin(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    if(GetPlayer()->GetTaxiState()) return;     // dont allow join while on taxi
    string channelname,pass;
    uint32 code;
    uint8 crap;

    // Skip the first bytes for 2.0.6 (dunno what the point of the crap is, anyway)
    if(client_build >= 6337)
        recvPacket >> code >> crap;

    recvPacket >> channelname;
    recvPacket >> pass;
    if(channelmgr.GetJoinChannel(channelname.c_str(),GetPlayer())->Join(GetPlayer(),pass.c_str()))
    {
        WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
        data << uint8(2) << channelname;
        if(client_build >= 6337)
            data << uint64(code);
        else
            data << _player->GetGUID();
        SendPacket(&data);
    }
}

void WorldSession::HandleChannelLeave(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
  //  if(GetPlayer()->GetTaxiState()) return;     // dont allow join while on taxi
    //if we leave game etc we should leave this channel, we should not join -- yes,but not leave
    string channelname;
    uint32 code;

    if(client_build >= 6337)
        recvPacket >> code;

    recvPacket >> channelname;

    if(!channelname.length())
        return;

    if(channelmgr.LeftChannel(channelname.c_str(),_player))
    {
        WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
        data << uint8(0x03) << channelname;
        if(client_build >= 6337)
            data << uint64(code);
        SendPacket(&data);
    }
}

void WorldSession::HandleChannelList(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname;
    recvPacket >> channelname;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->List(GetPlayer());
}

void WorldSession::HandleChannelPassword(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname,pass;
    recvPacket >> channelname;
    recvPacket >> pass;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->Password(GetPlayer(),pass.c_str());
}

void WorldSession::HandleChannelSetOwner(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname,newp;
    recvPacket >> channelname;
    recvPacket >> newp;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer());
    if(chn) chn->SetOwner(GetPlayer(),newp.c_str());
}

void WorldSession::HandleChannelOwner(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname;
    recvPacket >> channelname;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->GetOwner(GetPlayer());
}

void WorldSession::HandleChannelModerator(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname,otp;
    recvPacket >> channelname;
    recvPacket >> otp;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->SetModerator(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelUnmoderator(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname,otp;
    recvPacket >> channelname;
    recvPacket >> otp;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->UnsetModerator(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelMute(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname,otp;
    recvPacket >> channelname;
    recvPacket >> otp;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->SetMute(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelUnmute(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname,otp;
    recvPacket >> channelname;
    recvPacket >> otp;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->UnsetMute(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelInvite(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname,otp;
    recvPacket >> channelname;
    recvPacket >> otp;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->Invite(GetPlayer(),otp.c_str());
}
void WorldSession::HandleChannelKick(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname,otp;
    recvPacket >> channelname;
    recvPacket >> otp;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->Kick(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelBan(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname,otp;
    recvPacket >> channelname;
    recvPacket >> otp;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->Ban(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelUnban(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname,otp;
    recvPacket >> channelname;
    recvPacket >> otp;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->UnBan(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelAnnounce(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname;
    recvPacket >> channelname;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->Announce(GetPlayer());
}

void WorldSession::HandleChannelModerate(WorldPacket& recvPacket)
{
    CHECK_PACKET_SIZE(recvPacket, 1);
    string channelname;
    recvPacket >> channelname;
    Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->Moderate(GetPlayer());
}
