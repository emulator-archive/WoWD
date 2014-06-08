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

void WorldSession::HandleDuelAccepted(WorldPacket & recv_data)
{
    if(_player->DuelingWith == 0)
        return;

    _player->m_duelStatus = DUEL_STATUS_INBOUNDS;
    _player->DuelingWith->m_duelStatus = DUEL_STATUS_INBOUNDS;
    _player->m_duelState = DUEL_STATE_STARTED;
    _player->DuelingWith->m_duelState = DUEL_STATE_STARTED;

    WorldPacket data(SMSG_DUEL_COUNTDOWN, 4);
    data << uint32(3000);
    SendPacket(&data);
    _player->DuelingWith->m_session->SendPacket(&data);

    _player->m_duelCountdownTimer = 3000;
    sEventMgr.AddEvent(_player, &Player::DuelCountdown, EVENT_PLAYER_DUEL_COUNTDOWN, 1000, 3);
}

void WorldSession::HandleDuelCancelled(WorldPacket & recv_data)
{
    if(_player->DuelingWith == 0)
        return;
    _player->DuelingWith->EndDuel(DUEL_WINNER_RETREAT);
}
