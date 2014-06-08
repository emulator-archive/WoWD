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

uint32 QuestMgr::CalcQuestStatus(Object* quest_giver, Player* plr, QuestRelation* qst)
{
    uint32 status = CalcQuestStatus(quest_giver, plr, qst->qst, qst->type);
    if(status == QMGR_QUEST_FINISHED)
    {
        if(quest_giver->GetTypeId() == TYPEID_UNIT)
        {
            // Bleh, POI is the wrong icon.
            // Turns out, dynamic_flags should be 0x02 :P
            uint32 flags = quest_giver->GetUInt32Value(UNIT_DYNAMIC_FLAGS);
            flags |= 0x02;    // MINIMAP_TRACK

            quest_giver->BuildFieldUpdatePacket(plr, UNIT_DYNAMIC_FLAGS, flags);
        }
    }
    return status;
}

uint32 QuestMgr::PlayerMeetsReqs(Player* plr, Quest* qst)
{
    std::list<uint32>::iterator itr;
    uint32 status;

    if (!sQuestMgr.IsQuestRepeatable(qst))
        status = QMGR_QUEST_AVAILABLE;
    else
        status = QMGR_QUEST_REPEATABLE;

    if (plr->getLevel() < qst->min_level)
        status = QMGR_QUEST_AVAILABLELOW_LEVEL;

    if(qst->required_class)
        if(!(qst->required_class & plr->getClassMask()))
            status = QMGR_QUEST_NOT_AVAILABLE;

    /*if(qst->required_races)
        if(!(qst->required_races & plr->getRaceMask()))
            status = QMGR_QUEST_NOT_AVAILABLE;*/
    // FIXME: this seems to be bugged..

    // TODO: Check reputation

    for(uint32 i = 0; i < 4; ++i)
    {
        if (qst->required_quests[i] > 0 && !plr->HasFinishedQuest(qst->required_quests[i]))
        {
            status = QMGR_QUEST_NOT_AVAILABLE;
            break;
        }
    }

    if (plr->HasFinishedQuest(qst->id))
        status = QMGR_QUEST_NOT_AVAILABLE;

    return status;
}

uint32 QuestMgr::CalcQuestStatus(Object* quest_giver, Player* plr, Quest* qst, uint8 type)
{
    QuestLogEntry* qle;

    qle = plr->GetQuestLogForEntry(qst->id);

    if (!qle)
    {
        if (type & QUESTGIVER_QUEST_START)
        {
            return PlayerMeetsReqs(plr, qst);
        }
    }
    else
    {        
        if (!qle->CanBeFinished())
        {
            return QMGR_QUEST_NOT_FINISHED;
        }
        else
        {
            if (type & QUESTGIVER_QUEST_END) 
            {
                return QMGR_QUEST_FINISHED;                    
            }
            else
            {
                return QMGR_QUEST_NOT_FINISHED;
            }
        }
    }

    return QMGR_QUEST_NOT_AVAILABLE;
}

uint32 QuestMgr::CalcStatus(Object* quest_giver, Player* plr)
{
    uint32 status = QMGR_QUEST_NOT_AVAILABLE;
    std::list<QuestRelation *>::const_iterator itr;
    std::list<QuestRelation *>::const_iterator q_begin;
    std::list<QuestRelation *>::const_iterator q_end;
    bool bValid = false;

    if(quest_giver->GetTypeId() == TYPEID_GAMEOBJECT)
    {
        if(((GameObject*)quest_giver)->m_quests->size() > 0)
        {
            q_begin = ((GameObject*)quest_giver)->QuestsBegin();
            q_end   = ((GameObject*)quest_giver)->QuestsEnd();
            bValid = ((GameObject*)quest_giver)->HasQuests();
        }
    } 
    else if(quest_giver->GetTypeId() == TYPEID_UNIT)
    {
        bValid = ((Creature*)quest_giver)->HasQuests();
        if(bValid)
        {
            q_begin = ((Creature*)quest_giver)->QuestsBegin();
            q_end   = ((Creature*)quest_giver)->QuestsEnd();
        }
    }

    if(!bValid)
    {
        sLog.outDebug("QUESTS: Warning, invalid NPC "I64FMT" specified for CalcStatus. TypeId: %d.", quest_giver->GetGUID(), quest_giver->GetTypeId());
        return status;
    }

    for(itr = q_begin; itr != q_end; ++itr)
    {
        uint32 tmp_status = CalcQuestStatus(quest_giver, plr, *itr);    // save a call
        if (tmp_status > status)
            status = tmp_status;
    }

    return status;
}

uint32 QuestMgr::ActiveQuestsCount(Object* quest_giver, Player* plr)
{
    std::list<QuestRelation *>::const_iterator itr;
    map<uint32, uint8> tmp_map;
    uint32 questCount = 0;

    std::list<QuestRelation *>::const_iterator q_begin;
    std::list<QuestRelation *>::const_iterator q_end;
    bool bValid = false;

    if(quest_giver->GetTypeId() == TYPEID_GAMEOBJECT)
    {
        if(((GameObject*)quest_giver)->m_quests->size() > 0)
        {
            q_begin = ((GameObject*)quest_giver)->QuestsBegin();
            q_end   = ((GameObject*)quest_giver)->QuestsEnd();
            bValid = ((GameObject*)quest_giver)->HasQuests();
        }
    } 
    else if(quest_giver->GetTypeId() == TYPEID_UNIT)
    {
        bValid = ((Creature*)quest_giver)->HasQuests();
        if(bValid)
        {
            q_begin = ((Creature*)quest_giver)->QuestsBegin();
            q_end   = ((Creature*)quest_giver)->QuestsEnd();
        }
    }

    if(!bValid)
    {
        sLog.outDebug("QUESTS: Warning, invalid NPC "I64FMT" specified for ActiveQuestsCount. TypeId: %d.", quest_giver->GetGUID(), quest_giver->GetTypeId());
        return 0;
    }

    for(itr = q_begin; itr != q_end; ++itr)
    {
        if (CalcQuestStatus(quest_giver, plr, *itr) >= QMGR_QUEST_NOT_FINISHED)
        {
            if (tmp_map.find((*itr)->qst->id) == tmp_map.end())
            {
                tmp_map.insert(std::map<uint32,uint8>::value_type((*itr)->qst->id, 1));
                questCount++;
            }
        }
    }

    return questCount;
}

void QuestMgr::BuildOfferReward(WorldPacket *data, Quest* qst, Object* qst_giver, uint32 menutype)
{
    ItemPrototype * it;
    data->SetOpcode(SMSG_QUESTGIVER_OFFER_REWARD);
    *data << qst_giver->GetGUID();
    *data << qst->id;
    *data << qst->title;
    *data << qst->completiontext;
    
    //uint32 a = 0, b = 0, c = 1, d = 0, e = 1;

    *data << (qst->next_quest_id ? uint32(1) : uint32(0));      // next quest shit
    *data << uint32(0);                                         // maybe required money
    *data << uint32(1);                                         // emotes count
    *data << uint32(0);                                         // emote delay
    *data << uint32(1);                                         // emote type

    *data << qst->count_reward_choiceitem;
    for(uint32 i = 0; i < 6; ++i)
    {
        if(qst->reward_choiceitem[i])
        {
            *data << qst->reward_choiceitem[i];
            *data << qst->reward_choiceitemcount[i];
            it = objmgr.GetItemPrototype(qst->reward_choiceitem[i]);
            *data << (it ? it->DisplayInfoID : uint32(0));
        }
    }

    *data << qst->count_reward_item;
    for(uint32 i = 0; i < 6; ++i)
    {
        if(qst->reward_item[i])
        {
            *data << qst->reward_item[i];
            *data << qst->reward_itemcount[i];
            it = objmgr.GetItemPrototype(qst->reward_item[i]);
            *data << (it ? it->DisplayInfoID : uint32(0));
        }
    }

    *data << qst->reward_money;
    *data << qst->reward_spell;
    *data << uint32(8);
    *data << uint32(0);
    *data << uint32(0);
}

void QuestMgr::BuildQuestDetails(WorldPacket *data, Quest* qst, Object* qst_giver, uint32 menutype)
{
    std::map<uint32, uint8>::const_iterator itr;

    data->SetOpcode( SMSG_QUESTGIVER_QUEST_DETAILS );

    *data <<  qst_giver->GetGUID();
    *data <<  qst->id;
    *data <<  qst->title;
    *data <<  qst->details;
    *data <<  qst->objectives;
    *data <<  uint32(1);
    *data << uint32(0);         // "Suggested players"

    *data << qst->count_reward_choiceitem;
    ItemPrototype *ip;
    uint32 i;

    for(i = 0; i < 6; ++i)
    {
        ip = objmgr.GetItemPrototype(qst->reward_choiceitem[i]);
        if(!qst->reward_choiceitem[i]) continue;

        *data << qst->reward_choiceitem[i];
        *data << qst->reward_choiceitemcount[i];
        if(ip)
            *data << ip->DisplayInfoID;
        else
            *data << uint32(0);
    }

    *data << qst->count_reward_item;

    for(i = 0; i < 4; ++i)
    {
        ip = objmgr.GetItemPrototype(qst->reward_item[i]);
        if(!qst->reward_item[i]) continue;

        *data << qst->reward_item[i];
        *data << qst->reward_itemcount[i];
        if(ip)
            *data << ip->DisplayInfoID;
        else
            *data << uint32(0);
    }

    *data << qst->reward_money;
    *data << qst->reward_spell;

    *data << uint32(0);
    *data << uint32(0);
    *data << uint32(4);
    *data << uint32(1);
    *data << uint32(0);
    *data << uint32(1);
    *data << uint32(0);
    *data << uint32(0);
    *data << uint32(0);
    *data << uint32(0);
    *data << uint32(0);
}

void QuestMgr::BuildRequestItems(WorldPacket *data, Quest* qst, Object* qst_giver, uint32 menutype)
{
    ItemPrototype * it;
    data->SetOpcode( SMSG_QUESTGIVER_REQUEST_ITEMS );

    *data << qst_giver->GetGUID();
    *data << qst->id;
    *data << qst->title;

    /*if(qst_giver->GetTypeId() == TYPEID_GAMEOBJECT)
        type = ((GameObject*)qst_giver)->GetQuestRelation(qst->id);
    else if(qst_giver->GetTypeId() == TYPEID_UNIT)
        type = ((Creature*)qst_giver)->GetQuestRelation(qst->id);*/
    
    *data << (qst->incompletetext.length() ? qst->incompletetext : qst->details);
    
    *data << uint32(0);
    *data << uint32(1);                 // Emote count

    *data << uint32(0);                 // Emote delay
    *data << uint32(1);                 // Emote type
    *data << qst->required_money;       // Required Money

    // item count
    *data << qst->count_required_item;
    
    // (loop for each item)
    for(uint32 i = 0; i < 4; ++i)
    {
        if(qst->required_item[i] != 0)
        {
            *data << qst->required_item[i];
            *data << qst->required_itemcount[i];
            it = objmgr.GetItemPrototype(qst->required_item[i]);
            *data << (it ? it->DisplayInfoID : uint32(0));
        }
    }

    // wtf is this?
    *data << uint32(2);
    *data << uint32(8);
    *data << uint32(10);
}

void QuestMgr::BuildQuestComplete(WorldPacket *data, Quest* qst)
{
    uint32 i;

    data->Initialize( SMSG_QUESTGIVER_QUEST_COMPLETE );

    *data <<  qst->id;
    *data <<  uint32(3);
    if(qst->reward_xp > 0)
        *data <<  uint32(qst->reward_xp);
    else
        *data <<  uint32(GenerateQuestXP(NULL,qst)); //xp
    *data <<  uint32(qst->reward_money);
    *data <<  uint32(qst->count_reward_item); //Reward item count

    ItemPrototype *ip;
    for(i = 0; i < 4; ++i)
    {
        ip = objmgr.GetItemPrototype(qst->reward_item[i]);
        *data << qst->reward_item[i];
        *data << qst->reward_itemcount[i];
        *data << ((ip != NULL) ? ip->DisplayInfoID : uint32(0));
    }
}

void QuestMgr::BuildQuestList(WorldPacket *data, Object* qst_giver, Player *plr)
{
    list<QuestRelation *>::iterator it;
    list<QuestRelation *>::iterator st;
    list<QuestRelation *>::iterator ed;
    map<uint32, uint8> tmp_map;

    data->Initialize( SMSG_QUESTGIVER_QUEST_LIST );

    *data << qst_giver->GetGUID();
    *data << "How can I help you?"; //Hello line 
    *data << uint32(1);//Emote Delay
    *data << uint32(1);//Emote

    bool bValid = false;
    if(qst_giver->GetTypeId() == TYPEID_GAMEOBJECT)
    {
        bValid = ((GameObject*)qst_giver)->HasQuests();
        if(bValid)
        {
            st = ((GameObject*)qst_giver)->QuestsBegin();
            ed = ((GameObject*)qst_giver)->QuestsEnd();
        }
    } 
    else if(qst_giver->GetTypeId() == TYPEID_UNIT)
    {
        bValid = ((Creature*)qst_giver)->HasQuests();
        if(bValid)
        {
            st = ((Creature*)qst_giver)->QuestsBegin();
            ed = ((Creature*)qst_giver)->QuestsEnd();
        }
    }

    if(!bValid)
    {
        *data << uint8(0);
        return;
    }
    
    *data << uint8(sQuestMgr.ActiveQuestsCount(qst_giver, plr));

    for (it = st; it != ed; ++it)
    {
        if (sQuestMgr.CalcQuestStatus(qst_giver, plr, *it) >= QMGR_QUEST_NOT_FINISHED)
        {
            if (tmp_map.find((*it)->qst->id) == tmp_map.end())
            {
                tmp_map.insert(std::map<uint32,uint8>::value_type((*it)->qst->id, 1));

                *data << (*it)->qst->id;
                *data << sQuestMgr.CalcQuestStatus(qst_giver, plr, *it);
                *data << uint32(0);
                *data << (*it)->qst->title;
            }
        }
    }
}

void QuestMgr::BuildQuestUpdateAddItem(WorldPacket* data, uint32 itemid, uint32 count)
{
    data->Initialize(SMSG_QUESTUPDATE_ADD_ITEM);
    *data << itemid << count;
}

void QuestMgr::SendQuestUpdateAddKill(Player* plr, uint32 questid, uint32 entry, uint32 count, uint32 tcount, uint64 guid)
{
    WorldPacket data(32);
    data.SetOpcode(SMSG_QUESTUPDATE_ADD_KILL);
    data << questid << entry << count << tcount << guid;
    plr->GetSession()->SendPacket(&data);
}

void QuestMgr::BuildQuestUpdateComplete(WorldPacket* data, Quest* qst)
{
    data->Initialize(SMSG_QUESTUPDATE_COMPLETE);

    *data << qst->id;
}

bool QuestMgr::OnGameObjectActivate(Player *plr, GameObject *go)
{
    uint32 i, j;
    QuestLogEntry *qle;
    uint32 entry = go->GetEntry();

    for(i = 0; i < 25; ++i)
    {
        if(qle = plr->GetQuestLogInSlot(i))
        {
            // dont waste time on quests without mobs
            if(qle->GetQuest()->count_required_mob == 0)
                continue;

            for(j = 0; j < 4; ++j)
            {
                if(qle->GetQuest()->required_mob[j] == entry &&
                    qle->GetQuest()->required_mobtype[j] == QUEST_MOB_TYPE_GAMEOBJECT &&
                    qle->m_mobcount[j] < qle->GetQuest()->required_mobcount[j])
                {
                    // add another kill.
                    // (auto-dirtys it)
                    qle->SetMobCount(j, qle->m_mobcount[j] + 1);
                    qle->SendUpdateAddKill(j);

                    if(qle->CanBeFinished())
                        qle->SendQuestComplete();

                    qle->UpdatePlayerFields();
                    return true;
                }
            }
        }
    }
    return false;
}

void QuestMgr::OnPlayerKill(Player* plr, Creature* victim)
{
    uint32 i, j;
    uint32 entry = victim->GetEntry();
    QuestLogEntry *qle;
    for(i = 0; i < 25; ++i)
    {
        if(qle = plr->GetQuestLogInSlot(i))
        {
            // dont waste time on quests without mobs
            if(qle->GetQuest()->count_required_mob == 0)
                continue;

            for(j = 0; j < 4; ++j)
            {
                if(qle->GetQuest()->required_mob[j] == entry &&
                    qle->GetQuest()->required_mobtype[j] == QUEST_MOB_TYPE_CREATURE &&
                    qle->m_mobcount[j] < qle->GetQuest()->required_mobcount[j])
                {
                    // add another kill.
                    // (auto-dirtys it)
                    qle->SetMobCount(j, qle->m_mobcount[j] + 1);
                    qle->SendUpdateAddKill(j);
                    qle->UpdatePlayerFields();
                    break;
                }
            }
        }
    }
    
    // Shared kills
    Player *gplr = NULL;

    if(plr->InGroup())
    {
        if(Group* pGroup = plr->GetGroup())
        {
            if(pGroup->GetGroupType() != GROUP_TYPE_PARTY) return;  // Raid's don't get shared kills.

            GroupMembersSet::iterator gitr;
            for(uint32 k = 0; k < pGroup->GetSubGroupCount(); k++)
            {
                for(gitr = pGroup->GetSubGroup(k)->GetGroupMembersBegin(); gitr != pGroup->GetSubGroup(k)->GetGroupMembersEnd(); ++gitr)
                {
                    gplr = (*gitr);
                    if(gplr && gplr != plr) // dont double kills
                    {
                        for(i = 0; i < 20; ++i)
                        {
                            if(qle = gplr->GetQuestLogInSlot(i))
                            {
                                // dont waste time on quests without mobs
                                if(qle->GetQuest()->count_required_mob == 0)
                                    continue;

                                for(j = 0; j < 4; ++j)
                                {
                                    if(qle->GetQuest()->required_mob[j] == entry &&
                                        qle->GetQuest()->required_mobtype[j] == QUEST_MOB_TYPE_CREATURE &&
                                        qle->m_mobcount[j] < qle->GetQuest()->required_mobcount[j])
                                    {
                                        // add another kill.
                                        // (auto-dirtys it)
                                        qle->SetMobCount(j, qle->m_mobcount[j] + 1);
                                        qle->SendUpdateAddKill(j);
                                        qle->UpdatePlayerFields();

                                        // lua stuff
                                        //QUESTLUA_SendEvent(qst, victim, plr, ON_QUEST_KILLMOB, qle->m_mobcount[j]);
                                        if(qle->CanBeFinished())
                                            qle->SendQuestComplete();
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void QuestMgr::OnPlayerItemPickup(Player* plr, Item* item)
{
    uint32 i, j;
    uint32 pcount;
    uint32 entry = item->GetEntry();
    QuestLogEntry *qle;
    for(i = 0; i < 25; ++i)
    {
        if(qle = plr->GetQuestLogInSlot(i))
        {
            for(j = 0; j < qle->GetQuest()->count_required_item; ++j)
            {
                if(qle->GetQuest()->required_item[j] == entry)
                {
                    pcount = plr->GetItemInterface()->GetItemCount(entry, true);
                    if(pcount < qle->GetQuest()->required_itemcount[j])
                    {
                        WorldPacket data(8);
                        data.SetOpcode(SMSG_QUESTUPDATE_ADD_ITEM);
                        data << qle->GetQuest()->required_item[j] << uint32(1);
                        plr->GetSession()->SendPacket(&data);
                        if(qle->CanBeFinished())
                        {
                            plr->UpdateNearbyGameObjects();
                            qle->SendQuestComplete();
                        }
                        break;
                    }
                }
            }
        }
    }
}

void QuestMgr::OnPlayerExploreArea(Player* plr, uint32 AreaID)
{
    uint32 i, j;
    QuestLogEntry *qle;
    for(i = 0; i < 25; ++i)
    {
        if(qle = plr->GetQuestLogInSlot(i))
        {
            // dont waste time on quests without mobs
            if(qle->GetQuest()->count_requiredtriggers == 0)
                continue;

            for(j = 0; j < 4; ++j)
            {
                if(qle->GetQuest()->required_triggers[j] == AreaID &&
                    !qle->m_explored_areas[j])
                {
                    qle->SetTrigger(j);
                    qle->UpdatePlayerFields();
                    if(qle->CanBeFinished())
                    {
                        plr->UpdateNearbyGameObjects();
                        qle->SendQuestComplete();
                    }
                    break;
                }
            }
        }
    }
}

void QuestMgr::OnQuestFinished(Player* plr, Quest* qst, Object *qst_giver, uint32 reward_slot)
{
    QuestLogEntry *qle = plr->GetQuestLogForEntry(qst->id);
    if(!qle)
        return;

    WorldPacket data(500);
    BuildQuestComplete(&data, qst);
    plr->GetSession()->SendPacket(&data);

    qle->Finish();

    if(IsQuestRepeatable(qst))
        return;

    if(qst_giver->GetTypeId() == TYPEID_UNIT)
    {
        if(!((Creature*)qst_giver)->HasQuest(qst->id, 2))
        {
            sCheatLog.writefromsession(plr->GetSession(), "tried to finish quest from invalid npc.");
            plr->GetSession()->Disconnect();
            return;
        }
    }

    plr->ModUInt32Value(PLAYER_FIELD_COINAGE, qst->reward_money);

    //Xp reward
    uint32 xp = GenerateQuestXP(plr,qst);

    if(plr->getLevel() >= sWorld.LevelCap)
        plr->ModUInt32Value(PLAYER_FIELD_COINAGE, xp);
    else        
        plr->GiveXP(xp, 0, false);
    
    // Reputation reward
    for(int z = 0; z < 2; z++)
    {
        uint32 fact = 19;   // default to 19 if no factiondbc
        uint32 amt  = GenerateQuestXP(plr, qst) * (float)0.54;   // guess
        if(!qst->reward_repfaction[z])
        {
            if(z == 1)
                break;
            ////                        -  HUMAN   ORC     DWARF   NIGHTELF UNDEAD TAUREN  GNOME   TROLL
            //uint32 racefactions[9] = {0 ,19     ,14     ,20     ,21     ,17     ,16     ,18     ,15};

            // Let's do this properly. Determine the faction of the creature, and give reputation to his faction.
            if(qst_giver->GetTypeId() == TYPEID_UNIT)
                if(((Creature*)qst_giver)->m_factionDBC != NULL)
                    fact = ((Creature*)qst_giver)->m_factionDBC->unk1;
            if(qst_giver->GetTypeId() == TYPEID_GAMEOBJECT)
                fact = qst_giver->GetUInt32Value(GAMEOBJECT_FACTION);
        }
        else
        {
            fact = qst->reward_repfaction[z];
            if(qst->reward_repvalue[z])
                amt = qst->reward_repvalue[z];
        }
        plr->modReputation(fact,amt, true);      // burlex: we DO want to update the client for this one        
    }

    // Static Item reward
    for(uint32 i = 0; i < 4; ++i)
    {
        if(qst->reward_item[i])
        {
            ItemPrototype *proto = objmgr.GetItemPrototype(qst->reward_item[i]);
            if(!proto)
            {
                sLog.outError("Invalid item prototype in quest reward! ID %d, quest %d", qst->reward_item[i], qst->id);
            }
            else
            {   
                Item *itm = objmgr.CreateItem(qst->reward_item[i], plr);
                itm->SetUInt32Value(ITEM_FIELD_STACK_COUNT, uint32(qst->reward_itemcount[i]));
                plr->GetItemInterface()->AddItemToFreeSlot(itm); //possible memleak and no safe check.
            }
        }
    }

    // Choice Rewards
    if(qst->reward_choiceitem[reward_slot])
    {
        ItemPrototype *proto = objmgr.GetItemPrototype(qst->reward_choiceitem[reward_slot]);
        if(!proto)
        {
            sLog.outError("Invalid item prototype in quest reward! ID %d, quest %d", qst->reward_choiceitem[reward_slot], qst->id);
        }
        else
        {
            Item *itm = objmgr.CreateItem(qst->reward_choiceitem[reward_slot], plr);
            itm->SetUInt32Value(ITEM_FIELD_STACK_COUNT, qst->reward_choiceitemcount[reward_slot]);
            plr->GetItemInterface()->AddItemToFreeSlot(itm); //possible mem leak and no item check
        }
    }

    // Remove items
    for(uint32 i = 0; i < 4; ++i)
    {
        if(qst->required_item[i]) plr->GetItemInterface()->RemoveItemAmt(qst->required_item[i],qst->required_itemcount[i]);
    }

    if(qst->reward_spell)
    {
        if(!plr->HasSpell(qst->reward_spell))
        {
            // "Teaching" effect
            data.Initialize(SMSG_SPELL_START);
            data << qst_giver->GetNewGUID() << qst_giver->GetNewGUID();
            data << uint32(7763);
            data << uint16(0);
            data << uint32(0);
            data << uint16(2);
            data << plr->GetGUID();
            plr->GetSession()->SendPacket( &data );

            data.Initialize( SMSG_SPELL_GO );
            data << qst_giver->GetNewGUID() << qst_giver->GetNewGUID();
            data << uint32(7763);           // spellID
            data << uint8(0) << uint8(1);   // flags
            data << uint8(1);               // amount of targets
            data << plr->GetGUID();         // target
            data << uint8(0);
            data << uint16(2);
            data << plr->GetGUID();
            plr->GetSession()->SendPacket( &data );

            // Teach the spell
            plr->addSpell(qst->reward_spell);
        }
    }

    //Add to finished quests
    plr->AddToFinishedQuests(qst->id);
}

/////////////////////////////////////
//        Quest Management         //
/////////////////////////////////////

void QuestMgr::LoadNPCQuests(Creature *qst_giver)
{
    qst_giver->SetQuestList(GetCreatureQuestList(qst_giver->getNameEntry()));
}

void QuestMgr::LoadGOQuests(GameObject *go)
{
    go->SetQuestList(GetGOQuestList(go->GetEntry()));
}

QuestRelationList* QuestMgr::GetGOQuestList(uint32 entryid)
{
    HM_NAMESPACE::hash_map<uint32, QuestRelationList* > &olist = _GetList<GameObject>();

    if (olist.find(entryid) == olist.end())
    {
        return NULL;
    }
    else
    {
        return olist.find(entryid)->second;
    }
}

QuestRelationList* QuestMgr::GetCreatureQuestList(uint32 entryid)
{
    HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* > &olist = _GetList<Creature>();

    if (olist.find(entryid) == olist.end())
    {
        return NULL;
    }
    else
    {
        return olist.find(entryid)->second;
    }
}

template <class T> void QuestMgr::_AddQuest(uint32 entryid, Quest *qst, uint8 type)
{
    HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* > &olist = _GetList<T>();
    std::list<QuestRelation *>* nlist;
    QuestRelation *ptr = NULL;

    if (olist.find(entryid) == olist.end())
    {
        nlist = new std::list<QuestRelation *>;

        olist.insert(HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* >::value_type(entryid, nlist));
    }
    else
    {
        nlist = olist.find(entryid)->second;
    }

    list<QuestRelation *>::iterator it;
    for (it = nlist->begin(); it != nlist->end(); ++it)
    {
        if ((*it)->qst == qst)
        {
            ptr = (*it);
            break;
        }
    }

    if (ptr == NULL)
    {
        ptr = new QuestRelation;
        ptr->qst = qst;
        ptr->type = type;

        nlist->push_back(ptr);
    }
    else
    {
        ptr->type |= type;
    }
}

//////////////////////////////////
//        Quest Loading         //
//////////////////////////////////
bool QuestMgr::LoadSQLQuests()
{
    // find LUA engine

    // pull quests first
    QueryResult *pResult = sDatabase.Query("SELECT * FROM quests");
    Quest *qst = NULL;

    int total;
    int pos;

    if(pResult)
    {
        total = pResult->GetRowCount();
        pos = 0;
        do 
        {
            Field *data = pResult->Fetch();
            uint32 i = 0;
            uint32 k = 0;
            qst = new Quest;
            qst->count_required_item = 0;
            qst->count_reward_choiceitem = 0;
            qst->count_reward_item = 0;
            qst->count_required_mob = 0;
            qst->count_requiredtriggers = 0;
            qst->count_requiredquests = 0;
            qst->count_receiveitems = 0;
            //memset(qst, 0, sizeof(Quest));

            qst->id = data[i].GetUInt32();                      ++i;
            qst->zone_id = data[i].GetUInt32();                 ++i;
            qst->quest_sort = data[i].GetUInt32();              ++i;
            qst->quest_flags = data[i].GetUInt32();             ++i;
            qst->min_level = data[i].GetUInt32();               ++i;
            qst->max_level = data[i].GetUInt32();               ++i;
            qst->type = data[i].GetUInt32();                    ++i;
            qst->required_races = data[i].GetUInt32();          ++i;
            qst->required_class = data[i].GetUInt32();          ++i;
            qst->required_tradeskill = data[i].GetUInt32();     ++i;
            qst->required_rep_faction = data[i].GetUInt32();    ++i;
            qst->required_rep_value = data[i].GetUInt32();      ++i;
            qst->time = data[i].GetUInt32();                    ++i;
            qst->special_flags = data[i].GetUInt32();           ++i;
            qst->previous_quest_id = data[i].GetUInt32();       ++i;
            qst->next_quest_id = data[i].GetUInt32();           ++i;
            qst->srcitem = data[i].GetUInt32();                 ++i;
            qst->srcitemcount = data[i].GetUInt32();            ++i;
            qst->title = data[i].GetString();                   ++i;
            qst->details = data[i].GetString();                 ++i;
            qst->objectives = data[i].GetString();              ++i;
            qst->completiontext = data[i].GetString();          ++i;
            qst->incompletetext = data[i].GetString();          ++i;
            qst->endtext = data[i].GetString();                 ++i;

            for(k = 0; k < 4; ++k)
            {
                qst->objectivetexts[k] = data[i].GetString();   ++i;
            }

            for(k = 0; k < 4; ++k)
            {
                qst->required_item[k] = data[i].GetUInt32();   ++i;
                if(qst->required_item[k]) qst->count_required_item++;
            }

            for(k = 0; k < 4; ++k)
            {
                qst->required_itemcount[k] = data[i].GetUInt32();   ++i;
            }

            for(k = 0; k < 4; ++k)
            {
                qst->required_mob[k] = data[i].GetUInt32(); ++i;
                if(qst->required_mob[k])
                {
                    qst->count_required_mob++;
                    GameObjectInfo *go_info = objmgr.GetGameObjectNameNULL(qst->required_mob[k]);
                    CreatureInfo   *c_info  = objmgr.GetCreatureName(qst->required_mob[k]);
                    if(go_info && (go_info->Type == 10 || qst->quest_flags == 10 || !c_info))
                        qst->required_mobtype[k] = QUEST_MOB_TYPE_GAMEOBJECT;
                    else
                        qst->required_mobtype[k] = QUEST_MOB_TYPE_CREATURE;
                }
                else qst->required_mobtype[k] = 0;
            }

            for(k = 0; k < 4; ++k)
            {
                qst->required_mobcount[k] = data[i].GetUInt32();   ++i;
            }

            for(k = 0; k < 6; ++k)
            {
                qst->reward_choiceitem[k] = data[i].GetUInt32();   ++i;
                if(qst->reward_choiceitem[k]) qst->count_reward_choiceitem++;
            }

            for(k = 0; k < 6; ++k)
            {
                qst->reward_choiceitemcount[k] = data[i].GetUInt32();   ++i;
            }

            for(k = 0; k < 4; ++k)
            {
                qst->reward_item[k] = data[i].GetUInt32();   ++i;
                if(qst->reward_item[k]) qst->count_reward_item++;
            }

            for(k = 0; k < 4; ++k)
            {
                qst->reward_itemcount[k] = data[i].GetUInt32();   ++i;
            }

            qst->reward_repfaction[0] = data[i].GetUInt32();   ++i;
            qst->reward_repfaction[1] = data[i].GetUInt32();   ++i;

            qst->reward_repvalue[0] = data[i].GetUInt32();   ++i;
            qst->reward_repvalue[1] = data[i].GetUInt32();   ++i;

            qst->reward_money = data[i].GetUInt32();   ++i;
            qst->reward_xp = data[i].GetUInt32();   ++i;
            qst->reward_spell = data[i].GetUInt32();   ++i;
            qst->effect_on_player = data[i].GetUInt32();   ++i;

            qst->point_mapid = data[i].GetUInt32(); ++i;
            qst->point_x = data[i].GetUInt32(); ++i;
            qst->point_y = data[i].GetUInt32(); ++i;
            qst->point_opt = data[i].GetUInt32(); ++i;

            qst->required_money = data[i].GetUInt32(); ++i;

            for(k = 0; k < 4; ++k)
            {
                qst->required_triggers[k] = data[i].GetUInt32();   ++i;
                if(qst->required_triggers[k]) qst->count_requiredtriggers++;
            }

            for(k = 0; k < 4; ++k)
            {
                qst->required_quests[k] = data[i].GetUInt32();   ++i;
                if(qst->required_triggers[k]) qst->count_requiredquests++;
            }

            for(k = 0; k < 4; ++k)
            {
                qst->receive_items[k] = data[i].GetUInt32();   ++i;
                if(qst->receive_items[k]) qst->count_receiveitems++;
            }

            for(k = 0; k < 4; ++k)
            {
                qst->receive_itemcount[k] = data[i].GetUInt32();   ++i;
            }

            qst->is_repeatable = data[i].GetUInt32(); ++i;
            qst->reward_xp_as_money = sQuestMgr.GenerateQuestXP(NULL, qst);
            ASSERT(i == pResult->GetFieldCount());
            AddQuest(qst);
            ++pos;
            if(!(pos % 20)) SetProgressBar(pos, total, "Quests");

        } while(pResult->NextRow());
        delete pResult;
        ClearProgressBar();
    }

    // load creature starters
    HM_NAMESPACE::hash_map<uint32, Quest*>::iterator itr;
    uint32 creature, quest;

    pResult = sDatabase.Query("SELECT * FROM creature_quest_starter");
    pos = 0;
    if(pResult)
    {
        total = pResult->GetRowCount();
        do 
        {
            Field *data = pResult->Fetch();
            creature = data[0].GetUInt32();
            quest = data[1].GetUInt32();

            itr = m_quests.find(quest);
            if(itr == m_quests.end())
            {
                //printf("Tried to add starter to npc %d for non-existant quest %d.\n", creature, quest);
            }
            else 
            {
                _AddQuest<Creature>(creature, itr->second, 1);  // 1 = starter
            }
            ++pos;
            if(!(pos % 20)) SetProgressBar(pos, total, "Creature Quest Binding");

        } while(pResult->NextRow());
        delete pResult;
        ClearProgressBar();
    }
    
    pResult = sDatabase.Query("SELECT * FROM creature_quest_finisher");
    pos = 0;
    if(pResult)
    {
        total = pResult->GetRowCount();
        do 
        {
            Field *data = pResult->Fetch();
            creature = data[0].GetUInt32();
            quest = data[1].GetUInt32();

            itr = m_quests.find(quest);
            if(itr == m_quests.end())
            {
                //printf("Tried to add finisher to npc %d for non-existant quest %d.\n", creature, quest);
            } 
            else 
            {
                _AddQuest<Creature>(creature, itr->second, 2);  // 1 = starter
            }
            ++pos;
            if(!(pos % 20)) SetProgressBar(pos, total, "Creature Quest Binding");
        } while(pResult->NextRow());
        delete pResult;
        ClearProgressBar();
    }

    pResult = sDatabase.Query("SELECT * FROM gameobject_quest_starter");
    pos = 0;
    if(pResult)
    {
        total = pResult->GetRowCount();
        do 
        {
            Field *data = pResult->Fetch();
            creature = data[0].GetUInt32();
            quest = data[1].GetUInt32();

            itr = m_quests.find(quest);
            if(itr == m_quests.end())
            {
                //printf("Tried to add starter to go %d for non-existant quest %d.\n", creature, quest);
            } 
            else
            {
                _AddQuest<GameObject>(creature, itr->second, 1);  // 1 = starter
            }
            ++pos;
            if(!(pos % 20)) SetProgressBar(pos, total, "GameObject Quest Binding");
        } while(pResult->NextRow());
        delete pResult;
        ClearProgressBar();    
    }

    pResult = sDatabase.Query("SELECT * FROM gameobject_quest_finisher");
    pos = 0;
    if(pResult)
    {
        total = pResult->GetRowCount();
        do 
        {
            Field *data = pResult->Fetch();
            creature = data[0].GetUInt32();
            quest = data[1].GetUInt32();

            itr = m_quests.find(quest);
            if(itr == m_quests.end())
            {
                //printf("Tried to add finisher to go %d for non-existant quest %d.\n", creature, quest);
            } 
            else 
            {
                _AddQuest<GameObject>(creature, itr->second, 2);  // 2 = finish
            }
            ++pos;
            if(!(pos % 20)) SetProgressBar(pos, total, "GameObject Quest Binding");
        } while(pResult->NextRow());
        delete pResult;
        ClearProgressBar();
    }
    return true;
}

void QuestMgr::_CleanLine(std::string *str) 
{
    _RemoveChar("\r", str);
    _RemoveChar("\n", str);

    while (str->c_str()[0] == 32) 
    {
        str->erase(0,1);
    }
}

void QuestMgr::_RemoveChar(char *c, std::string *str) 
{
    string::size_type pos = str->find(c,0);

    while (pos != string::npos)
    {
        str->erase(pos, 1);
        pos = str->find(c, 0);
    }    
}

uint32 QuestMgr::GenerateQuestXP(Player *pl, Quest *qst)
{
    // Taken from ludmilla, and modified a little..
    if(qst->is_repeatable)
        return 0;

#define XP_INC 50
#define XP_DEC 10
#define XP_INC100 15
#define XP_DEC100 5
    double xp, pxp, mxp, mmx;

    xp  = qst->max_level * XP_INC;
    pxp  = xp + (xp / 100) * XP_INC100;

    xp   = XP_DEC;

    mxp  = xp + (xp / 100) * XP_DEC100;

    mmx = (pxp - mxp);

    if(qst->quest_flags & QUEST_FLAG_SPEAKTO)
        mmx *= 0.6;
    if(qst->quest_flags & QUEST_FLAG_TIMED)
        mmx *= 1.1;
    if(qst->quest_flags & QUEST_FLAG_EXPLORATION)
        mmx *= 1.2;

    mmx *= sWorld.getRate(RATE_QUESTXP);
    return (int)mmx;
}

void QuestMgr::SendQuestInvalid(INVALID_REASON reason, Player *plyr)
{
    if(!plyr)
        return;

    plyr->GetSession()->OutPacket(SMSG_QUESTGIVER_QUEST_INVALID, 4, &reason);
    sLog.outDebug("WORLD:Sent SMSG_QUESTGIVER_QUEST_INVALID");
}

void QuestMgr::SendQuestFailed(FAILED_REASON failed, Player *plyr)
{
    if(!plyr)
        return;

    plyr->GetSession()->OutPacket(SMSG_QUESTGIVER_QUEST_FAILED, 4, &failed);
    sLog.outDebug("WORLD:Sent SMSG_QUESTGIVER_QUEST_FAILED");
}

void QuestMgr::SendQuestUpdateFailedTimer(Quest *pQuest, Player *plyr)
{
    if(!plyr)
        return;

    plyr->GetSession()->OutPacket(SMSG_QUESTUPDATE_FAILEDTIMER, 4, &pQuest->id);
    sLog.outDebug("WORLD:Sent SMSG_QUESTUPDATE_FAILEDTIMER");
}

void QuestMgr::SendQuestUpdateFailed(Quest *pQuest, Player *plyr)
{
    if(!plyr)
        return;

    plyr->GetSession()->OutPacket(SMSG_QUESTUPDATE_FAILED, 4, &pQuest->id);
    sLog.outDebug("WORLD:Sent SMSG_QUESTUPDATE_FAILED");
}

void QuestMgr::SendQuestLogFull(Player *plyr)
{
    if(!plyr)
        return;

    plyr->GetSession()->OutPacket(SMSG_QUESTLOG_FULL);
    sLog.outDebug("WORLD:Sent QUEST_LOG_FULL_MESSAGE");
}

uint32 QuestMgr::GetGameObjectLootQuest(uint32 GO_Entry)
{
    HM_NAMESPACE::hash_map<uint32, uint32>::iterator itr = m_ObjectLootQuestList.find(GO_Entry);
    if(itr == m_ObjectLootQuestList.end()) return 0;
    
    return itr->second;
}

void QuestMgr::SetGameObjectLootQuest(uint32 GO_Entry, uint32 Item_Entry)
{
    if(m_ObjectLootQuestList.find(GO_Entry) != m_ObjectLootQuestList.end())
    {
        //sLog.outError("WARNING: Gameobject %d has more than 1 quest item allocated in it's loot template!", GO_Entry);
    }

    // Find the quest that has that item
    uint32 QuestID = 0;
    uint32 i;
    for(HM_NAMESPACE::hash_map<uint32, Quest*>::iterator itr=m_quests.begin();itr!=m_quests.end();++itr)
    {
        Quest *qst = itr->second;
        for(i = 0; i < 4; ++i)
            if(qst->required_item[i] == Item_Entry)
            {
                QuestID = qst->id;
                m_ObjectLootQuestList[GO_Entry] = QuestID;
                return;
            }
    }
    //sLog.outError("WARNING: No coresponding quest was found for quest item %d", Item_Entry);
}

void QuestMgr::BuildQuestFailed(WorldPacket* data, uint32 questid)
{
    data->Initialize(SMSG_QUESTUPDATE_FAILEDTIMER);
    *data << questid;
}

bool QuestMgr::OnActivateQuestGiver(Object *qst_giver, Player *plr)
{
    if(qst_giver->GetTypeId() == TYPEID_GAMEOBJECT && !((GameObject*)qst_giver)->HasQuests())
        return false;

    uint32 questCount = sQuestMgr.ActiveQuestsCount(qst_giver, plr);
    WorldPacket data(1000);    

    if (questCount == 0) 
    {
        sLog.outDebug("WORLD: Invalid NPC for CMSG_QUESTGIVER_HELLO.");
        return false;
    }
    else if (questCount == 1)
    {
        std::list<QuestRelation *>::const_iterator itr;
        std::list<QuestRelation *>::const_iterator q_begin;
        std::list<QuestRelation *>::const_iterator q_end;

        bool bValid = false;

        if(qst_giver->GetTypeId() == TYPEID_GAMEOBJECT)
        {
            if(((GameObject*)qst_giver)->m_quests->size() > 0)
            {
                q_begin = ((GameObject*)qst_giver)->QuestsBegin();
                q_end   = ((GameObject*)qst_giver)->QuestsEnd();
                bValid = ((GameObject*)qst_giver)->HasQuests();
            }
        } 
        else if(qst_giver->GetTypeId() == TYPEID_UNIT)
        {
            bValid = ((Creature*)qst_giver)->HasQuests();
            if(bValid)
            {
                q_begin = ((Creature*)qst_giver)->QuestsBegin();
                q_end   = ((Creature*)qst_giver)->QuestsEnd();
            }
        }

        if(!bValid)
        {
            sLog.outDebug("QUESTS: Warning, invalid NPC "I64FMT" specified for OnActivateQuestGiver. TypeId: %d.", qst_giver->GetGUID(), qst_giver->GetTypeId());
            return false;
        }
        
        for(itr = q_begin; itr != q_end; ++itr) 
            if (sQuestMgr.CalcQuestStatus(qst_giver, plr, *itr) >= QMGR_QUEST_NOT_FINISHED)
                break;

        if (sQuestMgr.CalcStatus(qst_giver, plr) < QMGR_QUEST_NOT_FINISHED)
            return false; 

        ASSERT(itr != q_end);

        uint32 status = sQuestMgr.CalcStatus(qst_giver, plr);

        if ((status == QMGR_QUEST_AVAILABLE) || (status == QMGR_QUEST_REPEATABLE))
        {
            sQuestMgr.BuildQuestDetails(&data, (*itr)->qst,qst_giver, 1);        // 1 because we have 1 quest, and we want goodbye to function
            plr->GetSession()->SendPacket(&data);
            sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_QUEST_DETAILS." );
        }
        else if (status == QMGR_QUEST_FINISHED)
        {
            sQuestMgr.BuildOfferReward(&data, (*itr)->qst, qst_giver, 1);
            plr->GetSession()->SendPacket(&data);
            //ss
            sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_OFFER_REWARD." );
        }
        else if (status == QMGR_QUEST_NOT_FINISHED)
        {
            sQuestMgr.BuildRequestItems(&data, (*itr)->qst, qst_giver, 1);
            plr->GetSession()->SendPacket(&data);
            sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_REQUEST_ITEMS." );
        }
    }
    else 
    {
        sQuestMgr.BuildQuestList(&data, qst_giver ,plr);
        plr->GetSession()->SendPacket(&data);
        sLog.outDebug( "WORLD: Sent SMSG_QUESTGIVER_QUEST_LIST." );
    }
    return true;
}

QuestMgr::~QuestMgr()
{
    HM_NAMESPACE::hash_map<uint32, Quest*>::iterator itr1;
    HM_NAMESPACE::hash_map<uint32, list<QuestRelation *>* >::iterator itr2;
    list<QuestRelation*>::iterator itr3;

    // clear quests
    for(itr1 = m_quests.begin(); itr1 != m_quests.end(); ++itr1)
    {
        delete itr1->second;
    }
    m_quests.clear();

    // clear relations
    for(itr2 = m_obj_quests.begin(); itr2 != m_obj_quests.end(); ++itr2)
    {
        itr3 = itr2->second->begin();
        for(; itr3 != itr2->second->end(); ++itr3)
        {
            delete (*itr3);
        }
        itr2->second->clear();
        delete itr2->second;
    }

    for(itr2 = m_npc_quests.begin(); itr2 != m_npc_quests.end(); ++itr2)
    {
        itr3 = itr2->second->begin();
        for(; itr3 != itr2->second->end(); ++itr3)
        {
            delete (*itr3);
        }
        itr2->second->clear();
        delete itr2->second;
    }

    for(itr2 = m_itm_quests.begin(); itr2 != m_itm_quests.end(); ++itr2)
    {
        itr3 = itr2->second->begin();
        for(; itr3 != itr2->second->end(); ++itr3)
        {
            delete (*itr3);
        }
        itr2->second->clear();
        delete itr2->second;
    }

}
