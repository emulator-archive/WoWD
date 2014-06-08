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
//  GM Chat Commands
//

#include "StdAfx.h"

bool ChatHandler::HandleAnnounceCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        return false;

    char pAnnounce[256*50];
    string input2;

    input2 = "|cffff6060<";
    if (m_session->GetPermissionCount())   input2+="GM";
    input2+=">|r|c1f40af20";
    input2+=m_session->GetPlayer()->GetName();
    input2+="|r|cffffffff broadcasts: |r";
    sprintf((char*)pAnnounce, "%s%s", input2.c_str(), args);   // Adds BROADCAST:
    sWorld.SendWorldText(pAnnounce); // send message
    sGMLog.writefromsession(m_session, "used announce command, [%s]", args);

    //sWorld.SendForcedIRCMessage(pAnnounce);
    return true;
}


bool ChatHandler::HandleWAnnounceCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        return false;

    char pAnnounce[256];
    string input2;

    input2 = "|cffff6060<";
    if (m_session->GetPermissionCount())   input2+="GM";
    input2+=">|r|c1f40af20";
    input2+=m_session->GetPlayer()->GetName();
    input2+=":|r ";
    sprintf((char*)pAnnounce, "%s%s", input2.c_str(), args);

    sWorld.SendWorldWideScreenText(pAnnounce); // send message
    sGMLog.writefromsession(m_session, "used wannounce command [%s]", args);
    //sWorld.SendForcedIRCMessage(pAnnounce);
    return true;
}


bool ChatHandler::HandleGMOnCommand(const char* args, WorldSession *m_session)
{
    /*uint32 newbytes = m_session->GetPlayer( )->GetUInt32Value(PLAYER_BYTES_2) | 0x8;
    m_session->GetPlayer( )->SetUInt32Value( PLAYER_BYTES_2, newbytes);

    GreenSystemMessage(m_session, "GM Flag Set.");*/
    GreenSystemMessage(m_session, "Setting GM Flag on yourself...");
    if(m_session->GetPlayer()->bGMTagOn)
        RedSystemMessage(m_session, "GM Flag is already set on. Use !gmoff to disable it.");
    else
    {
        m_session->GetPlayer()->bGMTagOn = true;
        m_session->GetPlayer()->SetFlag(PLAYER_FLAGS, PLAYER_FLAG_GM);    // <GM>
        BlueSystemMessage(m_session, "GM Flag Set. It will appear above your name and in chat messages until you use !gmoff.");
    }

    return true;
}


bool ChatHandler::HandleGMOffCommand(const char* args, WorldSession *m_session)
{
    //uint32 newbytes = m_session->GetPlayer( )->GetUInt32Value(PLAYER_BYTES_2) & ~(0x8);
    //m_session->GetPlayer( )->SetUInt32Value( PLAYER_BYTES_2, newbytes);

    //GreenSystemMessage(m_session, "GM Flag Unset.");
    GreenSystemMessage(m_session, "Unsetting GM Flag on yourself...");
    if(!m_session->GetPlayer()->bGMTagOn)
        RedSystemMessage(m_session, "GM Flag not set. Use !gmon to enable it.");
    else
    {
        m_session->GetPlayer()->bGMTagOn = false;
        m_session->GetPlayer()->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAG_GM);    // <GM>
        BlueSystemMessage(m_session, "GM Flag Removed. <GM> Will no longer show in chat messages or above your name.");
    }

    return true;
}


bool ChatHandler::HandleGPSCommand(const char* args, WorldSession *m_session)
{
    Object *obj;

    uint64 guid = m_session->GetPlayer()->GetSelection();
    if (guid != 0)
    {
        if(!(obj = (Object*)sObjHolder.GetObject<Player>(guid)) && !(obj = (Object*)sObjHolder.GetObject<Creature>(guid)))
        {
            SystemMessage(m_session, "You should select a character or a creature.");
            return true;
        }
    }
    else
        obj = (Object*)m_session->GetPlayer();

    char buf[256];
    sprintf((char*)buf, "|cff00ff00Current Position: |cffffffffMap: |cff00ff00%d |cffffffffX: |cff00ff00%f |cffffffffY: |cff00ff00%f |cffffffffZ: |cff00ff00%f |cffffffffOrientation: |cff00ff00%f|r",
        obj->GetMapId(), obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), obj->GetOrientation());
    
    
    SystemMessage(m_session, buf);

    return true;
}


bool ChatHandler::HandleKickCommand(const char* args, WorldSession *m_session)
{

    if(!*args)
    return false;
    char *pname = strtok((char*)args, " ");
    if(!pname)
    {
        RedSystemMessage(m_session, "No name specified.");
        return true;
    }
    Player *chr = objmgr.GetPlayer((const char*)pname, false);
    if (chr)
    {
        char *reason = strtok(NULL, "\n");
        std::string kickreason = "No reason";
        if(reason)
            kickreason = reason;

        BlueSystemMessage(m_session, "Attempting to kick %s from the server for \"%s\".", chr->GetName(), kickreason.c_str());

        /*if(m_session->GetSecurity() < chr->GetSession()->GetSecurity())
        {
            SystemMessage(m_session, "You cannot kick %s, as he is a higher GM level than you.", chr->GetName());
            return true;
        }*/ // we might have to re-work this

        char msg[200];
        sprintf(msg, "%sGM: %s was kicked from the server by %s. Reason: %s", MSG_COLOR_RED, chr->GetName(), m_session->GetPlayer()->GetName(), kickreason.c_str());
        sWorld.SendWorldText(msg, NULL);
        //sWorld.SendIRCMessage(msg);
        SystemMessageToPlr(chr, "You are being kicked from the server by %s. Reason: %s", m_session->GetPlayer()->GetName(), kickreason.c_str());

        chr->Kick(6000);
        
        return true;
    } else {
        RedSystemMessage(m_session, "Player is not online at the moment.");
        return true;
    }
    return true;
}


bool ChatHandler::HandleSetRankCommand(const char *args, WorldSession *m_session)
{
    if(strlen(args) < 1)
    {
        char buf[256];
        sprintf((char*)buf,".setrank <rank> - Sets player PVP rank.");
        SystemMessage(m_session, buf);
        return true;
    }

    int8 rank = atoi(args);
    Player *chr = getSelectedChar(m_session);
    if (chr == NULL) return true;
    
    chr->SetPVPRank(rank);
    return true;
}

bool ChatHandler::HandleGetRankCommand(const char *args, WorldSession *m_session)
{
    Player *chr = getSelectedChar(m_session);
    if (chr == NULL) return true;
   
    GreenSystemMessage(m_session, "%s's PVP Rank is |r%d", chr->GetName(), chr->GetPVPRank());
    return true;
}

bool ChatHandler::HandleAddInvItemCommand(const char *args, WorldSession *m_session)
{
    if(strlen(args) < 1)
    {
        return false;
    }

    int itemid = atoi(strtok((char*)args, " "));
    int count = 1;
    char *cCount = strtok(NULL, "\n");  
    if(cCount) count = atoi(cCount);

    Player *chr = getSelectedChar(m_session);
    if (chr == NULL) return true;
    
    ItemPrototype* it = objmgr.GetItemPrototype(itemid);
    if(it)
    {
        sGMLog.writefromsession(m_session, "used add item command, item id %u [%s] to %s", it->ItemId, it->Name1.c_str(), chr->GetName());
        Item *item;
        item = objmgr.CreateItem( itemid, chr);
        item->SetUInt32Value(ITEM_FIELD_STACK_COUNT, ((count > it->MaxCount) ? it->MaxCount : count));
      
        if(!chr->GetItemInterface()->AddItemToFreeSlot(item))
        {
            m_session->SendNotification("No free slots were found in your inventory!");
            delete item;
            return true;
        }

        char messagetext[128];
        sprintf(messagetext, "Adding item %d (%s) to %s's inventory.",it->ItemId,it->Name1.c_str(), chr->GetName());
        SystemMessage(m_session, messagetext);
        sprintf(messagetext, "%s added item %d (%s) to your inventory.", m_session->GetPlayer()->GetName(), itemid, it->Name1.c_str());
        SystemMessageToPlr(chr,  messagetext);

        return true;
    } else {
        RedSystemMessage(m_session, "Item %d is not a valid item!",itemid);
        return true;
    }
}

bool ChatHandler::HandleSummonCommand(const char* args, WorldSession *m_session)
{

    if(!*args)
        return false;

    Player *chr = objmgr.GetPlayer(args, false);
    if (chr)
    {
        // send message to user
        char buf[256];
        char buf0[256];
        if(chr->IsBeingTeleported()==true) {
            sprintf((char*)buf,"%s is already being teleported.", chr->GetName());
            SystemMessage(m_session, buf);
            return true;
        }
        sprintf((char*)buf,"You are summoning %s.", chr->GetName());
        SystemMessage(m_session, buf);

        if(!m_session->GetPlayer()->m_isGmInvisible)
        {
            // send message to player
            sprintf((char*)buf0,"You are being summoned by %s.", m_session->GetPlayer()->GetName());
            SystemMessageToPlr(chr, buf0);
        }

        Player * plr = m_session->GetPlayer();
        chr->SafeTeleport(plr->GetMapId(), plr->GetInstanceID(), plr->GetPosition());
    }
    else
    {
        char buf[256];
        sprintf((char*)buf,"Player (%s) does not exist or is not logged in.", args);
        SystemMessage(m_session, buf);
    }
    return true;
}


bool ChatHandler::HandleAppearCommand(const char* args, WorldSession *m_session)
{
    if(!*args)
        return false;

    Player *chr = objmgr.GetPlayer(args, false);
    if (chr)
    {
        char buf[256];
        if(chr->IsBeingTeleported()==true) {
            sprintf((char*)buf,"%s is already being teleported.", chr->GetName());
            SystemMessage(m_session, buf);
            return true;
        }
        sprintf((char*)buf,"Appearing at %s's location.", chr->GetName());  // -- europa
        SystemMessage(m_session, buf);

        if(!m_session->GetPlayer()->m_isGmInvisible)
        {
            char buf0[256];
            sprintf((char*)buf0,"%s is appearing to your location.", m_session->GetPlayer()->GetName());
            SystemMessageToPlr(chr, buf0);
        }

        m_session->GetPlayer()->SafeTeleport(chr->GetMapId(), chr->GetInstanceID(), chr->GetPosition());
    }
    else
    {
        char buf[256];
        sprintf((char*)buf,"Player (%s) does not exist or is not logged in.", args);
        SystemMessage(m_session, buf);
    }

    return true;
}

bool ChatHandler::HandleTaxiCheatCommand(const char* args, WorldSession *m_session)
{
    if (!*args)
        return false;

    int flag = atoi((char*)args);

    Player *chr = getSelectedChar(m_session);
    if (chr == NULL) return true;
    
    char buf[256];

    // send message to user
    if (flag != 0)
    {
        sprintf((char*)buf,"%s has all taxi nodes now.", chr->GetName());
    }
    else
    {
        sprintf((char*)buf,"%s has no more taxi nodes now.", chr->GetName());
    }
    GreenSystemMessage(m_session, buf);
    
    // send message to player
    if (flag != 0)
    {
        sprintf((char*)buf,"%s has given you all taxi nodes.",
            m_session->GetPlayer()->GetName());
    }
    else
    {
        sprintf((char*)buf,"%s has deleted all your taxi nodes.",
            m_session->GetPlayer()->GetName());
    }
    SystemMessage(m_session, buf);

    for (uint8 i=0; i<8; i++)
    {
        if (flag != 0)
        {
            m_session->GetPlayer()->SetTaximask(i, 0xFFFFFFFF);
        }
        else
        {
            m_session->GetPlayer()->SetTaximask(i, 0);
        }
    }

    return true;
}

bool ChatHandler::HandleModifySpeedCommand(const char* args, WorldSession *m_session)
{
    WorldPacket data;

    if (!*args)
        return false;

    float Speed = (float)atof((char*)args);

    if (Speed > 255 || Speed < 1)
    {
        RedSystemMessage(m_session, "Incorrect value. Range is 1..255");
        return true;
    }

    Player *chr = getSelectedChar(m_session);
    if (chr == NULL) return true;
    
    char buf[256];

    // send message to user
    BlueSystemMessage(m_session, "You set the speed to %2.2f of %s.", Speed, chr->GetName());

    // send message to player
    sprintf((char*)buf,"%s set your speed to %2.2f.", m_session->GetPlayer()->GetName(), Speed);
   SystemMessage(m_session, buf);

    chr->SetPlayerSpeed(RUN, Speed, 6, true);
    chr->SetPlayerSpeed(SWIM, Speed, 7, true);
    chr->SetPlayerSpeed(RUNBACK, Speed, 8, true);
    chr->SetPlayerSpeed(FLY, Speed, 0, true);

    return true;
}

bool ChatHandler::HandleLearnSkillCommand(const char *args, WorldSession *m_session)
{
    uint32 skill, min, max;
    min = max = 1;
    char *pSkill = strtok((char*)args, " ");
    if(!pSkill)
        return false;
    else
        skill = atol(pSkill);

    BlueSystemMessage(m_session, "Adding skill line %d", skill);

    char *pMin = strtok(NULL, " ");
    if(pMin)
    {
        min = atol(pMin);
        char *pMax = strtok(NULL, "\n");
        if(pMax)
            max = atol(pMax);
    } else {
        return false;
    }

    Player *plr = getSelectedChar(m_session, true);
    if(!plr) return false;
    if(plr->GetTypeId() != TYPEID_PLAYER) return false;
    sGMLog.writefromsession(m_session, "used add skill of %u %u %u on %s", skill, min, max, plr->GetName());

    plr->AddSkillLine(skill, min, max);   

    return true;
}

bool ChatHandler::HandleModifySkillCommand(const char *args, WorldSession *m_session)
{
    uint32 skill, min, max;
    min = max = 1;
    char *pSkill = strtok((char*)args, " ");
    if(!pSkill)
        return false;
    else
        skill = atol(pSkill);
    
    char *pMin = strtok(NULL, " ");
    uint32 cnt = 0;
    if(!pMin)
        cnt = 1;
    else
        cnt = atol(pMin);

    skill = atol(pSkill);
    
    BlueSystemMessage(m_session, "Modifying skill line %d. Advancing %d times.", skill, cnt);

    Player *plr = getSelectedChar(m_session, true);
    if(!plr) plr = m_session->GetPlayer();
    if(!plr) return false;
    sGMLog.writefromsession(m_session, "used modify skill of %u %u on %s", skill, cnt,plr->GetName());

    if(!plr->HasSkillLine(skill))
    {
        SystemMessage(m_session, "Does not have skill line, adding.");
        plr->AddSkillLine(skill, 1, 300);   
    } else {
        if(cnt + plr->GetSkillAmt(skill) > plr->GetSkillMax(skill))
            cnt = plr->GetSkillMax(skill) - plr->GetSkillAmt(skill);
        for(uint32 l=0;l<cnt;l++)
        {
            plr->AdvanceSkillLine(skill);
        }
    }       

    return true;
}

bool ChatHandler::HandleRemoveSkillCommand(const char *args, WorldSession *m_session)
{
    uint32 skill = 0;
    char *pSkill = strtok((char*)args, " ");
    if(!pSkill)
        return false;
    else 
        skill = atol(pSkill);
    BlueSystemMessage(m_session, "Removing skill line %d", skill);

    Player *plr = getSelectedChar(m_session, true);
    if(!plr) return true;
    sGMLog.writefromsession(m_session, "used remove skill of %u on %s", skill, plr->GetName());
    plr->RemoveSkillLine(skill);
    SystemMessageToPlr(plr, "%s removed skill line %d from you. ", m_session->GetPlayer()->GetName(), skill);
    return true;
}


bool ChatHandler::HandleEmoteCommand(const char* args, WorldSession *m_session)
{
    uint32 emote = atoi((char*)args);
    Unit* target = this->getSelectedCreature(m_session);
    if(!target) return false;
    if(target) target->SetUInt32Value(UNIT_NPC_EMOTESTATE,emote);

    return true;
}

bool ChatHandler::HandleModifyGoldCommand(const char* args, WorldSession *m_session)
{
    WorldPacket data;

    if (!*args)
        return false;

    int32 gold = atoi((char*)args);

    Player *chr = getSelectedChar(m_session, true);
    if(!chr) return true;
    
    sGMLog.writefromsession(m_session, "used modify gold on %s, gold: %u", chr->GetName(), gold);
    BlueSystemMessage(m_session, "Adding %d gold to %s's backpack...", gold, chr->GetName());

    uint32 currentgold = chr->GetUInt32Value(PLAYER_FIELD_COINAGE);
    int32 newgold = currentgold + gold;

    if(newgold < 0)
    {
        GreenSystemMessageToPlr(chr, "%s took the all gold from your backpack.", m_session->GetPlayer()->GetName());
        newgold = 0;
    } else {
        if(newgold > currentgold)
            GreenSystemMessageToPlr(chr, "%s added %d gold from your backpack.", m_session->GetPlayer()->GetName(), myabs(gold));
        else
            GreenSystemMessageToPlr(chr, "%s took %d gold to your backpack.", m_session->GetPlayer()->GetName(), myabs(gold));
    }

    chr->SetUInt32Value(PLAYER_FIELD_COINAGE, newgold);
    
    return true;
}

bool ChatHandler::HandleGenerateNameCommand(const char *args, WorldSession *m_session)
{
    SystemMessage(m_session, "Generating 7 names:");
    for(int i=0;i<7;i++)
        SystemMessage(m_session, "%s", sWorld.GenerateName(0).c_str());
    GreenSystemMessage(m_session, "Done.");
    return true;
}

bool ChatHandler::HandleTriggerCommand(const char* args, WorldSession* m_session)
{
    int32 instance_id;
    uint32 trigger_id;
    int valcount = sscanf(args, "%u %d", &trigger_id, &instance_id);
    if(!valcount)
        return false;
    if(valcount == 1)
        instance_id = 0;

    AreaTrigger *pTrigger = sWorld.GetAreaTrigger(trigger_id);
    if(trigger_id == 0 || pTrigger == NULL)
    {
        RedSystemMessage(m_session, "Could not find trigger %s", (args == NULL ? "NULL" : args));
        return true;
    }

    m_session->GetPlayer()->SafeTeleport(pTrigger->Mapid, instance_id, LocationVector(pTrigger->x, pTrigger->y,
            pTrigger->z, pTrigger->o));

    BlueSystemMessage(m_session, "Teleported to trigger %u on [%u][%.2f][%.2f][%.2f]", pTrigger->AreaTriggerID,
        pTrigger->Mapid, pTrigger->x, pTrigger->y, pTrigger->z);
    return true;
}

bool ChatHandler::HandleUnlearnCommand(const char* args, WorldSession * m_session)
{
    Player * plr = getSelectedChar(m_session, true);
    if(plr == 0)
        return true;

    uint32 SpellId = atol(args);
    if(SpellId == 0)
    {
        RedSystemMessage(m_session, "You must specify a spell id.");
        return true;
    }

    if(plr->HasSpell(SpellId))
    {
        GreenSystemMessageToPlr(plr, "Removed spell %u.", SpellId);
        plr->removeSpell(SpellId, false, false, 0);
    }
    else
    {
        RedSystemMessage(m_session, "That player does not have spell %u learnt.", SpellId);
    }

    return true;
}