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

///////////////////////////////////////////////
//  Admin Movement Commands
//

#include "StdAfx.h"

bool ChatHandler::HandleResetReputationCommand(const char *args, WorldSession *m_session)
{
    Player *plr = getSelectedChar(m_session);
    if(!plr)
    {
        SystemMessage(m_session, "Select a player or yourself first.");
        return true;
    }
    
    plr->_InitialReputation();
    SystemMessage(m_session, "Done. Relog for changes to take effect.");
    return true;
}

bool ChatHandler::HandleInvincibleCommand(const char *args, WorldSession *m_session)
{
    /*Player *chr = getSelectedChar(m_session);
    char msg[100];
    if(chr)
    {
        chr->m_invincible = !chr->m_invincible;
        sprintf(msg, "Invincibility is now %s", chr->m_invincible ? "ON. You may have to leave and re-enter this zone for changes to take effect." : "OFF. Exit and re-enter this zone for this change to take effect.");
    } else {
        sprintf(msg, "Select a player or yourself first.");
    }
    SystemMessage(m_session, msg);*/
    return true;
}

bool ChatHandler::HandleInvisibleCommand(const char *args, WorldSession *m_session)
{
    char msg[256];
    Player* pChar =m_session->GetPlayer();

    sprintf(msg, "Invisibility and Invincibility are now ");
    if(pChar->m_isGmInvisible)
    {
        pChar->m_isGmInvisible = false;
        //pChar->m_invincible = false;
        sSocialMgr.LoggedOut(pChar);
        sprintf(msg, "%s OFF.", msg);
    } else {
        pChar->m_isGmInvisible = true;
        //pChar->m_invincible = true;
        sSocialMgr.LoggedIn(pChar);
        sprintf(msg, "%s ON.", msg);
    }

    sprintf(msg, "%s You may have to leave and re-enter this zone for changes to take effect.", msg);

    GreenSystemMessage(m_session, (const char*)msg);
    return true;
}

bool ChatHandler::HandleGUIDCommand(const char* args, WorldSession *m_session)
{
    uint64 guid;
    guid = m_session->GetPlayer()->GetSelection();
    if (guid == 0)
    {
       SystemMessage(m_session, "No selection.");
        return true;
    }

    char buf[256];
    sprintf((char*)buf,"Object guid is: lowpart %u highpart %X", GUID_LOPART(guid), GUID_HIPART(guid));
   SystemMessage(m_session,  buf);
    return true;
}

bool ChatHandler::CreateGuildCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        return false;

    if(strlen((char*)args)>75)
    {
        // send message to user
        char buf[256];
        sprintf((char*)buf,"The name was too long by %i", strlen((char*)args)-75);
        SystemMessage(m_session, buf);
        return true;
    }

    for (int i = 0; i < strlen(args); i++) {
        if(!isalpha(args[i]) && args[i]!=' ') {
            SystemMessage(m_session, "Error, name can only contain chars A-Z and a-z.");
            return true;
        }
    }

    if(objmgr.GetGuildByGuildName(args))
    {
        WorldPacket data(SMSG_GUILD_COMMAND_RESULT, 100);
        data << uint32(0);
        data << args;
        data << uint32(C_R_GUILD_NAME_EXISTS);
        m_session->SendPacket(&data);
        return true;
    }

    Guild *pGuild = new Guild;
    uint32 guildId = pGuild->GetFreeGuildIdFromDb();

    if(guildId == 0)
    {
        printf("Error Getting Free Guild ID");
        delete pGuild;
        return false;
    }

    //Guild Setup
    pGuild->SetGuildId( guildId );
    pGuild->SetGuildName( args );
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

    Player *plyr = m_session->GetPlayer();

    //Guild Leader Setup
    plyr->SetGuildId( pGuild->GetGuildId() );
    plyr->SetUInt32Value(PLAYER_GUILDID, pGuild->GetGuildId() );
    plyr->SetGuildRank(GUILDRANK_GUILD_MASTER);
    plyr->SetUInt32Value(PLAYER_GUILDRANK,GUILDRANK_GUILD_MASTER);
    pGuild->SetGuildLeaderGuid( plyr->GetGUID() );
    pGuild->AddNewGuildMember( plyr );

    pGuild->SaveToDb();
    pGuild->SaveRanksToDb();

    return true;
}

bool ChatHandler::HandleNameCommand(const char* args, WorldSession *m_session)
{

    if(!*args)
        return false;

    if(strlen((char*)args)>75)
    {
        // send message to user
        char buf[256];
        sprintf((char*)buf,"The name was too long by %i", strlen((char*)args)-75);
        SystemMessage(m_session, buf);
        return true;
    }

    for (int i = 0; i < strlen(args); i++) {
        if(!isalpha(args[i]) && args[i]!=' ') {
            SystemMessage(m_session, "Error, name can only contain chars A-Z and a-z.");
            return false;
        }
    }

    uint64 guid;
    guid = m_session->GetPlayer()->GetSelection();
    if (guid == 0)
    {
        SystemMessage(m_session, "No selection.");
        return true;
    }

    Creature * pCreature = sObjHolder.GetObject<Creature>(guid);
    if(!pCreature)
    {
        SystemMessage(m_session, "You should select a creature.");
        return true;
    }

    pCreature->SetName(args);
    uint32 idname = objmgr.AddCreatureName(pCreature->GetName());
    pCreature->SetUInt32Value(OBJECT_FIELD_ENTRY, idname);

    pCreature->SaveToDB();

    return true;
}

bool ChatHandler::HandleSubNameCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        args = "";

    if(strlen((char*)args)>75)
    {
        // send message to user
        char buf[256];
        sprintf((char*)buf,"The subname was too long by %i", strlen((char*)args)-75);
        SystemMessage(m_session, buf);
        return true;
    }

    for (int i = 0; i < strlen(args); i++) {
        if(!isalpha(args[i]) && args[i]!=' ') {
            SystemMessage(m_session, "Error, name can only contain chars A-Z and a-z.");
            return false;
        }
    }
    uint64 guid;
    guid = m_session->GetPlayer()->GetSelection();
    if (guid == 0)
    {
        SystemMessage(m_session, "No selection.");
        return true;
    }

    Creature * pCreature = sObjHolder.GetObject<Creature>(guid);
    if(!pCreature)
    {
        SystemMessage(m_session, "You should select a creature.");
        return true;
    }

    uint32 idname = objmgr.AddCreatureSubName(pCreature->GetName(),args,pCreature->GetUInt32Value(UNIT_FIELD_DISPLAYID));
    pCreature->SetUInt32Value(OBJECT_FIELD_ENTRY, idname);

    pCreature->SaveToDB();

    return true;
}
/*
#define isalpha(c)  {isupper(c) || islower(c))
#define isupper(c)  (c >=  'A' && c <= 'Z')
#define islower(c)  (c >=  'a' && c <= 'z')
*/

bool ChatHandler::HandleSpawnEntryCommand(const char* args, WorldSession *m_session)
{

    std::stringstream sstext;

    char* pEntryID = strtok((char*)args, " ");
    if (!pEntryID)
        return false;

    uint32 EntryID  = atoi(pEntryID);
    CreatureSpawnTemplate *ct = objmgr.GetCreatureSpawnTemplate(EntryID);
    if(!ct)
    {
        sstext << "Creature Spawn Template '" << EntryID << "' Not Found" << '\0';
        SystemMessage(m_session, sstext.str().c_str());
        return true;
    }

    sLog.outDebug("Spawning Creature Template '%u'", EntryID);
    sstext << "Spawning Creature using Template '" << EntryID << "'" << '\0';
    SystemMessage(m_session, sstext.str().c_str());

    CreatureInfo *ci = objmgr.GetCreatureName(EntryID);
    if(!ci)
        return true;

    // Create the requested monster
    Player *chr = m_session->GetPlayer();
    float x = chr->GetPositionX();
    float y = chr->GetPositionY();
    float z = chr->GetPositionZ();
    float o = chr->GetOrientation();

    sGMLog.writefromsession(m_session, "used spawn command, entry %u [%s] at %f %f %f", EntryID, ci->Name.c_str(),
        x, y, z);

    Creature* pCreature = sObjHolder.Create<Creature>();

    pCreature->SetInstanceID(chr->GetInstanceID());
    pCreature->SetZoneId(chr->GetZoneId());

    pCreature->Create(ct, chr->GetMapId(), x, y, z, o);
    pCreature->SetCreatureName(ci);

    pCreature->AddToWorld();
    CreatureTemplate *t = new CreatureTemplate;
    t->Create(pCreature);
    pCreature->GetMapMgr()->GetBaseMap()->GetTemplate()->AddIndex<Creature>(pCreature, t);

    pCreature->SaveToDB();
    return true;
}

bool ChatHandler::HandleDeleteCommand(const char* args, WorldSession *m_session)
{

    uint64 guid = m_session->GetPlayer()->GetSelection();
    if (guid == 0)
    {
        SystemMessage(m_session, "No selection.");
        return true;
    }

    Creature *unit = sObjHolder.GetObject<Creature>(guid);
    if(!unit)
    {
        SystemMessage(m_session, "You should select a creature.");
        return true;
    }
    sGMLog.writefromsession(m_session, "used npc delete, sqlid %u, creature %s, pos %f %f %f",
        unit->GetSQL_id(), unit->GetCreatureName()->Name.c_str(), unit->GetPositionX(), unit->GetPositionY(),
        unit->GetPositionZ());

    if(unit->IsInWorld())
    {
        unit->GetMapMgr()->GetBaseMap()->GetTemplate()->RemoveIndex<Creature>(unit);    // remove index
        unit->RemoveFromWorld();
    }
    unit->DeleteFromDB();
    

    //sWorld.RemoveGlobalObject((Object*)unit);
    sObjHolder.Delete(unit);

    return true;
}

bool ChatHandler::HandleDeMorphCommand(const char* args, WorldSession *m_session)
{
    sLog.outError("Demorphed %s",m_session->GetPlayer()->GetName());
    m_session->GetPlayer()->DeMorph();
    return true;
}

bool ChatHandler::HandleItemCommand(const char* args, WorldSession *m_session)
{
    char* pitem = strtok((char*)args, " ");
    if (!pitem)
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

    uint32 item = atoi(pitem);
    int amount = -1;

    char* pamount = strtok(NULL, " ");
    if (pamount)
        amount = atoi(pamount);

    ItemPrototype* tmpItem = objmgr.GetItemPrototype(item);

    std::stringstream sstext;
    if(tmpItem)
    {
        std::stringstream ss;
        ss << "INSERT INTO vendors VALUES ('" << pCreature->GetUInt32Value(OBJECT_FIELD_ENTRY) << "', '" << item << "', '" << amount << "')" << '\0';
        sDatabase.Execute( ss.str().c_str() );

        pCreature->AddVendorItem(item, amount);

        sstext << "Item '" << item << "' '" << tmpItem->Name1 << "' Added to list" << '\0';
    }
    else
    {
        sstext << "Item '" << item << "' Not Found in Database." << '\0';
    }

    SystemMessage(m_session,  sstext.str().c_str());

    return true;
}

bool ChatHandler::HandleItemRemoveCommand(const char* args, WorldSession *m_session)
{
    char* iguid = strtok((char*)args, " ");
    if (!iguid)
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

    uint32 itemguid = atoi(iguid);
    int slot = pCreature->GetSlotByItemId(itemguid);

    std::stringstream sstext;
    if(slot != -1)
    {
        uint32 guidlow = GUID_LOPART(guid);

        std::stringstream ss;
        ss << "DELETE FROM vendors WHERE vendorGuid = " << guidlow << " AND itemGuid = " << itemguid << '\0';
        sDatabase.Execute( ss.str().c_str() );

        pCreature->RemoveVendorItem(itemguid);
        ItemPrototype* tmpItem = objmgr.GetItemPrototype(itemguid);
        if(tmpItem)
        {
            sstext << "Item '" << itemguid << "' '" << tmpItem->Name1 << "' Deleted from list" << '\0';
        }
        else
        {
            sstext << "Item '" << itemguid << "' Deleted from list" << '\0';
        }

    }
    else
    {
        sstext << "Item '" << itemguid << "' Not Found in List." << '\0';
    }

    SystemMessage(m_session, sstext.str().c_str());

    return true;
}

bool ChatHandler::HandleRunCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        return false;

    int option = atoi((char*)args);

    if(option != 0 && option != 1)
    {
        SystemMessage(m_session,  "Incorrect value, use 0 or 1");
        return true;
    }

    uint64 guid = m_session->GetPlayer()->GetSelection();
    if (guid == 0)
    {
        SystemMessage(m_session, "No selection.");
        return true;
    }

    Creature * pCreature = sObjHolder.GetObject<Creature>(guid);
    if(!pCreature)
    {
        SystemMessage(m_session, "You should select a creature.");
        return true;
    }

    char sql[512];

    sprintf(sql, "UPDATE creatures SET running = '%i' WHERE id = '%u'", option, GUID_LOPART(guid));
    sDatabase.Execute( sql );

    pCreature->GetAIInterface()->setMoveRunFlag(option > 0);

    SystemMessage(m_session, "Value saved.");

    return true;
}



bool ChatHandler::HandleNPCFlagCommand(const char* args, WorldSession *m_session)
{
    if (!*args)
        return false;

    uint32 npcFlags = (uint32) atoi((char*)args);

    uint64 guid = m_session->GetPlayer()->GetSelection();
    if (guid == 0)
    {
        SystemMessage(m_session, "No selection.");
        return true;
    }

    Creature * pCreature = sObjHolder.GetObject<Creature>(guid);
    if(!pCreature)
    {
        SystemMessage(m_session, "You should select a creature.");
        return true;
    }

    pCreature->SetUInt32Value(UNIT_NPC_FLAGS , npcFlags);
    pCreature->SaveToDB();
    SystemMessage(m_session, "Value saved, you may need to rejoin or clean your client cache.");

    return true;
}

bool ChatHandler::HandleSaveAllCommand(const char *args, WorldSession *m_session)
{
    HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;
    uint32 stime = now();
    uint32 count = 0;
    for (itr = sObjHolder.Begin<Player>(); itr != sObjHolder.End<Player>(); itr++)
    {
        if(itr->second->GetSession())
        {
            itr->second->SaveToDB();
            count++;
        }
    }
    char msg[100];
    sprintf(msg, "Saved all %d online players in %d msec.", count, (uint32)now() - stime);
    sWorld.SendWorldText(msg);
    sWorld.SendWorldWideScreenText(msg);
    //sWorld.SendIRCMessage(msg);
    return true;
}

bool ChatHandler::HandleKillCommand(const char *args, WorldSession *m_session)
{
    Unit * target = World::GetUnit(m_session->GetPlayer()->GetSelection());
    if(target == 0)
    {
        RedSystemMessage(m_session, "A valid selection is required.");
        return true;
    }

    sGMLog.writefromsession(m_session, "used kill command on "I64FMT, target->GetGUID());

    // If we're killing a player, send a message indicating a gm killed them.
    if(target->IsPlayer())
    {
        Player * plr = static_cast<Player*>(target);
        plr->SetUInt32Value(UNIT_FIELD_HEALTH, 0);
        plr->KillPlayer();
        BlueSystemMessageToPlr(plr, "%s killed you with a GM command.", m_session->GetPlayer()->GetName());
    }
    else
    {
/*
        // Cast insta-kill.
        SpellEntry * se = sSpellStore.LookupEntry(5);
        if(se == 0) return false;

        SpellCastTargets targets(target->GetGUID());
        Spell * sp = new Spell(m_session->GetPlayer(), se, true, 0);
        sp->prepare(&targets);
*/
        SpellEntry * se = sSpellStore.LookupEntry(20479);
        if(se == 0) return false;
        
        SpellCastTargets targets(target->GetGUID());
        Spell * sp = new Spell(target, se, true, 0);
        sp->prepare(&targets);
    }

    return true;
}

bool ChatHandler::HandleCastSpellCommand(const char* args, WorldSession *m_session)
{
    Unit *caster = m_session->GetPlayer();
    Unit *target = getSelectedChar(m_session, false);
    if(!target)
        target = getSelectedCreature(m_session, false);
    if(!target)
    {
        RedSystemMessage(m_session, "Must select a char or creature.");
        return false;
    }

    uint32 spellid = atol(args);
    SpellEntry *spellentry = sSpellStore.LookupEntry(spellid);
    if(!spellentry)
    {
        RedSystemMessage(m_session, "Invalid spell id!");
        return false;
    }
    
    Spell *sp = new Spell(caster, spellentry, false, NULL);
    if(!sp)
    {
        RedSystemMessage(m_session, "Spell failed creation!");
        delete sp;
        return false;
    }

    BlueSystemMessage(m_session, "Casting spell %d on target.", spellid);
    SpellCastTargets targets;
    targets.m_unitTarget = target->GetGUID();
    sp->prepare(&targets);
    return true;
}



bool ChatHandler::HandleCastSpellNECommand(const char* args, WorldSession *m_session)
{
    Unit *caster = m_session->GetPlayer();
    Unit *target = getSelectedChar(m_session, false);
    if(!target)
        target = getSelectedCreature(m_session, false);
    if(!target)
    {
        RedSystemMessage(m_session, "Must select a char or creature.");
        return false;
    }

    uint32 spellId = atol(args);
    SpellEntry *spellentry = sSpellStore.LookupEntry(spellId);
    if(!spellentry)
    {
        RedSystemMessage(m_session, "Invalid spell id!");
        return false;
    }
    BlueSystemMessage(m_session, "Casting spell %d on target.", spellId);

    WorldPacket data;

    data.Initialize( SMSG_SPELL_START );
    data << caster->GetNewGUID();
    data << caster->GetNewGUID();
    data << spellId;
    data << uint16(0);
    data << uint32(0);
    data << uint16(2);
    data << target->GetGUID();
    //        WPAssert(data.size() == 36);
    m_session->SendPacket( &data );

    data.Initialize( SMSG_SPELL_GO );
    data << caster->GetNewGUID();
    data << caster->GetNewGUID();
    data << spellId;
    data << uint8(0) << uint8(1) << uint8(1);
    data << target->GetGUID();
    data << uint8(0);
    data << uint16(2);
    data << target->GetGUID();
    //        WPAssert(data.size() == 42);
    m_session->SendPacket( &data );
    return true;
}

bool ChatHandler::HandleSpawnCommand(const char* args, WorldSession *m_session)
{

    char* pEntry = strtok((char*)args, " ");
    if (!pEntry)
        return false;

    /* Ignatich: not safe
    if (pEntry[1]=='x') {
    FillSystemMessageData(&data, "Please use decimal.");
    m_session->SendPacket( &data );
    return;
    }
    */
    char* pFlags = strtok(NULL, " ");
    if (!pFlags)
        return false;

    char* pFaction = strtok(NULL, " ");
    if (!pFaction)
        return false;

    char* pLevel = strtok(NULL, " ");
    if (!pLevel)
        return false;

    char* pName = strtok(NULL, "%");
    if (!pName)
        return false;

    uint32 npcFlags  = atoi(pFlags);
    uint32 faction_id  = atoi(pFaction);
    uint32 level  = atoi(pLevel);
    uint32 display_id = atoi(pEntry);

    if (display_id==0)
        return false;

    for (int i = 0; i < strlen(pName); i++) {
        if(!isalpha(pName[i]) && pName[i]!=' ') {
            SystemMessage(m_session,  "Error, name can only contain chars A-Z and a-z.");
            return false;
        }
    }
    SpawnCreature(m_session, pName, display_id, npcFlags, faction_id, level);

    return true;
}

bool ChatHandler::HandleMonsterSayCommand(const char* args, WorldSession *m_session)
{
    Unit *crt = getSelectedCreature(m_session, false);
    if(!crt)
        crt = getSelectedChar(m_session, false);

    if(!crt)
    {
        RedSystemMessage(m_session, "Please select a creature or player before using this command.");
        return true;
    }
    if(crt->GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket * data = this->FillMessageData(CHAT_MSG_SAY, LANG_UNIVERSAL, args, crt->GetGUID(), 0);
        crt->SendMessageToSet(data, true);
        delete data;
    }
    else
    {
        crt->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, args);
    }

    return true;
}

bool ChatHandler::HandleMonsterYellCommand(const char* args, WorldSession *m_session)
{
    Unit *crt = getSelectedCreature(m_session, false);
    if(!crt)
        crt = getSelectedChar(m_session, false);

    if(!crt)
    {
        RedSystemMessage(m_session, "Please select a creature or player before using this command.");
        return true;
    }
    if(crt->GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket * data = this->FillMessageData(CHAT_MSG_YELL, LANG_UNIVERSAL, args, crt->GetGUID(), 0);
        crt->SendMessageToSet(data, true);
        delete data;
    }
    else
    {
        crt->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, args);
    }

    return true;
}


bool ChatHandler::HandleGOSelect(const char *args, WorldSession *m_session)
{
    GameObject *GObj = NULL;

    std::set<Object*>::iterator Itr = m_session->GetPlayer()->GetInRangeSetBegin();
    std::set<Object*>::iterator Itr2 = m_session->GetPlayer()->GetInRangeSetEnd();
    float cDist = 9999.0f;
    float nDist = 0.0f;
    bool bUseNext = false;

    if(args)
    {
        if(args[0] == '1')
        {
            if(m_session->GetPlayer()->m_GM_SelectedGO == NULL)
                bUseNext = true;

            for(;;Itr++)
            {
                if(Itr == Itr2 && GObj == NULL && bUseNext)
                    Itr = m_session->GetPlayer()->GetInRangeSetBegin();
                else if(Itr == Itr2)
                    break;

                if((*Itr)->GetTypeId() == TYPEID_GAMEOBJECT)
                {
                    // Find the current go, move to the next one
                    if(bUseNext)
                    {
                        // Select the first.
                        GObj = static_cast<GameObject*>(*Itr);
                        break;
                    } else {
                        if(((*Itr) == m_session->GetPlayer()->m_GM_SelectedGO))
                        {
                            // Found him. Move to the next one, or beginning if we're at the end
                            bUseNext = true;
                        }
                    }
                }
            }
        }
    }
    if(!GObj)
    {
        for( ; Itr != Itr2; Itr++ )
        {
            if( (*Itr)->GetTypeId() == TYPEID_GAMEOBJECT )
            {
                if( (nDist = m_session->GetPlayer()->CalcDistance( *Itr )) < cDist )
                {
                    cDist = nDist;
                    nDist = 0.0f;
                    GObj = (GameObject*)(*Itr);
                }
            }
        }
    }


    if( GObj == NULL )
    {
        RedSystemMessage(m_session, "No inrange GameObject found.");
        return true;
    }

    m_session->GetPlayer()->m_GM_SelectedGO = GObj;

    GreenSystemMessage(m_session, "Selected GameObject [ %s ] which is %.3f meters away from you.",
        objmgr.GetGameObjectName(GObj->GetEntry())->Name.c_str(), m_session->GetPlayer()->CalcDistance(GObj));

    return true;
}

bool ChatHandler::HandleGODelete(const char *args, WorldSession *m_session)
{
    std::stringstream sstext;

    GameObject *GObj = m_session->GetPlayer()->m_GM_SelectedGO;
    if( !GObj )
    {
        RedSystemMessage(m_session, "No selected GameObject...");
        return true;
    }

    GObj->GetMapMgr()->GetBaseMap()->GetTemplate()->RemoveIndex<GameObject>(GObj);    // remove index
    GObj->Despawn(3600000);
    GObj->DeleteFromDB();
    sObjHolder.Delete<GameObject>(GObj);

    m_session->GetPlayer()->m_GM_SelectedGO = NULL;

    GreenSystemMessage(m_session, "GameObject successfully deleted from world and database !");

    return true;
}

bool ChatHandler::HandleGOSpawn(const char *args, WorldSession *m_session)
{
    std::stringstream sstext;

    char* pEntryID = strtok((char*)args, " ");
    if (!pEntryID)
        return false;

    uint32 EntryID  = atoi(pEntryID);
    
    bool Save = false;
    char* pSave = strtok(NULL, " ");
    if (pSave)
        Save = atoi(pSave);

    GameObjectInfo* goi = objmgr.GetGameObjectName(EntryID);
    if(!goi)
    {
        sstext << "GameObject Info '" << EntryID << "' Not Found" << '\0';
        SystemMessage(m_session, sstext.str().c_str());
        return true;
    }

    sLog.outDebug("Spawning GameObject By Entry '%u'", EntryID);
    sstext << "Spawning GameObject By Entry '" << EntryID << "'" << '\0';
    SystemMessage(m_session, sstext.str().c_str());

    GameObject *go = sObjHolder.Create<GameObject>();
    
    Player *chr = m_session->GetPlayer();
    uint32 mapid = chr->GetMapId();
    float x = chr->GetPositionX();
    float y = chr->GetPositionY();
    float z = chr->GetPositionZ();
    float o = chr->GetOrientation();

    go->SetInstanceID(chr->GetInstanceID());
    go->CreateFromProto(EntryID,mapid,x,y,z,o);
    go->AddToWorld();

    GameObjectTemplate *t = new GameObjectTemplate;
    t->Create(go);
    go->GetMapMgr()->GetBaseMap()->GetTemplate()->AddIndex<GameObject>(go, t);

    if(Save == true)
    {
        // If we're saving, create template and add index
        go->SaveToDB();
    }
    return true;
}

bool ChatHandler::HandleGOInfo(const char *args, WorldSession *m_session)
{
    std::stringstream sstext;
    GameObjectInfo *GOInfo = NULL;
    GameObject *GObj = NULL;

    GObj = m_session->GetPlayer()->m_GM_SelectedGO;
    if( !GObj )
    {
        RedSystemMessage(m_session, "No selected GameObject...");
        return true;
    }
    
    sstext
        << MSG_COLOR_SUBWHITE << "Informations:\n"
        << MSG_COLOR_GREEN << "Entry: " << MSG_COLOR_LIGHTBLUE << GObj->GetEntry()                          << "\n"
        << MSG_COLOR_GREEN << "Model: " << MSG_COLOR_LIGHTBLUE << GObj->GetUInt32Value(GAMEOBJECT_DISPLAYID)<< "\n"
        << MSG_COLOR_GREEN << "State: " << MSG_COLOR_LIGHTBLUE << GObj->GetUInt32Value(GAMEOBJECT_STATE)<< "\n"
        << MSG_COLOR_GREEN << "flags: " << MSG_COLOR_LIGHTBLUE << GObj->GetUInt32Value(GAMEOBJECT_FLAGS)<< "\n"
        << MSG_COLOR_GREEN << "dynflags:" << MSG_COLOR_LIGHTBLUE << GObj->GetUInt32Value(GAMEOBJECT_DYN_FLAGS) << "\n"
        << MSG_COLOR_GREEN << "faction: " << MSG_COLOR_LIGHTBLUE << GObj->GetUInt32Value(GAMEOBJECT_FACTION)<< "\n"
        << MSG_COLOR_GREEN << "Type: "  << MSG_COLOR_LIGHTBLUE << GObj->GetUInt32Value(GAMEOBJECT_TYPE_ID)  << " -- ";

    switch( GObj->GetUInt32Value(GAMEOBJECT_TYPE_ID) )
    {
    case GAMEOBJECT_TYPE_DOOR:          sstext << "Door";    break;
    case GAMEOBJECT_TYPE_BUTTON:        sstext << "Button";    break;
    case GAMEOBJECT_TYPE_QUESTGIVER:    sstext << "Quest Giver";    break;
    case GAMEOBJECT_TYPE_CHEST:         sstext << "Chest";    break;
    case GAMEOBJECT_TYPE_BINDER:        sstext << "Binder";    break;
    case GAMEOBJECT_TYPE_GENERIC:       sstext << "Generic";    break;
    case GAMEOBJECT_TYPE_TRAP:          sstext << "Trap";    break;
    case GAMEOBJECT_TYPE_CHAIR:         sstext << "Chair";    break;
    case GAMEOBJECT_TYPE_SPELL_FOCUS:   sstext << "Spell Focus";    break;
    case GAMEOBJECT_TYPE_TEXT:          sstext << "Text";    break;
    case GAMEOBJECT_TYPE_GOOBER:        sstext << "Goober";    break;
    case GAMEOBJECT_TYPE_TRANSPORT:     sstext << "Transport";    break;
    case GAMEOBJECT_TYPE_AREADAMAGE:    sstext << "Area Damage";    break;
    case GAMEOBJECT_TYPE_CAMERA:        sstext << "Camera";    break;
    case GAMEOBJECT_TYPE_MAP_OBJECT:    sstext << "Map Object";    break;
    case GAMEOBJECT_TYPE_MO_TRANSPORT:  sstext << "Mo Transport";    break;
    case GAMEOBJECT_TYPE_DUEL_ARBITER:  sstext << "Duel Arbiter";    break;
    case GAMEOBJECT_TYPE_FISHINGNODE:   sstext << "Fishing Node";    break;
    case GAMEOBJECT_TYPE_RITUAL:        sstext << "Ritual";    break;
    case GAMEOBJECT_TYPE_MAILBOX:       sstext << "Mailbox";    break;
    case GAMEOBJECT_TYPE_AUCTIONHOUSE:  sstext << "Auction House";    break;
    case GAMEOBJECT_TYPE_GUARDPOST:     sstext << "Guard Post";    break;
    case GAMEOBJECT_TYPE_SPELLCASTER:   sstext << "Spell Caster";    break;
    case GAMEOBJECT_TYPE_MEETINGSTONE:  sstext << "Meeting Stone";    break;
    case GAMEOBJECT_TYPE_FLAGSTAND:     sstext << "Flag Stand";    break;
    case GAMEOBJECT_TYPE_FISHINGHOLE:   sstext << "Fishing Hole";    break;
    case GAMEOBJECT_TYPE_FLAGDROP:      sstext << "Flag Drop";    break;
    default:                            sstext << "Unknown.";    break;
    }

    sstext
        << "\n"
        << MSG_COLOR_GREEN << "Distance: " << MSG_COLOR_LIGHTBLUE << GObj->CalcDistance((Object*)m_session->GetPlayer());

    GOInfo = objmgr.GetGameObjectName(GObj->GetEntry());
    if( !GOInfo )
    {
        RedSystemMessage(m_session, "This GameObject doesn't have template, you won't be able to get some informations nor to spawn a GO with this entry.");
        sstext << "|r";
        SystemMessage(m_session, sstext.str().c_str());
        return true;
    }

    sstext
        << "\n"
        << MSG_COLOR_GREEN << "Name: "  << MSG_COLOR_LIGHTBLUE << GOInfo->Name      << "\n"
        << MSG_COLOR_GREEN << "Size: "  << MSG_COLOR_LIGHTBLUE << GOInfo->Size      << "\n"
        << "|r";

    SystemMessage(m_session, sstext.str().c_str());

    return true;
}

bool ChatHandler::HandleGOEnable(const char *args, WorldSession *m_session)
{
    GameObject *GObj = NULL;

    GObj = m_session->GetPlayer()->m_GM_SelectedGO;
    if( !GObj )
    {
        RedSystemMessage(m_session, "No selected GameObject...");
        return true;
    }
    if(GObj->GetUInt32Value(GAMEOBJECT_DYN_FLAGS) == 1)
    {
        // Deactivate
        GObj->SetUInt32Value(GAMEOBJECT_DYN_FLAGS, 0);
    } else {
        // /Activate
        GObj->SetUInt32Value(GAMEOBJECT_DYN_FLAGS, 1);
    }
    BlueSystemMessage(m_session, "Gameobject activate/deactivated.");
    return true;
}

bool ChatHandler::HandleGOActivate(const char* args, WorldSession *m_session)
{
    GameObject *GObj = NULL;

    GObj = m_session->GetPlayer()->m_GM_SelectedGO;
    if( !GObj )
    {
        RedSystemMessage(m_session, "No selected GameObject...");
        return true;
    }
    if(GObj->GetUInt32Value(GAMEOBJECT_STATE) == 1)
    {
        // Close/Deactivate
        GObj->SetUInt32Value(GAMEOBJECT_STATE, 0);
        GObj->SetUInt32Value(GAMEOBJECT_FLAGS, (GObj->GetUInt32Value(GAMEOBJECT_FLAGS)-1));
    } else {
        // Open/Activate
        GObj->SetUInt32Value(GAMEOBJECT_STATE, 1);
        GObj->SetUInt32Value(GAMEOBJECT_FLAGS, (GObj->GetUInt32Value(GAMEOBJECT_FLAGS)+1));
    }
    BlueSystemMessage(m_session, "Gameobject opened/closed.");
    return true;
}

bool ChatHandler::HandleGOScale(const char* args, WorldSession* m_session)
{
    GameObject *go = m_session->GetPlayer()->m_GM_SelectedGO;
    if( !go )
    {
        RedSystemMessage(m_session, "No selected GameObject...");
        return true;
    }
    if(!args)
    {
        RedSystemMessage(m_session, "Invalid syntax. Should be .gobject scale 1.0");
        return false;
    }
    float scale = atof(args);
    if(!scale) scale = 1;
    go->SetFloatValue(OBJECT_FIELD_SCALE_X, scale);
    BlueSystemMessage(m_session, "Set scale to %.3f", scale);
    return true;
}

bool ChatHandler::HandleUptimeCommand(const char* args, WorldSession* m_session)
{
    BlueSystemMessage(m_session, "Server Uptime is %s", sWorld.GetUptimeString().c_str());
    return true;
}

bool ChatHandler::HandleReviveStringcommand(const char* args, WorldSession* m_session)
{
    Player *plr = objmgr.GetPlayer(args, false);
    if(!plr)
    {
        RedSystemMessage(m_session, "Could not find player %s.", args);
        return true;
    }

    if(plr->isDead())
    {
        plr->ResurrectPlayer();
        plr->SetMovement(MOVE_UNROOT, 5);
        plr->SetPlayerSpeed(RUN, (float)7.5, 6, true);
        plr->SetPlayerSpeed(SWIM, (float)4.9, 7, true);
        plr->SetMovement(MOVE_LAND_WALK, 8);
        plr->SetUInt32Value(UNIT_FIELD_HEALTH, plr->GetUInt32Value(UNIT_FIELD_MAXHEALTH) );
        GreenSystemMessage(m_session, "Revived player %s.", args);
    } else {
        GreenSystemMessage(m_session, "Player %s is not dead.", args);
    }
    return true;
}

bool ChatHandler::HandleMountCommand(const char *args, WorldSession *m_session)
{
    if(!args)
    {
        RedSystemMessage(m_session, "No model specified");
        return true;
    }
    uint32 modelid = atol(args);
    if(!modelid)
    {
        RedSystemMessage(m_session, "No model specified");
        return true;
    }

    Unit *m_target = NULL;
    Player *m_plyr = getSelectedChar(m_session, false);
    if(m_plyr)
        m_target = m_plyr;
    else
    {
        Creature *m_crt = getSelectedCreature(m_session, false);
        if(m_crt)
            m_target = m_crt;
    }

    if(!m_target)
    {
        RedSystemMessage(m_session, "No target found.");
        return true;
    }
    
    if(m_target->GetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID) != 0)
    {
        RedSystemMessage(m_session, "Target is already mounted.");
        return true;
    }

    m_target->SetUInt32Value( UNIT_FIELD_MOUNTDISPLAYID , modelid);
    if(!m_target->HasFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_MOUNT_SIT)) m_target->SetFlag( UNIT_FIELD_FLAGS , U_FIELD_FLAG_MOUNT_SIT );
    
    BlueSystemMessage(m_session, "Now mounted with model %d.", modelid);
    return true;
}

bool ChatHandler::HandleAddAIAgentCommand(const char* args, WorldSession *m_session)
{
    char* agent = strtok((char*)args, " ");
    if(!agent)
        return false;
    char* procEvent = strtok(NULL, " ");
    if(!procEvent)
        return false;
    char* procChance = strtok(NULL, " ");
    if(!procChance)
        return false;
    char* procCount = strtok(NULL, " ");
    if(!procCount)
        return false;
    char* spellId = strtok(NULL, " ");
    if(!spellId)
        return false;
    char* spellType = strtok(NULL, " ");
    if(!spellType)
        return false;
    char* spelltargetType = strtok(NULL, " ");
    if(!spelltargetType)
        return false;
    char* spellCooldown = strtok(NULL, " ");
    if(!spellCooldown)
        return false;
    char* floatMisc1 = strtok(NULL, " ");
    if(!floatMisc1)
        return false;
    char* Misc2 = strtok(NULL, " ");
    if(!Misc2)
        return false;

    Unit* target = sObjHolder.GetObject<Creature>(m_session->GetPlayer()->GetSelection());
    if(!target)
    {
        RedSystemMessage(m_session, "You have to select a Creature!");
        return false;
    }

    std::stringstream qry;
    qry << "INSERT INTO ai_agents set entryId = '" << target->GetUInt32Value(OBJECT_FIELD_ENTRY) << "', AI_AGENT = '" << atoi(agent) << "', procEvent = '" << atoi(procEvent)<< "', procChance = '" << atoi(procChance)<< "', procCount = '" << atoi(procCount)<< "', spellId = '" << atoi(spellId)<< "', spellType = '" << atoi(spellType)<< "', spelltargetType = '" << atoi(spelltargetType)<< "', spellCooldown = '" << atoi(spellCooldown)<< "', floatMisc1 = '" << atof(floatMisc1)<< "', Misc2  ='" << atoi(Misc2)<< "'";
    sDatabase.Execute( qry.str().c_str( ) );

    AI_Spell * sp = new AI_Spell;
    sp->agent = atoi(agent);
    sp->procEvent = atoi(procEvent);
    sp->procChance = atoi(procChance);
    sp->procCount = atoi(procCount);
    sp->spellId = atoi(spellId);
    sp->spellType = atoi(spellType);
    sp->spelltargetType = atoi(spelltargetType);
    sp->spellCooldown = atoi(spellCooldown);
    sp->floatMisc1 = atof(floatMisc1);
    sp->Misc2 = atof(Misc2);
    sp->minrange = GetMinRange(sSpellRange.LookupEntry(sSpellStore.LookupEntry(sp->spellId)->rangeIndex));
    sp->maxrange = GetMaxRange(sSpellRange.LookupEntry(sSpellStore.LookupEntry(sp->spellId)->rangeIndex));
    if(sp->agent == AGENT_CALLFORHELP)
        target->GetAIInterface()->m_canCallForHelp = true;
    else if(sp->agent == AGENT_FLEE)
        target->GetAIInterface()->m_canFlee = true;
    else if(sp->agent == AGENT_RANGED)
        target->GetAIInterface()->m_canRangedAttack = true;

    return true;
}

bool ChatHandler::HandleDelAIAgentCommand(const char* args, WorldSession *m_session)
{
    char* agent = strtok((char*)args, " ");
    if(!agent)
        return false;
    char* spellId = strtok(NULL, " ");
    if(!spellId)
        return false;

    Unit* target = sObjHolder.GetObject<Creature>(m_session->GetPlayer()->GetSelection());
    if(!target)
    {
        RedSystemMessage(m_session, "You have to select a Creature!");
        return false;
    }

    std::stringstream qry;
    qry << "DELETE FROM ai_agents where (entryId = '" << target->GetUInt32Value(OBJECT_FIELD_ENTRY) << "') AND (AI_AGENT = '" << atoi(agent) << "') AND (spellId = '" << atoi(spellId) << "')";

    return true;
}

bool ChatHandler::HandleListAIAgentCommand(const char* args, WorldSession *m_session)
{
    Unit* target = sObjHolder.GetObject<Creature>(m_session->GetPlayer()->GetSelection());
    if(!target)
    {
        RedSystemMessage(m_session, "You have to select a Creature!");
        return false;
    }

    std::stringstream sstext;
    sstext << "agentlist of creature: " << target->GetGUIDLow() << " " << target->GetGUIDHigh() << '\n';

    std::stringstream ss;
    ss << "SELECT * FROM ai_agents where entryId=" << target->GetUInt32Value(OBJECT_FIELD_ENTRY);
    QueryResult *result = sDatabase.Query( ss.str().c_str() );

    if( !result )
        return false;

    do
    {
        Field *fields = result->Fetch();
        sstext << "agent: "   << fields[1].GetUInt16()
            << " | spellId: " << fields[5].GetUInt32()
            << " | Event: "   << fields[2].GetUInt32()
            << " | chance: "  << fields[3].GetUInt32()
            << " | count: "   << fields[4].GetUInt32() << '\n';
    } while( result->NextRow() );

    delete result;

    SendMultilineMessage(m_session, sstext.str().c_str());

    return true;
}

bool ChatHandler::HandleGOAnimProgress(const char * args, WorldSession * m_session)
{
    if(!m_session->GetPlayer()->m_GM_SelectedGO)
        return false;

    uint32 ap = atol(args);
    m_session->GetPlayer()->m_GM_SelectedGO->SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, ap);
    BlueSystemMessage(m_session, "Set ANIMPROGRESS to %u", ap);
    return true;
}

bool ChatHandler::HandleGOExport(const char * args, WorldSession * m_session)
{
    if(!m_session->GetPlayer()->m_GM_SelectedGO)
        return false;
    
    std::stringstream name;
    if (*args)
    {
        name << "GO_" << args << ".sql";
    }
    else
    {
        name << "GO_" << m_session->GetPlayer()->m_GM_SelectedGO->GetEntry() << ".sql";
    }
   
    m_session->GetPlayer()->m_GM_SelectedGO->SaveToFile(name);

    BlueSystemMessage(m_session, "Go saved to: %s", name.str().c_str());
    return true;
}

bool ChatHandler::HandleNpcExport(const char * args, WorldSession * m_session)
{
    Unit* target = sObjHolder.GetObject<Creature>(m_session->GetPlayer()->GetSelection());
    if (!target)
        return false;
    
    std::stringstream name;
    if (*args)
    {
        name << "NPC_" << args << ".sql";
    }
    else
    {
        name << "NPC_" << target->GetEntry() << ".sql";
    }
    ((Creature*)target)->SaveToFile(name);
    BlueSystemMessage(m_session, "Creature/npc saved to: %s", name.str().c_str());
    return true;
}

bool ChatHandler::HandleNpcComeCommand(const char* args, WorldSession* m_session)
{
    // moves npc to players location
    Player * plr = m_session->GetPlayer();
    Creature * crt = getSelectedCreature(m_session, true);
    if(!crt) return true;

    crt->GetAIInterface()->MoveTo(plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), plr->GetOrientation());
    return true;
}
