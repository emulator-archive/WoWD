/************************************************************************/
/* Copyright (C) 2006 Burlex for WoWd Project                           */
/************************************************************************/

#include "StdAfx.h"

void WorldSession::HandleSetVisibleRankOpcode(WorldPacket& recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 4);
    //TODO: Check if the player can actually _have_ this rank.

    uint32 ChosenRank;
    recv_data >> ChosenRank;

    _player->SetUInt32Value(PLAYER_CHOSEN_TITLE, ChosenRank);
}

void HonorHandler::AddHonorPointsToPlayer(Player *pPlayer, uint32 uAmount)
{
    pPlayer->m_honorPoints += uAmount;
    pPlayer->m_honorToday += uAmount;

    RecalculateHonorFields(pPlayer);
}

void HonorHandler::PerformStartupTasks()
{

}

void HonorHandler::CalculateDailyRankings()
{

}

int32 HonorHandler::CalculateHonorPointsForKill(Player *pPlayer, Unit* pVictim)
{
    // this sucks.. ;p
    if(pVictim != pPlayer)
    {
        int pts = rand() % 100 + 100;
        return pts;
    }
    return 0;
}

void HonorHandler::OnPlayerKilledUnit(Player *pPlayer, Unit* pVictim)
{
    if(!pVictim)
        pVictim = pPlayer;

    if(!pVictim)
        return;

    if(pVictim && !pVictim->IsPlayer())
        return;

    // Calculate points
    int32 Points = CalculateHonorPointsForKill(pPlayer, pVictim);

    if(Points > 0)
    {
        pPlayer->m_killsToday++;
        pPlayer->m_killsLifetime++;
        AddHonorPointsToPlayer(pPlayer, Points);
        // Send PVP credit
        WorldPacket data(SMSG_PVP_CREDIT, 12);
        uint32 pvppoints = Points * 10;
        data << pvppoints << pVictim->GetGUID() << uint32(static_cast<Player*>(pVictim)->GetPVPRank());
        pPlayer->GetSession()->SendPacket(&data);
    }
}

void HonorHandler::RecalculateHonorFields(Player *pPlayer)
{
    // Why are we multiplying by 10.. ho well
    pPlayer->SetUInt32Value(PLAYER_FIELD_KILLS, pPlayer->m_killsToday);
    pPlayer->SetUInt32Value(PLAYER_FIELD_TODAY_CONTRIBUTION, pPlayer->m_honorToday);
    pPlayer->SetUInt32Value(PLAYER_FIELD_YESTERDAY_CONTRIBUTION, pPlayer->m_killsYesterday | ( (pPlayer->m_honorYesterday * 10) << 16));
    pPlayer->SetUInt32Value(PLAYER_FIELD_LIFETIME_HONORBALE_KILLS, pPlayer->m_killsLifetime);
    if(pPlayer->m_honorPoints < 0)
        return;
    pPlayer->SetUInt32Value(PLAYER_FIELD_HONOR_CURRENCY, pPlayer->m_honorPoints);
}

void HonorHandler::DailyFieldMove(Player *pPlayer)
{
    // Move fields
    pPlayer->m_honorYesterday = pPlayer->m_honorToday;
    pPlayer->m_killsYesterday = pPlayer->m_killsToday;
    
    pPlayer->m_killsToday = 0;
    pPlayer->m_honorToday = 0;

    // update fields
    RecalculateHonorFields(pPlayer);
}

bool ChatHandler::HandleAddKillCommand(const char* args, WorldSession* m_session)
{
    uint32 KillAmount = args ? atol(args) : 1;
    Player *plr = getSelectedChar(m_session, true);
    if(plr == 0)
        return true;

    BlueSystemMessage(m_session, "Adding %u kills to player %s.", KillAmount, plr->GetName());
    GreenSystemMessage(plr->GetSession(), "You have had %u honor kills added to your character.", KillAmount);

    for(uint32 i = 0; i < KillAmount; ++i)
        HonorHandler::OnPlayerKilledUnit(plr, 0);

    return true;
}

bool ChatHandler::HandleAddHonorCommand(const char* args, WorldSession* m_session)
{
    uint32 HonorAmount = args ? atol(args) : 1;
    Player *plr = getSelectedChar(m_session, true);
    if(plr == 0)
        return true;

    BlueSystemMessage(m_session, "Adding %u honor to player %s.", HonorAmount, plr->GetName());
    GreenSystemMessage(plr->GetSession(), "You have had %u honor points added to your character.", HonorAmount);

    HonorHandler::AddHonorPointsToPlayer(plr, HonorAmount);
    return true;
}

bool ChatHandler::HandlePVPCreditCommand(const char* args, WorldSession* m_session)
{
    uint32 Rank, Points;
    if(sscanf(args, "%u %u", &Rank, &Points) != 2)
    {
        RedSystemMessage(m_session, "Command must be in format <rank> <points>.");
        return true;
    }
    Points *= 10;
    uint64 Guid = m_session->GetPlayer()->GetSelection();
    if(Guid == 0)
    {
        RedSystemMessage(m_session, "A selection of a unit or player is required.");
        return true;
    }

    BlueSystemMessage(m_session, "Building packet with Rank %u, Points %u, GUID "I64FMT".", 
        Rank, Points, Guid);

    WorldPacket data(SMSG_PVP_CREDIT, 12);
    data << Points << Guid << Rank;
    m_session->SendPacket(&data);
    return true;
}

bool ChatHandler::HandleGlobalHonorDailyMaintenanceCommand(const char* args, WorldSession* m_session)
{
    return false;
}

bool ChatHandler::HandleNextDayCommand(const char* args, WorldSession* m_session)
{
    Player * plr = getSelectedChar(m_session);
    if(plr == 0)
        return true;

    BlueSystemMessage(m_session, "Moving honor fields to tomorrow for player %s.", plr->GetName());
    GreenSystemMessage(m_session, "%s moved your honor fields to tomorrow.", m_session->GetPlayer()->GetName());

    HonorHandler::DailyFieldMove(plr);
    return true;
}