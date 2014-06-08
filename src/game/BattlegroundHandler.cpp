// Copyright (C) 2004 WoW Daemon
// Copyright (C) 2006 Burlex
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

void WorldSession::HandleBattlefieldPortOpcode(WorldPacket &recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 5);
    sLog.outString("BATTLEGROUND: Recieved CMSG_BATTLEFIELD_PORT packet.");

    uint16 mapinfo;
    uint8 action;
    uint16 unk;
    uint32 Battlegroundtype;
    
    recv_data >> unk >> Battlegroundtype >> mapinfo >> action;
    
    if(action == 0)
    {
        // Leave queue
    }
    else
    {
        // if we are already in a BG
        Battleground *lastbg = sBattlegroundMgr.GetBattleground(GetPlayer()->m_bgLastBattlegroundID);
        if(lastbg)
            lastbg->RemovePlayer(GetPlayer(), false, false, false);
            
        // Enter battleground!
        Battleground *bg = sBattlegroundMgr.GetBattleground(GetPlayer()->m_bgBattlegroundID);
        if(!bg)
            sLog.outError("BATTLEGROUND: Warning! Could not find battleground instance %d", GetPlayer()->m_bgBattlegroundID);
        else
            bg->AddPlayer(GetPlayer(), true, true);
    }
}

void WorldSession::HandleBattlefieldStatusOpcode(WorldPacket &recv_data)
{
    //sLog.outDebug("BATTLEGROUND: Recieved CMSG_BATTLEFIELD_STATUS packet.");
    
    if(!GetPlayer()->m_bgInBattleground || GetPlayer()->GetCurrentBattleground() == NULL)
    {
        WorldPacket pkt;
        pkt.Initialize(SMSG_BATTLEFIELD_STATUS);
        pkt << uint32(0x00) << uint32(0x00);
        SendPacket(&pkt);
        return;
    }

    WorldPacket * pkt;
    Battleground * bg = GetPlayer()->GetCurrentBattleground();
    uint32 CurrentTime = uint32(time(NULL));
    pkt = sBattlegroundMgr.BuildBattlegroundStatusPacket(0, bg->GetBattlegroundType(), bg->GetInstanceID(), 3, CurrentTime, 0);
    SendPacket(pkt);
    delete pkt;
}

void WorldSession::HandleBattlefieldListOpcode(WorldPacket &recv_data)
{
    // TODO capt implement this
    sLog.outString("Recieved CMSG_BATTLEFIELD_LIST");
}

void WorldSession::SendBattlegroundList(Creature* pCreature, uint32 mapid)
{
    if(!pCreature)
        return;
    
    WorldPacket * pkt = sBattlegroundMgr.BuildBattlegroundListPacket(pCreature->GetGUID(), _player, mapid);
    SendPacket(pkt);
    delete pkt;
}

void WorldSession::HandleBattleMasterHelloOpcode(WorldPacket &recv_data)
{
    sLog.outString("Recieved CMSG_BATTLEMASTER_HELLO");
}

void WorldSession::HandleLeaveBattlefieldOpcode(WorldPacket &recv_data)
{
    sLog.outString("Recieved CMSG_LEAVEBATTLEFIELD");
    Battleground *bg = GetPlayer()->GetCurrentBattleground();
    if(!bg)
        return;
    bg->RemovePlayer(GetPlayer(),true,true,false);
}

void WorldSession::HandleAreaSpiritHealerQueryOpcode(WorldPacket &recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 8);
    if(!GetPlayer()->m_bgInBattleground) return;
    Battleground *bg = GetPlayer()->GetCurrentBattleground();
    if(!bg)
        return;
    
    sLog.outString("Recieved CMSG_AREA_SPIRITHEALER_QUERY");
    uint64 guid;
    uint32 rtime;
    recv_data >> guid;
    WorldPacket data(SMSG_AREA_SPIRIT_HEALER_TIME, 12);
    uint32 NextRes = bg->m_LastResurrect + 30;    // 30 secs between
    uint32 ResTime = NextRes - (uint32(time(NULL)));
    ResTime *= 1000;    // 1000 ms per sec
    rtime = ResTime;
    data << guid << rtime;
    SendPacket(&data);
}

void WorldSession::HandleAreaSpiritHealerQueueOpcode(WorldPacket &recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 8);
    sLog.outString("Recieved CMSG_AREA_SPIRITHEALER_QUEUE");
    uint64 guid;
    recv_data >> guid;
    sLog.outDetail("Guid: "I64FMT"", guid);
    Battleground *bg = GetPlayer()->GetCurrentBattleground();
    if(bg)
        bg->m_ReviveQueue[guid] = GetPlayer();
}

void WorldSession::HandleBattlegroundPlayerPositionsOpcode(WorldPacket &recv_data)
{
    Battleground *bg = sBattlegroundMgr.GetBattleground(GetPlayer()->m_bgBattlegroundID);
    if(bg == NULL) return;
    WorldPacket *pkt = bg->BuildPlayerPositionsPacket(GetPlayer()); 
    SendPacket(pkt);
    delete pkt;
}

void WorldSession::HandleBattleMasterJoinOpcode(WorldPacket &recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 17);
    sLog.outString("BATTLEGROUND: Recieved CMSG_BATTLEMASTER_JOIN");
    uint64 guid;
    uint32 BattleGroundType;
    uint32 instance;
    uint8 unk;
    recv_data >> guid >> BattleGroundType >> instance >> unk;
    
    Battleground *bg = sBattlegroundMgr.GetBattlegroundByInstanceID(instance,BattleGroundType);
    if(!bg)
        return;
        
    WorldPacket *pkt = sBattlegroundMgr.BuildBattlegroundStatusPacket(0, BattleGroundType, (instance==0 ? 1 : instance), 1, 0x00FFFF00, 0);
    SendPacket(pkt);
    delete pkt;

    if(bg->HasFreeSlots(sBattlegroundMgr.GenerateTeamByRace(GetPlayer()->getRace())))
    {    
        GetPlayer()->m_bgBattlegroundID = bg->GetID();
        
        pkt = sBattlegroundMgr.BuildBattlegroundStatusPacket(0, BattleGroundType, (instance==0 ? 1 : instance), 2, 120, 0);
        SendPacket(pkt);
        delete pkt;
    }
}

void WorldSession::HandleArenaJoinOpcode(WorldPacket &recv_data)
{
	CHECK_PACKET_SIZE(recv_data, 11);
	sLog.outString("BATTLEGROUND: Recieved CMSG_ARENA_JOIN");
	uint64 unk1;
	uint8 arenatype; // 0 - 2vs2 , 1 - 3vs3, 2 - 5vs5
	uint16 unk2;

	recv_data >> unk1 >> arenatype >> unk2; // dunno yet how to handle this.

    Battleground *bg = sBattlegroundMgr.GetBattlegroundByInstanceID(0,5); // search for the first instance for now... 
    if(!bg)
        return;

	WorldPacket *pkt = sBattlegroundMgr.BuildBattlegroundStatusPacket(0, 5, 1, 1, 0x00FFFF00, 0); // put w8ting status.
    SendPacket(pkt);
    delete pkt;

	if(bg->HasFreeSlots(sBattlegroundMgr.GenerateTeamByRace(GetPlayer()->getRace())))
    {    
        GetPlayer()->m_bgBattlegroundID = bg->GetID();
        
        pkt = sBattlegroundMgr.BuildBattlegroundStatusPacket(0, 5, 1, 2, 120, 0); // ask for player to join
        SendPacket(pkt);
        delete pkt;
    }
}

void WorldSession::HandleInspectHonorStatsOpcode(WorldPacket &recv_data)
{
    sLog.outString("Recieved MSG_INSPECT_HONOR_STATS");
}

void WorldSession::HandlePVPLogDataOpcode(WorldPacket &recv_data)
{
    Battleground *bg = sBattlegroundMgr.GetBattleground(GetPlayer()->m_bgBattlegroundID);
    if(bg == NULL) return;
    WorldPacket *pkt = bg->BuildPVPLogDataPacket(GetPlayer(), bg->GetBattleGroundStatus(), bg->GetBattleGameWin());
    SendPacket(pkt);
    delete pkt;
}

