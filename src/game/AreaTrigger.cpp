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

void WorldSession::HandleAreaTriggerOpcode(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 4);
    uint32 id = 0;
    WorldPacket data(80);
    AreaTrigger *pAreaTrigger = NULL;
    recv_data >> id;
    sLog.outDebug("AreaTrigger: %u", id);

    pAreaTrigger = sWorld.GetAreaTrigger(id);

    // Search quest log, find any exploration quests
    sQuestMgr.OnPlayerExploreArea(GetPlayer(),id);
    
    // if in BG handle is triggers
    if(GetPlayer()->m_bgInBattleground && GetPlayer()->GetCurrentBattleground() != NULL)
    {
        GetPlayer()->GetCurrentBattleground()->HandleBattlegroundAreaTrigger(GetPlayer(), id);
        return;
    }   

    if(pAreaTrigger && pAreaTrigger->Type == ATTYPE_BATTLEGROUND)
    {
        if(pAreaTrigger->Mapid == 489)        // hack fix
            pAreaTrigger->Mapid = 2;
        else if(pAreaTrigger->Mapid == 529)
            pAreaTrigger->Mapid = 3;
        else if(pAreaTrigger->Mapid == 30)
            pAreaTrigger->Mapid = 1;
            
        WorldPacket *pkt = sBattlegroundMgr.BuildBattlegroundListPacket(GetPlayer()->GetGUID(), _player,
            pAreaTrigger->Mapid);
        SendPacket(pkt);
        delete pkt;
        return;
    }

    if(pAreaTrigger)
    {
        bool bFailedPre = false;
        std::string failed_reason;

        if(pAreaTrigger->required_level)
        {
            if(GetPlayer()->getLevel() < pAreaTrigger->required_level)
            {
                bFailedPre = true;
                if(failed_reason.size() > 0)
                    failed_reason += ", and ";
                else
                    failed_reason = "You must be ";

                // mm hacky
                char lvltext[30];
                sprintf(lvltext, "at least level %d", pAreaTrigger->required_level);
                failed_reason += lvltext;
            }
        }
        if(bFailedPre)
        {
            failed_reason += " before you're allowed through here.";
            WorldPacket msg;
            msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
            msg << uint32(0) << failed_reason << uint8(0);
            SendPacket(&msg);
            sLog.outDebug("Player %s failed areatrigger prereq - %s", GetPlayer()->GetName(), failed_reason.c_str());
            return;
        }
        switch(pAreaTrigger->Type)
        {
        case ATTYPE_INSTANCE:
            {
                if(GetPlayer()->GetPlayerStatus() != TRANSFER_PENDING) //only ports if player is out of pendings
                {
                    GetPlayer()->SaveEntryPoint();
                    //death system check.
                    Corpse *pCorpse = NULL;
                    CorpseData *pCorpseData = NULL;
                    MapInfo *pMapinfo = NULL;

                    pMapinfo = sWorld.GetMapInformation(pAreaTrigger->Mapid);
                    if(pMapinfo && !pMapinfo->HasFlag(WMI_INSTANCE_ENABLED))
                    {
                        WorldPacket msg;
                        msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
                        msg << uint32(0) << "This instance is currently unavailable." << uint8(0) << uint8(0);
                        SendPacket(&msg);
                        return;
                    }

                    if(pMapinfo && pMapinfo->HasFlag(WMI_INSTANCE_XPACK_01) && !HasFlag(ACCOUNT_FLAG_XPACK_01))
                    {
                        WorldPacket msg;
                        msg.Initialize(SMSG_BROADCAST_MSG);
                        msg << uint32(3) << "You must have The Burning Crusade Expansion to access this content." << uint8(0);
                        SendPacket(&msg);
                        return;
                    }

                    if(!GetPlayer()->isAlive())
                    {
                        pCorpse = objmgr.GetCorpseByOwner(GetPlayer());
                        if(pCorpse)
                        {
                            pMapinfo = sWorld.GetMapInformation(pCorpse->GetMapId());
                            if(pMapinfo)
                            {
                                if(pMapinfo->type != INSTANCE_NULL && pMapinfo->type != INSTANCE_PVP  && pMapinfo->type != INSTANCE_NONRAID && GetPlayer()->GetMapId() != pCorpse->GetMapId() && pCorpse->GetMapId() == pAreaTrigger->Mapid  && !GetPlayer()->InGroup())
                                {
                                    GetPlayer()->ResurrectPlayer();
                                    return;
                                }
                                else if(pMapinfo->type != INSTANCE_NONRAID)
                                {
                                    GetPlayer()->RepopAtGraveyard(GetPlayer()->GetPositionX(),GetPlayer()->GetPositionY(),GetPlayer()->GetPositionZ(), GetPlayer()->GetMapId());
                                    return;
                                }
                            }
                        }
                        else
                        {
                            GetPlayer()->RepopAtGraveyard(GetPlayer()->GetPositionX(),GetPlayer()->GetPositionY(),GetPlayer()->GetPositionZ(),GetPlayer()->GetMapId());
                            return;
                        }
                    }
                    
                    sWorldCreator.CheckInstanceForObject(static_cast<Object*>(_player), pMapinfo);
                    _player->SaveEntryPoint();
                    _player->SafeTeleport(pAreaTrigger->Mapid, 0, LocationVector(pAreaTrigger->x, pAreaTrigger->y, pAreaTrigger->z, pAreaTrigger->o));
                }
            }break;
        case ATTYPE_QUESTTRIGGER:
            {

            }break;
        case ATTYPE_INN:
            {
                // Inn
                if (!GetPlayer()->m_isResting) GetPlayer()->ApplyPlayerRestState(true);
            }break;
        case ATTYPE_TELEPORT:
            {
                if(GetPlayer()->GetPlayerStatus() != TRANSFER_PENDING) //only ports if player is out of pendings
                {
                    _player->SaveEntryPoint();
                    _player->SafeTeleport(pAreaTrigger->Mapid, 0, LocationVector(pAreaTrigger->x, pAreaTrigger->y, pAreaTrigger->z, pAreaTrigger->o));
                }
            }break;
        case ATTYPE_NULL:
            {
                MapInfo *pMapinfo = NULL;
                pMapinfo = sWorld.GetMapInformation(pAreaTrigger->Mapid);
                if(pMapinfo && pMapinfo->HasFlag(WMI_INSTANCE_XPACK_01) && !HasFlag(ACCOUNT_FLAG_XPACK_01))
                {
                    WorldPacket msg;
                    msg.Initialize(SMSG_BROADCAST_MSG);
                    msg << uint32(3) << "You must have The Burning Crusade Expansion to access this content." << uint8(0);
                    SendPacket(&msg);
                    return;
                }
            }
        default:break;
        }
    }
}

void World::LoadAreaTriggerInformation()
{
    QueryResult *result = NULL;
//    uint32 MaxID = 0;
    AreaTrigger *pAT = NULL;

    result = sDatabase.Query("SELECT * FROM areatriggers");

    if(result==NULL)
    {
        sLog.outString("  Query failed.");
        return;
    }
    uint32 total =(uint32) result->GetRowCount();
    int num = 0;

    do
    {
        Field *fields = result->Fetch();
        pAT = new AreaTrigger;

        pAT->AreaTriggerID = fields[0].GetUInt32();
        pAT->Type = (uint8)fields[1].GetUInt32();
        pAT->Mapid = fields[2].GetUInt32();
        pAT->PendingScreen = fields[3].GetUInt32();
        pAT->Name= fields[4].GetString();
        pAT->x = fields[5].GetFloat();
        pAT->y = fields[6].GetFloat();
        pAT->z = fields[7].GetFloat();
        pAT->o = fields[8].GetFloat();
        if(result->GetFieldCount() > 9) // bur: crash fix for lazy bastards who dont update their db >.>
        {
            pAT->required_honor_rank = fields[9].GetUInt32();
            pAT->required_level = fields[10].GetUInt32();
        }

        AddAreaTrigger(pAT);
        ++num;
        if(!(num % 10)) SetProgressBar(num, total, "Area Triggers");
    } while( result->NextRow() );
    delete result;
    ClearProgressBar();
}

AreaTrigger *World::GetAreaTrigger(uint32 id)
{
    AreaTriggerMap::iterator iter, end;
    for( iter = m_AreaTrigger.begin(), end = m_AreaTrigger.end(); iter != end; iter++ )
    {
        AreaTrigger *pAT = iter->second;
        if(pAT->AreaTriggerID == id)
            return pAT;
    }
    return NULL;
}

void World::AddAreaTrigger(AreaTrigger *pArea)
{
    ASSERT( pArea->AreaTriggerID );
    ASSERT( m_AreaTrigger.find(pArea->AreaTriggerID) == m_AreaTrigger.end() );

    m_AreaTrigger[pArea->AreaTriggerID] = pArea;
}

