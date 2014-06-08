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
#include <openssl/md5.h>
#include "../shared/AuthCodes.h"

#ifdef WIN32
#include "../shared/svn_revision.h"
#endif

#if PLATFORM == PLATFORM_WIN32
#define PLATFORM_TEXT "win32"
#elif PLATFORM == PLATFORM_UNIX
#define PLATFORM_TEXT "*nix"
#else
#define PLATFORM_TEXT "unknown"
#endif

#define SERVER_NAME ""

void WorldSession::HandleCharEnumOpcode( WorldPacket & recv_data )
{    
    uint32 start_time = getMSTime();

    // loading characters
    QueryResult* result = sDatabase.Query("SELECT guid, data, name, positionX, positionY, positionZ, mapId, zoneId, banned, restState, deathstate FROM characters WHERE acct=%u ORDER BY guid", GetAccountId());
    uint8 num = 0;
    SetSide(0);

    // should be more than enough.. 200 bytes per char..
    WorldPacket data((result ? result->GetRowCount() * 200 : 1));    

    SetSide(0);
    // parse m_characters and build a mighty packet of
    // characters to send to the client.
    data.SetOpcode(SMSG_CHAR_ENUM);

    data << num;

    if( result )
    {
        Player *plr;
        uint64 guid;
        Field *fields;
        do
        {
            fields = result->Fetch();
            guid = fields[0].GetUInt32();
            
            plr = World::GetPlayer(guid);
            if(plr)
            {
                // we already have that player in world... for some strange reason...
                continue;
            }

            plr = new Player(HIGHGUID_PLAYER, guid);
            ASSERT(plr);
            plr->SetSession(this);

            // added to catch an assertion failure at Player::LoadFromDB function.
            plr->LoadFromDB_Light( fields, guid );
            sLog.outDebug("Loaded char guid "I64FMTD" [%s] from account %d for enum build.",guid,plr->GetName(), GetAccountId());
            plr->BuildEnumData( &data );

            delete plr;

            num++;
        }
        while( result->NextRow() );

        delete result;
    }

    data.put<uint8>(0, num);

    sLog.outDetail("[Character Enum] Built in %u ms.", getMSTime() - start_time);
    SendPacket( &data );
}


void WorldSession::HandleCharCreateOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 10);
    std::string name;

    recv_data >> name;
    recv_data.rpos(0);

    //FIXME:
    //There is potential bug, if a name contains apostraphy, query would look like "name = 'my'nick'"
    //though using of 1 apostrophy in nick is allowed
    //the same for insertion into table char with nick that contains ' will not be saved into db
    //as temp fix disable names containing '
    //fix to use only a-z and A-Z letters
    const char * szName=name.c_str();
    for(uint32 x=0;x<strlen(szName);x++)
    {
        if((int)szName[x]<65||((int)szName[x]>90&&(int)szName[x]<97)||(int)szName[x]>122)
        {
            OutPacket(SMSG_CHAR_CREATE, 1, "\x31");
            return;
        }
    }

    QueryResult *result = sDatabase.Query("SELECT guid FROM characters WHERE name = '%s'", szName);
    if (result)
    {
        delete result;
        //CHAR_CREATE_IN_USE
        OutPacket(SMSG_CHAR_CREATE, 1, "\x31");
        return;
    }

    // loading characters
    
    //this check is useless, client will not allow to create more than 10 chars - 'create' button will not appear
    //unless we want to decrease maximum number of characters
   /* result = sDatabase.Query("SELECT COUNT(*) FROM characters WHERE acct=%u",GetAccountId());
    if (result)
    {
        uint32 count=result->Fetch()->GetUInt32();
        delete result;
        if (count  >= 10)
        {
            data.Initialize(SMSG_CHAR_CREATE);
            data << (uint8)0x34; // Should be a better error code i think
            SendPacket( &data );
            return;
        }
        
    }*/

    Player * pNewChar = sObjHolder.Create<Player>(objmgr.GenerateLowGuid(HIGHGUID_PLAYER));
    pNewChar->SetSession(this);
    if(!pNewChar->Create( recv_data ))
    {
        // failed.
        sObjHolder.Delete(pNewChar);
        return;
    }
    if(!HasPermissions())
    {
        if(pNewChar->getRace() == RACE_DRAENEI
            || pNewChar->getRace() == RACE_DWARF
            || pNewChar->getRace() == RACE_GNOME
            || pNewChar->getRace() == RACE_HUMAN
            || pNewChar->getRace() == RACE_NIGHTELF)
        {
            if(GetSide() == 2)
            {
                sObjHolder.Delete(pNewChar);
                WorldPacket data;
                data.Initialize(SMSG_CHAR_CREATE);
                data << (uint8)ALL_CHARS_ON_PVP_REALM_MUST_AT_SAME_SIDE;
                SendPacket( &data );
                return;
            }
        }
        else
        {
            if(GetSide() == 1)
            {
                sObjHolder.Delete(pNewChar);
                WorldPacket data;
                data.Initialize(SMSG_CHAR_CREATE);
                data << (uint8)ALL_CHARS_ON_PVP_REALM_MUST_AT_SAME_SIDE;
                SendPacket( &data );
                return;
            }
        }
    }
    pNewChar->UnSetBanned();
    
    pNewChar->addSpell(22027);      // Remove Insignia

    // done in login
    /*if( CanUseCommand('c') ) // Learn all languages for GM !
    {
        // Common
        pNewChar->addSpell(668);
        pNewChar->AddSkillLine(98,300,300);
        // Darnassian
        pNewChar->addSpell(671);
        pNewChar->AddSkillLine(113,300,300);
        // Dwarven
        pNewChar->addSpell(672);
        pNewChar->AddSkillLine(111,300,300);
        // Gnome
        pNewChar->addSpell(7340);
        pNewChar->AddSkillLine(313,300,300);
        // Orc
        pNewChar->addSpell(669);
        pNewChar->AddSkillLine(109,300,300);
        // Troll
        pNewChar->addSpell(7341);
        pNewChar->AddSkillLine(315,300,300);
        // Taurane
        pNewChar->addSpell(670);
        pNewChar->AddSkillLine(115,300,300);
        // Gutterspeak
        pNewChar->addSpell(17737);
        pNewChar->AddSkillLine(673,300,300);
    }*/

    if(pNewChar->getClass() == WARLOCK)
    {
        pNewChar->AddSummonSpell(416, 3110);        // imp fireball
        pNewChar->AddSummonSpell(417, 19505);
        pNewChar->AddSummonSpell(1860, 3716);
        pNewChar->AddSummonSpell(1863, 7814);
    }

    pNewChar->SaveToDB(true);    

    PlayerInfo pn ;
    pn.guid = pNewChar->GetGUID();
    pn.name = pNewChar->GetName();
    pn.cl = pNewChar->getClass();
    pn.race = pNewChar->getRace();
    pn.gender = pNewChar->getGender();
    pn.publicNote="";
    pn.officerNote="";
    
    if(pn.race==RACE_HUMAN||pn.race==RACE_DWARF||pn.race==RACE_GNOME||pn.race==RACE_NIGHTELF)
        pn.team =0;
    else 
        pn.team=1;
    objmgr.AddPlayerInfo(&pn);

    sObjHolder.Delete(pNewChar);

    // CHAR_CREATE_SUCCESS
    OutPacket(SMSG_CHAR_CREATE, 1, "\x2E");
}

/* FOR 1.10.1
SMSG_CHAR_CREATE Error Codes:
0x00 Success
0x01 Failure
0x02 Canceled
0x03 Disconnect from server
0x04 Failed to connect
0x05 Connected
0x06 Wrong client version
0x07 Connecting to server
0x08 Negotiating security
0x09 Negotiating security complete
0x0A Negotiating security failed
0x0B Authenticating
0x0C Authentication successful
0x0D Authentication failed
0x0E Login unavailable - Please contact Tech Support
0x0F Server is not valid
0x10 System unavailable 
0x11 System error
0x12 Billing system error
0x13 Account billing has expired
0x14 Wrong client version
0x15 Unknown account
0x16 Incorrect password
0x17 Session expired
0x18 Server Shutting Down
0x19 Already logged in
0x1A Invalid login server
0x1B Position in Queue: 0
0x1C This account has been banned
0x1D This character is still logged on
0x1E Your WoW subscription has expired
0x1F This session has timed out
0x20 This account has been temporarily suspended
0x21 Access to this account blocked by parental controls 
0x22 Retrieving realmlist
0x23 Realmlist retrieved
0x24 Unable to connect to realmlist server
0x25 Invalid realmlist
0x26 The game server is currently down
0x27 Creating account
0x28 Account created
0x29 Account creation failed
0x2A Retrieving character list
0x2B Character list retrieved
0x2C Error retrieving character list
0x2D Creating character
0x2E Character created
0x2F Error creating character
0x30 Character creation failed
0x31 That name is unavailable
0x32 Creation of that race/class is disabled
0x33 You cannot have both horde and alliance character at pvp realm
0x33 All characters on a PVP realm must be on the same team
0x34 You already have maximum number of characters
0x35 You already have maximum number of characters
0x36 The server is currently queued
0x37 Only players who have characters on this realm..
0x38 Creation of that race requires an account that has been upgraded to the approciate expansion
0x39 Deleting character
0x3A Character deleted
0x3B Char deletion failed
0x3c Entering world of warcraft
0x3D Login successful
0x3E World server is down
0x3F A character with that name already exists
0x40 No instance server available
0x41 Login failed
0x42 Login for that race/class is disabled
0x43 Character not found
0x44 Enter a name for your character
0x45 Names must be atleast 2 characters long
0x46 Names must be no more then 12 characters
0x47 Names can only contain letters
0x48 Names must contain only one language
0x49 That name contains profanity
0x4A That name is unavailable
0x4B You cannot use an apostrophe
0x4C You can only have one apostrophe
0x4D You cannot use the same letter three times consecutively
0x4E You cannit use space as the first or last character of your name
0x4F <Blank>
0x50 Invalid character name
0x51 <Blank>
All further codes give the number in dec.
*/

void WorldSession::HandleCharDeleteOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 8);
    uint8 fail = 0x3A;

    uint64 guid;
    recv_data >> guid;

    if(World::GetPlayer(guid) != NULL)
    {
        // "Char deletion failed"
        fail = 0x3B;
    } else {

        Player* plr = new Player( HIGHGUID_PLAYER, GUID_LOPART(guid));
        ASSERT(plr);
        plr->SetSession(this);

        if(!plr->LoadFromDB( GUID_LOPART(guid) ))
        {
            // Disconnect us
            sCheatLog.writefromsession(this, "Tried to delete non-existant player %u\n", guid);
            Disconnect();
            //sObjHolder.Delete<Player>(plr);
            return;
        }
        plr->DeleteFromDB();
        //if charter leader
        if(plr->m_signedCharter)
            objmgr.DeletePlayerSignature(guid);
        
        Guild *pGuild = objmgr.GetGuild(plr->GetGuildId());
        if(pGuild)
        {
            pGuild->DeleteGuildMember(plr->GetGUID());
        }

        delete plr;

    }

    OutPacket(SMSG_CHAR_DELETE, 1, &fail);
}


void WorldSession::HandlePlayerLoginOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 8);
    uint64 playerGuid = 0;

    sLog.outDebug( "WORLD: Recvd Player Logon Message" );

    recv_data >> playerGuid; // this is the GUID selected by the player

    if(World::GetPlayer(playerGuid) != NULL)
    {
        // A character with that name already exists 0x3E
        uint8 respons = CHAR_LOGIN_DUPLICATE_CHARACTER;
        OutPacket(SMSG_CHARACTER_LOGIN_FAILED, 1, &respons);
        return;
    }

    Player* plr = sObjHolder.Create<Player>(GUID_LOPART(playerGuid));
    ASSERT(plr);
    plr->SetSession(this);
    
    if(!plr->LoadFromDB(GUID_LOPART(playerGuid)))
    {
        // kick em.
        sCheatLog.writefromsession(this, "Tried to log in with invalid player guid %u.", playerGuid);
        Disconnect();
        sObjHolder.Delete<Player>(GUID_LOPART(playerGuid));
        return;
    }

    SetPlayer(plr); 
    m_MoverWoWGuid.Init(plr->GetGUID());
    
    plr->LoadPropertiesFromDB();

    // account data == UI config
    WorldPacket data(SMSG_ACCOUNT_DATA_MD5, 128);

    for (int i = 0; i < 8; i++)
    {
        AccountDataEntry* acct_data = GetAccountData(i);
        
        if (!acct_data->data)
        {
            data << uint64(0) << uint64(0);                // Nothing.
            continue;
        }
        MD5_CTX ctx;
        MD5_Init(&ctx);
       
        MD5_Update(&ctx, acct_data->data, acct_data->sz);
        uint8 md5hash[MD5_DIGEST_LENGTH];
        MD5_Final(md5hash, &ctx);
        data.append(md5hash, MD5_DIGEST_LENGTH);
    }
    SendPacket(&data);

    // Set TIME OF LOGIN
    sDatabase.Execute (
        "UPDATE characters SET online = 1 WHERE guid = %u" , plr->GetGUIDLow());

    // cast passive initial spells      -- burlex note: these shouldnt require plyr to be in world
    SpellSet::iterator itr;
    SpellEntry *info;
    for (itr = plr->mSpells.begin(); itr != plr->mSpells.end(); ++itr)
    {
        info = sSpellStore.LookupEntry(*itr);

        if(info && (info->Attributes & ATTRIBUTES_PASSIVE)  ) // passive
        {
            Spell * spell=new Spell(plr,info,true,NULL);
            SpellCastTargets targets;
            targets.m_unitTarget = plr->GetGUID();
            spell->prepare(&targets);
        }
    }
    
    // this needs to be after the cast of passive spells, because it will cast ghost form, after the remove making it in ghost alive, if no corpse.
    //death system checkout
    if(plr->GetUInt32Value(UNIT_FIELD_HEALTH) <= 0 && !plr->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_DEATH_WORLD_ENABLE))
    {
        plr->setDeathState(CORPSE);
    }
    else if(plr->HasFlag(PLAYER_FLAGS, PLAYER_FLAG_DEATH_WORLD_ENABLE))
    {
        // Check if we have an existing corpse.
        Corpse * corpse = objmgr.GetCorpseByOwner(plr);
        if(corpse == 0)
        {
            // Revive us, we lost our corpse somehow :/
            plr->ResurrectPlayer();
            plr->SetUInt32Value(UNIT_FIELD_HEALTH, plr->GetUInt32Value(UNIT_FIELD_MAXHEALTH) / 2);
        }
        else
        {
            // Set proper deathstate
            plr->setDeathState(CORPSE);
        }
    }

    QueryResult *result = sDatabase.Query(
        "SELECT spell,dur FROM char_auras WHERE charId=%u",plr->GetGUIDLow());
    if(result)
    {
        do
        {
            Field *fields = result->Fetch();
            uint32 spellid=fields[0].GetUInt32();
            Spell * spell=new Spell(plr,sSpellStore.LookupEntry(spellid),true,NULL);
            SpellCastTargets targets;
            targets.m_unitTarget = plr->GetGUID();
            spell->prepare(&targets);
            plr->SetAurDuration(spellid,plr,fields[1].GetUInt32());
            
        }
        while( result->NextRow() );

        delete result;
    }

    bool enter_world = true;

    // Find our transporter and add us if we're on one.
    if(plr->m_TransporterGUID != 0)
    {
        Transporter * pTrans = sObjHolder.GetObject<Transporter>(plr->m_TransporterGUID);
        if(pTrans)
        {
            float c_tposx = pTrans->GetPositionX() + plr->m_TransporterX;
            float c_tposy = pTrans->GetPositionY() + plr->m_TransporterY;
            float c_tposz = pTrans->GetPositionZ() + plr->m_TransporterZ;
            if(plr->GetMapId() != pTrans->GetMapId())       // loaded wrong map
            {
                plr->SetMapId(pTrans->GetMapId());
                WorldPacket data(SMSG_NEW_WORLD, 20);
                data << pTrans->GetMapId() << c_tposx << c_tposy << c_tposz << plr->GetOrientation();
                SendPacket(&data);

                // shit is sent in worldport ack.
                enter_world = false;
            }

            plr->SetPosition(c_tposx, c_tposy, c_tposz, plr->GetOrientation(), false);
            plr->m_CurrentTransporter = pTrans;
            pTrans->AddPlayer(plr);
        }
    }

    // if we're a gm, add languages
    if(CanUseCommand('c'))
    {
        plr->RemoveLanguages();
        plr->AddLanguages();
    }

    if(enter_world)
    {
        plr->SendInitialLogonPackets();
        plr->AddToWorld();
    }

    char welcomemsg[80];
    sprintf(welcomemsg, "Player %s (%s %s %s), logged in.", plr->GetName(), plr->getGender() ? "Female" : "Male", 
        sCharRaceStore.LookupString(plr->myRace->name2), sCharClassStore.LookupString(plr->myClass->name));
    if(sScriptMgr.HasPlugins())
    {
        WorldPacket wdata(PSMSG_MESSAGE, 80);
        wdata << welcomemsg;
        PostPluginMessage(wdata);
    }
    printf(welcomemsg);
    printf("\n");

    // send extended message
    sWorld.BroadcastExtendedMessage(this, "[SM:PLRLOGIN:%u:%u]%s", plr->getRace(), plr->getClass(), plr->GetName());
    if(HasFlag(ACCOUNT_FLAG_XTEND_INFO))
    {
        // send gm list
        stringstream ssg;
        uint32 c = 0;
        for(set<WorldSession*>::iterator itr = sWorld.gmList.begin(); itr != sWorld.gmList.end(); ++itr)
        {
            if((*itr)->GetPlayer())
            {
                ssg << (*itr)->GetPlayer()->GetNameString();
                ssg << ",";
                ++c;
            }
        }
        _player->BroadcastMessage("[SM:GMLIST:%s]", ssg.str().c_str());
    }

    if(HasPermissions())
    {
        sWorld.BroadcastExtendedMessage(this, "[SM:GMLOGIN]%s", plr->GetName());
    }

    if(plr->GetTeam() == 1)
        sWorld.HordePlayers++;
    else
        sWorld.AlliancePlayers++;

    // send info
    sWorld.BroadcastExtendedMessage(0, "[SM:INFO:%u:%u]", sWorld.HordePlayers, sWorld.AlliancePlayers);

    if(plr->m_FirstLogin)
    {
        uint32 racecinematic = plr->myRace->cinematic_id;
        OutPacket(SMSG_TRIGGER_CINEMATIC, 4, &racecinematic);
    }

    sLog.outDetail( "WORLD: Created new player for existing players (%s)", plr->GetName() );

    // Login time, will be used for played time calc
    plr->m_playedtime[2] = (uint32)time(NULL);

    //Issue a message telling all guild members that this player has signed on
    if(plr->IsInGuild())
    {
        Guild *pGuild = objmgr.GetGuild( plr->GetGuildId() );
        if(pGuild)
        {
            WorldPacket data(50);
            data.Initialize(SMSG_GUILD_EVENT);
            data << uint8(GUILD_EVENT_MOTD);
            data << uint8(0x01);
            data << pGuild->GetGuildMotd();
            SendPacket(&data);

            data.Initialize(SMSG_GUILD_EVENT);
            data << uint8(GUILD_EVENT_HASCOMEONLINE);
            data << uint8(0x01);
            data << plr->GetName();
            data << plr->GetGUID();
            pGuild->SendMessageToGuild(0,&data,G_MSGTYPE_ALL);
        }
    }

    plr->_LoadPetSpells();
    plr->_LoadPet();

    if(plr->GetTaxiState())
    {
        // Create HAS to be sent before this!
        plr->ProcessPendingUpdates();
        plr->TaxiStart(plr->GetTaxiPath(), 
            plr->GetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID),
            plr->lastNode);
    }

    // useless logon spell
    Spell *logonspell = new Spell(plr, sSpellStore.LookupEntry(836), false, NULL);
    SpellCastTargets targets;
    targets.m_unitTarget = plr->GetGUID();
    targets.m_targetMask = 0x2;
    logonspell->prepare(&targets);

    // Send online status to people having this char in friendlist
    sSocialMgr.LoggedIn( GetPlayer() );

    // Send revision (if enabled)
    if(sWorld.sendRevisionOnJoin)
    {
        uint32 rev = WOWD_SVN_REVISION;
        _player->BroadcastMessage("%sServer Version: %s%s%s Core v%u.%u/%s|r %s %s", MSG_COLOR_SUBWHITE, MSG_COLOR_LIGHTBLUE, 
            SERVER_NAME, MSG_COLOR_WHITE, rev / 1000, rev % 1000, PLATFORM_TEXT, __DATE__, __TIME__);
    }

    // Send MOTD
    _player->BroadcastMessage(sWorld.GetMotd());

    if(plr->m_FirstLogin) plr->m_FirstLogin = false;

    // Calculate rested experience if there is time between lastlogoff and now
    uint32 currenttime = (uint32)time(NULL);
    float timediff = currenttime - plr->m_timeLogoff;

    if(plr->m_timeLogoff > 0 && plr->GetUInt32Value(UNIT_FIELD_LEVEL) < 60)    // if timelogoff = 0 then it's the first login
    {
        if(plr->m_isResting) 
        {
            // We are resting at an inn, calculate XP and add it.
            uint32 RestXP = plr->CalculateRestXP((uint32)timediff);
            plr->AddRestXP(RestXP);
            sLog.outDebug("REST: Added %d of rest XP.", RestXP);
            plr->ApplyPlayerRestState(true);
        }
        else if(timediff > 0)
        {
            // We are resting in the wilderness at a slower rate.
            uint32 RestXP = plr->CalculateRestXP((uint32)timediff);
            RestXP >>= 2;        // divide by 4 because its at 1/4 of the rate
            plr->AddRestXP(RestXP);
        }
    }

    // burlex: Set health and mana to saved values on login
    _player->SetUInt32Value(UNIT_FIELD_HEALTH, _player->GetBaseUInt32Value(UNIT_FIELD_HEALTH));
    _player->SetUInt32Value(UNIT_FIELD_POWER1, _player->GetBaseUInt32Value(UNIT_FIELD_POWER1)); 

/*    if(sWorld.m_useIrc)
        sIRCBot.SendMessage("> %s from account %u entered world.", _player->GetName(), GetAccountId());*/

    // burlex: check if we're in a battleground, and if so send the packets that set up scoreboard etc
    if(_player->GetMapId() == 489 ||
        _player->GetMapId() == 529 || _player->GetMapId() == 30)
    {
        uint32 battlegroundtype = sBattlegroundMgr.GetBattleGroundTypeByMapId(_player->GetMapId());
        Battleground *battleground = sBattlegroundMgr.GetBattlegroundByInstanceID(_player->GetInstanceID(),battlegroundtype);
        if(battleground == NULL)    // bad battleground
        {
            _player->SetInstanceID(_player->m_bgEntryPointInstance);
            _player->_Relocate(_player->m_bgEntryPointMap, LocationVector(_player->m_bgEntryPointX, _player->m_bgEntryPointY, _player->m_bgEntryPointZ, 
                _player->m_bgEntryPointO), true, true);
        } else {
            // we are ALREADY in battleground! no need to transport ;)
            battleground->AddPlayer(_player, false, true);
        }
    }

    sInstanceSavingManager.BuildSavedInstancesForPlayer(plr);
}
