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

// Written by DK ask him if you dont understand somewhere
// Ranks by Mangos

#include "StdAfx.h"

void WorldSession::HandleGuildQuery(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 4);
    WorldPacket data;

    Guild *pGuild;
    uint32 guildId;

    recv_data >> guildId;

    pGuild = objmgr.GetGuild( guildId );

    if(!pGuild)
        return;

    pGuild->FillQueryData(&data);
    SendPacket(&data);
}

void WorldSession::HandleCreateGuild(WorldPacket & recv_data)
{
    sLog.outDebug("GUILD CREATE");
    /*    std::string guildName;
    uint64 count;
    int i;
    Player * plyr = GetPlayer();

    if(!plyr)
    return;

    recv_data >> guildName;

    Guild *pGuild = new Guild;

    count = objmgr.GetTotalGuildCount();

    pGuild->SetGuildId( (uint32)(count+1) );
    pGuild->SetGuildName( guildName );
    pGuild->SetGuildRankName("Guild Master", 0);
    pGuild->SetGuildRankName("Officer", 1);
    pGuild->SetGuildRankName("Veteran", 2);  
    pGuild->SetGuildRankName("Member", 3);
    pGuild->SetGuildRankName("Initiate", 4);

    for(i = 5;i < 10;i++)
    pGuild->SetGuildRankName("Unused", i);

    pGuild->SetGuildEmblemStyle( 0xFFFF );
    pGuild->SetGuildEmblemColor( 0xFFFF );
    pGuild->SetGuildBorderStyle( 0xFFFF );
    pGuild->SetGuildBorderColor( 0xFFFF );
    pGuild->SetGuildBackgroundColor( 0xFFFF );

    objmgr.AddGuild(pGuild);

    plyr->SetGuildId( pGuild->GetGuildId() );
    //plyr->SetUInt32Value(PLAYER_GUILDID, pGuild->GetGuildId() );
    plyr->SetGuildRank(GUILDRANK_GUILD_MASTER);
    //plyr->SetUInt32Value(PLAYER_GUILDRANK,GUILDRANK_GUILD_MASTER);
    pGuild->SetGuildLeaderGuid( plyr->GetGUID() );

    pGuild->AddNewGuildMember( plyr );

    pGuild->SaveToDb();*/
}

void WorldSession::SendGuildCommandResult(uint32 typecmd,const char *  str,uint32 cmdresult)
{
    WorldPacket data;
    data.SetOpcode(SMSG_GUILD_COMMAND_RESULT);
    data << typecmd;
    data << str;
    data << cmdresult;
    SendPacket(&data);
}

void WorldSession::HandleInviteToGuild(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 1);
    WorldPacket data;
    std::string inviteeName;

    recv_data >> inviteeName;

    Player *plyr = objmgr.GetPlayer( inviteeName.c_str() , false);
    Player *inviter = GetPlayer();
    Guild *pGuild = objmgr.GetGuild( inviter->GetGuildId() );
     
    
    if(!plyr)
    {
        SendGuildCommandResult(GUILD_INVITE_S,inviteeName.c_str(),GUILD_PLAYER_NOT_FOUND);
        return;
    }
    else if(!pGuild)
    {
        SendGuildCommandResult(GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
        return;
    }
    if( plyr->GetGuildId() )
    {
        SendGuildCommandResult(GUILD_INVITE_S,plyr->GetName(),ALREADY_IN_GUILD);
        return;
    }
    else if( plyr->GetGuildInvitersGuid())
    {
        SendGuildCommandResult(GUILD_INVITE_S,plyr->GetName(),ALREADY_INVITED_TO_GUILD);
        return;
    }
    else if(!pGuild->HasRankRight(inviter->GetGuildRank(),GR_RIGHT_INVITE))
    {
        SendGuildCommandResult(GUILD_INVITE_S,"",GUILD_PERMISSIONS);
        return;
    }
    else if(plyr->GetTeam()!=_player->GetTeam() && _player->GetSession()->GetPermissionCount() == 0)
    {
        SendGuildCommandResult(GUILD_INVITE_S,"",GUILD_NOT_ALLIED);
        return;
    }
    SendGuildCommandResult(GUILD_INVITE_S,inviteeName.c_str(),GUILD_U_HAVE_INVITED);
    //41
  
    data.Initialize(SMSG_GUILD_INVITE);
    data << inviter->GetName();
    data << pGuild->GetGuildName();
    plyr->GetSession()->SendPacket(&data);
    plyr->SetGuildInvitersGuid( inviter->GetGUID() );    
}

void WorldSession::HandleGuildAccept(WorldPacket & recv_data)
{
    Player *plyr = GetPlayer();

    if(!plyr)
        return;

    Player *inviter = objmgr.GetPlayer( plyr->GetGuildInvitersGuid() );
    plyr->UnSetGuildInvitersGuid();

    if(!inviter)
    {
        return;
    }

    Guild *pGuild = objmgr.GetGuild( inviter->GetGuildId() );

    if(!pGuild)
    {
        return;
    }

    plyr->SetGuildId( pGuild->GetGuildId() );
    plyr->SetGuildRank(GUILDRANK_INITIATE);
 
    plyr->UnSetCharterSigned();

    pGuild->AddNewGuildMember( plyr );
    //plyr->SaveGuild();

    WorldPacket data;
    data.Initialize(SMSG_GUILD_EVENT);
    data << uint8(GUILD_EVENT_JOINED);
    data << uint8(1);
    data << plyr->GetName();
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);
}

void WorldSession::HandleGuildDecline(WorldPacket & recv_data)
{
    WorldPacket data;

    Player *plyr = GetPlayer();

    if(!plyr)
        return;

    Player *inviter = objmgr.GetPlayer( (uint64)(plyr->GetGuildInvitersGuid()) );
    plyr->UnSetGuildInvitersGuid(); 

    if(!inviter)
        return;

    data.Initialize(SMSG_GUILD_DECLINE);
    data << plyr->GetName();
    inviter->GetSession()->SendPacket(&data);
}
void WorldSession::HandleSetGuildInformation(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 1);
    WorldPacket data;
    std::string NewGuildInfo;
    recv_data >> NewGuildInfo;

    Guild *pGuild = objmgr.GetGuild( GetPlayer()->GetGuildId() );

    if(!pGuild)
        return;

    uint32 plyrRank = GetPlayer()->GetGuildRank();

    if(!pGuild->HasRankRight(plyrRank,GR_RIGHT_EGUILDINFO))
    {
        SendGuildCommandResult(3,"",GUILD_PERMISSIONS);
         return;
    }

    pGuild->SetGuildInfo(NewGuildInfo);

    pGuild->FillGuildRosterData(&data);
    GetPlayer()->GetSession()->SendPacket(&data);

    pGuild->UpdateGuildToDb();
}

void WorldSession::HandleGuildInfo(WorldPacket & recv_data)
{
    WorldPacket data;

    Guild *pGuild = objmgr.GetGuild( GetPlayer()->GetGuildId() );

    if(!pGuild)
        return;

    data.Initialize(SMSG_GUILD_INFO);//not sure
    data << pGuild->GetGuildName();
    data << pGuild->GetCreatedYear();
    data << pGuild->GetCreatedMonth();
    data << pGuild->GetCreatedDay();
    data << pGuild->GetGuildMembersCount();
    data << pGuild->GetGuildMembersCount();//accountcount

    SendPacket(&data);
}

void WorldSession::HandleGuildRoster(WorldPacket & recv_data)
{
    WorldPacket data;

    Guild *pGuild = objmgr.GetGuild( GetPlayer()->GetGuildId() );

    if(!pGuild)
        return;

    pGuild->FillGuildRosterData(&data);
    GetPlayer()->GetSession()->SendPacket(&data);
}

void WorldSession::HandleGuildPromote(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 1);
    std::string name;
    uint32 plyrRank;
    uint32 pTargetRank;

    recv_data >> name;

    Player *plyr = GetPlayer();

    if(!plyr)
        return;

    Guild *pGuild = objmgr.GetGuild( plyr->GetGuildId() );

    if(!pGuild)
        return;

    PlayerInfo *pGuildMember = pGuild->GetGuildMember(name);

    if(!pGuildMember)
    {
         SendGuildCommandResult(GUILD_FOUNDER_S,name.c_str(),GUILD_PLAYER_NOT_IN_GUILD_S);
        return;
    }

    plyrRank = plyr->GetGuildRank();
    pTargetRank = pGuildMember->Rank;

    if(!pGuild->HasRankRight(plyrRank,GR_RIGHT_PROMOTE))
    {
         SendGuildCommandResult(3,"",GUILD_PERMISSIONS);
        return;
    }

    if( plyr->GetGUID() == pGuildMember->guid )
    {
        sChatHandler.SystemMessage(this, "You cant promote yourself!");
        return;
    }

    if( plyrRank >= pTargetRank )
    {
        sChatHandler.SystemMessage(this, "You must be a higher ranking guild member than the person you are going to promote!");
        return;
    }

    if( (pTargetRank - plyrRank) == 1 )
    {
        sChatHandler.SystemMessage(this, "You can only promote up to one rank below yours!");
        return;
    }          

    pTargetRank--;

    if(pTargetRank < GUILDRANK_GUILD_MASTER)
        pTargetRank = GUILDRANK_GUILD_MASTER;

    pGuildMember->Rank = pTargetRank;

    Player *pTarget = objmgr.GetPlayer( name.c_str() );
    if(pTarget)
    {
        //They Online
        pTarget->SetGuildRank(pTargetRank);
    //    pTarget->SetUInt32Value(PLAYER_GUILDRANK, pTargetRank);
    }

    //Save new Info to DB
    pGuild->UpdateGuildMembersDB(pGuildMember);

    //check if its unused rank or not

    WorldPacket data(SMSG_GUILD_EVENT, 100);
    data << uint8(GUILD_EVENT_PROMOTION);
    data << uint8(3);
    data << plyr->GetName();
    data << name.c_str();
    data << pGuild->GetRankName( pTargetRank );
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);
}

void WorldSession::HandleGuildDemote(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 1);
    std::string name;
    uint32 plyrRank;
    uint32 pTargetRank;

    recv_data >> name;

    Player *plyr = GetPlayer();

    if(!plyr)
        return;

    Guild *pGuild = objmgr.GetGuild( plyr->GetGuildId() );

    if(!pGuild)
        return;

    PlayerInfo *pGuildMember = pGuild->GetGuildMember(name);

    if(!pGuildMember)
    {
        SendGuildCommandResult(GUILD_FOUNDER_S,name.c_str(),GUILD_PLAYER_NOT_IN_GUILD_S);
        return;
    }

    plyrRank = plyr->GetGuildRank();
    pTargetRank = pGuildMember->Rank;

    if(!pGuild->HasRankRight(plyrRank,GR_RIGHT_DEMOTE))
    {
         SendGuildCommandResult(3,"",GUILD_PERMISSIONS);
        return;
    }

    if( plyr->GetGUID() == pGuildMember->guid )
    {
        sChatHandler.SystemMessage(this, "You cant demote yourself!");
        return;
    }

    if( plyrRank >= pTargetRank )
    {
        sChatHandler.SystemMessage(this, "You must be a higher ranking guild member than the person you are going to demote!");
        return;
    }

    if(pTargetRank == pGuild->GetNrRanks()-1)
    {
        sChatHandler.SystemMessage(this, "You cannot demote that member any further!");
        return;
    }

    pTargetRank++;
    if(pTargetRank > pGuild->GetNrRanks()-1)
        pTargetRank = pGuild->GetNrRanks()-1;

    pGuildMember->Rank = pTargetRank;

    Player *pTarget = objmgr.GetPlayer( name.c_str() );
    if(pTarget)
    {
        //They Online
        pTarget->SetGuildRank(pTargetRank);
        //pTarget->SetUInt32Value(PLAYER_GUILDRANK, pTargetRank);
    }

    //Save new Info to DB
    pGuild->UpdateGuildMembersDB(pGuildMember);

    WorldPacket data(SMSG_GUILD_EVENT, 100);
    data.Initialize(SMSG_GUILD_EVENT);
    data << uint8(GUILD_EVENT_DEMOTION);
    data << uint8(3);
    data << plyr->GetName();
    data << name.c_str();
    data << pGuild->GetRankName( pTargetRank );
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);
}

void WorldSession::HandleGuildLeave(WorldPacket & recv_data)
{
    Player *plyr = GetPlayer();

    if(!plyr)
        return;

    Guild *pGuild = objmgr.GetGuild( plyr->GetGuildId() );

    if(!pGuild)
        return;

    if( pGuild->GetGuildLeaderGuid() == plyr->GetGUID() )
        return;

    plyr->SetGuildId(0);
    plyr->SetGuildRank(0);
    pGuild->DeleteGuildMember(plyr->GetGUID());

    WorldPacket data(100);
    data.Initialize(SMSG_GUILD_EVENT);
    data << uint8(GUILD_EVENT_LEFT);
    data << uint8(1);
    data << plyr->GetName();
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);
}

void WorldSession::HandleGuildRemove(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 1);
    WorldPacket data(100);
    std::string name;
    bool result = false;

    recv_data >> name;
    Player *plyr = objmgr.GetPlayer( name.c_str() );
    Player *pRemover = GetPlayer();

    if(!pRemover)
        return;

    Guild *pGuild = objmgr.GetGuild( pRemover->GetGuildId() );

    if(!pGuild)
        return;

    uint32 RemoverRank = pRemover->GetGuildRank();

    if(!pGuild->HasRankRight(RemoverRank,GR_RIGHT_REMOVE))
    {
        //Players not allowed to remove a guild member
         SendGuildCommandResult(3,"",GUILD_PERMISSIONS);
        return;
    }

    if(plyr)
    {
        plyr->SetGuildId(0);
        plyr->SetGuildRank(0);
        result = pGuild->DeleteGuildMember(plyr->GetGUID());
    }
    else
    {
        result = pGuild->DeleteGuildMember(name);
    }

    if(result)
    {
        data.Initialize(SMSG_GUILD_EVENT);
        data << uint8(GUILD_EVENT_REMOVED);
        data << uint8(2);
        data << name.c_str();
        data << pRemover->GetName();
        pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);
    }
}

void WorldSession::HandleGuildDisband(WorldPacket & recv_data)
{
    Player *pLeader = GetPlayer();

    if(!pLeader)
        return;

    Guild *pGuild = objmgr.GetGuild( pLeader->GetGuildId() );

    if(!pGuild)
        return;

    if(pLeader->GetGUID() != pGuild->GetGuildLeaderGuid())
        return;

    pGuild->DeleteGuildMembers();
    pGuild->RemoveFromDb();
}

void WorldSession::HandleGuildLeader(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 1);
    std::string name;
    recv_data >> name;

    Player *pLeader = GetPlayer();

    if(!pLeader)
        return;

    Guild *pGuild = objmgr.GetGuild( pLeader->GetGuildId() );

    if(!pGuild)
        return;

    PlayerInfo *pGuildMember = pGuild->GetGuildMember(name);
    if(!pGuildMember)
        return;

    if(pLeader->GetGUID() == pGuildMember->guid)
        return;

    if(pLeader->GetGUID() != pGuild->GetGuildLeaderGuid())
        return;

    PlayerInfo *pGuildLeader = pGuild->GetGuildMember(pLeader->GetGUID());
    if(!pGuildLeader)
        return;

    pGuildLeader->Rank = GUILDRANK_OFFICER;
    pLeader->SetGuildRank(GUILDRANK_OFFICER);  

    pGuildMember->Rank = GUILDRANK_GUILD_MASTER;
    Player *pNewLeader = objmgr.GetPlayer( name.c_str() );
    if(pNewLeader)
    {
  //      pNewLeader->SetUInt32Value(PLAYER_GUILDRANK,GUILDRANK_GUILD_MASTER);
        pNewLeader->SetGuildRank(GUILDRANK_GUILD_MASTER);
       // pNewLeader->SaveGuild();
    }
    else
    {
        pGuild->UpdateGuildMembersDB(pGuildMember);
    }

    pGuild->SetGuildLeaderGuid( pGuildMember->guid );
    pGuild->UpdateGuildToDb();

    WorldPacket data;
    data.Initialize(SMSG_GUILD_EVENT);
    data << (uint8)GUILD_EVENT_LEADER_CHANGED;
    data << (uint8)2;
    data << pLeader->GetName();
    data << pGuildMember->name.c_str();
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);

    /*
    //demote old guildleader
    data.Initialize(SMSG_GUILD_EVENT);
    data << uint8(GUILD_EVENT_DEMOTION);
    data << uint8(3);
    data << pLeader->GetName();
    data << pLeader->GetName();
    data << pGuild->GetRankName( GUILDRANK_OFFICER );
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);

    //promote new guildmaster
    data.Initialize(SMSG_GUILD_EVENT);
    data << uint8(GUILD_EVENT_PROMOTION);
    data << uint8(3);
    data << pLeader->GetName();
    data << pNewLeader->GetName();
    data << pGuild->GetRankName( GUILDRANK_GUILD_MASTER );
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);
    */
    //data.clear();
    //pGuild->FillGuildRosterData(&data);
    //pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);
}

void WorldSession::HandleGuildMotd(WorldPacket & recv_data)
{
    WorldPacket data;
    std::string motd;
    if(recv_data.size())
        recv_data >> motd;

    Guild *pGuild = objmgr.GetGuild( GetPlayer()->GetGuildId() );

    if(!pGuild)
        return;

    pGuild->SetGuildMotd(motd);

    data.SetOpcode(SMSG_GUILD_EVENT);
    data << uint8(GUILD_EVENT_MOTD);
    data << uint8(0x01);
    data << pGuild->GetGuildMotd();
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);

    pGuild->UpdateGuildToDb();
}

void WorldSession::HandleGuildRank(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 9);
    WorldPacket data;
    Guild *pGuild;
    std::string rankname;
    uint32 rankId;
    uint32 rights;

    pGuild = objmgr.GetGuild(GetPlayer()->GetGuildId());
    if(!pGuild)
    {
        SendGuildCommandResult(GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
        return;
    }

    if(GetPlayer()->GetGUID() != pGuild->GetGuildLeaderGuid())
    {
        SendGuildCommandResult(GUILD_INVITE_S,"",GUILD_PERMISSIONS);
        return;
    }

    recv_data >> rankId;
    recv_data >> rights;
    recv_data >> rankname;

    sLog.outDebug( "WORLD: Changed RankName to %s , Rights to 0x%.4X",rankname.c_str() ,rights );

    pGuild->SetRankName(rankId,rankname);
    pGuild->SetRankRights(rankId,rights);

    pGuild->FillQueryData(&data);
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);

    data.clear();
    pGuild->FillGuildRosterData(&data);
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);

    pGuild->SaveRanksToDb();
}

void WorldSession::HandleGuildAddRank(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 1);
    WorldPacket data;
    Guild *pGuild;
    std::string rankname;

    pGuild = objmgr.GetGuild(GetPlayer()->GetGuildId());
    if(!pGuild)
    {
        //not in Guild
        SendGuildCommandResult(GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
        return;
    }

    if(GetPlayer()->GetGUID() != pGuild->GetGuildLeaderGuid())
    {
        //not GuildMaster
        SendGuildCommandResult(GUILD_INVITE_S,"",GUILD_PERMISSIONS);
        return;
    }

    recv_data >> rankname;

    pGuild->CreateRank(rankname,GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);

    pGuild->FillQueryData(&data);
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);

    data.clear();
    //Maybe theres an event for this
    pGuild->FillGuildRosterData(&data);
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);

    pGuild->SaveRanksToDb();
}

void WorldSession::HandleGuildDelRank(WorldPacket & recv_data)
{
    WorldPacket data;
    Guild *pGuild;

    pGuild = objmgr.GetGuild(GetPlayer()->GetGuildId());    
    if(!pGuild)
    {
        SendGuildCommandResult(GUILD_CREATE_S,"",GUILD_PLAYER_NOT_IN_GUILD);
        return;
    }

    if(GetPlayer()->GetGUID() != pGuild->GetGuildLeaderGuid())
    {
        SendGuildCommandResult(GUILD_INVITE_S,"",GUILD_PERMISSIONS);
        return;
    }

    pGuild->DelRank();

    pGuild->FillQueryData(&data);
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);
    data.clear();

    //Maybe theres an event for this
    pGuild->FillGuildRosterData(&data);
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);

    pGuild->SaveRanksToDb();
}

void WorldSession::HandleGuildSetPublicNote(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 2);
    WorldPacket data;
    std::string TargetsName;
    std::string publicNote;

    recv_data >> TargetsName;
    recv_data >> publicNote;

    Player *pPlayer = GetPlayer();

    if(!pPlayer)
        return;

    Guild *pGuild = objmgr.GetGuild( pPlayer->GetGuildId() );

    if(!pGuild)
        return;

    PlayerInfo *pGuildMember = pGuild->GetGuildMember(TargetsName);

    if(!pGuildMember)
    {
        SendGuildCommandResult(GUILD_FOUNDER_S,TargetsName.c_str(),GUILD_PLAYER_NOT_IN_GUILD_S);
        return;
    }

    uint32 plyrRank = pPlayer->GetGuildRank();

    if(!pGuild->HasRankRight(plyrRank,GR_RIGHT_EPNOTE))
    {
        SendGuildCommandResult(3,"",GUILD_PERMISSIONS);
        return;
    }

    pGuild->SetPublicNote(pGuildMember->guid, publicNote);

    //Save new Info to DB
    pGuild->UpdateGuildMembersDB(pGuildMember);

    //Send Update (this seems to be how blizz does it couldn't find an event for it)
    pGuild->FillGuildRosterData(&data);
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);
}

void WorldSession::HandleGuildSetOfficerNote(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 2);
    WorldPacket data;
    std::string TargetsName;
    std::string officerNote;

    recv_data >> TargetsName;
    recv_data >> officerNote;

    Player *pPlayer = GetPlayer();


    if(!pPlayer)
        return;

    Guild *pGuild = objmgr.GetGuild( pPlayer->GetGuildId() );

    if(!pGuild)
        return;

    PlayerInfo *pGuildMember = pGuild->GetGuildMember(TargetsName);

    if(!pGuildMember)
    {
        SendGuildCommandResult(GUILD_FOUNDER_S, TargetsName.c_str(),GUILD_PLAYER_NOT_IN_GUILD_S);
        return;
    }

    uint32 plyrRank = pPlayer->GetGuildRank();

    if(!pGuild->HasRankRight(plyrRank,GR_RIGHT_EOFFNOTE))
    {
        SendGuildCommandResult(3,"",GUILD_PERMISSIONS);
        return;
    }

    pGuild->SetOfficerNote(pGuildMember->guid, officerNote);

    //Save new Info to DB
    pGuild->UpdateGuildMembersDB(pGuildMember);

    //Send Update (this seems to be how blizz does it couldn't find an event for it)
    pGuild->FillGuildRosterData(&data);
    pGuild->SendMessageToGuild(0, &data, G_MSGTYPE_ALL);
}

void WorldSession::HandleSaveGuildEmblem(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 20);
    WorldPacket data;

    Guild *pGuild = objmgr.GetGuild( GetPlayer()->GetGuildId() );

    if(!pGuild)
    {
        data.Initialize(MSG_SAVE_GUILD_EMBLEM);
        data <<    uint32(ERR_GUILDEMBLEM_NOGUILD);
        SendPacket(&data);
        return;
    }

    Player *plyr = GetPlayer();

    if(!plyr)
        return;

    if(plyr->GetGuildRank() != GUILDRANK_GUILD_MASTER)
    {
        data.Initialize(MSG_SAVE_GUILD_EMBLEM);
        data <<    uint32(ERR_GUILDEMBLEM_NOTGUILDMASTER);
        SendPacket(&data);
        return;
    }
    uint32 Money = plyr->GetUInt32Value(PLAYER_FIELD_COINAGE);
    uint32 Cost = 100000; //10 Gold
    if(Money < Cost)
    {
        data.Initialize(MSG_SAVE_GUILD_EMBLEM);
        data << uint32(ERR_GUILDEMBLEM_NOTENOUGHMONEY);
        SendPacket(&data);
        return;
    }
    plyr->SetUInt32Value(PLAYER_FIELD_COINAGE,(Money-Cost));

    uint64 DesignerGuid;
    recv_data >> DesignerGuid;

    uint32 emblemPart;
    recv_data >> emblemPart;
    pGuild->SetGuildEmblemStyle( emblemPart );
    recv_data >> emblemPart;
    pGuild->SetGuildEmblemColor( emblemPart );
    recv_data >> emblemPart;
    pGuild->SetGuildBorderStyle( emblemPart );
    recv_data >> emblemPart;
    pGuild->SetGuildBorderColor( emblemPart );
    recv_data >> emblemPart;
    pGuild->SetGuildBackgroundColor( emblemPart );

    data.Initialize(MSG_SAVE_GUILD_EMBLEM);
    data <<    uint32(ERR_GUILDEMBLEM_SUCCESS);
    SendPacket(&data);

    data.clear();
    pGuild->FillQueryData(&data);
    pGuild->SendMessageToGuild(plyr->GetGUID(), &data, G_MSGTYPE_ALL);

    //data.Initialize(SMSG_GUILD_EVENT);
    //data << uint8(GUILD_EVENT_TABARDCHANGE);
    //data << uint8(0x00);
    //data << pGuild->GetGuildMotd();
    //pGuild->SendMessageToGuild(NULL, &data, G_MSGTYPE_ALL);

    pGuild->UpdateGuildToDb();
}

// Charter part
void WorldSession::HandleCharterBuy(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 25);
    /*   Opcode: CMSG_PETITION_BUY (0x01BD)
    guid npc
    uint32 unk 0
    uint32 unk 0
    uint32 unk 0
    string guild name
    uint32 unk 0
    uint32 unk 0
    uint32 unk 0
    uint32 unk 0
    uint32 unk 0
    uint32 unk 0
    uint32 unk 0
    uint32 unk 0
    uint32 unk 0
    uint32 unk 0
    uint32 unk 0
    uint32 unk 0
    uint32 unk 1*/
 
    uint32 npcGuid;
    uint32 temp;
    uint32 unk;
    uint64 itemGuid;
    uint8 error;
    std::string guildName;

    WorldPacket data;
    recv_data >> npcGuid;
    recv_data >> unk;
    recv_data >> temp;
    recv_data >> temp;
    recv_data >> temp;
    recv_data >> guildName;

    Player *plyr = GetPlayer();

    if(!plyr)
        return;
    if(!_player->CanSignCharter())
        return;
    //Dont buy secondtime    
    
    //Money 10 silver
    int32 curMoney = plyr->GetUInt32Value(PLAYER_FIELD_COINAGE)-1000;
    if(curMoney < 0)
    {
        data.Initialize( SMSG_BUY_FAILED );
        data << uint64(0);
        data << uint32(5863);
        data << uint8(2); //not enough money
        SendPacket( &data );
        return;
    }    

    ItemPrototype *proto = objmgr.GetItemPrototype(5863);
    if(error = _player->GetItemInterface()->CanReceiveItem(proto, 1))
    {
        _player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, error);
        return;
    }

    Item *item = objmgr.CreateItem(5863, plyr);
    itemGuid = item->GetGUIDLow();

    item->SetUInt32Value(ITEM_FIELD_FLAGS, 0x00000001);
    //item->Enchant(false,128887,0);
    EnchantEntry * enchant = sEnchantStore.LookupEntry(128887);
    if(enchant && enchant->Id == 128887)
    {
        item->AddEnchantment(enchant, 0, true,false);
    }
    else
    {
        EnchantEntry *pEnchant = new EnchantEntry;
        memset(pEnchant,0,sizeof(EnchantEntry));

        pEnchant->Id = 128887;
        item->AddEnchantment(pEnchant,0,true,true, false);
    }
    item->SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, 962119440);
    
    if(!plyr->GetItemInterface()->AddItemToFreeSlot(item))
    {
        plyr->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_INVENTORY_FULL);
        delete item;
        return;
    }
    plyr->SetUInt32Value(PLAYER_FIELD_COINAGE, curMoney);

    Charter *gc = new Charter;
    gc->itemGuid = itemGuid;
    gc->guildName = guildName;
    gc->leaderGuid = plyr->GetGUID();

    gc->signCount = 0;
    memset(gc->signer,0,sizeof(uint64)*9);
    objmgr.AddCharter(gc);
    plyr->SetCharterSigned();

    data.Initialize( SMSG_BUY_ITEM );
    data << uint64(npcGuid);
    data << uint32(1) << int32(-3);
    SendPacket(&data);
}

void WorldSession::HandleCharterShowSignatures(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 8);
    /*    Opcode: CMSG_PETITION_SHOW_SIGNATURES (0x01BE)
    guid item

    Opcode: SMSG_PETITION_SHOW_SIGNATURES (0x01BF)
    guid item
    uint64 player guid
    uint32 Used for wdb (GuildLow)
    uint8 0*/

    WorldPacket data;
    uint64 itemGuid;

    recv_data >> itemGuid;

    Player *plyr = GetPlayer();

    if(!plyr)
        return;

    Charter *gc = objmgr.GetGuildCharterByCharterGuid( GUID_LOPART(itemGuid) );

    if(!gc)
    {
        sLog.outDebug("Charter Error");
        return;
    }

    data.SetOpcode(SMSG_PETITION_SHOW_SIGNATURES);
    data << itemGuid;
    data << plyr->GetGUID();
    data << uint32(GUID_LOPART(itemGuid));
    data << (uint8)gc->signCount;// signer count
    for(uint32 x=0;x<gc->signCount;x++)
    {
        data << gc->signer[x];
        data << uint32(1);//unk
    }
    SendPacket(&data);
}

void WorldSession::HandleCharterQuery(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 12);
    /*   Opcode: CMSG_PETITION_QUERY (0x01C6)
    uint32 20 2B 00 00 unk3 //maybe item thing
    guid item?

    Opcode: SMSG_PETITION_QUERY_RESPONSE
    uint32 20 2B 00 00 unk3
    guid player // does he signed answer
    string guild name
    uint8 0
    uint32 1
    uint32 9
    uint32 9
    9 * uint32 0
    2 * uint8 0*/
    WorldPacket data;
    uint64 itemGuid;
    uint32 charterId;
    int i = 0;

    recv_data >> charterId;
    recv_data >> itemGuid;

    Player *plyr = GetPlayer();

    if(!plyr)
        return;

    Charter *gc = objmgr.GetGuildCharterByCharterGuid( GUID_LOPART(itemGuid) );

    if(!gc)
    {
        sLog.outDebug("Charter Error");
        return;
    }

    /*
    [!] SERVER->CLIENT pkt 0x01C7 [len 0x0053]
    OFFSET  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | 0123456789ABCDEF
    --------------------------------------------------------------------------
    000010  2C 27 01 00 AB 97 3C 00 00 00 00 00 42 72 69 6E   ,'....<.....Brin  
    000020  67 65 72 73 20 6F 66 20 41 6E 61 72 63 68 79 00   gers of Anarchy.  
    000030  00 01 00 00 00 09 00 00 00 09 00 00 00 00 00 00   ................  
    000040  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................  
    000050  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................  
    000060  00 00 00                                          ...  
    */
    data.Initialize( SMSG_PETITION_QUERY_RESPONSE );
    data << uint32(charterId);
    data << gc->leaderGuid;
    data << gc->guildName;
    data << uint8(0);
    data << uint32(9);
    data << uint32(9);
    for(i=0;i < 9;i++)
        data << uint32(0);
    data << uint8(0);
    data << uint8(0);
    SendPacket(&data);
}

void WorldSession::HandleCharterOffer( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 20);
    /*
    Opcode: CMSG_OFFER_PETITION (0x01C3)
    guid 1 item
    guid 2 target

    [!] CLIENT->SERVER pkt 0x01C3 [len 0x0010]
    OFFSET  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | 0123456789ABCDEF
    --------------------------------------------------------------------------
    000010  B4 A2 11 4A ED 02 00 40 7C 4F 05 01 00 00 00 00   ...J...@|O...... 
    */
    /*
    Opcode: MSG_PETITION_DECLINE (0x01C2)
    guid 2 target

    [!] SERVER->CLIENT pkt 0x01C2 [len 0x0008]
    OFFSET  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | 0123456789ABCDEF
    --------------------------------------------------------------------------
    000010  7C 4F 05 01 00 00 00 00                           |O...... 
    */
    /*
    Opcode: SMSG_PETITION_SIGN_RESULTS (0x01C1)
    guid 1 item
    guid 3 target
    uint32 0

    [!] SERVER->CLIENT pkt 0x01C1 [len 0x0014]
    OFFSET  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | 0123456789ABCDEF
    --------------------------------------------------------------------------
    000010  B4 A2 11 4A ED 02 00 40 7D 4F 05 01 00 00 00 00   ...J...@}O......  
    000020  00 00 00 00                                       .... 

    00 00 00 00 72 00 00 00 | 05 00 00 00 10 00 00 00
    00 00 00 00
    */

    WorldPacket data;
    uint64 itemGuid;
    uint64 targetGuid;
    //change this to 2B20 to prevent sign from same machine
    uint32 CharterType;
    recv_data >> CharterType; // guessed this function as with the 2v2 3v3 5v5 arena charters
    recv_data >> itemGuid;
    recv_data >> targetGuid;

    Player *plyr = GetPlayer();

    if(!plyr)
        return;

    Player *pTarget = objmgr.GetPlayer( targetGuid );

    if(!pTarget)
        return;
    
    if(pTarget->GetTeam()!=_player->GetTeam())
        return;

    Charter *gc = objmgr.GetGuildCharterByCharterGuid( GUID_LOPART(itemGuid) );
    if(!gc)
    {
        sLog.outDebug("Charter Error");
        return;
    }

    data.Initialize(SMSG_PETITION_SHOW_SIGNATURES);
    data << itemGuid;
    data << plyr->GetGUID();
    data << uint32(GUID_LOPART(itemGuid));
    data << (uint8)gc->signCount;// signer count
    for(uint32 x=0;x<gc->signCount;x++)
    {
        data << gc->signer[x];
        data << uint32(1);
    
    }

    pTarget->GetSession()->SendPacket( &data );
}

void WorldSession::HandleCharterSign( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 9);
    /*  Opcode: CMSG_PETITION_SIGN (0x01C0)
    item guid
    uint8 1

    Opcode: SMSG_PETITION_SIGN_RESULTS (0x01C1)
    item guid
    target guid yeah target guid
    uint32 0*/

    WorldPacket data;
    uint64 itemGuid;
    uint8 unk;

    recv_data >> itemGuid;
    recv_data >> unk;

    Player *plyr = GetPlayer();

    if(!plyr)
        return;

    Charter *gc = objmgr.GetGuildCharterByCharterGuid( GUID_LOPART(itemGuid) );
    if(!gc)
    {
        sLog.outDebug("Charter Error");
        return;
    }
    if(gc->signCount>=9)
        return;
    Player *pLeader = objmgr.GetPlayer( gc->leaderGuid );

    if(!pLeader)
        return;
    //already signed on is in guild
    if(!_player->CanSignCharter())
        return;

    plyr->SetCharterSigned(); 
    
    gc->signer[gc->signCount++]=_player->GetGUID();
    objmgr.SaveCharter(gc);

    data.Initialize(SMSG_PETITION_SIGN_RESULTS);
    data << itemGuid;
    data << plyr->GetGUID();
    data << uint32(0);
    SendPacket( &data );
    pLeader->GetSession()->SendPacket( &data );
}

void WorldSession::HandleCharterTurnInCharter(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 8);
    /*    Opcode: CMSG_TURN_IN_PETITION (0x01C4)
    guid item

    Opcode: SMSG_TURN_IN_PETITION_RESULTS (0x01C5)
    unk uint32 4*/
    WorldPacket data;
    uint64 itemGuid;
    uint32 guildId = 0;

    Player *plyr = GetPlayer();
    if(!plyr)
        return;

    recv_data >> itemGuid;

    Charter *gc = objmgr.GetGuildCharterByCharterGuid( GUID_LOPART(itemGuid) );

    if(!gc)
    {
        sLog.outDebug("Charter I64FMT doesn't exist", itemGuid);
        return;
    }

    if(plyr->GetGUID() != gc->leaderGuid)
    {
        data.Initialize(SMSG_TURN_IN_PETITION_RESULTS);
        data << uint32(ERR_PETITION_CREATOR);
        SendPacket( &data );
        return;

        /*
        data.Initialize(SMSG_GUILD_COMMAND_RESULT);
        data << uint32(0);
        data << gc->guildName;
        data << uint32(C_R_DONT_HAVE_PERMISSION);
        SendPacket(&data);
        return;
        */
    }

    if(gc->signCount != 9)
    {
        data.Initialize(SMSG_TURN_IN_PETITION_RESULTS);
        data << uint32(ERR_PETITION_NOT_ENOUGH_SIGNATURES);
        SendPacket( &data );
        return;
    }

    if(plyr->IsInGuild())
    {
        data.Initialize(SMSG_TURN_IN_PETITION_RESULTS);
        data << uint32(ERR_PETITION_IN_GUILD);
        SendPacket( &data );
        return;
    }

    if(objmgr.GetGuildByGuildName(gc->guildName))
    {
        //SendGuildCommandResult(GUILD_CREATE_S,gc->guildName.c_str(),GUILD_NAME_EXISTS);
        return;
    }

    // dont know hacky or not but only solution for now
    // If everything is fine create guild

    Guild *pGuild = new Guild;

    guildId = pGuild->GetFreeGuildIdFromDb();

    if(guildId == 0)
    {
        printf("Error Getting Free Guild ID");
        delete pGuild;
        return;
    }

    //Guild Setup
    pGuild->SetGuildId( guildId );
    pGuild->SetGuildName( gc->guildName );
    pGuild->CreateRank("Guild Master", GR_RIGHT_ALL);
    pGuild->CreateRank("Officer", GR_RIGHT_ALL);
    pGuild->CreateRank("Veteran", GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);  
    pGuild->CreateRank("Member", GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);
    pGuild->CreateRank("Initiate", GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);
    pGuild->SetGuildEmblemStyle( 0xFFFF );
    pGuild->SetGuildEmblemColor( 0xFFFF );
    pGuild->SetGuildBorderStyle( 0xFFFF );
    pGuild->SetGuildBorderColor( 0xFFFF );
    pGuild->SetGuildBackgroundColor( 0xFFFF );

    objmgr.AddGuild(pGuild);

    //Guild Leader Setup
    plyr->SetGuildId( pGuild->GetGuildId() );
    //plyr->SetUInt32Value(PLAYER_GUILDID, pGuild->GetGuildId() );
    plyr->SetGuildRank(GUILDRANK_GUILD_MASTER);
    //plyr->SetUInt32Value(PLAYER_GUILDRANK,GUILDRANK_GUILD_MASTER);
    pGuild->SetGuildLeaderGuid( plyr->GetGUID() );

    pGuild->AddNewGuildMember( plyr );

    //Other Guild Members Setup
    Player *pMember;  

      for(uint32 x=0;x<9;x++)
    {       
        pMember = objmgr.GetPlayer(gc->signer[x]);
        if(pMember)//online
        {
            pMember->SetGuildId( pGuild->GetGuildId() );
            pMember->SetGuildRank(GUILDRANK_MEMBER);        
            //Charters
            pMember->UnSetCharterSigned();
        }

        PlayerInfo *p=objmgr.GetPlayerInfo(gc->signer[x]);
        if(!p)continue;//this should not ever happen though
        p->Rank = GUILDRANK_MEMBER;
        pGuild->AddGuildMember(p);            
    }    

    //We finished with charter
    objmgr.DeleteCharter(gc->leaderGuid);
    plyr->UnSetCharterSigned();

    //Deleting Charter from inventory
    bool result = plyr->GetItemInterface()->SafeFullRemoveItemByGuid(itemGuid);
    if(!result)
    {
        printf("HandleCharterTurnIn: Item Destruction failed");
    }

    //we deleted item so we dont want character corrupted!
    plyr->SaveToDB();

    pGuild->SaveToDb();
    pGuild->SaveAllGuildMembersToDb();
    pGuild->SaveRanksToDb();

    data.Initialize(SMSG_TURN_IN_PETITION_RESULTS);
    data << uint32(0);
    SendPacket( &data );

    /*
    data.Initialize(SMSG_GUILD_COMMAND_RESULT);
    data << uint32(0);
    data << gc->guildName;
    data << uint32(C_R_CREATED);
    SendPacket(&data);    */
}

void WorldSession::HandleCharterRename(WorldPacket & recv_data)
{
    CHECK_PACKET_SIZE(recv_data, 9);
    /*MSG_PETITION_RENAME
    guid item
    string guildname*/
    WorldPacket data;
    uint64 itemGuid;
    std::string newGuildName;
    int i = 0;

    Player *plyr = GetPlayer();

    if(!plyr)
        return;

    recv_data >> itemGuid;
    recv_data >> newGuildName;

    Charter *gc = objmgr.GetGuildCharterByCharterGuid( GUID_LOPART(itemGuid) );

    if(!gc)
    {
        sLog.outDebug("Charter Error");
        return;
    }

    gc->guildName = newGuildName;

    data.Initialize( SMSG_PETITION_QUERY_RESPONSE );
    data << uint32(GUID_LOPART(itemGuid));
    data << gc->leaderGuid;
    data << gc->guildName;
    data << uint8(0);
    data << uint32(9);
    data << uint32(9);
    for(i=0;i < 9;i++)
        data << uint32(0);
    data << uint8(0);
    data << uint8(0);
    SendPacket(&data);
    objmgr.SaveCharter(gc);

    data.Initialize(MSG_PETITION_RENAME);
    data << itemGuid;
    data << newGuildName;
    SendPacket(&data);
}
