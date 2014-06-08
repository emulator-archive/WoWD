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

bool ChatHandler::HandleRecallGoCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        return false;

    QueryResult *result = sDatabase.Query( "SELECT * FROM recall" );

    do
    {
        Field *fields = result->Fetch();
        const char * locname = fields[1].GetString();
        uint32 locmap = fields[2].GetUInt32();
        float x = fields[3].GetFloat();
        float y = fields[4].GetFloat();
        float z = fields[5].GetFloat();

        if (strnicmp((char*)args,locname,strlen(args))==0)
        {
            m_session->GetPlayer()->SafeTeleport(locmap, 0, LocationVector(x, y, z));
            delete result;
            return true;
        }

    }while (result->NextRow());

    delete result;
    return false;
}

bool ChatHandler::HandleRecallAddCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        return false;
    
    QueryResult *result = sDatabase.Query( "SELECT locname FROM recall" );
    do
    {
        Field *fields = result->Fetch();
        const char * locname = fields[0].GetString();
        if (strncmp((char*)args,locname,strlen(locname))==0)
        {
            RedSystemMessage(m_session, "Name in use, please use another name for your location.");
            delete result;
            return true;
        }
    }while (result->NextRow());
    delete result;

    Player* plr = m_session->GetPlayer();
    std::stringstream ss;
    
    const char * rc_locname = args;

    ss << "INSERT INTO recall (locname, mapid, positionX, positionY, positionZ) VALUES ('"
    << rc_locname << "' , "
    << plr->GetMapId() << ", "
    << plr->GetPositionX() << ", " 
    << plr->GetPositionY() << ", "
    << plr->GetPositionZ() << ");";
    sDatabase.Execute( ss.str( ).c_str( ) );

    char buf[256]; 
    sprintf((char*)buf, "Added location to DB with MapID: %d, X: %f, Y: %f, Z: %f",
        plr->GetMapId(), plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ());
    GreenSystemMessage(m_session, buf);
    sGMLog.writefromsession(m_session, "used recall add, added \"%s\" location to database.", rc_locname);

    return true;
}

bool ChatHandler::HandleRecallDelCommand(const char* args, WorldSession *m_session)
{
       if(!*args)
        return false;

    QueryResult *result = sDatabase.Query( "SELECT id,locname FROM recall" );

    do
    {
        Field *fields = result->Fetch();
        float id = fields[0].GetFloat();
        const char * locname = fields[1].GetString();

        if (strnicmp((char*)args,locname,strlen(locname))==0)
        {
            std::stringstream ss;
            ss << "DELETE FROM recall WHERE id = "<< (int)id <<";";
            sDatabase.Execute( ss.str( ).c_str( ) );
            GreenSystemMessage(m_session, "Recall location removed.");
            sGMLog.writefromsession(m_session, "used recall delete, removed \"%s\" location from database.", args);
            delete result;
            return true;
        }

    }while (result->NextRow());

    delete result;
    return false;
}

bool ChatHandler::HandleRecallListCommand(const char* args, WorldSession *m_session)
{
    QueryResult *result = sDatabase.Query( "SELECT id,locname FROM recall" );
    std::string recout;
    uint32 count = 0;

    recout = "|cff00ff00Recall locations|r:\n\n";
    do
    {
        Field *fields = result->Fetch();
        float id = fields[0].GetFloat();
        const char * locname = fields[1].GetString();
        recout += "|cff00ccff";
        recout += locname;
        recout += "|r, ";
        count++;
        
        if(count == 5)
        {
            recout += "\n";
            count = 0;
        }
    }while (result->NextRow());
    SendMultilineMessage(m_session, recout.c_str());

    delete result;
    return true;
}