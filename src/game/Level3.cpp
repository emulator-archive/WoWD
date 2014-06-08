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

/////////////////////////////////////////////////
//  Admin Chat Commands
//

#include "StdAfx.h"
#include "../wowd/Master.h"

bool ChatHandler::HandleWeatherCommand(const char* args, WorldSession *m_session)
{
    WorldPacket data;
    char *ptype = strtok((char*)args, " ");
    if(!ptype)
        return false;
    char *pintensity = strtok(NULL, " ");
    char *punk = strtok(NULL, " ");
    char *punk2 = strtok(NULL, " ");
    if(!pintensity || !punk || !punk2)
        return false;

    uint32 type = atol(ptype);
    float intensity = (float)atol(args);
    uint32 sound = atol(punk);
    uint8 switchTimeFlag = atol(punk2);

    //    sLog.outString("Recived set weather command. Type: %u, Intensity: %.3f, Unknown: %u", type, intensity, unk);
    data.Initialize(SMSG_WEATHER);
    data << (uint32)type;
    data << (float)intensity;
    data << (uint32)sound;
    data << (uint8)switchTimeFlag;
    m_session->GetPlayer()->SendMessageToSet(&data,true);
    sLog.outDebug("SENT SMSG_WEATHER");
    return true;
}

bool ChatHandler::HandleSecurityCommand(const char* args, WorldSession *m_session)
{
    WorldPacket data;

    char* pName = strtok((char*)args, " ");
    if (!pName)
        return false;

    char* pgm = strtok(NULL, " ");
    if (!pgm)
        return false;

    char buf[256];

    Player *chr = objmgr.GetPlayer((const char*)pName, false);
    if (chr)
    {
        // send message to user
        sprintf((char*)buf,"You change security string of %s to [%s].", chr->GetName(), pgm);
        SystemMessage(m_session, buf);

        // send message to player
        sprintf((char*)buf,"%s changed your security string to [%s].", m_session->GetPlayer()->GetName(), pgm);
        SystemMessage(chr->GetSession(),  buf);
        chr->GetSession()->SetSecurity(pgm);
        //sLogonDatabase.Execute("UPDATE accounts SET gm='%s' WHERE acct=%u", pgm, chr->GetSession()->GetAccountId());
        sLogonCommHandler.LogonDatabaseSQLExecute(buf);
    }
    else
    {
        sprintf((char*)buf,"Player (%s) does not exist or is not logged in.", pName);
        SystemMessage(m_session, buf);
    }

    return true;
}


bool ChatHandler::HandleWorldPortCommand(const char* args, WorldSession *m_session)
{
    char* pContinent = strtok((char*)args, " ");
    if (!pContinent)
        return false;

    char* px = strtok(NULL, " ");
    char* py = strtok(NULL, " ");
    char* pz = strtok(NULL, " ");

    if (!px || !py || !pz)
        return false;
    
    uint32 mapId = atoi(pContinent);
    //m_session->GetPlayer()->Relocate(mapId, atof(px), atof(py), atof(pz), m_session->GetPlayer()->GetOrientation(), true, true);
    m_session->GetPlayer()->SafeTeleport(mapId, 0, LocationVector(atof(px), atof(py), atof(pz)));

    return true;
}

bool ChatHandler::HandleAllowMovementCommand(const char* args, WorldSession *m_session)
{
    WorldPacket data;
    if(sWorld.getAllowMovement())
    {
        sWorld.SetAllowMovement(false);
        SystemMessage(m_session, "Creature Movement Disabled.");
    }
    else
    {
        sWorld.SetAllowMovement(true);
        SystemMessage(m_session, "Creature Movement Enabled.");
    }

    return true;
}

bool ChatHandler::HandleAddSpiritCommand(const char* args, WorldSession *m_session)
{
    sLog.outDetail("Spawning Spirit Healers\n");

/*    std::stringstream query;
    std::stringstream ss,ss2,ss3;
    QueryResult *result;
    Creature* pCreature;
    UpdateMask unitMask;
    WorldPacket data;

    query << "select X,Y,Z,F,name_id,mapId,zoneId,faction_id from spirithealers";
    result = sDatabase.Query( query.str( ).c_str( ) );

    if(!result)
    {
        FillSystemMessageData(&data, "No spirit healers in db, exiting.");
        m_session->SendPacket( &data );

        return true;
    }

    uint32 name;
    do
    {
        Field* fields = result->Fetch();

        name = fields[4].GetUInt32();
        sLog.outDetail("%s name is %d\n", fields[4].GetString(), name);

        pCreature = new Creature();

        pCreature->Create(objmgr.GenerateLowGuid(HIGHGUID_UNIT), objmgr.GetCreatureName(name)->Name.c_str(), fields[5].GetUInt16(),
            fields[0].GetFloat(), fields[1].GetFloat(), fields[2].GetFloat(), fields[3].GetFloat());

        pCreature->SetZoneId( fields[6].GetUInt16() );
        pCreature->SetUInt32Value( OBJECT_FIELD_ENTRY, name );
        pCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
        pCreature->SetUInt32Value(UNIT_FIELD_DISPLAYID, 5233);
        pCreature->SetUInt32Value(UNIT_NPC_FLAGS, 65);//33
        pCreature->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE , 35);
        pCreature->SetUInt32Value(UNIT_FIELD_HEALTH, 100);
        pCreature->SetUInt32Value(UNIT_FIELD_MAXHEALTH, 100);
        pCreature->SetUInt32Value(UNIT_FIELD_LEVEL, 60);
        pCreature->SetUInt32Value(UNIT_FIELD_FLAGS, 768);
        pCreature->SetUInt32Value(UNIT_FIELD_AURA+0, 10848);
        pCreature->SetUInt32Value(UNIT_FIELD_AURALEVELS+0, 0xEEEEEE3C);
        pCreature->SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS+0, 0xEEEEEE00);
        pCreature->SetUInt32Value(UNIT_FIELD_AURAFLAGS+0, 0x00000009);
        pCreature->SetFloatValue(UNIT_FIELD_COMBATREACH , 1.5f);
        pCreature->SetFloatValue(UNIT_FIELD_MAXDAMAGE ,  5.0f);
        pCreature->SetFloatValue(UNIT_FIELD_MINDAMAGE , 8.0f);
        pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 1900);
        pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME+1, 2000);
        pCreature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 2.0f);
        pCreature->_setFaction();

        sLog.outError("AddObject at Level3.cpp line 182");
        objmgr.AddObject(pCreature);
        pCreature->AddToWorld();

        pCreature->SaveToDB();
    }
    while( result->NextRow() );

    ss.rdbuf()->str("");
    ss2.rdbuf()->str("");
    ss3.rdbuf()->str("");*/
    /*result2 = sDatabase.Query( "SELECT MAX(ID) FROM npc_gossip" );
    if( result2 )
    {
        result3 = sDatabase.Query( "SELECT MAX(ID) FROM npc_text" );
        if( result3 )
        {
            textId = (*result3)[0].GetUInt32();
            ss3 << "INSERT INTO npc_text ( ID , TYPE_UNUSED, TEXT) VALUES ("
                << textId+1 << ", "
                << 0 << ", '"
                << "It is not yet your time. I shall aid your journey back to the realm of the living... For a price." << "')";
            sDatabase.Execute( ss3.str( ).c_str( ) );
            delete result3;
            result3 = NULL;
        }

        gossipId = (*result2)[0].GetUInt32();
        ss2 << "INSERT INTO npc_gossip ( ID , NPC_ENTRY, GOSSIP_TYPE, TEXTID, OPTION_COUNT) VALUES ("
            << gossipId+1 << ", "
            << pCreature->getNameEntry() << ", "
            << 1 << ", "
            << textId+1 << ", "
            << 1 << ")";

        sDatabase.Execute( ss2.str( ).c_str( ) );
        delete result2;
        result2 = NULL;


        result2 = sDatabase.Query( "SELECT MAX(ID) FROM npc_options" );
        if( result2 )
        {
            ss << "INSERT INTO npc_options VALUES ("
                << (*result2)[0].GetUInt32()+1 << ", "
                << gossipId+1 << ", "
                << 0 << ", '"
                << "Return me to life." << "', "
                << 0 << ", "
                << 2 << ", "
                << 0 << ", "
                << 0 << ", "
                << 0 << ", "
                << 0 << ", "
                << 0 << ", '')";
            sDatabase.Execute( ss.str( ).c_str( ) );
            delete result2;
            result2 = NULL;
        }
    }*/

    /*delete result;

    FillSystemMessageData(&data, "Restart server for spirit healers.");
    m_session->SendPacket( &data );

    return true;*/
#define SPIRITHEALER_NAMEID 6491
    // Loop through all graveyard locations
    uint32 entries=sWorldSafeLocsStore.GetNumRows();
    GraveyardEntry*g;
    BlueSystemMessage(m_session, "Spawning spirit healers. This may take some time.");
    for(uint32 c=0;c<entries;c++)
    {
        g=sWorldSafeLocsStore.LookupEntry(c);
        // Let's create a creature
        Creature *pCreature = sObjHolder.Create<Creature>();
        float x = g->x + 2;
        float y = g->y - 2;        // move it a little..
                
        pCreature->Create(objmgr.GetCreatureName(SPIRITHEALER_NAMEID)->Name.c_str(), g->mapid, x, y, g->z, 3.14);
        pCreature->SetUInt32Value( OBJECT_FIELD_ENTRY, SPIRITHEALER_NAMEID );
        pCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
        pCreature->SetUInt32Value(UNIT_FIELD_DISPLAYID, 5233);
        pCreature->SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, 5233);
        pCreature->SetUInt32Value(UNIT_NPC_FLAGS, 35);//33
        pCreature->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE , 35);
        pCreature->SetUInt32Value(UNIT_FIELD_HEALTH, 100);
        pCreature->SetUInt32Value(UNIT_FIELD_MAXHEALTH, 100);
        pCreature->SetUInt32Value(UNIT_FIELD_LEVEL, 60);
        pCreature->SetUInt32Value(UNIT_FIELD_FLAGS, 768);
        pCreature->SetUInt32Value(UNIT_FIELD_AURA, 10848);
        pCreature->SetUInt32Value(UNIT_FIELD_AURALEVELS+0, 0x0000003C);
        pCreature->SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS+0, 0x000000FF);
        pCreature->SetUInt32Value(UNIT_FIELD_AURAFLAGS+0, 0x00000009);
        pCreature->SetFloatValue(UNIT_FIELD_COMBATREACH , 1.5f);
        pCreature->SetFloatValue(UNIT_FIELD_MAXDAMAGE ,  5.0f);
        pCreature->SetFloatValue(UNIT_FIELD_MINDAMAGE , 8.0f);
        pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 1900);
        pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME+1, 2000);
        pCreature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 2.0f);
        pCreature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0x01010100);
        pCreature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0x01000000);
        pCreature->SetUInt32Value(UNIT_FIELD_BYTES_2, 0x00001001);

        pCreature->_setFaction();
        pCreature->SaveToDB();
    }
    GreenSystemMessage(m_session, "%d spirit healers spawned.", entries);
    return true;
}

bool ChatHandler::HandleLearnCommand(const char* args, WorldSession *m_session)
{

    if (!*args)
        return false;

    uint32 spell = atol((char*)args);
    
    Player *plr = getSelectedChar(m_session, true);
    if(!plr)
    {
        plr = m_session->GetPlayer();
        SystemMessage(m_session, "Auto-targeting self.");
    }
    if(!plr) return false;

    if (plr->HasSpell(spell)) // check to see if char already knows
    {
        std::string OutStr = plr->GetName();
        OutStr += " already knows that spell.";

        SystemMessage(m_session, OutStr.c_str());
        return true;
    }

    plr->addSpell(spell);
    BlueSystemMessageToPlr(plr, "%s taught you Spell %d", m_session->GetPlayer()->GetName(), spell);

    return true;
}

bool ChatHandler::HandleAddWeaponCommand(const char* args, WorldSession *m_session)
{
    if (!*args)
        return false;

    uint64 guid = m_session->GetPlayer()->GetSelection();
    if (guid == 0)
    {
        SystemMessage(m_session, "No selection.");
        return true;
    }

    Creature * pCreature = objmgr.GetCreature(guid);
    if(!pCreature)
    {
        SystemMessage(m_session, "You should select a creature.");
        return true;
    }

    char* pSlotID = strtok((char*)args, " ");
    if (!pSlotID)
        return false;

    char* pItemID = strtok(NULL, " ");
    if (!pItemID)
        return false;       

    uint32 ItemID = atoi(pItemID);
    uint32 SlotID = atoi(pSlotID);

    ItemPrototype* tmpItem = objmgr.GetItemPrototype(ItemID);

    bool added = false;
    std::stringstream sstext;
    if(tmpItem)
    {
        switch(SlotID)
        {
        case 1:
            pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, ItemID);
            added = true;
            break;
        case 2:
            pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY_01, ItemID);
            added = true;
            break;
        case 3:
            pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY_02, ItemID);
            added = true;
            break;
        default:
            sstext << "Item Slot '" << SlotID << "' doesn't exist." << '\0';
            added = false;
            break;
        }
        if(added)
        {
            sstext << "Item '" << ItemID << "' '" << tmpItem->Name1 << "' Added to Slot " << SlotID << '\0';
        }
    }
    else
    {
        sstext << "Item '" << ItemID << "' Not Found in Database." << '\0';
        return true;
    }
    SystemMessage(m_session, sstext.str().c_str());
    return true;
}

bool ChatHandler::HandleReviveCommand(const char* args, WorldSession *m_session)
{
    Player* SelectedPlayer = getSelectedChar(m_session, true);
    if(!SelectedPlayer) return true;

    
    SelectedPlayer->SetMovement(MOVE_UNROOT, 1);
    SelectedPlayer->ResurrectPlayer();
    SelectedPlayer->SetUInt32Value(UNIT_FIELD_HEALTH, SelectedPlayer->GetUInt32Value(UNIT_FIELD_MAXHEALTH) );
    return true;
}

bool ChatHandler::HandleMorphCommand(const char* args, WorldSession *m_session)
{
    if (!*args)
        return false;

    uint16 display_id = (uint16)atoi((char*)args);

    m_session->GetPlayer()->SetUInt32Value(UNIT_FIELD_DISPLAYID, display_id);
    //m_session->GetPlayer()->UpdateObject( );
    //m_session->GetPlayer()->SendMessageToSet(&data, true);

    return true;
}


bool ChatHandler::HandleAddGraveCommand(const char* args, WorldSession *m_session)
{
    QueryResult *result;
    std::stringstream ss;
    GraveyardTeleport *pGrave;

    ss.rdbuf()->str("");
    pGrave = new GraveyardTeleport;

    result = sDatabase.Query( "SELECT MAX(ID) FROM graveyards" );
    if( result )
    {
        pGrave->ID = result->Fetch()[0].GetUInt32()+1;

        delete result;
    }
    pGrave->X = m_session->GetPlayer()->GetPositionX();
    pGrave->Y = m_session->GetPlayer()->GetPositionY();
    pGrave->Z = m_session->GetPlayer()->GetPositionZ();
    pGrave->O = m_session->GetPlayer()->GetOrientation();
    pGrave->ZoneId = m_session->GetPlayer()->GetZoneId();
    pGrave->MapId = m_session->GetPlayer()->GetMapId();
    pGrave->FactionID = 0;

    ss << "INSERT INTO graveyards ( X, Y, Z, O, zoneId, mapId) VALUES ("
        << pGrave->X << ", "
        << pGrave->Y << ", "
        << pGrave->Z << ", "
        << pGrave->O<< ", "
        << pGrave->ZoneId << ", "
        << pGrave->MapId << ")";

    sDatabase.Execute( ss.str( ).c_str( ) );

    objmgr.AddGraveyard(pGrave);
    return true;
}

bool ChatHandler::HandleAddSHCommand(const char *args, WorldSession *m_session)
{
    uint32 gossipId,textId;
    QueryResult *result2,*result3;
    std::stringstream ss,ss2,ss3;

    // Create the requested monster
    Player *chr = m_session->GetPlayer();
    float x = chr->GetPositionX();
    float y = chr->GetPositionY();
    float z = chr->GetPositionZ();
    float o = chr->GetOrientation();

    Creature* pCreature = sObjHolder.Create<Creature>();

    pCreature->Create("Spirit Healer", chr->GetMapId(), x, y, z, o);
    pCreature->SetZoneId(chr->GetZoneId());
    pCreature->SetUInt32Value(OBJECT_FIELD_ENTRY, objmgr.AddCreatureName(pCreature->GetName(), 5233));
    /*    pCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
    pCreature->SetUInt32Value(UNIT_FIELD_DISPLAYID, 5233);
    pCreature->SetUInt32Value(UNIT_NPC_FLAGS, 33);
    pCreature->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE , 35);
    pCreature->SetUInt32Value(UNIT_FIELD_HEALTH, 100);
    pCreature->SetUInt32Value(UNIT_FIELD_MAXHEALTH, 100);
    pCreature->SetUInt32Value(UNIT_FIELD_LEVEL, 60);
    pCreature->SetUInt32Value(UNIT_FIELD_FLAGS, 768);
    pCreature->SetUInt32Value(UNIT_FIELD_AURA+0, 10848);
    pCreature->SetUInt32Value(UNIT_FIELD_AURALEVELS+0, 0xEEEEEE3C);
    pCreature->SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS+0, 0xEEEEEE00);
    pCreature->SetUInt32Value(UNIT_FIELD_AURAFLAGS+0, 0x00000009);
    pCreature->SetFloatValue(UNIT_FIELD_COMBATREACH , 1.5f);
    pCreature->SetFloatValue(UNIT_FIELD_MAXDAMAGE ,  5.0f);
    pCreature->SetFloatValue(UNIT_FIELD_MINDAMAGE , 8.0f);
    pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 1900);
    pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME+1, 2000);
    pCreature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 2.0f);
    sLog.outError("AddObject at Level3.cpp line 455");
    objmgr.AddObject(pCreature);
    pCreature->AddToWorld();
    pCreature->SaveToDB();

    std::stringstream ss,ss2,ss3;
    QueryResult *result;

    result = sDatabase.Query( "SELECT MAX(ID) FROM npc_gossip" );
    if( result )
    {
    ss2 << "INSERT INTO npc_gossip ( ID , NPC_ENTRY, GOSSIP_TYPE, TEXTID, OPTION_COUNT) VALUES ("
    << (*result)[0].GetUInt32()+1 << ", "
    << pCreature->GetGUIDLow() << ", "
    << 1 << ", "
    << 1 << ", "
    << 1 << ")";

    sDatabase.Execute( ss2.str( ).c_str( ) );
    delete result;
    result = NULL;


    result = sDatabase.Query( "SELECT MAX(ID) FROM npc_options" );
    if( result )
    {
    ss << "INSERT INTO npc_options ( ID , GOSSIP_ID, TYPE, OPTION, NPC_TEXT_NEXTID, SPECIAL) VALUES ("
    << (*result)[0].GetUInt32()+1 << ", "
    << (*result)[0].GetUInt32()+2 << ", "
    << 0 << ", '"
    << "Return me to life." << "', "
    << 0 << ", "
    << 2 << ")";

    sDatabase.Execute( ss.str( ).c_str( ) );
    delete result;
    result = NULL;
    }
    result = sDatabase.Query( "SELECT MAX(ID) FROM npc_text" );
    if( result )
    {
    ss3 << "INSERT INTO npc_text ( ID , TYPE_UNUSED, TEXT) VALUES ("
    << (*result)[0].GetUInt32()+1 << ", "
    << 0 << ", '"
    << "It is not yet your time. I shall aid your journey back to the realm of the living... For a price." << "')";
    sDatabase.Execute( ss3.str( ).c_str( ) );
    delete result;
    result = NULL;
    }
    }*/
    pCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
    pCreature->SetUInt32Value(UNIT_FIELD_DISPLAYID, 5233);
    pCreature->SetUInt32Value(UNIT_NPC_FLAGS, 65);
    pCreature->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE , 35);
    pCreature->SetUInt32Value(UNIT_FIELD_HEALTH, 100);
    pCreature->SetUInt32Value(UNIT_FIELD_MAXHEALTH, 100);
    pCreature->SetUInt32Value(UNIT_FIELD_LEVEL, 60);
    pCreature->SetUInt32Value(UNIT_FIELD_FLAGS, 768);
    pCreature->SetUInt32Value(UNIT_FIELD_AURA+0, 10848);
    pCreature->SetUInt32Value(UNIT_FIELD_AURALEVELS+0, 0xEEEEEE3C);
    pCreature->SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS+0, 0xEEEEEE00);
    pCreature->SetUInt32Value(UNIT_FIELD_AURAFLAGS+0, 0x00000009);
    pCreature->SetFloatValue(UNIT_FIELD_COMBATREACH , 1.5f);
    pCreature->SetFloatValue(UNIT_FIELD_MAXDAMAGE ,  5.0f);
    pCreature->SetFloatValue(UNIT_FIELD_MINDAMAGE , 8.0f);
    pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 1900);
    pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME+1, 2000);
    pCreature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 2.0f);
    pCreature->_setFaction();

    sLog.outError("AddObject at Level3.cpp line 759");
    pCreature->AddToWorld();

    pCreature->SaveToDB();


    ss.rdbuf()->str("");
    ss2.rdbuf()->str("");
    ss3.rdbuf()->str("");
    result2 = sDatabase.Query( "SELECT MAX(ID) FROM npc_gossip" );
    if( result2 )
    {
        result3 = sDatabase.Query( "SELECT MAX(ID) FROM npc_text" );
        if( result3 )
        {
            textId = result3->Fetch()[0].GetUInt32();
            ss3 << "INSERT INTO npc_text ( ID , TYPE_UNUSED, TEXT) VALUES ("
                << textId+1 << ", "
                << 0 << ", '"
                << "It is not yet your time. I shall aid your journey back to the realm of the living... For a price." << "')";
            sDatabase.Execute( ss3.str( ).c_str( ) );
            delete result3;
            result3 = NULL;
        }

        gossipId = result2->Fetch()[0].GetUInt32();
        ss2 << "INSERT INTO npc_gossip ( ID , NPC_ENTRY, GOSSIP_TYPE, TEXTID, OPTION_COUNT) VALUES ("
            << gossipId+1 << ", "
            << pCreature->getNameEntry() << ", "
            << 1 << ", "
            << textId+1 << ", "
            << 1 << ")";

        sDatabase.Execute( ss2.str( ).c_str( ) );
        delete result2;
        result2 = NULL;


        result2 = sDatabase.Query( "SELECT MAX(ID) FROM npc_options" );
        if( result2 )
        {
            ss << "INSERT INTO npc_options VALUES ("
                << result2->Fetch()[0].GetUInt32()+1 << ", "
                << gossipId+1 << ", "
                << 0 << ", '"
                << "Return me to life." << "', "
                << 0 << ", "
                << 2 << ", "
                << 0 << ", "
                << 0 << ", "
                << 0 << ", "
                << 0 << ", "
                << 0 << ", '')";

            sDatabase.Execute( ss.str( ).c_str( ) );
            delete result2;
            result2 = NULL;
        }
    }

    return true;
}

bool ChatHandler::HandleExploreCheatCommand(const char* args, WorldSession *m_session)
{
    if (!*args)
        return false;

    int flag = atoi((char*)args);

    Player *chr = getSelectedChar(m_session);
    if (chr == NULL)
    {
        SystemMessage(m_session, "No character selected.");
        return true;
    }

    char buf[256];

    // send message to user
    if (flag != 0)
    {
        sprintf((char*)buf,"%s has explored all zones now.", chr->GetName());
    }
    else
    {
        sprintf((char*)buf,"%s has no more explored zones.", chr->GetName());
    }
    SystemMessage(m_session, buf);

    // send message to player
    if (flag != 0)
    {
        sprintf((char*)buf,"%s has explored all zones for you.",
            m_session->GetPlayer()->GetName());
    }
    else
    {
        sprintf((char*)buf,"%s has hidden all zones from you.", 
            m_session->GetPlayer()->GetName());
    }
    SystemMessage(m_session,  buf);


    for (uint8 i=0; i<64; i++)
    {
        if (flag != 0)
        {
            chr->SetFlag(PLAYER_EXPLORED_ZONES_1+i,0xFFFFFFFF);
        }
        else
        {
            chr->SetFlag(PLAYER_EXPLORED_ZONES_1+i,0);
        }
    }

    return true;
}

bool ChatHandler::HandleLevelUpCommand(const char* args, WorldSession *m_session)
{
    int levels = 0;

    if (!*args)
        levels = 1;
    else
        levels = atoi(args);

    Player *plr = getSelectedChar(m_session, true);
    if(!plr) plr = m_session->GetPlayer();
    if(!plr) return false;
    sGMLog.writefromsession(m_session, "used level up command on %s, with %u levels", plr->GetName(), levels);

    uint32 startlvl = plr->GetUInt32Value(UNIT_FIELD_LEVEL);
    for(int i = startlvl; i < (startlvl+levels);i++)
    {
        uint32 curXP = plr->GetUInt32Value(PLAYER_XP);
        uint32 nextLvlXP = plr->GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
        uint32 givexp = nextLvlXP - curXP;

        plr->GiveXP(givexp,plr->GetGUID(), true);
        if(plr->getLevel() >= 255) break;
    }

    WorldPacket data;
    std::stringstream sstext;
    sstext << "You have been leveled up to Level " << startlvl+levels << '\0';
    SystemMessage(plr->GetSession(), sstext.str().c_str());

    sSocialMgr.SendUpdateToFriends( plr );

    return true;
}

//DK
bool ChatHandler::HandleBanCharacterCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        return false;

    char* pName = strtok((char*)args, " ");
    if (!pName)
        return false;

    char* pReason = (char*)args + strlen(pName);

    Player *chr = objmgr.GetPlayer(pName, false);
    if (chr == NULL) 
    {
        SystemMessage(m_session, "There is no character with that name.");
        return true;
    }

    if(chr->IsBanned())
        return false;

    if (*pReason)
        chr->SetBanned(pReason);
    else
        chr->SetBanned();

    sGMLog.writefromsession(m_session, "used ban character on %s reason %s", chr->GetName(), pReason);
    SystemMessage(m_session, "Character is banned!");
    return true;

}

bool ChatHandler::HandleBanReasonCharacterCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        return false;

    Player *chr = objmgr.GetPlayer(args, false);
    if (chr == NULL) 
    {
        SystemMessage(m_session, "There is no character with that name.");
        return true;
    }

    if(!chr->IsBanned())
        return false;

    SystemMessage(m_session, "Character is banned because: %s", chr->GetBanReason().size() ? chr->GetBanReason().c_str() : "No reason given");
    return true;
}

bool ChatHandler::HandleUnBanCharacterCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        return false;

    Player *chr = objmgr.GetPlayer(args, false);
    if (chr == NULL) 
    {
        SystemMessage(m_session, "There is no character with that name.");
        return true;
    }

    if(!chr->IsBanned())
        return false;

    chr->UnSetBanned();
    sGMLog.writefromsession(m_session, "used unban character on %s", chr->GetName());
    SystemMessage(m_session, "Character is unbanned!");
    return true;
}

bool ChatHandler::HandleGMTicketGetAllCommand(const char* args, WorldSession *m_session)
{
    Channel *chn = channelmgr.GetChannel(sWorld.getGmClientChannel().c_str(),m_session->GetPlayer());
    if(!chn)
        return false;

    chn->Say(m_session->GetPlayer(), "GmTicket 2", m_session->GetPlayer());
    for(std::list<GM_Ticket*>::iterator itr = objmgr.GM_TicketList.begin(); itr != objmgr.GM_TicketList.end(); itr++)
    {
        uint32 cont = 0;
        uint32 zone = 0;
        Player* plr = objmgr.GetPlayer((*itr)->guid);
        if(plr)
            if(plr->IsInWorld())
            {
                zone = plr->GetZoneId();
                cont = plr->GetMapId();
            }

            std::stringstream str;
            str << "GmTicket 0,";
            str << (*itr)->name.c_str() << ","  << (*itr)->level << ","  << (*itr)->type << ",";
            str << zone;

            chn->Say(m_session->GetPlayer(),str.str().c_str(), m_session->GetPlayer());
    }

    return true;
}

bool ChatHandler::HandleGMTicketGetByIdCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        return false;


    std::list<GM_Ticket*>::iterator i;
    for(i = objmgr.GM_TicketList.begin(); i != objmgr.GM_TicketList.end(); i++)
    {
        if(strcmp((*i)->name.c_str(), args) == 0)
        {
            Channel *chn = channelmgr.GetChannel(sWorld.getGmClientChannel().c_str(),m_session->GetPlayer());
            if(!chn)
                return false;

            std::stringstream str;
            str << "GmTicket 3,";
            str << (*i)->name.c_str() << "," << (*i)->message;
            chn->Say(m_session->GetPlayer(),str.str().c_str(), m_session->GetPlayer());
        }
    }
    return true;
}

bool ChatHandler::HandleGMTicketDelByIdCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        return false;

    std::list<GM_Ticket*>::iterator i;
    int32 guid = -1;
    for(i = objmgr.GM_TicketList.begin(); i != objmgr.GM_TicketList.end(); i++)
    {
        if(strcmp((*i)->name.c_str(), args) == 0)
        {
            guid = (*i)->guid;
            break;
        }
    }
    if(guid != -1)
    {
        objmgr.remGMTicket(guid);

        std::stringstream str;
        str << "GmTicket 1," << args;

        Channel *chn = channelmgr.GetChannel(sWorld.getGmClientChannel().c_str(),m_session->GetPlayer());
        if(!chn)
            return false;

        chn->Say(m_session->GetPlayer(), str.str().c_str(), m_session->GetPlayer());


        Player* plr = objmgr.GetPlayer(guid);
        if(!plr)
            return true;
        if(!plr->IsInWorld())
            return true;

        WorldPacket data(SMSG_GMTICKET_DELETETICKET, 4);
        data << uint32(9);

        plr->GetSession()->SendPacket( &data );
    }

    return true;
}

bool ChatHandler::HandleAddSkillCommand(const char* args, WorldSession *m_session)
{
    char buf[256];
    Player* target = sObjHolder.GetObject<Player>(m_session->GetPlayer()->GetSelection());

    if(!target) {
        SystemMessage(m_session, "Select A Player first.");
        return true;
    }

    uint32 skillline;
    uint16 cur, max;

    char* pSkillline = strtok((char*)args, " ");
    if (!pSkillline)
        return false;

    char* pCurrent = strtok(NULL, " ");
    if (!pCurrent)
        return false;

    char* pMax = strtok(NULL, " ");
    if (!pMax)
        return false;

    skillline = (uint32)atol(pSkillline);
    cur = (uint16)atol(pCurrent);
    max = (uint16)atol(pMax);

    target->AddSkillLine(skillline,cur,max);

    sprintf(buf,"SkillLine: %u CurrentValue %u Max Value %u Added.",skillline,cur,max);
    SystemMessage(m_session, buf);

    return true;
}

bool ChatHandler::HandleNpcInfoCommand(const char *args, WorldSession *m_session)
{
    char msg[512];
    uint32 guid = GUID_LOPART(m_session->GetPlayer()->GetSelection());
    Creature *crt = getSelectedCreature(m_session);
    if(!crt) return false;
    if(crt->GetCreatureName())
        BlueSystemMessage(m_session, "Showing creature info for %s", crt->GetCreatureName()->Name.c_str());
    sprintf(msg,"GUID: %d\nFaction: %d\nNPCFlags: %d\nDisplayID: %d", guid, crt->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE), crt->GetUInt32Value(UNIT_NPC_FLAGS), crt->GetUInt32Value(UNIT_FIELD_DISPLAYID));
    SystemMessage(m_session, msg);
    if(crt->m_faction)
        GreenSystemMessage(m_session, "Combat Support: 0x%.3X", crt->m_faction->fightSupport);
    GreenSystemMessage(m_session, "Base Health: %d", crt->GetUInt32Value(UNIT_FIELD_BASE_HEALTH));
    GreenSystemMessage(m_session, "Base Armor: %d", crt->GetUInt32Value(UNIT_FIELD_STAT1)*2);
    GreenSystemMessage(m_session, "Base Mana: %d", crt->GetUInt32Value(UNIT_FIELD_MAXPOWER1));
    GreenSystemMessage(m_session, "Base Holy: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_01));
    GreenSystemMessage(m_session, "Base Fire: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_02));
    GreenSystemMessage(m_session, "Base Nature: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_03));
    GreenSystemMessage(m_session, "Base Frost: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_04));
    GreenSystemMessage(m_session, "Base Shadow: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_05));
    GreenSystemMessage(m_session, "Base Arcane: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_06));
    GreenSystemMessage(m_session, "Damage min/max: %f/%f", crt->GetFloatValue(UNIT_FIELD_MINDAMAGE),crt->GetFloatValue(UNIT_FIELD_MAXDAMAGE));
    
    ColorSystemMessage(m_session, MSG_COLOR_RED, "Entry ID: %d", crt->GetUInt32Value(OBJECT_FIELD_ENTRY));
    // show byte
    std::stringstream sstext;
    uint32 theBytes = crt->GetUInt32Value(UNIT_FIELD_BYTES_0);
    sstext << "UNIT_FIELD_BYTES_0 are " << uint16((uint8)theBytes & 0xFF) << " " << uint16((uint8)(theBytes >> 8) & 0xFF) << " ";
    sstext << uint16((uint8)(theBytes >> 16) & 0xFF) << " " << uint16((uint8)(theBytes >> 24) & 0xFF) << '\0';
    BlueSystemMessage(m_session, sstext.str().c_str());
    return true;
}

bool ChatHandler::HandleIncreaseWeaponSkill(const char *args, WorldSession *m_session)
{
    char *pMin = strtok((char*)args, " ");
    uint32 cnt = 0;
    if(!pMin)
        cnt = 1;
    else
        cnt = atol(pMin);

    Player *pr = getSelectedChar(m_session, true);
    
    uint32 SubClassSkill = 0;
    if(!pr) pr = m_session->GetPlayer();
    if(!pr) return false;
    Item *it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
    ItemPrototype* proto = NULL;
    if (!it)
        it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
    if (it)
        proto = it->GetProto();
    if (proto)
    {
        switch(proto->SubClass)
        {
            // Weapons
        case 0:    // 1 handed axes
            SubClassSkill = SKILL_AXES;
            break;
        case 1:    // 2 handed axes
            SubClassSkill = SKILL_2H_AXES;
            break;        
        case 2:    // bows
            SubClassSkill = SKILL_BOWS;
            break;
        case 3:    // guns
            SubClassSkill = SKILL_GUNS;
            break;        
        case 4:    // 1 handed mace
            SubClassSkill = SKILL_MACES;
            break;
        case 5:    // 2 handed mace
            SubClassSkill = SKILL_2H_MACES;
            break;        
        case 6:    // polearms
            SubClassSkill = SKILL_POLEARMS;
            break;
        case 7: // 1 handed sword
            SubClassSkill = SKILL_SWORDS;
            break;
        case 8: // 2 handed sword
            SubClassSkill = SKILL_2H_SWORDS;
            break;
        case 9: // obsolete
            SubClassSkill = 136;
            break;
        case 10: //1 handed exotic
            SubClassSkill = 136;
            break;
        case 11: // 2 handed exotic
            SubClassSkill = 0;
            break;
        case 12: // fist
            SubClassSkill = SKILL_FIST_WEAPONS;
            break;
        case 13: // misc
            SubClassSkill = 0;
            break;
        case 15: // daggers
            SubClassSkill = SKILL_DAGGERS;
            break;
        case 16: // thrown
            SubClassSkill = SKILL_THROWN;
            break;
        case 17: // spears
            SubClassSkill = SKILL_SPEARS;
            break;
        case 18: // crossbows
            SubClassSkill = SKILL_CROSSBOWS;
            break;
        case 19: // wands
            SubClassSkill = SKILL_WANDS;
            break;
        case 20: // fishing
            SubClassSkill = SKILL_FISHING;
            break;
        }
    } 
    else
    {
        SubClassSkill = 162;
    }

    if(!SubClassSkill)
    {
        RedSystemMessage(m_session, "Can't find skill ID :-/");
        return false;
    }

    uint32 skill = SubClassSkill;

    BlueSystemMessage(m_session, "Modifying skill line %d. Advancing %d times.", skill, cnt);

    if(!pr->HasSkillLine(skill))
    {
        SystemMessage(m_session, "Does not have skill line, adding.");
        pr->AddSkillLine(skill, 1, 300);   
    } 
    else 
    {
        if(cnt + pr->GetSkillAmt(skill) > pr->GetSkillMax(skill))
        {
            cnt = pr->GetSkillMax(skill) - pr->GetSkillAmt(skill);
        }
        for(uint32 l=0;l<cnt;l++)
        {
            pr->AdvanceSkillLine(skill);
        }
    }       
    return true;    
}

bool ChatHandler::HandleCellDeleteCommand(const char *args, WorldSession *m_session)
{
    if(strcmp((const char*)strtok((char*)args," "), "YES") != 0)
        return false;

    MapCell* pCell = m_session->GetPlayer()->GetMapCell();
    if(strcmp((const char*)strtok(NULL," "), "YES") != 0)
    {
        RedSystemMessage(m_session, "Starting cells content deletion...");

        MapCell* cCell = NULL;
        MapMgr* pMapMgr = m_session->GetPlayer()->GetMapMgr();
        uint32 x=0,y=0,mX=0,mY=0;
        for(x=pCell->GetX()-1, mX=(pCell->GetX()+2); x<mX; x++)
        {
            for(y=pCell->GetY()-1, mY=(pCell->GetY()+2); y<mY; y++)
            {
                cCell = pMapMgr->GetCell(x,y);
                if(cCell->IsLoaded())
                {
                    BlueSystemMessage(m_session, "Starting cell [%d][%d] content deletion...", x, y );
                    cCell->RemoveObjects();
                    BlueSystemMessage(m_session, "Cell [%d][%d] content deletion DONE.", x, y );
                }
            }
        }
        GreenSystemMessage(m_session, "Cells content deletion DONE.");
        return true;
    }
    
    RedSystemMessage(m_session, "Starting cell [%d][%d] content deletion...", pCell->GetX(), pCell->GetY());
    pCell->RemoveObjects();
    GreenSystemMessage(m_session, "Cell deletion DONE.");

    return true;
}

bool ChatHandler::HandleAddRestXPCommand(const char* args, WorldSession *m_session)
{
    Player *plr = getSelectedChar(m_session, false);

    if(!plr) return true;

    int32 amt = atoi(args);
    plr->AddRestXP(amt);
    BlueSystemMessage(m_session, "Adding %d worth of rest XP to %s", amt, plr->GetName());
    GreenSystemMessageToPlr(plr, "%s gave you %d worth of REST XP.", m_session->GetPlayer()->GetName(), amt);
    sLog.outDebug("REST: %d added to %s from GM command (source: %s)", amt, plr->GetName(), m_session->GetPlayer()->GetName());

    return true;
}

bool ChatHandler::HandleShowAttackersCommand(const char* args, WorldSession *m_session)
{
    Player *plr = this->getSelectedChar(m_session, false);
    std::string name;
    Unit *target;
    if(!plr)
    {
        // try creature
        Creature *crt = this->getSelectedCreature(m_session, false);
        if(!crt)
        {
            RedSystemMessage(m_session, "A selection of a player or creature is required.");
            return true;
        }
        target = crt;
        name = crt->GetName();
    } else {
        target = plr;
        name = plr->GetName();
    }

    AttackerSet::iterator itr = target->GetAttackersetBegin();
    GreenSystemMessage(m_session, "Showing attackers for %s on minimap.", name.c_str());
    uint32 count = 0;
    for(;itr!=target->GetAttackersetEnd();++itr)
    {
        Unit *unit = World::GetUnit((*itr));
        if(!unit) continue;
        //WorldPacket *pkt = unit->BuildFieldUpdatePacket(UNIT_DYNAMIC_FLAGS, U_DYN_FLAG_UNIT_TRACKABLE);
        //m_session->SendPacket(pkt);
        //delete pkt;
        unit->BuildFieldUpdatePacket(m_session->GetPlayer(), UNIT_DYNAMIC_FLAGS, U_DYN_FLAG_UNIT_TRACKABLE);
        ++count;
    }
    SystemMessage(m_session, "Total of %d attacking units.", count);
    return true;
}

bool ChatHandler::HandleAttackerInfoCommand(const char* args, WorldSession *m_session)
{
    Player *plr = this->getSelectedChar(m_session, false);
    std::string name;
    Unit *target;
    if(!plr)
    {
        // try creature
        Creature *crt = this->getSelectedCreature(m_session, false);
        if(!crt)
        {
            RedSystemMessage(m_session, "A selection of a player or creature is required.");
            return true;
        }
        target = crt;
        name = crt->GetName();
    } else {
        target = plr;
        name = plr->GetName();
    }

    AttackerSet::iterator itr = target->GetAttackersetBegin();
    std::string add;
    GreenSystemMessage(m_session, "Showing attackers for %s.", name.c_str());
    uint32 count = 0;
    for(;itr!=target->GetAttackersetEnd();++itr)
    {
        Unit *unit = World::GetUnit((*itr));
        if(!unit) continue;
        add = "   ";
        if(unit->GetTypeId() == TYPEID_UNIT)
        {
            add += "Creature: ";
            add += ((Creature*)unit)->GetName();
        } else {
            add += "Player: ";
            add += ((Player*)unit)->GetName();
        }
        BlueSystemMessage(m_session, "%s", add.c_str());
        add.clear();
        ++count;
    }
    SystemMessage(m_session, "Total of %d attacking units.", count);
    add.clear();
    Unit *unit = World::GetUnit(target->getAttackTarget());
    add = MSG_COLOR_CYAN;
    add += "Attack Target: ";
    if(unit)
    {
        if(unit->GetTypeId() == TYPEID_UNIT)
        {
            add += "Creature: ";
            add += ((Creature*)unit)->GetName();
        } else {
            add += "Player: ";
            add += ((Player*)unit)->GetName();
        }
    } else {
        add += "None";
    }
    SystemMessage(m_session, add.c_str());
    return true;
}

bool ChatHandler::HandleResetLevelCommand(const char* args, WorldSession *m_session)
{
    Player *plr = this->getSelectedChar(m_session);
    if(!plr) return true;
    // Set level to 1 and level 1 stats
    
    plr->Reset_ToLevel1();

    SystemMessage(m_session, "Reset stats of %s to level 1. Use .levelup to change level, and .resettalents and/or .resetspells if necessary.", plr->GetName());;
    BlueSystemMessage(m_session, "%s reset all your stats to starting values.", m_session->GetPlayer()->GetName());


    sSocialMgr.SendUpdateToFriends( plr );
    return true;
}

bool ChatHandler::HandleResetTalentsCommand(const char* args, WorldSession *m_session)
{
    Player *plr = this->getSelectedChar(m_session);
    if(!plr) return true;

    plr->Reset_Talents();

    SystemMessage(m_session, "Reset talents of %s.", plr->GetName());;
    BlueSystemMessageToPlr(plr, "%s reset all your talents.", m_session->GetPlayer()->GetName());
    return true;
}

bool ChatHandler::HandleResetSpellsCommand(const char* args, WorldSession *m_session)
{
    Player *plr = this->getSelectedChar(m_session);
    if(!plr) return true;

    plr->Reset_Spells();
    
    SystemMessage(m_session, "Reset spells of %s to level 1.", plr->GetName());;
    BlueSystemMessage(m_session, "%s reset all your spells to starting values.", m_session->GetPlayer()->GetName());
    return true;
}

bool ChatHandler::HandleCreateAccountCommand(const char* args, WorldSession *m_session)
{
    char *user = strtok((char *)args, " ");
    if(!user) return false;
    char *pass = strtok(NULL, " ");
    if(!pass) return false;
    char *email = strtok(NULL, "\n");
    if(!email) return false;

    std::stringstream ss;
    sLog.outString("%s creating account: %s %s %s.", m_session->GetPlayer()->GetName(), user, pass, email);
    BlueSystemMessage(m_session, "Attempting to create account: %s, %s (Email: %s)...", user, pass, email);

    ss << "INSERT INTO accounts (login, password, email) VALUES(\""
        << user << "\",\""
        << pass << "\",\""
        << email << "\");";

    sLogonCommHandler.LogonDatabaseSQLExecute(ss.str().c_str());
    /*if(sLogonDatabase.Execute(ss.str().c_str()))
    {*/
        GreenSystemMessage(m_session, "Creation succedded. That player can now log in.");
        return true;
    /*} else {
        RedSystemMessage(m_session, "Creation failed. The player may or may not be able to log in.");
        return true;
    }*/
}

bool ChatHandler::HandleGetTransporterTime(const char* args, WorldSession* m_session)
{
    Player *plyr = m_session->GetPlayer();
    if(plyr->m_TransporterGUID == 0) return false;

    Transporter *trans = sObjHolder.GetObject<Transporter>(plyr->m_TransporterGUID);
    if(!trans) return false;

//    BlueSystemMessage(m_session, "Transporter Time: %d", trans->m_TravelTime);
    return true;
}

bool ChatHandler::HandleRemoveAurasCommand(const char *args, WorldSession *m_session)
{
    Player *plr = getSelectedChar(m_session, true);
    if(!plr) return false;

    BlueSystemMessage(m_session, "Removing all auras...");
    for(uint32 i = 0; i < MAX_AURAS; ++i)
    {
        if(plr->m_auras[i] != 0) plr->m_auras[i]->Remove();
    }
    return true;
}

bool ChatHandler::HandleParalyzeCommand(const char* args, WorldSession *m_session)
{
    //Player *plr = getSelectedChar(m_session, true);
    //if(!plr) return false;
    Unit *plr = World::GetUnit(m_session->GetPlayer()->GetSelection());
    if(!plr)
    {
        RedSystemMessage(m_session, "Invalid target.");
        return true;
    }

    BlueSystemMessage(m_session, "Rooting target.");
    WorldPacket data;
    data.Initialize(SMSG_FORCE_MOVE_ROOT);
    data << plr->GetNewGUID();
    data << uint32(1);

    plr->SendMessageToSet(&data, true);
    return true;
}

bool ChatHandler::HandleUnParalyzeCommand(const char* args, WorldSession *m_session)
{
    //Player *plr = getSelectedChar(m_session, true);
    //if(!plr) return false;
    Unit *plr = World::GetUnit(m_session->GetPlayer()->GetSelection());
    if(!plr)
    {
        RedSystemMessage(m_session, "Invalid target.");
        return true;
    }
    
    WorldPacket data;
    data.Initialize(SMSG_FORCE_MOVE_UNROOT);
    data << plr->GetNewGUID();
    data << uint32(5);

    plr->SendMessageToSet(&data, true);
    return true;
}

bool ChatHandler::HandleSetMotdCommand(const char* args, WorldSession* m_session)
{
    if(!args || strlen(args) < 2)
    {
        RedSystemMessage(m_session, "You must specify a message.");
        return true;
    }

    World::getSingleton().SetMotd(args);
    return true;
}

bool ChatHandler::HandleAddItemSetCommand(const char* args, WorldSession* m_session)
{
    uint32 setid = (args ? atoi(args) : 0);
    if(!setid)
    {
        RedSystemMessage(m_session, "You must specify a setid.");
        return true;
    }

    Player *chr = getSelectedChar(m_session);
    if (chr == NULL) {
    RedSystemMessage(m_session, "Unable to select character.");
    return true;
    }

    ItemSetEntry *entry = sItemSetStore.LookupEntry(setid);
    std::list<ItemPrototype*>* l = objmgr.GetListForItemSet(setid);
    if(!entry || !l)
    {
        RedSystemMessage(m_session, "Invalid item set.");
        return true;
    }
    const char* setname = sItemSetStore.LookupString(entry->name);
    BlueSystemMessage(m_session, "Searching item set %u (%s)...", setid, setname ? setname : "UNKNOWN");
    uint32 start = getMSTime();
    sGMLog.writefromsession(m_session, "used add item set command, set %u [%s], target %s", setid, setname, chr->GetName());
    for(std::list<ItemPrototype*>::iterator itr = l->begin(); itr != l->end(); ++itr)
    {
        Item *itm = objmgr.CreateItem((*itr)->ItemId, m_session->GetPlayer());
        if(!itm) continue;
        if(!chr->GetItemInterface()->AddItemToFreeSlot(itm))
        {
            m_session->SendNotification("No free slots left!");
            delete itm;
            return true;
        } else {
            SystemMessage(m_session, "Added item: %s [%u]", (*itr)->Name1.c_str(), (*itr)->ItemId);
        }
    }
    GreenSystemMessage(m_session, "Added set to inventory complete. Time: %u ms", getMSTime() - start);
    return true;
}

bool ChatHandler::HandleGoInstanceCommand(const char* args, WorldSession* m_session)
{
    float x,y,z;
    uint32 instanceid, mapid = 1000;
    int valcount = sscanf(args, "%u %f %f %f %u", &instanceid, &x, &y, &z, &mapid);
    if(valcount < 4)
    {
        RedSystemMessage(m_session, "Must be in format <instanceid> <x> <y> <z> <optional: mapid>");
        return true;
    }

    if(mapid != 1000)
    {
        Player *plr = m_session->GetPlayer();
        plr->SaveEntryPoint();
        plr->RemoveFromWorld();
        plr->SetInstanceID(instanceid);
        plr->SetMapId(mapid);
        plr->SetPosition(x,y,z,0,true);
        WorldPacket data(SMSG_TRANSFER_PENDING, 4);
        uint32 pid = mapid;
        data << pid;
        m_session->SendPacket(&data);
        WorldPacket data2(SMSG_NEW_WORLD, 20);
        data2 << mapid << x << y << z << (float)3.14;
        m_session->SendPacket(&data2);
    } else {
        MapMgr *instance = sWorldCreator.GetInstance(instanceid);
        if(!instance)
        {
            BlueSystemMessage(m_session, "Invalid instance specified.");
            return true;
        }
        Player *plr = m_session->GetPlayer();
        plr->SaveEntryPoint();
        plr->RemoveFromWorld();
        plr->SetInstanceID(instanceid);
        mapid = instance->GetMapId();
        plr->SetMapId(mapid);
        plr->SetPosition(x,y,z,0,true);
        WorldPacket data(SMSG_TRANSFER_PENDING, 4);
        uint32 pid = mapid;
        data << pid;
        m_session->SendPacket(&data);
        WorldPacket data2(SMSG_NEW_WORLD, 20);
        data2 << mapid << x << y << z << (float)3.14;
        m_session->SendPacket(&data2);
    }
    GreenSystemMessage(m_session, "Ported to instance %u on map %u", instanceid, mapid);
    return true;
}
bool ChatHandler::HandleCreateInstanceCommand(const char* args, WorldSession* m_session)
{
    uint32 mapid;
    int ret =sscanf(args, "%u", &mapid);
    if(!ret)
    {
        RedSystemMessage(m_session, "Needs to be in format .createinstance <mapid>");
        return true;
    }
    Map *pMap = sWorldCreator.GetMap(mapid);
    MapMgr *pMapMgr = pMap->CreateMapMgrInstance();

    GreenSystemMessage(m_session, "Instance %u on map %u created.", pMapMgr->GetInstanceID(), pMapMgr->GetMapId());
    return true;
}

bool ChatHandler::HandleExitInstanceCommand(const char* args, WorldSession* m_session)
{
    BlueSystemMessage(m_session, "Attempting to exit from instance...");
    bool result = m_session->GetPlayer()->ExitInstance();
    if(!result)
    {
        RedSystemMessage(m_session, "Entry points not found.");
        return true;
    } else {
        GreenSystemMessage(m_session, "Removal successful.");
        return true;
    }
}

bool ChatHandler::HandleCastTimeCheatCommand(const char* args, WorldSession* m_session)
{
    Player * plyr = getSelectedChar(m_session, true);
    if(!plyr) return true;

    bool val = plyr->CastTimeCheat;
    BlueSystemMessage(m_session, "%s cast time cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
    GreenSystemMessageToPlr(plyr, "%s %s a cast time cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

    plyr->CastTimeCheat = !val;
    return true;
}

bool ChatHandler::HandleCooldownCheatCommand(const char* args, WorldSession* m_session)
{
    Player * plyr = getSelectedChar(m_session, true);
    if(!plyr) return true;

    bool val = plyr->CooldownCheat;
    BlueSystemMessage(m_session, "%s cooldown cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
    GreenSystemMessageToPlr(plyr, "%s %s a cooldown cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

    plyr->CooldownCheat = !val;
    return true;
}

bool ChatHandler::HandleGodModeCommand(const char* args, WorldSession* m_session)
{
    Player * plyr = getSelectedChar(m_session, true);
    if(!plyr) return true;

    bool val = plyr->GodModeCheat;
    BlueSystemMessage(m_session, "%s godmode cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
    GreenSystemMessageToPlr(plyr, "%s %s a godmode cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

    plyr->GodModeCheat = !val;
    return true;
}

bool ChatHandler::HandlePowerCheatCommand(const char* args, WorldSession* m_session)
{
    Player * plyr = getSelectedChar(m_session, true);
    if(!plyr) return true;

    bool val = plyr->PowerCheat;
    BlueSystemMessage(m_session, "%s power cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
    GreenSystemMessageToPlr(plyr, "%s %s a power cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

    plyr->PowerCheat = !val;
    return true;
}

bool ChatHandler::HandleShowCheatsCommand(const char* args, WorldSession* m_session)
{
    Player * plyr = getSelectedChar(m_session, true);
    if(!plyr) return true;

    uint32 active = 0, inactive = 0;
#define print_cheat_status(CheatName, CheatVariable) SystemMessage(m_session, "%s%s: %s%s", MSG_COLOR_LIGHTBLUE, CheatName, \
        CheatVariable ? MSG_COLOR_LIGHTRED : MSG_COLOR_GREEN, CheatVariable ? "Active" : "Inactive");  \
        if(CheatVariable) \
        active++; \
        else \
        inactive++; 

    GreenSystemMessage(m_session, "Showing cheat status for: %s", plyr->GetName());
    print_cheat_status("Cooldown", plyr->CooldownCheat);
    print_cheat_status("CastTime", plyr->CastTimeCheat);
    print_cheat_status("GodMode", plyr->GodModeCheat);
    print_cheat_status("Power", plyr->PowerCheat);
    print_cheat_status("Fly", plyr->FlyCheat);
    print_cheat_status("AuraStack", plyr->stack_cheat);
    SystemMessage(m_session, "%u cheats active, %u inactive.", active, inactive);

#undef print_cheat_status

    return true;
}

bool ChatHandler::HandleFlyCommand(const char* args, WorldSession* m_session)
{
    WorldPacket fly(835, 13);
    
    Player *chr = getSelectedChar(m_session);
    
    if(!chr)
        chr = m_session->GetPlayer();
    
    chr->FlyCheat = true;
    fly << chr->GetNewGUID();
    fly << uint32(2);
    chr->SendMessageToSet(&fly, true);
    BlueSystemMessage(chr->GetSession(), "Flying mode enabled.");
    return 1;
}

bool ChatHandler::HandleLandCommand(const char* args, WorldSession* m_session)
{
    WorldPacket fly(836, 13);
    
    Player *chr = getSelectedChar(m_session);
    
    if(!chr)
        chr = m_session->GetPlayer();
    
    chr->FlyCheat = false;
    fly << chr->GetNewGUID();
    fly << uint32(5);
    chr->SendMessageToSet(&fly, true);
    BlueSystemMessage(chr->GetSession(), "Flying mode disabled.");
    return 1;
}

bool ChatHandler::HandleDBReloadCommand(const char* args, WorldSession* m_session)
{
    char str[200];
    uint32 mstime = getMSTime();
    sprintf(str, "%sA database reload was initiated by %s. The server may experience some lag while this occurs.",
        MSG_COLOR_LIGHTRED, m_session->GetPlayer()->GetName());
    sWorld.SendWorldText(str, 0);
    objmgr.ReloadTables();
    sprintf(str, "%sDatabase reload completed in %u ms.", MSG_COLOR_LIGHTBLUE, getMSTime() - mstime);
    sWorld.SendWorldText(str, 0);
    return true;
}

bool ChatHandler::HandleResetHPCommand(const char* args, WorldSession* m_session)
{
    Creature *pCreature = getSelectedCreature(m_session, true);
    if(pCreature == NULL)
        return true;

    pCreature->_LoadHealth();
    GreenSystemMessage(m_session, "HP reloaded for %s.", pCreature->GetName());
    return true;
}

bool ChatHandler::HandleFlySpeedCheatCommand(const char* args, WorldSession* m_session)
{
    float Speed = atof(args);
    if(Speed == 0)
        Speed = 20;

    Player * plr = getSelectedChar(m_session);
    if(plr == 0)
        return true;

    BlueSystemMessage(m_session, "Setting the fly speed of %s to %f.", plr->GetName(), Speed);
    GreenSystemMessage(plr->GetSession(), "%s set your fly speed to %f.", m_session->GetPlayer()->GetName(), Speed);
    
    WorldPacket data(SMSG_MOVE_SET_FLY_SPEED, 16);
    data << plr->GetNewGUID();
    data << uint32(0) << Speed;
    plr->SendMessageToSet(&data, true);
    
    plr->m_flySpeed = Speed;
    
    return true;
}

bool ChatHandler::HandleSpawnSpiritGuideCommand(const char* args, WorldSession * m_session)
{
    uint32 faction = args ? atol(args) : 0;
    if(faction > 1)
        faction = 1;

    CreatureInfo * pInfo = objmgr.GetCreatureName(13116 + faction);
    if(pInfo == 0)
    {
        RedSystemMessage(m_session, "Could not find entry in object manager.");
        return true;
    }

    Creature * pCreature = sObjHolder.Create<Creature>();
    if(pCreature == 0)
        return true;

    Player * plr = m_session->GetPlayer();
    pCreature->Create(pInfo->Name.c_str(), plr->GetMapId(), plr->GetPositionX(), plr->GetPositionY(),
        plr->GetPositionZ(), plr->GetOrientation());
    
    pCreature->SetInstanceID(plr->GetInstanceID());
    pCreature->SetUInt32Value(OBJECT_FIELD_ENTRY, 13116 + faction);
    pCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);

    pCreature->SetUInt32Value(UNIT_FIELD_HEALTH, 100);
    pCreature->SetUInt32Value(UNIT_FIELD_BASE_HEALTH, 100);
    pCreature->SetUInt32Value(UNIT_FIELD_POWER1, 4868);
    pCreature->SetUInt32Value(UNIT_FIELD_POWER3, 200);
    pCreature->SetUInt32Value(UNIT_FIELD_POWER5, 2000000);

    pCreature->SetUInt32Value(UNIT_FIELD_MAXHEALTH, 100);
    pCreature->SetUInt32Value(UNIT_FIELD_MAXPOWER1, 4868);
    pCreature->SetUInt32Value(UNIT_FIELD_MAXPOWER3, 200);
    pCreature->SetUInt32Value(UNIT_FIELD_MAXPOWER5, 2000000);

    pCreature->SetUInt32Value(UNIT_FIELD_LEVEL, 60);
    pCreature->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, 84 - faction);
    pCreature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0 | (2 << 8) | (1 << 16));

    pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 22802);
    pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, 2 | (0xA << 8) | (2 << 16) | (0x11 << 24));
    pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO_01, 2);

    pCreature->SetUInt32Value(UNIT_FIELD_FLAGS, 4928);

    pCreature->SetUInt32Value(UNIT_FIELD_AURA, 22011);
    pCreature->SetUInt32Value(UNIT_FIELD_AURAFLAGS, 9);
    pCreature->SetUInt32Value(UNIT_FIELD_AURALEVELS, 0x3C);
    pCreature->SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS, 0xFF);

    pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 2000);
    pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_01, 2000);
    pCreature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.208f);
    pCreature->SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);

    pCreature->SetUInt32Value(UNIT_FIELD_DISPLAYID, 13337);
    pCreature->SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, 13337);
    
    pCreature->SetUInt32Value(UNIT_CHANNEL_SPELL, 22011);
    pCreature->SetUInt32Value(UNIT_MOD_CAST_SPEED, 1065353216);

    pCreature->SetUInt32Value(UNIT_NPC_FLAGS, 35);
    pCreature->SetUInt32Value(UNIT_FIELD_BYTES_2, 1 | (0x10 << 8));

    pCreature->DisableAI();
    pCreature->AddToWorld();

    BlueSystemMessage(m_session, "Spawned %s spirit healer.", faction ? "horde" : "alliance");
    return true;
}

bool ChatHandler::HandleModifyLevelCommand(const char* args, WorldSession* m_session)
{
    Player * plr = getSelectedChar(m_session, true);
    if(plr == 0) return true;

    uint32 Level = args ? atol(args) : 0;
    if(Level == 0)
    {
        RedSystemMessage(m_session, "A level (numeric) is required to be specified after this command.");
        return true;
    }

    // Set level message
    BlueSystemMessage(m_session, "Setting the level of %s to %u.", plr->GetName(), Level);
    GreenSystemMessageToPlr(plr, "%s set your level to %u.", m_session->GetPlayer()->GetName(), Level);

    sGMLog.writefromsession(m_session, "used modify level on %s, level %u", plr->GetName(), Level);

    // lookup level information
    LevelInfo * Info = objmgr.GetLevelInfo(plr->getRace(), plr->getClass(), Level);
    if(Info == 0)
    {
        RedSystemMessage(m_session, "Levelup information not found.");
        return true;
    }

    plr->ApplyLevelInfo(Info, Level);
    return true;
}

bool ChatHandler::HandleCreatePetCommand(const char* args, WorldSession* m_session)
{
    if(!args || strlen(args) < 2)
        return false;

    uint32 Entry = atol(args);
    if(!Entry)
        return false;

    CreatureSpawnTemplate * pTemplate = objmgr.GetCreatureSpawnTemplate(Entry);
    CreatureInfo * pCreatureInfo = objmgr.GetCreatureName(Entry);
    if(!pTemplate || !pCreatureInfo)
    {
        RedSystemMessage(m_session, "Invalid creature spawn template: %u", Entry);
        return true;
    }

    // spawn a creature of this id to create from
    Creature * pCreature = sObjHolder.Create<Creature>();
    Player * plr = m_session->GetPlayer();
    pCreature->Create(pTemplate, plr->GetMapId(), plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), 0);

    Pet *old_tame = plr->GetSummon();
    if(old_tame != NULL)
    {
        old_tame->Dismiss(true);
    }

    // create a pet from this creature
    Pet * pPet = sObjHolder.Create<Pet>();
    pPet->SetInstanceID(plr->GetInstanceID());
    pPet->CreateAsSummon(Entry, pCreatureInfo, pCreature, plr, NULL, 0x2, 0);
    
    // remove the temp creature
    sObjHolder.Delete<Creature>(pCreature);

    return true;
}

bool ChatHandler::HandleAddPetSpellCommand(const char* args, WorldSession* m_session)
{
    Player * plr = m_session->GetPlayer();
    Pet * pPet = plr->GetSummon();
    if(pPet == 0)
    {
        RedSystemMessage(m_session, "You have no pet.");
        return true;
    }

    uint32 SpellId = atol(args);
    SpellEntry * spell = sSpellStore.LookupEntry(SpellId);
    if(!SpellId || !spell)
    {
        RedSystemMessage(m_session, "Invalid spell id requested.");
        return true;
    }

    pPet->AddSpell(spell);
    GreenSystemMessage(m_session, "Added spell %u to your pet.", SpellId);
    return true;
}

bool ChatHandler::HandleRemovePetSpellCommand(const char* args, WorldSession* m_session)
{
    Player * plr = m_session->GetPlayer();
    Pet * pPet = plr->GetSummon();
    if(pPet == 0)
    {
        RedSystemMessage(m_session, "You have no pet.");
        return true;
    }

    uint32 SpellId = atol(args);
    SpellEntry * spell = sSpellStore.LookupEntry(SpellId);
    if(!SpellId || !spell)
    {
        RedSystemMessage(m_session, "Invalid spell id requested.");
        return true;
    }

    pPet->RemoveSpell(SpellId);
    GreenSystemMessage(m_session, "Added spell %u to your pet.", SpellId);
    return true;
}

bool ChatHandler::HandleEnableRenameCommand(const char* args, WorldSession* m_session)
{
    Player * plr = m_session->GetPlayer();
    Pet * pPet = plr->GetSummon();
    if(pPet == 0)
    {
        RedSystemMessage(m_session, "You have no pet.");
        return true;
    }

    // todo: set flags
    return true;
}

bool ChatHandler::HandleRenamePetCommand(const char* args, WorldSession* m_session)
{
    Player * plr = m_session->GetPlayer();
    Pet * pPet = plr->GetSummon();
    if(pPet == 0)
    {
        RedSystemMessage(m_session, "You have no pet.");
        return true;
    }

    if(strlen(args) < 1)
    {
        RedSystemMessage(m_session, "You must specify a name.");
        return true;
    }

    GreenSystemMessage(m_session, "Renamed your pet to %s.", args);
    pPet->Rename(args);
    return true;
}

bool ChatHandler::HandleShutdownCommand(const char* args, WorldSession* m_session)
{
    uint32 shutdowntime = atol(args);
    if(!args)
        shutdowntime = 5;

    shutdowntime *= 1000;
    char msg[500];
    sprintf(msg, "%sServer shutdown initiated by %s, shutting down in %u seconds.", MSG_COLOR_LIGHTBLUE,
        m_session->GetPlayer()->GetName(), shutdowntime);

    sWorld.SendWorldText(msg);
    sMaster.m_ShutdownTimer = shutdowntime;
    sMaster.m_ShutdownEvent = true;
    sMaster.m_restartEvent = false;
    return true;
}

bool ChatHandler::HandleShutdownRestartCommand(const char* args, WorldSession* m_session)
{
    uint32 shutdowntime = atol(args);
    if(!args)
        shutdowntime = 5;
    shutdowntime *= 1000;

    char msg[500];
    sprintf(msg, "%sServer restart initiated by %s, shutting down in %u seconds.", MSG_COLOR_LIGHTBLUE,
        m_session->GetPlayer()->GetName(), shutdowntime);

    sWorld.SendWorldText(msg);
    sMaster.m_ShutdownTimer = shutdowntime;
    sMaster.m_ShutdownEvent = true;
    sMaster.m_restartEvent = true;
    return true;
}

bool ChatHandler::HandleAllowWhispersCommand(const char* args, WorldSession* m_session)
{
    if(args == 0 || strlen(args) < 2) return false;
    Player * plr = objmgr.GetPlayer(args, false);
    if(!plr)
    {
        RedSystemMessage(m_session, "Player not found.");
        return true;
    }

    m_session->GetPlayer()->gmTargets.insert(plr);
    BlueSystemMessage(m_session, "Now accepting whispers from %s.", plr->GetName());
    return true;
}

bool ChatHandler::HandleBlockWhispersCommand(const char* args, WorldSession* m_session)
{
    if(args == 0 || strlen(args) < 2) return false;
    Player * plr = objmgr.GetPlayer(args, false);
    if(!plr)
    {
        RedSystemMessage(m_session, "Player not found.");
        return true;
    }

    m_session->GetPlayer()->gmTargets.erase(plr);
    BlueSystemMessage(m_session, "Now blocking whispers from %s.", plr->GetName());
    return true;
}

bool ChatHandler::HandleAdvanceAllSkillsCommand(const char* args, WorldSession* m_session)
{
    uint32 amt = args ? atol(args) : 0;
    if(!amt)
    {
        RedSystemMessage(m_session, "An amount to increment is required.");
        return true;
    }

    Player * plr = getSelectedChar(m_session);
    if(!plr)
        return true;

    uint32 id;
    uint32 val;
    uint32 max;
    // loop the skills
    for(uint32 i = PLAYER_SKILL_INFO_1_1; i < PLAYER_SKILL_INFO_1_1+3*127; i += 3)
    {
        id = (uint16)plr->GetUInt32Value(i);
        if(id != 0)
        {
            val = plr->GetBaseSkillAmt(id);
            max = plr->GetSkillMax(id);
            if(val >= max)
                continue;

            // figure out the amount we have to add
            int32 add = amt;
            if((val + add) > max)
                add = max - val;

            if(add > 0)
            {
                for(uint32 j = 0; j < add; ++j)
                    plr->AdvanceSkillLine(id);
            }
        }
    }

    GreenSystemMessageToPlr(plr, "Advanced all your skill lines by %u points.", amt);
    return true;
}

bool ChatHandler::HandleKillByPlayerCommand(const char* args, WorldSession* m_session)
{
    if(!args || strlen(args) < 2)
    {
        RedSystemMessage(m_session, "A player's name is required.");
        return true;
    }

    Player * plr = objmgr.GetPlayer(args, false);
    if(plr == 0)
    {
        RedSystemMessage(m_session, "Player %s not found.", args);
        return true;
    }

    GreenSystemMessage(m_session, "Disconnecting %s.", plr->GetName());
    plr->GetSession()->Disconnect();
    return true;
}

bool ChatHandler::HandleKillBySessionCommand(const char* args, WorldSession* m_session)
{
    if(!args || strlen(args) < 2)
    {
        RedSystemMessage(m_session, "A player's name is required.");
        return true;
    }

    WorldSession * session = sWorld.FindSessionByName(args);
    if(session == 0)
    {
        RedSystemMessage(m_session, "Active session with name %s not found.", args);
        return true;
    }

    GreenSystemMessage(m_session, "Disconnecting %s.", session->GetAccountName().c_str());
    session->Disconnect();
    return true;
}

bool ChatHandler::HandleUnlockMovementCommand(const char* args, WorldSession* m_session)
{
    Player * plr = getSelectedChar(m_session);
    if(!plr) return true;

    GreenSystemMessage(m_session, "Unlocking movement.");
    WorldPacket data(50);
    data.SetOpcode(SMSG_MOVE_UNLOCK_MOVEMENT);
    data << uint32(0);
    plr->GetSession()->SendPacket(&data);

    plr->SetMovement(MOVE_UNROOT, 1);
    return true;
}

bool ChatHandler::HandleCastAllCommand(const char* args, WorldSession* m_session)
{
    if(!args || strlen(args) < 2)
    {
        RedSystemMessage(m_session, "No spellid specified.");
        return true;
    }
    Player * plr;
    uint32 spellid = atol(args);
    SpellEntry * info = sSpellStore.LookupEntry(spellid);
    if(!info)
    {
        RedSystemMessage(m_session, "Invalid spell specified.");
        return true;
    }

    // this makes sure no moron casts a learn spell on everybody and wrecks the server
    for (int i = 0; i < 3; i++)
    {
        if (info->Effect[i] == 36) //SPELL_EFFECT_LEARN_SPELL - 36
        {
            sGMLog.writefromsession(m_session, "used wrong / learnall castall command, spellid %u", spellid);
            RedSystemMessage(m_session, "Learn spell specified.");
            return true;
        }
    }

    sGMLog.writefromsession(m_session, "used castall command, spellid %u", spellid);

    HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;
    for (itr = sObjHolder.Begin<Player>(); itr != sObjHolder.End<Player>(); itr++)
    {
        plr = itr->second;
        if(plr->GetSession() && plr->IsInWorld())
        {
            Spell * sp = new Spell(plr, info, true, 0);
            SpellCastTargets targets(plr->GetGUID());
            sp->prepare(&targets);
        }
    }

    BlueSystemMessage(m_session, "Casted spell %u on all players!", spellid);
    return true;
}

bool ChatHandler::HandleNpcReturnCommand(const char* args, WorldSession* m_session)
{
    Creature * creature = getSelectedCreature(m_session);
    if(!creature) return true;

    // return to respawn coords
    float x = creature->respawn_cord[0];
    float y = creature->respawn_cord[1];
    float z = creature->respawn_cord[2];
    float o = creature->respawn_cord[3];
    
    // restart movement
    creature->GetAIInterface()->SetAIState(STATE_IDLE);
    creature->GetAIInterface()->WipeHateList();
    creature->GetAIInterface()->WipeTargetList();
    creature->GetAIInterface()->MoveTo(x, y, z, o);

    return true;
}

bool ChatHandler::HandleSetRateCommand(const char* args, WorldSession* m_session)
{
    if(!args) return false;
    float rateval;
    uint32 rate;
    if(sscanf(args, "%u %f", &rate, &rateval) != 2 || rate >= MAX_RATES)
    {
        RedSystemMessage(m_session, "Command must be specified in the format <rate> <value>.");
        return true;
    }

    sWorld.setRate(rate, rateval);
    GreenSystemMessage(m_session, "Set rate %u to %f.", rate, rateval);
    return true;
}

bool ChatHandler::HandleGetRateCommand(const char* args, WorldSession* m_session)
{
    uint32 rate = args ? atol(args) : 0;
    if(rate >= MAX_RATES)
    {
        RedSystemMessage(m_session, "Bad rate id.");
        return true;
    }

    float rv = sWorld.getRate(rate);
    GreenSystemMessage(m_session, "Rate %u is set to %f.", rate, rv);
    return true;
}

bool ChatHandler::HandleModPeriodCommand(const char* args, WorldSession * m_session)
{
    Transporter * trans = m_session->GetPlayer()->m_CurrentTransporter;
    if(trans == 0)
    {
        RedSystemMessage(m_session, "You must be on a transporter.");
        return true;
    }

    uint32 np = args ? atol(args) : 0;
    if(np == 0)
    {
        RedSystemMessage(m_session, "A time in ms must be specified.");
        return true;
    }

    trans->SetPeriod(np);
    BlueSystemMessage(m_session, "Period of %s set to %u.", trans->GetInfo()->Name.c_str(), np);
    return true;
}

bool ChatHandler::HandleFormationLink1Command(const char* args, WorldSession * m_session)
{
    // set formation "master"
    Creature * pCreature = getSelectedCreature(m_session, true);
    if(pCreature == 0) return true;

    m_session->GetPlayer()->linkTarget = pCreature;
    BlueSystemMessage(m_session, "Linkup \"master\" set to %s.", pCreature->GetName());
    return true;
}

bool ChatHandler::HandleFormationLink2Command(const char* args, WorldSession * m_session)
{
    // set formation "slave" with distance and angle
    float ang, dist;
    if(*args == 0 || sscanf(args, "%f %f", &dist, &ang) != 2)
    {
        RedSystemMessage(m_session, "You must specify a distance and angle.");
        return true;
    }

    if(m_session->GetPlayer()->linkTarget == 0 || m_session->GetPlayer()->linkTarget->GetTypeId() != TYPEID_UNIT)
    {
        RedSystemMessage(m_session, "Master not selected. select the master, and use formationlink1.");
        return true;
    }

    Creature * slave = getSelectedCreature(m_session, true);
    if(slave == 0) return true;

    slave->GetAIInterface()->m_formationFollowDistance = dist;
    slave->GetAIInterface()->m_formationFollowAngle = ang;
    slave->GetAIInterface()->m_formationLinkTarget = ((Creature*)m_session->GetPlayer()->linkTarget);
    slave->GetAIInterface()->m_formationLinkSqlId = slave->GetAIInterface()->m_formationLinkTarget->GetSQL_id();
    slave->GetAIInterface()->SetUnitToFollowAngle(ang);
    
    // add to db
    sDatabase.Execute("INSERT INTO creature_formations VALUES(%u, %u, '%f', '%f')", 
        slave->GetSQL_id(), slave->GetAIInterface()->m_formationLinkSqlId, ang, dist);

    BlueSystemMessage(m_session, "%s linked up to %s with a distance of %f at %f radians.", slave->GetName(), 
        ((Creature*)m_session->GetPlayer()->linkTarget)->GetName(), dist, ang);

    return true;
}

bool ChatHandler::HandleNpcFollowCommand(const char* args, WorldSession * m_session)
{
    Creature * creature = getSelectedCreature(m_session, true);
    if(!creature) return true;

    creature->GetAIInterface()->SetUnitToFollow(m_session->GetPlayer());
    return true;
}

bool ChatHandler::HandleFormationClearCommand(const char* args, WorldSession * m_session)
{
    Creature * c = getSelectedCreature(m_session, true);
    if(!c) return true;

    c->GetAIInterface()->m_formationLinkSqlId = 0;
    c->GetAIInterface()->m_formationLinkTarget = 0;
    c->GetAIInterface()->m_formationFollowAngle = 0.0f;
    c->GetAIInterface()->m_formationFollowDistance = 0.0f;
    c->GetAIInterface()->SetUnitToFollow(0);
    
    sDatabase.Execute("DELETE FROM creature_formations WHERE creature_sqlid=%u", c->GetSQL_id());
    return true;
}

bool ChatHandler::HandleNullFollowCommand(const char* args, WorldSession * m_session)
{
    Creature * c = getSelectedCreature(m_session, true);
    if(!c) return true;

    // restart movement
    c->GetAIInterface()->SetAIState(STATE_IDLE);
    c->GetAIInterface()->SetUnitToFollow(0);
    return true;
}

bool ChatHandler::HandleStackCheatCommand(const char* args, WorldSession * m_session)
{
    Player * plyr = getSelectedChar(m_session, true);
    if(!plyr) return true;

    bool val = plyr->stack_cheat;
    BlueSystemMessage(m_session, "%s aura stack cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
    GreenSystemMessageToPlr(plyr, "%s %s an aura stack cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

    plyr->stack_cheat = !val;
    return true;
}

bool ChatHandler::HandleResetSkillsCommand(const char* args, WorldSession * m_session)
{
    Player * plr = getSelectedChar(m_session, true);
    if(!plr) return true;

    for(uint32 i = PLAYER_SKILL_INFO_1_1; i < PLAYER_CHARACTER_POINTS1; ++i)
        plr->SetUInt32Value(i, 0);

    // Load skills from create info.
    PlayerCreateInfo * info = objmgr.GetPlayerCreateInfo(plr->getRace(), plr->getClass());
    if(!info) return true;

    for(std::list<CreateInfo_SkillStruct>::iterator ss = info->skills.begin(); ss!=info->skills.end(); ss++)
    {
        if(ss->skillid && ss->currentval && ss->maxval)
            plr->AddSkillLine(ss->skillid, ss->currentval, ss->maxval);        
    }
    //Chances depend on stats must be in this order!
    plr->UpdateStats();
    plr->UpdateChances();
    BlueSystemMessage(m_session, "Reset skills to default.");
    return true;
}

bool ChatHandler::HandlePlayerInfo(const char* args, WorldSession * m_session)
{
    Player* plr = getSelectedChar(m_session, true);
    if(!plr) return true;
    if(!plr->GetSession())
    {
        RedSystemMessage(m_session, "ERROR: this player hasn't got any session !");
        return true;
    }
    if(!plr->GetSession()->GetSocket())
    {
        RedSystemMessage(m_session, "ERROR: this player hasn't got any socket !");
        return true;
    }
    WorldSession* sess = plr->GetSession();

    char* infos = new char[128];
    static const char* classes[12] =
    {"None","Warrior", "Paladin", "Hunter", "Rogue", "Priest", "None", "Shaman", "Mage", "Warlock", "None", "Druid"};
    static const char* races[12] =
    {"None","Human","Orc","Dwarf","Night Elf","Undead","Tauren","Gnome","Troll","None","Blood Elf","Draenei"};

    char playedLevel[64];
    char playedTotal[64];

    int seconds = (plr->GetPlayedtime())[0];
    int mins=0;
    int hours=0;
    int days=0;
    if(seconds >= 60)
    {
        mins = seconds / 60;
        if(mins)
        {
            seconds -= mins*60;
            if(mins >= 60)
            {
                hours = mins / 60;
                if(hours)
                {
                    mins -= hours*60;
                    if(hours >= 24)
                    {
                        days = hours/24;
                        if(days)
                            hours -= days*24;
                    }
                }
            }
        }
    }
    sprintf(playedLevel, "[%d days, %d hours, %d minutes, %d seconds]", days, hours, mins, seconds);

    seconds = (plr->GetPlayedtime())[1];
    mins=0;
    hours=0;
    days=0;
    if(seconds >= 60)
    {
        mins = seconds / 60;
        if(mins)
        {
            seconds -= mins*60;
            if(mins >= 60)
            {
                hours = mins / 60;
                if(hours)
                {
                    mins -= hours*60;
                    if(hours >= 24)
                    {
                        days = hours/24;
                        if(days)
                            hours -= days*24;
                    }
                }
            }
        }
    }
    sprintf(playedTotal, "[%d days, %d hours, %d minutes, %d seconds]", days, hours, mins, seconds);

    GreenSystemMessage(m_session, "%s is a %s %s %s", plr->GetName(),
        (plr->getGender()?"Female":"Male"), races[plr->getRace()], classes[plr->getClass()]);

    BlueSystemMessage(m_session, "%s has played %s at this level",(plr->getGender()?"She":"He"), playedLevel);
    BlueSystemMessage(m_session, "and %s overall", playedTotal);

    BlueSystemMessage(m_session, "%s is connecting from account '%s'[%u] with permissions '%s'",
        (plr->getGender()?"She":"He"), sess->GetAccountName().c_str(), sess->GetAccountId(), sess->GetPermissions());

    BlueSystemMessage(m_session, "%s uses %s (build %u)", (plr->getGender()?"She":"He"),
        (sess->HasFlag(ACCOUNT_FLAG_XPACK_01)?"WoW Burning Crusade":"WoW"), sess->GetClientBuild());

    BlueSystemMessage(m_session, "%s IP is '%s', and has a latency of %ums", (plr->getGender()?"Her":"His"),
        sess->GetSocket()->RetreiveClientIP(), sess->GetLatency());

    return true;
}
