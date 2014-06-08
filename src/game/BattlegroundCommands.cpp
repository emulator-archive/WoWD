#include "StdAfx.h"

bool ChatHandler::HandleSetBGScoreCommand(const char* args, WorldSession *m_session)
{
    int team = 0;
    int score = 0;
    char *t = strtok((char*)args, " ");
    if(!t) return false;
    char *s = strtok(NULL, "\n");
    if(!s) return false;

    team = atoi(t);
    score = atoi(s);

    uint16 offset = 0x62D + team;

    WorldPacket *data = sBattlegroundMgr.BuildWorldStateUpdatePacket(offset, score);
    // Get BG
    Battleground *bg = m_session->GetPlayer()->GetCurrentBattleground();
    if(bg)
        bg->SendPacketToAll(data);

    delete data;
    return true;
}

bool ChatHandler::HandleStartBGCommand(const char *args, WorldSession *m_session)
{
    Battleground *bg = m_session->GetPlayer()->GetCurrentBattleground();
    if(!bg) return false;

    if(bg->GetMapId() == 489)
    {
        WarsongGulch *wg = static_cast<WarsongGulch*>(bg);
        wg->Start();
    }
    else if(bg->GetMapId() == 529)
    {
        ArathiBasin *ab = static_cast<ArathiBasin*>(bg);
        ab->Start();
    }
    return true;
}

bool ChatHandler::HandlePauseBGCommand(const char *args, WorldSession *m_session)
{
    return true;
}

bool ChatHandler::HandleBGInfoCommnad(const char *args, WorldSession *m_session)
{
    return true;
}

bool ChatHandler::HandleBattlegroundCommand(const char* args, WorldSession *m_session)
{
    Player *plr = getSelectedChar(m_session, false);
    if(!plr) return false;
    
    char *mapid = strtok((char *)args, " ");
    if(!mapid) return false;
    uint32 MapId = atoi(mapid);
    if(MapId == 489)        // OLD SYNTAX CORRECTION
        MapId = 2;
    else if(MapId == 529)
        MapId = 3;
    
    WorldPacket *pkt = sBattlegroundMgr.BuildBattlegroundListPacket(plr->GetGUID(), plr, MapId);
    plr->GetSession()->SendPacket(pkt);
    delete pkt;
    return true;
}

bool ChatHandler::HandleSetWorldStateCommand(const char* args, WorldSession *m_session)
{
    uint32 field = 0;
    uint32 value = 0;
    if(args[1] == 'x' || args[1] == 'X')
    {
        // Is hex..
        char *cfield = strtok((char*)args, " ");
        if(!cfield) return false;
        char *cvalue = strtok(NULL, "\n");
        if(!cvalue) return false;
        sscanf(cfield, "0x%X", &field);
        value = atoi(cvalue);        
    }
    else
    {
        // Is dec..
        char *cfield = strtok((char*)args, " ");
        if(!cfield) return false;
        char *cvalue = strtok(NULL, "\n");
        if(!cvalue) return false;
        field = atoi(cfield);
        value = atoi(cvalue);        
    }
    if(field)
    {
        if(m_session->GetPlayer()->GetCurrentBattleground() != NULL)
            m_session->GetPlayer()->GetCurrentBattleground()->SetWorldStateValue(field, value);
        else
        {
            SystemMessage(m_session, "Not in bg...");
            return false;
        }
    } else {
        SystemMessage(m_session, "Bad Field/Value.");
        return false;
    }
    return true;
}

bool ChatHandler::HandlePlaySoundCommand(const char* args, WorldSession *m_session)
{
    uint32 value = 0;
    if(args[1] == 'x' || args[1] == 'X')
    {
        // Is hex..
        char *cfield = strtok((char*)args, " ");
        if(!cfield) return false;
        sscanf(cfield, "0x%X", &value); 
    }
    else
    {
        // Is dec..
        char *cfield = strtok((char*)args, " ");
        if(!cfield) return false;
        value = atoi(cfield);    
    }

    WorldPacket pkt(SMSG_PLAY_SOUND, 4);
    pkt << value;
    if(value)
    {
        if(m_session->GetPlayer()->GetCurrentBattleground() != NULL)
            m_session->GetPlayer()->GetCurrentBattleground()->SendPacketToAll(&pkt);
        else
        {
            m_session->SendPacket(&pkt);
            return false;
        }
    } else {
        SystemMessage(m_session, "Bad Value.");
        return false;
    }
    return true;
}

bool ChatHandler::HandleSetBattlefieldStatusCommand(const char* args, WorldSession *m_session)
{
    WorldPacket data;
    uint32 mapid = 489;
    uint32 iid = (1 << 8);
    uint32 ssid = 0;
    uint32 unk1 = 0;
    uint32 unk2 = 0;
    uint32 t = static_cast<uint32>(time(NULL));
    uint8 unk3 = 0;

    data.Initialize(SMSG_BATTLEFIELD_STATUS); // SMSG_BATTLEFIELD_STATUS
    data << uint32(unk1);               // Unknown 1
    data << uint32(mapid);              // MapID
    data << uint8(0);                   // Unknown
    data << uint32(iid);                // Instance ID
    data << uint32(ssid);               // Status ID
    data << uint32(t);                  // Time
    if(m_session->GetPlayer()->GetCurrentBattleground() != NULL)
    m_session->GetPlayer()->GetCurrentBattleground()->SendPacketToAll(&data);
    return false;
}

bool ChatHandler::HandleBattlegroundExitCommand(const char* args, WorldSession* m_session)
{
    Player *plr = getSelectedChar(m_session, false);
    if(!plr) return false;
    Battleground *bg = plr->GetCurrentBattleground();
    if(!bg)
    {
        RedSystemMessage(m_session, "Player not in battleground!");
        return true;
    }

    GreenSystemMessage(m_session, "Removed player from battleground.");
    bg->RemovePlayer(plr, true, true, false);
    return true;
}
